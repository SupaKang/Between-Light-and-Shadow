#include "ui.h"

namespace yy {

// ---------------------------------------------------------------- battle
void bmsg(std::string t, std::function<void()> fx) { g.bt.msgs.push_back({std::move(t), std::move(fx)}); }

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
    for (const char* id : {"cheongsimhwan", "contract_talisman"})
        if (have(id) > 0) {
            const data::ItemDef* d = data::find_item(id);
            e.push_back({std::string(d->name_ko) + " x" + std::to_string(have(id)), d->desc, true});
        }
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
        std::vector<std::string> items;
        for (const char* id : {"cheongsimhwan", "contract_talisman"}) if (have(id) > 0) items.push_back(id);
        std::string it = items[sel];
        if (it == "contract_talisman") {
            bmsg("요기에 물든 장승이다. 약조를 맺을 수 없다!");
            b.after = [] { g.bt.st = Battle::Cmd; };
            return;
        }
        g.items["cheongsimhwan"]--;
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

}  // namespace yy
