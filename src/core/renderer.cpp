#include "renderer.hpp"
#include <cmath>

namespace JoseonRPG {

Renderer::Renderer() {
    clear(Palette::Black);
}

void Renderer::clear(Color color) {
    uint32_t c = color.toARGB();
    std::fill(m_framebuffer.begin(), m_framebuffer.end(), c);
}

void Renderer::setPixel(int x, int y, Color color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    if (color.a == 0) return;
    if (color.a == 255) {
        m_framebuffer[y * SCREEN_WIDTH + x] = color.toARGB();
    } else {
        // Alpha blend
        uint32_t bg = m_framebuffer[y * SCREEN_WIDTH + x];
        uint8_t bgB = bg & 0xFF;
        uint8_t bgG = (bg >> 8) & 0xFF;
        uint8_t bgR = (bg >> 16) & 0xFF;

        float alpha = color.a / 255.0f;
        uint8_t outR = static_cast<uint8_t>(color.r * alpha + bgR * (1.0f - alpha));
        uint8_t outG = static_cast<uint8_t>(color.g * alpha + bgG * (1.0f - alpha));
        uint8_t outB = static_cast<uint8_t>(color.b * alpha + bgB * (1.0f - alpha));

        Color blended(outR, outG, outB);
        m_framebuffer[y * SCREEN_WIDTH + x] = blended.toARGB();
    }
}

Color Renderer::getPixel(int x, int y) const {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return Palette::Black;
    uint32_t c = m_framebuffer[y * SCREEN_WIDTH + x];
    Color out;
    out.b = c & 0xFF;
    out.g = (c >> 8) & 0xFF;
    out.r = (c >> 16) & 0xFF;
    out.a = (c >> 24) & 0xFF;
    return out;
}

void Renderer::drawLine(int x0, int y0, int x1, int y1, Color color) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        setPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void Renderer::drawRect(int x, int y, int w, int h, Color color) {
    drawLine(x, y, x + w - 1, y, color);
    drawLine(x, y + h - 1, x + w - 1, y + h - 1, color);
    drawLine(x, y, x, y + h - 1, color);
    drawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
}

void Renderer::fillRect(int x, int y, int w, int h, Color color) {
    int xStart = std::max(0, x);
    int yStart = std::max(0, y);
    int xEnd = std::min(SCREEN_WIDTH, x + w);
    int yEnd = std::min(SCREEN_HEIGHT, y + h);

    for (int cy = yStart; cy < yEnd; ++cy) {
        for (int cx = xStart; cx < xEnd; ++cx) {
            setPixel(cx, cy, color);
        }
    }
}

void Renderer::drawTileProcedural(int px, int py, int tileId) {
    // Procedural 16x16 tile pattern generator for size budget efficiency
    switch (tileId) {
        case 0: // Grass / Earth (바닥 풀밭)
            fillRect(px, py, 16, 16, Color(42, 100, 52));
            setPixel(px + 3, py + 4, Color(60, 140, 75));
            setPixel(px + 4, py + 3, Color(60, 140, 75));
            setPixel(px + 11, py + 12, Color(60, 140, 75));
            setPixel(px + 12, py + 11, Color(60, 140, 75));
            break;
        case 1: // Hanok Wall / Stone Wall (돌담/벽)
            fillRect(px, py, 16, 16, Color(90, 85, 80));
            drawRect(px, py, 16, 16, Color(50, 45, 40));
            drawLine(px, py + 8, px + 15, py + 8, Color(50, 45, 40));
            drawLine(px + 8, py, px + 8, py + 8, Color(50, 45, 40));
            drawLine(px + 4, py + 8, px + 4, py + 15, Color(50, 45, 40));
            break;
        case 2: // Dirt Road (흙길)
            fillRect(px, py, 16, 16, Color(160, 130, 90));
            setPixel(px + 2, py + 6, Color(130, 100, 70));
            setPixel(px + 9, py + 3, Color(130, 100, 70));
            setPixel(px + 13, py + 11, Color(130, 100, 70));
            break;
        case 3: // Hanok Wooden Floor / Maru (마루)
            fillRect(px, py, 16, 16, Color(180, 120, 60));
            drawLine(px, py, px + 15, py, Color(120, 80, 40));
            drawLine(px, py + 4, px + 15, py + 4, Color(120, 80, 40));
            drawLine(px, py + 8, px + 15, py + 8, Color(120, 80, 40));
            drawLine(px, py + 12, px + 15, py + 12, Color(120, 80, 40));
            break;
        case 4: // Hanok Tile Roof / Giwa (기와 지붕)
            fillRect(px, py, 16, 16, Color(35, 40, 50));
            drawLine(px, py + 3, px + 15, py + 3, Color(60, 70, 85));
            drawLine(px, py + 7, px + 15, py + 7, Color(60, 70, 85));
            drawLine(px, py + 11, px + 15, py + 11, Color(60, 70, 85));
            drawLine(px, py + 15, px + 15, py + 15, Color(20, 22, 28));
            break;
        case 5: // Sacred Tree / Dangsan tree (당산나무)
            fillRect(px, py, 16, 16, Color(28, 85, 42));
            fillRect(px + 4, py + 4, 8, 8, Color(45, 125, 65));
            drawRect(px + 2, py + 2, 12, 12, Color(20, 60, 30));
            break;
        default:
            fillRect(px, py, 16, 16, Palette::DarkGray);
            break;
    }
}

void Renderer::drawSprite(int px, int py, int spriteId, int frame, bool flipX) {
    (void)flipX;
    // SD 2-head-high character or Yokai sprite (16x16)
    if (spriteId == 0) {
        // Player: Joseon Exorcist (벽사청 영술사 - 도포 & 갓)
        // Gat (Korean Hat)
        fillRect(px + 3, py + 1, 10, 2, Palette::Black);
        fillRect(px + 5, py, 6, 2, Palette::Black);
        
        // Face
        fillRect(px + 5, py + 3, 6, 4, Color(250, 215, 175));
        setPixel(px + 6, py + 4, Palette::Black); // Eye L
        setPixel(px + 9, py + 4, Palette::Black); // Eye R
        
        // Dopo (White Robe with Blue Ribbon)
        fillRect(px + 4, py + 7, 8, 6, Palette::White);
        fillRect(px + 6, py + 7, 4, 2, Palette::Blue); // Ribbon
        
        // Feet / Animation frame
        if (frame % 2 == 0) {
            fillRect(px + 4, py + 13, 3, 2, Palette::DarkGray);
            fillRect(px + 9, py + 13, 3, 2, Palette::DarkGray);
        } else {
            fillRect(px + 5, py + 13, 3, 2, Palette::DarkGray);
            fillRect(px + 8, py + 13, 3, 2, Palette::DarkGray);
        }
    } else if (spriteId == 1) {
        // Dokkaebi (도깨비 - 뿔, 붉은 피부, 방망이)
        fillRect(px + 4, py + 2, 8, 7, Palette::Red);
        setPixel(px + 5, py + 1, Palette::Yellow); // Horn L
        setPixel(px + 10, py + 1, Palette::Yellow); // Horn R
        setPixel(px + 6, py + 4, Palette::White); // Eye L
        setPixel(px + 9, py + 4, Palette::White); // Eye R
        fillRect(px + 5, py + 6, 6, 2, Palette::Black); // Wide Grin
        setPixel(px + 6, py + 6, Palette::White); // Fang
        
        // Body / loincloth
        fillRect(px + 4, py + 9, 8, 5, Color(190, 90, 30));
        // Club (방망이)
        fillRect(px + 12, py + 4, 3, 9, Color(140, 95, 45));
    }
}

} // namespace JoseonRPG
