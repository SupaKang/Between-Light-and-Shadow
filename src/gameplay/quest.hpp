#pragma once
#include <string>
#include <vector>

namespace JoseonRPG {

enum class QuestType {
    Main,
    Side
};

enum class QuestState {
    NotStarted,
    InProgress,
    Completed
};

struct QuestReward {
    int exp = 0;
    int money = 0;
    std::string artifactId;
};

struct Quest {
    std::string id;
    QuestType type;
    std::string titleKo;
    int chapter = 1;
    std::string summary;
    std::vector<std::string> objectives;
    size_t currentObjectiveIndex = 0;
    QuestReward reward;
    QuestState state = QuestState::NotStarted;

    bool isComplete() const { return state == QuestState::Completed; }
    const std::string& getCurrentObjective() const {
        if (currentObjectiveIndex < objectives.size()) {
            return objectives[currentObjectiveIndex];
        }
        static std::string completeStr = "완료됨";
        return completeStr;
    }
};

class QuestManager {
public:
    QuestManager();

    void registerQuest(Quest quest);
    bool startQuest(const std::string& questId);
    bool advanceQuest(const std::string& questId);
    bool completeQuest(const std::string& questId);

    Quest* getQuest(const std::string& questId);
    const Quest* getQuest(const std::string& questId) const;

    const std::vector<Quest>& getAllQuests() const { return m_quests; }
    std::vector<const Quest*> getActiveQuests() const;

private:
    std::vector<Quest> m_quests;
};

} // namespace JoseonRPG
