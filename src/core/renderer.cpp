#include "renderer.hpp"
#include "gen1_assets.hpp"
#include <cmath>

namespace JoseonRPG {

void Renderer::drawGen1Bitmap(int px, int py, int w, int h, const uint8_t* packedData, bool transparentKey0, bool flipX) {
    if (!packedData) return;
    int pixelIdx = 0;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int byteIdx = pixelIdx / 4;
            int shift = 6 - (pixelIdx % 4) * 2;
            uint8_t colorIdx = (packedData[byteIdx] >> shift) & 0x03;
            pixelIdx++;

            if (transparentKey0 && colorIdx == 0) continue;

            int targetX = flipX ? (px + (w - 1 - x)) : (px + x);
            int targetY = py + y;

            if (targetX >= 0 && targetX < SCREEN_WIDTH && targetY >= 0 && targetY < SCREEN_HEIGHT) {
                m_framebuffer[targetY * SCREEN_WIDTH + targetX] = Gen1Assets::DMG_PALETTE[colorIdx];
            }
        }
    }
}

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

void Renderer::drawPanel(int x, int y, int w, int h, Color bg, Color border) {
    fillRect(x, y, w, h, bg);
    drawRect(x, y, w, h, border);
    drawRect(x + 1, y + 1, w - 2, h - 2, Palette::Black);
}

void Renderer::draw9SliceBox(int x, int y, int w, int h, UITheme theme) {
    if (w < 6 || h < 6) return;

    Color bg, borderOuter, borderInner, cornerAccent;
    switch (theme) {
        case UITheme::Paper:
            bg = Color(224, 248, 208);         // Pale DMG Off-White / Hanji
            borderOuter = Color(8, 24, 32);     // Darkest Ink Black
            borderInner = Color(52, 104, 86);   // Dark Olive / Changhoji frame
            cornerAccent = Color(136, 192, 112);
            break;
        case UITheme::Dark:
            bg = Color(14, 18, 24);            // Dark Slate Ink
            borderOuter = Color(80, 96, 120);   // Steel Blue Border
            borderInner = Color(36, 44, 58);
            cornerAccent = Color(180, 195, 215);
            break;
        case UITheme::Royal:
            bg = Color(28, 16, 20);            // Deep Royal Crimson
            borderOuter = Color(212, 175, 55);  // Royal Gold
            borderInner = Color(120, 32, 40);
            cornerAccent = Color(245, 220, 110);
            break;
        case UITheme::Inverted:
            bg = Color(8, 24, 32);
            borderOuter = Color(224, 248, 208);
            borderInner = Color(136, 192, 112);
            cornerAccent = Color(224, 248, 208);
            break;
    }

    // 1. Fill Inner Background
    fillRect(x + 1, y + 1, w - 2, h - 2, bg);

    // 2. Draw Outer 1px Border
    drawRect(x, y, w, h, borderOuter);

    // 3. Draw Inner Inset 1px Frame
    if (w >= 10 && h >= 10) {
        drawRect(x + 2, y + 2, w - 4, h - 4, borderInner);
        
        // 4. Korean Lattice / Brass Corner Details
        setPixel(x + 1, y + 1, borderOuter);
        setPixel(x + 3, y + 3, cornerAccent);
        
        setPixel(x + w - 2, y + 1, borderOuter);
        setPixel(x + w - 4, y + 3, cornerAccent);
        
        setPixel(x + 1, y + h - 2, borderOuter);
        setPixel(x + 3, y + h - 4, cornerAccent);
        
        setPixel(x + w - 2, y + h - 2, borderOuter);
        setPixel(x + w - 4, y + h - 4, cornerAccent);
    }
}

void Renderer::drawGaugeBar(int x, int y, int w, int h, int curVal, int maxVal, Color fillColor, Color bgColor, Color borderColor) {
    if (w <= 2 || h <= 2) return;
    
    fillRect(x, y, w, h, bgColor);
    drawRect(x, y, w, h, borderColor);

    if (maxVal <= 0) return;
    float ratio = std::clamp(static_cast<float>(curVal) / maxVal, 0.0f, 1.0f);
    int fillW = static_cast<int>((w - 2) * ratio);
    
    if (fillW > 0) {
        fillRect(x + 1, y + 1, fillW, h - 2, fillColor);
        
        // 1px Top Highlight for high-end retro meter feel
        if (h >= 4) {
            Color highlight(
                static_cast<uint8_t>(std::min(255, fillColor.r + 45)),
                static_cast<uint8_t>(std::min(255, fillColor.g + 45)),
                static_cast<uint8_t>(std::min(255, fillColor.b + 45))
            );
            drawLine(x + 1, y + 1, x + fillW, y + 1, highlight);
        }
    }
}

void Renderer::drawHealthBar(int x, int y, int w, int h, int curVal, int maxVal, Color fillColor, Color bgColor) {
    drawGaugeBar(x, y, w, h, curVal, maxVal, fillColor, bgColor, Palette::Black);
}

