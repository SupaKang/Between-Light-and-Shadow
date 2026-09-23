#include "game.h"
#include "art.h"
#include "art_gen.h"
#include "art_legacy.h"
#include "gfx.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <deque>
#include <functional>
#include <random>
#include <set>
#include <vector>

using namespace gfx;
using gfx::Font;

Phase phase_of(float m) {
    int h = (int)m / 60;
    if (h >= 5 && h < 7) return Phase::Dawn;
    if (h >= 7 && h < 17) return Phase::Day;
    if (h >= 17 && h < 19) return Phase::Dusk;
    return Phase::Night;
}

void rest(Clock& c) {
    c.day += 1;
    c.minute = 6 * 60;
}

namespace {


// ---------------------------------------------------------------- world data
enum Dir { DOWN, UP, LEFT, RIGHT };
const int DX[] = {0, 0, -1, 1}, DY[] = {1, -1, 0, 0};

struct Warp { int x, y, to, tx, ty; Dir face; };
struct Npc { int x, y; Dir face; const Sprite* down; const Sprite* side; const Sprite* up; std::string name; };
struct Map { std::vector<std::string> rows; std::vector<Warp> warps; std::vector<Npc> npcs; bool indoor; };

enum { ROOM, VILLAGE };
std::vector<Map> maps;

// ponytail: maps live in code until the Lua/data map format lands with the event system.
void build_maps() {
    Map room;
    room.indoor = true;
    room.rows = {
        "########",
        "#B==R=J#",
        "#======#",
        "#======#",
        "#======#",
        "###D####",
    };
    room.warps = {{3, 5, VILLAGE, 6, 9, DOWN}};

    Map v;
    v.indoor = false;
    v.rows = {
        "TTTTTTTTTTTT....TTTTTTTTTTTT",
        "TTTTTTTTTTTG....GTTTTTTTTTTT",
        "T,,,,,,,,,,,....,,,,,,,,,,,T",
        "T,,,,,,,,,,,....,,,,,,,,,,,T",
        "T,SSSSSSSSS,....,,hhhhhh,,,T",
        "T,S,,,,,,,S,....,,hhhhhh,,,T",
        "T,S,hhhhh,S,....,,HHeHHH,,,T",
        "T,S,hhhhh,S,............,,,T",
        "T,S,HHdHH,S,............,,,T",
        "T,S,...,JJS,....,,,,,,,,,,,T",
        "T,S,,,.,JJS,....,,,,W,,,,,,T",
        "T,S,,,.,,,S,....,,,,,,,,,,,T",
        "T,S,,,.,,,S,....,,,,,,,,,,,T",
        "T,SSSS.SSSS,....,,,,,,,,,,,T",
        "T,,,,,..........,,,,,,,,,,,T",
        "T,,,,,,,,,,,....,,,,,,,,,,,T",
        "T,,,,,,,,,,,....,,,,,,,,,,,T",
        "TTTTTTTTTTTT....TTTTTTTTTTTT",
        "TTTTTTTTTTTTTTTTTTTTTTTTTTTT",
    };
    v.warps = {{6, 8, ROOM, 3, 4, UP}};
    v.npcs = {{5, 11, DOWN, &art::npc_jumo_field_down, &art::npc_jumo_field_side, &art::npc_jumo_field_up, "주모"},
              {11, 3, RIGHT, &art::npc_village_elder_field_down, &art::npc_village_elder_field_side,
               &art::npc_village_elder_field_up, "노인"}};
    maps = {room, v};
}

char tile_at(int m, int x, int y) {
    const Map& mp = maps[m];
    if (y < 0 || y >= (int)mp.rows.size() || x < 0 || x >= (int)mp.rows[y].size()) return '~';
    return mp.rows[y][x];
}

bool solid_tile(char c) { return std::string("#BRJSThHWGe~").find(c) != std::string::npos; }

// ---------------------------------------------------------------- items
struct ItemDef { const char* name; const char* desc; int price; };
const ItemDef ITEMS[] = {
    {"한지", "부적을 쓰는 데 드는 종이. 부적 조제 재료.", 5},
    {"청심환", "체력을 20 회복한다.", 30},
    {"계약 부적", "약해진 요괴와 약조를 맺을 때 쓴다.", 40},
};
enum { HANJI, CHEONGSIM, CONTRACT, ITEM_COUNT };

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
    int items[ITEM_COUNT] = {0, 1, 0};
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
} g;

// ---------------------------------------------------------------- helpers
void say(std::vector<std::string> texts, std::string speaker = "", std::function<void(int)> done = {},
         std::vector<std::string> choices = {}) {
    Dialog d;
    d.speaker = std::move(speaker);
    for (auto& t : texts) {
        auto lines = wrap(t, 570);
        for (size_t i = 0; i < lines.size(); i += 2)
            d.pages.push_back(lines[i] + (i + 1 < lines.size() ? "\n" + lines[i + 1] : ""));
    }
    d.choices = std::move(choices);
    d.done = std::move(done);
    g.dq.push_back(std::move(d));
}

void show_toast(const std::string& s) { g.toast = s; g.toast_t = 180; }

void transition(std::function<void()> mid) {
    if (g.trans_t >= 0) return;
    g.trans_t = 0;
    g.trans_mid = std::move(mid);
}
constexpr int kTrans = 18;
constexpr int kEnc = 48;

int rnd(int lo, int hi) { return std::uniform_int_distribution<int>(lo, hi)(g.rng); }

unsigned hash3(int a, int b, int c) {
    unsigned h = (unsigned)a * 73856093u ^ (unsigned)b * 19349663u ^ (unsigned)c * 83492791u;
    h ^= h >> 13;
    h *= 0x5bd1e995u;
    return h ^ (h >> 15);
}

std::string clock_str() {
    int m = (int)g.clock.minute;
    char buf[48];
    const char* names[] = {"새벽", "낮", "저녁", "밤"};
    snprintf(buf, sizeof buf, "%d일째 · %s %02d:%02d", g.clock.day, names[(int)phase_of(g.clock.minute)], m / 60, m % 60);
    return buf;
}

// ---------------------------------------------------------------- story beats
void wake_up() {
    g.scene = Scene::Field;
    g.map = ROOM;
    g.px = 1; g.py = 2; g.dir = DOWN;
    g.hp = g.hp_max; g.ng = g.ng_max;
    g.last_phase = phase_of(g.clock.minute);
}

void start_prologue() {
    g.scene = Scene::Prologue;
    say({"……어서 오게. 밤길이 차지. 등불 곁으로 앉게나.",
         "옛날 옛적, 하늘과 땅의 기운이 아직 제 길을 찾지 못하던 때의 일이오.",
         "산천의 영맥이 뒤엉켜, 백팔 가지 요괴가 밤낮을 가리지 않고 날뛰었지.",
         "자네, 혹시 이 이야기를 들어 본 적 있는가?"},
        "노승", [](int c) {
            say({c == 0 ? "허허, 그렇다면 끝까지 들어 보게. 사람들이 잊은 대목이 있으니."
                        : "그렇겠지. 요즘 사람들은 이런 옛이야기를 잘 하지 않으니.",
                 "그때 이름 없는 음양사 하나가 나타났소. 그는 요괴를 베지 않고, 하나하나 달래어 약조를 맺었다오.",
                 "백팔 요괴와 맺은 약조를 한 장의 부적에 새기니, 그것이 천지음양부(天地陰陽符)라.",
                 "부적이 영맥을 붙들자 세상은 낮과 밤을 되찾았고, 관상감이 대대로 그것을 지켜왔지.",
                 "……허나 요즘 그 부적이 찢어졌다는 소문이 돌더이다. 그래, 자네는 어디로 가는 길인가?"},
                "노승", [](int c2) {
                    say({c2 == 0 ? "도선사라… 붉은 안개가 낀 곳이지. 등불을 꺼뜨리지 말게."
                                 : "말이 없는 이가 멀리 가는 법이지. 몸조심하게.",
                         "자네가 이 이야기의 다음 장이 되겠구먼."},
                        "노승", [](int) { transition([] { g.scene = Scene::PlaceCard; g.card_t = 0; }); });
                }, {"도선사로 갑니다", "(말없이 고개를 숙인다)"});
        }, {"들어 보았습니다", "처음 듣습니다"});
}

