#include "yokai.hpp"
#include <cmath>
#include <algorithm>

namespace JoseonRPG {

static float getGradeMultiplier(YokaiGrade grade) {
    switch (grade) {
        case YokaiGrade::Grade1: return 1.00f;
        case YokaiGrade::Grade2: return 1.25f;
        case YokaiGrade::Grade3: return 1.50f;
        case YokaiGrade::Grade4: return 1.85f;
        case YokaiGrade::Grade5: return 2.30f;
        default: return 1.00f;
    }
}

Yokai::Yokai(int number, std::string id, std::string name, YokaiGrade grade, Element element,
             Stats baseStats, std::string origin, std::string lore)
    : m_number(number), m_id(std::move(id)), m_name(std::move(name)), m_grade(grade), m_element(element),
      m_origin(std::move(origin)), m_lore(std::move(lore)), m_baseStats(baseStats), m_level(1), m_exp(0) {
    calculateStats();
    m_stats.hp = m_stats.maxHp;
    m_stats.qi = m_stats.maxQi;
}

void Yokai::calculateStats() {
    float mult = getGradeMultiplier(m_grade);
    m_stats.maxHp = static_cast<int>((m_baseStats.maxHp + (m_level - 1) * 8) * mult);
    m_stats.maxQi = static_cast<int>((m_baseStats.maxQi + (m_level - 1) * 3) * mult);
    m_stats.atk = static_cast<int>((m_baseStats.atk + (m_level - 1) * 3) * mult);
    m_stats.def = static_cast<int>((m_baseStats.def + (m_level - 1) * 2) * mult);
    m_stats.spd = static_cast<int>((m_baseStats.spd + (m_level - 1) * 2) * mult);
}

int Yokai::getExpToNextLevel() const {
    if (m_level >= MAX_LEVEL) return 0;
    return 15 * m_level * m_level + 50 * m_level;
}

void Yokai::gainExp(int amount) {
    if (m_level >= MAX_LEVEL) return;
    m_exp += amount;
    while (m_level < MAX_LEVEL && m_exp >= getExpToNextLevel()) {
        m_exp -= getExpToNextLevel();
        levelUp();
    }
}

void Yokai::levelUp() {
    if (m_level >= MAX_LEVEL) return;
    m_level++;
    int prevMaxHp = m_stats.maxHp;
    int prevMaxQi = m_stats.maxQi;
    calculateStats();
    m_stats.hp += (m_stats.maxHp - prevMaxHp);
    m_stats.qi += (m_stats.maxQi - prevMaxQi);
}

bool Yokai::canPromote() const {
    int currentGrade = static_cast<int>(m_grade);
    if (currentGrade >= 5) return false;

    // Promotion level thresholds: Ⅰ->Ⅱ (Lv 10), Ⅱ->Ⅲ (Lv 20), Ⅲ->Ⅳ (Lv 35), Ⅳ->Ⅴ (Lv 50)
    if (currentGrade == 1 && m_level >= 10) return true;
    if (currentGrade == 2 && m_level >= 20) return true;
    if (currentGrade == 3 && m_level >= 35) return true;
    if (currentGrade == 4 && m_level >= 50) return true;

    return false;
}

bool Yokai::promoteGrade() {
    if (!canPromote()) return false;
    m_grade = static_cast<YokaiGrade>(static_cast<int>(m_grade) + 1);
    int prevMaxHp = m_stats.maxHp;
    int prevMaxQi = m_stats.maxQi;
    calculateStats();
    m_stats.hp += (m_stats.maxHp - prevMaxHp);
    m_stats.qi += (m_stats.maxQi - prevMaxQi);
    return true;
}

bool Yokai::takeDamage(int amount) {
    m_stats.hp = std::max(0, m_stats.hp - amount);
    return isFainted();
}

void Yokai::healHp(int amount) {
    m_stats.hp = std::min(m_stats.maxHp, m_stats.hp + amount);
}

void Yokai::setHp(int hp) {
    m_stats.hp = std::clamp(hp, 0, m_stats.maxHp);
}

bool Yokai::consumeQi(int amount) {
    if (m_stats.qi < amount) return false;
    m_stats.qi -= amount;
    return true;
}

void Yokai::restoreQi(int amount) {
    m_stats.qi = std::min(m_stats.maxQi, m_stats.qi + amount);
}

void Yokai::setQi(int qi) {
    m_stats.qi = std::clamp(qi, 0, m_stats.maxQi);
}

bool Yokai::addSkill(const Skill& skill) {
    if (m_skills.size() >= MAX_SKILLS) return false;
    m_skills.push_back(skill);
    return true;
}

} // namespace JoseonRPG
