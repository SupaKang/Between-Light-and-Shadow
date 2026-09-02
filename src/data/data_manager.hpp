#pragma once
#include "../gameplay/yokai.hpp"
#include "../gameplay/artifact.hpp"
#include "../gameplay/encyclopedia.hpp"
#include "../gameplay/quest.hpp"
#include "../world/npc.hpp"
#include <vector>
#include <string_view>

namespace JoseonRPG {

class DataManager {
public:
    static void init();
    
    static const std::vector<Yokai>& getAllYokaiTemplates();
    static const std::vector<Skill>& getAllSkills();
    static const std::vector<Artifact>& getAllArtifacts();
    static const std::vector<NPC>& getAllNPCs();

    static Encyclopedia& getEncyclopedia() { return s_encyclopedia; }
    static QuestManager& getQuestManager() { return s_questManager; }

    static Yokai createYokaiById(std::string_view id);
    static Yokai createYokaiByNumber(int number);
    static Artifact createArtifactById(std::string_view id);
    static std::vector<NPC> getNPCsForMap(int mapId);

private:
    static std::vector<Yokai> s_yokaiDatabase;
    static std::vector<Skill> s_skillDatabase;
    static std::vector<Artifact> s_artifactDatabase;
    static std::vector<NPC> s_npcDatabase;
    static Encyclopedia s_encyclopedia;
    static QuestManager s_questManager;
};

} // namespace JoseonRPG
