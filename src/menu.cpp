#include "ui.h"
#include "save.h"

namespace yy {

const Sprite* const MENU_ICON[] = {&art::icon_yokai, &art::icon_book, &art::icon_pouch, &art::icon_taeguk,
                                   &art::icon_brush, &art::icon_knot, &art::icon_close};

// ponytail: icon picked by id until items carry their own icon
static const Sprite* item_icon(const std::string& id) {
    return id.find("talisman") != std::string::npos ? &art::icon_talisman : &art::icon_pouch;
}

void open_menu() {
    g.menu = true;
    g.panel = -1;
    g.menu_list = ListMenu{{"요괴", "도감", "가방", "음양사", "기록", "설정", "닫기"}, {}, 0, 0, 7,
                           {std::begin(MENU_ICON), std::end(MENU_ICON)}};
}

static std::vector<const data::ItemDef*> bag_items() {  // data order keeps the bag stable
    std::vector<const data::ItemDef*> v;
    for (int i = 0; i < data::item_count(); ++i)
        if (have(data::item_at(i).id) > 0) v.push_back(&data::item_at(i));
    return v;
}

static void fill_bag() {
    ListMenu& b = g.bag_list;
    b.items.clear();
    b.icons.clear();
    for (auto* it : bag_items()) { b.items.push_back(it->name_ko); b.icons.push_back(item_icon(it->id)); }
}

// Slot rows: "1. 주막 마을 · 2일째 · 0:37" / "비어 있음" / "손상됨". Loading enables good slots; saving enables all when allowed.
void fill_slots(bool for_load) {
    ListMenu& l = g.slot_list;
    l.items.clear();
    l.enabled.clear();
    for (int i = 0; i < kSaveSlots; ++i) {
        SaveInfo s = save_info(i);
        char buf[160];
        if (!s.exists) snprintf(buf, sizeof buf, "%d. 비어 있음", i + 1);
        else if (s.corrupt) snprintf(buf, sizeof buf, "%d. 손상됨", i + 1);
        else snprintf(buf, sizeof buf, "%d. %s · %d일째 · %d:%02d", i + 1, s.map_name.c_str(), s.day, s.playtime_min / 60, s.playtime_min % 60);
        l.items.push_back(buf);
        l.enabled.push_back(for_load ? s.exists && !s.corrupt : save_allowed());
    }
    l.sel = 0;
    while (l.sel < kSaveSlots - 1 && !l.enabled[l.sel]) l.sel++;
}

static void write_slot(int slot) {
    show_toast(save_game(slot) ? "기록했다" : "기록하지 못했다");
    int sel = g.slot_list.sel;
    fill_slots(false);
    g.slot_list.sel = sel;
}

void update_menu(const Input& in) {
    int c;
    if (g.panel == 4) {
        if (!g.slot_list.update(in, c)) return;
        if (c < 0) { g.panel = -1; return; }
        if (!save_info(c).exists) return write_slot(c);
        say({"이 기록을 덮어쓸까?"}, "", [c](int k) { if (k == 0) write_slot(c); }, {"예", "아니오"});
        return;
    }
    if (g.panel == 2) {
        fill_bag();
        if (g.bag_list.update(in, c) && c < 0) g.panel = -1;
        return;
    }
    if (g.panel >= 0) {
        if (in.pressed[K_A] || in.pressed[K_B]) g.panel = -1;
        return;
    }
    if (in.pressed[K_START]) { g.menu = false; return; }
    if (!g.menu_list.update(in, c)) return;
    if (c < 0 || c == (int)g.menu_list.items.size() - 1) { g.menu = false; return; }
    g.panel = c;
    if (c == 2) g.bag_list.sel = g.bag_list.top = 0;
    if (c == 4) fill_slots(false);
}

void open_shop(const std::vector<std::string>& ids) {
    g.shop = true;
    g.buying = false;
    g.shop_items = ids;
    g.shop_list = ListMenu{};
    for (auto& id : ids) {
        g.shop_list.items.push_back(data::find_item(id)->name_ko);
        g.shop_list.icons.push_back(item_icon(id));
    }
    g.shop_list.items.push_back("나간다");
    g.shop_list.icons.push_back(&art::icon_close);
}

void update_shop(const Input& in) {
    int c;
    if (g.buying) {
        if (!g.qty.update(in, c)) return;
        g.buying = false;
        if (c < 0) return;
        const data::ItemDef* it = data::find_item(g.shop_items[g.shop_list.sel]);
        g.money -= it->price * c;
        g.items[it->id] += c;
        show_toast(std::string(it->name_ko) + " " + std::to_string(c) + "개를 샀다");
        return;
    }
    if (!g.shop_list.update(in, c)) return;
    if (c < 0 || c == (int)g.shop_items.size()) {
        g.shop = false;
        script_resume(0);  // the shop() call returns
        return;
    }
    const data::ItemDef* it = data::find_item(g.shop_items[c]);
    if (g.money < it->price) { show_toast("엽전이 모자란다"); return; }
    g.qty = NumberPicker{1, 1, std::min(99, it->price > 0 ? g.money / it->price : 99)};
    g.buying = true;
}

void header(int x, int y, int icon, const std::string& s) {
    sprite(*MENU_ICON[icon], x, y + 2);
    text(x + 15, y, s, TXT_RED);
    rect(x, y + 17, 150, 1, UI_LINE_D);
}

void render_menu() {
    const int bx = 508, by = 10, bw = 122;
    panel(bx, by, bw, (int)g.menu_list.items.size() * 20 + 16);
    g.menu_list.render(bx + 10, by + 9, bw - 20);
    if (g.panel < 0) return;
    panel(10, 10, g.panel == 1 || g.panel == 2 ? 490 : 344, g.panel == 2 ? 196 : 180);
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
            std::string m = "엽전 " + std::to_string(g.money) + "냥";
            text(x + 458 - text_width(m), y, m, TXT);
            auto its = bag_items();
            if (its.empty()) { text(x, y + 28, "비어 있다.", TXT_DIM); break; }
            fill_bag();
            std::vector<std::string> right;
            for (auto* it : its) right.push_back("x" + std::to_string(have(it->id)));
            g.bag_list.render(x, y + 28, 170, right);
            rect(x + 186, y + 28, 1, 136, UI_LINE_D);
            auto lines = wrap(its[std::min(g.bag_list.sel, (int)its.size() - 1)]->desc, 262, Font::Small);
            for (int i = 0; i < (int)lines.size() && i < 8; ++i) text(x + 198, y + 30 + i * 14, lines[i], TXT_DIM, -1, Font::Small);
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
            text(x, y + 128, !flag("quest_gate") ? "임무 · 마을 어귀의 요괴 퇴치" : "임무 · 도선사 고개를 넘어라", TXT_RED);
            sprite(art::npc_hero_portrait, 346 - art::npc_hero_portrait.w, 186 - art::npc_hero_portrait.h);
            break;
        case 4:
            header(x, y, 4, "기록");
            g.slot_list.render(x, y + 30, 300);
            if (!save_allowed()) text(x, y + 104, "지금은 기록할 수 없다.", TXT_DIM, -1, Font::Small);
            break;
        case 5:
            header(x, y, 5, "설정");
            text(x, y + 30, "글자 속도 · 보통", TXT);
            text(x, y + 50, "화면 · 정수배 확대 (F11 전체화면)", TXT);
            break;
    }
}

