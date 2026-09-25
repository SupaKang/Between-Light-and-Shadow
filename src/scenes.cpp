#include "ui.h"

namespace yy {

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
    int w = 0;
    for (auto& s : g.title_menu.items) w = std::max(w, text_width(s));
    g.title_menu.render((UW - w) / 2 - 16, 250, w + 16);
    if (g.title_pick) {
        panel(150, 238, 340, 78);  // covers the title menu while picking
        g.slot_list.render(166, 248, 308);
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

}  // namespace yy
