#include "script.h"
#include <cstdlib>
#include <cstring>
#include "state.h"
#include "lua_inc.h"
#include "scripts_gen.h"

using namespace yy;

static lua_State* L = nullptr;
static lua_State* co = nullptr;  // running event coroutine (one at a time); anchored on L's stack
static std::string cur_event, last_error;

// Every script failure is logged, kept for script_last_error(), and shown to the player; control returns.
static void report(const std::string& what, const std::string& msg) {
    last_error = msg;
    std::fprintf(stderr, "script error in %s: %s\n", what.c_str(), msg.c_str());
    show_toast("이벤트 오류: " + what);
}

static void finish(int status) {
    if (status == LUA_YIELD) return;  // waiting for a dialog, battle, shop or fade
    if (status != LUA_OK) report(cur_event, lua_tostring(co, -1) ? lua_tostring(co, -1) : "unknown");
    co = nullptr;
    lua_settop(L, 0);  // drop the thread so it can be collected
}

bool script_start(const std::string& event, int x, int y) {
    if (co || !L) return false;  // never overlap events
    lua_getglobal(L, event.c_str());
    bool fn = lua_isfunction(L, -1);
    lua_pop(L, 1);
    if (!fn) return false;
    cur_event = event;
    co = lua_newthread(L);
    lua_getglobal(co, event.c_str());
    lua_pushinteger(co, x);
    lua_pushinteger(co, y);
    int nres = 0;
    finish(lua_resume(co, L, 2, &nres));
    return true;
}

void script_resume(int value) {
    if (!co) return;
    lua_pushinteger(co, value);
    int nres = 0;
    finish(lua_resume(co, L, 1, &nres));
}

bool script_busy() { return co != nullptr; }
std::string script_last_error() { return last_error; }
int script_flag(const std::string& name) { return flag(name); }

// Scripts resume only from callbacks: dialog/shop close, battle end, or a fade midpoint. Inside a
// midpoint another fade cannot start, so the change applies at once and the event keeps running.
static int fade_then(lua_State* s, std::function<void()> fn) {
    if (g.trans_t >= 0) { fn(); return 0; }
    transition([fn] { fn(); script_resume(0); });
    return lua_yield(s, 0);
}

static std::vector<std::string> texts(lua_State* s, int from, int to) {
    std::vector<std::string> v;
    for (int i = from; i <= to; ++i) v.push_back(luaL_checkstring(s, i));
    return v;
}

// say(speaker, text, ...)
static int l_say(lua_State* s) {
    std::string who = luaL_checkstring(s, 1);
    luaL_checkstring(s, 2);  // a dialog needs at least one page
    say(texts(s, 2, lua_gettop(s)), who, [](int) { script_resume(0); });
    return lua_yield(s, 0);
}

// ask(speaker, text, ..., {"choice", ...}) -> 1-based choice; choices show on the last page
static int l_ask(lua_State* s) {
    int n = lua_gettop(s);
    std::string who = luaL_checkstring(s, 1);
    luaL_checkstring(s, 2);
    luaL_checktype(s, n, LUA_TTABLE);
    std::vector<std::string> choices;
    for (lua_Integer i = 1; i <= luaL_len(s, n); ++i) {
        lua_geti(s, n, i);
        choices.push_back(luaL_checkstring(s, -1));
        lua_pop(s, 1);
    }
    if (choices.empty()) return luaL_error(s, "ask: no choices");
    say(texts(s, 2, n - 1), who, [](int c) { script_resume(c + 1); }, choices);
    return lua_yield(s, 0);
}

// battle(enemy_id) -> 1 win, 0 lose (end_battle resumes)
static int l_battle(lua_State* s) {
    g.battle_enemy = luaL_checkstring(s, 1);  // ponytail: not validated until M2 defines enemy ids
    start_battle();
    return lua_yield(s, 0);
}

static int l_flag(lua_State* s) { lua_pushinteger(s, flag(luaL_checkstring(s, 1))); return 1; }
static int l_set_flag(lua_State* s) { g.flags[luaL_checkstring(s, 1)] = (int)luaL_checkinteger(s, 2); return 0; }

static const char* item_arg(lua_State* s, int i) {
    const char* id = luaL_checkstring(s, i);
    if (!data::find_item(id)) luaL_error(s, "unknown item %s", id);
    return id;
}
static int l_give(lua_State* s) { g.items[item_arg(s, 1)] += (int)luaL_optinteger(s, 2, 1); return 0; }
static int l_count(lua_State* s) { lua_pushinteger(s, have(item_arg(s, 1))); return 1; }
// take(item_id, n) -> true and removes n if the bag has them, else false and nothing changes
static int l_take(lua_State* s) {
    std::string id = item_arg(s, 1);
    int n = (int)luaL_optinteger(s, 2, 1), left = have(id) - n;
    if (left < 0) { lua_pushboolean(s, 0); return 1; }
    if (left == 0) g.items.erase(id); else g.items[id] = left;
    lua_pushboolean(s, 1);
    return 1;
}
static int l_money(lua_State* s) {
    g.money = std::max(0, g.money + (int)luaL_optinteger(s, 1, 0));
    lua_pushinteger(s, g.money);
    return 1;
}

