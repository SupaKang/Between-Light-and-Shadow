#include "status_rules.h"
namespace status_rules {
int tick_damage(Kind kind, int base_damage) { return kind == Kind::Burn ? 3 : base_damage; }
bool skips_action(Kind kind, int turn) { return (kind == Kind::Freeze && turn % 2 == 0) || (kind == Kind::Paralysis && turn % 3 == 0); }
int adjusted_damage(Kind kind, int damage) { return kind == Kind::Fear ? damage * 3 / 2 : damage; }
}
