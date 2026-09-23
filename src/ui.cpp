#include "ui.h"

namespace yy {

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

}  // namespace yy
