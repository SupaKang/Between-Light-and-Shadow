#pragma once

namespace status_rules {
enum class Kind { None, Burn, Freeze, Seal, Paralysis, Fear };

int tick_damage(Kind kind, int base_damage);
bool skips_action(Kind kind, int turn);
int adjusted_damage(Kind kind, int damage, double fear_multiplier = 1.5);
int attack_modifier(Kind kind, int damage);
Kind on_hit_effect(Kind kind);
int default_duration(Kind kind);
const char* name(Kind kind);
const char* short_tag(Kind kind);
const char* name_ko(Kind kind);
const char* short_tag_ko(Kind kind);
}
