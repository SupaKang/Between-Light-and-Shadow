#pragma once
#include "save_types.hpp"
#include "../gameplay/party.hpp"
#include "../gameplay/artifact.hpp"
#include "../gameplay/encyclopedia.hpp"
#include "../gameplay/quest.hpp"
#include <string>

namespace JoseonRPG {

struct GameRuntimeContext {
    int mapId = 0;
    int gridX = 7;
    int gridY = 6;
    int facing = 0;
    int money = 500;
    uint32_t playTimeSeconds = 0;
    Party* party = nullptr;
    ArtifactInventory* artifacts = nullptr;
    Encyclopedia* encyclopedia = nullptr;
    QuestManager* questManager = nullptr;
};

class SaveSystem {
public:
    static bool saveToSlot(int slotIndex, const GameRuntimeContext& ctx);
    static bool loadFromSlot(int slotIndex, GameRuntimeContext& ctx);
    static bool hasSaveFile(int slotIndex);
    static bool getSaveSummary(int slotIndex, std::string& outSummary);
    static void deleteSaveFile(int slotIndex);

    static uint16_t calculateChecksum(const SaveBlock& block);
    static std::string getSaveFilePath(int slotIndex);
};

} // namespace JoseonRPG
