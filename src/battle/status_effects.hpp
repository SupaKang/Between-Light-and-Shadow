#pragma once
#include "../core/types.hpp"
#include <string_view>

namespace JoseonRPG {

class Yokai;

struct StatusInstance {
    StatusEffect effect = StatusEffect::None;
    int durationTurns = 0;
    int sealedSkillIndex = -1; // For StatusEffect::Seal
};

class StatusEffectSystem {
public:
    static std::string_view getStatusName(StatusEffect effect);
    static Color getStatusColor(StatusEffect effect);

    // Apply status effect to target
    static bool applyStatus(Yokai& target, StatusEffect effect, int duration = 3, int sealedSkillIdx = -1);
    
    // Process start of turn (returns true if yokai CAN act, false if turn skipped/incapacitated)
    static bool onTurnStart(Yokai& target, std::string& outLog);

    // Process on-action attempt (e.g. Burn check when using attack skill)
    static void onActionUsed(Yokai& target, bool isAttackSkill, std::string& outLog);

    // Process on-damage received (e.g. Freeze shatter, Fear amplification)
    static int onDamageReceived(Yokai& target, int rawDamage, std::string& outLog);

    // Process end of turn duration decrement
    static void onTurnEnd(Yokai& target, std::string& outLog);
};

} // namespace JoseonRPG
