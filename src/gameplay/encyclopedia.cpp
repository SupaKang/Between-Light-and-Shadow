#include "encyclopedia.hpp"
#include <algorithm>

namespace JoseonRPG {

Encyclopedia::Encyclopedia() {
    m_entries.reserve(108);
}

void Encyclopedia::registerTemplate(int number, std::string id, std::string nameKo, std::string nameEn,
                                    YokaiGrade baseGrade, Element element, std::string origin, std::string lore) {
    EncyclopediaEntry entry;
    entry.number = number;
    entry.id = std::move(id);
    entry.nameKo = std::move(nameKo);
    entry.nameEn = std::move(nameEn);
    entry.baseGrade = baseGrade;
    entry.element = element;
    entry.origin = std::move(origin);
    entry.lore = std::move(lore);
    entry.status = DiscoveryStatus::Unseen;

    m_entries.push_back(std::move(entry));
}

void Encyclopedia::markSeen(std::string_view id) {
    for (auto& entry : m_entries) {
        if (entry.id == id) {
            if (entry.status == DiscoveryStatus::Unseen) {
                entry.status = DiscoveryStatus::Seen;
            }
            return;
        }
    }
}

void Encyclopedia::markCaptured(std::string_view id) {
    for (auto& entry : m_entries) {
        if (entry.id == id) {
            entry.status = DiscoveryStatus::Captured;
            return;
        }
    }
}

void Encyclopedia::markSeen(int number) {
    for (auto& entry : m_entries) {
        if (entry.number == number) {
            if (entry.status == DiscoveryStatus::Unseen) {
                entry.status = DiscoveryStatus::Seen;
            }
            return;
        }
    }
}

void Encyclopedia::markCaptured(int number) {
    for (auto& entry : m_entries) {
        if (entry.number == number) {
            entry.status = DiscoveryStatus::Captured;
            return;
        }
    }
}

const EncyclopediaEntry* Encyclopedia::getEntry(int number) const {
    for (const auto& entry : m_entries) {
        if (entry.number == number) return &entry;
    }
    return nullptr;
}

const EncyclopediaEntry* Encyclopedia::getEntryById(std::string_view id) const {
    for (const auto& entry : m_entries) {
        if (entry.id == id) return &entry;
    }
    return nullptr;
}

int Encyclopedia::getSeenCount() const {
    int count = 0;
    for (const auto& entry : m_entries) {
        if (entry.status == DiscoveryStatus::Seen || entry.status == DiscoveryStatus::Captured) {
            count++;
        }
    }
    return count;
}

int Encyclopedia::getCapturedCount() const {
    int count = 0;
    for (const auto& entry : m_entries) {
        if (entry.status == DiscoveryStatus::Captured) {
            count++;
        }
    }
    return count;
}

float Encyclopedia::getCompletionRate() const {
    if (m_entries.empty()) return 0.0f;
    return static_cast<float>(getCapturedCount()) / static_cast<float>(m_entries.size());
}

} // namespace JoseonRPG
