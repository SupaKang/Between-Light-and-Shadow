#include "battle_rules.h"

namespace battle {
int damage(const Combatant& attacker, const Combatant& defender, int power, bool feared) {
    int value = attacker.attack + power - defender.defense;
    if (feared) value = value * 3 / 2;
    return value < 1 ? 1 : value;
}
bool can_capture(int enemy_hp, int max_hp, int turn) {
    if (max_hp <= 0 || enemy_hp > max_hp / 2) return false;
    const int weakness = (max_hp - enemy_hp) * 100 / max_hp;
    return weakness >= 50 && (turn % 2 == 0);
}
}
