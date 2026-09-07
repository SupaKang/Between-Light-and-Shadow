#pragma once
namespace world {
struct State {
    int region_id = 1;
    int player_x = 15, player_y = 8;
    bool shrine_event_seen = false;
    bool can_interact(int x, int y) const;
    void mark_shrine_seen();
};
}
