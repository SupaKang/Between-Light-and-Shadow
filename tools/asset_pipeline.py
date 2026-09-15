#!/usr/bin/env python3
"""
tools/asset_pipeline.py - 108: Yin-Yang Chronicle High-Density Art Pipeline
Generates 4bpp indexed RLE compressed C++ sprite headers for tiles and characters
using the traditional Dancheong 16-color palette.
"""

import os
import sys

# Dancheong 16-Color Palette (ARGB uint32)
PALETTE_DANCHEONG_16 = [
    (0x00000000, "Transparent/Key"),
    (0xFF121214, "Ink Black / Outline"),
    (0xFF2A2E28, "Deep Charcoal"),
    (0xFF5A6068, "Tile Slate Gray"),
    (0xFFFAF8F5, "Hanji Paper White"),
    (0xFFF0D4B2, "Peach Skin"),
    (0xFFB83020, "Dancheong Vermilion"),
    (0xFFE85D5D, "Coral Red / Aura"),
    (0xFF1C4828, "Deep Dancheong Green"),
    (0xFF38804A, "Jade Green"),
    (0xFF1C3B5E, "Deep Indigo"),
    (0xFF4D88D8, "Han Sky Blue"),
    (0xFFD4A237, "Dancheong Gold Ochre"),
    (0xFF8A5D3B, "Clay Wood Brown"),
    (0xFF542572, "Imperial Violet"),
    (0xFFFCE278, "Straw / Talisman Yellow")
]

class PixelCanvas:
    def __init__(self, w, h, bg=0):
        self.w = w
        self.h = h
        self.grid = [bg] * (w * h)

    def set(self, x, y, color):
        if 0 <= x < self.w and 0 <= y < self.h:
            self.grid[y * self.w + x] = color

    def get(self, x, y):
        if 0 <= x < self.w and 0 <= y < self.h:
            return self.grid[y * self.w + x]
        return 0

    def rect(self, x, y, w, h, color):
        for cy in range(y, y + h):
            for cx in range(x, x + w):
                self.set(cx, cy, color)

    def stroke_rect(self, x, y, w, h, color):
        for cx in range(x, x + w):
            self.set(cx, y, color)
            self.set(cx, y + h - 1, color)
        for cy in range(y, y + h):
            self.set(x, cy, color)
            self.set(x + w - 1, cy, color)

    def circle(self, cx, cy, r, color):
        for y in range(cy - r, cy + r + 1):
            for x in range(cx - r, cx + r + 1):
                if (x - cx) ** 2 + (y - cy) ** 2 <= r ** 2:
                    self.set(x, y, color)

    def compress_rle(self):
        bytes_out = []
        n = len(self.grid)
        i = 0
        while i < n:
            col = self.grid[i]
            run = 1
            while i + run < n and self.grid[i + run] == col and run < 16:
                run += 1
            byte_val = ((run - 1) << 4) | (col & 0x0F)
            bytes_out.append(byte_val)
            i += run
        return bytes_out


def create_tile_village_grass():
    c = PixelCanvas(32, 32, bg=9) # Jade green base
    # Deep green grass blades and texture
    for y in range(32):
        for x in range(32):
            if (x * 7 + y * 13 + (x ^ y)) % 5 == 0:
                c.set(x, y, 8) # Deep green shadow blade
            elif (x * 11 + y * 17) % 19 == 0:
                c.set(x, y, 15) # Tiny yellow wildflower / straw fleck
            elif (x + y) % 8 == 0:
                c.set(x, y, 8)
    # Subtle grass tufts
    for (gx, gy) in [(4, 6), (18, 10), (8, 20), (22, 24), (12, 28)]:
        c.set(gx, gy, 8)
        c.set(gx + 1, gy - 1, 8)
        c.set(gx + 1, gy, 8)
        c.set(gx + 2, gy - 2, 9)
        c.set(gx + 1, gy - 2, 15) # Blossom dot
    return c

