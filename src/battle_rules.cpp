#include "battle_rules.h"

namespace battle {

int damage(const Combatant& attacker, const Combatant& defender, int power, bool feared) {
    return damage(attacker, defender, power, false, feared);
}

int damage(const Combatant& attacker, const Combatant& defender, int power, bool attacker_feared, bool defender_feared) {
    int eff_attack = attacker.attack;
    if (attacker_feared) eff_attack = eff_attack * 3 / 4;
    int value = eff_attack + power - defender.defense;
    if (defender_feared) value = value * 3 / 2;
    return value < 1 ? 1 : value;
}

int capture_rate_percent(int enemy_hp, int max_hp, bool has_status, int charm_bonus) {
    if (max_hp <= 0) return 0;
    if (enemy_hp > max_hp / 2) return 0;
    int weakness = (max_hp - enemy_hp) * 60 / max_hp;
    int status_bonus = has_status ? 25 : 0;
    int total = weakness + status_bonus + charm_bonus;
    if (total > 100) total = 100;
    if (total < 0) total = 0;
    return total;
}

bool can_capture(int enemy_hp, int max_hp, int turn) {
    if (max_hp <= 0 || enemy_hp > max_hp / 2) return false;
    const int weakness = (max_hp - enemy_hp) * 100 / max_hp;
    return weakness >= 50 && (turn % 2 == 0);
}

bool can_capture(int enemy_hp, int max_hp, int turn, bool has_status, int charm_bonus) {
    if (max_hp <= 0 || enemy_hp > max_hp / 2) return false;
    int rate = capture_rate_percent(enemy_hp, max_hp, has_status, charm_bonus);
    if (rate <= 0) return false;
    return (turn % 2 == 0) || (has_status && (turn % 3 != 0));
}

}
