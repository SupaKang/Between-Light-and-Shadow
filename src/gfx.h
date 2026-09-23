// Software framebuffers. No SDL here.
// Two layers: the world is drawn at 320x180 (chunky 16px tiles), then doubled into the
// 640x360 output where UI and text are drawn at finer pixels. 640x360 x3 = 1920x1080.
#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace gfx {

constexpr int W = 320, H = 180;    // world layer
constexpr int UW = 640, UH = 360;  // UI layer = final output
using Color = std::uint32_t;       // 0xRRGGBB
extern Color out[UW * UH];

void target_world();  // subsequent draws go to the 320x180 world layer
void target_ui();     // subsequent draws go to the 640x360 output
void compose();       // world x2 -> output

// Rows are strings of w chars: hex digits index the sprite's palette (4-colour ramps or the 16-colour
// GHOST palette), '.' = transparent.
struct Sprite { int w, h; const char* const* rows; const Color* ramp; };

void clear(Color c);
void pset(int x, int y, Color c);
void rect(int x, int y, int w, int h, Color c);
void rrect(int x, int y, int w, int h, Color c);  // corners cut by 1px
void frame_rect(int x, int y, int w, int h, Color c);
void disc(int cx, int cy, int r, Color c);
void shade(int x, int y, int w, int h, float k);
void tint(float r, float g, float b);
// Tint everything except a light pool around (cx, cy): untinted inside r_in, checker-dithered ring to r_out.
void tint_lit(int cx, int cy, int r_in, int r_out, float r, float g, float b);
void fade(float amount);

// ramp overrides the sprite's own ramp (palette swap, e.g. the elder reuses the hero sprite).
void sprite(const Sprite& s, int x, int y, bool flip = false, int scale = 1, const Color* ramp = nullptr);
void sprite_tint(float r, float g, float b);  // multiply applied by sprite(); (1,1,1) = off

// Ghost-story UI colours (black / greens, blood accent).
constexpr Color UI_BG = 0x0a120c, UI_EDGE = 0x040605, UI_LINE = 0x3e6e44, UI_LINE_D = 0x1c3426,
                UI_TEXT = 0xd8e8c8, UI_DIM = 0x9aa890, UI_GLOW = 0xa8e08a, UI_HP = 0x62a05a, UI_BLOOD = 0xa83a2e;
void panel(int x, int y, int w, int h);  // near-black panel, double green line, glowing corner knots
void tag(int x, int y, int w, int h);    // small name label
void bar(int x, int y, int w, int value, int max, Color fill);

enum class Font { Body, Small };  // Galmuri11 (Hangul 12px advance), Galmuri9 (10px)
int line_h(Font f);
int text(int x, int y, const std::string& s, Color c, int max_cp = -1, Font f = Font::Body, int scale = 1);
int text_width(const std::string& s, Font f = Font::Body, int scale = 1);
int cp_count(const std::string& s);
std::vector<std::string> wrap(const std::string& s, int width, Font f = Font::Body);

}  // namespace gfx
