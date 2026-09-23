// Baked map data (tools/bake_maps.py -> maps_gen.h) and lookups.
#pragma once
#include <string>

struct NpcDef { const char* name; int x, y; char face; const char* sprite; const char* talk_event; };  // face: D U L R
struct WarpDef { int x, y; const char* to_map; int tx, ty; char face; };
struct TriggerDef { int x, y; const char* event; bool once; };
struct MapDef {
    const char* id; const char* name_ko; int w, h; bool indoor; const char* tiles;  // w*h tile-kind chars
    const NpcDef* npcs; int npc_n; const WarpDef* warps; int warp_n; const TriggerDef* triggers; int trig_n;
};

const MapDef* find_map(const std::string& id);
bool warp_valid(const std::string& map, int x, int y);  // map exists, inside bounds, tile walkable
