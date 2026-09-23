#include "ui.h"

namespace yy {

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

}  // namespace yy
