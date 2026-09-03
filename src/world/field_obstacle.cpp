#include "field_obstacle.hpp"
#include "../gameplay/party.hpp"
#include "../audio/audio_engine.hpp"

namespace JoseonRPG {

std::vector<FieldObstacle> FieldObstacleManager::s_obstacles;

void FieldObstacleManager::init() {
    s_obstacles = {
        {1, 3, 20, 45, ObstacleType::CursedBriars, false},  // Map 3 북한산 요기 덤불
        {2, 7, 40, 20, ObstacleType::HeavyBoulder, false}, // Map 7 소백산 대협곡 거대 바위
        {3, 9, 25, 25, ObstacleType::IronBars, false},     // Map 9 무쇠광산 상층 무쇠 창살
        {4, 12, 30, 38, ObstacleType::WaterRapids, false}  // Map 12 남해안 포구 영맥 급류
    };
}

FieldObstacle* FieldObstacleManager::getObstacleAt(int mapId, int x, int y) {
    for (auto& obs : s_obstacles) {
        if (obs.mapId == mapId && obs.x == x && obs.y == y && !obs.cleared) {
            return &obs;
        }
    }
    return nullptr;
}

bool FieldObstacleManager::tryClearObstacle(FieldObstacle& obs, const Party& party, std::string& outMsg) {
    if (obs.cleared) return true;

    bool hasRequired = false;
    std::string helperName;

    for (size_t i = 0; i < party.getSize(); ++i) {
        const Yokai* y = party.getYokai(i);
        if (!y || y->isFainted()) continue;

        switch (obs.type) {
            case ObstacleType::CursedBriars:
                if (y->getElement() == Element::Fire || y->getId() == "YOKAI_001" || y->getId() == "YOKAI_019") {
                    hasRequired = true;
                    helperName = y->getName();
                }
                break;
            case ObstacleType::HeavyBoulder:
                if (y->getElement() == Element::Earth || y->getId() == "YOKAI_009" || y->getId() == "YOKAI_010" || y->getStats().atk >= 30) {
                    hasRequired = true;
                    helperName = y->getName();
                }
                break;
            case ObstacleType::IronBars:
                if (y->getId() == "YOKAI_003" || y->getTrait() == YokaiTrait::IronDiet) {
                    hasRequired = true;
                    helperName = y->getName();
                }
                break;
            case ObstacleType::WaterRapids:
                if (y->getElement() == Element::Water || y->getId() == "YOKAI_020") {
                    hasRequired = true;
                    helperName = y->getName();
                }
                break;
        }
        if (hasRequired) break;
    }

    if (hasRequired) {
        obs.cleared = true;
        AudioEngine::playSfx(SfxId::HitPhysical);

        switch (obs.type) {
            case ObstacleType::CursedBriars:
                outMsg = "★ [" + helperName + "]의 도깨비불로 요기 덤불을 불태워 길을 뚫었습니다! ★";
                break;
            case ObstacleType::HeavyBoulder:
                outMsg = "★ [" + helperName + "]의 엄청난 괴력으로 거대한 바위를 밀어냈습니다! ★";
                break;
            case ObstacleType::IronBars:
                outMsg = "★ [" + helperName + "]이(가) 무쇠 창살을 씹어삼켜 통로를 개방했습니다! ★";
                break;
            case ObstacleType::WaterRapids:
                outMsg = "★ [" + helperName + "]의 용오름 신법으로 거센 영맥 급류를 건넜습니다! ★";
                break;
        }
        return true;
    } else {
        AudioEngine::playSfx(SfxId::MenuCancel);
        switch (obs.type) {
            case ObstacleType::CursedBriars:
                outMsg = "가시덤불이 길을 막고 있습니다. (화속성 요괴의 불길이 필요합니다)";
                break;
            case ObstacleType::HeavyBoulder:
                outMsg = "거대한 바위가 길을 막고 있습니다. (두두리나 괴력을 지닌 토속성 요괴 필요)";
                break;
            case ObstacleType::IronBars:
                outMsg = "견고한 무쇠 창살입니다. (쇠를 먹는 불가사리가 필요합니다)";
                break;
            case ObstacleType::WaterRapids:
                outMsg = "거센 영맥 급류입니다. (수속성 요괴나 이무기의 신법 필요)";
                break;
        }
        return false;
    }
}

const std::vector<FieldObstacle>& FieldObstacleManager::getAllObstacles() {
    return s_obstacles;
}

} // namespace JoseonRPG