void after_place_card() {
    transition([] {
        wake_up();
        say({"(……꿈이었나. 등불 아래 노승이 옛이야기를 들려주던.)",
             "(벽사청의 명을 받아 도선사로 향하는 길이다. 오늘은 채비를 갖추고 길을 나서야 한다.)"},
            "음양사", [](int) { show_toast("임무: 마을 어귀의 요괴를 퇴치하라"); });
    });
}

void start_battle();

void talk_npc(Npc& n) {
    n.face = (Dir)(g.dir == DOWN ? UP : g.dir == UP ? DOWN : g.dir == LEFT ? RIGHT : LEFT);
    bool night = phase_of(g.clock.minute) == Phase::Night;
    if (n.name == "주모") {
        if (g.quest == 0)
            say({"일어났구려, 나리. 벽사청에서 오셨다지요?",
                 "요 며칠 마을 어귀 장승에서 검붉은 기운이 새어 나와요. 밤이면 장승이 걸어 다닌다는 소문까지 돌고…",
                 "길 떠나시기 전에 저 건너 부적방에 들러 채비를 하시구려. 어귀는 북쪽 큰길 끝이오."},
                n.name);
        else
            say({"어귀가 조용해졌어요! 이제야 발 뻗고 자겠네.", "고단하면 방에서 쉬어 가시오. 방값은 벌써 치렀으니."}, n.name);
    } else {
        if (g.quest == 0) {
            std::vector<std::string> t = {"거기 서게. 저 장승이 요사스러워진 뒤로 아무도 어귀를 못 지나다니네.",
                                          "자네가 음양사라면… 부디 장승 할아범을 달래 주게."};
            if (night) t.push_back("…보게, 밤이 되니 장승 눈이 붉게 번들거리지 않나.");
            say(t, n.name);
        } else {
            say({"장승이 제 모습을 찾았구먼. 고맙네, 젊은 음양사.", "도선사로 가려거든 이 길로 북쪽 고개를 넘게."}, n.name);
        }
    }
}

void give(int item, int n, const std::string& msg) {
    g.items[item] += n;
    say({msg});
}

void search(int x, int y) {
    char c = tile_at(g.map, x, y);
    long key = (long)g.map * 100000 + y * 1000 + x;
    bool again = g.searched.count(key) > 0;
    bool night = phase_of(g.clock.minute) == Phase::Night;
    switch (c) {
        case 'R':
            if (again) return say({"쌀이 반쯤 차 있는 뒤주다."});
            g.searched.insert(key);
            return give(HANJI, 2, "뒤주 안 쌀 사이에서 한지 2장을 찾았다!");
        case 'J': {
            if (again) return say({"이미 들여다본 항아리다."});
            g.searched.insert(key);
            unsigned h = hash3(g.clock.day, x, y) % 3;
            if (h == 0) { g.money += 10; return say({"항아리 바닥에서 엽전 10냥을 찾았다!"}); }
            if (h == 1) return give(CHEONGSIM, 1, "항아리 속에 청심환 1개가 숨겨져 있었다!");
            return say({"장 냄새만 가득하다. 아무것도 없다."});
        }
        case 'W':
            if (night) return say({"우물 속 깊은 곳에서 누군가 이름을 부르는 것 같다……", "(밤에는 물가를 조심해야겠다.)"});
            return say({"맑은 우물이다. 두레박이 걸려 있다."});
        case 'B':
            return say({"이부자리가 깔려 있다. 쉬어 갈까?"}, "", [](int c) {
                if (c != 0) return;
                transition([] {
                    rest(g.clock);
                    g.searched.clear();
                    g.hp = g.hp_max; g.ng = g.ng_max;
                    g.last_phase = phase_of(g.clock.minute);
                    show_toast(clock_str() + " — 몸이 가뿐하다");
                });
            }, {"쉰다", "그만둔다"});
        case 'e':
            return say({"어서 오시오. 부적방이오. 무얼 드릴까?"}, "부적방", [](int) { g.shop = true; g.shop_sel = 0; });
        case 'G':
            if (g.quest == 0) return start_battle();
            return say({"요기가 걷힌 장승이 묵묵히 마을 어귀를 지키고 있다."});
        case 'S': return say({"이끼 낀 돌담이다."});
        case 'H': case 'h': return say({"초가집이다."});
        default: break;
    }
}

// ---------------------------------------------------------------- battle
void bmsg(std::string t, std::function<void()> fx = {}) { g.bt.msgs.push_back({std::move(t), std::move(fx)}); }

void end_battle(bool won) {
    transition([won] {
        g.scene = Scene::Field;
        if (won) {
            g.quest = 1;
            say({"장승의 눈빛이 순하게 가라앉았다.", "임무 완료: 마을 어귀의 요괴 퇴치", "(북쪽 도선사 고개로 가는 길이 열렸다.)"});
        } else {
            rest(g.clock);
            g.searched.clear();
            wake_up();
            say({"(……정신을 차려 보니 주막 방이다. 누군가 여기까지 데려다준 모양이다.)"});
        }
    });
}

void enemy_turn() {
    int d = rnd(3, 5);
    g.hp = std::max(0, g.hp - d);
    int v = g.hp;
    bmsg("장승귀의 뿌리 휘감기!", [] { g.bt.shake = 12; });
    bmsg("음양사는 " + std::to_string(d) + "의 피해를 입었다.", [v] { g.bt.dhp = v; g.bt.flash = 8; });
    if (g.hp == 0) {
        bmsg("눈앞이 캄캄해졌다……");
        g.bt.after = [] { end_battle(false); };
    }
}

void check_enemy_down() {
    if (g.bt.ehp > 0) return enemy_turn();
    bmsg("장승귀를 물리쳤다!");
    bmsg("장승에 서린 요기가 흩어졌다.");
    bmsg("경험치 15를 얻었다.");
    g.bt.after = [] { end_battle(true); };
}

void hit_enemy(int d) {
    g.bt.ehp = std::max(0, g.bt.ehp - d);
    int v = g.bt.ehp;
    bmsg("장승귀에게 " + std::to_string(d) + "의 피해!", [v] { g.bt.dehp = v; g.bt.flash = -8; });
    check_enemy_down();
}

struct Entry { std::string name, info; bool ok; };

std::vector<Entry> battle_entries(int list) {
    if (list == 0) return {{"지팡이", "기본 공격 · 내공 소모 없음", true}};
    if (list == 1) return {{"금침술 (金)", "내공 5 · 금(金)은 목(木)을 누른다", g.ng >= 5},
                           {"소환", "계약한 요괴가 없다", false}};
    std::vector<Entry> e;
    for (int i : {CHEONGSIM, CONTRACT})
        if (g.items[i] > 0) e.push_back({std::string(ITEMS[i].name) + " x" + std::to_string(g.items[i]), ITEMS[i].desc, true});
    return e;
}

void battle_act(int list, int sel) {
    Battle& b = g.bt;
    b.st = Battle::Msgs;
    if (list == 0) {
        bmsg("음양사의 지팡이 공격!", [] { g.bt.shake = -6; });
        hit_enemy(rnd(5, 7));
    } else if (list == 1) {
        if (sel == 1) { bmsg("계약한 요괴가 없다."); b.after = [] { g.bt.st = Battle::Cmd; }; return; }
        if (g.ng < 5) { bmsg("내공이 부족하다."); b.after = [] { g.bt.st = Battle::Cmd; }; return; }
        g.ng -= 5;
        int v = g.ng;
        bmsg("음양사가 금침술을 펼쳤다!", [v] { g.bt.dng = v; });
        bmsg("금(金)이 목(木)을 누른다! 효과가 굉장하다!");
        hit_enemy(rnd(6, 7) * 2);
    } else {
        auto items = std::vector<int>{};
        for (int i : {CHEONGSIM, CONTRACT}) if (g.items[i] > 0) items.push_back(i);
        int it = items[sel];
        if (it == CONTRACT) {
            bmsg("요기에 물든 장승이다. 약조를 맺을 수 없다!");
            b.after = [] { g.bt.st = Battle::Cmd; };
            return;
        }
        g.items[CHEONGSIM]--;
        g.hp = std::min(g.hp_max, g.hp + 20);
        int v = g.hp;
        bmsg("청심환을 먹었다. 체력이 회복되었다.", [v] { g.bt.dhp = v; });
        enemy_turn();
    }
}

