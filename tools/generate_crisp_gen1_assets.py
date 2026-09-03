import os

def create_pixel_assets():
    # 0: Pale Off-White (0)
    # 1: Light Olive (1)
    # 2: Dark Olive (2)
    # 3: Darkest Black (3)
    
    # Let's generate authentic, clean, 16x16 pixel-perfect tiles
    tiles = []
    
    # Tile 0: Grass / Earth (Clean Gen 1 Pokemon grass with grass tufts)
    t0 = [
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,
        0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,
        0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,
        0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
        0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0
    ]
    tiles.append(t0)

    # Tile 1: Stone Wall (Joseon mossy stone wall / Dol-dam)
    t1 = [
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        3,0,0,0,2,3,0,0,0,0,0,2,3,0,0,3,
        3,0,1,0,2,3,0,1,1,0,0,2,3,0,1,3,
        3,2,2,2,2,3,2,2,2,2,2,2,3,2,2,3,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        3,0,0,0,0,0,2,3,0,0,0,0,2,3,0,3,
        3,0,1,1,0,0,2,3,0,1,0,0,2,3,1,3,
        3,2,2,2,2,2,2,3,2,2,2,2,2,3,2,3,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        3,0,0,0,2,3,0,0,0,0,0,2,3,0,0,3,
        3,0,1,0,2,3,0,1,1,0,0,2,3,0,1,3,
        3,2,2,2,2,3,2,2,2,2,2,2,3,2,2,3,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        3,0,0,0,0,0,2,3,0,0,0,0,2,3,0,3,
        3,0,1,1,0,0,2,3,0,1,0,0,2,3,1,3,
        3,2,2,2,2,2,2,3,2,2,2,2,2,3,2,3
    ]
    tiles.append(t1)

    # Tile 2: Dirt Road (Smooth dirt road with scattered pebbles)
    t2 = [
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,2,3,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,3,2,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,2,3,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,3,2,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,2,3,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,3,2,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    ]
    tiles.append(t2)

    # Tile 3: Wooden Floor / Maru
    t3 = [
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
    ]
    tiles.append(t3)

    # Tile 4: Hanok Tile Roof (Giwa)
    t4 = [
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        3,2,2,3,2,2,3,2,2,3,2,2,3,2,2,3,
        3,1,1,3,1,1,3,1,1,3,1,1,3,1,1,3,
        3,0,0,3,0,0,3,0,0,3,0,0,3,0,0,3,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        3,2,2,3,2,2,3,2,2,3,2,2,3,2,2,3,
        3,1,1,3,1,1,3,1,1,3,1,1,3,1,1,3,
        3,0,0,3,0,0,3,0,0,3,0,0,3,0,0,3,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        3,2,2,3,2,2,3,2,2,3,2,2,3,2,2,3,
        3,1,1,3,1,1,3,1,1,3,1,1,3,1,1,3,
        3,0,0,3,0,0,3,0,0,3,0,0,3,0,0,3,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        3,2,2,3,2,2,3,2,2,3,2,2,3,2,2,3,
        3,1,1,3,1,1,3,1,1,3,1,1,3,1,1,3,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
    ]
    tiles.append(t4)

    # Fill remaining tiles 5~32 with distinct thematic patterns (water, bridge, changhoji, dangsan tree, chest, etc.)
    for idx in range(5, 33):
        t = [0] * 256
        if idx == 6 or idx == 7: # Water / Waves
            for y in range(16):
                for x in range(16):
                    if y % 4 == 0 and (x + (y // 4) * 2) % 4 == 0: t[y * 16 + x] = 3
                    elif y % 4 == 1 and (x + (y // 4) * 2) % 4 == 0: t[y * 16 + x] = 2
                    elif y % 4 == 2: t[y * 16 + x] = 1
                    else: t[y * 16 + x] = 0
        elif idx == 10: # Treasure Chest
            for y in range(16):
                for x in range(16):
                    if y >= 4 and y <= 12 and x >= 3 and x <= 12:
                        if y == 4 or y == 12 or x == 3 or x == 12 or y == 7: t[y * 16 + x] = 3
                        elif y == 8 and (x == 7 or x == 8): t[y * 16 + x] = 0
                        else: t[y * 16 + x] = 1
                    else: t[y * 16 + x] = 0
        elif idx == 14: # Onggi Pot
            for y in range(16):
                for x in range(16):
                    if y >= 4 and y <= 13 and x >= 4 and x <= 11:
                        if y == 4 or y == 13 or x == 4 or x == 11: t[y * 16 + x] = 3
                        elif y == 5 or y == 6: t[y * 16 + x] = 2
                        else: t[y * 16 + x] = 1
                    else: t[y * 16 + x] = 0
        elif idx == 29: # Changhoji Door
            for y in range(16):
                for x in range(16):
                    if x == 0 or x == 15 or y == 0 or y == 15 or x == 7 or y == 7: t[y * 16 + x] = 3
                    elif x % 4 == 0 or y % 4 == 0: t[y * 16 + x] = 2
                    else: t[y * 16 + x] = 0
        elif idx == 25: # Cliff Wall
            for y in range(16):
                for x in range(16):
                    if y == 0 or y == 15 or (x + y) % 6 == 0: t[y * 16 + x] = 3
                    elif y < 4: t[y * 16 + x] = 0
                    elif y < 10: t[y * 16 + x] = 1
                    else: t[y * 16 + x] = 2
        elif idx == 26: # Stone Stairs
            for y in range(16):
                for x in range(16):
                    if y % 4 == 0: t[y * 16 + x] = 3
                    elif y % 4 == 1: t[y * 16 + x] = 0
                    elif y % 4 == 2: t[y * 16 + x] = 1
                    else: t[y * 16 + x] = 2
        else:
            for y in range(16):
                for x in range(16):
                    if (x + y) % 4 == 0: t[y * 16 + x] = 2
                    elif (x * y) % 5 == 0: t[y * 16 + x] = 1
                    else: t[y * 16 + x] = 0
        tiles.append(t)

    # 2. Authentic 16x24 Joseon Exorcist Protagonist (4 Directions x 2 Frames)
    # Clear pixel outlines, 2-head-tall, Gat hat, robe, eyes
    player_frames = []
    
    # Base template for 16x24 Exorcist
    # Down 0, Down 1, Up 0, Up 1, Left 0, Left 1, Right 0, Right 1
    for direction in range(4):
        for f in range(2):
            sprite = [0] * (16 * 24)
            # Gat Hat (Rows 1 to 6)
            for y in range(1, 4):
                for x in range(5, 11): sprite[y * 16 + x] = 3
            for x in range(2, 14): sprite[4 * 16 + x] = 3 # Brim
            for x in range(3, 13): sprite[5 * 16 + x] = 2 # Brim shade
            
            # Face (Rows 6 to 11)
            if direction != 1: # Front / Side
                for y in range(6, 11):
                    for x in range(5, 11): sprite[y * 16 + x] = 0 # Face skin
                # Eyes
                if direction == 0: # Down
                    sprite[8 * 16 + 6] = 3; sprite[8 * 16 + 9] = 3
                elif direction == 2: # Left
                    sprite[8 * 16 + 5] = 3; sprite[8 * 16 + 7] = 3
                elif direction == 3: # Right
                    sprite[8 * 16 + 8] = 3; sprite[8 * 16 + 10] = 3
            else: # Back (Up)
                for y in range(6, 11):
                    for x in range(5, 11): sprite[y * 16 + x] = 3 # Back of head
            
            # Robe Body (Rows 11 to 18)
            for y in range(11, 19):
                for x in range(4, 12):
                    if y == 14: sprite[y * 16 + x] = 3 # Waist belt
                    elif y < 14: sprite[y * 16 + x] = 1 # Upper robe
                    else: sprite[y * 16 + x] = 0 # Lower robe skirt
            
            # Outline body
            for y in range(11, 19):
                sprite[y * 16 + 3] = 3; sprite[y * 16 + 12] = 3
                
            # Legs / Feet (Rows 19 to 23)
            if f == 0:
                for y in range(19, 23):
                    sprite[y * 16 + 5] = 3; sprite[y * 16 + 6] = 2
                    sprite[y * 16 + 9] = 3; sprite[y * 16 + 10] = 2
            else:
                for y in range(19, 23):
                    sprite[y * 16 + 4] = 3; sprite[y * 16 + 5] = 2
                    sprite[y * 16 + 10] = 3; sprite[y * 16 + 11] = 2
            player_frames.append(sprite)

    # 3. 32x32 Dokkaebi Battler (Spiked Club, Horns, Muscles)
    dokkaebi_32 = [0] * (32 * 32)
    # Head & Horns
    dokkaebi_32[4 * 32 + 10] = 3; dokkaebi_32[5 * 32 + 10] = 3; dokkaebi_32[6 * 32 + 11] = 3 # Horn L
    dokkaebi_32[4 * 32 + 21] = 3; dokkaebi_32[5 * 32 + 21] = 3; dokkaebi_32[6 * 32 + 20] = 3 # Horn R
    for y in range(7, 16):
        for x in range(10, 22):
            if x == 10 or x == 21 or y == 7 or y == 15: dokkaebi_32[y * 32 + x] = 3
            elif (y == 10 and (x == 13 or x == 18)): dokkaebi_32[y * 32 + x] = 3 # Eyes
            elif y == 13 and x >= 12 and x <= 19: dokkaebi_32[y * 32 + x] = 3 # Mouth
            else: dokkaebi_32[y * 32 + x] = 1 # Skin
    # Torso & Arms
    for y in range(16, 26):
        for x in range(8, 24):
            if x == 8 or x == 23 or y == 25: dokkaebi_32[y * 32 + x] = 3
            elif y >= 22: dokkaebi_32[y * 32 + x] = 2 # Belt / Loincloth
            else: dokkaebi_32[y * 32 + x] = 1
    # Club on left side (x=3..7, y=5..22)
    for y in range(6, 22):
        for x in range(3, 8):
            if x == 3 or x == 7 or y == 6 or y == 21: dokkaebi_32[y * 32 + x] = 3
            elif (y % 4 == 0): dokkaebi_32[y * 32 + x] = 0 # Spikes
            else: dokkaebi_32[y * 32 + x] = 2
    # Legs
    for y in range(26, 31):
        for x in range(8, 13): dokkaebi_32[y * 32 + x] = 3
        for x in range(19, 24): dokkaebi_32[y * 32 + x] = 3

    # 4. 48x48 Gumiho Battler (9 Tails, Sleek Fox, Fox-fire)
    gumiho_48 = [0] * (48 * 48)
    # Draw 9 Tails fan
    for tail in range(9):
        tx = 6 + tail * 4
        ty = 8 + abs(tail - 4) * 2
        for y in range(ty, ty + 22):
            for x in range(tx, tx + 3):
                if y < 48 and x < 48:
                    if x == tx or y == ty: gumiho_48[y * 48 + x] = 3
                    elif y > ty + 12: gumiho_48[y * 48 + x] = 1
                    else: gumiho_48[y * 48 + x] = 0
    # Body & Head
    for y in range(20, 36):
        for x in range(22, 38):
            if x == 22 or x == 37 or y == 20 or y == 35: gumiho_48[y * 48 + x] = 3
            elif (y == 24 and (x == 26 or x == 33)): gumiho_48[y * 48 + x] = 3 # Eyes
            else: gumiho_48[y * 48 + x] = 0
    # Legs
    for y in range(36, 46):
        for x in range(24, 27): gumiho_48[y * 48 + x] = 3
        for x in range(33, 36): gumiho_48[y * 48 + x] = 3

    # 5. 48x48 Gumiho Dialogue Portrait (Detailed 6-head Bust)
    portrait_48 = [0] * (48 * 48)
    # Fox Ears
    for y in range(2, 12):
        for x in range(8, 16):
            if x == 8 or y == 2 or x == 15: portrait_48[y * 48 + x] = 3
            else: portrait_48[y * 48 + x] = 1
        for x in range(32, 40):
            if x == 32 or y == 2 or x == 39: portrait_48[y * 48 + x] = 3
            else: portrait_48[y * 48 + x] = 1
    # Hair & Face
    for y in range(10, 34):
        for x in range(12, 36):
            if x == 12 or x == 35 or y == 33: portrait_48[y * 48 + x] = 3
            elif y >= 14 and y <= 28 and x >= 16 and x <= 31:
                # Face
                if (y == 20 and (x == 19 or x == 28)): portrait_48[y * 48 + x] = 3 # Eyes
                elif (y == 26 and x >= 22 and x <= 25): portrait_48[y * 48 + x] = 3 # Lips
                else: portrait_48[y * 48 + x] = 0
            else:
                portrait_48[y * 48 + x] = 3 # Black hair
    # Hanbok Collar
    for y in range(34, 48):
        for x in range(8, 40):
            if x == 8 or x == 39 or y == 47: portrait_48[y * 48 + x] = 3
            elif x >= 20 and x <= 27: portrait_48[y * 48 + x] = 3 # Collar trim
            else: portrait_48[y * 48 + x] = 0

    # Pack 2-bit values into uint8_t arrays (4 pixels per byte)
    def pack_2bit(pix_array):
        bytes_out = []
        for i in range(0, len(pix_array), 4):
            b = 0
            for j in range(4):
                if i + j < len(pix_array):
                    b |= (pix_array[i + j] & 0x03) << (6 - j * 2)
            bytes_out.append(b)
        return bytes_out

    os.makedirs('src/core', exist_ok=True)
    with open('src/core/gen1_assets.hpp', 'w', encoding='utf-8') as f:
        f.write('#pragma once\n')
        f.write('#include <cstdint>\n')
        f.write('#include <array>\n\n')
        f.write('namespace JoseonRPG {\n')
        f.write('namespace Gen1Assets {\n\n')
        
        f.write('// GB 4-Shade DMG Palette: 0=Pale Off-White, 1=Light Olive, 2=Dark Olive, 3=Darkest Ink Black\n')
        f.write('constexpr uint32_t DMG_PALETTE[4] = {\n')
        f.write('    0xFFE0F8D0, // 0: Pale Off-White\n')
        f.write('    0xFF88C070, // 1: Light Olive Green\n')
        f.write('    0xFF346856, // 2: Dark Olive Green\n')
        f.write('    0xFF081820  // 3: Darkest Ink Black\n')
        f.write('};\n\n')

        f.write(f'constexpr int TOTAL_TILES = {len(tiles)};\n')
        f.write('constexpr uint8_t TILES_16x16[33][64] = {\n')
        for t in tiles:
            packed = pack_2bit(t)
            hex_str = ', '.join(f'0x{b:02X}' for b in packed)
            f.write(f'    {{ {hex_str} }},\n')
        f.write('};\n\n')

        f.write('constexpr uint8_t PLAYER_16x24[8][96] = {\n')
        for s in player_frames:
            packed = pack_2bit(s)
            hex_str = ', '.join(f'0x{b:02X}' for b in packed)
            f.write(f'    {{ {hex_str} }},\n')
        f.write('};\n\n')

        packed_dok = pack_2bit(dokkaebi_32)
        f.write('constexpr uint8_t DOKKAEBI_32x32[256] = {\n')
        f.write('    ' + ', '.join(f'0x{b:02X}' for b in packed_dok) + '\n')
        f.write('};\n\n')

        packed_gum = pack_2bit(gumiho_48)
        f.write('constexpr uint8_t GUMIHO_48x48[576] = {\n')
        f.write('    ' + ', '.join(f'0x{b:02X}' for b in packed_gum) + '\n')
        f.write('};\n\n')

        packed_port = pack_2bit(portrait_48)
        f.write('constexpr uint8_t PORTRAIT_GUMIHO_48x48[576] = {\n')
        f.write('    ' + ', '.join(f'0x{b:02X}' for b in packed_port) + '\n')
        f.write('};\n\n')

        f.write('} // namespace Gen1Assets\n')
        f.write('} // namespace JoseonRPG\n')

    print("Pixel-perfect Gen 1 assets successfully written!")

if __name__ == '__main__':
    create_pixel_assets()
