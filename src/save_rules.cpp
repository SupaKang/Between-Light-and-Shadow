#include "save_rules.h"

namespace save_rules {
std::uint32_t checksum(
    std::uint32_t version, int x, int y, int hp, int qi, int level, int xp, int party,
    const int ids[3], bool contracted, bool artifact, bool shrine_seen,
    int region_id, int quest_step
) {
    std::uint32_t h = version ^ 0x9E3779B9u;
    h = (h << 5) + h + static_cast<std::uint32_t>(x * 31 + y * 17);
    h = (h << 5) + h + static_cast<std::uint32_t>(hp * 13 + qi * 7);
    h = (h << 5) + h + static_cast<std::uint32_t>(level * 101 + xp * 3);
    h = (h << 5) + h + static_cast<std::uint32_t>(party * 23 + ids[0] * 7 + ids[1] * 11 + ids[2] * 19);
    h = (h << 5) + h + (contracted ? 0xAAu : 0x55u);
    h = (h << 5) + h + (artifact ? 0xCCu : 0x33u);
    h = (h << 5) + h + (shrine_seen ? 0xF0u : 0x0Fu);
    h = (h << 5) + h + static_cast<std::uint32_t>(region_id * 53 + quest_step * 89);
    return h;
}
}
