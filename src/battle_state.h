#pragma once
#include "status_rules.h"
namespace battle {
struct State {
    int player_hp = 100, player_qi = 30, enemy_hp = 80, enemy_max_hp = 80, enemy_level = 1;
    int turn = 0;
    status_rules::Kind enemy_status = status_rules::Kind::None;
    bool active = false, contractable = false;
    void reset();
};
}
