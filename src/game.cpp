#include "ui.h"

using namespace yy;

// ---------------------------------------------------------------- public API
void game_init(unsigned seed) {
    g = Game{};
    g.rng.seed(seed);
    script_init();
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
        {
            Input t = in;  // Enter confirms too
            t.pressed[K_A] = in.pressed[K_A] || in.pressed[K_START];
            int c;
            if (g.title_menu.update(t, c) && c == 0) transition([] { g.scene = Scene::Prologue; script_start("prologue"); });
            break;
        }
        case Scene::Prologue: break;
        case Scene::PlaceCard:
            if (++g.card_t == 150 || (g.card_t > 20 && (in.pressed[K_A] || in.pressed[K_B] || in.pressed[K_START]))) transition([] { wake_up(); script_start("wake"); });
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
        load_map("village"); g.px = 13; g.py = 9; g.dir = DOWN;
        g.clock.minute = hour * 60.f;
        g.last_phase = phase_of(g.clock.minute);
    };
    if (name == "title") return true;
    if (name == "prologue") { g.scene = Scene::Prologue; script_start("prologue"); g.dq.front().page = 2; finish_text(); return true; }
    if (name == "card") { g.scene = Scene::PlaceCard; return true; }
    if (name == "encounter") { village(12); g.px = 13; g.py = 2; g.dir = UP; g.enc_t = 20; return true; }
    if (name == "choice") { g.scene = Scene::Prologue; script_start("prologue"); g.dq.front().page = 3; finish_text(); return true; }
    if (name == "wake") { wake_up(); say({"(벽사청의 명을 받아 도선사로 향하는 길이다. 오늘은 채비를 갖추고 길을 나서야 한다.)"}, "음양사"); finish_text(); return true; }
    if (name == "village") { village(12); return true; }
    if (name == "yard") { village(10); g.px = 7; g.py = 10; g.dir = LEFT; return true; }
    if (name == "gate") { village(12); g.px = 13; g.py = 3; g.dir = UP; return true; }
    if (name == "dusk") { village(18); return true; }
    if (name == "night") { village(22); g.px = 18; g.py = 10; g.dir = RIGHT; return true; }
    if (name == "talk") { village(10); g.px = 5; g.py = 12; g.dir = UP; g.npcs[0].face = DOWN; run_event("talk_jumo"); finish_text(); return true; }
    if (name == "rest") { wake_up(); g.dir = UP; g.px = 1; g.py = 2; run_event("search", 1, 1); finish_text(); return true; }
    if (name == "menu") { village(12); open_menu(); g.menu_list.sel = 3; g.panel = 3; return true; }
    if (name == "bag") {
        village(12); open_menu(); g.menu_list.sel = 2; g.panel = 2;
        g.items = {{"cheongsimhwan", 3}, {"contract_talisman", 5}, {"hanji", 2}};
        return true;
    }
    if (name == "shop_buy") {
        village(12); g.px = 20; g.py = 7; g.dir = UP; g.money = 90;
        open_shop({"cheongsimhwan", "contract_talisman"}); g.shop_list.sel = 1; g.qty = NumberPicker{2, 1, 2}; g.buying = true;
        return true;
    }
    if (name == "shop") { village(12); g.px = 20; g.py = 7; g.dir = UP; open_shop({"cheongsimhwan", "contract_talisman"}); return true; }
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
    return {(int)g.scene, g.map_id, g.px, g.py, flag("quest_gate"),
            !g.dq.empty() || g.trans_t >= 0 || g.menu || g.shop || g.moving || script_busy()};
}
