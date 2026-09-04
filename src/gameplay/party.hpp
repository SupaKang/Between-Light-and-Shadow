#pragma once
#include "yokai.hpp"
#include <vector>
#include <memory>

namespace JoseonRPG {

enum class ResonanceType {
    None,
    YinYangHarmony,     // Light + Dark (음양상화): 공방 +15%
    FlameEarthSurge,    // Fire + Earth (화생토): 화/토 위력 +25%
    VitalityFlow,       // Water + Physical (수생목): 매 턴 5% 체력 재생
    TriadSynergy,       // 3 of same element (삼원합일): 신법 +10, 기력소모 감소
    FiveElementsHarmony // 3 distinct elements (오행조화): 치명타율 +20%
};

struct ResonanceInfo {
    ResonanceType type = ResonanceType::None;
    std::string nameKo;
    std::string descKo;
    float atkMod = 1.0f;
    float defMod = 1.0f;
    float elemDmgMod = 1.0f;
    float hpRegenRatio = 0.0f;
    int speedBonus = 0;
    int critBonus = 0;
};

class Party {
public:
    Party() = default;

    bool addYokai(const Yokai& yokai);
    bool removeYokai(size_t index);
    bool swapYokai(size_t indexA, size_t indexB);

    Yokai* getActiveYokai();
    const Yokai* getActiveYokai() const;

    Yokai* getYokai(size_t index);
    const Yokai* getYokai(size_t index) const;

    size_t getSize() const { return m_members.size(); }
    bool isAllFainted() const;
    void healAll();
    void clear() { m_members.clear(); }

    // Party Elemental Resonance System
    ResonanceInfo getActiveResonance() const;

private:
    std::vector<Yokai> m_members;
};

} // namespace JoseonRPG