void Renderer::drawTileProcedural(int px, int py, int tileId) {
    if (tileId >= 0 && tileId < Gen1Assets::TOTAL_TILES) {
        drawGen1Bitmap(px, py, 16, 16, Gen1Assets::TILES_16x16[tileId], false);
        return;
    }
    // Fallback if tileId is out of range
    switch (tileId) {
        case 0: // Grass / Earth (바닥 풀밭 & 들꽃)
            fillRect(px, py, 16, 16, Color(42, 100, 52));
            setPixel(px + 3, py + 4, Color(60, 140, 75));
            setPixel(px + 4, py + 3, Color(60, 140, 75));
            setPixel(px + 11, py + 12, Color(60, 140, 75));
            setPixel(px + 12, py + 11, Color(60, 140, 75));
            // Subtle wild white/yellow flower dots
            setPixel(px + 8, py + 7, Palette::GardeniaYellow);
            setPixel(px + 14, py + 3, Palette::BaegokWhite);
            break;
        case 1: // Hanok Wall / Stone Wall (돌담/벽 & 이끼)
            fillRect(px, py, 16, 16, Color(90, 85, 80));
            drawRect(px, py, 16, 16, Color(50, 45, 40));
            drawLine(px, py + 8, px + 15, py + 8, Color(50, 45, 40));
            drawLine(px + 8, py, px + 8, py + 8, Color(50, 45, 40));
            drawLine(px + 4, py + 8, px + 4, py + 15, Color(50, 45, 40));
            setPixel(px + 1, py + 14, Color(45, 95, 55)); // Moss
            setPixel(px + 2, py + 14, Color(45, 95, 55));
            break;
        case 2: // Dirt Road (황토 흙길 & 자갈)
            fillRect(px, py, 16, 16, Color(160, 130, 90));
            setPixel(px + 2, py + 6, Color(130, 100, 70));
            setPixel(px + 9, py + 3, Color(130, 100, 70));
            setPixel(px + 13, py + 11, Color(130, 100, 70));
            setPixel(px + 6, py + 13, Color(185, 155, 115)); // Small pebble
            break;
        case 3: // Hanok Wooden Floor / Maru (대청마루 나무결)
            fillRect(px, py, 16, 16, Color(180, 120, 60));
            drawLine(px, py, px + 15, py, Color(120, 80, 40));
            drawLine(px, py + 4, px + 15, py + 4, Color(120, 80, 40));
            drawLine(px, py + 8, px + 15, py + 8, Color(120, 80, 40));
            drawLine(px, py + 12, px + 15, py + 12, Color(120, 80, 40));
            break;
        case 4: // Hanok Tile Roof / Giwa (기와 지붕 & 청기와)
            fillRect(px, py, 16, 16, Color(35, 40, 50));
            drawLine(px, py + 3, px + 15, py + 3, Color(60, 70, 85));
            drawLine(px, py + 7, px + 15, py + 7, Color(60, 70, 85));
            drawLine(px, py + 11, px + 15, py + 11, Color(60, 70, 85));
            drawLine(px, py + 15, px + 15, py + 15, Color(20, 22, 28));
            break;
        case 5: // Sacred Tree / Dangsan tree (당산나무 & 오색 천)
            fillRect(px, py, 16, 16, Color(28, 85, 42));
            fillRect(px + 4, py + 4, 8, 8, Color(45, 125, 65));
            drawRect(px + 2, py + 2, 12, 12, Color(20, 60, 30));
            setPixel(px + 7, py + 8, Palette::CinnabarRed);     // Red spirit cloth
            setPixel(px + 8, py + 8, Palette::GardeniaYellow);  // Yellow spirit cloth
            setPixel(px + 9, py + 8, Palette::IndigoBlue);      // Blue spirit cloth
            break;
        case 6: // Water / Stream / Sea (시냇물 / 바다 & 물결)
            fillRect(px, py, 16, 16, Color(30, 80, 150));
            drawLine(px + 2, py + 4, px + 7, py + 4, Color(70, 130, 210));
            drawLine(px + 9, py + 10, px + 14, py + 10, Color(70, 130, 210));
            break;
        case 7: // Wooden Bridge (나무 다리 & 난간)
            fillRect(px, py, 16, 16, Color(140, 90, 45));
            drawRect(px, py, 16, 16, Color(90, 55, 25));
            drawLine(px + 4, py, px + 4, py + 15, Color(90, 55, 25));
            drawLine(px + 11, py, px + 11, py + 15, Color(90, 55, 25));
            break;
        case 8: // Bamboo Forest / Tall Grass (대나무 덤불 / 야생 수풀)
            fillRect(px, py, 16, 16, Color(35, 90, 45));
            drawLine(px + 3, py + 14, px + 3, py + 2, Color(70, 180, 80));
            drawLine(px + 8, py + 14, px + 8, py + 1, Color(90, 200, 100));
            drawLine(px + 13, py + 14, px + 13, py + 3, Color(70, 180, 80));
            setPixel(px + 4, py + 4, Color(120, 220, 110));
            setPixel(px + 9, py + 3, Color(120, 220, 110));
            break;
        case 9: // Dark Obsidian Altar (음양당 흑요석 제단 & 부적 문양)
            fillRect(px, py, 16, 16, Color(24, 20, 32));
            drawRect(px + 2, py + 2, 12, 12, Color(90, 40, 120));
            fillRect(px + 6, py + 6, 4, 4, Palette::CinnabarRed);
            setPixel(px + 7, py + 7, Palette::GardeniaYellow);
            break;
        case 10: // Treasure Chest (보물 상자 & 황금 자물쇠)
            fillRect(px, py, 16, 16, Color(42, 100, 52)); // grass base
            fillRect(px + 2, py + 4, 12, 10, Color(180, 130, 40));
            drawRect(px + 2, py + 4, 12, 10, Color(90, 60, 20));
            fillRect(px + 6, py + 7, 4, 3, Palette::Yellow);
            break;
        case 11: // Hongsalmun Gate (홍살문 / 관문 기둥)
            fillRect(px, py, 16, 16, Color(160, 130, 90)); // dirt base
            fillRect(px + 1, py + 1, 3, 14, Palette::CinnabarRed);
            fillRect(px + 12, py + 1, 3, 14, Palette::CinnabarRed);
            fillRect(px + 1, py + 3, 14, 3, Palette::CinnabarRed);
            setPixel(px + 7, py + 2, Palette::IndigoBlue); // Taegeuk center
            setPixel(px + 8, py + 2, Palette::CinnabarRed);
            break;
        case 12: // Jangseung & Sotdae (천하대장군 장승 & 솟대 기둥)
            fillRect(px, py, 16, 16, Color(42, 100, 52)); // grass base
            fillRect(px + 5, py + 2, 6, 13, Color(130, 85, 45)); // Wood pole
            fillRect(px + 4, py + 3, 8, 4, Color(155, 105, 55));  // Face block
            setPixel(px + 5, py + 4, Palette::SongyeonInk);       // Left eye
            setPixel(px + 8, py + 4, Palette::SongyeonInk);       // Right eye
            fillRect(px + 6, py + 6, 2, 1, Palette::BaegokWhite); // Big teeth grin
            // Sotdae bird on top
            setPixel(px + 7, py + 1, Color(90, 55, 25));
            setPixel(px + 8, py, Color(90, 55, 25));
            break;
        case 13: // Doltap & Spirit Cairns (서낭당 돌탑 & 오색 깃발)
            fillRect(px, py, 16, 16, Color(42, 100, 52));
            fillRect(px + 3, py + 10, 10, 5, Color(110, 105, 100)); // Base stone
            fillRect(px + 5, py + 6, 6, 4, Color(140, 135, 130));   // Middle stone
            fillRect(px + 7, py + 3, 2, 3, Color(165, 160, 155));   // Top stone
            setPixel(px + 7, py + 1, Palette::CinnabarRed);         // Small wish cloth
            setPixel(px + 8, py + 1, Palette::IndigoBlue);
            break;
        case 14: // Onggi Kimchi & Soy Pots (장독대 옹기 항아리)
            fillRect(px, py, 16, 16, Color(150, 140, 130)); // Stone platform
            // Big Pot L
            fillRect(px + 2, py + 5, 5, 8, Color(70, 45, 30));
            fillRect(px + 1, py + 6, 7, 5, Color(85, 55, 35));
            fillRect(px + 3, py + 4, 3, 2, Color(50, 30, 20)); // Lid
            // Small Pot R
            fillRect(px + 9, py + 7, 5, 6, Color(70, 45, 30));
            fillRect(px + 10, py + 6, 3, 2, Color(50, 30, 20)); // Lid
            break;
        case 15: // Straw Thatch Eaves / Chogajip (초가지붕 처마 & 볏짚)
            fillRect(px, py, 16, 16, Color(210, 165, 75)); // Golden straw
            drawLine(px, py + 4, px + 15, py + 4, Color(170, 130, 50));
            drawLine(px, py + 8, px + 15, py + 8, Color(170, 130, 50));
            drawLine(px, py + 12, px + 15, py + 12, Color(170, 130, 50));
            drawLine(px, py + 15, px + 15, py + 15, Color(130, 95, 35)); // Rafter shadow
            break;
        case 16: // Iron Ore Vein & Blacksmith Anvil (무쇠광산 원석 & 모루)
            fillRect(px, py, 16, 16, Color(75, 70, 70)); // Mine rock
            fillRect(px + 4, py + 4, 8, 7, Color(140, 135, 150)); // Raw metallic lump
            setPixel(px + 6, py + 5, Color(230, 225, 240));       // Silver specular glint
            setPixel(px + 9, py + 8, Color(230, 225, 240));
            // Anvil base
            fillRect(px + 3, py + 12, 10, 3, Color(45, 45, 50));
            break;
        case 17: // Bioluminescent Coral & Deep Sea Sponges (심해 산호초 & 해초)
            fillRect(px, py, 16, 16, Color(15, 35, 75)); // Deep ocean floor
            // Magenta Coral
            fillRect(px + 3, py + 6, 4, 8, Color(215, 60, 140));
            fillRect(px + 2, py + 4, 2, 3, Color(245, 100, 175));
            fillRect(px + 6, py + 3, 2, 4, Color(245, 100, 175));
            // Cyan Sea Anemone
            fillRect(px + 10, py + 8, 4, 6, Color(30, 200, 220));
            setPixel(px + 11, py + 6, Color(100, 255, 255));
            setPixel(px + 13, py + 7, Color(100, 255, 255));
            break;
        case 18: // Sacred Cloud Platform & Celestial Mist (선계 운해 징검다리)
            fillRect(px, py, 16, 16, Color(120, 160, 210)); // Sky background
            fillRect(px + 2, py + 4, 12, 8, Color(245, 250, 255)); // Fluffy cloud
            fillRect(px + 4, py + 2, 8, 12, Color(230, 240, 255));
            drawRect(px + 2, py + 4, 12, 8, Color(180, 210, 240));
            setPixel(px + 7, py + 7, Palette::GardeniaYellow); // Divine aura glint
            break;
        case 19: // Glacial Snow & Ice Sheet (백두산 천지 만년설 빙판)
            fillRect(px, py, 16, 16, Color(220, 235, 250)); // Snow white
            fillRect(px + 3, py + 4, 10, 8, Color(165, 205, 240)); // Clear blue ice
            drawLine(px + 4, py + 5, px + 8, py + 5, Color(245, 255, 255)); // Ice glint
            setPixel(px + 11, py + 9, Color(245, 255, 255));
            break;
        case 20: // Obsidian Rune Ward Stone (음양당 결계 비석 & 보라빛 룬)
            fillRect(px, py, 16, 16, Color(30, 25, 40));
            fillRect(px + 4, py + 2, 8, 12, Color(60, 45, 75));
            drawRect(px + 4, py + 2, 8, 12, Color(110, 70, 150));
            // Glowing Purple Sigil
            setPixel(px + 7, py + 5, Color(220, 140, 255));
            setPixel(px + 8, py + 6, Color(220, 140, 255));
            setPixel(px + 7, py + 7, Color(220, 140, 255));
            setPixel(px + 8, py + 8, Color(220, 140, 255));
            break;
        case 21: // Ghost Ship Broken Timber & Nets (난파선 파손 선체 & 밧줄)
            fillRect(px, py, 16, 16, Color(40, 70, 110)); // Sea water base
            fillRect(px + 2, py + 3, 12, 10, Color(80, 55, 35)); // Rotten plank
            drawLine(px + 2, py + 6, px + 13, py + 6, Color(50, 35, 20));
            // Weathered green fishing net
            setPixel(px + 4, py + 8, Color(90, 140, 100));
            setPixel(px + 6, py + 9, Color(90, 140, 100));
            setPixel(px + 8, py + 8, Color(90, 140, 100));
            break;
        case 22: // Water Lily & Lotus Pond (연꽃 연못 & 찰랑이는 수련)
            fillRect(px, py, 16, 16, Color(35, 90, 140)); // Pond water
            fillRect(px + 3, py + 4, 6, 5, Color(45, 135, 65)); // Lotus pad
            fillRect(px + 8, py + 7, 5, 4, Color(45, 135, 65));
            // Blooming Pink Lotus Flower
            setPixel(px + 6, py + 5, Palette::PeonyPink);
            setPixel(px + 5, py + 5, Palette::BaegokWhite);
            break;
        case 23: // Secret Stone Well Rim / Aqueduct Grate (비밀 수로 우물 & 이끼)
            fillRect(px, py, 16, 16, Color(160, 130, 90)); // Dirt surround
            fillRect(px + 3, py + 3, 10, 10, Color(90, 85, 80)); // Stone well rim
            fillRect(px + 5, py + 5, 6, 6, Color(20, 25, 40));   // Dark deep water
            drawRect(px + 3, py + 3, 10, 10, Color(60, 55, 50));
            setPixel(px + 4, py + 4, Color(60, 140, 75)); // Green moss
            setPixel(px + 11, py + 10, Color(60, 140, 75));
            break;
        case 24: // Martial Arena Dancheong Banner (환상비무대 단청 깃발)
            fillRect(px, py, 16, 16, Color(130, 110, 90)); // Arena flagstone
            fillRect(px + 3, py + 2, 2, 13, Palette::SongyeonInk); // Flagpole
            // Fluttering Dancheong Banner
            fillRect(px + 5, py + 2, 8, 3, Palette::CinnabarRed);
            fillRect(px + 5, py + 5, 8, 3, Palette::IndigoBlue);
            setPixel(px + 7, py + 3, Palette::GardeniaYellow);
            break;
        case 25: // Cliff / Rock Wall Elevation (바위 절벽 암벽 & 음영)
            fillRect(px, py, 16, 16, Color(95, 88, 80)); // Cliff rock
            fillRect(px, py, 16, 4, Color(125, 118, 110)); // Top highlight
            drawLine(px + 2, py + 5, px + 5, py + 14, Color(65, 58, 50)); // Deep fissure
            drawLine(px + 9, py + 3, px + 14, py + 15, Color(65, 58, 50));
            setPixel(px + 3, py + 2, Color(55, 120, 65)); // Moss on ledge
            setPixel(px + 12, py + 2, Color(55, 120, 65));
            break;
        case 26: // Stone Staircase / Cliff Steps (돌계단 & 고저차 통로)
            fillRect(px, py, 16, 16, Color(140, 135, 130));
            for (int step = 0; step < 4; ++step) {
                fillRect(px + 1, py + step * 4, 14, 2, Color(175, 170, 165)); // Step surface
                fillRect(px + 1, py + step * 4 + 2, 14, 2, Color(90, 85, 80));   // Step riser shadow
            }
            break;
        case 27: // Wooden Ferry Pier / Dock (나루터 목조 선착장 데크)
            fillRect(px, py, 16, 16, Color(30, 80, 150)); // Water backdrop
            fillRect(px + 1, py + 2, 14, 12, Color(145, 95, 50)); // Planks
            drawRect(px + 1, py + 2, 14, 12, Color(85, 50, 25));
            drawLine(px + 5, py + 2, px + 5, py + 13, Color(85, 50, 25));
            drawLine(px + 10, py + 2, px + 10, py + 13, Color(85, 50, 25));
            // Mooring Post
            fillRect(px + 2, py, 2, 4, Color(70, 40, 20));
            fillRect(px + 12, py, 2, 4, Color(70, 40, 20));
            break;
        case 28: // Wooden Ferry Boat (나루터 나룻배 선체 & 노)
            fillRect(px, py, 16, 16, Color(30, 80, 150)); // Water
            fillRect(px + 2, py + 4, 12, 8, Color(160, 105, 55)); // Hull
            fillRect(px + 3, py + 5, 10, 6, Color(110, 70, 35));  // Deck interior
            setPixel(px + 1, py + 7, Color(160, 105, 55));        // Prow
            setPixel(px + 14, py + 7, Color(160, 105, 55));       // Stern
            drawLine(px + 5, py + 1, px + 10, py + 14, Color(210, 170, 100)); // Oar (노)
            break;
        case 29: // Hanok Sliding Paper Door (창호지 미닫이 문 & 문살)
            fillRect(px, py, 16, 16, Color(245, 240, 230)); // Changhoji Paper
            drawRect(px + 1, py + 1, 14, 14, Color(130, 85, 45)); // Wood frame
            // Delicate Grid Lattice (세살 문살)
            drawLine(px + 5, py + 1, px + 5, py + 14, Color(150, 100, 55));
            drawLine(px + 10, py + 1, px + 10, py + 14, Color(150, 100, 55));
            drawLine(px + 1, py + 5, px + 14, py + 5, Color(150, 100, 55));
            drawLine(px + 1, py + 10, px + 14, py + 10, Color(150, 100, 55));
            break;
        case 30: // Traditional Folding Screen (산수화 8폭 병풍)
            fillRect(px, py, 16, 16, Color(235, 225, 205)); // Silk canvas
            drawRect(px + 1, py + 1, 14, 14, Color(90, 55, 30)); // Frame
            // Ink Landscape Mountain & Pine (수묵 산수화)
            drawLine(px + 3, py + 12, px + 8, py + 6, Palette::SongyeonInk);
            drawLine(px + 8, py + 6, px + 13, py + 12, Palette::SongyeonInk);
            fillRect(px + 5, py + 8, 2, 2, Palette::BambooGreen); // Pine tree needle
            setPixel(px + 11, py + 4, Palette::CinnabarRed);     // Red seal stamp (낙관)
            break;
        case 31: // Ceramic Brazier & Firewood (따스한 숯불 화로 & 장작)
            fillRect(px, py, 16, 16, Color(180, 120, 60)); // Floor base
            fillRect(px + 4, py + 6, 8, 7, Color(120, 115, 110)); // Ceramic bowl
            fillRect(px + 5, py + 7, 6, 3, Color(60, 25, 20));    // Charcoal pit
            // Glowing Embers
            setPixel(px + 6, py + 8, Palette::CinnabarRed);
            setPixel(px + 7, py + 7, Palette::TigerOrange);
            setPixel(px + 8, py + 8, Palette::GardeniaYellow);
            // Firewood log beside
            fillRect(px + 1, py + 12, 4, 3, Color(90, 55, 25));
            break;
        case 32: // Tea Table & Porcelain Set (오동나무 찻상 & 백자 다기 세트)
            fillRect(px, py, 16, 16, Color(180, 120, 60)); // Floor
            fillRect(px + 3, py + 5, 10, 7, Color(100, 60, 30)); // Small table
            fillRect(px + 4, py + 6, 8, 5, Color(130, 80, 40));
            // White Porcelain Teapot & Cups
            fillRect(px + 5, py + 7, 3, 3, Palette::BaegokWhite); // Teapot
            setPixel(px + 9, py + 7, Palette::BaegokWhite);       // Teacup
            setPixel(px + 10, py + 8, Palette::BaegokWhite);
            setPixel(px + 6, py + 6, Palette::BichuiJade);        // Green tea lid
            break;
        default:
            fillRect(px, py, 16, 16, Palette::DarkGray);
            break;
    }
}

