// Minimal self-checks for platform-free core logic.
#undef NDEBUG  // asserts must run in Release builds too
#include <cassert>
#include <cstdio>
#include <filesystem>
#include "../src/game.h"
#include "../src/gfx.h"
#include "../src/data.h"
#include "../src/maps_api.h"
#include "../src/script.h"
#include "../src/ui.h"
#include "../src/save.h"
#include "../src/lua_inc.h"

// A Lua error raised from a C++ binding must still run that binding's destructors (no leaks, no UB).
static int dtor_runs = 0;
struct Guard { ~Guard() { ++dtor_runs; } };
static int l_throw(lua_State* s) { Guard g; return luaL_error(s, "boom"); }

int main() {
    {
        lua_State* s = luaL_newstate();
        lua_pushcfunction(s, l_throw);
        assert(lua_pcall(s, 0, 0, 0) != LUA_OK);
        lua_close(s);
        assert(dtor_runs == 1);
    }

    // Clock phases and rest rule: rest always wakes on the next day at 06:00.
    assert(phase_of(6 * 60) == Phase::Dawn);
    assert(phase_of(12 * 60) == Phase::Day);
    assert(phase_of(18 * 60) == Phase::Dusk);
    assert(phase_of(23 * 60) == Phase::Night);
    assert(phase_of(2 * 60) == Phase::Night);
    Clock c{3, 23 * 60 + 30};
    rest(c);
    assert(c.day == 4 && c.minute == 6 * 60);

    // Korean text: Hangul advance is 12px, wrap keeps lines inside the box.
    assert(gfx::text_width("가") == 12);
    assert(gfx::cp_count("음양사 abc") == 7);
    auto lines = gfx::wrap("산천의 영맥이 뒤엉켜, 백팔 가지 요괴가 밤낮을 가리지 않고 날뛰었지.", 120);
    assert(lines.size() > 1);
    for (auto& l : lines) assert(gfx::text_width(l) <= 120);

    // Every debug scene builds and renders without crashing.
    for (const char* s : {"title", "prologue", "card", "wake", "village", "night", "talk", "rest", "menu", "save", "title_load", "bag", "shop", "shop_buy",
                          "battle", "battle_list", "battle_msg", "encounter", "choice"}) {
        game_init(1);
        assert(game_debug_scene(s));
        Input none;
        game_update(none);
        game_render();
    }
    // Scripted playthrough: title -> prologue -> wake -> walk to the gate -> win battle -> quest done.
    game_init(7);
    auto frame = [](int key, bool press) {
        Input in;
        if (key >= 0) { in.held[key] = true; in.pressed[key] = press; }
        game_update(in);
        game_render();
    };
    auto tap = [&](int key) { frame(key, true); frame(-1, false); };
    auto until = [&](auto cond, auto step, int cap) { while (!cond() && cap--) step(); return cond(); };
    auto info = game_debug_info;
    const int FIELD = 3, BATTLE = 4;
    tap(K_A);
    assert(until([&] { return info().scene == FIELD && !info().busy; }, [&] { tap(K_A); }, 3000));
    assert(info().map == "tavern_room");
    auto walk = [&](int key, auto cond) { return until(cond, [&] { frame(key, false); }, 2000); };
    assert(walk(K_RIGHT, [&] { return info().x == 3 && !info().busy; }));
    assert(walk(K_DOWN, [&] { return info().map == "village" && !info().busy; }));
    assert(walk(K_DOWN, [&] { return info().y == 14 && !info().busy; }));
    assert(walk(K_RIGHT, [&] { return info().x == 12 && !info().busy; }));
    assert(walk(K_UP, [&] { return info().y == 2; }));
    assert(until([&] { return info().scene == BATTLE; }, [&] { tap(K_A); }, 3000));
    assert(until([&] { return info().scene == FIELD && info().quest == 1 && !info().busy; }, [&] { tap(K_A); }, 6000));

    // Baked game data: 108 yokai with five-phase elements, items with prices.
    assert(data::yokai_count() == 108);
    assert(data::find_yokai("yokai_076")->element == data::Element::Wood);   // 장승 목신
    assert(data::find_yokai("yokai_006")->element == data::Element::Water);  // 해태: fire ward
    assert(data::overcomes(data::Element::Metal, data::Element::Wood));
    assert(!data::overcomes(data::Element::Wood, data::Element::Metal));
    assert(data::find_item("contract_talisman")->price == 40);
    assert(data::find_item("nope") == nullptr);
    int korean = 0;
    for (int i = 0; i < data::yokai_count(); ++i) korean += data::yokai_at(i).korean;
    assert(korean == 76);  // AGENTS: 70% Korean folklore

    // Baked maps: same layout as the old in-code maps; bad warps are rejected.
    const MapDef* v = find_map("village");
    assert(v && v->w == 28 && v->h == 19 && !v->indoor);
    assert(v->tiles[1 * v->w + 11] == 'G');  // left jangseung
    assert(find_map("tavern_room")->indoor);
    assert(warp_valid("tavern_room", 3, 4));
    assert(!warp_valid("tavern_room", 0, 0));  // wall
    assert(!warp_valid("no_such_map", 1, 1));
    assert(!warp_valid("village", 99, 99));    // out of bounds

    // Lua events: a broken script reports an error and returns control; events never overlap.
    game_init(3);
    assert(script_last_error().empty());                // every baked script loads
    assert(!script_load("bad.lua", "function ("));     // a broken script is reported, not swallowed
    assert(script_last_error().find("bad.lua") != std::string::npos && yy::g.toast == "이벤트 오류: bad.lua");
    yy::run_event("no_such_event");                    // map data naming a missing event
    assert(script_last_error().find("no_such_event") != std::string::npos && yy::g.toast == "이벤트 오류: no_such_event");
    assert(!script_start("no_such_event"));
    assert(script_start("__test_error"));  // common.lua: calls an undefined function
    assert(!script_busy());
    assert(script_last_error().find("undefined_function_for_test") != std::string::npos);
    assert(yy::g.toast == "이벤트 오류: __test_error");
    assert(script_start("__test_ask"));    // asks one question, stores the answer in flag "t_ans"
    assert(script_busy());
    assert(!script_start("__test_ask"));   // second start while busy is refused
    {
        Input a; a.pressed[K_A] = true;     // finish typing, then pick the first choice
        for (int i = 0; i < 4 && script_busy(); ++i) { game_update(a); game_update(Input{}); }
    }
    assert(!script_busy());
    assert(script_flag("t_ans") == 1);

    // A prologue event that died (script error, missing function) must not strand the player there.
    game_init(1);
    yy::g.scene = yy::Scene::Prologue;
    for (int i = 0; i < 60; ++i) game_update(Input{});
    assert(yy::g.scene == yy::Scene::PlaceCard);

    // Walking off the village's north edge runs village_bump from Lua.
    game_init(1);
    game_debug_scene("village");
    yy::g.px = 13; yy::g.py = 0; yy::g.dir = yy::UP;
    {
        Input up; up.pressed[K_UP] = up.held[K_UP] = true;
        game_update(up);
    }
    assert(!yy::g.dq.empty() && yy::g.dq.front().pages[0].find("장승의 요기") == 0);

    // ListMenu: wraps, skips disabled rows, scrolls, B cancels.
    {
        yy::ListMenu m; m.items = {"a", "b", "c", "d", "e", "f", "g", "h"}; m.enabled = {1, 0, 1, 1, 1, 1, 1, 1}; m.rows = 3;
        Input down; down.pressed[K_DOWN] = true;
        Input up; up.pressed[K_UP] = true;
        Input a; a.pressed[K_A] = true;
        Input b; b.pressed[K_B] = true;
        int c = 99;
        m.update(down, c); assert(m.sel == 2);          // skips disabled "b"
        m.update(up, c); m.update(up, c); assert(m.sel == 7 && m.top == 5);  // wraps to the end and scrolls
        assert(m.update(a, c) && c == 7);
        assert(m.update(b, c) && c == -1);
    }
    {
        yy::NumberPicker p; p.max = 5;
        Input up; up.pressed[K_UP] = true;
        int c = 0;
        for (int i = 0; i < 9; ++i) p.update(up, c);
        assert(p.value == 5);                             // clamps at max
    }

    // Save/load: round trip, refuse unsafe moments, survive corrupt or old files.
    {
        namespace fs = std::filesystem;
        std::string dir = (fs::temp_directory_path() / "yyc_save_test").string();
        fs::remove_all(dir);
        fs::create_directories(dir);
        yy::save_set_dir(dir);
        game_init(5);
        assert(!yy::g.title_menu.enabled[1]);                 // no saves yet: continue disabled
        game_debug_scene("village");
        assert(yy::save_allowed());
        assert(yy::save_game(0));
        DebugInfo before = game_debug_info();
        game_init(6);
        assert(yy::g.title_menu.enabled[1]);                  // a valid save enables continue
        assert(yy::load_game(0));
        DebugInfo after = game_debug_info();
        assert(after.map == before.map && after.x == before.x && after.y == before.y && after.quest == before.quest);

        game_debug_scene("battle");
        assert(!yy::save_allowed());                          // Review Focus 5

        std::FILE* f = std::fopen((dir + "/slot1.sav").c_str(), "w");
        std::fputs("version=1\nmap=no_such_map\nx=1\ny=1\n", f); std::fclose(f);
        DebugInfo held = game_debug_info();
        assert(!yy::load_game(1) && yy::save_info(1).corrupt);  // Review Focus 1: unknown map
        assert(game_debug_info().scene == held.scene && game_debug_info().map == held.map);  // state untouched

        f = std::fopen((dir + "/slot2.sav").c_str(), "w");
        std::fputs("version=1\nmap=village\nx=13\ny=9\n", f); std::fclose(f);  // old file: most keys missing
        assert(yy::load_game(2));                              // defaults fill the rest
        assert(!yy::save_info(0).corrupt && yy::save_info(0).exists);
        assert(!yy::save_info(2).corrupt);
        fs::remove_all(dir);
        yy::save_set_dir("");
    }

    std::puts("core_test ok");
}
