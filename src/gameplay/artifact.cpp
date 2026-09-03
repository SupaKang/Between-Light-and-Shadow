#include "artifact.hpp"
#include "yokai.hpp"
#include <algorithm>

namespace JoseonRPG {

bool ArtifactInventory::addArtifact(const Artifact& artifact) {
    if (m_artifacts.size() >= MAX_ARTIFACTS) return false;
    m_artifacts.push_back(artifact);
    return true;
}

const Artifact* ArtifactInventory::getArtifact(size_t index) const {
    if (index >= m_artifacts.size()) return nullptr;
    return &m_artifacts[index];
}

bool ArtifactInventory::destroyArtifact(size_t index, Yokai* activeYokai, std::string* outMsg) {
    if (index >= m_artifacts.size()) return false;
    
    std::string artName = m_artifacts[index].name;
    m_artifacts.erase(m_artifacts.begin() + index);

    if (activeYokai) {
        // Sacrifice Surge: recover 25 HP & 25 Qi and +50 EXP
        activeYokai->healHp(25);
        activeYokai->restoreQi(25);
        activeYokai->gainExp(50);
        if (outMsg) {
            *outMsg = "[" + artName + "] 파괴 완료! 영기 환원으로 HP/Qi +25 회복 & EXP +50 획득!";
        }
    } else {
        if (outMsg) {
            *outMsg = "[" + artName + "] 유물이 파괴되어 효과가 소멸했습니다.";
        }
    }

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

bool ArtifactInventory::hasFreezeFearImmunity() const {
    for (const auto& a : m_artifacts) {
        if (a.buffType == ArtifactBuffType::ImmunityFreezeFear) {
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

float ArtifactInventory::getCaptureRateBonus() const {
    float bonus = 0.0f;
    for (const auto& a : m_artifacts) {
        if (a.buffType == ArtifactBuffType::CaptureRateBoost) {
            bonus += (a.buffValue / 100.0f);
        }
    }
    return bonus;
}

float ArtifactInventory::getStatusChanceMultiplier() const {
    float mult = 1.0f;
    for (const auto& a : m_artifacts) {
        if (a.buffType == ArtifactBuffType::StatusChanceBoost) {
            mult += (a.buffValue / 100.0f);
        }
    }
    return mult;
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

float ArtifactInventory::getExpMultiplier() const {
    float penalty = 0.0f;
    for (const auto& a : m_artifacts) {
        if (a.debuffType == ArtifactDebuffType::ExpPenalty) {
            penalty += (a.debuffValue / 100.0f);
        }
    }
    return std::max(0.1f, 1.0f - penalty);
}

int ArtifactInventory::getHpDrainPerTurn(int maxHp) const {
    int totalDrain = 0;
    for (const auto& a : m_artifacts) {
        if (a.debuffType == ArtifactDebuffType::HpDrainPerTurn) {
            totalDrain += std::max(1, static_cast<int>(maxHp * (a.debuffValue / 100.0f)));
        }
    }
    return totalDrain;
}

std::string ArtifactInventory::getBuffDescription(const Artifact& a) {
    switch (a.buffType) {
        case ArtifactBuffType::CritRateBoost:
            return "치명타 확률 +" + std::to_string(a.buffValue) + "%";
        case ArtifactBuffType::ImmunityBurn:
            return "화상(Burn) 상태이상 완전 면역";
        case ArtifactBuffType::ImmunityFreezeFear:
            return "빙결 및 공포 상태이상 완전 면역";
        case ArtifactBuffType::MagicAtkBoost:
            return "영술 공격력 +" + std::to_string(a.buffValue) + "%";
        case ArtifactBuffType::DamageReduction:
            return "받는 모든 피해 " + std::to_string(a.buffValue) + "% 경감";
        case ArtifactBuffType::CaptureRateBoost:
            return "부적 계약(포획) 성공률 +" + std::to_string(a.buffValue) + "%p";
        case ArtifactBuffType::StatusChanceBoost:
            return "상태이상 부여 확률 +" + std::to_string(a.buffValue) + "%";
        case ArtifactBuffType::PhysAtkBoost:
            return "물리 공격력 +" + std::to_string(a.buffValue) + "%";
        case ArtifactBuffType::ImmunitySeal:
            return "봉인 및 마비 상태이상 완전 면역";
        case ArtifactBuffType::AllStatsBoost:
            return "전능(모든 기본 능력치) +" + std::to_string(a.buffValue) + "%";
        default:
            return "알 수 없는 이점";
    }
}

std::string ArtifactInventory::getDebuffDescription(const Artifact& a) {
    switch (a.debuffType) {
        case ArtifactDebuffType::QiDrainPerTurn:
            return "매 턴 시작 시 영력 " + std::to_string(a.debuffValue) + " 강제 소모";
        case ArtifactDebuffType::MaxHpReduction:
            return "파티 최대 체력 " + std::to_string(a.debuffValue) + "% 감소";
        case ArtifactDebuffType::DefReduction:
            return "호신(방어력) " + std::to_string(a.debuffValue) + "% 감소";
        case ArtifactDebuffType::SpdReduction:
            return "신법(민첩) " + std::to_string(a.debuffValue) + "% 감소 & 도망 불가";
        case ArtifactDebuffType::ExpPenalty:
            return "전투 후 획득 경험치 " + std::to_string(a.debuffValue) + "% 감소";
        case ArtifactDebuffType::HpDrainPerTurn:
            return "매 턴 시작 시 최대 HP의 " + std::to_string(a.debuffValue) + "% 자가 피해";
        case ArtifactDebuffType::MaxQiReduction:
            return "최대 영력(Qi) " + std::to_string(a.debuffValue) + "% 감소";
        case ArtifactDebuffType::DamageTakenIncrease:
            return "받는 모든 피해 " + std::to_string(a.debuffValue) + "% 증가";
        default:
            return "알 수 없는 대가";
    }
}

} // namespace JoseonRPG
