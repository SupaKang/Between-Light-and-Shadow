#!/usr/bin/env python3
import os
import glob
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont

# 1. Collect all Korean text in data/ and key game strings
korean_chars = set()

for path in Path("data").glob("**/*.json"):
    try:
        content = path.read_text(encoding="utf-8")
        for ch in content:
            if 0xAC00 <= ord(ch) <= 0xD7A3:
                korean_chars.add(ch)
    except Exception:
        pass

# Additional UI, dialogue, and story terms
vocab = [
    "도선사 어귀 마을", "북한산 고갯길", "도선사 대웅전", "주막 주모", "고대 신목", "안내판", "괴승 묘각",
    "기본 공격", "잔불 부적", "서리 숨결", "봉인 문양", "도깨비", "신목의 파편",
    "음양견문록", "체력", "영력", "도감", "파티", "유물", "저장", "불러오기", "계약", "전투", "도주",
    "도깨비가 나타났다!", "계약에 성공했다!", "주막에 오신 것을 환영하오!", "체력과 영력이 모두 회복되었소.",
    "음양당의 사악한 의식을 막아야 하오!", "성지가 정화되었습니다.", "선택", "실행", "퇴각", "완료", "실패", "발견",
    "화상", "빙결", "봉인", "마비", "공포", "위력", "명중", "지속", "턴", "보너스", "파괴", "조선시대",
    "슬롯", "경험치", "레벨", "상태", "현재 임무", "사설 음양학파", "관상감 벽사청", "괴승 묘각을 처치하라",
    "생", "덤", "합", "히", "쉬", "쪽", "격", "파", "승", "리", "패", "배", "도", "주", "야생 도깨비가 나타났다!", "보스 전투: 음양당 괴승 묘각이 덤벼든다!", "계약 성공! 도깨비가 동료로 합류했다!", "전투 승리! 경험치를 획득했다!", "주막에 오신 것을 환영하오! 편히 쉬어가시게.", "음양당의 요기가 북쪽 도선사에 번지고 있소!", "체력과 영력이 모두 회복되었소.", "사악한 요기가 물러가고 대웅전이 정화되었습니다.", "적 체력을 50% 이하로 깎아야 함", "성공률", "요괴와 영맥 계약 체결", "전투를 이탈하여 필드로 복귀", "필요 영력", "보유", "봉인됨", "턴 동안 사용 불가", "기본공격", "잔불부적", "서리숨결", "봉인문양"
]

for phrase in vocab:
    for ch in phrase:
        if 0xAC00 <= ord(ch) <= 0xD7A3:
            korean_chars.add(ch)

sorted_chars = sorted(list(korean_chars), key=lambda c: ord(c))
print(f"Rasterizing {len(sorted_chars)} Korean glyphs...")

# 2. Font rendering
font_path = "/System/Library/Fonts/Supplemental/AppleGothic.ttf"
if not os.path.exists(font_path):
    candidates = glob.glob("/System/Library/Fonts/**/AppleSDGothic*.ttc", recursive=True)
    font_path = candidates[0] if candidates else None

if not font_path:
    raise RuntimeError("No Korean TrueType font found on system!")

font = ImageFont.truetype(font_path, 11)

glyph_entries = []

