#pragma once
#include "../core/types.hpp"
#include <functional>

namespace JoseonRPG {

enum class Direction : uint8_t {
    South = 0,
    North = 1,
    West  = 2,
    East  = 3
};

class Tilemap;

class GridController {
public:
    GridController(int gridX = 7, int gridY = 6, Direction facing = Direction::South);

    void setPosition(int gridX, int gridY);
    void setFacing(Direction dir) { m_facing = dir; }

    void update(float dt);
    
    // Attempt to move or face in direction (returns true if a step started)
    bool tryMove(Direction dir, std::function<bool(int tx, int ty)> isWalkable);

    bool isMoving() const { return m_isMoving; }
    int getGridX() const { return m_gridX; }
    int getGridY() const { return m_gridY; }
    Direction getFacing() const { return m_facing; }

    int getPixelX() const;
    int getPixelY() const;
    int getAnimFrame() const;

    void setOnStepComplete(std::function<void(int gx, int gy)> callback) {
        m_onStepComplete = std::move(callback);
    }

private:
    int m_gridX = 7;
    int m_gridY = 6;
    int m_targetGridX = 7;
    int m_targetGridY = 6;

    Direction m_facing = Direction::South;
    bool m_isMoving = false;
    float m_stepProgress = 0.0f;
    float m_stepDuration = 0.16f; // Time in seconds per 1-tile step
    int m_walkCycle = 0;

    std::function<void(int gx, int gy)> m_onStepComplete = nullptr;
};

} // namespace JoseonRPG
