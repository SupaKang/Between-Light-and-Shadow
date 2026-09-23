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
#include "game.h"
#include "gfx.h"

namespace yy {
using namespace gfx;
using gfx::Font;



// ---------------------------------------------------------------- world data
enum Dir { DOWN, UP, LEFT, RIGHT };
const int DX[] = {0, 0, -1, 1}, DY[] = {1, -1, 0, 0};

struct Warp { int x, y, to, tx, ty; Dir face; };
struct Npc { int x, y; Dir face; const Sprite* down; const Sprite* side; const Sprite* up; std::string name; };
struct Map { std::vector<std::string> rows; std::vector<Warp> warps; std::vector<Npc> npcs; bool indoor; };

enum { ROOM, VILLAGE };



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

struct Game {
    Scene scene = Scene::Title;
    unsigned frame = 0;
    std::mt19937 rng;
    Clock clock;
    Phase last_phase = Phase::Dawn;

    int map = ROOM, px = 1, py = 2;
    Dir dir = DOWN;
    int step = 0, turn_wait = 0;
    bool moving = false, chain = false;

    int hp = 40, hp_max = 40, ng = 20, ng_max = 20, level = 1, money = 120;
    std::map<std::string, int> items{{"cheongsimhwan", 1}};  // item id -> count (ids from data/items.json)
    int quest = 0;  // 0: village-gate yokai not yet purified, 1: done
    std::set<long> searched;

    std::deque<Dialog> dq;
    bool menu = false;
    int menu_sel = 0, panel = -1;
    bool shop = false;
    int shop_sel = 0;
    std::string toast;
    int toast_t = 0;

    int title_sel = 0;
    int card_t = 0;

    int trans_t = -1;
    int enc_t = 0;  // encounter flash/wipe frames left
    std::function<void()> trans_mid;

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
extern std::vector<Map> maps;
void build_maps();
char tile_at(int m, int x, int y);
bool solid_tile(char c);
void say(std::vector<std::string> texts, std::string speaker = "", std::function<void(int)> done = {},
         std::vector<std::string> choices = {});
void show_toast(const std::string& s);
void transition(std::function<void()> mid);
int rnd(int lo, int hi);
unsigned hash3(int a, int b, int c);
std::string clock_str();
void wake_up();
void start_prologue();
void after_place_card();
void talk_npc(Npc& n);
void give(const std::string& item, int n, const std::string& msg);
void search(int x, int y);
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
void draw_grass(int sx, int sy, int tx, int ty);
void draw_dirt(int sx, int sy, int tx, int ty);
void draw_floor(int sx, int sy);
void draw_tile(int tx, int ty, int sx, int sy);
void field_camera(int& cx, int& cy);
void phase_tint(float& r, float& gg, float& b);
bool lantern_lit();
void render_field();
void render_actors();
void cursor(int x, int y);
void more_mark(int x, int y);
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
