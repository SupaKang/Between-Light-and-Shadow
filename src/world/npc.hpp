#pragma once
#include <string>
#include <vector>

namespace JoseonRPG {

enum class NPCActionType {
    None,
    TavernRest,     // 주막 휴식 (파티 전원 HP/Qi 완치)
    QuestTrigger,   // 퀘스트 대화 및 수주/진행
    TalismanShop,   // 벽사 부적 및 유물 거래
    BossEncounter   // 음양당 보스전 트리거
};

struct NPC {
    std::string id;
    int mapId = 0;
    int gridX = 0;
    int gridY = 0;
    int spriteId = 1;
    std::string nameKo;
    std::string titleKo;
    std::vector<std::string> dialogue;
    NPCActionType actionType = NPCActionType::None;
    std::string associatedQuestId;
};

} // namespace JoseonRPG
