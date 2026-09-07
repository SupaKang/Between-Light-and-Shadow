#pragma once
#include "status_rules.h"
#include <cstring>

namespace skills {

struct State {
    static constexpr int MaxSlots = 4;
    int ids[MaxSlots] = {0, 0, 0, 0};
    char names[MaxSlots][16] = {"STRIKE", "EMBER", "FROST", "SEAL"};
    int powers[MaxSlots] = {20, 8, 6, 4};
    int qi_costs[MaxSlots] = {0, 0, 0, 0};
    int accuracies[MaxSlots] = {100, 95, 90, 85};
    status_rules::Kind statuses[MaxSlots] = {
        status_rules::Kind::None,
        status_rules::Kind::Burn,
        status_rules::Kind::Freeze,
        status_rules::Kind::Seal
    };
    int seal_duration[MaxSlots] = {0, 0, 0, 1};
    int sealed_turns[MaxSlots] = {0, 0, 0, 0};
    int selected = 0;

    bool usable(int slot, int qi) const;
    void tick();
    void seal(int slot, int turns);
    void configure_slot(int slot, int id, const char* name, int power, int qi_cost, int accuracy, status_rules::Kind status, int seal_dur);
};

} // namespace skills
