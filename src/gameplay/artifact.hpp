#pragma once
#include "../core/types.hpp"
#include <string>
#include <vector>

namespace JoseonRPG {

class Yokai;

enum class ArtifactBuffType {
    CritRateBoost,      // 치명타 확률 증가
    ImmunityBurn,       // 화상 면역
    ImmunityFreezeFear, // 빙결 및 공포 면역
    MagicAtkBoost,      // 영술 위력 증폭 (%)
    DamageReduction,    // 받는 물리 피해 경감 (%)
    CaptureRateBoost,   // 계약 부적 성공률 증가 (%)
    StatusChanceBoost,  // 상태이상 부여 확률 증폭 (%)
    PhysAtkBoost,       // 물리 공격력 증폭 (%)
    ImmunitySeal,       // 봉인 및 마비 면역
    AllStatsBoost       // 전능(모든 스탯) 증폭 (%)
};

enum class ArtifactDebuffType {
    QiDrainPerTurn,     // 매 턴 영력(Qi) 강제 소모
    MaxHpReduction,     // 최대 체력 감소 (%)
    DefReduction,       // 방어력 감소 (%)
    SpdReduction,       // 신법(SPD) 감소 (%)
    ExpPenalty,         // 획득 경험치 감소 (%)
    HpDrainPerTurn,     // 매 턴 체력(HP) 소모 (%)
    MaxQiReduction,     // 최대 영력 감소 (%)
    DamageTakenIncrease // 받는 피해 증가 (%)
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
    bool destroyArtifact(size_t index, Yokai* activeYokai = nullptr, std::string* outMsg = nullptr);

    const std::vector<Artifact>& getArtifacts() const { return m_artifacts; }
    const Artifact* getArtifact(size_t index) const;
    size_t getCount() const { return m_artifacts.size(); }
    bool isFull() const { return m_artifacts.size() >= MAX_ARTIFACTS; }
    void clear() { m_artifacts.clear(); }

    // Calculated aggregated modifiers
    int getCritRateBonus() const;
    bool hasBurnImmunity() const;
    bool hasFreezeFearImmunity() const;
    float getMagicAtkMultiplier() const;
    float getDamageReductionMultiplier() const;
    float getCaptureRateBonus() const;
    float getStatusChanceMultiplier() const;

    int getQiDrainPerTurn() const;
    float getMaxHpMultiplier() const;
    float getDefMultiplier() const;
    float getSpdMultiplier() const;
    float getExpMultiplier() const;
    int getHpDrainPerTurn(int maxHp) const;

    // String helpers
    static std::string getBuffDescription(const Artifact& a);
    static std::string getDebuffDescription(const Artifact& a);

private:
    std::vector<Artifact> m_artifacts;
};

} // namespace JoseonRPG
