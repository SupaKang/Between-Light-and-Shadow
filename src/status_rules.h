#pragma once
namespace status_rules {
enum class Kind { None, Burn, Freeze, Seal, Paralysis, Fear };
int tick_damage(Kind kind, int base_damage);
bool skips_action(Kind kind, int turn);
int adjusted_damage(Kind kind, int damage);
}
