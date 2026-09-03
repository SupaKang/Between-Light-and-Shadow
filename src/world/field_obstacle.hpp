#pragma once
#include "../core/types.hpp"
#include <string>
#include <vector>

namespace JoseonRPG {

class Party;

enum class ObstacleType {
    IronBars,     // 무쇠 창살 (불가사리로 파쇄)
    HeavyBoulder, // 거대 바위 (두두리/두억시니로 밀어내기)
    WaterRapids,  // 영맥 급류 (이무기로 도하)
    CursedBriars  // 요기 덤불 (도깨비/불개로 화염 연소)
};

struct FieldObstacle {
    int id = 0;
    int mapId = 0;
    int x = 0;
    int y = 0;
    ObstacleType type = ObstacleType::IronBars;
    bool cleared = false;
};

class FieldObstacleManager {
public:
    static void init();
    static FieldObstacle* getObstacleAt(int mapId, int x, int y);
    static bool tryClearObstacle(FieldObstacle& obs, const Party& party, std::string& outMsg);
    static const std::vector<FieldObstacle>& getAllObstacles();

private:
    static std::vector<FieldObstacle> s_obstacles;
};

} // namespace JoseonRPG
