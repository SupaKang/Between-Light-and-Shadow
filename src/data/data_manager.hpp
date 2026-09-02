#pragma once
#include "../gameplay/yokai.hpp"
#include "../gameplay/artifact.hpp"
#include <vector>
#include <string_view>

namespace JoseonRPG {

class DataManager {
public:
    static void init();
    
    static const std::vector<Yokai>& getAllYokaiTemplates();
    static const std::vector<Skill>& getAllSkills();
    static const std::vector<Artifact>& getAllArtifacts();

    static Yokai createYokaiById(std::string_view id);
    static Artifact createArtifactById(std::string_view id);

private:
    static std::vector<Yokai> s_yokaiDatabase;
    static std::vector<Skill> s_skillDatabase;
    static std::vector<Artifact> s_artifactDatabase;
};

} // namespace JoseonRPG
