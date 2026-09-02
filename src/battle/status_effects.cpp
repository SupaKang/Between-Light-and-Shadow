#include "status_effects.hpp"
#include "../gameplay/yokai.hpp"
#include <random>

namespace JoseonRPG {

static std::mt19937 s_rng(42);

std::string_view StatusEffectSystem::getStatusName(StatusEffect effect) {
    switch (effect) {
        case StatusEffect::Burn: return "화상 (Burn)";
        case StatusEffect::Freeze: return "빙결 (Freeze)";
        case StatusEffect::Seal: return "봉인 (Seal)";
        case StatusEffect::Paralysis: return "마비 (Paralysis)";
        case StatusEffect::Fear: return "공포 (Fear)";
        default: return "정상 (Normal)";
    }
}

Color StatusEffectSystem::getStatusColor(StatusEffect effect) {
    switch (effect) {
        case StatusEffect::Burn: return Palette::Red;
        case StatusEffect::Freeze: return Palette::Blue;
        case StatusEffect::Seal: return Palette::Purple;
        case StatusEffect::Paralysis: return Palette::Yellow;
        case StatusEffect::Fear: return Palette::MidGray;
        default: return Palette::White;
    }
}

bool StatusEffectSystem::applyStatus(Yokai& target, StatusEffect effect, int duration, int sealedSkillIdx) {
    if (effect == StatusEffect::None) return false;
    if (target.getStatus().effect != StatusEffect::None) {
        // Only one active status effect at a time
        return false;
    }
    StatusInstance inst;
    inst.effect = effect;
    inst.durationTurns = duration;
    inst.sealedSkillIndex = sealedSkillIdx;
    target.getStatus() = inst;
    return true;
}

bool StatusEffectSystem::onTurnStart(Yokai& target, std::string& outLog) {
    StatusInstance& status = target.getStatus();
    if (status.effect == StatusEffect::None) return true;

    if (status.effect == StatusEffect::Freeze) {
        outLog = target.getName() + " is frozen solid and cannot act!";
        return false; // Cannot act
    }

    if (status.effect == StatusEffect::Paralysis) {
        std::uniform_int_distribution<int> dist(1, 100);
        if (dist(s_rng) <= 33) {
            outLog = target.getName() + " is paralyzed and unable to move!";
            return false; // 33% action failure
        }
    }

    return true;
}

void StatusEffectSystem::onActionUsed(Yokai& target, bool isAttackSkill, std::string& outLog) {
    StatusInstance& status = target.getStatus();
    if (status.effect == StatusEffect::Burn && isAttackSkill) {
        int burnDmg = std::max(1, target.getStats().maxHp / 8); // 12.5% max HP
        target.takeDamage(burnDmg);
        outLog = target.getName() + " takes " + std::to_string(burnDmg) + " burn backlash damage!";
    }
}

int StatusEffectSystem::onDamageReceived(Yokai& target, int rawDamage, std::string& outLog) {
    StatusInstance& status = target.getStatus();
    int finalDmg = rawDamage;

    // Fear: Takes 25% increased damage
    if (status.effect == StatusEffect::Fear) {
        finalDmg = static_cast<int>(finalDmg * 1.25f);
        outLog += " [Fear Amplified]";
    }

    // Freeze: Shatters upon taking damage
    if (status.effect == StatusEffect::Freeze) {
        status.effect = StatusEffect::None;
        status.durationTurns = 0;
        outLog += " [Ice Shattered!]";
    }

    return finalDmg;
}

void StatusEffectSystem::onTurnEnd(Yokai& target, std::string& outLog) {
    (void)outLog;
    StatusInstance& status = target.getStatus();
    if (status.effect == StatusEffect::None) return;

    if (status.effect != StatusEffect::Seal) { // Seal lasts until battle ends
        status.durationTurns--;
        if (status.durationTurns <= 0) {
            status.effect = StatusEffect::None;
        }
    }
}

} // namespace JoseonRPG
