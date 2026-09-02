#include "tilemap.hpp"
#include "../core/renderer.hpp"

namespace JoseonRPG {

Tilemap::Tilemap() {
    loadDefaultMap();
}

void Tilemap::loadDefaultMap() {
    m_width = 20;
    m_height = 12;

    m_tiles = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 2, 2, 2, 1, 3, 3, 3, 3, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
        1, 2, 0, 2, 1, 3, 0, 0, 3, 1, 2, 0, 0, 0, 0, 0, 0, 0, 2, 1,
        1, 2, 0, 2, 2, 2, 0, 0, 2, 2, 2, 0, 4, 4, 0, 4, 4, 0, 2, 1,
        1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 4, 4, 0, 2, 1,
        1, 2, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1,
        1, 2, 2, 2, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1,
        1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 2, 1,
        1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 0, 0, 0, 2, 1,
        1, 0, 0, 2, 2, 2, 2, 0, 0, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 1,
        1, 0, 0, 0, 0, 0, 2, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    m_collision = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };
}

void Tilemap::render(Renderer& renderer, int cameraX, int cameraY) const {
    int startCol = cameraX / TILE_SIZE;
    int endCol = (cameraX + SCREEN_WIDTH) / TILE_SIZE + 1;
    int startRow = cameraY / TILE_SIZE;
    int endRow = (cameraY + SCREEN_HEIGHT) / TILE_SIZE + 1;

    startCol = std::max(0, startCol);
    endCol = std::min(m_width, endCol);
    startRow = std::max(0, startRow);
    endRow = std::min(m_height, endRow);

    for (int r = startRow; r < endRow; ++r) {
        for (int c = startCol; c < endCol; ++c) {
            int tileId = m_tiles[r * m_width + c];
            int screenX = c * TILE_SIZE - cameraX;
            int screenY = r * TILE_SIZE - cameraY;
            renderer.drawTileProcedural(screenX, screenY, tileId);
        }
    }
}

bool Tilemap::isSolid(int tileX, int tileY) const {
    if (tileX < 0 || tileX >= m_width || tileY < 0 || tileY >= m_height) return true;
    return m_collision[tileY * m_width + tileX];
}

} // namespace JoseonRPG