// warp(map, x, y, face="D"): fade, then move
static int l_warp(lua_State* s) {
    std::string map = luaL_checkstring(s, 1);
    int x = (int)luaL_checkinteger(s, 2), y = (int)luaL_checkinteger(s, 3);
    Dir face = dir_of(luaL_optstring(s, 4, "D")[0]);
    if (!warp_valid(map, x, y)) return luaL_error(s, "bad warp %s(%d,%d)", map.c_str(), x, y);
    return fade_then(s, [map, x, y, face] { load_map(map); g.px = x; g.py = y; g.dir = face; });
}

// rest(): next day 06:00, full recovery, searchable objects refill
static int l_rest(lua_State* s) {
    return fade_then(s, [] {
        rest(g.clock);
        g.searched.clear();
        g.hp = g.hp_max; g.ng = g.ng_max;
        g.last_phase = phase_of(g.clock.minute);
    });
}

// shop({item_id, ...}): resumes when the player leaves
static int l_shop(lua_State* s) {
    luaL_checktype(s, 1, LUA_TTABLE);
    std::vector<std::string> ids;
    for (lua_Integer i = 1; i <= luaL_len(s, 1); ++i) {
        lua_geti(s, 1, i);
        ids.push_back(item_arg(s, -1));
        lua_pop(s, 1);
    }
    open_shop(ids);
    return lua_yield(s, 0);
}

// scene("placecard"): fade to a story scene
static int l_scene(lua_State* s) {
    std::string name = luaL_checkstring(s, 1);
    if (name != "placecard") return luaL_error(s, "unknown scene %s", name.c_str());
    return fade_then(s, [] { g.scene = Scene::PlaceCard; g.card_t = 0; });
}

static int l_toast(lua_State* s) { show_toast(luaL_checkstring(s, 1)); return 0; }
static int l_clock(lua_State* s) { lua_pushstring(s, clock_str().c_str()); return 1; }
static int l_phase(lua_State* s) {
    const char* names[] = {"dawn", "day", "dusk", "night"};
    lua_pushstring(s, names[(int)phase_of(g.clock.minute)]);
    return 1;
}
static int l_tile(lua_State* s) {
    char c = tile_at((int)luaL_checkinteger(s, 1), (int)luaL_checkinteger(s, 2));
    lua_pushlstring(s, &c, 1);
    return 1;
}
static std::string obj_key(lua_State* s) {
    return g.map_id + ":" + std::to_string(luaL_checkinteger(s, 1)) + ":" + std::to_string(luaL_checkinteger(s, 2));
}
static int l_searched(lua_State* s) { lua_pushboolean(s, g.searched.count(obj_key(s)) > 0); return 1; }
static int l_mark_searched(lua_State* s) { g.searched.insert(obj_key(s)); return 0; }
// daily_roll(n, x, y) -> 0..n-1, fixed per object per day
static int l_daily_roll(lua_State* s) {
    int n = (int)luaL_checkinteger(s, 1);
    luaL_argcheck(s, n > 0, 1, "must be positive");
    lua_pushinteger(s, hash3(g.clock.day, (int)luaL_checkinteger(s, 2), (int)luaL_checkinteger(s, 3)) % n);
    return 1;
}
// face("U"): turn the player
static int l_face(lua_State* s) { g.dir = dir_of(luaL_checkstring(s, 1)[0]); return 0; }
// face_player(npc_name): turn an NPC toward the player
static int l_face_player(lua_State* s) {
    Npc* n = npc_named(luaL_checkstring(s, 1));
    if (!n) return luaL_error(s, "no npc %s here", lua_tostring(s, 1));
    int dx = g.px - n->x, dy = g.py - n->y;
    n->face = std::abs(dx) > std::abs(dy) ? (dx < 0 ? LEFT : RIGHT) : (dy < 0 ? UP : DOWN);
    return 0;
}

void script_init() {
    if (L) lua_close(L);
    co = nullptr;
    last_error.clear();
    L = luaL_newstate();
    luaL_openlibs(L);
    const luaL_Reg api[] = {
        {"say", l_say}, {"ask", l_ask}, {"battle", l_battle}, {"flag", l_flag}, {"set_flag", l_set_flag},
        {"give", l_give}, {"take", l_take}, {"count", l_count}, {"money", l_money}, {"warp", l_warp},
        {"rest", l_rest}, {"shop", l_shop}, {"scene", l_scene}, {"toast", l_toast}, {"clock", l_clock},
        {"phase", l_phase}, {"tile", l_tile}, {"searched", l_searched}, {"mark_searched", l_mark_searched},
        {"daily_roll", l_daily_roll}, {"face", l_face}, {"face_player", l_face_player}, {nullptr, nullptr}};
    for (const luaL_Reg* r = api; r->name; ++r) lua_register(L, r->name, r->func);
    for (const auto& src : kScripts) script_load(src[0], src[1]);
}

bool script_load(const std::string& name, const char* src) {
    if (!L) return false;
    std::string chunk = "@" + name;
    if (luaL_loadbuffer(L, src, std::strlen(src), chunk.c_str()) == LUA_OK && lua_pcall(L, 0, 0, 0) == LUA_OK) return true;
    report(name, lua_tostring(L, -1) ? lua_tostring(L, -1) : "unknown");
    lua_settop(L, 0);
    return false;
}

namespace yy {
void run_event(const std::string& name, int x, int y) {
    if (!script_start(name, x, y) && !script_busy()) report(name, "unknown event " + name);
}
}  // namespace yy
