#pragma once
#include "../core/types.hpp"

namespace JoseonRPG {

class Renderer;

class FontRenderer {
public:
    static void drawChar(Renderer& renderer, int x, int y, char c, Color color);
    static void drawHangul(Renderer& renderer, int x, int y, uint16_t unicode, Color color);
    static void drawText(Renderer& renderer, int x, int y, std::string_view text, Color color);
    static void drawTextShadowed(Renderer& renderer, int x, int y, std::string_view text, Color color, Color shadowColor = Palette::Black);
};

} // namespace JoseonRPG
