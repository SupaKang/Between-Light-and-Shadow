#include "status_rules.h"
namespace status_rules {
int tick_damage(Kind kind, int base_damage) { return kind == Kind::Burn ? base_damage : 0; }
bool skips_action(Kind kind, int turn) { return (kind == Kind::Freeze && turn % 2 == 0) || (kind == Kind::Paralysis && turn % 3 == 0); }
int adjusted_damage(Kind kind, int damage, double fear_multiplier) { return kind == Kind::Fear ? static_cast<int>(damage * fear_multiplier) : damage; }
}
