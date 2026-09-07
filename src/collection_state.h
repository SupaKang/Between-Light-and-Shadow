#pragma once

namespace collection {

struct State {
    static constexpr int MaxParty = 3;
    static constexpr int TotalYokai = 108;

    int party_ids[MaxParty] = {0, 0, 0};
    int party_count = 0;
    int active_party_index = 0;
    bool discovered[TotalYokai] = {};
    bool contracted[TotalYokai] = {};

    bool add_contract(int yokai_id);
    bool contains(int yokai_id) const;
    bool discover(int yokai_id);
    int discovered_count() const;
    int contracted_count() const;
    bool swap_party(int slot1, int slot2);
    int active_yokai_id() const;
    void reset();
};

} // namespace collection