void begin_battle() {
    g.scene = Scene::Battle;
    g.bt = Battle{};
    g.bt.dhp = g.hp; g.bt.dng = g.ng;
    bmsg("장승귀가 길을 막아섰다!");
}

void start_battle() {
    say({"장승에 서린 요기가 꿈틀거린다……!"}, "", [](int) { g.enc_t = kEnc; });
}

void update_battle(const Input& in) {
    Battle& b = g.bt;
    if (b.shake > 0) b.shake--; else if (b.shake < 0) b.shake++;
    if (b.flash > 0) b.flash--; else if (b.flash < 0) b.flash++;
    if (b.intro < 40) { b.intro++; return; }
    if (b.st == Battle::Msgs) {
        if (b.msgs.empty()) {
            if (b.after) { auto a = b.after; b.after = {}; a(); }
            else b.st = Battle::Cmd;
            return;
        }
        Msg& m = b.msgs.front();
        if (!b.fx_done) { b.fx_done = true; if (m.fx) m.fx(); }
        b.shown += 0.75f;
        if (in.pressed[K_A] || in.pressed[K_B] || in.pressed[K_START]) {
            if (b.shown < cp_count(m.text)) b.shown = 999;
            else { b.msgs.pop_front(); b.shown = 0; b.fx_done = false; }
        }
        return;
    }
    if (b.st == Battle::Cmd) {
        if (in.pressed[K_LEFT] || in.pressed[K_RIGHT]) b.cmd ^= 1;
        if (in.pressed[K_UP] || in.pressed[K_DOWN]) b.cmd ^= 2;
        if (in.pressed[K_A]) {
            if (b.cmd == 3) { b.st = Battle::Msgs; bmsg("마을을 지켜야 한다. 물러설 수 없다!"); return; }
            if (b.cmd == 2 && battle_entries(2).empty()) { b.st = Battle::Msgs; bmsg("쓸 수 있는 도구가 없다."); return; }
            b.list = b.cmd; b.sel = 0; b.st = Battle::List;
        }
        return;
    }
    auto e = battle_entries(b.list);
    int n = (int)e.size();
    if (in.pressed[K_UP]) b.sel = (b.sel + n - 1) % n;
    if (in.pressed[K_DOWN]) b.sel = (b.sel + 1) % n;
    if (in.pressed[K_B]) b.st = Battle::Cmd;
    if (in.pressed[K_A]) battle_act(b.list, b.sel);
}

// ---------------------------------------------------------------- field update
void on_arrive() {
    for (auto& w : maps[g.map].warps)
        if (w.x == g.px && w.y == g.py) {
            Warp wp = w;
            transition([wp] { g.map = wp.to; g.px = wp.tx; g.py = wp.ty; g.dir = wp.face; });
            return;
        }
    if (g.map == VILLAGE && g.py == 2 && g.px >= 12 && g.px <= 15 && g.quest == 0) {
        g.dir = UP;
        say({"어귀의 장승 둘이 붉은 눈으로 이쪽을 내려다본다."}, "", [](int) { start_battle(); });
    }
}

bool npc_at(int x, int y) {
    for (auto& n : maps[g.map].npcs) if (n.x == x && n.y == y) return true;
    return false;
}

void bump_edge(int ny) {
    if (g.map != VILLAGE) return;
    if (ny < 0) {
        if (g.quest == 0) say({"장승의 요기가 길을 막고 있다. 먼저 장승을 살펴야 한다."});
        else say({"(도선사 고개 — 이 너머는 다음 구간에서 이어진다.)"});
    } else if (ny >= 17 && g.py == 16) {
        say({"남쪽은 한양으로 돌아가는 길이다. 지금은 도선사로 가야 한다."});
    }
}

void update_field(const Input& in) {
    // Clock only runs while the player is free to act.
    g.clock.minute += 1.0f / 60.0f;
    if (g.clock.minute >= 1440) { g.clock.minute -= 1440; g.clock.day++; }
    Phase ph = phase_of(g.clock.minute);
    if (ph != g.last_phase) {
        g.last_phase = ph;
        const char* t[] = {"동이 튼다", "날이 밝았다", "해가 저문다", "밤이 깊었다. 요기가 짙어진다"};
        show_toast(t[(int)ph]);
    }

    if (g.moving) {
        g.step += in.held[K_B] ? 2 : 1;
        if (g.step >= 16) {
            g.px += DX[g.dir]; g.py += DY[g.dir];
            g.step = 0; g.moving = false; g.chain = true;
            on_arrive();
        }
        return;
    }
    if (in.pressed[K_START]) { g.menu = true; g.menu_sel = 0; g.panel = -1; return; }
    if (in.pressed[K_A]) {
        int tx = g.px + DX[g.dir], ty = g.py + DY[g.dir];
        for (auto& n : maps[g.map].npcs) if (n.x == tx && n.y == ty) return talk_npc(n);
        return search(tx, ty);
    }
    int d = in.held[K_UP] ? UP : in.held[K_DOWN] ? DOWN : in.held[K_LEFT] ? LEFT : in.held[K_RIGHT] ? RIGHT : -1;
    if (d < 0) { g.chain = false; g.turn_wait = 0; return; }
    if (d != g.dir && !g.chain) { g.dir = (Dir)d; g.turn_wait = 5; return; }  // tap = turn in place
    g.dir = (Dir)d;
    if (g.turn_wait > 0) { g.turn_wait--; return; }
    int nx = g.px + DX[d], ny = g.py + DY[d];
    if (solid_tile(tile_at(g.map, nx, ny)) || npc_at(nx, ny)) {
        if (in.pressed[d]) bump_edge(ny);
        g.chain = false;
        return;
    }
    g.moving = true;
}

// ---------------------------------------------------------------- overlays update
void update_dialog(const Input& raw) {
    Input in = raw;  // Enter confirms in dialogs too
    in.pressed[K_A] = raw.pressed[K_A] || raw.pressed[K_START];  // X (B) also advances; on choices it picks the last
    Dialog& d = g.dq.front();
    int n = cp_count(d.pages[d.page]);
    d.shown += 0.75f;
    bool last = d.page + 1 == d.pages.size();
    if (last && d.shown >= n && !d.choices.empty()) {
        int c = (int)d.choices.size();
        if (in.pressed[K_UP]) d.choice = (d.choice + c - 1) % c;
        if (in.pressed[K_DOWN]) d.choice = (d.choice + 1) % c;
        if (in.pressed[K_B]) d.choice = c - 1;
        if (!in.pressed[K_A] && !in.pressed[K_B]) return;
    } else if (!in.pressed[K_A] && !in.pressed[K_B]) {
        return;
    }
    if (d.shown < n) { d.shown = 999; return; }
    if (!last) { d.page++; d.shown = 0; return; }
    auto done = d.done;
    int choice = d.choice;
    g.dq.pop_front();
    if (done) done(choice);
}

const char* MENU[] = {"요괴", "도감", "가방", "음양사", "기록", "설정", "닫기"};
constexpr int kMenuN = 7;

void update_menu(const Input& in) {
    if (g.panel >= 0) {
        if (in.pressed[K_A] || in.pressed[K_B]) g.panel = -1;
        return;
    }
    if (in.pressed[K_UP]) g.menu_sel = (g.menu_sel + kMenuN - 1) % kMenuN;
    if (in.pressed[K_DOWN]) g.menu_sel = (g.menu_sel + 1) % kMenuN;
    if (in.pressed[K_B] || in.pressed[K_START]) g.menu = false;
    if (in.pressed[K_A]) {
        if (g.menu_sel == kMenuN - 1) g.menu = false;
        else g.panel = g.menu_sel;
    }
}

