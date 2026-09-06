#pragma once

namespace battle {
struct Combatant { int attack, defense, hp, qi, speed; };
int damage(const Combatant& attacker, const Combatant& defender, int power, bool feared);
bool can_capture(int enemy_hp, int max_hp, int turn);
}
