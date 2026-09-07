#pragma once
namespace skills {
struct State {
    static constexpr int MaxSlots = 4;
    int ids[MaxSlots] = {0,0,0,0};
    int selected = 0;
    int sealed_turns[MaxSlots] = {0,0,0,0};
    bool usable(int slot, int qi) const;
    void tick();
};
}