void render_shop() {
    const ListMenu& l = g.shop_list;
    int n = (int)l.items.size();
    std::vector<std::string> right;
    for (auto& id : g.shop_items) right.push_back(std::to_string(data::find_item(id)->price) + "냥");
    panel(16, 16, 268, std::min(n, l.rows) * 20 + 18);
    l.render(28, 25, 238, right);
    std::string m = "엽전 " + std::to_string(g.money) + "냥";
    int w = text_width(m) + 28;
    panel(UW - 16 - w, 16, w, 26);
    text(UW - 16 - w + 14, 23, m, TXT);
    panel(DX0, DY0, DW, DH);
    const data::ItemDef* it = l.sel < n - 1 ? data::find_item(g.shop_items[l.sel]) : nullptr;
    text(DX0 + 18, DY0 + 14, it ? it->desc : "가게를 나선다.", TXT);
    if (g.buying) {
        text(DX0 + 18, DY0 + 32, "몇 개 살까? · 합계 " + std::to_string(it->price * g.qty.value) + "냥", TXT_RED, -1, Font::Small);
        g.qty.render(290, 21 + (l.sel - l.top) * 20);
    } else if (it) {
        text(DX0 + 18, DY0 + 32, "가진 수 " + std::to_string(have(it->id)), TXT_DIM, -1, Font::Small);
    }
}

}  // namespace yy
