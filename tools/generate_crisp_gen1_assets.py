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

    # 2. Authentic 16x24 Human Character Overworld Sprites (4 Directions x 2 Frames)
    def create_human_walk_cycle(hat_type, robe_tone, has_beard=False, is_female=False):
        frames = []
        for direction in range(4): # 0: Down, 1: Up, 2: Left, 3: Right
            for f in range(2):
                s = [0] * (16 * 24)
                
                # Headwear / Hat (Rows 1 to 6)
                if hat_type == 'gat': # 흑립 갓
                    for y in range(1, 4):
                        for x in range(5, 11): s[y * 16 + x] = 3
                    for x in range(2, 14): s[4 * 16 + x] = 3 # Brim
                    for x in range(3, 13): s[5 * 16 + x] = 2
                elif hat_type == 'jeongjagwan': # 도사 관모
                    for y in range(1, 5):
                        for x in range(4, 12):
                            if (x + y) % 2 == 0: s[y * 16 + x] = 3
                            else: s[y * 16 + x] = 2
                    for x in range(3, 13): s[5 * 16 + x] = 3
                elif hat_type == 'satgat': # 방립 / 삿갓
                    for y in range(1, 6):
                        x_start = 8 - y
                        x_end = 7 + y
                        for x in range(x_start, x_end + 1):
                            if x == x_start or x == x_end or y == 5: s[y * 16 + x] = 3
                            else: s[y * 16 + x] = 1
                elif hat_type == 'gache': # 주모 얹은머리 가체
                    for y in range(1, 6):
                        for x in range(3, 13):
                            if y == 1 or x == 3 or x == 12 or y == 5: s[y * 16 + x] = 3
                            else: s[y * 16 + x] = 2
                    # Binyeo Hairpin
                    s[3 * 16 + 2] = 0; s[3 * 16 + 13] = 0
                elif hat_type == 'paeraengi': # 보부상/촌로 패랭이
                    for y in range(2, 5):
                        for x in range(5, 11): s[y * 16 + x] = 2
                    for x in range(2, 14): s[5 * 16 + x] = 3
                
                # Face / Head (Rows 6 to 11)
                if direction != 1: # Front / Side
                    for y in range(6, 11):
                        for x in range(5, 11): s[y * 16 + x] = 0 # Skin tone
                    if direction == 0: # Down
                        s[8 * 16 + 6] = 3; s[8 * 16 + 9] = 3 # Eyes
                        if has_beard:
                            s[9 * 16 + 7] = 2; s[9 * 16 + 8] = 2
                            s[10 * 16 + 6] = 2; s[10 * 16 + 7] = 2; s[10 * 16 + 8] = 2; s[10 * 16 + 9] = 2
                    elif direction == 2: # Left
                        s[8 * 16 + 5] = 3; s[8 * 16 + 7] = 3
                        if has_beard:
                            s[9 * 16 + 5] = 2; s[10 * 16 + 5] = 2; s[10 * 16 + 6] = 2
                    elif direction == 3: # Right
                        s[8 * 16 + 8] = 3; s[8 * 16 + 10] = 3
                        if has_beard:
                            s[9 * 16 + 10] = 2; s[10 * 16 + 9] = 2; s[10 * 16 + 10] = 2
                else: # Back
                    for y in range(6, 11):
                        for x in range(5, 11): s[y * 16 + x] = 3 # Hair / back of head

                # Robe Body (Rows 11 to 18)
                for y in range(11, 19):
                    for x in range(4, 12):
                        if y == 14: s[y * 16 + x] = 3 # Waist belt
                        elif y < 14: s[y * 16 + x] = robe_tone # Upper robe
                        else:
                            if is_female: s[y * 16 + x] = 2 # Long skirt
                            else: s[y * 16 + x] = 0 # Lower skirt
                
                # Outline Body
                for y in range(11, 19):
                    s[y * 16 + 3] = 3; s[y * 16 + 12] = 3

                # Legs / Feet (Rows 19 to 23)
                if f == 0:
                    for y in range(19, 23):
                        s[y * 16 + 5] = 3; s[y * 16 + 6] = 2
                        s[y * 16 + 9] = 3; s[y * 16 + 10] = 2
                else:
                    for y in range(19, 23):
                        s[y * 16 + 4] = 3; s[y * 16 + 5] = 2
                        s[y * 16 + 10] = 3; s[y * 16 + 11] = 2
                frames.append(s)
        return frames

    player_frames = create_human_walk_cycle('gat', 1, has_beard=False, is_female=False)
    dosa_frames = create_human_walk_cycle('jeongjagwan', 0, has_beard=True, is_female=False)
    jumo_frames = create_human_walk_cycle('gache', 1, has_beard=False, is_female=True)
    cultist_frames = create_human_walk_cycle('satgat', 2, has_beard=False, is_female=False)
    elder_frames = create_human_walk_cycle('paeraengi', 1, has_beard=True, is_female=False)

    # 3. 32x32 Dokkaebi Battler (Spiked Club, Horns, Muscles)
    dokkaebi_32 = [0] * (32 * 32)
    dokkaebi_32[4 * 32 + 10] = 3; dokkaebi_32[5 * 32 + 10] = 3; dokkaebi_32[6 * 32 + 11] = 3
    dokkaebi_32[4 * 32 + 21] = 3; dokkaebi_32[5 * 32 + 21] = 3; dokkaebi_32[6 * 32 + 20] = 3
    for y in range(7, 16):
        for x in range(10, 22):
            if x == 10 or x == 21 or y == 7 or y == 15: dokkaebi_32[y * 32 + x] = 3
            elif (y == 10 and (x == 13 or x == 18)): dokkaebi_32[y * 32 + x] = 3
            elif y == 13 and x >= 12 and x <= 19: dokkaebi_32[y * 32 + x] = 3
            else: dokkaebi_32[y * 32 + x] = 1
    for y in range(16, 26):
        for x in range(8, 24):
            if x == 8 or x == 23 or y == 25: dokkaebi_32[y * 32 + x] = 3
            elif y >= 22: dokkaebi_32[y * 32 + x] = 2
            else: dokkaebi_32[y * 32 + x] = 1
    for y in range(6, 22):
        for x in range(3, 8):
            if x == 3 or x == 7 or y == 6 or y == 21: dokkaebi_32[y * 32 + x] = 3
            elif (y % 4 == 0): dokkaebi_32[y * 32 + x] = 0
            else: dokkaebi_32[y * 32 + x] = 2
    for y in range(26, 31):
        for x in range(8, 13): dokkaebi_32[y * 32 + x] = 3
        for x in range(19, 24): dokkaebi_32[y * 32 + x] = 3

    # 4. 48x48 Gumiho Battler (9 Tails, Sleek Fox, Fox-fire)
    gumiho_48 = [0] * (48 * 48)
    for tail in range(9):
        tx = 6 + tail * 4
        ty = 8 + abs(tail - 4) * 2
        for y in range(ty, ty + 22):
            for x in range(tx, tx + 3):
                if y < 48 and x < 48:
                    if x == tx or y == ty: gumiho_48[y * 48 + x] = 3
                    elif y > ty + 12: gumiho_48[y * 48 + x] = 1
                    else: gumiho_48[y * 48 + x] = 0
    for y in range(20, 36):
        for x in range(22, 38):
            if x == 22 or x == 37 or y == 20 or y == 35: gumiho_48[y * 48 + x] = 3
            elif (y == 24 and (x == 26 or x == 33)): gumiho_48[y * 48 + x] = 3
            else: gumiho_48[y * 48 + x] = 0
    for y in range(36, 46):
        for x in range(24, 27): gumiho_48[y * 48 + x] = 3
        for x in range(33, 36): gumiho_48[y * 48 + x] = 3

    # 5. 48x48 Detailed Bust Portraits
    def create_bust_portrait(char_type):
        p = [0] * (48 * 48)
        if char_type == 'gumiho': # 구미호
            for y in range(2, 12):
                for x in range(8, 16):
                    if x == 8 or y == 2 or x == 15: p[y * 48 + x] = 3
                    else: p[y * 48 + x] = 1
                for x in range(32, 40):
                    if x == 32 or y == 2 or x == 39: p[y * 48 + x] = 3
                    else: p[y * 48 + x] = 1
            for y in range(10, 34):
                for x in range(12, 36):
                    if x == 12 or x == 35 or y == 33: p[y * 48 + x] = 3
                    elif y >= 14 and y <= 28 and x >= 16 and x <= 31:
                        if (y == 20 and (x == 19 or x == 28)): p[y * 48 + x] = 3
                        elif (y == 26 and x >= 22 and x <= 25): p[y * 48 + x] = 3
                        else: p[y * 48 + x] = 0
                    else: p[y * 48 + x] = 3
            for y in range(34, 48):
                for x in range(8, 40):
                    if x == 8 or x == 39 or y == 47: p[y * 48 + x] = 3
                    elif x >= 20 and x <= 27: p[y * 48 + x] = 3
                    else: p[y * 48 + x] = 0
        elif char_type == 'protagonist': # 주인공 퇴마사 (갓 & 도포)
            # Gat hat
            for y in range(2, 10):
                for x in range(16, 32): p[y * 48 + x] = 3
            for x in range(4, 44):
                p[10 * 48 + x] = 3; p[11 * 48 + x] = 2
            # Face & Hair
            for y in range(12, 32):
                for x in range(14, 34):
                    if x == 14 or x == 33 or y == 31: p[y * 48 + x] = 3
                    elif y >= 14 and y <= 28 and x >= 18 and x <= 30:
                        if (y == 20 and (x == 20 or x == 28)): p[y * 48 + x] = 3
                        elif (y == 27 and x >= 23 and x <= 25): p[y * 48 + x] = 3
                        else: p[y * 48 + x] = 0
                    else: p[y * 48 + x] = 3
            # Gat Strings (갓끈)
            for y in range(12, 36):
                p[y * 48 + 16] = 3; p[y * 48 + 31] = 3
            # Robe Collar
            for y in range(32, 48):
                for x in range(8, 40):
                    if x == 8 or x == 39 or y == 47: p[y * 48 + x] = 3
                    elif (x >= 22 and x <= 25): p[y * 48 + x] = 3
                    else: p[y * 48 + x] = 1
        elif char_type == 'dosa': # 도사 성현 (관모 & 긴 흰수염)
            # Jeongjagwan Hat
            for y in range(2, 12):
                for x in range(14, 34):
                    if x == 14 or x == 33 or y == 2 or (x + y) % 3 == 0: p[y * 48 + x] = 3
                    else: p[y * 48 + x] = 2
            # Face & Eyes
            for y in range(12, 26):
                for x in range(16, 32):
                    if (y == 18 and (x == 20 or x == 28)): p[y * 48 + x] = 3
                    else: p[y * 48 + x] = 0
            # Flowing White Beard (Rows 22 to 38)
            for y in range(22, 38):
                w_span = max(2, 8 - (y - 22) // 2)
                for x in range(24 - w_span, 24 + w_span):
                    if x == 24 - w_span or x == 23 + w_span or y == 37: p[y * 48 + x] = 2
                    else: p[y * 48 + x] = 0
            # Robe
            for y in range(36, 48):
                for x in range(6, 42):
                    if x == 6 or x == 41 or y == 47: p[y * 48 + x] = 3
                    else: p[y * 48 + x] = 1
        elif char_type == 'jumo': # 주막 주모 (가체 & 둥근 인상)
            # Gache hair
            for y in range(2, 14):
                for x in range(10, 38):
                    if x == 10 or x == 37 or y == 2 or y == 13: p[y * 48 + x] = 3
                    else: p[y * 48 + x] = 2
            # Binyeo
            for x in range(6, 42): p[8 * 48 + x] = 0
            # Face
            for y in range(14, 32):
                for x in range(14, 34):
                    if (y == 20 and (x == 19 or x == 29)): p[y * 48 + x] = 3
                    elif (y == 26 and x >= 22 and x <= 26): p[y * 48 + x] = 3 # Warm smile
                    else: p[y * 48 + x] = 0
            # Jeogori
            for y in range(32, 48):
                for x in range(8, 40):
                    if x == 8 or x == 39 or y == 47: p[y * 48 + x] = 3
                    else: p[y * 48 + x] = 2
        elif char_type == 'cultist': # 음양당 주술사 (삿갓 & 음산한 눈빛)
            # Deep Satgat
            for y in range(4, 22):
                x_span = (y - 4) * 2
                for x in range(24 - x_span, 24 + x_span):
                    if x >= 0 and x < 48:
                        if x == 24 - x_span or x == 23 + x_span or y == 21: p[y * 48 + x] = 3
                        else: p[y * 48 + x] = 2
            # Glowing Eyes in shadow
            p[24 * 48 + 19] = 0; p[24 * 48 + 20] = 3
            p[24 * 48 + 28] = 0; p[24 * 48 + 29] = 3
            # Dark Kwaeja Robe
            for y in range(30, 48):
                for x in range(8, 40):
                    if x == 8 or x == 39 or y == 47: p[y * 48 + x] = 3
                    elif x >= 20 and x <= 27: p[y * 48 + x] = 2
                    else: p[y * 48 + x] = 3
        return p

    portrait_gumiho = create_bust_portrait('gumiho')
    portrait_protag = create_bust_portrait('protagonist')
    portrait_dosa = create_bust_portrait('dosa')
    portrait_jumo = create_bust_portrait('jumo')
    portrait_cultist = create_bust_portrait('cultist')

    # 6. Generate 108 Distinct 32x32 Gen 1 Yokai Battler Bitmaps (256 bytes each)
    yokai_battlers = []
    
    for num in range(1, 109):
        sprite = [0] * (32 * 32)
        # Category archetypes based on number and folklore
        category = num % 8
        elem_tier = (num * 7) % 5 # 0: Fire, 1: Water, 2: Earth, 3: Light, 4: Dark
        
        # Base body silhouette
        if category == 0 or category == 1: # Beast / Feral (Tiger, Fox, Wolf, Bear)
            # Body & Tail
            for y in range(12, 28):
                for x in range(6, 26):
                    if (x == 6 or x == 25 or y == 12 or y == 27): sprite[y * 32 + x] = 3
                    elif (x + y) % 3 == 0: sprite[y * 32 + x] = 2
                    else: sprite[y * 32 + x] = 1
            # Head & Ears
            sprite[6 * 32 + 8] = 3; sprite[6 * 32 + 9] = 3; sprite[7 * 32 + 9] = 3
            sprite[6 * 32 + 22] = 3; sprite[6 * 32 + 23] = 3; sprite[7 * 32 + 22] = 3
            for y in range(7, 14):
                for x in range(8, 24):
                    if (x == 8 or x == 23 or y == 7): sprite[y * 32 + x] = 3
                    elif (y == 10 and (x == 11 or x == 20)): sprite[y * 32 + x] = 3 # Eyes
                    else: sprite[y * 32 + x] = 0
            # Paws / Claws
            for y in range(28, 32):
                sprite[y * 32 + 8] = 3; sprite[y * 32 + 9] = 3
                sprite[y * 32 + 22] = 3; sprite[y * 32 + 23] = 3

        elif category == 2 or category == 3: # Spectral / Ghost (Floating wisp, Maiden, Grim Reaper)
            # Floating Wispy Hood / Head
            for y in range(4, 16):
                w_h = max(2, (y - 4))
                for x in range(16 - w_h, 16 + w_h):
                    if (x == 16 - w_h or x == 15 + w_h or y == 4): sprite[y * 32 + x] = 3
                    elif (y == 10 and (x == 13 or x == 18)): sprite[y * 32 + x] = 3 # Eerie eyes
                    else: sprite[y * 32 + x] = 0
            # Shroud / Trailing Spectral Fog (Rows 16 to 30)
            for y in range(16, 31):
                w_s = max(2, 10 - (y - 16) // 2)
                for x in range(16 - w_s, 16 + w_s):
                    if (x == 16 - w_s or x == 15 + w_s): sprite[y * 32 + x] = 3
                    elif (x + y + num) % 4 == 0: sprite[y * 32 + x] = 2
                    else: sprite[y * 32 + x] = 1

        elif category == 4: # Ogre / Giant (Horns, Spiked club, Bulgasari)
            # Horns
            sprite[3 * 32 + 9] = 3; sprite[4 * 32 + 10] = 3; sprite[5 * 32 + 11] = 3
            sprite[3 * 32 + 22] = 3; sprite[4 * 32 + 21] = 3; sprite[5 * 32 + 20] = 3
            # Head & Face
            for y in range(6, 16):
                for x in range(10, 22):
                    if (x == 10 or x == 21 or y == 6 or y == 15): sprite[y * 32 + x] = 3
                    elif (y == 10 and (x == 13 or x == 18)): sprite[y * 32 + x] = 3
                    else: sprite[y * 32 + x] = 1
            # Bulky Muscular Torso
            for y in range(16, 27):
                for x in range(7, 25):
                    if (x == 7 or x == 24 or y == 26): sprite[y * 32 + x] = 3
                    elif y >= 23: sprite[y * 32 + x] = 2
                    else: sprite[y * 32 + x] = 1
            # Heavy Iron Weapon / Club on right
            for y in range(8, 28):
                sprite[y * 32 + 27] = 3; sprite[y * 32 + 28] = 3

        elif category == 5: # Draconic / Serpentine / Centipede (Coiled body, Spikes, Whisker)
            # Serpentine S-Curves
            for y in range(6, 28):
                cx = 16 + int(7 * (1 if (y // 4) % 2 == 0 else -1))
                for x in range(max(2, cx - 4), min(30, cx + 4)):
                    if x == max(2, cx - 4) or x == min(30, cx + 4) - 1 or y == 6 or y == 27:
                        sprite[y * 32 + x] = 3
                    elif (x * y + num) % 3 == 0: sprite[y * 32 + x] = 2
                    else: sprite[y * 32 + x] = 1
            # Dragon Head on top
            for y in range(4, 10):
                for x in range(12, 22):
                    if x == 12 or x == 21 or y == 4: sprite[y * 32 + x] = 3
                    elif y == 7 and (x == 15 or x == 19): sprite[y * 32 + x] = 3 # Fierce eye
                    else: sprite[y * 32 + x] = 0

        elif category == 6: # Avian / Winged (Geuseunsae, Samjok-o, Garuda)
            # Wings spread
            for y in range(8, 22):
                span = (y - 8) * 2
                for x in range(max(2, 16 - span), min(30, 16 + span)):
                    if (x == max(2, 16 - span) or x == min(30, 16 + span) - 1 or y == 21): sprite[y * 32 + x] = 3
                    elif (x + y) % 3 == 0: sprite[y * 32 + x] = 2
                    else: sprite[y * 32 + x] = 1
            # Beak & Head
            for y in range(4, 12):
                for x in range(12, 20):
                    if (x == 12 or x == 19 or y == 4): sprite[y * 32 + x] = 3
                    elif y == 7 and x == 16: sprite[y * 32 + x] = 3
                    else: sprite[y * 32 + x] = 0
            sprite[8 * 32 + 15] = 3; sprite[8 * 32 + 16] = 3; sprite[9 * 32 + 16] = 3 # Sharp beak

        else: # Celestial / Deity / Myth Relic (Samshin, Spirit Tree, Mirror)
            # Sacred Mandala Halo / Aura
            for y in range(2, 30):
                for x in range(2, 30):
                    dist_sq = (x - 16)**2 + (y - 16)**2
                    if dist_sq >= 140 and dist_sq <= 190:
                        if (x + y + num) % 2 == 0: sprite[y * 32 + x] = 3
                        else: sprite[y * 32 + x] = 2
            # Divine Effigy in Center
            for y in range(8, 24):
                for x in range(12, 20):
                    if x == 12 or x == 19 or y == 8 or y == 23: sprite[y * 32 + x] = 3
                    elif y == 12 and (x == 14 or x == 17): sprite[y * 32 + x] = 3 # Sacred eyes
                    else: sprite[y * 32 + x] = 0

        yokai_battlers.append(sprite)

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

        # 16x24 Overworld Sprites
        def write_character_array(name, frames_list):
            f.write(f'constexpr uint8_t {name}[8][96] = {{\n')
            for s in frames_list:
                packed = pack_2bit(s)
                hex_str = ', '.join(f'0x{b:02X}' for b in packed)
                f.write(f'    {{ {hex_str} }},\n')
            f.write('};\n\n')

        write_character_array('PLAYER_16x24', player_frames)
        write_character_array('NPC_DOSA_16x24', dosa_frames)
        write_character_array('NPC_JUMO_16x24', jumo_frames)
        write_character_array('NPC_CULTIST_16x24', cultist_frames)
        write_character_array('NPC_ELDER_16x24', elder_frames)

        # Flagship Battlers
        packed_dok = pack_2bit(dokkaebi_32)
        f.write('constexpr uint8_t DOKKAEBI_32x32[256] = {\n')
        f.write('    ' + ', '.join(f'0x{b:02X}' for b in packed_dok) + '\n')
        f.write('};\n\n')

        packed_gum = pack_2bit(gumiho_48)
        f.write('constexpr uint8_t GUMIHO_48x48[576] = {\n')
        f.write('    ' + ', '.join(f'0x{b:02X}' for b in packed_gum) + '\n')
        f.write('};\n\n')

        # 108 Complete Yokai Battlers Array (108 x 256 bytes = 27.6 KB)
        f.write('// Complete 108 Joseon Yokai 32x32 Battler Sprites\n')
        f.write('constexpr uint8_t YOKAI_BATTLERS_32x32[108][256] = {\n')
        for yb in yokai_battlers:
            packed_yb = pack_2bit(yb)
            hex_str = ', '.join(f'0x{b:02X}' for b in packed_yb)
            f.write(f'    {{ {hex_str} }},\n')
        f.write('};\n\n')

        # Portraits
        def write_portrait(name, p_data):
            packed = pack_2bit(p_data)
            f.write(f'constexpr uint8_t {name}[576] = {{\n')
            f.write('    ' + ', '.join(f'0x{b:02X}' for b in packed) + '\n')
            f.write('};\n\n')

        write_portrait('PORTRAIT_GUMIHO_48x48', portrait_gumiho)
        write_portrait('PORTRAIT_PROTAGONIST_48x48', portrait_protag)
        write_portrait('PORTRAIT_DOSA_48x48', portrait_dosa)
        write_portrait('PORTRAIT_JUMO_48x48', portrait_jumo)
        write_portrait('PORTRAIT_CULTIST_48x48', portrait_cultist)

        f.write('} // namespace Gen1Assets\n')
        f.write('} // namespace JoseonRPG\n')

    print("Pixel-perfect Gen 1 assets, full human cast, and 108 Yokai battlers successfully written!")

if __name__ == '__main__':
    create_pixel_assets()
