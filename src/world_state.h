#pragma once

namespace world {

enum class MapId {
    Village = 1,
    MountainPass = 2,
    Daewoongjeon = 3
};

struct State {
    int region_id = 1; // 1 = Village, 2 = MountainPass, 3 = Daewoongjeon
    int player_x = 15, player_y = 8;
    bool shrine_event_seen = false;
    int main_quest_step = 0; // 0 = start, 1 = talk to keeper, 2 = boss defeated
    bool boss_defeated = false;

    bool can_interact(int x, int y) const;
    void mark_shrine_seen();

    bool is_blocked(int x, int y) const;
    bool check_portal(int x, int y, int& out_map, int& out_x, int& out_y) const;
    int check_npc_interaction(int x, int y) const;
    const char* current_map_name() const;
    int map_encounter_rate() const;
    void change_map(int map_id, int spawn_x, int spawn_y);
    void advance_quest();
};

} // namespace world
