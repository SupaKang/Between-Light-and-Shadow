#include "status_rules.h"

namespace status_rules {
int tick_damage(Kind kind, int base_damage) {
    return kind == Kind::Burn ? base_damage : 0;
}
bool skips_action(Kind kind, int turn) {
    return (kind == Kind::Freeze) || (kind == Kind::Paralysis && turn % 3 == 0);
}
int adjusted_damage(Kind kind, int damage, double fear_multiplier) {
    return kind == Kind::Fear ? static_cast<int>(damage * fear_multiplier) : damage;
}
int attack_modifier(Kind kind, int damage) {
    return kind == Kind::Fear ? (damage * 3 / 4) : damage;
}
Kind on_hit_effect(Kind kind) {
    return kind == Kind::Freeze ? Kind::None : kind;
}
int default_duration(Kind kind) {
    switch (kind) {
        case Kind::Burn: return 3;
        case Kind::Freeze: return 2;
        case Kind::Seal: return 1;
        case Kind::Paralysis: return 4;
        case Kind::Fear: return 3;
        default: return 0;
    }
}
const char* name(Kind kind) {
    switch (kind) {
        case Kind::Burn: return "BURN";
        case Kind::Freeze: return "FREEZE";
        case Kind::Seal: return "SEAL";
        case Kind::Paralysis: return "PARALYSIS";
        case Kind::Fear: return "FEAR";
        default: return "NONE";
    }
}
const char* short_tag(Kind kind) {
    switch (kind) {
        case Kind::Burn: return "[BRN]";
        case Kind::Freeze: return "[FRZ]";
        case Kind::Seal: return "[SEL]";
        case Kind::Paralysis: return "[PAR]";
        case Kind::Fear: return "[FEAR]";
        default: return "";
    }
}
const char* name_ko(Kind kind) {
    switch (kind) {
        case Kind::Burn: return "화상";
        case Kind::Freeze: return "빙결";
        case Kind::Seal: return "봉인";
        case Kind::Paralysis: return "마비";
        case Kind::Fear: return "공포";
        default: return "정상";
    }
}
const char* short_tag_ko(Kind kind) {
    switch (kind) {
        case Kind::Burn: return "[화상]";
        case Kind::Freeze: return "[빙결]";
        case Kind::Seal: return "[봉인]";
        case Kind::Paralysis: return "[마비]";
        case Kind::Fear: return "[공포]";
        default: return "";
    }
}
}
