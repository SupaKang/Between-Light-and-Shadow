#pragma once
#include "../core/types.hpp"
#include "../battle/status_effects.hpp"
#include <string>
#include <vector>
#include <array>

namespace JoseonRPG {

enum class YokaiTrait {
    None,
    DokkaebiPower,    // 치명타율 +15%
    FoxCharm,         // 전투 진입 시 적에게 공포/혼란 확률 부여
    IronDiet,         // 물리 피격 시 방어력 15% 상승
    GrimGaze,         // 상대 HP 30% 이하 시 공격력 +40%
    AquaSurge,        // 수속성 기술 사용 시 영력 소모 25% 감소
    DuduriProtection, // 받는 피해 10% 감소
    FlameBody,        // 접촉 피격 시 30% 확률로 공격자에게 화상 부여
    HolyAura,         // 매 턴 체력 5% 자연 치유
    WindGrace,        // 신법(SPD) +20% & 회피율 +15%
    TaijiHarmony      // 음양 조화: 상태이상 지속시간 1턴 감소 & 상성 유리 시 피해 +25%
};

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
          Stats baseStats, std::string origin = "KOREAN_FOLKLORE", std::string lore = "",
          YokaiTrait trait = YokaiTrait::None);

    void calculateStats();
    void gainExp(int amount);
    void levelUp();
    void setLevel(int level);

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

    // Trait (고유 특성)
    YokaiTrait getTrait() const { return m_trait; }
    void setTrait(YokaiTrait trait) { m_trait = trait; }
    std::string getTraitName() const;
    std::string getTraitDescription() const;

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
    YokaiTrait m_trait = YokaiTrait::None;

    Stats m_baseStats;
    Stats m_stats;

    std::vector<Skill> m_skills;
    StatusInstance m_status;
};

} // namespace JoseonRPG
