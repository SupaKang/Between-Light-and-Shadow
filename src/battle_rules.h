#pragma once

namespace battle {
struct Combatant { int attack, defense, hp, qi, speed; };

int damage(const Combatant& attacker, const Combatant& defender, int power, bool feared = false);
int damage(const Combatant& attacker, const Combatant& defender, int power, bool attacker_feared, bool defender_feared);

int capture_rate_percent(int enemy_hp, int max_hp, bool has_status = false, int charm_bonus = 0);
bool can_capture(int enemy_hp, int max_hp, int turn);
bool can_capture(int enemy_hp, int max_hp, int turn, bool has_status, int charm_bonus = 0);
}