for ch in sorted_chars:
    cp = ord(ch)
    img = Image.new("1", (12, 12), 0)
    draw = ImageDraw.Draw(img)
    bbox = draw.textbbox((0, 0), ch, font=font)
    w = bbox[2] - bbox[0]
    h = bbox[3] - bbox[1]
    ox = max(0, (12 - w) // 2)
    oy = max(0, (12 - h) // 2 - 1)
    draw.text((ox, oy), ch, font=font, fill=1)

    rows = []
    for y in range(12):
        row_val = 0
        for x in range(12):
            if img.getpixel((x, y)):
                row_val |= (1 << (11 - x))
        rows.append(f"0x{row_val:03X}")
    rows_str = ", ".join(rows)
    glyph_entries.append(f"    {{0x{cp:04X}, {{{rows_str}}}}}, // {ch}")

header_content = f"""#pragma once
#include <cstdint>
#include <cstddef>
#include "font.h"

namespace korean_font {{

struct Glyph {{
    std::uint32_t codepoint;
    std::uint16_t rows[12];
}};

// Total {len(sorted_chars)} rasterized 12x12 Korean syllables (sorted by Unicode codepoint)
inline const Glyph glyphs[] = {{
{chr(10).join(glyph_entries)}
}};

constexpr std::size_t GlyphCount = sizeof(glyphs) / sizeof(glyphs[0]);

inline const std::uint16_t* find_glyph(std::uint32_t cp) {{
    std::size_t low = 0, high = GlyphCount;
    while (low < high) {{
        std::size_t mid = low + (high - low) / 2;
        if (glyphs[mid].codepoint < cp) {{
            low = mid + 1;
        }} else if (glyphs[mid].codepoint > cp) {{
            high = mid;
        }} else {{
            return glyphs[mid].rows;
        }}
    }}
    return nullptr;
}}

inline void draw_korean_char(std::uint32_t* buf, int buf_w, int buf_h, std::uint32_t cp, int ox, int oy, int scale, std::uint32_t color) {{
    const std::uint16_t* rows = find_glyph(cp);
    if (!rows) {{
        for (int y = 0; y < 12; ++y) {{
            for (int x = 0; x < 12; ++x) {{
                if (x == 0 || x == 11 || y == 0 || y == 11) {{
                    for (int py = 0; py < scale; ++py) {{
                        for (int px = 0; px < scale; ++px) {{
                            int dx = ox + x * scale + px;
                            int dy = oy + y * scale + py;
                            if (dx >= 0 && dx < buf_w && dy >= 0 && dy < buf_h) {{
                                buf[dy * buf_w + dx] = color;
                            }}
                        }}
                    }}
                }}
            }}
        }}
        return;
    }}

    for (int y = 0; y < 12; ++y) {{
        std::uint16_t r = rows[y];
        for (int x = 0; x < 12; ++x) {{
            if ((r >> (11 - x)) & 1) {{
                for (int py = 0; py < scale; ++py) {{
                    for (int px = 0; px < scale; ++px) {{
                        int dx = ox + x * scale + px;
                        int dy = oy + y * scale + py;
                        if (dx >= 0 && dx < buf_w && dy >= 0 && dy < buf_h) {{
                            buf[dy * buf_w + dx] = color;
                        }}
                    }}
                }}
            }}
        }}
    }}
}}

inline void draw_utf8_text(std::uint32_t* buf, int buf_w, int buf_h, const char* str, int x, int y, int scale, std::uint32_t color = 0xFFE8D8A0u) {{
    const unsigned char* p = reinterpret_cast<const unsigned char*>(str);
    while (*p) {{
        if (*p < 0x80) {{
            if (*p == ' ') {{
                x += 6 * scale;
                ++p;
                continue;
            }}
            font::draw_char(buf, buf_w, buf_h, static_cast<char>(*p), x, y + 2 * scale, scale, color);
            x += 6 * scale;
            ++p;
        }} else if ((*p & 0xE0) == 0xC0) {{
            std::uint32_t cp = ((*p & 0x1F) << 6) | (p[1] & 0x3F);
            p += 2;
            draw_korean_char(buf, buf_w, buf_h, cp, x, y, scale, color);
            x += 13 * scale;
        }} else if ((*p & 0xF0) == 0xE0) {{
            std::uint32_t cp = ((*p & 0x0F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F);
            p += 3;
            draw_korean_char(buf, buf_w, buf_h, cp, x, y, scale, color);
            x += 13 * scale;
        }} else if ((*p & 0xF8) == 0xF0) {{
            std::uint32_t cp = ((*p & 0x07) << 18) | ((p[1] & 0x3F) << 12) | ((p[2] & 0x3F) << 6) | (p[3] & 0x3F);
            p += 4;
            draw_korean_char(buf, buf_w, buf_h, cp, x, y, scale, color);
            x += 13 * scale;
        }} else {{
            ++p;
        }}
    }}
}}

}} // namespace korean_font
"""

with open("src/korean_font.h", "w", encoding="utf-8") as f:
    f.write(header_content)

print(f"Generated src/korean_font.h with {len(sorted_chars)} glyphs ({len(header_content)} bytes)")
