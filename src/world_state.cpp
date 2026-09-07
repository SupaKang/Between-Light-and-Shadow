#include "world_state.h"
#include <cstdlib>

namespace world {

bool State::can_interact(int x, int y) const {
    if (region_id == 1) { // Village: Shrine is at (15, 7) or (20, 7)
        int dist1 = std::abs(x - 15) + std::abs(y - 7);
        int dist2 = std::abs(x - 20) + std::abs(y - 7);
        return (dist1 <= 1) || (dist2 <= 1);
    }
    return false;
}

void State::mark_shrine_seen() {
    shrine_event_seen = true;
}

bool State::is_blocked(int x, int y) const {
    if (x < 2 || x > 27 || y < 2 || y > 14) return true;

    if (region_id == 1) { // Village
        // Tavern house at (6..10, 4..6)
        if (x >= 6 && x <= 10 && y >= 4 && y <= 5) return true;
        // Shrine at (20, 7) or (15, 7)
        if (x == 20 && y == 7) return true;
        if (x == 15 && y == 7) return true;
    } else if (region_id == 2) { // Mountain Pass
        // Rocky cliffs and boulders
        if ((x < 10 || x > 19) && (y >= 6 && y <= 10)) return true;
        if (x == 14 && y == 8) return true; // Giant Boulder
    } else if (region_id == 3) { // Daewoongjeon Temple
        // Temple hall boundary
        if (x >= 8 && x <= 21 && y >= 3 && y <= 5) return true;
        // Boss Monk tile
        if (x == 15 && y == 6) return true;
    }
    return false;
}

bool State::check_portal(int x, int y, int& out_map, int& out_x, int& out_y) const {
    if (region_id == 1) { // Village
        // North Exit -> Mountain Pass
        if (x >= 13 && x <= 16 && y <= 2) {
            out_map = 2; out_x = 14; out_y = 13; return true;
        }
    } else if (region_id == 2) { // Mountain Pass
        // South Exit -> Village
        if (x >= 13 && x <= 16 && y >= 14) {
            out_map = 1; out_x = 14; out_y = 3; return true;
        }
        // North Exit -> Daewoongjeon
        if (x >= 13 && x <= 16 && y <= 2) {
            out_map = 3; out_x = 14; out_y = 13; return true;
        }
    } else if (region_id == 3) { // Daewoongjeon
        // South Exit -> Mountain Pass
        if (x >= 13 && x <= 16 && y >= 14) {
            out_map = 2; out_x = 14; out_y = 3; return true;
        }
    }
    return false;
}

int State::check_npc_interaction(int x, int y) const {
    // Returns NPC ID:
    // 1: Tavern Keeper (Village)
    // 2: Ancient Shrine (Village)
    // 3: Mountain Signpost (Mountain Pass)
    // 4: Corrupt Monk Myogak (Daewoongjeon Boss)
    if (region_id == 1) {
        if (std::abs(x - 8) + std::abs(y - 6) <= 1) return 1; // Tavern Keeper
        if (std::abs(x - 20) + std::abs(y - 7) <= 1 || std::abs(x - 15) + std::abs(y - 7) <= 1) return 2; // Shrine
    } else if (region_id == 2) {
        if (std::abs(x - 13) + std::abs(y - 8) <= 1) return 3; // Mountain Signpost
    } else if (region_id == 3) {
        if (std::abs(x - 15) + std::abs(y - 6) <= 1) return 4; // Boss Monk
    }
    return 0;
}

const char* State::current_map_name() const {
    switch (region_id) {
        case 1: return "DOSEONSA VILLAGE EDGE";
        case 2: return "BUKHANSAN MOUNTAIN PASS";
        case 3: return "DAEWOONGJEON TEMPLE GROUNDS";
        default: return "UNKNOWN REGION";
    }
}

int State::map_encounter_rate() const {
    switch (region_id) {
        case 1: return 0; // Peaceful Village
        case 2: return 8; // Wild Mountain Pass (every 8 steps)
        case 3: return 0; // Boss Courtyard
        default: return 12;
    }
}

void State::change_map(int map_id, int spawn_x, int spawn_y) {
    region_id = map_id;
    player_x = spawn_x;
    player_y = spawn_y;
}

void State::advance_quest() {
    if (main_quest_step < 3) ++main_quest_step;
}

} // namespace world
