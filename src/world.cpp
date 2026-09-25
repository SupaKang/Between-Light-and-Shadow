#include "ui.h"

namespace yy {
void load_map(const std::string& id) {
    g.map_id = id;
    g.mdef = find_map(id);
    g.npcs.clear();
    for (int i = 0; g.mdef && i < g.mdef->npc_n; ++i) {
        const NpcDef& n = g.mdef->npcs[i];
        g.npcs.push_back({n.x, n.y, dir_of(n.face), n.name, n.sprite, n.talk_event});
    }
}

char tile_at(int x, int y) {
    const MapDef* m = g.mdef;
    if (!m || y < 0 || y >= m->h || x < 0 || x >= m->w) return '~';
    return m->tiles[y * m->w + x];
}

Npc* npc_named(const std::string& name) {
    for (auto& n : g.npcs) if (n.name == name) return &n;
    return nullptr;
}

bool solid_tile(char c) { return std::string("#BRJSThHWGe~").find(c) != std::string::npos; }

// ---------------------------------------------------------------- story beats
void wake_up() {
    g.scene = Scene::Field;
    load_map("tavern_room");
    g.px = 1; g.py = 2; g.dir = DOWN;
    g.hp = g.hp_max; g.ng = g.ng_max;
    g.last_phase = phase_of(g.clock.minute);
}

// ---------------------------------------------------------------- field update
void on_arrive() {
    const MapDef* m = g.mdef;
    for (int i = 0; i < m->warp_n; ++i) {
        const WarpDef& w = m->warps[i];
        if (w.x != g.px || w.y != g.py) continue;
        if (!warp_valid(w.to_map, w.tx, w.ty)) {  // baked data is checked at build time; stay put if it is still wrong
            std::fprintf(stderr, "bad warp %s(%d,%d) -> %s(%d,%d)\n", g.map_id.c_str(), w.x, w.y, w.to_map, w.tx, w.ty);
            return;
        }
        std::string to = w.to_map;
        int tx = w.tx, ty = w.ty;
        Dir face = dir_of(w.face);
        transition([to, tx, ty, face] { load_map(to); g.px = tx; g.py = ty; g.dir = face; });
        return;
    }
    for (int i = 0; i < m->trig_n; ++i) {
        const TriggerDef& t = m->triggers[i];
        if (t.x != g.px || t.y != g.py) continue;
        std::string key = "trig:" + g.map_id + ":" + std::to_string(t.x) + ":" + std::to_string(t.y);
        if (t.once && g.flags[key]) continue;
        if (t.once) g.flags[key] = 1;
        run_event(t.event, t.x, t.y);
        return;
    }
}

bool npc_at(int x, int y) {
    for (auto& n : g.npcs) if (n.x == x && n.y == y) return true;
    return false;
}

// Walking into a wall or off the map runs "<map>_bump"(x, y) if the map's script defines it.
void bump_edge(int nx, int ny) { script_start(g.map_id + "_bump", nx, ny); }

void update_field(const Input& in) {
    if (script_busy()) return;  // an event owns the player until it ends
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
    if (in.pressed[K_START]) { open_menu(); return; }
    if (in.pressed[K_A]) {
        int tx = g.px + DX[g.dir], ty = g.py + DY[g.dir];
        for (auto& n : g.npcs)
            if (n.x == tx && n.y == ty) {
                n.face = (Dir)(g.dir == DOWN ? UP : g.dir == UP ? DOWN : g.dir == LEFT ? RIGHT : LEFT);
                return run_event(n.talk_event);
            }
        return run_event("search", tx, ty);
    }
    int d = in.held[K_UP] ? UP : in.held[K_DOWN] ? DOWN : in.held[K_LEFT] ? LEFT : in.held[K_RIGHT] ? RIGHT : -1;
    if (d < 0) { g.chain = false; g.turn_wait = 0; return; }
    if (d != g.dir && !g.chain) { g.dir = (Dir)d; g.turn_wait = 5; return; }  // tap = turn in place
    g.dir = (Dir)d;
    if (g.turn_wait > 0) { g.turn_wait--; return; }
    int nx = g.px + DX[d], ny = g.py + DY[d];
    if (solid_tile(tile_at(nx, ny)) || npc_at(nx, ny)) {
        const Key key_of[] = {K_DOWN, K_UP, K_LEFT, K_RIGHT};  // Dir order differs from Key order
        if (in.pressed[key_of[d]]) bump_edge(nx, ny);
        g.chain = false;
        return;
    }
    g.moving = true;
}

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
    auto grassy = [&](int x, int y) { char c = tile_at(x, y); return c == ',' || c == 'T' || c == 'S'; };
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
    char c = tile_at(tx, ty);
    bool indoor = g.mdef->indoor;
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
            if (floorish(tile_at(tx, ty + 1))) {
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
            bool top = tile_at(tx, ty - 1) != 'h', bot = tile_at(tx, ty + 1) != 'h';
            bool lft = tile_at(tx - 1, ty) != 'h', rgt = tile_at(tx + 1, ty) != 'h';
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
            if (tile_at(tx - 1, ty) != 'H' || tx % 2 == 0) rect(sx, sy, 2, 13, EA[2]);
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
    int mw = g.mdef->w * 16, mh = g.mdef->h * 16;
    int wx = g.px * 16 + DX[g.dir] * g.step, wy = g.py * 16 + DY[g.dir] * g.step;
    cx = mw <= W ? (mw - W) / 2 : std::clamp(wx + 8 - W / 2, 0, mw - W);
    cy = mh <= H ? (mh - H) / 2 : std::clamp(wy + 8 - H / 2, 0, mh - H);
}

void phase_tint(float& r, float& gg, float& b) {
    Phase ph = phase_of(g.clock.minute);
    r = gg = b = 1;
    if (g.mdef->indoor) {
        if (ph == Phase::Night) r = 0.8f, gg = 0.7f, b = 0.62f;
    } else {
        if (ph == Phase::Dawn) r = 0.86f, gg = 0.84f, b = 0.98f;
        if (ph == Phase::Dusk) r = 1.0f, gg = 0.78f, b = 0.62f;
        if (ph == Phase::Night) r = 0.42f, gg = 0.48f, b = 0.78f;
    }
}

constexpr int kLampIn = 22, kLampOut = 34;  // world pixels
bool lantern_lit() { return !g.mdef->indoor && phase_of(g.clock.minute) == Phase::Night; }

// World layer (320x180): tiles only.
void render_field() {
    int cx, cy;
    field_camera(cx, cy);
    clear(0x101010);
    for (int ty = cy / 16 - 1; ty <= (cy + H) / 16 + 1; ++ty)
        for (int tx = cx / 16 - 1; tx <= (cx + W) / 16 + 1; ++tx)
            if (tile_at(tx, ty) != '~') draw_tile(tx, ty, tx * 16 - cx, ty * 16 - cy);
    float r, gg, b;
    phase_tint(r, gg, b);
    if (lantern_lit()) {  // night outdoors: the hero's ghost-fire lantern keeps a small pool of light
        int hx = g.px * 16 + DX[g.dir] * g.step - cx + 8, hy = g.py * 16 + DY[g.dir] * g.step - cy + 8;
        tint_lit(hx, hy, kLampIn, kLampOut, r, gg, b);
    } else if (r != 1 || gg != 1 || b != 1) {
        tint(r, gg, b);
    }
}

// Field sprite by name (from map data). nullptr = no art yet; callers draw placeholder().
const Sprite* field_sprite(const std::string& name, Dir d) {
    struct Set { const char* name; const Sprite *down, *side, *up; };
    static const Set sets[] = {
        {"jumo", &art::npc_jumo_field_down, &art::npc_jumo_field_side, &art::npc_jumo_field_up},
        {"village_elder", &art::npc_village_elder_field_down, &art::npc_village_elder_field_side, &art::npc_village_elder_field_up},
    };
    for (const Set& s : sets)
        if (name == s.name) return d == UP ? s.up : d == DOWN ? s.down : s.side;
    return nullptr;
}

// Stand-in for art that does not exist yet: a labelled silhouette so content stays playable.
void placeholder(const std::string& label, int x, int y, int w, int h) {
    rrect(x, y, w, h, UI_EDGE);
    rrect(x + 1, y + 1, w - 2, h - 2, UI_LINE_D);
    text(x + w / 2 - text_width(label, Font::Small) / 2, y + h / 2 - 6, label, UI_DIM, -1, Font::Small);
}

// UI layer (640x360): 32x48 characters at twice the world resolution, Y-sorted by feet.
void render_actors() {
    const MapDef& m = *g.mdef;
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
    for (int ty = 0; ty < m.h; ++ty)
        for (int tx = 0; tx < m.w; ++tx)
            if (m.tiles[ty * m.w + tx] == 'G') {
                int sx = ux(tx * 16), sy = uy(ty * 16);
                ds.push_back({ty * 16 + 15, [=] {
                    lit(tx * 16, ty * 16);
                    sprite(art::jangseung, sx, sy - 32, false, 2);
                    if (!flag("quest_gate") && (g.frame / 20) % 2) { rect(sx + 10, sy - 20, 2, 2, UI_GLOW); rect(sx + 20, sy - 20, 2, 2, UI_GLOW); }
                }});
            }
    for (auto& n : g.npcs) {
        int sx = ux(n.x * 16), sy = uy(n.y * 16);
        const Npc* np = &n;
        ds.push_back({n.y * 16 + 15, [=] {
            lit(np->x * 16, np->y * 16);
            if (const Sprite* s = field_sprite(np->sprite, np->face)) sprite(*s, sx, sy - 18, np->face == LEFT);
            else placeholder(np->name, sx + 4, sy - 16, 24, 32);
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

}  // namespace yy