const int SHOP_ITEMS[] = {CHEONGSIM, CONTRACT};

void update_shop(const Input& in) {
    int n = 3;  // two goods + leave
    if (in.pressed[K_UP]) g.shop_sel = (g.shop_sel + n - 1) % n;
    if (in.pressed[K_DOWN]) g.shop_sel = (g.shop_sel + 1) % n;
    if (in.pressed[K_B] || (in.pressed[K_A] && g.shop_sel == 2)) {
        g.shop = false;
        say({"또 들르시오. 길 조심하시고."}, "부적방");
        return;
    }
    if (in.pressed[K_A]) {
        int it = SHOP_ITEMS[g.shop_sel];
        if (g.money < ITEMS[it].price) { show_toast("엽전이 모자란다"); return; }
        g.money -= ITEMS[it].price;
        g.items[it]++;
        show_toast(std::string(ITEMS[it].name) + "을(를) 샀다");
    }
}

// ---------------------------------------------------------------- rendering: tiles (4-colour ramps)
const Color* const GR = art::RAMP_GRASS;
const Color* const EA = art::RAMP_EARTH;
const Color* const ST = art::RAMP_STONE;

void draw_grass(int sx, int sy, int tx, int ty) {
    rect(sx, sy, 16, 16, GR[1]);
    for (int k = 0; k < 3; ++k) {
        unsigned h = hash3(tx, ty, k);
        int x = sx + h % 13, y = sy + (h >> 8) % 13;
        pset(x, y + 1, GR[2]); pset(x + 1, y + 2, GR[2]); pset(x + 2, y + 1, GR[2]);
        pset(x + 1, y, GR[0]);
    }
}

void draw_dirt(int sx, int sy, int tx, int ty) {
    rect(sx, sy, 16, 16, EA[0]);
    for (int k = 0; k < 2; ++k) {
        unsigned h = hash3(tx, ty, k + 7);
        pset(sx + h % 15, sy + (h >> 8) % 15, EA[1]);
    }
    // Ragged grass fringe where the road meets grass.
    auto grassy = [&](int x, int y) { char c = tile_at(g.map, x, y); return c == ',' || c == 'T' || c == 'S'; };
    for (int i = 0; i < 16; ++i) {
        bool jag = (i + tx + ty) % 3 != 0;
        if (grassy(tx - 1, ty)) { pset(sx, sy + i, GR[1]); if (jag) pset(sx + 1, sy + i, GR[1]); }
        if (grassy(tx + 1, ty)) { pset(sx + 15, sy + i, GR[1]); if (jag) pset(sx + 14, sy + i, GR[1]); }
        if (grassy(tx, ty - 1)) { pset(sx + i, sy, GR[1]); if (jag) pset(sx + i, sy + 1, GR[1]); }
        if (grassy(tx, ty + 1)) { pset(sx + i, sy + 15, GR[1]); if (jag) pset(sx + i, sy + 14, GR[1]); }
    }
}

void draw_floor(int sx, int sy) {
    rect(sx, sy, 16, 16, EA[1]);
    rect(sx + 15, sy, 1, 16, EA[2]);
    rect(sx, sy + 15, 16, 1, EA[2]);
    pset(sx + 4, sy + 4, EA[0]);
}

void draw_tile(int tx, int ty, int sx, int sy) {
    char c = tile_at(g.map, tx, ty);
    bool indoor = maps[g.map].indoor;
    auto floorish = [](char k) { return k == '=' || k == 'B' || k == 'R' || k == 'J' || k == 'D'; };
    switch (c) {
        case '=': draw_floor(sx, sy); break;
        case 'D':
            draw_floor(sx, sy);
            rect(sx + 2, sy + 3, 12, 10, EA[3]);
            rect(sx + 3, sy + 4, 10, 8, EA[2]);
            break;
        case 'B':
            draw_floor(sx, sy);
            rect(sx + 1, sy + 1, 14, 14, EA[3]);
            rect(sx + 2, sy + 2, 12, 12, EA[2]);
            rect(sx + 2, sy + 2, 12, 4, EA[0]);
            rect(sx + 2, sy + 6, 12, 1, EA[3]);
            rect(sx + 4, sy + 9, 3, 1, EA[1]); rect(sx + 9, sy + 11, 3, 1, EA[1]);
            break;
        case 'R':
            draw_floor(sx, sy);
            rect(sx + 2, sy + 13, 2, 3, EA[3]); rect(sx + 12, sy + 13, 2, 3, EA[3]);
            rect(sx + 1, sy + 3, 14, 11, EA[3]);
            rect(sx + 2, sy + 4, 12, 9, EA[2]);
            rect(sx + 1, sy + 1, 14, 3, EA[3]);
            rect(sx + 2, sy + 2, 12, 1, EA[1]);
            rect(sx + 7, sy + 6, 2, 3, EA[0]);
            pset(sx + 2, sy + 4, EA[0]); pset(sx + 13, sy + 4, EA[0]);
            break;
        case '#':
            if (floorish(tile_at(g.map, tx, ty + 1))) {
                rect(sx, sy, 16, 4, EA[3]);
                rect(sx, sy + 4, 16, 9, EA[0]);
                rect(sx, sy + 8, 16, 1, EA[1]);
                rect(sx, sy + 13, 16, 3, EA[2]);
                if (tx % 3 == 0) rect(sx, sy + 4, 2, 9, EA[2]);
            } else {
                rect(sx, sy, 16, 16, EA[3]);
            }
            break;
        case ',': draw_grass(sx, sy, tx, ty); break;
        case '.': case 'G': draw_dirt(sx, sy, tx, ty); break;
        case 'T':
            draw_grass(sx, sy, tx, ty);
            rect(sx + 6, sy + 12, 4, 4, GR[3]);
            disc(sx + 8, sy + 7, 8, GR[3]);
            disc(sx + 8, sy + 7, 7, GR[2]);
            disc(sx + 6, sy + 5, 3, GR[1]);
            pset(sx + 5, sy + 4, GR[0]);
            break;
        case 'S':
            draw_grass(sx, sy, tx, ty);
            for (int r = 0; r < 2; ++r)
                for (int k = -1; k < 3; ++k) {
                    int x = sx + k * 6 + (r ? 3 : 0), y = sy + 3 + r * 6;
                    int x0 = std::max(x, sx), x1 = std::min(x + 6, sx + 16);
                    rect(x0, y, x1 - x0, 6, ST[3]);
                    int i0 = std::max(x + 1, sx), i1 = std::min(x + 5, sx + 16);
                    rect(i0, y + 1, i1 - i0, 4, ST[1]);
                    if (x + 1 >= sx && x + 1 < sx + 16) pset(x + 1, y + 1, ST[0]);
                }
            break;
        case 'h': {
            bool top = tile_at(g.map, tx, ty - 1) != 'h', bot = tile_at(g.map, tx, ty + 1) != 'h';
            bool lft = tile_at(g.map, tx - 1, ty) != 'h', rgt = tile_at(g.map, tx + 1, ty) != 'h';
            if (top) draw_grass(sx, sy, tx, ty);
            for (int y = top ? 3 : 0; y < 16; ++y)
                for (int x = 0; x < 16; ++x) {
                    int wx = tx * 16 + x, wy = ty * 16 + y;
                    bool line = (wy % 4 == 0 && hash3(wx / 3, wy, 1) % 4) || (wx + wy) % 8 == 0 || (wx - wy + 800) % 8 == 0;
                    pset(sx + x, sy + y, line ? EA[2] : EA[1]);
                }
            if (top) { rect(sx, sy + 3, 16, 1, EA[3]); rect(sx, sy + 4, 16, 2, EA[2]); }
            if (bot) { rect(sx, sy + 13, 16, 3, EA[3]); rect(sx, sy + 12, 16, 1, EA[2]); }
            if (lft) rect(sx, sy + (top ? 3 : 0), 1, 16 - (top ? 3 : 0), EA[3]);
            if (rgt) rect(sx + 15, sy + (top ? 3 : 0), 1, 16 - (top ? 3 : 0), EA[3]);
            break;
        }
        case 'H': case 'd': case 'e': {
            rect(sx, sy, 16, 16, EA[0]);
            rect(sx, sy, 16, 2, EA[1]);
            rect(sx, sy + 13, 16, 3, EA[2]);
            rect(sx, sy + 13, 16, 1, EA[3]);
            if (tile_at(g.map, tx - 1, ty) != 'H' || tx % 2 == 0) rect(sx, sy, 2, 13, EA[2]);
            if (c == 'H' && tx % 2 == 1) {
                rect(sx + 4, sy + 4, 8, 7, EA[2]);
                rect(sx + 5, sy + 5, 6, 5, EA[0]);
                rect(sx + 7, sy + 5, 1, 5, EA[1]); rect(sx + 5, sy + 7, 6, 1, EA[1]);
            } else if (c != 'H') {
                rect(sx + 3, sy + 2, 10, 11, EA[2]);
                rect(sx + 4, sy + 3, 8, 10, EA[0]);
                for (int x = 5; x < 12; x += 3) rect(sx + x, sy + 3, 1, 10, EA[1]);
                rect(sx + 4, sy + 8, 8, 1, EA[1]);
                rect(sx + 3, sy + 13, 10, 3, EA[1]);
                if (c == 'e') { rect(sx + 5, sy, 6, 6, EA[3]); rect(sx + 7, sy + 2, 2, 2, EA[1]); }
            }
            break;
        }
        case 'W':
            draw_grass(sx, sy, tx, ty);
            disc(sx + 8, sy + 9, 7, ST[3]);
            disc(sx + 8, sy + 9, 6, ST[1]);
            disc(sx + 8, sy + 9, 4, ST[2]);
            disc(sx + 8, sy + 9, 3, ST[3]);
            pset(sx + 7, sy + 8, ST[0]);
            break;
        case 'J':
            if (indoor) draw_floor(sx, sy); else draw_dirt(sx, sy, tx, ty);
            disc(sx + 8, sy + 9, 6, EA[3]);
            disc(sx + 8, sy + 9, 5, EA[2]);
            rect(sx + 5, sy + 2, 7, 3, EA[3]);
            rect(sx + 6, sy + 2, 5, 2, EA[2]);
            pset(sx + 5, sy + 7, EA[1]); pset(sx + 6, sy + 6, EA[1]);
            break;
        default:
            rect(sx, sy, 16, 16, 0x101010);
    }
}

