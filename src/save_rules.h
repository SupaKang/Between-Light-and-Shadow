#pragma once
#include <cstdint>

namespace save_rules {
constexpr std::uint32_t Magic = 0x59435931u;
constexpr std::uint32_t Version = 8u;

std::uint32_t checksum(
    std::uint32_t version, int x, int y, int hp, int qi, int level, int xp, int party,
    const int ids[3], bool contracted, bool artifact, bool shrine_seen,
    int region_id = 1, int quest_step = 0
);
}
