#include "gfx.h"
#include "font11_data.h"
#include "font9_data.h"
#include <algorithm>
#include <cmath>

namespace gfx {

Color out[UW * UH];
static Color world[W * H];
static Color* tgt = world;
static int tw = W, th = H;

void target_world() { tgt = world; tw = W; th = H; }
void target_ui() { tgt = out; tw = UW; th = UH; }

void compose() {
    for (int y = 0; y < UH; ++y)
        for (int x = 0; x < UW; ++x) out[y * UW + x] = world[(y / 2) * W + x / 2];
}

void clear(Color c) { std::fill(tgt, tgt + tw * th, c); }

void pset(int x, int y, Color c) {
    if (x >= 0 && y >= 0 && x < tw && y < th) tgt[y * tw + x] = c;
}

void rect(int x, int y, int w, int h, Color c) {
    int x0 = std::max(x, 0), y0 = std::max(y, 0), x1 = std::min(x + w, tw), y1 = std::min(y + h, th);
    for (int j = y0; j < y1; ++j)
        for (int i = x0; i < x1; ++i) tgt[j * tw + i] = c;
}

void rrect(int x, int y, int w, int h, Color c) {
    rect(x + 1, y, w - 2, h, c);
    rect(x, y + 1, w, h - 2, c);
}

void frame_rect(int x, int y, int w, int h, Color c) {
    rect(x, y, w, 1, c);
    rect(x, y + h - 1, w, 1, c);
    rect(x, y, 1, h, c);
    rect(x + w - 1, y, 1, h, c);
}

void disc(int cx, int cy, int r, Color c) {
    for (int j = -r; j <= r; ++j)
        for (int i = -r; i <= r; ++i)
            if (i * i + j * j <= r * r + r / 2) pset(cx + i, cy + j, c);
}

static Color mul(Color c, float r, float g, float b) {
    auto ch = [](Color v, int s, float k) { return (Color)std::min(255.f, ((v >> s) & 255) * k) << s; };
    return ch(c, 16, r) | ch(c, 8, g) | ch(c, 0, b);
}

void shade(int x, int y, int w, int h, float k) {
    int x0 = std::max(x, 0), y0 = std::max(y, 0), x1 = std::min(x + w, tw), y1 = std::min(y + h, th);
    for (int j = y0; j < y1; ++j)
        for (int i = x0; i < x1; ++i) tgt[j * tw + i] = mul(tgt[j * tw + i], k, k, k);
}

void tint(float r, float g, float b) {
    for (int i = 0; i < tw * th; ++i) tgt[i] = mul(tgt[i], r, g, b);
}

void tint_lit(int cx, int cy, int r_in, int r_out, float r, float g, float b) {
    for (int y = 0; y < th; ++y)
        for (int x = 0; x < tw; ++x) {
            int d2 = (x - cx) * (x - cx) + (y - cy) * (y - cy) * 5 / 4;
            if (d2 < r_in * r_in) { tgt[y * tw + x] = mul(tgt[y * tw + x], 1.0f, 1.05f, 0.95f); continue; }
            if (d2 < r_out * r_out && (x + y) % 2) continue;
            tgt[y * tw + x] = mul(tgt[y * tw + x], r, g, b);
        }
}

void fade(float a) {
    if (a > 0) tint(1 - a, 1 - a, 1 - a);
}

static float str = 1, stg = 1, stb = 1;
void sprite_tint(float r, float g, float b) { str = r; stg = g; stb = b; }

void sprite(const Sprite& s, int x, int y, bool flip, int scale, const Color* ramp) {
    const Color* r = ramp ? ramp : s.ramp;
    bool tinted = str != 1 || stg != 1 || stb != 1;
    for (int j = 0; j < s.h; ++j)
        for (int i = 0; i < s.w; ++i) {
            char k = s.rows[j][flip ? s.w - 1 - i : i];
            if (k == '.') continue;
            Color c = r[k <= '9' ? k - '0' : k - 'a' + 10];
            rect(x + i * scale, y + j * scale, scale, scale, tinted ? mul(c, str, stg, stb) : c);
        }
}

void panel(int x, int y, int w, int h) {
    rrect(x, y, w, h, UI_EDGE);
    rrect(x + 1, y + 1, w - 2, h - 2, UI_BG);
    frame_rect(x + 2, y + 2, w - 4, h - 4, UI_LINE_D);
    frame_rect(x + 4, y + 4, w - 8, h - 8, UI_LINE);
    for (int cx : {x + 2, x + w - 4})
        for (int cy : {y + 2, y + h - 4}) rect(cx, cy, 2, 2, UI_GLOW);
}

void tag(int x, int y, int w, int h) {
    rrect(x, y, w, h, UI_EDGE);
    rrect(x + 1, y + 1, w - 2, h - 2, UI_LINE_D);
    rect(x + 2, y + h - 2, w - 4, 1, UI_LINE);
}

void bar(int x, int y, int w, int value, int max, Color fill) {
    rrect(x, y, w, 6, UI_EDGE);
    rect(x + 1, y + 1, w - 2, 4, UI_LINE_D);
    int f = max > 0 ? (w - 2) * std::clamp(value, 0, max) / max : 0;
    rect(x + 1, y + 1, f, 4, fill);
    rect(x + 1, y + 1, f, 1, UI_GLOW);
}

struct FontData { const FontGlyph* glyphs; int count; const std::uint16_t* rows; int ascent, line; };
static const FontData FONTS[] = {
    {kFont11Glyphs, kFont11GlyphCount, kFont11Rows, 11, 16},
    {kFont9Glyphs, kFont9GlyphCount, kFont9Rows, 9, 12},
};

int line_h(Font f) { return FONTS[(int)f].line; }

static std::uint32_t next_cp(const std::string& s, size_t& i) {
    unsigned char c = s[i++];
    if (c < 0x80) return c;
    int n = c >= 0xF0 ? 3 : c >= 0xE0 ? 2 : 1;
    std::uint32_t cp = c & (0x3F >> n);
    while (n-- && i < s.size()) cp = (cp << 6) | (s[i++] & 0x3F);
    return cp;
}

static const FontGlyph* glyph(const FontData& fd, std::uint32_t cp) {
    auto end = fd.glyphs + fd.count;
    auto find = [&](std::uint32_t v) {
        return std::lower_bound(fd.glyphs, end, v, [](const FontGlyph& a, std::uint32_t b) { return a.cp < b; });
    };
    auto g = find(cp);
    return g != end && g->cp == cp ? g : find('?');
}

int text(int x, int y, const std::string& s, Color c, int max_cp, Font f, int scale) {
    const FontData& fd = FONTS[(int)f];
    int pen = x, n = 0;
    for (size_t i = 0; i < s.size() && (max_cp < 0 || n < max_cp); ++n) {
        std::uint32_t cp = next_cp(s, i);
        if (cp == '\n') { pen = x; y += fd.line * scale; continue; }
        const FontGlyph* g = glyph(fd, cp);
        int top = y + (fd.ascent - g->yo - g->h) * scale;
        for (int r = 0; r < g->h; ++r) {
            std::uint16_t bits = fd.rows[g->row + r];
            for (int b = 0; b < g->w; ++b)
                if (bits & (0x8000 >> b)) rect(pen + (g->xo + b) * scale, top + r * scale, scale, scale, c);
        }
        pen += g->adv * scale;
    }
    return pen - x;
}

int text_width(const std::string& s, Font f, int scale) {
    int w = 0;
    for (size_t i = 0; i < s.size();) w += glyph(FONTS[(int)f], next_cp(s, i))->adv * scale;
    return w;
}

int cp_count(const std::string& s) {
    int n = 0;
    for (size_t i = 0; i < s.size(); ++n) next_cp(s, i);
    return n;
}

std::vector<std::string> wrap(const std::string& s, int width, Font f) {
    std::vector<std::string> lines;
    std::string line, word;
    auto flush_word = [&] {
        if (!line.empty() && text_width(line + word, f) > width) {
            while (!line.empty() && line.back() == ' ') line.pop_back();
            lines.push_back(line);
            line.clear();
            while (!word.empty() && word[0] == ' ') word.erase(0, 1);
        }
        line += word;
        word.clear();
    };
    for (size_t i = 0; i < s.size();) {
        size_t start = i;
        std::uint32_t cp = next_cp(s, i);
        if (cp == '\n') { flush_word(); lines.push_back(line); line.clear(); continue; }
        if (cp == ' ') flush_word();
        word.append(s, start, i - start);
    }
    flush_word();
    if (!line.empty()) lines.push_back(line);
    return lines;
}

}  // namespace gfx