void field_camera(int& cx, int& cy) {
    const Map& m = maps[g.map];
    int mw = (int)m.rows[0].size() * 16, mh = (int)m.rows.size() * 16;
    int wx = g.px * 16 + DX[g.dir] * g.step, wy = g.py * 16 + DY[g.dir] * g.step;
    cx = mw <= W ? (mw - W) / 2 : std::clamp(wx + 8 - W / 2, 0, mw - W);
    cy = mh <= H ? (mh - H) / 2 : std::clamp(wy + 8 - H / 2, 0, mh - H);
}

void phase_tint(float& r, float& gg, float& b) {
    Phase ph = phase_of(g.clock.minute);
    r = gg = b = 1;
    if (maps[g.map].indoor) {
        if (ph == Phase::Night) r = 0.8f, gg = 0.7f, b = 0.62f;
    } else {
        if (ph == Phase::Dawn) r = 0.86f, gg = 0.84f, b = 0.98f;
        if (ph == Phase::Dusk) r = 1.0f, gg = 0.78f, b = 0.62f;
        if (ph == Phase::Night) r = 0.42f, gg = 0.48f, b = 0.78f;
    }
}

constexpr int kLampIn = 22, kLampOut = 34;  // world pixels
bool lantern_lit() { return !maps[g.map].indoor && phase_of(g.clock.minute) == Phase::Night; }

// World layer (320x180): tiles only.
void render_field() {
    int cx, cy;
    field_camera(cx, cy);
    clear(0x101010);
    for (int ty = cy / 16 - 1; ty <= (cy + H) / 16 + 1; ++ty)
        for (int tx = cx / 16 - 1; tx <= (cx + W) / 16 + 1; ++tx)
            if (tile_at(g.map, tx, ty) != '~') draw_tile(tx, ty, tx * 16 - cx, ty * 16 - cy);
    float r, gg, b;
    phase_tint(r, gg, b);
    if (lantern_lit()) {  // night outdoors: the hero's ghost-fire lantern keeps a small pool of light
        int hx = g.px * 16 + DX[g.dir] * g.step - cx + 8, hy = g.py * 16 + DY[g.dir] * g.step - cy + 8;
        tint_lit(hx, hy, kLampIn, kLampOut, r, gg, b);
    } else if (r != 1 || gg != 1 || b != 1) {
        tint(r, gg, b);
    }
}

// UI layer (640x360): 32x48 characters at twice the world resolution, Y-sorted by feet.
void render_actors() {
    const Map& m = maps[g.map];
    int cx, cy;
    field_camera(cx, cy);
    auto ux = [cx](int wx) { return (wx - cx) * 2; };
    auto uy = [cy](int wy) { return (wy - cy) * 2; };
    int hwx = g.px * 16 + DX[g.dir] * g.step, hwy = g.py * 16 + DY[g.dir] * g.step;
    float tr, tg, tb;
    phase_tint(tr, tg, tb);
    // Actors inside the lantern pool keep their colours at night; everyone else takes the phase tint.
    auto lit = [=](int wx, int wy) {
        bool in = lantern_lit() && (wx - hwx) * (wx - hwx) + (wy - hwy) * (wy - hwy) < kLampIn * kLampIn;
        if (in) sprite_tint(1, 1, 1); else sprite_tint(tr, tg, tb);
    };
    struct D { int y; std::function<void()> f; };
    std::vector<D> ds;
    for (int ty = 0; ty < (int)m.rows.size(); ++ty)
        for (int tx = 0; tx < (int)m.rows[ty].size(); ++tx)
            if (m.rows[ty][tx] == 'G') {
                int sx = ux(tx * 16), sy = uy(ty * 16);
                ds.push_back({ty * 16 + 15, [=] {
                    lit(tx * 16, ty * 16);
                    sprite(art::jangseung, sx, sy - 32, false, 2);
                    if (g.quest == 0 && (g.frame / 20) % 2) { rect(sx + 10, sy - 20, 2, 2, UI_GLOW); rect(sx + 20, sy - 20, 2, 2, UI_GLOW); }
                }});
            }
    for (auto& n : m.npcs) {
        int sx = ux(n.x * 16), sy = uy(n.y * 16);
        const Npc* np = &n;
        ds.push_back({n.y * 16 + 15, [=] {
            lit(np->x * 16, np->y * 16);
            const Sprite* s = np->face == UP ? np->up : np->face == DOWN ? np->down : np->side;
            sprite(*s, sx, sy - 18, np->face == LEFT);
        }});
    }
    int wx = g.px * 16 + DX[g.dir] * g.step, wy = g.py * 16 + DY[g.dir] * g.step;
    int hx = ux(wx), hy = uy(wy);
    ds.push_back({wy + 15, [=] {
        lit(hwx, hwy);
        // stride alternates per tile so consecutive steps use opposite feet
        int stride = !g.moving ? 0 : ((g.step < 8) == ((g.px + g.py) % 2 == 0) ? 1 : 2);
        // Day: staff only. Night: staff + ghost-fire lantern.
        static const Sprite* const day[3][3] = {
            {&art::npc_hero_field_down, &art::npc_hero_field_down_step1, &art::npc_hero_field_down_step2},
            {&art::npc_hero_field_up, &art::npc_hero_field_up_step1, &art::npc_hero_field_up_step2},
            {&art::npc_hero_field_side, &art::npc_hero_field_side_step1, &art::npc_hero_field_side_step2}};
        static const Sprite* const night[3][3] = {
            {&art::npc_hero_night_field_down, &art::npc_hero_night_field_down_step1, &art::npc_hero_night_field_down_step2},
            {&art::npc_hero_night_field_up, &art::npc_hero_night_field_up_step1, &art::npc_hero_night_field_up_step2},
            {&art::npc_hero_night_field_side, &art::npc_hero_night_field_side_step1, &art::npc_hero_night_field_side_step2}};
        int row = g.dir == DOWN ? 0 : g.dir == UP ? 1 : 2;
        bool dark = phase_of(g.clock.minute) == Phase::Night;
        sprite(*(dark ? night : day)[row][stride], hx, hy - 18, g.dir == LEFT);
    }});
    std::stable_sort(ds.begin(), ds.end(), [](const D& a, const D& b) { return a.y < b.y; });
    for (auto& d : ds) d.f();
    sprite_tint(1, 1, 1);
}

