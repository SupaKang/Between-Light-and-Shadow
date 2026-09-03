import os
import glob
from PIL import Image, ImageDraw, ImageFont

def generate_korean_font_header():
    # 1. Collect all Korean characters across src
    chars = set()
    for path in glob.glob('src/**/*.cpp', recursive=True) + glob.glob('src/**/*.hpp', recursive=True) + glob.glob('tests/**/*.cpp', recursive=True):
        with open(path, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
            for ch in content:
                if ord(ch) >= 0xAC00 and ord(ch) <= 0xD7A3:
                    chars.add(ch)
                    
    # Also add standard UI characters and symbols
    extra_symbols = "★☆▶▼▲◀1234567890.:,!?/()[]{}<>-+=%&'\"~`_ 냥배속레벨체력영력공격방어민첩도감유물임무퀘스트전투기술계약교체도망승리패배"
    for ch in extra_symbols:
        if ord(ch) >= 0xAC00 and ord(ch) <= 0xD7A3:
            chars.add(ch)
            
    sorted_chars = sorted(list(chars))
    print(f"Total unique Hangul syllables to render: {len(sorted_chars)}")

    # 2. Render each 12x12 1-bit bitmap using Windows Dotum/Gulim pixel font
    font_path = 'C:\\Windows\\Fonts\\gulim.ttc'
    if not os.path.exists(font_path):
        font_path = 'C:\\Windows\\Fonts\\malgun.ttf'
        
    font = ImageFont.truetype(font_path, 11)
    
    # 12x12 glyphs = 144 bits = 18 bytes per glyph
    # Let's generate a fast map / binary search array:
    # struct HangulGlyph { uint16_t code; uint8_t bitmap[18]; };
    
    glyphs = []
    for ch in sorted_chars:
        img = Image.new('1', (12, 12), color=0)
        draw = ImageDraw.Draw(img)
        draw.text((0, -1), ch, font=font, fill=1)
        
        # Extract 144 bits -> 18 bytes
        bits = []
        for y in range(12):
            for x in range(12):
                bits.append(1 if img.getpixel((x, y)) else 0)
                
        bytes_18 = []
        for i in range(0, 144, 8):
            b = 0
            for j in range(8):
                if i + j < 144 and bits[i + j]:
                    b |= (1 << (7 - j))
            bytes_18.append(b)
            
        code = ord(ch)
        glyphs.append((code, bytes_18, ch))
        
    # Write src/ui/korean_font.hpp
    with open('src/ui/korean_font.hpp', 'w', encoding='utf-8') as f:
        f.write('#pragma once\n')
        f.write('#include <cstdint>\n')
        f.write('#include <cstddef>\n\n')
        f.write('namespace JoseonRPG {\n\n')
        f.write('struct HangulGlyph {\n')
        f.write('    uint16_t unicode;\n')
        f.write('    uint8_t bitmap[18]; // 12x12 1-bit packed (144 bits)\n')
        f.write('};\n\n')
        f.write(f'constexpr size_t HANGUL_GLYPH_COUNT = {len(glyphs)};\n')
        f.write('inline const HangulGlyph HANGUL_GLYPHS[HANGUL_GLYPH_COUNT] = {\n')
        for code, b18, ch in glyphs:
            b_str = ', '.join(f'0x{b:02X}' for b in b18)
            f.write(f'    {{ 0x{code:04X}, {{ {b_str} }} }}, // {ch}\n')
        f.write('};\n\n')
        
        # Fast binary search
        f.write('''inline const uint8_t* findHangulBitmap(uint16_t unicode) {
    int left = 0, right = static_cast<int>(HANGUL_GLYPH_COUNT) - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (HANGUL_GLYPHS[mid].unicode == unicode) {
            return HANGUL_GLYPHS[mid].bitmap;
        }
        if (HANGUL_GLYPHS[mid].unicode < unicode) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return nullptr;
}

} // namespace JoseonRPG
''')

    print("Successfully generated src/ui/korean_font.hpp with 12x12 crisp Hangul bitmaps!")

if __name__ == '__main__':
    generate_korean_font_header()
