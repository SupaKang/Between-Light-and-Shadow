#pragma once
#include "../core/types.hpp"
#include <vector>
#include <string>

namespace JoseonRPG {

class Renderer;

struct WarpTrigger {
    int fromX = 0;
    int fromY = 0;
    int targetMapId = 0;
    int targetX = 0;
    int targetY = 0;
};

class Tilemap {
public:
    Tilemap();

    void loadMap(int mapId);
    void render(Renderer& renderer, int cameraX, int cameraY) const;
    bool isSolid(int tileX, int tileY) const;
    const WarpTrigger* checkWarp(int tileX, int tileY) const;

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getMapId() const { return m_mapId; }
    const std::string& getMapName() const { return m_mapName; }

private:
    int m_mapId = 0;
    std::string m_mapName;
    int m_width = 20;
    int m_height = 12;
    std::vector<int> m_tiles;
    std::vector<bool> m_collision;
    std::vector<WarpTrigger> m_warps;
};

} // namespace JoseonRPG
