#include "artifact.hpp"

namespace JoseonRPG {

bool ArtifactInventory::addArtifact(const Artifact& artifact) {
    if (m_artifacts.size() >= MAX_ARTIFACTS) return false;
    m_artifacts.push_back(artifact);
    return true;
}

bool ArtifactInventory::destroyArtifact(size_t index) {
    if (index >= m_artifacts.size()) return false;
    m_artifacts.erase(m_artifacts.begin() + index);
    return true;
}

int ArtifactInventory::getCritRateBonus() const {
    int total = 0;
    for (const auto& a : m_artifacts) {
        if (a.buffType == ArtifactBuffType::CritRateBoost) {
            total += a.buffValue;
        }
    }
    return total;
}

bool ArtifactInventory::hasBurnImmunity() const {
    for (const auto& a : m_artifacts) {
        if (a.buffType == ArtifactBuffType::ImmunityBurn) {
            return true;
        }
    }
    return false;
}

float ArtifactInventory::getMagicAtkMultiplier() const {
    float mult = 1.0f;
    for (const auto& a : m_artifacts) {
        if (a.buffType == ArtifactBuffType::MagicAtkBoost) {
            mult += (a.buffValue / 100.0f);
        }
    }
    return mult;
}

float ArtifactInventory::getDamageReductionMultiplier() const {
    float reduction = 0.0f;
    for (const auto& a : m_artifacts) {
        if (a.buffType == ArtifactBuffType::DamageReduction) {
            reduction += (a.buffValue / 100.0f);
        }
    }
    return 1.0f - std::min(0.75f, reduction);
}

int ArtifactInventory::getQiDrainPerTurn() const {
    int drain = 0;
    for (const auto& a : m_artifacts) {
        if (a.debuffType == ArtifactDebuffType::QiDrainPerTurn) {
            drain += a.debuffValue;
        }
    }
    return drain;
}

float ArtifactInventory::getMaxHpMultiplier() const {
    float red = 0.0f;
    for (const auto& a : m_artifacts) {
        if (a.debuffType == ArtifactDebuffType::MaxHpReduction) {
            red += (a.debuffValue / 100.0f);
        }
    }
    return std::max(0.2f, 1.0f - red);
}

float ArtifactInventory::getDefMultiplier() const {
    float red = 0.0f;
    for (const auto& a : m_artifacts) {
        if (a.debuffType == ArtifactDebuffType::DefReduction) {
            red += (a.debuffValue / 100.0f);
        }
    }
    return std::max(0.2f, 1.0f - red);
}

float ArtifactInventory::getSpdMultiplier() const {
    float red = 0.0f;
    for (const auto& a : m_artifacts) {
        if (a.debuffType == ArtifactDebuffType::SpdReduction) {
            red += (a.debuffValue / 100.0f);
        }
    }
    return std::max(0.2f, 1.0f - red);
}

} // namespace JoseonRPG
