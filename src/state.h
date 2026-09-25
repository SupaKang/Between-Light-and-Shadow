// Internal game state and cross-module declarations (namespace yy). Public API is game.h.
#pragma once
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <deque>
#include <functional>
#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>
#include "art.h"
#include "art_gen.h"
#include "art_legacy.h"
#include "data.h"
#include "maps_api.h"
#include "game.h"
#include "gfx.h"
#include "script.h"

namespace yy {
using namespace gfx;
using gfx::Font;



// ---------------------------------------------------------------- world data
enum Dir { DOWN, UP, LEFT, RIGHT };
const int DX[] = {0, 0, -1, 1}, DY[] = {1, -1, 0, 0};

// Runtime NPC on the current map (built from the baked NpcDef by load_map).
struct Npc { int x, y; Dir face; std::string name, sprite, talk_event; };
inline Dir dir_of(char c) { return c == 'U' ? UP : c == 'L' ? LEFT : c == 'R' ? RIGHT : DOWN; }




// ---------------------------------------------------------------- state
enum class Scene { Title, Prologue, PlaceCard, Field, Battle };

struct Dialog {
    std::string speaker;
    std::vector<std::string> pages;
    size_t page = 0;
    float shown = 0;
    std::vector<std::string> choices;
    int choice = 0;
    std::function<void(int)> done;
};

struct Msg { std::string text; std::function<void()> fx; };

struct Battle {
    int ehp = 36, emax = 36;
    enum St { Msgs, Cmd, List } st = Msgs;
    int cmd = 0, list = 0, sel = 0;
    std::deque<Msg> msgs;
    float shown = 0;
    bool fx_done = false;
    std::function<void()> after;
    int shake = 0, flash = 0;
    int intro = 0;  // slide-in frames
    int dhp = 0, dng = 0, dehp = 36;  // displayed values; message fx move them
};

// Scrolling cursor list (menus, bag, shop, title). Rows are 20px; icons, if given, sit between cursor and text.
struct ListMenu {
    std::vector<std::string> items;
    std::vector<bool> enabled;           // empty = all enabled
    int sel = 0, top = 0, rows = 6;      // visible rows
    std::vector<const Sprite*> icons;    // empty = no icon column
    // Returns true when the player confirmed or cancelled; chosen = index or -1 on cancel.
    bool update(const Input& in, int& chosen);
    void render(int x, int y, int w, const std::vector<std::string>& right = {}) const;  // right-aligned column
};
// Quantity picker: up/down +-1, right/left +-10, clamped to [min, max].
struct NumberPicker {
    int value = 1, min = 1, max = 99;
    bool update(const Input& in, int& chosen);
    void render(int x, int y) const;
};

struct Game {
    Scene scene = Scene::Title;
    unsigned frame = 0;
    std::mt19937 rng;
    Clock clock;
    Phase last_phase = Phase::Dawn;

    std::string map_id = "tavern_room";
    const MapDef* mdef = nullptr;    // cached find_map(map_id); set by load_map
    std::vector<Npc> npcs;           // runtime NPCs of the current map
    std::map<std::string, int> flags;
    int px = 1, py = 2;
    Dir dir = DOWN;
    int step = 0, turn_wait = 0;
    bool moving = false, chain = false;

    int hp = 40, hp_max = 40, ng = 20, ng_max = 20, level = 1, money = 120;
    std::map<std::string, int> items{{"cheongsimhwan", 1}};  // item id -> count (ids from data/items.json)
    std::set<std::string> searched;  // "map:x:y" objects searched today

    std::deque<Dialog> dq;
    bool menu = false;
    ListMenu menu_list;  // filled by open_menu
    int panel = -1;
    ListMenu bag_list{{}, {}, 0, 0, 7, {}};
    bool shop = false, buying = false;
    std::vector<std::string> shop_items;  // item ids on sale (open_shop)
    ListMenu shop_list;
    NumberPicker qty;
    std::string toast;
    int toast_t = 0;

    ListMenu title_menu{{"새로 시작", "이어하기"}, {true, false}, 0, 0, 2, {}};  // continue: enabled by saves (Task 7)
    int card_t = 0;

    int trans_t = -1;
    int enc_t = 0;  // encounter flash/wipe frames left
    std::function<void()> trans_mid;

    std::string battle_enemy;  // set by the battle() script call
    Battle bt;
};
constexpr int kTrans = 18;
constexpr int kEnc = 48;

// ---------------------------------------------------------------- rendering: tiles (4-colour ramps)
const Color* const GR = art::RAMP_GRASS;
const Color* const EA = art::RAMP_EARTH;
const Color* const ST = art::RAMP_STONE;

extern Game g;
inline int have(const std::string& id) { auto it = g.items.find(id); return it == g.items.end() ? 0 : it->second; }
inline int flag(const std::string& k) { auto it = g.flags.find(k); return it == g.flags.end() ? 0 : it->second; }
void load_map(const std::string& id);
char tile_at(int x, int y);             // current map; '~' outside
void run_event(const std::string& name, int x = -1, int y = -1);
Npc* npc_named(const std::string& name);
bool solid_tile(char c);
void say(std::vector<std::string> texts, std::string speaker = "", std::function<void(int)> done = {},
         std::vector<std::string> choices = {});
void show_toast(const std::string& s);
void transition(std::function<void()> mid);
int rnd(int lo, int hi);
unsigned hash3(int a, int b, int c);
std::string clock_str();
void wake_up();
void bmsg(std::string t, std::function<void()> fx = {});
void end_battle(bool won);
void enemy_turn();
void check_enemy_down();
void hit_enemy(int d);
void battle_act(int list, int sel);
void begin_battle();
void start_battle();
void update_battle(const Input& in);
void on_arrive();
bool npc_at(int x, int y);
void bump_edge(int ny);
void update_field(const Input& in);
void update_dialog(const Input& raw);
void update_menu(const Input& in);
void update_shop(const Input& in);
void open_menu();
void open_shop(const std::vector<std::string>& ids);
void draw_grass(int sx, int sy, int tx, int ty);
void draw_dirt(int sx, int sy, int tx, int ty);
void draw_floor(int sx, int sy);
void draw_tile(int tx, int ty, int sx, int sy);
void field_camera(int& cx, int& cy);
void phase_tint(float& r, float& gg, float& b);
bool lantern_lit();
void render_field();
void render_actors();
const Sprite* field_sprite(const std::string& name, Dir d);
void placeholder(const std::string& label, int x, int y, int w, int h);
void cursor(int x, int y);
void more_mark(int x, int y);
void more_up(int x, int y);
void name_tag(int x, int y, const std::string& s);
void render_dialog();
void render_toast();
void header(int x, int y, int icon, const std::string& s);
void render_menu();
void render_shop();
void render_encounter();
void title_world();
void title_ui();
void prologue_world();
void prologue_ui();
void card_ui();
void battle_world();
void battle_sprites();
void battle_ui();
}  // namespace yy
