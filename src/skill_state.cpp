#include "skill_state.h"

namespace skills {

bool State::usable(int slot, int qi) const {
    if (slot < 0 || slot >= MaxSlots) return false;
    if (ids[slot] <= 0) return false;
    if (sealed_turns[slot] > 0) return false;
    return qi >= qi_costs[slot];
}

void State::tick() {
    for (int& value : sealed_turns) {
        if (value > 0) --value;
    }
}

void State::seal(int slot, int turns) {
    if (slot >= 0 && slot < MaxSlots) {
        sealed_turns[slot] = turns;
    }
}

void State::configure_slot(int slot, int id, const char* name, int power, int qi_cost, int accuracy, status_rules::Kind status, int seal_dur) {
    if (slot < 0 || slot >= MaxSlots) return;
    ids[slot] = id;
    std::strncpy(names[slot], name, sizeof(names[slot]) - 1);
    names[slot][sizeof(names[slot]) - 1] = '\0';
    powers[slot] = power;
    qi_costs[slot] = qi_cost;
    accuracies[slot] = accuracy;
    statuses[slot] = status;
    seal_duration[slot] = seal_dur;
}

}
