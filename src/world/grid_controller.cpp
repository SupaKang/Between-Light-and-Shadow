#include "grid_controller.hpp"
#include <cmath>
#include <algorithm>

namespace JoseonRPG {

GridController::GridController(int gridX, int gridY, Direction facing)
    : m_gridX(gridX), m_gridY(gridY), m_targetGridX(gridX), m_targetGridY(gridY), m_facing(facing) {}

void GridController::setPosition(int gridX, int gridY) {
    m_gridX = gridX;
    m_gridY = gridY;
    m_targetGridX = gridX;
    m_targetGridY = gridY;
    m_isMoving = false;
    m_stepProgress = 0.0f;
}

bool GridController::tryMove(Direction dir, std::function<bool(int tx, int ty)> isWalkable) {
    if (m_isMoving) return false;

    // Turn to face direction
    m_facing = dir;

    int dx = 0;
    int dy = 0;
    switch (dir) {
        case Direction::North: dy = -1; break;
        case Direction::South: dy = 1; break;
        case Direction::West:  dx = -1; break;
        case Direction::East:  dx = 1; break;
    }

    int nextX = m_gridX + dx;
    int nextY = m_gridY + dy;

    if (isWalkable && isWalkable(nextX, nextY)) {
        m_targetGridX = nextX;
        m_targetGridY = nextY;
        m_isMoving = true;
        m_stepProgress = 0.0f;
        m_walkCycle = (m_walkCycle + 1) % 2;
        return true;
    }

    return false;
}

void GridController::update(float dt) {
    if (!m_isMoving) return;

    m_stepProgress += dt / m_stepDuration;
    if (m_stepProgress >= 1.0f) {
        m_stepProgress = 0.0f;
        m_isMoving = false;
        m_gridX = m_targetGridX;
        m_gridY = m_targetGridY;

        if (m_onStepComplete) {
            m_onStepComplete(m_gridX, m_gridY);
        }
    }
}

int GridController::getPixelX() const {
    if (!m_isMoving) {
        return m_gridX * TILE_SIZE;
    }
    float t = std::clamp(m_stepProgress, 0.0f, 1.0f);
    float px = (1.0f - t) * (m_gridX * TILE_SIZE) + t * (m_targetGridX * TILE_SIZE);
    return static_cast<int>(std::round(px));
}

int GridController::getPixelY() const {
    if (!m_isMoving) {
        return m_gridY * TILE_SIZE;
    }
    float t = std::clamp(m_stepProgress, 0.0f, 1.0f);
    float py = (1.0f - t) * (m_gridY * TILE_SIZE) + t * (m_targetGridY * TILE_SIZE);
    return static_cast<int>(std::round(py));
}

int GridController::getAnimFrame() const {
    if (!m_isMoving) return 0;

    // Walk animation cycle: 0 -> 1 -> 0 -> 2
    if (m_stepProgress < 0.25f) return (m_walkCycle == 0) ? 1 : 2;
    if (m_stepProgress < 0.50f) return 0;
    if (m_stepProgress < 0.75f) return (m_walkCycle == 0) ? 2 : 1;
    return 0;
}

} // namespace JoseonRPG
