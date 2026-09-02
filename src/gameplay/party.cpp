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

} // namespace JoseonRPG