// ---------------------------------------------------------------- rendering: UI (640x360 layer)
const Color TXT = UI_TEXT, TXT_DIM = UI_DIM, TXT_RED = UI_GLOW, TXT_LIGHT = UI_TEXT;  // TXT_RED = accent
constexpr int DX0 = 16, DY0 = 288, DW = 608, DH = 60;  // dialog box

void cursor(int x, int y) { sprite(art::ui_cursor, x + (int)((g.frame / 12) % 2), y); }

void more_mark(int x, int y) {
    int b = (g.frame / 10) % 2;
    for (int i = 0; i < 4; ++i) rect(x + i, y + b + i, 7 - 2 * i, 1, TXT_RED);
}

void name_tag(int x, int y, const std::string& s) {
    int w = text_width(s, Font::Small) + 16;
    tag(x, y, w, 17);
    text(x + 8, y + 3, s, TXT_LIGHT, -1, Font::Small);
}

const Sprite* portrait_of(const std::string& who) {
    if (who == "노승") return &art::npc_old_monk_portrait;
    if (who == "음양사") return &art::npc_hero_portrait;
    if (who == "주모") return &art::npc_jumo_portrait;
    if (who == "노인") return &art::portrait_elder;
    return nullptr;
}

void render_dialog() {
    if (g.dq.empty()) return;
    const Dialog& d = g.dq.front();
    const Sprite* p = g.scene == Scene::Prologue ? nullptr : portrait_of(d.speaker);
    if (p) {
        panel(DX0, DY0 - 108, 112, 116);
        sprite(*p, DX0 + 56 - p->w / 2, DY0 - 4 - p->h);
    }
    panel(DX0, DY0, DW, DH);
    if (!d.speaker.empty()) name_tag(p ? DX0 + 120 : DX0 + 10, DY0 - 11, d.speaker);
    text(DX0 + 18, DY0 + 14, d.pages[d.page], TXT, (int)d.shown);
    bool complete = d.shown >= cp_count(d.pages[d.page]);
    bool last = d.page + 1 == d.pages.size();
    if (complete && (d.choices.empty() || !last)) more_mark(DX0 + DW - 22, DY0 + DH - 17);
    if (complete && last && !d.choices.empty()) {
        int n = (int)d.choices.size(), w = 0;
        for (auto& c : d.choices) w = std::max(w, text_width(c));
        int bw = w + 44, bh = n * 20 + 14, bx = DX0 + DW - bw, by = DY0 - bh - 8;
        panel(bx, by, bw, bh);
        for (int i = 0; i < n; ++i) {
            int y = by + 8 + i * 20;
            if (i == d.choice) { cursor(bx + 12, y + 4); }
            text(bx + 26, y + 2, d.choices[i], TXT);
        }
    }
}

void render_toast() {
    if (g.toast_t <= 0) return;
    int w = text_width(g.toast) + 28;
    panel((UW - w) / 2, 10, w, 26);
    text((UW - w) / 2 + 14, 17, g.toast, TXT);
}

const Sprite* const MENU_ICON[] = {&art::icon_yokai, &art::icon_book, &art::icon_pouch, &art::icon_taeguk,
                                   &art::icon_brush, &art::icon_knot, &art::icon_close};

void header(int x, int y, int icon, const std::string& s) {
    sprite(*MENU_ICON[icon], x, y + 2);
    text(x + 15, y, s, TXT_RED);
    rect(x, y + 17, 150, 1, UI_LINE_D);
}

void render_menu() {
    const int bx = 508, by = 10, bw = 122, rh = 20;
    panel(bx, by, bw, kMenuN * rh + 16);
    for (int i = 0; i < kMenuN; ++i) {
        int y = by + 9 + i * rh;
        if (i == g.menu_sel) { cursor(bx + 10, y + 3); }
        sprite(*MENU_ICON[i], bx + 20, y + 1);
        text(bx + 36, y, MENU[i], TXT);
    }
    if (g.panel < 0) return;
    panel(10, 10, g.panel == 1 || g.panel == 2 ? 490 : 344, 180);
    int x = 26, y = 24;
    switch (g.panel) {
        case 0:
            header(x, y, 0, "동행 요괴");
            for (int i = 0; i < 3; ++i) {
                int sx = x + i * 104;
                rrect(sx, y + 30, 98, 64, UI_LINE_D);
                rrect(sx + 1, y + 31, 96, 62, UI_EDGE);
                text(sx + 49 - text_width("빈 자리", Font::Small) / 2, y + 56, "빈 자리", TXT_DIM, -1, Font::Small);
            }
            text(x, y + 110, "약해진 요괴에게 계약 부적을 쓰면", TXT_DIM, -1, Font::Small);
            text(x, y + 124, "약조를 맺을 수 있다.", TXT_DIM, -1, Font::Small);
            break;
        case 1:
            header(x, y, 1, "요괴 도감");
            text(x + 330, y, "기록 0 / 108", TXT);
            for (int i = 0; i < 108; ++i) {
                int cx = x + (i % 27) * 17, cy = y + 30 + (i / 27) * 17;
                rrect(cx, cy, 15, 15, UI_LINE_D);
                rrect(cx + 1, cy + 1, 13, 13, UI_EDGE);
            }
            text(x, y + 104, "만난 요괴는 이곳에 그림과 함께 적힌다.", TXT_DIM, -1, Font::Small);
            break;
        case 2: {
            header(x, y, 2, "가방");
            int row = 0;
            for (int i = 0; i < ITEM_COUNT; ++i)
                if (g.items[i] > 0) {
                    int ry = y + 28 + row * 20;
                    sprite(i == CONTRACT ? art::icon_talisman : art::icon_pouch, x, ry + 2);
                    text(x + 16, ry, ITEMS[i].name, TXT);
                    text(x + 110, ry, "x" + std::to_string(g.items[i]), TXT);
                    text(x + 150, ry + 2, ITEMS[i].desc, TXT_DIM, -1, Font::Small);
                    ++row;
                }
            if (!row) text(x, y + 28, "비어 있다.", TXT_DIM);
            text(x, y + 132, "엽전 " + std::to_string(g.money) + "냥", TXT);
            break;
        }
        case 3:
            header(x, y, 3, "음양사 · 벽사청");
            text(x + 270, y, "Lv." + std::to_string(g.level), TXT);
            text(x, y + 30, "체력", TXT, -1, Font::Small);
            bar(x + 34, y + 32, 160, g.hp, g.hp_max, UI_HP);
            text(x + 202, y + 30, std::to_string(g.hp) + "/" + std::to_string(g.hp_max), TXT, -1, Font::Small);
            text(x, y + 48, "내공", TXT, -1, Font::Small);
            bar(x + 34, y + 50, 160, g.ng, g.ng_max, UI_DIM);
            text(x + 202, y + 48, std::to_string(g.ng) + "/" + std::to_string(g.ng_max), TXT, -1, Font::Small);
            text(x, y + 76, clock_str(), TXT);
            text(x, y + 98, "엽전 " + std::to_string(g.money) + "냥", TXT);
            text(x, y + 128, g.quest == 0 ? "임무 · 마을 어귀의 요괴 퇴치" : "임무 · 도선사 고개를 넘어라", TXT_RED);
            sprite(art::npc_hero_portrait, 346 - art::npc_hero_portrait.w, 186 - art::npc_hero_portrait.h);
            break;
        case 4: header(x, y, 4, "기록"); text(x, y + 30, "저장 기능은 다음 단계에서 붙는다.", TXT_DIM); break;  // TODO(save)
        case 5:
            header(x, y, 5, "설정");
            text(x, y + 30, "글자 속도 · 보통", TXT);
            text(x, y + 50, "화면 · 정수배 확대 (F11 전체화면)", TXT);
            break;
    }
}

