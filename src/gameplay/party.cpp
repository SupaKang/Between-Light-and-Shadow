#include "party.hpp"

namespace JoseonRPG {

bool Party::addYokai(const Yokai& yokai) {
    if (m_members.size() >= MAX_PARTY_SIZE) return false;
    m_members.push_back(yokai);
    return true;
}

bool Party::removeYokai(size_t index) {
    if (index >= m_members.size()) return false;
    m_members.erase(m_members.begin() + index);
    return true;
}

bool Party::swapYokai(size_t indexA, size_t indexB) {
    if (indexA >= m_members.size() || indexB >= m_members.size()) return false;
    std::swap(m_members[indexA], m_members[indexB]);
    return true;
}

Yokai* Party::getActiveYokai() {
    if (m_members.empty()) return nullptr;
    return &m_members[0];
}

const Yokai* Party::getActiveYokai() const {
    if (m_members.empty()) return nullptr;
    return &m_members[0];
}

Yokai* Party::getYokai(size_t index) {
    if (index >= m_members.size()) return nullptr;
    return &m_members[index];
}

const Yokai* Party::getYokai(size_t index) const {
    if (index >= m_members.size()) return nullptr;
    return &m_members[index];
}

bool Party::isAllFainted() const {
    for (const auto& m : m_members) {
        if (!m.isFainted()) return false;
    }
    return true;
}

void Party::healAll() {
    for (auto& m : m_members) {
        m.getStats().hp = m.getStats().maxHp;
        m.getStats().qi = m.getStats().maxQi;
        m.clearStatus();
    }
}

ResonanceInfo Party::getActiveResonance() const {
    ResonanceInfo info;
    if (m_members.size() < 2) return info;

    bool hasLight = false;
    bool hasDark = false;
    bool hasFire = false;
    bool hasEarth = false;
    bool hasWater = false;
    bool hasPhys = false;

    std::vector<Element> elements;
    for (const auto& m : m_members) {
        Element e = m.getElement();
        elements.push_back(e);
        if (e == Element::Light) hasLight = true;
        if (e == Element::Dark) hasDark = true;
        if (e == Element::Fire) hasFire = true;
        if (e == Element::Earth) hasEarth = true;
        if (e == Element::Water) hasWater = true;
        if (e == Element::Physical) hasPhys = true;
    }

    // 1. Check Triad Synergy (3 of same element)
    if (elements.size() >= 3 && elements[0] == elements[1] && elements[1] == elements[2]) {
        info.type = ResonanceType::TriadSynergy;
        info.nameKo = "삼원합일 (三元合一)";
        info.descKo = "동일 원소 3체의 결속: 신법 +10 & 기력 소모 감소";
        info.speedBonus = 10;
        info.elemDmgMod = 1.15f;
        return info;
    }

    // 2. Check Yin-Yang Harmony (Light + Dark)
    if (hasLight && hasDark) {
        info.type = ResonanceType::YinYangHarmony;
        info.nameKo = "음양상화 (陰陽相和)";
        info.descKo = "빛과 어둠의 조화: 완력 +15% & 호신 +15%";
        info.atkMod = 1.15f;
        info.defMod = 1.15f;
        return info;
    }

    // 3. Check Flame Earth Surge (Fire + Earth)
    if (hasFire && hasEarth) {
        info.type = ResonanceType::FlameEarthSurge;
        info.nameKo = "화생토 (火生土)";
        info.descKo = "불과 대지의 분출: 화/토 도술 피해 +25%";
        info.elemDmgMod = 1.25f;
        return info;
    }

    // 4. Check Vitality Flow (Water + Physical)
    if (hasWater && hasPhys) {
        info.type = ResonanceType::VitalityFlow;
        info.nameKo = "수생목 생기 (水生木)";
        info.descKo = "물과 생명의 흐름: 매 턴 최대 체력 5% 재생";
        info.hpRegenRatio = 0.05f;
        return info;
    }

    // 5. Check Five Elements Harmony (3 distinct elements)
    if (elements.size() >= 3 && elements[0] != elements[1] && elements[1] != elements[2] && elements[0] != elements[2]) {
        info.type = ResonanceType::FiveElementsHarmony;
        info.nameKo = "오행조화 (五行調和)";
        info.descKo = "상호 보완의 극치: 치명타 확률 +20%";
        info.critBonus = 20;
        return info;
    }

    return info;
}

} // namespace JoseonRPG
