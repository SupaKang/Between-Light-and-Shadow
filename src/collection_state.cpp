#include "collection_state.h"

namespace collection {

bool State::add_contract(int yokai_id) {
    if (yokai_id < 1 || yokai_id > TotalYokai) return false;
    discover(yokai_id);
    contracted[yokai_id - 1] = true;
    if (contains(yokai_id)) return false;
    if (party_count >= MaxParty) return false;
    party_ids[party_count++] = yokai_id;
    return true;
}

bool State::contains(int yokai_id) const {
    for (int i = 0; i < party_count; ++i) {
        if (party_ids[i] == yokai_id) return true;
    }
    return false;
}

bool State::discover(int yokai_id) {
    if (yokai_id < 1 || yokai_id > TotalYokai) return false;
    discovered[yokai_id - 1] = true;
    return true;
}

int State::discovered_count() const {
    int count = 0;
    for (bool b : discovered) if (b) ++count;
    return count;
}

int State::contracted_count() const {
    int count = 0;
    for (bool b : contracted) if (b) ++count;
    return count;
}

bool State::swap_party(int slot1, int slot2) {
    if (slot1 < 0 || slot1 >= party_count || slot2 < 0 || slot2 >= party_count) return false;
    int tmp = party_ids[slot1];
    party_ids[slot1] = party_ids[slot2];
    party_ids[slot2] = tmp;
    return true;
}

int State::active_yokai_id() const {
    if (active_party_index >= 0 && active_party_index < party_count) {
        return party_ids[active_party_index];
    }
    return party_count > 0 ? party_ids[0] : 0;
}

void State::reset() {
    for (int i = 0; i < MaxParty; ++i) party_ids[i] = 0;
    party_count = 0;
    active_party_index = 0;
    for (int i = 0; i < TotalYokai; ++i) {
        discovered[i] = false;
        contracted[i] = false;
    }
}

}
