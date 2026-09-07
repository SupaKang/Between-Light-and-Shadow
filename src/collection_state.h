#pragma once
namespace collection {
struct State {
    static constexpr int MaxParty = 3;
    int party_ids[MaxParty] = {0, 0, 0};
    int party_count = 0;
    bool discovered[108] = {};
    bool add_contract(int yokai_id);
    bool contains(int yokai_id) const;
    bool discover(int yokai_id);
    int discovered_count() const;
};
}