def create_tile_village_path():
    c = PixelCanvas(32, 32, bg=13) # Clay wood brown base
    # Dirt texture and stones
    for y in range(32):
        for x in range(32):
            val = (x * 17 + y * 23 + (x * y)) % 11
            if val in (1, 2):
                c.set(x, y, 12) # Ochre earth highlight
            elif val in (3, 4):
                c.set(x, y, 2) # Deep charcoal dirt fissure
    # River pebbles embedded in path
    pebbles = [(6, 8, 3), (7, 8, 2), (20, 14, 3), (21, 14, 2), (12, 22, 3), (26, 6, 3), (15, 27, 4)]
    for (px, py, col) in pebbles:
        c.set(px, py, col)
        c.set(px, py + 1, 2)
    return c

def create_tile_mountain_rock():
    c = PixelCanvas(32, 32, bg=3) # Slate gray base
    # Mountain granite rock crags
    for y in range(32):
        for x in range(32):
            if (x * 13 + y * 31) % 7 == 0:
                c.set(x, y, 2) # Deep charcoal crag
            elif (x * 19 + y * 7) % 13 == 0:
                c.set(x, y, 4) # Quartz vein / highlight
            elif (x * 3 + y * 5) % 17 == 0:
                c.set(x, y, 8) # High-altitude mountain moss
    # Diagonal rock fault lines
    for i in range(12):
        c.set(4 + i, 10 + i // 2, 1) # Outline crack
        c.set(16 + i, 20 + i // 2, 1)
    return c

def create_tile_temple_stone():
    c = PixelCanvas(32, 32, bg=3) # Dressed stone slate
    # Flagstone borders
    c.stroke_rect(0, 0, 32, 32, 2) # Charcoal mortar joint
    c.stroke_rect(1, 1, 30, 30, 1) # Dark outline
    # Dancheong geometric center inlay
    c.rect(6, 6, 20, 20, 2)
    c.rect(8, 8, 16, 16, 3)
    c.rect(12, 12, 8, 8, 6) # Dancheong vermilion center seal
    c.rect(14, 14, 4, 4, 12) # Gold dot in center
    # Slate texture
    for y in range(2, 6):
        for x in range(2, 30):
            if (x + y) % 3 == 0: c.set(x, y, 4) # Polished highlight
    return c

def create_hero_field():
    c = PixelCanvas(32, 32, bg=0)
    # Joseon Exorcist Scholar (32x32 SD)
    # Gat (Black hat) - brim and crown
    c.rect(8, 2, 16, 3, 1) # Hat crown
    c.rect(4, 5, 24, 2, 1) # Wide brim
    c.rect(10, 3, 12, 2, 2) # Crown highlight
    # Hat string (Gatkeun)
    c.set(9, 7, 10); c.set(9, 8, 10); c.set(10, 9, 10)
    c.set(22, 7, 10); c.set(22, 8, 10); c.set(21, 9, 10)
    # Face
    c.rect(11, 7, 10, 8, 5) # Peach face
    c.set(13, 10, 1); c.set(14, 10, 1) # Left eye
    c.set(17, 10, 1); c.set(18, 10, 1) # Right eye
    c.set(15, 12, 6) # Determined mouth/nose
    # Dopo / Hanbok Body
    c.rect(9, 15, 14, 11, 4) # Hanji white robe
    c.rect(11, 15, 10, 7, 10) # Deep indigo Jeonbok vest
    c.rect(13, 15, 6, 5, 4) # White inner collar V-neck
    c.rect(10, 20, 12, 2, 11) # Sky blue belt / sash
    # Exorcism Talisman at waist
    c.rect(20, 19, 3, 5, 15) # Yellow paper
    c.set(21, 20, 6); c.set(21, 22, 6) # Red cinnabar characters
    # Hands
    c.rect(7, 18, 3, 3, 5) # Left hand
    c.rect(22, 18, 3, 3, 5) # Right hand
    # Pants / Shoes
    c.rect(11, 26, 4, 3, 4) # Left white sock
    c.rect(17, 26, 4, 3, 4) # Right white sock
    c.rect(10, 29, 5, 2, 1) # Left black shoe
    c.rect(17, 29, 5, 2, 1) # Right black shoe
    return c

def create_jumo_field():
    c = PixelCanvas(32, 32, bg=0)
    # Tavern Keeper (주모)
    # Gache hair bun
    c.circle(16, 5, 4, 1) # Black hair bun
    c.rect(13, 7, 6, 2, 1)
    c.rect(18, 4, 3, 1, 9) # Jade hairpin (비녀)
    # Face
    c.rect(11, 8, 10, 7, 5) # Face
    c.set(13, 10, 1); c.set(18, 10, 1) # Eyes
    c.set(12, 11, 7); c.set(19, 11, 7) # Rosy cheeks
    c.set(15, 12, 6); c.set(16, 12, 6) # Lips
    # Jeogori Jacket (Pale yellow/straw with vermilion collar)
    c.rect(10, 15, 12, 5, 15) # Straw yellow jacket
    c.rect(14, 15, 4, 3, 6) # Vermilion collar band
    c.set(15, 18, 11); c.set(15, 19, 11) # Blue ribbon tie (고름)
    # Chima Skirt (Crimson vermilion voluminous skirt)
    c.rect(8, 20, 16, 10, 6)
    c.rect(6, 24, 20, 6, 6)
    for y in range(21, 30):
        c.set(11, y, 7); c.set(16, y, 7); c.set(21, y, 7) # Skirt folds highlight
    # White socks and shoes under skirt
    c.rect(11, 30, 4, 2, 4)
    c.rect(17, 30, 4, 2, 4)
    # Earthenware wine pot in hand
    c.circle(7, 21, 2, 13)
    c.set(7, 19, 1) # Pot neck
    return c

def create_shrine_field():
    c = PixelCanvas(32, 32, bg=0)
    # Ancient Sacred Tree (고대 신목)
    # Rich green leafy canopy
    c.circle(16, 9, 8, 8)
    c.circle(11, 9, 6, 9)
    c.circle(21, 9, 6, 9)
    c.circle(16, 5, 5, 9)
    # Leaf highlights
    for (lx, ly) in [(14, 4), (18, 6), (10, 8), (22, 8), (15, 11)]:
        c.rect(lx, ly, 2, 2, 15) # Sacred golden leaf glimmer
    # Mighty Gnarled Tree Trunk
    c.rect(12, 15, 8, 15, 13) # Clay wood brown trunk
    c.rect(10, 24, 12, 6, 13) # Trunk base flare
    # Bark fissures
    for y in range(16, 29):
        if y % 2 == 0:
            c.set(14, y, 2)
            c.set(17, y, 2)
    # Sacred Straw Rope (금줄) wrapped around trunk
    c.rect(11, 19, 10, 2, 12) # Golden straw rope
    c.rect(11, 20, 10, 1, 15)
    # Hanging white sacred paper streamers (백지)
    c.rect(12, 21, 2, 3, 4)
    c.rect(16, 21, 2, 4, 4)
    c.rect(19, 21, 2, 3, 4)
    # Ancient tree roots grasping earth
    c.rect(8, 29, 5, 3, 13)
    c.rect(19, 29, 5, 3, 13)
    return c

def create_signpost_field():
    c = PixelCanvas(32, 32, bg=0)
    # Mountain Wooden Signpost / Jangseung
    # Main post
    c.rect(14, 12, 4, 18, 13)
    c.rect(13, 28, 6, 4, 2) # Base stones
    # Wooden direction boards
    c.rect(6, 6, 20, 5, 13) # Top board
    c.stroke_rect(6, 6, 20, 5, 1)
    c.rect(8, 8, 4, 1, 12); c.rect(14, 8, 4, 1, 12); c.rect(20, 8, 3, 1, 12) # Engraved characters
    # Lower arrow board pointing up/down
    c.rect(8, 13, 16, 5, 12)
    c.stroke_rect(8, 13, 16, 5, 1)
    c.rect(10, 15, 12, 1, 6) # Directional crimson marker
    return c

def create_monk_field():
    c = PixelCanvas(32, 32, bg=0)
    # Corrupt Monk Myogak (Field SD)
    # Shaved head
    c.circle(16, 6, 4, 5) # Peach head
    c.rect(14, 8, 2, 1, 7) # Sinister glowing eyes
    c.rect(17, 8, 2, 1, 7)
    # Violet Kasaya Robe
    c.rect(10, 12, 12, 14, 14) # Imperial violet robe
    c.rect(12, 12, 8, 12, 1) # Dark under-robe
    c.rect(10, 14, 3, 10, 14) # Left sleeve
    c.rect(19, 14, 3, 10, 14) # Right sleeve
    # Prayer beads (염주)
    c.rect(13, 13, 6, 1, 12)
    c.set(12, 14, 12); c.set(19, 14, 12)
    c.set(14, 16, 12); c.set(17, 16, 12)
    # Dark miasma/aura at feet
    c.rect(8, 26, 16, 3, 14)
    c.rect(6, 28, 20, 2, 2)
    c.set(7, 25, 7); c.set(24, 25, 7)
    return c

def create_dokkaebi_battle():
    c = PixelCanvas(48, 48, bg=0)
    # Wild Dokkaebi Battle Sprite (48x48)
    # Golden horns
    c.rect(17, 4, 3, 6, 12); c.set(18, 3, 15)
    c.rect(28, 4, 3, 6, 12); c.set(29, 3, 15)
    # Wild unruly black hair
    c.circle(24, 12, 9, 1)
    for (hx, hy) in [(15, 6), (18, 5), (29, 5), (32, 7), (13, 11), (34, 11)]:
        c.rect(hx, hy, 2, 2, 2)
    # Face (Jade green skin)
    c.rect(16, 11, 16, 12, 9)
    # Fiery Glowing Eyes
    c.rect(18, 14, 3, 2, 7); c.set(19, 14, 15) # Left eye with pupil
    c.rect(27, 14, 3, 2, 7); c.set(28, 14, 15) # Right eye with pupil
    # Wide toothy grin with fangs
    c.rect(18, 19, 12, 3, 1)
    c.set(19, 19, 4); c.set(28, 19, 4) # Sharp upward fangs
    c.rect(21, 19, 6, 1, 4) # Upper teeth
    # Muscular Jade Green Torso
    c.rect(14, 23, 20, 13, 9)
    c.rect(17, 25, 6, 4, 8); c.rect(25, 25, 6, 4, 8) # Pectorals shadow
    c.rect(19, 30, 4, 5, 8); c.set(24, 31, 8) # Abs shadow
    # Tiger-pelt loincloth
    c.rect(13, 36, 22, 6, 12) # Ochre pelt
    for tx in [15, 19, 23, 27, 31]:
        c.rect(tx, 37, 2, 4, 1) # Tiger stripes
    # Legs
    c.rect(15, 42, 6, 5, 9); c.rect(27, 42, 6, 5, 9)
    c.rect(14, 46, 7, 2, 8); c.rect(27, 46, 7, 2, 8)
    # Giant Spiked Wooden Club (방망이) in right hand
    c.rect(34, 14, 7, 26, 13) # Wood body
    c.rect(35, 10, 5, 5, 13) # Club head
    c.stroke_rect(34, 10, 7, 30, 1)
    # Golden Spikes / Studs on club
    spikes = [(32, 12), (42, 14), (32, 20), (42, 22), (32, 28), (42, 30)]
    for (sx, sy) in spikes:
        c.rect(sx, sy, 2, 2, 12)
        c.set(sx, sy, 15)
    # Hands gripping club
    c.rect(32, 24, 4, 5, 9)
    c.rect(10, 24, 4, 6, 9) # Left raised fist
    return c

def create_dokkaebi_ally_battle():
    c = PixelCanvas(48, 48, bg=0)
    # Ally Dokkaebi (Facing front-left, friendly resolute pose)
    # Golden horns
    c.rect(18, 5, 3, 6, 12); c.set(19, 4, 15)
    c.rect(27, 5, 3, 6, 12); c.set(28, 4, 15)
    # Hair
    c.circle(24, 12, 8, 1)
    # Jade skin face
    c.rect(17, 12, 14, 11, 9)
    # Friendly determined eyes
    c.rect(19, 15, 3, 2, 11); c.set(20, 15, 4) # Han Blue spiritual eyes
    c.rect(26, 15, 3, 2, 11); c.set(27, 15, 4)
    c.rect(20, 20, 8, 2, 4) # Confident grin
    # Torso & Blue sash
    c.rect(15, 23, 18, 13, 9)
    c.rect(14, 28, 20, 3, 10) # Blue spiritual martial sash
    # Tiger pelt
    c.rect(15, 36, 18, 6, 12)
    c.rect(18, 37, 2, 4, 1); c.rect(24, 37, 2, 4, 1)
    # Club rested on shoulder
    c.rect(7, 10, 6, 28, 13)
    c.stroke_rect(7, 10, 6, 28, 1)
    c.set(5, 12, 12); c.set(14, 14, 12); c.set(5, 20, 12) # Golden studs
    # Arms
    c.rect(11, 23, 5, 5, 9)
    c.rect(30, 24, 5, 7, 9)
    # Legs
    c.rect(16, 42, 5, 5, 9); c.rect(26, 42, 5, 5, 9)
    c.rect(15, 46, 6, 2, 10); c.rect(26, 46, 6, 2, 10) # Wrapped martial anklets
    return c

def create_boss_myogak_battle():
    c = PixelCanvas(48, 48, bg=0)
    # Boss: Corrupt Monk Myogak (48x48)
    # Looming dark miasma aura
    for y in range(4, 46):
        for x in range(4, 44):
            if (x * 7 + y * 13) % 17 == 0:
                c.set(x, y, 14) # Purple miasma wisps
    # Shaved pale head
    c.circle(24, 10, 6, 5) # Pale peach head
    # Sinister glowing crimson eyes
    c.rect(21, 10, 2, 2, 7); c.rect(26, 10, 2, 2, 7)
    c.set(21, 10, 15); c.set(26, 10, 15) # Red-hot glare
    # Billowing Imperial Violet Kasaya Robe
    c.rect(14, 16, 20, 26, 14) # Robe body
    c.rect(8, 20, 8, 18, 14) # Wide billowing left sleeve
    c.rect(32, 20, 8, 18, 14) # Wide billowing right sleeve
    # Dark black undergarment
    c.rect(18, 16, 12, 14, 1)
    # Forbidden Yin-Yang gold embroidery on robe
    c.circle(24, 25, 4, 12)
    c.circle(24, 25, 2, 1)
    c.set(24, 24, 7); c.set(24, 26, 12)
    # Large wooden prayer beads (염주)
    beads = [(18, 17), (20, 19), (22, 20), (26, 20), (28, 19), (30, 17), (24, 21)]
    for (bx, by) in beads:
        c.rect(bx, by, 2, 2, 13)
        c.set(bx, by, 12)
    # Dark Bronze Shakujo Staff (석장) in right hand
    c.rect(38, 6, 2, 38, 12) # Golden bronze shaft
    c.circle(39, 7, 4, 12) # Ring head
    c.circle(39, 7, 2, 0)
    c.set(36, 5, 15); c.set(42, 5, 15) # Rattling rings
    # Cursed dark miasma base
    c.rect(10, 42, 28, 5, 14)
    c.rect(6, 44, 36, 3, 1)
    c.set(12, 41, 7); c.set(35, 41, 7)
    return c

ALL_SPRITES = [
    ("tile_village_grass", 32, 32, create_tile_village_grass()),
    ("tile_village_path", 32, 32, create_tile_village_path()),
    ("tile_mountain_rock", 32, 32, create_tile_mountain_rock()),
    ("tile_temple_stone", 32, 32, create_tile_temple_stone()),
    ("hero_field", 32, 32, create_hero_field()),
    ("jumo_field", 32, 32, create_jumo_field()),
    ("shrine_field", 32, 32, create_shrine_field()),
    ("signpost_field", 32, 32, create_signpost_field()),
    ("monk_field", 32, 32, create_monk_field()),
    ("dokkaebi_battle", 48, 48, create_dokkaebi_battle()),
    ("dokkaebi_ally_battle", 48, 48, create_dokkaebi_ally_battle()),
    ("boss_myogak_battle", 48, 48, create_boss_myogak_battle())
]

def generate_header(out_path):
    lines = []
    lines.append("// Auto-generated by tools/asset_pipeline.py - DO NOT EDIT MANUALLY")
    lines.append("#pragma once")
    lines.append("#include <cstdint>")
    lines.append("#include <cstddef>")
    lines.append("")
    lines.append("namespace art {")
    lines.append("")
    lines.append("// Dancheong 16-Color Palette (ARGB uint32)")
    lines.append("inline constexpr std::uint32_t PALETTE_DANCHEONG_16[16] = {")
    for val, name in PALETTE_DANCHEONG_16:
        lines.append(f"    0x{val:08X}u, // {name}")
    lines.append("};")
    lines.append("")
    lines.append("struct RleSprite {")
    lines.append("    int width;")
    lines.append("    int height;")
    lines.append("    int rle_size;")
    lines.append("    const std::uint8_t* data;")
    lines.append("};")
    lines.append("")

    total_uncompressed = 0
    total_compressed = 0

    for name, w, h, canvas in ALL_SPRITES:
        rle = canvas.compress_rle()
        raw_size = w * h
        comp_size = len(rle)
        total_uncompressed += raw_size
        total_compressed += comp_size

        lines.append(f"// {name} ({w}x{h}, raw: {raw_size}B, rle: {comp_size}B, ratio: {comp_size/raw_size*100:.1f}%)")
        lines.append(f"inline constexpr std::uint8_t SPRITE_DATA_{name}[{comp_size}] = {{")
        chunk_str = []
        for i in range(0, len(rle), 16):
            chunk = rle[i:i+16]
            chunk_str.append("    " + ", ".join(f"0x{b:02X}" for b in chunk) + ",")
        lines.extend(chunk_str)
        lines.append("};")
        lines.append(f"inline constexpr RleSprite SPRITE_{name} = {{ {w}, {h}, {comp_size}, SPRITE_DATA_{name} }};")
        lines.append("")

    lines.append("// Fast RLE Draw Functions")
    lines.append("inline void draw_rle_sprite(std::uint32_t* fb, int fb_w, int fb_h, const RleSprite& sprite, int dx, int dy, int scale = 1, bool transparent_zero = true) {")
    lines.append("    int cur_x = 0;")
    lines.append("    int cur_y = 0;")
    lines.append("    for (int i = 0; i < sprite.rle_size; ++i) {")
    lines.append("        std::uint8_t b = sprite.data[i];")
    lines.append("        int run = ((b >> 4) & 0x0F) + 1;")
    lines.append("        int col_idx = b & 0x0F;")
    lines.append("        std::uint32_t color = PALETTE_DANCHEONG_16[col_idx];")
    lines.append("")
    lines.append("        for (int r = 0; r < run; ++r) {")
    lines.append("            if (!transparent_zero || col_idx != 0) {")
    lines.append("                for (int sy = 0; sy < scale; ++sy) {")
    lines.append("                    int py = dy + cur_y * scale + sy;")
    lines.append("                    if (py < 0 || py >= fb_h) continue;")
    lines.append("                    for (int sx = 0; sx < scale; ++sx) {")
    lines.append("                        int px = dx + cur_x * scale + sx;")
    lines.append("                        if (px < 0 || px >= fb_w) continue;")
    lines.append("                        fb[py * fb_w + px] = color;")
    lines.append("                    } // sx")
    lines.append("                } // sy")
    lines.append("            } // not transparent")
    lines.append("            cur_x++;")
    lines.append("            if (cur_x >= sprite.width) {")
    lines.append("                cur_x = 0;")
    lines.append("                cur_y++;")
    lines.append("                if (cur_y >= sprite.height) break;")
    lines.append("            }")
    lines.append("        } // r")
    lines.append("    } // i")
    lines.append("}")
    lines.append("")
    lines.append("inline void draw_rle_tile(std::uint32_t* fb, int fb_w, int fb_h, const RleSprite& tile, int dx, int dy) {")
    lines.append("    draw_rle_sprite(fb, fb_w, fb_h, tile, dx, dy, 1, false);")
    lines.append("}")
    lines.append("")
    lines.append("} // namespace art")
    lines.append("")

    with open(out_path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))

    print(f"[OK] Generated {out_path}")
    print(f"Total assets: {len(ALL_SPRITES)}")
    print(f"Total raw pixels: {total_uncompressed} B")
    print(f"Total RLE compressed: {total_compressed} B ({total_compressed / total_uncompressed * 100:.1f}%)")

if __name__ == "__main__":
    out = os.path.join(os.path.dirname(__file__), "..", "src", "generated_sprites.h")
    generate_header(out)
