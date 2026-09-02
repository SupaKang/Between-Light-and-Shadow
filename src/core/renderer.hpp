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
    
    // Draw 16x16 tile from raw bitmap or procedural pattern
    void drawTileProcedural(int x, int y, int tileId);
    
    // Draw 16x16 character sprite
    void drawSprite(int x, int y, int spriteId, int frame, bool flipX = false);

    const uint32_t* getFramebuffer() const { return m_framebuffer.data(); }
    int getWidth() const { return SCREEN_WIDTH; }
    int getHeight() const { return SCREEN_HEIGHT; }

private:
    std::array<uint32_t, SCREEN_WIDTH * SCREEN_HEIGHT> m_framebuffer;
};

} // namespace JoseonRPG
