#pragma once
#include "../core/types.hpp"
#include <string>
#include <vector>

namespace JoseonRPG {

enum class ArtifactBuffType {
    CritRateBoost,
    ImmunityBurn,
    MagicAtkBoost,
    DamageReduction
};

enum class ArtifactDebuffType {
    QiDrainPerTurn,
    MaxHpReduction,
    DefReduction,
    SpdReduction
};

struct Artifact {
    std::string id;
    std::string name;
    ArtifactBuffType buffType;
    int buffValue = 0;
    ArtifactDebuffType debuffType;
    int debuffValue = 0;
    std::string lore;
};

class ArtifactInventory {
public:
    ArtifactInventory() = default;

    bool addArtifact(const Artifact& artifact);
    bool destroyArtifact(size_t index); // Instantly destroy artifact

    const std::vector<Artifact>& getArtifacts() const { return m_artifacts; }
    size_t getCount() const { return m_artifacts.size(); }

    // Calculated aggregated modifiers
    int getCritRateBonus() const;
    bool hasBurnImmunity() const;
    float getMagicAtkMultiplier() const;
    float getDamageReductionMultiplier() const;
    int getQiDrainPerTurn() const;
    float getMaxHpMultiplier() const;
    float getDefMultiplier() const;
    float getSpdMultiplier() const;

private:
    std::vector<Artifact> m_artifacts;
};

} // namespace JoseonRPG
