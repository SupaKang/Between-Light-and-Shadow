#pragma once
#include "../core/types.hpp"
#include <vector>

namespace JoseonRPG {

class Renderer;

class Tilemap {
public:
    Tilemap();

    void loadDefaultMap();
    void render(Renderer& renderer, int cameraX, int cameraY) const;
    bool isSolid(int tileX, int tileY) const;

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    int m_width = 20;
    int m_height = 12;
    std::vector<int> m_tiles;
    std::vector<bool> m_collision;
};

} // namespace JoseonRPG
