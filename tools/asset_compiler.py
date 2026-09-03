import os
from PIL import Image

def build_assets():
    # 1. Load Gen 1 Sample Tileset (1024x1024)
    # The image contains a grid of tiles (e.g. 8x8 grid of blocks)
    tile_img = Image.open('docs/samples/gen1_sample_tileset.jpg').convert('L')
    
    # We want 33 tiles of 16x16 size.
    # Let's crop 33 distinct 16x16 tiles from the 1024x1024 sheet.
    # Grid in 1024x1024 is approximately 8x8 regions (~128x128 each)
    tiles_16x16 = []
    
    # Extract 33 representative tiles (0 to 32)
    # We downscale each 128x128 cell to 16x16 using high quality resampling
    cell_w = 1024 // 8
    cell_h = 1024 // 8
    
    for row in range(8):
        for col in range(8):
            if len(tiles_16x16) >= 33:
                break
            box = (col * cell_w + 10, row * cell_h + 10, (col + 1) * cell_w - 10, (row + 1) * cell_h - 10)
            sub = tile_img.crop(box).resize((16, 16), Image.Resampling.LANCZOS)
            
            # Quantize sub to 0..3 (2-bit DMG)
            pix = list(sub.getdata())
            qpix = []
            for p in pix:
                if p > 195: q = 0       # Pale White
                elif p > 130: q = 1     # Light Green/Olive
                elif p > 65: q = 2      # Dark Green/Olive
                else: q = 3             # Darkest Black/Green
                qpix.append(q)
            tiles_16x16.append(qpix)
            
    print(f"Extracted {len(tiles_16x16)} tiles of 16x16.")

    # 2. Load Gen 1 Spritesheet (1024x1024)
    sprite_img = Image.open('docs/samples/gen1_sample_spritesheet.jpg').convert('L')
    
    # Extract Player 16x24 walking sprites (4 directions x 2 frames)
    # Top section has Down, Up, Left, Right
    # Down is roughly around (100, 100) to (450, 250)
    player_sprites = []
    
    # Down 0, 1
    # We can crop and resize specific sub-boxes for 16x24
    regions_player = [
        (120, 100, 230, 260), # Down 0
        (300, 100, 410, 260), # Down 1
        (580, 100, 690, 260), # Up 0
        (760, 100, 870, 260), # Up 1
        (120, 310, 230, 470), # Left 0
        (300, 310, 410, 470), # Left 1
        (580, 310, 690, 470), # Right 0
        (760, 310, 870, 470), # Right 1
    ]
    
    for box in regions_player:
        sub = sprite_img.crop(box).resize((16, 24), Image.Resampling.LANCZOS)
        pix = list(sub.getdata())
        qpix = []
        for p in pix:
            if p > 195: q = 0
            elif p > 130: q = 1
            elif p > 65: q = 2
            else: q = 3
            qpix.append(q)
        player_sprites.append(qpix)

    # Dokkaebi 32x32 (Bottom Left)
    dok_box = (60, 580, 420, 950)
    dok_sub = sprite_img.crop(dok_box).resize((32, 32), Image.Resampling.LANCZOS)
    dok_pix = [0 if p > 195 else 1 if p > 130 else 2 if p > 65 else 3 for p in list(dok_sub.getdata())]

    # Gumiho 48x48 (Bottom Right)
    gum_box = (530, 560, 960, 960)
    gum_sub = sprite_img.crop(gum_box).resize((48, 48), Image.Resampling.LANCZOS)
    gum_pix = [0 if p > 195 else 1 if p > 130 else 2 if p > 65 else 3 for p in list(gum_sub.getdata())]

    # Gumiho 6-head Bust Portrait 48x48 for dialogue
    mockup_img = Image.open('docs/samples/gen1_sample_mockup.jpg').convert('L')
    portrait_box = (45, 545, 235, 960)
    portrait_sub = mockup_img.crop(portrait_box).resize((48, 48), Image.Resampling.LANCZOS)
    portrait_pix = [0 if p > 195 else 1 if p > 130 else 2 if p > 65 else 3 for p in list(portrait_sub.getdata())]

    # 3. Generate C++ Header file: src/core/gen1_assets.hpp
    # Pack 2-bit values: 4 pixels per byte!
    # 16x16 tile = 256 pixels = 64 bytes
    # 16x24 sprite = 384 pixels = 96 bytes
    # 32x32 battler = 1024 pixels = 256 bytes
    # 48x48 battler = 2304 pixels = 576 bytes
    
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
        
        # Color Palette (4 DMG Shades)
        f.write('// GB 4-Shade Palette: 0=White/Lightest, 1=Light Olive, 2=Dark Olive, 3=Black/Darkest\n')
        f.write('constexpr uint32_t DMG_PALETTE[4] = {\n')
        f.write('    0xFFE0F8D0, // 0: Pale Off-White\n')
        f.write('    0xFF88C070, // 1: Light Olive Green\n')
        f.write('    0xFF346856, // 2: Dark Olive Green\n')
        f.write('    0xFF081820  // 3: Darkest Ink Black\n')
        f.write('};\n\n')

        # Tiles (33 tiles x 64 bytes = 2,112 bytes)
        f.write(f'constexpr int TOTAL_TILES = {len(tiles_16x16)};\n')
        f.write('constexpr uint8_t TILES_16x16[33][64] = {\n')
        for idx, t in enumerate(tiles_16x16):
            packed = pack_2bit(t)
            hex_str = ', '.join(f'0x{b:02X}' for b in packed)
            f.write(f'    {{ {hex_str} }},\n')
        f.write('};\n\n')

        # Player Walking Sprites (8 sprites x 96 bytes = 768 bytes)
        f.write('// Player 16x24 (4 Directions x 2 Frames: Down0, Down1, Up0, Up1, Left0, Left1, Right0, Right1)\n')
        f.write('constexpr uint8_t PLAYER_16x24[8][96] = {\n')
        for s in player_sprites:
            packed = pack_2bit(s)
            hex_str = ', '.join(f'0x{b:02X}' for b in packed)
            f.write(f'    {{ {hex_str} }},\n')
        f.write('};\n\n')

        # Dokkaebi 32x32 (256 bytes)
        packed_dok = pack_2bit(dok_pix)
        f.write('// Dokkaebi 32x32 Battler (256 bytes)\n')
        f.write('constexpr uint8_t DOKKAEBI_32x32[256] = {\n')
        f.write('    ' + ', '.join(f'0x{b:02X}' for b in packed_dok) + '\n')
        f.write('};\n\n')

        # Gumiho 48x48 Battler (576 bytes)
        packed_gum = pack_2bit(gum_pix)
        f.write('// Gumiho 48x48 Battler (576 bytes)\n')
        f.write('constexpr uint8_t GUMIHO_48x48[576] = {\n')
        f.write('    ' + ', '.join(f'0x{b:02X}' for b in packed_gum) + '\n')
        f.write('};\n\n')

        # Gumiho Dialogue Portrait 48x48 (576 bytes)
        packed_port = pack_2bit(portrait_pix)
        f.write('// Gumiho 6-Head Manga Bust Portrait 48x48 for Cutscenes/Dialogues (576 bytes)\n')
        f.write('constexpr uint8_t PORTRAIT_GUMIHO_48x48[576] = {\n')
        f.write('    ' + ', '.join(f'0x{b:02X}' for b in packed_port) + '\n')
        f.write('};\n\n')

        f.write('} // namespace Gen1Assets\n')
        f.write('} // namespace JoseonRPG\n')

    print("Successfully generated src/core/gen1_assets.hpp!")

if __name__ == '__main__':
    build_assets()
