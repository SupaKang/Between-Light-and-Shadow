#pragma once
#include "types.hpp"
#include <vector>
#include <cstring>

namespace JoseonRPG {

class Renderer {
public:
    Renderer();
    ~Renderer() = default;

    void clear(Color color = Palette::Black);
    void setPixel(int x, int y, Color color);
    Color getPixel(int x, int y) const;

    void drawLine(int x0, int y0, int x1, int y1, Color color);
    void drawRect(int x, int y, int w, int h, Color color);
    void fillRect(int x, int y, int w, int h, Color color);
    void drawPanel(int x, int y, int w, int h, Color bg = Palette::DarkGray, Color border = Palette::MidGray);
    void drawHealthBar(int x, int y, int w, int h, int curVal, int maxVal, Color fillColor, Color bgColor = Palette::DarkGray);
    
    // Draw 16x16 tile from procedural pattern / real Gen 1 2-bit DMG bitmap
    void drawTileProcedural(int x, int y, int tileId);
    
    // Draw 16x16 / 16x24 / 32x32 character or environmental sprite
    void drawSprite(int x, int y, int spriteId, int frame, bool flipX = false);

    // Draw Gen 1 2-bit DMG packed bitmap (4 pixels per byte) with optional transparency (key = 0)
    void drawGen1Bitmap(int x, int y, int w, int h, const uint8_t* packedData, bool transparentKey0 = false, bool flipX = false);

    // Post-processing CRT Scanline & Vignette
    void applyPostProcess(bool crtScanlines = true, bool vignette = true);

    // Screen Fade Transition (0.0 = black, 1.0 = fully visible)
    void applyFade(float brightness);

    const uint32_t* getFramebuffer() const { return m_framebuffer.data(); }
    int getWidth() const { return SCREEN_WIDTH; }
    int getHeight() const { return SCREEN_HEIGHT; }

private:
    std::array<uint32_t, SCREEN_WIDTH * SCREEN_HEIGHT> m_framebuffer;
};

} // namespace JoseonRPG
