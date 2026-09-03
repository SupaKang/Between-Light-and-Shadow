#include "save_system.hpp"
#include "../data/data_manager.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace JoseonRPG {

std::string SaveSystem::getSaveFilePath(int slotIndex) {
    return "save_slot_" + std::to_string(slotIndex) + ".sav";
}

uint16_t SaveSystem::calculateChecksum(const SaveBlock& block) {
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&block);
    size_t len = sizeof(SaveBlock) - sizeof(uint16_t); // Exclude the checksum field itself

    uint16_t sum1 = 0;
    uint16_t sum2 = 0;
    for (size_t i = 0; i < len; ++i) {
        sum1 = (sum1 + bytes[i]) % 255;
        sum2 = (sum2 + sum1) % 255;
    }
    return (sum2 << 8) | sum1;
}

bool SaveSystem::hasSaveFile(int slotIndex) {
    std::ifstream file(getSaveFilePath(slotIndex), std::ios::binary);
    if (!file.is_open()) return false;

    SaveBlock block;
    file.read(reinterpret_cast<char*>(&block), sizeof(SaveBlock));
    if (file.gcount() != sizeof(SaveBlock)) return false;

    if (block.magic != 0x4750524A) return false;
    return (calculateChecksum(block) == block.checksum);
}

bool SaveSystem::getSaveSummary(int slotIndex, std::string& outSummary) {
    std::ifstream file(getSaveFilePath(slotIndex), std::ios::binary);
    if (!file.is_open()) return false;

    SaveBlock block;
    file.read(reinterpret_cast<char*>(&block), sizeof(SaveBlock));
    if (file.gcount() != sizeof(SaveBlock)) return false;
    if (block.magic != 0x4750524A || calculateChecksum(block) != block.checksum) return false;

    std::ostringstream oss;
    oss << "Map:" << static_cast<int>(block.mapId) 
        << " | Party:" << static_cast<int>(block.partySize)
        << " | Money:" << block.money << "G";
    outSummary = oss.str();
    return true;
}

void SaveSystem::deleteSaveFile(int slotIndex) {
    std::remove(getSaveFilePath(slotIndex).c_str());
}

bool SaveSystem::saveToSlot(int slotIndex, const GameRuntimeContext& ctx) {
    SaveBlock block = {};
    block.magic = 0x4750524A;
    block.version = 1;
    block.playTimeSeconds = ctx.playTimeSeconds;

    // 1. Player Info
    block.mapId = static_cast<uint8_t>(ctx.mapId);
    block.gridX = static_cast<uint8_t>(ctx.gridX);
    block.gridY = static_cast<uint8_t>(ctx.gridY);
    block.facing = static_cast<uint8_t>(ctx.facing);
    block.money = static_cast<uint32_t>(ctx.money);

    // 2. Party Serialization
    if (ctx.party) {
        block.partySize = static_cast<uint8_t>(std::min<size_t>(3, ctx.party->getSize()));
        for (size_t i = 0; i < block.partySize; ++i) {
            const Yokai* y = ctx.party->getYokai(i);
            if (!y) continue;
            SaveYokaiSlot& slot = block.party[i];
            
            std::strncpy(slot.id, y->getId().data(), sizeof(slot.id) - 1);
            slot.level = static_cast<uint8_t>(y->getLevel());
            slot.grade = static_cast<uint8_t>(y->getGrade());
            slot.exp = static_cast<uint32_t>(y->getExp());
            slot.curHp = static_cast<int16_t>(y->getStats().hp);
            slot.curQi = static_cast<int16_t>(y->getStats().qi);
            slot.statusEffect = static_cast<uint8_t>(y->getStatus().effect);

            const auto& skills = y->getSkills();
            for (size_t s = 0; s < 4 && s < skills.size(); ++s) {
                std::strncpy(slot.skillIds[s], skills[s].id.data(), sizeof(slot.skillIds[s]) - 1);
            }
        }
    }

    // 3. Artifact Serialization
    if (ctx.artifacts) {
        const auto& list = ctx.artifacts->getArtifacts();
        block.artifactCount = static_cast<uint8_t>(std::min<size_t>(8, list.size()));
        for (size_t i = 0; i < block.artifactCount; ++i) {
            std::strncpy(block.artifactIds[i], list[i].id.c_str(), sizeof(block.artifactIds[i]) - 1);
        }
    }

    // 4. Encyclopedia Bitset Serialization (108 slots)
    if (ctx.encyclopedia) {
        for (int num = 1; num <= 108; ++num) {
            const auto* entry = ctx.encyclopedia->getEntry(num);
            if (!entry) continue;

            int byteIdx = (num - 1) / 8;
            int bitIdx = (num - 1) % 8;

            if (entry->status == DiscoveryStatus::Seen || entry->status == DiscoveryStatus::Captured) {
                block.encyclopediaSeen[byteIdx] |= (1 << bitIdx);
            }
            if (entry->status == DiscoveryStatus::Captured) {
                block.encyclopediaCaptured[byteIdx] |= (1 << bitIdx);
            }
        }
    }

    // 5. Quest Serialization
    if (ctx.questManager) {
        const Quest* activeMq = nullptr;
        for (const auto& q : ctx.questManager->getAllQuests()) {
            if (q.type == QuestType::Main) {
                if (q.state == QuestState::InProgress) {
                    activeMq = &q;
                    break;
                } else if (q.state == QuestState::Completed) {
                    activeMq = &q;
                }
            }
        }
        if (activeMq) {
            std::strncpy(block.currentMainQuestId, activeMq->id.c_str(), sizeof(block.currentMainQuestId) - 1);
            block.mainQuestState = static_cast<uint8_t>(activeMq->state);
            block.mainQuestStep = static_cast<uint8_t>(activeMq->currentObjectiveIndex);
        }
    }

    // 6. Checksum calculation
    block.checksum = calculateChecksum(block);

    // 7. Write to binary file
    std::ofstream file(getSaveFilePath(slotIndex), std::ios::binary);
    if (!file.is_open()) return false;

    file.write(reinterpret_cast<const char*>(&block), sizeof(SaveBlock));
    return file.good();
}

