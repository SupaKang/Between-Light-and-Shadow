// 4-colour art (Pokemon gen-1 / Super Game Boy style): every sprite uses one 4-shade ramp.
// Row digits: 0 = lightest .. 3 = ink, '.' = transparent. Field characters are 16x16.
#pragma once
#include "gfx.h"

namespace art {

using gfx::Color;
// Ghost-story palette (black / greens, rare blood red). Ramps: {light, mid, accent/dark, darkest}.
inline const Color GHOST[16] = {0x040605, 0x0a120c, 0x12201a, 0x1c3426, 0x2a4c34, 0x3e6e44, 0x62a05a, 0xa8e08a,
                                0xd8e8c8, 0x9aa890, 0x5e6a5a, 0x3a1216, 0x7a1c1c, 0xc0b494, 0x86785e, 0x22262c};
inline const Color RAMP_UI[4] = {0xd8e8c8, 0x62a05a, 0xa83a2e, 0x2a4c34};
inline const Color RAMP_HERO[4] = {0xd8e8c8, 0xc0b494, 0x8a2a24, 0x040605};
inline const Color RAMP_ELDER[4] = {0xd8e8c8, 0xc0b494, 0x5e6a5a, 0x040605};
inline const Color RAMP_JUMO[4] = {0xd8e8c8, 0xc0b494, 0x3a1216, 0x040605};
inline const Color RAMP_MONK[4] = {0xd8e8c8, 0x9aa890, 0x3e6e44, 0x040605};
inline const Color RAMP_WOOD[4] = {0x9aa890, 0x5e6a5a, 0x1c3426, 0x040605};
inline const Color RAMP_FLAME[4] = {0xd8f8c0, 0xa8e08a, 0x3e6e44, 0x040605};
inline const Color RAMP_GRASS[4] = {0x3e6e44, 0x2a4c34, 0x1c3426, 0x040605};
inline const Color RAMP_EARTH[4] = {0x9aa890, 0x5e6a5a, 0x2e3a2c, 0x040605};
inline const Color RAMP_STONE[4] = {0x9aa890, 0x5e6a5a, 0x1c3426, 0x040605};
inline const Color RAMP_NIGHT[4] = {0xd8e8c8, 0x3e6e44, 0x12201a, 0x040605};

#define ART_SPRITE(name, w, h, ramp, ...) \
    inline const char* const name##_rows[] = {__VA_ARGS__}; \
    static_assert(sizeof(name##_rows) / sizeof(char*) == h, #name " row count"); \
    inline const gfx::Sprite name{w, h, name##_rows, ramp};

ART_SPRITE(jangseung, 16, 33, RAMP_WOOD,
    ".....333333.....",
    "....32222223....",
    "..333333333333..",
    "...3111111113...",
    "...3111111113...",
    "...3003113003...",
    "...3033113303...",
    "...3111221113...",
    "...3122222213...",
    "...3120000213...",
    "...3122222213...",
    "...3111111113...",
    "....31111113....",
    "....31111113....",
    "....31133113....",
    "....31111113....",
    "....31111113....",
    "....31133113....",
    "....31111113....",
    "....31111113....",
    "....31133113....",
    "....31111113....",
    "....31111113....",
    "....31133113....",
    "....31111113....",
    "....31111113....",
    "....31133113....",
    "....31111113....",
    "....31111113....",
    "....31111113....",
    "...3322222233...",
    "..333333333333..",
    "................")

ART_SPRITE(lamp_a, 8, 12, RAMP_FLAME,
    "...0....",
    "..010...",
    "..101...",
    "...1....",
    "...3....",
    "..3223..",
    ".322223.",
    ".322223.",
    "..3223..",
    "...33...",
    "..3223..",
    ".333333.")

ART_SPRITE(lamp_b, 8, 12, RAMP_FLAME,
    "....0...",
    "...01...",
    "..101...",
    "...1....",
    "...3....",
    "..3223..",
    ".322223.",
    ".322223.",
    "..3223..",
    "...33...",
    "..3223..",
    ".333333.")

ART_SPRITE(icon_yokai, 9, 9, RAMP_UI,
    "1.......1",
    "31.....13",
    ".3222223.",
    "322222223",
    "320232023",
    "322222223",
    "322000223",
    "322222223",
    ".3333333.")

ART_SPRITE(icon_book, 9, 9, RAMP_UI,
    ".33333333",
    "333333303",
    "332222303",
    "333333303",
    "333333303",
    "333333303",
    "333333303",
    "333333303",
    ".00000003")

ART_SPRITE(icon_pouch, 9, 9, RAMP_UI,
    "...333...",
    "..32123..",
    "...323...",
    "..31113..",
    ".3111113.",
    "311111113",
    "311111113",
    ".3111113.",
    "..33333..")

ART_SPRITE(icon_taeguk, 9, 9, RAMP_UI,
    "..33333..",
    ".3222223.",
    "322222223",
    "322223333",
    "322333333",
    "323333333",
    "333333333",
    ".3333333.",
    "..33333..")

ART_SPRITE(icon_brush, 9, 9, RAMP_UI,
    ".......33",
    "......313",
    ".....313.",
    "....313..",
    "...313...",
    "..333....",
    ".3333....",
    "333......",
    "33.......")

ART_SPRITE(icon_knot, 9, 9, RAMP_UI,
    "...222...",
    "..2...2..",
    "..2...2..",
    "...222...",
    ".2222222.",
    "...222...",
    "..2.2.2..",
    ".2..2..2.",
    "2...2...2")

ART_SPRITE(icon_close, 9, 9, RAMP_UI,
    ".........",
    ".3.....3.",
    "..3...3..",
    "...3.3...",
    "....3....",
    "...3.3...",
    "..3...3..",
    ".3.....3.",
    ".........")

ART_SPRITE(icon_staff, 9, 9, RAMP_UI,
    ".....111.",
    ".....1.1.",
    ".....111.",
    "....1....",
    "...1.....",
    "..1......",
    ".1.......",
    "1........",
    ".........")

ART_SPRITE(icon_talisman, 9, 9, RAMP_UI,
    ".3333333.",
    ".3111113.",
    ".3122213.",
    ".3111113.",
    ".3121213.",
    ".3112113.",
    ".3121213.",
    ".3111113.",
    ".3333333.")

ART_SPRITE(icon_feet, 9, 9, RAMP_UI,
    ".........",
    "...33....",
    "..3113...",
    "..3113...",
    "...33.33.",
    ".....3113",
    ".....3113",
    "......33.",
    ".........")

ART_SPRITE(ui_cursor, 5, 7, RAMP_UI,
    "3....",
    "323..",
    "3223.",
    "32223",
    "3223.",
    "323..",
    "3....")

#undef ART_SPRITE
}  // namespace art
