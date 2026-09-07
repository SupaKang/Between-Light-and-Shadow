#pragma once
#include <string>
#include <vector>
#include "status_rules.h"

namespace data {

struct SkillDef {
    std::string id;
    std::string name_ko;
    int power = 0;
    int qi_cost = 0;
    int accuracy = 100;
    status_rules::Kind status = status_rules::Kind::None;
    int seal_turns = 0;
};

struct YokaiDef {
    std::string id;
    std::string name_ko;
    std::string origin;
    std::string grade = "I";
    int level = 1;
    int hp = 100;
    int qi = 30;
    int attack = 10;
    int defense = 10;
    int speed = 10;
    std::string skills[4];
    int skill_count = 0;
};

struct ArtifactDef {
    std::string id;
    std::string name_ko;
    int attack_bonus = 0;
    int hp_loss_per_battle = 0;
    bool destroyable = true;
};

class Registry {
public:
    static std::string load_text(const char* path);
    static int integer(const std::string& text, const char* key, int fallback);
    static double number(const std::string& text, const char* key, double fallback);
    static std::string string_value(const std::string& text, const char* key, const std::string& fallback);
    static std::vector<std::string> string_list(const std::string& text, const char* key);

    static SkillDef parse_skill(const std::string& json);
    static YokaiDef parse_yokai(const std::string& json);
    static ArtifactDef parse_artifact(const std::string& json);
};

} // namespace data
