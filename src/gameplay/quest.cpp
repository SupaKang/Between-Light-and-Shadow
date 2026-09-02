#include "quest.hpp"

namespace JoseonRPG {

QuestManager::QuestManager() {
    m_quests.reserve(16);
}

void QuestManager::registerQuest(Quest quest) {
    m_quests.push_back(std::move(quest));
}

bool QuestManager::startQuest(const std::string& questId) {
    for (auto& q : m_quests) {
        if (q.id == questId) {
            if (q.state == QuestState::NotStarted) {
                q.state = QuestState::InProgress;
                q.currentObjectiveIndex = 0;
                return true;
            }
            return false;
        }
    }
    return false;
}

bool QuestManager::advanceQuest(const std::string& questId) {
    for (auto& q : m_quests) {
        if (q.id == questId && q.state == QuestState::InProgress) {
            q.currentObjectiveIndex++;
            if (q.currentObjectiveIndex >= q.objectives.size()) {
                q.state = QuestState::Completed;
            }
            return true;
        }
    }
    return false;
}

bool QuestManager::completeQuest(const std::string& questId) {
    for (auto& q : m_quests) {
        if (q.id == questId) {
            q.state = QuestState::Completed;
            q.currentObjectiveIndex = q.objectives.size();
            return true;
        }
    }
    return false;
}

Quest* QuestManager::getQuest(const std::string& questId) {
    for (auto& q : m_quests) {
        if (q.id == questId) return &q;
    }
    return nullptr;
}

const Quest* QuestManager::getQuest(const std::string& questId) const {
    for (const auto& q : m_quests) {
        if (q.id == questId) return &q;
    }
    return nullptr;
}

std::vector<const Quest*> QuestManager::getActiveQuests() const {
    std::vector<const Quest*> active;
    for (const auto& q : m_quests) {
        if (q.state == QuestState::InProgress) {
            active.push_back(&q);
        }
    }
    return active;
}

} // namespace JoseonRPG