void Renderer::drawSprite(int px, int py, int spriteId, int frame, bool flipX) {
    if (spriteId == 0) {
        // [0] Player: Joseon Exorcist (16x24 Gen 1 2-bit DMG Sprite)
        int animFrame = frame % 8;
        drawGen1Bitmap(px, py - 4, 16, 24, Gen1Assets::PLAYER_16x24[animFrame], true, flipX);
        return;
    } else if (spriteId == 1) {
        // [1] Master Dosa / Doseon (16x24 Gen 1 2-bit DMG Sprite)
        int animFrame = frame % 8;
        drawGen1Bitmap(px, py - 4, 16, 24, Gen1Assets::NPC_DOSA_16x24[animFrame], true, flipX);
        return;
    } else if (spriteId == 2) {
        // [2] Tavern Hostess Jumo (16x24 Gen 1 2-bit DMG Sprite)
        int animFrame = frame % 8;
        drawGen1Bitmap(px, py - 4, 16, 24, Gen1Assets::NPC_JUMO_16x24[animFrame], true, flipX);
        return;
    } else if (spriteId == 3) {
        // [3] Cultist / Yin-Yang Sorcerer (16x24 Gen 1 2-bit DMG Sprite)
        int animFrame = frame % 8;
        drawGen1Bitmap(px, py - 4, 16, 24, Gen1Assets::NPC_CULTIST_16x24[animFrame], true, flipX);
        return;
    } else if (spriteId == 5) {
        // [5] Village Elder / Peddler (16x24 Gen 1 2-bit DMG Sprite)
        int animFrame = frame % 8;
        drawGen1Bitmap(px, py - 4, 16, 24, Gen1Assets::NPC_ELDER_16x24[animFrame], true, flipX);
        return;
    } else if (spriteId == 12) {
        // [12] Dokkaebi #001 (32x32 Gen 1 2-bit DMG Battler)
        drawGen1Bitmap(px, py, 32, 32, Gen1Assets::DOKKAEBI_32x32, true, flipX);
        return;
    } else if (spriteId == 13) {
        // [13] Gumiho #002 (48x48 Gen 1 2-bit DMG Battler)
        drawGen1Bitmap(px, py, 48, 48, Gen1Assets::GUMIHO_48x48, true, flipX);
        return;
    }

    // Remastered 16x16 / 32x32 Joseon Folklore Character & Yokai Sprites
    if (spriteId == 0) {
        // [0] Player: Joseon Exorcist Soseul (벽사청 영술사 소슬 - 흑립 갓, 백색 도포, 갓끈, 옥패)
        // Gat (흑립 - 갓모자 & 갓양태)
        fillRect(px + 2, py + 1, 12, 2, Palette::SongyeonInk);
        fillRect(px + 5, py, 6, 2, Palette::SongyeonInk);
        // Gat Strap (갓끈 흔들림)
        setPixel(px + 4, py + 3, Palette::SongyeonInk);
        setPixel(px + 4, py + 4, Palette::SongyeonInk);
        setPixel(px + 11, py + 3, Palette::SongyeonInk);
        setPixel(px + 11, py + 4, Palette::SongyeonInk);

        // Face & Eyes
        fillRect(px + 5, py + 3, 6, 4, Color(250, 220, 185));
        setPixel(px + 6, py + 4, Palette::SongyeonInk); // Eye L
        setPixel(px + 9, py + 4, Palette::SongyeonInk); // Eye R

        // Dopo & Gwaeja (백색 도포 & 푸른 쾌자 조끼)
        fillRect(px + 4, py + 7, 8, 6, Palette::BaegokWhite);
        fillRect(px + 5, py + 7, 6, 3, Palette::IndigoBlue); // Gwaeja vest
        setPixel(px + 7, py + 8, Palette::BichuiJade);       // Jade waist pendant (옥패)
        setPixel(px + 8, py + 8, Palette::CinnabarRed);      // Red tassel (홍술)

        // Feet / 4-frame walk animation
        if (frame % 2 == 0) {
            fillRect(px + 4, py + 13, 3, 2, Palette::SongyeonInk);
            fillRect(px + 9, py + 13, 3, 2, Palette::SongyeonInk);
        } else {
            fillRect(px + 5, py + 13, 3, 2, Palette::SongyeonInk);
            fillRect(px + 8, py + 13, 3, 2, Palette::SongyeonInk);
        }
    } else if (spriteId == 1) {
        // [1] High Monk Doseon (도선 대사 - 가사, 108 염주, 육환장)
        fillRect(px + 5, py + 2, 6, 5, Color(245, 215, 180)); // Shaved head
        setPixel(px + 6, py + 4, Palette::SongyeonInk);
        setPixel(px + 9, py + 4, Palette::SongyeonInk);
        fillRect(px + 6, py + 6, 4, 1, Palette::BaegokWhite); // Pure smile
        // Kasaya Monk Robe (주황/갈색 가사 & 백색 장삼)
        fillRect(px + 4, py + 7, 8, 7, Color(160, 82, 45));
        fillRect(px + 4, py + 7, 4, 7, Palette::GardeniaYellow); // Diagonal stole
        // 108 Prayer Beads (염주알)
        setPixel(px + 6, py + 8, Palette::SongyeonInk);
        setPixel(px + 7, py + 9, Palette::SongyeonInk);
        setPixel(px + 8, py + 9, Palette::SongyeonInk);
        setPixel(px + 9, py + 8, Palette::SongyeonInk);
        // Khakkhara Staff (육환장)
        fillRect(px + 13, py + 3, 2, 11, Palette::LightGray);
        setPixel(px + 13, py + 2, Palette::GardeniaYellow);
        setPixel(px + 14, py + 2, Palette::GardeniaYellow);
    } else if (spriteId == 2) {
        // [2] Tavern Hostess Jumo (주막 주모 - 얹은머리, 은비녀, 연분홍 저고리, 남색 치마)
        fillRect(px + 3, py + 1, 10, 3, Palette::SongyeonInk); // Eonjeun Meori (Braided hair)
        fillRect(px + 1, py + 2, 3, 2, Palette::LightGray);   // Silver Binyeo Hairpin
        // Face & Beauty Mark
        fillRect(px + 5, py + 4, 6, 4, Color(255, 225, 195));
        setPixel(px + 6, py + 5, Palette::SongyeonInk);
        setPixel(px + 9, py + 5, Palette::SongyeonInk);
        setPixel(px + 9, py + 7, Palette::CinnabarRed); // Lip rouge
        // Hanbok: Peony Pink Jeogori & Indigo Chima
        fillRect(px + 4, py + 8, 8, 2, Palette::PeonyPink);
        fillRect(px + 3, py + 10, 10, 5, Palette::IndigoBlue);
        // Wine bowl (막걸리 사발)
        fillRect(px + 12, py + 8, 3, 2, Palette::BaegokWhite);
    } else if (spriteId == 3) {
        // [3] Royal Physician (혜민서 의원 - 사모관대, 옥색 관복, 침통, 산삼 약탕관)
        fillRect(px + 5, py, 6, 3, Palette::SongyeonInk); // Samo Hat
        fillRect(px + 3, py + 2, 2, 1, Palette::SongyeonInk); // Hat wings
        fillRect(px + 11, py + 2, 2, 1, Palette::SongyeonInk);
        // Face & Wise Beard
        fillRect(px + 5, py + 3, 6, 4, Color(245, 215, 180));
        setPixel(px + 6, py + 4, Palette::SongyeonInk);
        setPixel(px + 9, py + 4, Palette::SongyeonInk);
        fillRect(px + 7, py + 6, 2, 3, Palette::BaegokWhite); // White Beard
        // Jade Official Robe & Medicine Bottle
        fillRect(px + 4, py + 7, 8, 7, Palette::BichuiJade);
        fillRect(px + 12, py + 8, 3, 4, Color(160, 110, 60)); // Decoction kettle
        setPixel(px + 13, py + 7, Palette::White);             // Steam
    } else if (spriteId == 4) {
        // [4] Police Commander (포도대장 - 전립 상모 깃털, 붉은 융복, 조선 환도)
        fillRect(px + 4, py + 1, 8, 3, Palette::SongyeonInk); // Jeonrip Hat
        fillRect(px + 8, py, 2, 2, Palette::CinnabarRed);     // Crimson Peacock Feather
        // Face & Fierce Eyes
        fillRect(px + 5, py + 4, 6, 4, Color(240, 200, 160));
        setPixel(px + 6, py + 5, Palette::SongyeonInk);
        setPixel(px + 9, py + 5, Palette::SongyeonInk);
        fillRect(px + 6, py + 7, 4, 1, Palette::SongyeonInk); // Mustache
        // Crimson Military Robe (융복) & Hwando Sword
        fillRect(px + 4, py + 8, 8, 6, Palette::CinnabarRed);
        fillRect(px + 2, py + 9, 2, 6, Palette::GardeniaYellow); // Sword scabbard & tassel
    } else if (spriteId == 5) {
        // [5] Peddler Merchant (조선 보부상 - 목화솜 패랭이, 짚신, 목발 지게)
        fillRect(px + 3, py + 1, 10, 2, Color(190, 140, 70)); // Paeraengi Hat
        setPixel(px + 8, py, Palette::White); // Cotton fluff on hat
        // Face
        fillRect(px + 5, py + 3, 6, 4, Color(240, 205, 165));
        setPixel(px + 6, py + 4, Palette::SongyeonInk);
        setPixel(px + 9, py + 4, Palette::SongyeonInk);
        // Commoner Hanbok
        fillRect(px + 4, py + 7, 8, 7, Palette::BaegokWhite);
        // Heavy Wooden A-Frame (지게 짐보따리)
        fillRect(px + 1, py + 5, 2, 9, Color(120, 75, 35));
        fillRect(px + 0, py + 6, 3, 4, Palette::LightGray); // Brasswares parcel
    } else if (spriteId == 6) {
        // [6] Shaman Mudang (당골네 무당 - 화려한 원삼, 오색 신칼, 붉은 댕기)
        fillRect(px + 4, py + 1, 8, 3, Palette::SongyeonInk);
        fillRect(px + 7, py + 2, 2, 8, Palette::CinnabarRed); // Daenggi ribbon
        // Face & Shamanic makeup
        fillRect(px + 5, py + 3, 6, 4, Color(255, 230, 210));
        setPixel(px + 6, py + 4, Palette::RoyalPurple);
        setPixel(px + 9, py + 4, Palette::RoyalPurple);
        setPixel(px + 7, py + 6, Palette::CinnabarRed);
        // Vibrant Shamanic Wonsam (오색 원삼: 황/녹/적)
        fillRect(px + 4, py + 7, 8, 7, Palette::GardeniaYellow);
        fillRect(px + 3, py + 8, 2, 5, Palette::BambooGreen);
        // Sacred Divine Blade (오색 신칼)
        fillRect(px + 12, py + 5, 2, 8, Palette::White);
        setPixel(px + 12, py + 4, Palette::BichuiJade);
    } else if (spriteId == 7) {
        // [7] Village Teacher (서당 훈장 - 유건, 청백색 도포, 긴 수염, 회초리)
        fillRect(px + 5, py, 6, 3, Palette::SongyeonInk); // Yugeon Hat
        fillRect(px + 5, py + 3, 6, 4, Color(245, 215, 175));
        setPixel(px + 6, py + 4, Palette::SongyeonInk);
        setPixel(px + 9, py + 4, Palette::SongyeonInk);
        fillRect(px + 6, py + 6, 4, 4, Palette::BaegokWhite); // Long Sage Beard
        // Scholar Robe (청백색 도포) & Bamboo Cane
        fillRect(px + 4, py + 7, 8, 7, Color(175, 205, 235));
        fillRect(px + 13, py + 5, 1, 9, Palette::BambooGreen); // Cane
    } else if (spriteId == 8) {
        // [8] Centipede Demon (지네귀신 - 칠흑/보랏빛 갑각, 8쌍 다리, 녹색 독니)
        fillRect(px + 5, py + 2, 6, 4, Palette::RoyalPurple); // Head
        setPixel(px + 5, py + 1, Palette::BambooGreen);       // Toxic Fangs
        setPixel(px + 10, py + 1, Palette::BambooGreen);
        setPixel(px + 6, py + 3, Palette::CinnabarRed);        // Crimson eyes
        setPixel(px + 9, py + 3, Palette::CinnabarRed);
        fillRect(px + 6, py + 6, 4, 8, Palette::SongyeonInk); // Segmented armor
        // 8-pairs of twitching legs
        for (int i = 0; i < 4; ++i) {
            setPixel(px + 4, py + 6 + i * 2, Palette::Purple);
            setPixel(px + 11, py + 6 + i * 2, Palette::Purple);
        }
    } else if (spriteId == 9) {
        // [9] Tree God Dangsan Moksin (당산목신 - 500년 거목, 금줄, 소원지, 황금 안광)
        fillRect(px + 2, py + 1, 12, 8, Palette::BambooGreen); // Mighty Foliage
        fillRect(px + 5, py + 9, 6, 6, Color(105, 65, 30));     // Ancient Trunk
        fillRect(px + 3, py + 8, 10, 2, Palette::GardeniaYellow); // Geumjul (Sacred straw rope)
        setPixel(px + 5, py + 9, Palette::BaegokWhite); // Paper prayer streamers
        setPixel(px + 8, py + 9, Palette::BaegokWhite);
        setPixel(px + 11, py + 9, Palette::BaegokWhite);
        // Benevolent Spirit Eyes
        setPixel(px + 6, py + 5, Palette::GoldHalo);
        setPixel(px + 9, py + 5, Palette::GoldHalo);
    } else if (spriteId == 10) {
        // [10] Cult Master Mukyeong (음양당 총수 묵영 - 흑자색 삿갓, 붉은 도깨비눈, 태극령)
        fillRect(px + 3, py + 1, 10, 3, Palette::SongyeonInk); // Wide Dark Bamboo Hat
        fillRect(px + 5, py + 4, 6, 3, Color(220, 200, 180)); // Shadowed Mask
        setPixel(px + 6, py + 5, Palette::CinnabarRed);        // Glowing Evil Eyes
        setPixel(px + 9, py + 5, Palette::CinnabarRed);
        fillRect(px + 3, py + 7, 10, 8, Palette::RoyalPurple); // Void Silk Robe
        // Taegeuk Bell Staff (태극령) & Shadow Wisp
        fillRect(px + 13, py + 2, 2, 12, Palette::LightGray);
        setPixel(px + 13, py + 1, Palette::CinnabarRed);
        setPixel(px + 14, py + 1, Palette::IndigoBlue);
        setPixel(px + 2, py + 12, Palette::Purple); // Dark Miasma
    } else if (spriteId == 11) {
        // [11] Ancient Chaos Beast (태고의 혼돈 흉수 - 5성 보스, 심연 코어, 6개 안광, 촉수)
        fillRect(px + 2, py + 1, 12, 13, Palette::SongyeonInk); // Void Mass
        fillRect(px + 4, py + 3, 8, 8, Palette::RoyalPurple);   // Corrupt Core
        setPixel(px + 5, py + 5, Palette::CinnabarRed);        // Multi-Eyes
        setPixel(px + 10, py + 5, Palette::CinnabarRed);
        setPixel(px + 6, py + 8, Palette::GoldHalo);
        setPixel(px + 9, py + 8, Palette::GoldHalo);
        setPixel(px + 7, py + 4, Palette::CinnabarRed);
        setPixel(px + 8, py + 4, Palette::CinnabarRed);
        // Chaos tendrils & Thunder
        setPixel(px + 1, py + 3, Palette::Purple);
        setPixel(px + 14, py + 3, Palette::Purple);
        setPixel(px + 1, py + 12, Palette::GoldHalo);
        setPixel(px + 14, py + 12, Palette::GoldHalo);
    } else if (spriteId == 12) {
        // [12] Dokkaebi #001 (도깨비 - 2개 황금 뿔, 붉은 피부, 호피무늬 허리띠, 목각 방망이)
        fillRect(px + 4, py + 2, 8, 7, Palette::CinnabarRed);
        setPixel(px + 5, py + 1, Palette::GardeniaYellow); // Horn L
        setPixel(px + 10, py + 1, Palette::GardeniaYellow); // Horn R
        setPixel(px + 6, py + 4, Palette::White); // Eye L
        setPixel(px + 9, py + 4, Palette::White); // Eye R
        fillRect(px + 5, py + 6, 6, 2, Palette::SongyeonInk); // Mischievous Grin
        setPixel(px + 6, py + 6, Palette::White); // Sharp Fang
        // Tiger-skin loincloth (호피무늬 점 3개)
        fillRect(px + 4, py + 9, 8, 5, Palette::TigerOrange);
        setPixel(px + 5, py + 10, Palette::SongyeonInk);
        setPixel(px + 8, py + 11, Palette::SongyeonInk);
        setPixel(px + 10, py + 10, Palette::SongyeonInk);
        // Spiked Ironwood Club (쇠징 박힌 방망이)
        fillRect(px + 12, py + 4, 3, 9, Color(130, 85, 40));
        setPixel(px + 12, py + 5, Palette::LightGray);
        setPixel(px + 14, py + 7, Palette::LightGray);
    } else if (spriteId == 13) {
        // [13] Gumiho #002 (구미호 - 백색 9갈래 부채꼴 꼬리, 눈가 붉은 아이라이너, 도깨비불)
        setPixel(px + 4, py + 1, Palette::White);
        setPixel(px + 5, py + 2, Palette::CinnabarRed); // Ear L
        setPixel(px + 11, py + 1, Palette::White);
        setPixel(px + 10, py + 2, Palette::CinnabarRed); // Ear R
        // Face & Fox-eye makeup
        fillRect(px + 5, py + 3, 6, 5, Palette::BaegokWhite);
        setPixel(px + 6, py + 5, Palette::CinnabarRed); // Eyes
        setPixel(px + 9, py + 5, Palette::CinnabarRed);
        setPixel(px + 5, py + 5, Palette::PeonyPink); // Eyeliner
        setPixel(px + 10, py + 5, Palette::PeonyPink);
        // Silk Hanbok
        fillRect(px + 4, py + 8, 8, 6, Palette::CinnabarRed);
        // 9-Tails Fan (아홉 꼬리 실루엣)
        setPixel(px + 1, py + 8, Palette::White);
        setPixel(px + 2, py + 9, Palette::White);
        setPixel(px + 0, py + 11, Palette::White);
        setPixel(px + 13, py + 8, Palette::White);
        setPixel(px + 14, py + 9, Palette::White);
        setPixel(px + 15, py + 11, Palette::White);
        // Orbital Fox Fire (청색 도깨비불)
        setPixel(px + 13, py + 4, Palette::IndigoBlue);
        setPixel(px + 14, py + 3, Palette::BichuiJade);
    } else if (spriteId == 14) {
        // [14] Bulgasari #003 (불가사리 - 쇳빛 코끼리 코, 등 뒤의 쇠칼 파편, 화약 증기)
        fillRect(px + 3, py + 2, 10, 10, Color(80, 85, 95)); // Iron Flesh
        fillRect(px + 5, py + 4, 6, 2, Palette::CinnabarRed); // Furnace Eyes
        fillRect(px + 7, py + 7, 2, 6, Color(55, 60, 70));   // Snout/Trunk
        fillRect(px + 2, py + 11, 4, 4, Color(45, 50, 58));  // Sturdy Legs
        fillRect(px + 10, py + 11, 4, 4, Color(45, 50, 58));
        // Embedded Broken Blades on back (등의 쇠칼 파편)
        setPixel(px + 3, py + 1, Palette::LightGray);
        setPixel(px + 11, py + 1, Palette::LightGray);
        setPixel(px + 8, py + 13, Palette::TigerOrange); // Flame steam from nose
    } else if (spriteId == 15) {
        // [15] Cheonmyeong-Yeongho #108 (천명영호 - 전설의 영수, 삼태극 영륜, 찬란한 옥빛 오라)
        // Shimmering White-Gold Celestial Fox
        fillRect(px + 4, py + 2, 8, 7, Palette::BaegokWhite);
        fillRect(px + 5, py + 1, 2, 2, Palette::GardeniaYellow); // Horn/Ears
        fillRect(px + 9, py + 1, 2, 2, Palette::GardeniaYellow);
        setPixel(px + 6, py + 4, Palette::BichuiJade); // Divine Jade Eyes
        setPixel(px + 9, py + 4, Palette::BichuiJade);
        fillRect(px + 3, py + 9, 10, 5, Palette::BaegokWhite);
        // Sam-Taegeuk Halo (삼태극 영륜: 적/청/황)
        setPixel(px + 7, py, Palette::CinnabarRed);
        setPixel(px + 8, py, Palette::IndigoBlue);
        setPixel(px + 7, py - 1, Palette::GardeniaYellow);
        setPixel(px + 8, py - 1, Palette::GoldHalo);
        // Celestial Tails with Jade Auras
        setPixel(px + 1, py + 8, Palette::BichuiJade);
        setPixel(px + 14, py + 8, Palette::BichuiJade);
        setPixel(px + 2, py + 12, Palette::GoldHalo);
        setPixel(px + 13, py + 12, Palette::GoldHalo);
    } else if (spriteId == 16) {
        // [16] Ferryman NPC (나룻배 뱃사공 - 갈색 삿갓, 무명 두루마기, 긴 노)
        fillRect(px + 3, py + 1, 10, 2, Color(160, 110, 50)); // Wide Satgat bamboo hat
        fillRect(px + 5, py, 6, 2, Color(130, 85, 35));
        // Sun-tanned Face
        fillRect(px + 5, py + 3, 6, 4, Color(225, 175, 130));
        setPixel(px + 6, py + 4, Palette::SongyeonInk);
        setPixel(px + 9, py + 4, Palette::SongyeonInk);
        // Commoner Hanbok & Indigo Sash
        fillRect(px + 4, py + 7, 8, 7, Palette::BaegokWhite);
        fillRect(px + 4, py + 9, 8, 2, Palette::IndigoBlue); // Waist sash
        // Long Wooden Oar (긴 삿대 노)
        fillRect(px + 13, py + 2, 2, 13, Color(140, 95, 45));
        setPixel(px + 12, py + 14, Color(170, 120, 60));
    } else if (spriteId == 17) {
        // [17] Village Elder / Scholar (마을 원로 백발 노인 - 탕건, 순백 도포, 흰 수염)
        fillRect(px + 5, py + 1, 6, 2, Palette::SongyeonInk); // Tanggeon horsehair hat
        // Face & Gentle Sage Eyes
        fillRect(px + 5, py + 3, 6, 4, Color(245, 220, 185));
        setPixel(px + 6, py + 4, Palette::SongyeonInk);
        setPixel(px + 9, py + 4, Palette::SongyeonInk);
        fillRect(px + 6, py + 6, 4, 3, Palette::BaegokWhite); // Long White Beard
        // Pure White Silk Robe
        fillRect(px + 4, py + 7, 8, 7, Palette::BaegokWhite);
        setPixel(px + 7, py + 8, Palette::BichuiJade); // Jade Norigae pendant
    }
}

