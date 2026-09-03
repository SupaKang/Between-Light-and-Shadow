#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <string_view>
#include <algorithm>

namespace JoseonRPG {

constexpr int SCREEN_WIDTH = 320;
constexpr int SCREEN_HEIGHT = 180;
constexpr int TILE_SIZE = 16;
constexpr int MAX_PARTY_SIZE = 3;
constexpr int MAX_SKILLS = 4;
constexpr int MAX_ARTIFACTS = 8;
constexpr int MAX_LEVEL = 50;

struct Color {
    uint8_t b, g, r, a;

    constexpr Color() : b(0), g(0), r(0), a(255) {}
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : b(b), g(g), r(r), a(a) {}

    uint32_t toARGB() const {
        return (static_cast<uint32_t>(a) << 24) |
               (static_cast<uint32_t>(r) << 16) |
               (static_cast<uint32_t>(g) << 8)  |
               static_cast<uint32_t>(b);
    }
};

namespace Palette {
    constexpr Color Black{18, 18, 20};
    constexpr Color DarkGray{34, 36, 43};
    constexpr Color MidGray{80, 84, 98};
    constexpr Color LightGray{180, 184, 195};
    constexpr Color White{250, 248, 245};
    
    // Obangsaek (Korean Traditional 5 Colors) & Dancheong Mineral Pigments
    constexpr Color Blue{43, 108, 196};
    constexpr Color Red{179, 42, 42};
    constexpr Color Yellow{212, 162, 55};
    constexpr Color Green{48, 140, 78};
    constexpr Color Jade{92, 180, 150};
    constexpr Color HanjiBackground{237, 234, 225};
    constexpr Color InkBlack{20, 20, 24};
    constexpr Color Purple{128, 64, 160};

    // Authentic Dancheong Pigments
    constexpr Color IndigoBlue{30, 75, 130};
    constexpr Color CinnabarRed{184, 38, 38};
    constexpr Color GardeniaYellow{230, 180, 34};
    constexpr Color BaegokWhite{237, 232, 208};
    constexpr Color SongyeonInk{24, 24, 32};
    constexpr Color BichuiJade{42, 157, 143};
    constexpr Color PeonyPink{231, 111, 81};
    constexpr Color RoyalPurple{74, 21, 75};
    constexpr Color TigerOrange{211, 84, 0};
    constexpr Color GoldHalo{244, 208, 63};
    constexpr Color BambooGreen{39, 174, 96};
}

struct Vec2i {
    int x = 0;
    int y = 0;
};

struct Recti {
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
};

enum class YokaiGrade : uint8_t {
    Grade1 = 1, // Common (하급)
    Grade2 = 2, // Uncommon (중급)
    Grade3 = 3, // Rare (상급)
    Grade4 = 4, // Epic (영수/대요괴)
    Grade5 = 5  // Mythic/Divine (신령/환수)
};

enum class Element : uint8_t {
    Physical,
    Fire,   // 화 (火)
    Water,  // 수 (水)
    Earth,  // 토 (土)
    Wind,   // 목/풍 (木/風)
    Light,  // 양/신 (陽/神)
    Dark    // 음/귀 (陰/鬼)
};

enum class StatusEffect : uint8_t {
    None,
    Burn,       // 화상: 공격 기술 사용 시 자가 피해
    Freeze,     // 빙결: 1~3턴 행동 불가, 피격 시 해제
    Seal,       // 봉인: 마지막 사용 기술 봉인
    Paralysis,  // 마비: 33% 행동 실패, 3~5턴 지속
    Fear        // 공포: 공격력 감소 + 받는 피해 증가
};

} // namespace JoseonRPG
