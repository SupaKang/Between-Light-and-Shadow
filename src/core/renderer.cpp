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

void Renderer::drawPanel(int x, int y, int w, int h, Color bg, Color border) {
    fillRect(x, y, w, h, bg);
    drawRect(x, y, w, h, border);
    drawRect(x + 1, y + 1, w - 2, h - 2, Palette::Black);
}

void Renderer::drawHealthBar(int x, int y, int w, int h, int curVal, int maxVal, Color fillColor, Color bgColor) {
    fillRect(x, y, w, h, bgColor);
    drawRect(x, y, w, h, Palette::Black);
    if (maxVal <= 0) return;
    float ratio = std::clamp(static_cast<float>(curVal) / maxVal, 0.0f, 1.0f);
    int fillW = static_cast<int>((w - 2) * ratio);
    if (fillW > 0) {
        fillRect(x + 1, y + 1, fillW, h - 2, fillColor);
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
        case 6: // Water / Stream / Sea (시냇물 / 바다)
            fillRect(px, py, 16, 16, Color(30, 80, 150));
            drawLine(px + 2, py + 4, px + 7, py + 4, Color(70, 130, 210));
            drawLine(px + 9, py + 10, px + 14, py + 10, Color(70, 130, 210));
            break;
        case 7: // Wooden Bridge (나무 다리)
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
        case 9: // Dark Obsidian Altar (음양당 흑요석 제단)
            fillRect(px, py, 16, 16, Color(24, 20, 32));
            drawRect(px + 2, py + 2, 12, 12, Color(90, 40, 120));
            fillRect(px + 6, py + 6, 4, 4, Palette::Red);
            break;
        case 10: // Treasure Chest (보물 상자)
            fillRect(px, py, 16, 16, Color(42, 100, 52)); // grass base
            fillRect(px + 2, py + 4, 12, 10, Color(180, 130, 40));
            drawRect(px + 2, py + 4, 12, 10, Color(90, 60, 20));
            fillRect(px + 6, py + 7, 4, 3, Palette::Yellow);
            break;
        case 11: // Hongsalmun Gate (홍살문 / 관문)
            fillRect(px, py, 16, 16, Color(160, 130, 90)); // dirt base
            fillRect(px + 1, py + 1, 3, 14, Palette::Red);
            fillRect(px + 12, py + 1, 3, 14, Palette::Red);
            fillRect(px + 1, py + 3, 14, 3, Palette::Red);
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
    } else if (spriteId == 2) {
        // Gumiho (구미호 - 흰/붉은 여우귀, 붉은 눈, 아홉 꼬리 실루엣)
        setPixel(px + 4, py + 1, Palette::White);
        setPixel(px + 5, py + 2, Palette::Red); // Ear L
        setPixel(px + 11, py + 1, Palette::White);
        setPixel(px + 10, py + 2, Palette::Red); // Ear R

        fillRect(px + 5, py + 3, 6, 5, Color(250, 240, 230)); // Face
        setPixel(px + 6, py + 5, Palette::Red); // Eye L
        setPixel(px + 9, py + 5, Palette::Red); // Eye R

        fillRect(px + 4, py + 8, 8, 6, Palette::Red); // Hanbok robe
        // Tails
        setPixel(px + 1, py + 9, Palette::White);
        setPixel(px + 2, py + 10, Palette::White);
        setPixel(px + 13, py + 9, Palette::White);
        setPixel(px + 14, py + 10, Palette::White);
        // Fox fire
        setPixel(px + 13, py + 4, Palette::Blue);
    } else if (spriteId == 3) {
        // Cheonyeogwishin (처녀귀신 - 긴 산발머리, 소복, 푸른 기운)
        fillRect(px + 4, py + 1, 8, 12, Palette::InkBlack); // Hair
        fillRect(px + 6, py + 4, 4, 4, Color(220, 235, 235)); // Pale face
        setPixel(px + 7, py + 5, Palette::Red); // Ghost eye
        fillRect(px + 4, py + 8, 8, 7, Palette::White); // White Sobok
        setPixel(px + 3, py + 12, Palette::Blue); // Ghostly mist
        setPixel(px + 12, py + 11, Palette::Blue);
    } else if (spriteId == 4) {
        // Bulgasari (불가사리 - 쇳빛 몸체, 코끼리 코)
        fillRect(px + 3, py + 2, 10, 10, Color(80, 85, 95)); // Iron Body
        fillRect(px + 5, py + 4, 6, 2, Palette::Red); // Glowing Eyes
        fillRect(px + 7, py + 7, 2, 6, Color(60, 65, 75)); // Trunk/Snout
        fillRect(px + 2, py + 11, 4, 4, Color(50, 55, 60)); // Leg L
        fillRect(px + 10, py + 11, 4, 4, Color(50, 55, 60)); // Leg R
    } else if (spriteId == 5) {
        // Jeoseungsaja (저승사자 - 검은 갓, 검은 도포, 붉은 명부)
        fillRect(px + 2, py + 1, 12, 2, Palette::Black); // Wide Gat Brim
        fillRect(px + 5, py, 6, 2, Palette::Black);
        fillRect(px + 5, py + 3, 6, 4, Color(210, 220, 225)); // Pale ghost face
        setPixel(px + 6, py + 4, Palette::Red); // Crimson eyes
        setPixel(px + 9, py + 4, Palette::Red);
        fillRect(px + 4, py + 7, 8, 8, Palette::InkBlack); // Black Robe
        fillRect(px + 12, py + 8, 3, 4, Palette::Red); // Death Scroll
    } else if (spriteId == 6) {
        // Mountain Tiger (호랑이 산신 - 황금 털, 검은 줄무늬, 흰 수염)
        fillRect(px + 3, py + 2, 10, 8, Color(220, 140, 30)); // Tiger face
        fillRect(px + 4, py + 1, 2, 2, Color(160, 90, 10)); // Ear L
        fillRect(px + 10, py + 1, 2, 2, Color(160, 90, 10)); // Ear R
        setPixel(px + 5, py + 4, Palette::Yellow); // Glowing eyes
        setPixel(px + 10, py + 4, Palette::Yellow);
        fillRect(px + 6, py + 2, 4, 2, Palette::Black); // '王' Head mark
        fillRect(px + 7, py + 3, 2, 3, Palette::Black);
        fillRect(px + 5, py + 8, 6, 2, Palette::White); // Whiskers/Fangs
        fillRect(px + 2, py + 10, 12, 5, Color(210, 130, 20)); // Body
        setPixel(px + 4, py + 11, Palette::Black); // Stripes
        setPixel(px + 8, py + 11, Palette::Black);
        setPixel(px + 11, py + 11, Palette::Black);
    } else if (spriteId == 7) {
        // Imoogi / Dragon (이무기 - 푸른 비늘, 황금 눈, 여의주)
        fillRect(px + 4, py + 2, 8, 6, Color(35, 110, 160)); // Dragon head
        setPixel(px + 5, py + 1, Color(210, 180, 50)); // Horn L
        setPixel(px + 10, py + 1, Color(210, 180, 50)); // Horn R
        setPixel(px + 6, py + 4, Palette::Yellow); // Eyes
        setPixel(px + 9, py + 4, Palette::Yellow);
        fillRect(px + 3, py + 8, 10, 5, Color(25, 90, 140)); // Serpentine body
        fillRect(px + 5, py + 13, 6, 3, Color(20, 70, 120));
        fillRect(px + 12, py + 6, 3, 3, Palette::Yellow); // Yeouiju (Dragon Pearl)
        setPixel(px + 13, py + 7, Palette::White);
    } else if (spriteId == 8) {
        // Centipede Demon (지네귀신 - 보랏빛 몸체, 다지류 다리, 독니)
        fillRect(px + 5, py + 2, 6, 4, Color(110, 30, 130)); // Head
        setPixel(px + 5, py + 1, Palette::Green); // Toxic fangs
        setPixel(px + 10, py + 1, Palette::Green);
        fillRect(px + 6, py + 6, 4, 8, Color(90, 20, 110)); // Segment body
        // Segmented legs
        for (int i = 0; i < 4; ++i) {
            setPixel(px + 4, py + 6 + i * 2, Color(160, 40, 180));
            setPixel(px + 11, py + 6 + i * 2, Color(160, 40, 180));
        }
    } else if (spriteId == 9) {
        // Tree God / Dangsan Moksin (목신 - 거목 줄기, 잎사귀, 금줄)
        fillRect(px + 3, py + 1, 10, 8, Color(30, 120, 50)); // Tree Crown
        fillRect(px + 5, py + 9, 6, 6, Color(110, 70, 35)); // Trunk
        fillRect(px + 4, py + 8, 8, 2, Palette::Yellow); // Sacred Geumjul Rope
        setPixel(px + 6, py + 9, Palette::White); // Paper prayer streamer
        setPixel(px + 9, py + 9, Palette::White);
    } else if (spriteId == 10) {
        // Eumyang-dang Cult Master Mukyeong (음양당 총수 묵영 - 흑자색 도포, 태극령)
        fillRect(px + 4, py + 1, 8, 3, Palette::InkBlack); // Hood/Hat
        fillRect(px + 5, py + 4, 6, 3, Color(230, 210, 190)); // Masked Face
        setPixel(px + 6, py + 5, Palette::Red); // Glowing Evil Eyes
        setPixel(px + 9, py + 5, Palette::Purple);
        fillRect(px + 3, py + 7, 10, 8, Color(60, 20, 80)); // Shadow Robe
        // Yin-Yang Staff
        fillRect(px + 13, py + 2, 2, 12, Palette::LightGray);
        setPixel(px + 13, py + 1, Palette::Red);
        setPixel(px + 14, py + 1, Palette::Blue);
    } else if (spriteId == 11) {
        // Ancient Chaos Beast (태고의 혼돈 흉수 - 거대 암흑 아우라, 붉은 촉수)
        fillRect(px + 2, py + 1, 12, 13, Palette::InkBlack); // Void Body
        fillRect(px + 4, py + 3, 8, 8, Color(70, 10, 30)); // Corrupt Core
        setPixel(px + 5, py + 5, Palette::Red); // Multi-Eyes
        setPixel(px + 10, py + 5, Palette::Red);
        setPixel(px + 7, py + 8, Palette::Yellow);
        setPixel(px + 8, py + 8, Palette::Yellow);
        // Chaos tendrils
        setPixel(px + 1, py + 3, Palette::Purple);
        setPixel(px + 14, py + 3, Palette::Purple);
        setPixel(px + 1, py + 12, Palette::Purple);
        setPixel(px + 14, py + 12, Palette::Purple);
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

} // namespace JoseonRPG
