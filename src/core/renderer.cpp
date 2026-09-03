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
