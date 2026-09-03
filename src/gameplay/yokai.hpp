#pragma once
#include "../core/types.hpp"
#include "../battle/status_effects.hpp"
#include <string>
#include <vector>
#include <array>

namespace JoseonRPG {

struct Skill {
    std::string id;
    std::string name;
    int power = 40;
    int qiCost = 10;
    Element element = Element::Physical;
    int accuracy = 95;
    StatusEffect statusEffect = StatusEffect::None;
    int statusChance = 0;
};

struct Stats {
    int maxHp = 100;
    int hp = 100;
    int maxQi = 100;
    int qi = 100;
    int atk = 20;
    int def = 15;
    int spd = 12;
};

class Yokai {
public:
    Yokai() = default;
    Yokai(std::string id, std::string name, YokaiGrade grade, Element element, Stats baseStats)
        : Yokai(1, std::move(id), std::move(name), grade, element, baseStats) {}
    Yokai(int number, std::string id, std::string name, YokaiGrade grade, Element element,
          Stats baseStats, std::string origin = "KOREAN_FOLKLORE", std::string lore = "");

    void calculateStats();
    void gainExp(int amount);
    void levelUp();

    bool canPromote() const;
    bool promoteGrade();

    bool takeDamage(int amount);
    void healHp(int amount);
    void setHp(int hp);
    bool consumeQi(int amount);
    void restoreQi(int amount);
    void setQi(int qi);

    // Getters
    int getNumber() const { return m_number; }
    const std::string& getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    YokaiGrade getGrade() const { return m_grade; }
    Element getElement() const { return m_element; }
    int getLevel() const { return m_level; }
    int getExp() const { return m_exp; }
    int getExpToNextLevel() const;
    const std::string& getOrigin() const { return m_origin; }
    const std::string& getLore() const { return m_lore; }
    const Stats& getStats() const { return m_stats; }
    Stats& getStats() { return m_stats; }
    bool isFainted() const { return m_stats.hp <= 0; }

    // Skills
    const std::vector<Skill>& getSkills() const { return m_skills; }
    bool addSkill(const Skill& skill);

    // Status Effect
    const StatusInstance& getStatus() const { return m_status; }
    StatusInstance& getStatus() { return m_status; }
    void clearStatus() { m_status = StatusInstance{}; }

private:
    int m_number = 1;
    std::string m_id;
    std::string m_name;
    YokaiGrade m_grade = YokaiGrade::Grade1;
    Element m_element = Element::Physical;
    int m_level = 1;
    int m_exp = 0;
    std::string m_origin;
    std::string m_lore;

    Stats m_baseStats;
    Stats m_stats;

    std::vector<Skill> m_skills;
    StatusInstance m_status;
};

} // namespace JoseonRPG
