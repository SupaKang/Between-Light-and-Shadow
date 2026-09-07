#include "data_registry.h"
#include <fstream>

namespace data {

std::string Registry::load_text(const char* path) {
    std::ifstream f(path, std::ios::binary);
    if (!f.is_open()) return "";
    return std::string((std::istreambuf_iterator<char>(f)), {});
}

int Registry::integer(const std::string& text, const char* key, int fallback) {
    const std::string needle = std::string("\"") + key + "\"";
    const auto pos = text.find(needle);
    if (pos == std::string::npos) return fallback;
    const auto colon = text.find(':', pos);
    if (colon == std::string::npos) return fallback;
    try {
        return std::stoi(text.substr(colon + 1));
    } catch (...) {
        return fallback;
    }
}

double Registry::number(const std::string& text, const char* key, double fallback) {
    const std::string needle = std::string("\"") + key + "\"";
    const auto pos = text.find(needle);
    if (pos == std::string::npos) return fallback;
    const auto colon = text.find(':', pos);
    if (colon == std::string::npos) return fallback;
    try {
        return std::stod(text.substr(colon + 1));
    } catch (...) {
        return fallback;
    }
}

std::string Registry::string_value(const std::string& text, const char* key, const std::string& fallback) {
    const std::string needle = std::string("\"") + key + "\"";
    const auto pos = text.find(needle);
    if (pos == std::string::npos) return fallback;
    const auto start = text.find('"', text.find(':', pos) + 1);
    if (start == std::string::npos) return fallback;
    const auto end = text.find('"', start + 1);
    if (end == std::string::npos) return fallback;
    return text.substr(start + 1, end - start - 1);
}

std::vector<std::string> Registry::string_list(const std::string& text, const char* key) {
    std::vector<std::string> result;
    const std::string needle = std::string("\"") + key + "\"";
    const auto pos = text.find(needle);
    if (pos == std::string::npos) return result;
    const auto start_bracket = text.find('[', pos);
    if (start_bracket == std::string::npos) return result;
    const auto end_bracket = text.find(']', start_bracket);
    if (end_bracket == std::string::npos) return result;

    size_t cur = start_bracket + 1;
    while (cur < end_bracket) {
        auto q1 = text.find('"', cur);
        if (q1 == std::string::npos || q1 >= end_bracket) break;
        auto q2 = text.find('"', q1 + 1);
        if (q2 == std::string::npos || q2 > end_bracket) break;
        result.push_back(text.substr(q1 + 1, q2 - q1 - 1));
        cur = q2 + 1;
    }
    return result;
}

SkillDef Registry::parse_skill(const std::string& json) {
    SkillDef def;
    def.id = string_value(json, "id", "");
    def.name_ko = string_value(json, "name_ko", "");
    def.power = integer(json, "power", 0);
    def.qi_cost = integer(json, "qi_cost", 0);
    def.accuracy = integer(json, "accuracy", 100);
    def.seal_turns = integer(json, "seal_turns", 0);
    std::string st = string_value(json, "status", "");
    if (st == "burn") def.status = status_rules::Kind::Burn;
    else if (st == "freeze") def.status = status_rules::Kind::Freeze;
    else if (st == "seal") def.status = status_rules::Kind::Seal;
    else if (st == "paralysis") def.status = status_rules::Kind::Paralysis;
    else if (st == "fear") def.status = status_rules::Kind::Fear;
    else def.status = status_rules::Kind::None;
    return def;
}

YokaiDef Registry::parse_yokai(const std::string& json) {
    YokaiDef def;
    def.id = string_value(json, "id", "");
    def.name_ko = string_value(json, "name_ko", "");
    def.origin = string_value(json, "origin", "");
    def.grade = string_value(json, "grade", "I");
    def.level = integer(json, "level", 1);
    def.hp = integer(json, "hp", 100);
    def.qi = integer(json, "qi", 30);
    def.attack = integer(json, "attack", 10);
    def.defense = integer(json, "defense", 10);
    def.speed = integer(json, "speed", 10);
    auto list = string_list(json, "skills");
    def.skill_count = static_cast<int>(list.size() < 4 ? list.size() : 4);
    for (int i = 0; i < def.skill_count; ++i) {
        def.skills[i] = list[i];
    }
    return def;
}

ArtifactDef Registry::parse_artifact(const std::string& json) {
    ArtifactDef def;
    def.id = string_value(json, "id", "");
    def.name_ko = string_value(json, "name_ko", "");
    def.attack_bonus = integer(json, "attack_bonus", 0);
    def.hp_loss_per_battle = integer(json, "hp_loss_per_battle", 0);
    def.destroyable = (json.find("\"destroyable\": true") != std::string::npos ||
                       json.find("\"destroyable\":true") != std::string::npos);
    return def;
}

} // namespace data
