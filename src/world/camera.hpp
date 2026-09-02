#pragma once
#include "../core/types.hpp"

namespace JoseonRPG {

class Camera {
public:
    Camera() = default;

    void update(int targetX, int targetY, int mapWidthTiles, int mapHeightTiles) {
        int targetCamX = targetX - SCREEN_WIDTH / 2;
        int targetCamY = targetY - SCREEN_HEIGHT / 2;

        int maxCamX = std::max(0, mapWidthTiles * TILE_SIZE - SCREEN_WIDTH);
        int maxCamY = std::max(0, mapHeightTiles * TILE_SIZE - SCREEN_HEIGHT);

        m_x = std::clamp(targetCamX, 0, maxCamX);
        m_y = std::clamp(targetCamY, 0, maxCamY);
    }

    int getX() const { return m_x; }
    int getY() const { return m_y; }

private:
    int m_x = 0;
    int m_y = 0;
};

} // namespace JoseonRPG