void render_shop() {
    const Sprite* icons[] = {&art::icon_pouch, &art::icon_talisman, &art::icon_close};
    panel(16, 16, 268, 3 * 20 + 18);
    for (int i = 0; i < 3; ++i) {
        int y = 25 + i * 20;
        if (i == g.shop_sel) { cursor(28, y + 3); }
        sprite(*icons[i], 40, y + 1);
        if (i < 2) {
            text(56, y, ITEMS[SHOP_ITEMS[i]].name, TXT);
            std::string p = std::to_string(ITEMS[SHOP_ITEMS[i]].price) + "냥";
            text(266 - text_width(p), y, p, TXT);
        } else {
            text(56, y, "나간다", TXT);
        }
    }
    std::string m = "엽전 " + std::to_string(g.money) + "냥";
    int w = text_width(m) + 28;
    panel(UW - 16 - w, 16, w, 26);
    text(UW - 16 - w + 14, 23, m, TXT);
    panel(DX0, DY0, DW, DH);
    text(DX0 + 18, DY0 + 14, g.shop_sel < 2 ? ITEMS[SHOP_ITEMS[g.shop_sel]].desc : "가게를 나선다.", TXT);
    if (g.shop_sel < 2) text(DX0 + 18, DY0 + 32, "가진 수 " + std::to_string(g.items[SHOP_ITEMS[g.shop_sel]]), TXT_DIM, -1, Font::Small);
}

// Pokemon-style encounter: flashes, then black bars sweep in from alternating sides.
void render_encounter() {
    int t = kEnc - g.enc_t;
    if (t < 24) {
        if ((t / 4) % 2 == 0) tint(1.9f, 1.9f, 1.9f);
        return;
    }
    float p = (t - 24) / 24.0f;
    for (int band = 0; band < 12; ++band) {
        int w = (int)(UW * p), y = band * 30;
        rect(band % 2 ? UW - w : 0, y, w, 30, 0x08060a);
    }
}

// ---------------------------------------------------------------- rendering: scenes
void title_world() {
    const Color* N = art::RAMP_NIGHT;
    clear(N[3]);
    for (int k = 0; k < 40; ++k) { unsigned h = hash3(k, 5, 5); pset(h % W, (h >> 9) % 90, N[1]); }
    disc(262, 40, 13, N[1]);
    disc(259, 38, 11, N[0]);
    for (int layer = 0; layer < 2; ++layer)
        for (int x = 0; x < W; ++x) {
            int h = (int)(108 + layer * 22 + 16 * std::sin(x * (0.018f + layer * 0.01f) + layer * 2) + 7 * std::sin(x * 0.05f + layer));
            rect(x, h, 1, H - h, layer ? N[3] : N[2]);
            if (layer) pset(x, h, N[2]);
        }
}

void title_ui() {
    rrect(150, 72, 42, 42, 0x7a1c1c);
    frame_rect(154, 76, 34, 34, UI_TEXT);
    text(171 - text_width("108") / 2, 86, "108", UI_TEXT);
    text(204, 66, "음양견문록", UI_TEXT, -1, Font::Body, 3);
    text(206, 106, "陰陽見聞錄 · 조선 괴담 견문록", UI_HP, -1, Font::Small);
    const char* items[] = {"새로 시작", "이어하기"};
    for (int i = 0; i < 2; ++i) {
        int y = 252 + i * 22, x = (UW - text_width(items[i])) / 2;
        text(x, y, items[i], i == 1 ? UI_LINE : UI_TEXT);
        if (i == g.title_sel) sprite(art::ui_cursor, x - 14 + (int)((g.frame / 12) % 2), y + 3);
    }
}

void prologue_world() {
    clear(0x040605);
    int flick = (g.frame / 7) % 3;
    disc(176, 100, 70 + flick, 0x0a120c);
    disc(176, 100, 44 + flick, 0x12201a);
    rect(0, 136, W, 1, 0x1c3426);
    sprite((g.frame / 9) % 2 ? art::lamp_a : art::lamp_b, 190, 124);
}

void prologue_ui() {
    const Sprite& p = art::npc_old_monk_portrait_big;
    sprite(p, 290 - p.w / 2, 272 - p.h);
}

void card_ui() {
    std::string a = "한양 외곽 · 주막", b = clock_str();
    text((UW - text_width(a, Font::Body, 2)) / 2, 150, a, UI_TEXT, -1, Font::Body, 2);
    text((UW - text_width(b)) / 2, 190, b, UI_DIM);
}

void battle_world() {
    Battle& b = g.bt;
    clear(0x0a120c);
    float k = std::max(0.f, 1 - b.intro / 30.f);
    int slide = (int)(220 * k * k);
    auto platform = [](int cx, int cy, int rx) {
        for (int j = -5; j <= 5; ++j) {
            int w = (int)(rx * std::sqrt(1 - j * j / 25.f));
            rect(cx - w, cy + j, w * 2, 1, GR[1]);
        }
        for (int i = -rx; i < rx; i += 2) pset(cx + i, cy + 5, GR[2]);
    };
    platform(236 - slide, 80, 40);
    platform(84 + slide, 134, 46);
}

// Battle sprites sit on the UI layer at integer scale; the phase tint is applied after them.
void battle_sprites() {
    Battle& b = g.bt;
    float k = std::max(0.f, 1 - b.intro / 30.f);
    int slide = 2 * (int)(220 * k * k);
    const Sprite& e = art::battle_jangseung;
    int ex = 472 - e.w / 2 - slide + (b.shake > 0 ? ((b.shake / 2) % 2 ? 4 : -4) : 0);
    if (!(b.flash < 0 && (b.flash / 2) % 2) && b.dehp > 0) {
        sprite(e, ex, 168 - e.h);
        for (int i = 0; i < 12; ++i) {
            unsigned h = hash3(i, 1, 1);
            rect(ex - 8 + h % (e.w + 16), 160 - (int)((g.frame * 2 + i * 23) % 120), 2, 2, i % 3 ? UI_GLOW : UI_BLOOD);
        }
    }
    const Sprite& hb = art::battle_hero_back;
    int hx = 168 - hb.w / 2 + slide + (b.shake < 0 ? 6 : 0);
    if (!(b.flash > 0 && (b.flash / 2) % 2)) sprite(hb, hx, 276 - hb.h);
    Phase ph = phase_of(g.clock.minute);
    if (ph == Phase::Dusk) tint(1.0f, 0.8f, 0.66f);
    if (ph == Phase::Night) tint(0.5f, 0.55f, 0.82f);
    if (ph == Phase::Dawn) tint(0.88f, 0.86f, 0.98f);
}