bool SaveSystem::loadFromSlot(int slotIndex, GameRuntimeContext& ctx) {
    std::ifstream file(getSaveFilePath(slotIndex), std::ios::binary);
    if (!file.is_open()) return false;

    SaveBlock block;
    file.read(reinterpret_cast<char*>(&block), sizeof(SaveBlock));
    if (file.gcount() != sizeof(SaveBlock)) return false;

    if (block.magic != 0x4750524A) return false;
    if (calculateChecksum(block) != block.checksum) return false;

    // 1. Restore Player Info
    ctx.mapId = block.mapId;
    ctx.gridX = block.gridX;
    ctx.gridY = block.gridY;
    ctx.facing = block.facing;
    ctx.money = block.money;
    ctx.playTimeSeconds = block.playTimeSeconds;

    // 2. Restore Party
    if (ctx.party) {
        ctx.party->clear();
        for (uint8_t i = 0; i < block.partySize; ++i) {
            const SaveYokaiSlot& slot = block.party[i];
            Yokai y = DataManager::createYokaiById(slot.id);
            if (!y.getId().empty()) {
                y.gainExp(slot.exp);
                // Adjust level and grade if different
                while (static_cast<uint8_t>(y.getGrade()) < slot.grade) {
                    y.promoteGrade();
                }
                y.setHp(slot.curHp);
                y.setQi(slot.curQi);
                ctx.party->addYokai(y);
            }
        }
    }

    // 3. Restore Artifacts
    if (ctx.artifacts) {
        ctx.artifacts->clear();
        for (uint8_t i = 0; i < block.artifactCount; ++i) {
            Artifact art = DataManager::createArtifactById(block.artifactIds[i]);
            if (!art.id.empty()) {
                ctx.artifacts->addArtifact(art);
            }
        }
    }

    // 4. Restore Encyclopedia Bitsets
    if (ctx.encyclopedia) {
        for (int num = 1; num <= 108; ++num) {
            int byteIdx = (num - 1) / 8;
            int bitIdx = (num - 1) % 8;

            bool isSeen = (block.encyclopediaSeen[byteIdx] & (1 << bitIdx)) != 0;
            bool isCaptured = (block.encyclopediaCaptured[byteIdx] & (1 << bitIdx)) != 0;

            if (isCaptured) {
                ctx.encyclopedia->markCaptured(num);
            } else if (isSeen) {
                ctx.encyclopedia->markSeen(num);
            }
        }
    }

    // 5. Restore Quests
    if (ctx.questManager) {
        Quest* mq = ctx.questManager->getQuest(block.currentMainQuestId);
        if (mq) {
            mq->state = static_cast<QuestState>(block.mainQuestState);
            mq->currentObjectiveIndex = block.mainQuestStep;

            for (auto& q : const_cast<std::vector<Quest>&>(ctx.questManager->getAllQuests())) {
                if (q.type == QuestType::Main && q.chapter < mq->chapter) {
                    q.state = QuestState::Completed;
                }
            }
        }
    }

    return true;
}

} // namespace JoseonRPG
