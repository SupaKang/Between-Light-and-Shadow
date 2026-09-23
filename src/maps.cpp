#include "maps_api.h"
#include "maps_gen.h"

const MapDef* find_map(const std::string& id) {
    for (const MapDef& m : kMaps)
        if (id == m.id) return &m;
    return nullptr;
}

bool warp_valid(const std::string& map, int x, int y) {
    const MapDef* m = find_map(map);
    if (!m || x < 0 || y < 0 || x >= m->w || y >= m->h) return false;
    return std::string("#BRJSThHWGe~").find(m->tiles[y * m->w + x]) == std::string::npos;
}