void battle_ui() {
    Battle& b = g.bt;
    battle_sprites();
    if (b.intro >= 30) {
        panel(24, 20, 222, 50);
        text(40, 30, "장승귀", TXT);
        text(176, 32, "Lv.3", TXT, -1, Font::Small);
        rrect(212, 28, 18, 18, UI_LINE_D);
        text(215, 31, "木", TXT_LIGHT);
        text(40, 50, "체력", TXT, -1, Font::Small);
        bar(72, 52, 158, b.dehp, b.emax, b.dehp * 4 > b.emax ? UI_HP : UI_BLOOD);

        panel(392, 198, 230, 66);
        text(408, 208, "음양사", TXT);
        text(572, 210, "Lv." + std::to_string(g.level), TXT, -1, Font::Small);
        text(408, 228, "체력", TXT, -1, Font::Small);
        bar(440, 230, 124, b.dhp, g.hp_max, b.dhp * 4 > g.hp_max ? UI_HP : UI_BLOOD);
        text(570, 228, std::to_string(b.dhp) + "/" + std::to_string(g.hp_max), TXT, -1, Font::Small);
        text(408, 244, "내공", TXT, -1, Font::Small);
        bar(440, 246, 124, b.dng, g.ng_max, UI_DIM);
        text(570, 244, std::to_string(b.dng) + "/" + std::to_string(g.ng_max), TXT, -1, Font::Small);
    }
    const int by = 276, bh = 72;
    if (b.st == Battle::Msgs && !b.msgs.empty()) {
        panel(DX0, by, DW, bh);
        text(DX0 + 18, by + 16, b.msgs.front().text, TXT, (int)b.shown);
        if (b.shown >= cp_count(b.msgs.front().text)) more_mark(DX0 + DW - 22, by + bh - 17);
    } else if (b.st == Battle::List) {
        auto e = battle_entries(b.list);
        panel(DX0, by, DW, bh);
        for (int i = 0; i < (int)e.size() && i < 3; ++i) {
            int y = by + 12 + i * 18;
            if (i == b.sel) { cursor(DX0 + 14, y + 4); }
            text(DX0 + 28, y + 1, e[i].name, e[i].ok ? TXT : TXT_DIM);
        }
        rect(DX0 + 310, by + 12, 1, bh - 24, UI_LINE_D);
        if (!e.empty()) {
            auto lines = wrap(e[b.sel].info, 270, Font::Small);
            for (int i = 0; i < (int)lines.size() && i < 4; ++i) text(DX0 + 322, by + 14 + i * 13, lines[i], TXT, -1, Font::Small);
        }
    } else if (b.intro >= 30) {
        panel(DX0, by, 372, bh);
        text(DX0 + 18, by + 16, "무엇을 할까?", TXT);
        panel(392, by, 232, bh);
        const char* cmds[] = {"기술", "도술", "도구", "도주"};
        const Sprite* ic[] = {&art::icon_staff, &art::icon_talisman, &art::icon_pouch, &art::icon_feet};
        for (int i = 0; i < 4; ++i) {
            int bx = 402 + (i % 2) * 108, y = by + 11 + (i / 2) * 26;
            bool sel = b.st == Battle::Cmd && i == b.cmd;
            rrect(bx, y, 104, 24, sel ? UI_GLOW : UI_LINE_D);
            rrect(bx + 1, y + 1, 102, 22, sel ? UI_LINE_D : UI_BG);
            sprite(*ic[i], bx + 12, y + 8);
            text(bx + 28, y + 6, cmds[i], TXT);
        }
    }
}

}  // namespace

// ---------------------------------------------------------------- public API
void game_init(unsigned seed) {
    g = Game{};
    g.rng.seed(seed);
    build_maps();
}

void game_update(const Input& in) {
    g.frame++;
    if (g.toast_t > 0) g.toast_t--;
    if (g.trans_t >= 0) {
        if (++g.trans_t == kTrans && g.trans_mid) { auto f = g.trans_mid; g.trans_mid = {}; f(); }
        if (g.trans_t >= 2 * kTrans) g.trans_t = -1;
        return;
    }
    if (g.enc_t > 0) {
        if (--g.enc_t == 0) begin_battle();
        return;
    }
    if (!g.dq.empty()) return update_dialog(in);
    switch (g.scene) {
        case Scene::Title:
            if (in.pressed[K_UP] || in.pressed[K_DOWN]) g.title_sel ^= 1;
            if ((in.pressed[K_A] || in.pressed[K_START]) && g.title_sel == 0) transition(start_prologue);
            break;
        case Scene::Prologue: break;
        case Scene::PlaceCard:
            if (++g.card_t == 150 || (g.card_t > 20 && (in.pressed[K_A] || in.pressed[K_B] || in.pressed[K_START]))) after_place_card();
            break;
        case Scene::Field:
            if (g.shop) return update_shop(in);
            if (g.menu) return update_menu(in);
            update_field(in);
            break;
        case Scene::Battle: update_battle(in); break;
    }
}

void game_render() {
    target_world();
    switch (g.scene) {
        case Scene::Title: title_world(); break;
        case Scene::Prologue: prologue_world(); break;
        case Scene::PlaceCard: clear(0x08060a); break;
        case Scene::Field: render_field(); break;
        case Scene::Battle: battle_world(); break;
    }
    compose();
    target_ui();
    switch (g.scene) {
        case Scene::Title: title_ui(); break;
        case Scene::Prologue: prologue_ui(); break;
        case Scene::PlaceCard: card_ui(); break;
        case Scene::Field:
            render_actors();
            if (g.menu) render_menu();
            if (g.shop) render_shop();
            break;
        case Scene::Battle: battle_ui(); break;
    }
    render_dialog();
    render_toast();
    if (g.enc_t > 0) render_encounter();
    if (g.trans_t >= 0) fade(g.trans_t < kTrans ? g.trans_t / (float)kTrans : (2 * kTrans - g.trans_t) / (float)kTrans);
}

bool game_debug_scene(const std::string& name) {
    auto finish_text = [] { for (auto& d : g.dq) d.shown = 999; };
    auto village = [](int hour) {
        wake_up();
        g.map = VILLAGE; g.px = 13; g.py = 9; g.dir = DOWN;
        g.clock.minute = hour * 60.f;
        g.last_phase = phase_of(g.clock.minute);
    };
    if (name == "title") return true;
    if (name == "prologue") { start_prologue(); g.dq.front().page = 2; finish_text(); return true; }
    if (name == "card") { g.scene = Scene::PlaceCard; return true; }
    if (name == "encounter") { village(12); g.px = 13; g.py = 2; g.dir = UP; g.enc_t = 20; return true; }
    if (name == "choice") { start_prologue(); g.dq.front().page = 3; finish_text(); return true; }
    if (name == "wake") { wake_up(); say({"(벽사청의 명을 받아 도선사로 향하는 길이다. 오늘은 채비를 갖추고 길을 나서야 한다.)"}, "음양사"); finish_text(); return true; }
    if (name == "village") { village(12); return true; }
    if (name == "yard") { village(10); g.px = 7; g.py = 10; g.dir = LEFT; return true; }
    if (name == "gate") { village(12); g.px = 13; g.py = 3; g.dir = UP; return true; }
    if (name == "dusk") { village(18); return true; }
    if (name == "night") { village(22); g.px = 18; g.py = 10; g.dir = RIGHT; return true; }
    if (name == "talk") { village(10); g.px = 5; g.py = 12; g.dir = UP; talk_npc(maps[VILLAGE].npcs[0]); finish_text(); return true; }
    if (name == "rest") { wake_up(); g.dir = UP; g.px = 1; g.py = 2; search(1, 1); finish_text(); return true; }
    if (name == "menu") { village(12); g.menu = true; g.menu_sel = 3; g.panel = 3; return true; }
    if (name == "shop") { village(12); g.px = 20; g.py = 7; g.dir = UP; g.shop = true; return true; }
    if (name == "battle" || name == "battle_list" || name == "battle_msg") {
        village(12);
        g.scene = Scene::Battle;
        g.bt = Battle{};
        g.bt.dhp = g.hp; g.bt.dng = g.ng;
        g.bt.intro = 40;
        g.bt.st = Battle::Cmd;
        if (name == "battle_list") { g.bt.st = Battle::List; g.bt.list = 1; g.bt.cmd = 1; }
        if (name == "battle_msg") { g.bt.st = Battle::Msgs; g.bt.ehp = g.bt.dehp = 22; bmsg("금(金)이 목(木)을 누른다! 효과가 굉장하다!"); g.bt.shown = 999; g.bt.fx_done = true; }
        return true;
    }
    return false;
}

DebugInfo game_debug_info() {
    return {(int)g.scene, g.map, g.px, g.py, g.quest, !g.dq.empty() || g.trans_t >= 0 || g.menu || g.shop || g.moving};
}