void Renderer::applyPostProcess(bool crtScanlines, bool vignette) {
    if (!crtScanlines && !vignette) return;

    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        bool isScanline = crtScanlines && ((y % 2) == 1);
        float dy = (y - (SCREEN_HEIGHT / 2.0f)) / (SCREEN_HEIGHT / 2.0f);

        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            float mult = 1.0f;

            if (isScanline) {
                mult *= 0.82f; // Subtle scanline darkening
            }

            if (vignette) {
                float dx = (x - (SCREEN_WIDTH / 2.0f)) / (SCREEN_WIDTH / 2.0f);
                float distSq = dx * dx + dy * dy;
                if (distSq > 0.6f) {
                    mult *= (1.0f - (distSq - 0.6f) * 0.35f);
                }
            }

            if (mult < 0.999f) {
                uint32_t c = m_framebuffer[y * SCREEN_WIDTH + x];
                uint8_t b = static_cast<uint8_t>((c & 0xFF) * mult);
                uint8_t g = static_cast<uint8_t>(((c >> 8) & 0xFF) * mult);
                uint8_t r = static_cast<uint8_t>(((c >> 16) & 0xFF) * mult);
                m_framebuffer[y * SCREEN_WIDTH + x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
            }
        }
    }
}

void Renderer::applyFade(float brightness) {
    if (brightness >= 0.999f) return;
    brightness = std::clamp(brightness, 0.0f, 1.0f);
    for (size_t i = 0; i < m_framebuffer.size(); ++i) {
        uint32_t c = m_framebuffer[i];
        uint8_t b = static_cast<uint8_t>((c & 0xFF) * brightness);
        uint8_t g = static_cast<uint8_t>(((c >> 8) & 0xFF) * brightness);
        uint8_t r = static_cast<uint8_t>(((c >> 16) & 0xFF) * brightness);
        m_framebuffer[i] = (0xFF << 24) | (r << 16) | (g << 8) | b;
    }
}

} // namespace JoseonRPG
