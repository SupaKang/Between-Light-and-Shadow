#pragma once
#include <cstdint>
#include <cstring>
#include <array>

namespace JoseonRPG {

#pragma pack(push, 1)

struct SaveYokaiSlot {
    char id[16] = {0};
    uint8_t level = 1;
    uint8_t grade = 1;
    uint32_t exp = 0;
    int16_t curHp = 100;
    int16_t curQi = 50;
    uint8_t statusEffect = 0;
    char skillIds[4][16] = {{0}};
};

struct SaveBlock {
    uint32_t magic = 0x4750524A; // "JRPG" in little-endian
    uint16_t version = 1;
    uint32_t playTimeSeconds = 0;

    // Player State (8 bytes)
    uint8_t mapId = 0;
    uint8_t gridX = 7;
    uint8_t gridY = 6;
    uint8_t facing = 0; // 0: South, 1: North, 2: West, 3: East
    uint32_t money = 500;

    // Party State (Max 3) (274 bytes)
    uint8_t partySize = 0;
    SaveYokaiSlot party[3] = {};

    // Artifacts (Max 8) (129 bytes)
    uint8_t artifactCount = 0;
    char artifactIds[8][16] = {{0}};

    // 108 Encyclopedia Bitsets (28 bytes)
    uint8_t encyclopediaSeen[14] = {0};
    uint8_t encyclopediaCaptured[14] = {0};

    // Quest Flags (22 bytes)
    char currentMainQuestId[16] = "MQ_001";
    uint8_t mainQuestState = 1; // 0: NotStarted, 1: InProgress, 2: Completed
    uint8_t mainQuestStep = 0;
    uint32_t completedQuestBits = 0;

    // Reserved Padding to align to exactly 510 bytes before 2-byte checksum
    uint8_t reserved[39] = {0};

    // 16-bit Checksum (2 bytes)
    uint16_t checksum = 0;
};

#pragma pack(pop)

static_assert(sizeof(SaveBlock) == 512, "SaveBlock size must be exactly 512 bytes!");

} // namespace JoseonRPG
