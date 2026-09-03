#include "tilemap.hpp"
#include "../core/renderer.hpp"
#include <algorithm>
#include <cmath>

namespace JoseonRPG {

Tilemap::Tilemap() {
    loadMap(0);
}

void Tilemap::loadMap(int mapId) {
    m_mapId = mapId;
    m_warps.clear();
    m_chests.clear();

    switch (mapId) {
        case 0:  initMap0_Village(); break;
        case 1:  initMap1_TavernInterior(); break;
        case 2:  initMap2_ExorcistBureau(); break;
        case 3:  initMap3_MountainPass(); break;
        case 4:  initMap4_MountainCave(); break;
        case 5:  initMap5_BossTemple(); break;
        case 6:  initMap6_JungryeongPass(); break;
        case 7:  initMap7_SobaekCanyon(); break;
        case 8:  initMap8_SimmaniHut(); break;
        case 9:  initMap9_IronMineUpper(); break;
        case 10: initMap10_IronMineDeep(); break;
        case 11: initMap11_NamhaeReeds(); break;
        case 12: initMap12_NamhaePort(); break;
        case 13: initMap13_HaenyeoShelter(); break;
        case 14: initMap14_GhostShipUpper(); break;
        case 15: initMap15_GhostShipDeep(); break;
        case 16: initMap16_JirisanEntry(); break;
        case 17: initMap17_JirisanForest(); break;
        case 18: initMap18_DosaHermitage(); break;
        case 19: initMap19_FoxValleyForest(); break;
        case 20: initMap20_FoxGrottoDungeon(); break;
        case 21: initMap21_FortressMoat(); break;
        case 22: initMap22_FortressCorridor(); break;
        case 23: initMap23_GuardianTower(); break;
        case 24: initMap24_FinalSanctum(); break;
        case 25: initMap25_OriginAbyss(); break;
        case 26: initMap26_HanyangBoulevard(); break;
        case 27: initMap27_Geumgangsan(); break;
        case 28: initMap28_HeuksanDragonPalace(); break;
        case 29: initMap29_BaekdusanSanctuary(); break;
        case 30: initMap30_PantheonLabyrinth(); break;
        case 31: initMap31_SubterraneanAqueduct(); break;
        case 32: initMap32_BeaconRidge(); break;
        case 33: initMap33_SunkenGrotto(); break;
        case 34: initMap34_FoxLeylinePass(); break;
        case 35: initMap35_SecretCatacombs(); break;
        case 36: initMap36_SamshindanApex(); break;
        case 37: initMap37_ElderHanokInterior(); break;
        case 38: initMap38_ImjinFerryCrossing(); break;
        default: initMap0_Village(); break;
    }
}

static void makeBoundaryWalls(int w, int h, std::vector<int>& tiles, std::vector<bool>& collision, int wallTile = 1) {
    for (int x = 0; x < w; ++x) {
        tiles[0 * w + x] = wallTile; collision[0 * w + x] = true;
        tiles[(h - 1) * w + x] = wallTile; collision[(h - 1) * w + x] = true;
    }
    for (int y = 0; y < h; ++y) {
        tiles[y * w + 0] = wallTile; collision[y * w + 0] = true;
        tiles[y * w + (w - 1)] = wallTile; collision[y * w + (w - 1)] = true;
    }
}

// -------------------------------------------------------------
// [Map 0] 도선사 주막마을 (80 x 60) - 포켓몬 골드 규격 정밀 메타타일셋 배치
// -------------------------------------------------------------
void Tilemap::initMap0_Village() {
    m_mapName = "제1구역: 도선사 주막마을";
    m_width = 80;
    m_height = 60;
    m_tiles.assign(m_width * m_height, 0); // Grass base (Tile 0)
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 16); // Forest Boundary (Tile 16)

    // Helper Lambdas for Metatile Prefabs
    auto placeGiwaHouse = [this](int startX, int startY, int w, int h, int doorX, int warpTargetMap, int warpTargetX, int warpTargetY) {
        // Roof layer (Top 2 rows)
        for (int x = startX; x < startX + w; ++x) {
            m_tiles[startY * m_width + x] = (x == startX) ? 5 : ((x == startX + w - 1) ? 7 : 6);
            m_collision[startY * m_width + x] = true;
            m_tiles[(startY + 1) * m_width + x] = 6;
            m_collision[(startY + 1) * m_width + x] = true;
        }
        // Wall & Door Layer (Rows 2..h-1)
        for (int y = startY + 2; y < startY + h; ++y) {
            for (int x = startX; x < startX + w; ++x) {
                if (y == startY + h - 1 && x == doorX) {
                    m_tiles[y * m_width + x] = 9; // Changhoji Door (Tile 9)
                    m_collision[y * m_width + x] = false;
                    if (warpTargetMap >= 0) {
                        m_warps.push_back({x, y, warpTargetMap, warpTargetX, warpTargetY});
                    }
                } else {
                    m_tiles[y * m_width + x] = 8; // Pillar Wall (Tile 8)
                    m_collision[y * m_width + x] = true;
                }
            }
        }
    };

    auto placeChogaHouse = [this](int startX, int startY, int w, int h, int doorX, int warpTargetMap, int warpTargetX, int warpTargetY) {
        // Thatched Roof layer
        for (int x = startX; x < startX + w; ++x) {
            m_tiles[startY * m_width + x] = (x == startX) ? 10 : ((x == startX + w - 1) ? 12 : 11);
            m_collision[startY * m_width + x] = true;
            m_tiles[(startY + 1) * m_width + x] = 11;
            m_collision[(startY + 1) * m_width + x] = true;
        }
        // Mud Wall & Door
        for (int y = startY + 2; y < startY + h; ++y) {
            for (int x = startX; x < startX + w; ++x) {
                if (y == startY + h - 1 && x == doorX) {
                    m_tiles[y * m_width + x] = 9; // Door
                    m_collision[y * m_width + x] = false;
                    if (warpTargetMap >= 0) {
                        m_warps.push_back({x, y, warpTargetMap, warpTargetX, warpTargetY});
                    }
                } else {
                    m_tiles[y * m_width + x] = 13; // Mud Wall (Tile 13)
                    m_collision[y * m_width + x] = true;
                }
            }
        }
    };

    auto placeDangsanTree = [this](int rootX, int rootY) {
        // 2x2 Canopy Top + 2x1 Trunk Base
        m_tiles[(rootY - 2) * m_width + rootX] = 14; m_collision[(rootY - 2) * m_width + rootX] = true;
        m_tiles[(rootY - 2) * m_width + rootX + 1] = 14; m_collision[(rootY - 2) * m_width + rootX + 1] = true;
        m_tiles[(rootY - 1) * m_width + rootX] = 14; m_collision[(rootY - 1) * m_width + rootX] = true;
        m_tiles[(rootY - 1) * m_width + rootX + 1] = 14; m_collision[(rootY - 1) * m_width + rootX + 1] = true;
        m_tiles[rootY * m_width + rootX] = 15; m_collision[rootY * m_width + rootX] = true;
        m_tiles[rootY * m_width + rootX + 1] = 15; m_collision[rootY * m_width + rootX + 1] = true;
    };

    // 1. Natural Organic Winding River / Stream (X=52..56)
    for (int y = 1; y < m_height - 1; ++y) {
        int streamX = 52 + static_cast<int>(3.5 * std::sin(y * 0.18));
        for (int w = 0; w < 3; ++w) {
            int cx = streamX + w;
            if (cx >= 1 && cx < m_width - 1) {
                m_tiles[y * m_width + cx] = 17; // Water (Tile 17)
                m_collision[y * m_width + cx] = true;
            }
        }
    }
    // Wooden Arched Bridge across stream
    for (int y = 28; y <= 31; ++y) {
        int streamX = 52 + static_cast<int>(3.5 * std::sin(y * 0.18));
        for (int w = 0; w < 3; ++w) {
            int cx = streamX + w;
            m_tiles[y * m_width + cx] = 18; // Bridge (Tile 18)
            m_collision[y * m_width + cx] = false;
        }
    }

    // 2. Structured Dirt Paths & Plaza
    // Central Crossroads Plaza (Tile 2 Paved Cobblestone)
    for (int y = 24; y <= 34; ++y) {
        for (int x = 30; x <= 42; ++x) {
            m_tiles[y * m_width + x] = 2;
        }
    }
    // West Main Road to entrance (Tile 1 Dirt Road)
    for (int x = 1; x < 30; ++x) {
        int ry = 28 + static_cast<int>(2.5 * std::sin(x * 0.22));
        for (int dy = -1; dy <= 1; ++dy) {
            if (ry + dy >= 1 && ry + dy < m_height - 1) m_tiles[(ry + dy) * m_width + x] = 1;
        }
    }
    // East Road across Bridge
    for (int x = 42; x < m_width - 1; ++x) {
        int ry = 29 + static_cast<int>(1.8 * std::cos(x * 0.2));
        for (int dy = -1; dy <= 1; ++dy) {
            if (ry + dy >= 1 && ry + dy < m_height - 1) {
                if (m_tiles[(ry + dy) * m_width + x] != 18 && m_tiles[(ry + dy) * m_width + x] != 17) {
                    m_tiles[(ry + dy) * m_width + x] = 1;
                }
            }
        }
    }
    // North & South Branching Paths
    for (int y = 1; y < 24; ++y) {
        int rx = 36 + static_cast<int>(3.0 * std::sin(y * 0.15));
        for (int dx = -1; dx <= 1; ++dx) m_tiles[y * m_width + (rx + dx)] = 1;
    }
    for (int y = 34; y < m_height - 1; ++y) {
        int rx = 38 + static_cast<int>(3.0 * std::cos(y * 0.15));
        for (int dx = -1; dx <= 1; ++dx) m_tiles[y * m_width + (rx + dx)] = 1;
    }

    // 3. Compact Pokemon-Scale Buildings (4x3 / 5x3) & Dol-dam Courtyard Enclosures
    // [Building A] Village Tavern Inn (X=12~16, Y=12~14: 5x3)
    placeGiwaHouse(12, 12, 5, 3, 14, 1, 5, 6);
    // Tavern Courtyard Dol-dam Fences (ㄷ자형 돌담 감싸기)
    for (int x = 10; x <= 18; ++x) { m_tiles[10 * m_width + x] = 3; m_collision[10 * m_width + x] = true; }
    for (int y = 11; y <= 16; ++y) {
        m_tiles[y * m_width + 10] = 4; m_collision[y * m_width + 10] = true;
        m_tiles[y * m_width + 18] = 4; m_collision[y * m_width + 18] = true;
    }
    // Front Fence with Gate opening at X=14
    for (int x = 10; x <= 18; ++x) {
        if (x != 14) { m_tiles[16 * m_width + x] = 3; m_collision[16 * m_width + x] = true; }
        else { m_tiles[16 * m_width + x] = 1; m_collision[16 * m_width + x] = false; }
    }
    // Tavern Courtyard Tables & Onggi Soy Pots
    m_tiles[15 * m_width + 12] = 24; m_collision[15 * m_width + 12] = true;
    m_tiles[15 * m_width + 16] = 24; m_collision[15 * m_width + 16] = true;
    m_tiles[11 * m_width + 11] = 19; m_collision[11 * m_width + 11] = true;
    m_tiles[12 * m_width + 11] = 19; m_collision[12 * m_width + 11] = true;

    // [Building B] Exorcist Bureau Branch (X=14~18, Y=36~38: 5x3)
    placeGiwaHouse(14, 36, 5, 3, 16, 2, 5, 6);
    for (int x = 12; x <= 20; ++x) { m_tiles[34 * m_width + x] = 3; m_collision[34 * m_width + x] = true; }
    for (int y = 35; y <= 40; ++y) {
        m_tiles[y * m_width + 12] = 4; m_collision[y * m_width + 12] = true;
        m_tiles[y * m_width + 20] = 4; m_collision[y * m_width + 20] = true;
    }
    for (int x = 12; x <= 20; ++x) {
        if (x != 16) { m_tiles[40 * m_width + x] = 3; m_collision[40 * m_width + x] = true; }
        else { m_tiles[40 * m_width + x] = 1; m_collision[40 * m_width + x] = false; }
    }

    // [Building C] Village Elder Hanok House (X=28~32, Y=10~12: 5x3)
    placeGiwaHouse(28, 10, 5, 3, 30, 37, 12, 16);
    for (int x = 26; x <= 34; ++x) { m_tiles[8 * m_width + x] = 3; m_collision[8 * m_width + x] = true; }
    for (int y = 9; y <= 14; ++y) {
        m_tiles[y * m_width + 26] = 4; m_collision[y * m_width + 26] = true;
        m_tiles[y * m_width + 34] = 4; m_collision[y * m_width + 34] = true;
    }
    for (int x = 26; x <= 34; ++x) {
        if (x != 30) { m_tiles[14 * m_width + x] = 3; m_collision[14 * m_width + x] = true; }
        else { m_tiles[14 * m_width + x] = 1; m_collision[14 * m_width + x] = false; }
    }

    // [Building D] Riverside Thatched Cottage (X=64~68, Y=14~16: 5x3)
    placeChogaHouse(64, 14, 5, 3, 66, -1, 0, 0);
    for (int x = 62; x <= 70; ++x) { m_tiles[12 * m_width + x] = 3; m_collision[12 * m_width + x] = true; }
    for (int y = 13; y <= 18; ++y) {
        m_tiles[y * m_width + 62] = 4; m_collision[y * m_width + 62] = true;
        m_tiles[y * m_width + 70] = 4; m_collision[y * m_width + 70] = true;
    }
    for (int x = 62; x <= 70; ++x) {
        if (x != 66) { m_tiles[18 * m_width + x] = 3; m_collision[18 * m_width + x] = true; }
        else { m_tiles[18 * m_width + x] = 1; m_collision[18 * m_width + x] = false; }
    }

    // 4. Korean Folklore Environmental Props & Natural Landmark Prefabs
    // Central Sacred Dangsan Tree in Plaza
    placeDangsanTree(36, 26);
    m_tiles[26 * m_width + 39] = 20; m_collision[26 * m_width + 39] = true; // Dol-tap Altar (Tile 20)

    // Village West Entrance: Great General Jangseung Totem Pair (Tile 21)
    m_tiles[26 * m_width + 4] = 21; m_collision[26 * m_width + 4] = true;
    m_tiles[31 * m_width + 4] = 21; m_collision[31 * m_width + 4] = true;

    // Hongsalmun Gate before Mountain Pass (Tile 27)
    m_tiles[27 * m_width + 74] = 27; m_collision[27 * m_width + 74] = true;
    m_tiles[31 * m_width + 74] = 27; m_collision[31 * m_width + 74] = true;

    // Onggi Kimchi / Soy Pots Clusters in Courtyards (Tile 19)
    m_tiles[10 * m_width + 27] = 19; m_collision[10 * m_width + 27] = true;
    m_tiles[15 * m_width + 72] = 19; m_collision[15 * m_width + 72] = true;

    // Lotus Pond in Southern Meadow (Tile 22)
    for (int dy = 0; dy < 3; ++dy) {
        for (int dx = 0; dx < 4; ++dx) {
            m_tiles[(48 + dy) * m_width + (60 + dx)] = 22;
            m_collision[(48 + dy) * m_width + (60 + dx)] = true;
        }
    }

    // Secret Stone Well Rim (Tile 23, Shortcut to Map 31)
    m_tiles[50 * m_width + 22] = 23; m_collision[50 * m_width + 22] = false;
    m_warps.push_back({22, 50, 31, 30, 2});

    // Treasure Chests (Tile 28)
    m_tiles[8 * m_width + 11] = 28; m_collision[8 * m_width + 11] = true;
    m_chests.push_back({1, 11, 8, "", 200, 100, false});
    m_tiles[13 * m_width + 73] = 28; m_collision[13 * m_width + 73] = true;
    m_chests.push_back({2, 73, 13, "", 300, 150, false});

    // Exits & Warps
    m_tiles[29 * m_width + 78] = 1; m_collision[29 * m_width + 78] = false;
    m_tiles[29 * m_width + 79] = 1; m_collision[29 * m_width + 79] = false;
    m_warps.push_back({79, 29, 3, 20, 2});
    m_warps.push_back({78, 29, 3, 20, 2});
    m_warps.push_back({79, 30, 3, 20, 2}); // Compatibility
    m_warps.push_back({78, 30, 3, 20, 2}); // Compatibility
    m_warps.push_back({14, 18, 1, 12, 16}); // Backward compatibility
    m_warps.push_back({28, 18, 2, 12, 16}); // Backward compatibility
    m_warps.push_back({20, 50, 31, 30, 2}); // Backward compatibility

    // South Warp to Map 26 (한양 육조거리)
    m_tiles[59 * m_width + 38] = 1; m_collision[59 * m_width + 38] = false;
    m_warps.push_back({38, 59, 26, 45, 58});

    // West Warp to Map 38 (임진나루 도강지대)
    m_tiles[28 * m_width + 0] = 1; m_collision[28 * m_width + 0] = false;
    m_warps.push_back({0, 28, 38, 88, 30});
}

// -------------------------------------------------------------
// [Map 1] 주막 본채 실내 (10 x 8) - 포켓몬 금/은 정통 실내 규격
// -------------------------------------------------------------
void Tilemap::initMap1_TavernInterior() {
    m_mapName = "도선사 주막 본채 (실내)";
    m_width = 10;
    m_height = 8;
    m_tiles.assign(m_width * m_height, 29); // Maru wood floor (Tile 29)
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 8); // Pillar walls (Tile 8)

    // Inner Kitchen Partition (Folding Screen Tile 30)
    m_tiles[1 * m_width + 3] = 30; m_collision[1 * m_width + 3] = true;
    m_tiles[2 * m_width + 3] = 30; m_collision[2 * m_width + 3] = true;
    m_tiles[3 * m_width + 3] = 30; m_collision[3 * m_width + 3] = true;

    // Tavern Dining Table & Bench (Tile 24)
    m_tiles[3 * m_width + 6] = 24; m_collision[3 * m_width + 6] = true;
    m_tiles[4 * m_width + 6] = 24; m_collision[4 * m_width + 6] = true;

    // Exit Door to Village Courtyard (Tile 9 Changhoji Door)
    m_tiles[7 * m_width + 5] = 9; m_collision[7 * m_width + 5] = false;
    m_warps.push_back({5, 7, 0, 15, 16});
}

// -------------------------------------------------------------
// [Map 2] 관상감 벽사청 북악출장소 (10 x 8) - 포켓몬 정통 실내 규격
// -------------------------------------------------------------
void Tilemap::initMap2_ExorcistBureau() {
    m_mapName = "관상감 벽사청 북악출장소 (실내)";
    m_width = 10;
    m_height = 8;
    m_tiles.assign(m_width * m_height, 29);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 8);

    m_tiles[2 * m_width + 5] = 31; m_collision[2 * m_width + 5] = true; // Ceremonial Desk (Tile 31)
    m_tiles[2 * m_width + 8] = 28; m_collision[2 * m_width + 8] = true; // Key Relic Chest (Tile 28)
    m_chests.push_back({3, 8, 2, "ART_HEONGSAL_WOOD", 250, 200, false});

    // Exit Door to Village (Tile 9 Changhoji Door)
    m_tiles[7 * m_width + 5] = 9; m_collision[7 * m_width + 5] = false;
    m_warps.push_back({5, 7, 0, 28, 19});
}

// -------------------------------------------------------------
// [Map 3] 북한산 대협곡 종단로 (40 x 120)
// -------------------------------------------------------------
void Tilemap::initMap3_MountainPass() {
    m_mapName = "제1구역: 북한산 대협곡 종단로";
    m_width = 40;
    m_height = 120;
    m_tiles.assign(m_width * m_height, 0);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    for (int y = 1; y < m_height - 1; ++y) {
        int rx = 20 + ((y / 5) % 4);
        m_tiles[y * m_width + rx] = 6; m_collision[y * m_width + rx] = true;
    }
    m_tiles[30 * m_width + 22] = 7; m_collision[30 * m_width + 22] = false;
    m_tiles[60 * m_width + 21] = 7; m_collision[60 * m_width + 21] = false;
    m_tiles[90 * m_width + 23] = 7; m_collision[90 * m_width + 23] = false;

    for (int y = 1; y < m_height - 1; ++y) {
        int px = (y < 30) ? 14 : ((y < 60) ? 26 : ((y < 90) ? 14 : 20));
        m_tiles[y * m_width + px] = 2;
    }

    // Mountain Highway Props: Doltap & Dangsan Trees
    m_tiles[20 * m_width + 10] = 13; m_collision[20 * m_width + 10] = true;
    m_tiles[50 * m_width + 30] = 5;  m_collision[50 * m_width + 30] = true;
    m_tiles[80 * m_width + 10] = 13; m_collision[80 * m_width + 10] = true;
    m_tiles[110 * m_width + 25] = 12; m_collision[110 * m_width + 25] = true; // Jangseung

    // Cave Entrance (Map 4) at (8, 55)
    m_tiles[55 * m_width + 8] = 9;
    m_warps.push_back({8, 55, 4, 18, 32});

    // Chests
    m_tiles[40 * m_width + 32] = 10;
    m_chests.push_back({4, 32, 40, "ART_SAMDU_FEATHER", 300, 400, false});

    m_tiles[90 * m_width + 6] = 10;
    m_chests.push_back({5, 6, 90, "", 500, 250, false});

    // North Return to Village
    m_tiles[1 * m_width + 20] = 2; m_collision[1 * m_width + 20] = false;
    m_warps.push_back({20, 1, 0, 78, 30});

    // South Exit to Temple (Map 5)
    m_tiles[119 * m_width + 20] = 2; m_collision[119 * m_width + 20] = false;
    m_warps.push_back({20, 119, 5, 24, 2});
}

// -------------------------------------------------------------
// [Map 4] 북한산 고대 석굴 (36 x 36)
// -------------------------------------------------------------
void Tilemap::initMap4_MountainCave() {
    m_mapName = "북한산 고대 석굴 (지하 1층)";
    m_width = 36;
    m_height = 36;
    m_tiles.assign(m_width * m_height, 2);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    for (int y = 6; y <= 28; y += 6) {
        for (int x = 6; x <= 30; x += 6) {
            m_tiles[y * m_width + x] = 1; m_collision[y * m_width + x] = true;
        }
    }

    m_tiles[6 * m_width + 18] = 9;
    m_tiles[6 * m_width + 19] = 10;
    m_chests.push_back({6, 19, 6, "ART_JADE_MIRROR", 400, 500, false});

    m_tiles[35 * m_width + 18] = 2; m_collision[35 * m_width + 18] = false;
    m_warps.push_back({18, 35, 3, 8, 56});

    m_tiles[18 * m_width + 35] = 23; m_collision[18 * m_width + 35] = false; // Secret Aqueduct Rim
    m_warps.push_back({35, 18, 31, 0, 30});
}

// -------------------------------------------------------------
// [Map 5] 도선사 대웅전 (48 x 36)
// -------------------------------------------------------------
void Tilemap::initMap5_BossTemple() {
    m_mapName = "제1구역: 도선사 대웅전 (음양당 제단)";
    m_width = 48;
    m_height = 36;
    m_tiles.assign(m_width * m_height, 3);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    for (int y = 8; y <= 26; y += 6) {
        m_tiles[y * m_width + 14] = 1; m_collision[y * m_width + 14] = true;
        m_tiles[y * m_width + 34] = 1; m_collision[y * m_width + 34] = true;
    }

    m_tiles[26 * m_width + 24] = 9; // Boss Altar

    m_tiles[8 * m_width + 8] = 10;
    m_chests.push_back({7, 8, 8, "ART_DOKKAEBI_HAT", 500, 300, false});

    m_tiles[1 * m_width + 24] = 2; m_collision[1 * m_width + 24] = false;
    m_warps.push_back({24, 1, 3, 20, 118});

    m_tiles[26 * m_width + 47] = 2; m_collision[26 * m_width + 47] = false;
    m_warps.push_back({47, 26, 6, 4, 30}); // To Jungryeong Pass (Map 6)
}

// -------------------------------------------------------------
// [Map 6] 죽령 옛길 초입 (80 x 60)
// -------------------------------------------------------------
void Tilemap::initMap6_JungryeongPass() {
    m_mapName = "제2구역: 소백산맥 죽령 옛길";
    m_width = 80;
    m_height = 60;
    m_tiles.assign(m_width * m_height, 0);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    for (int x = 2; x < 78; ++x) { m_tiles[30 * m_width + x] = 2; }
    for (int y = 10; y < 50; ++y) { m_tiles[y * m_width + 40] = 2; }

    m_tiles[12 * m_width + 50] = 10;
    m_chests.push_back({8, 50, 12, "ART_GAKSI_MASK", 550, 350, false});

    m_tiles[30 * m_width + 1] = 2; m_collision[30 * m_width + 1] = false;
    m_warps.push_back({1, 30, 5, 46, 26}); // Back to Temple

    m_tiles[30 * m_width + 79] = 2; m_collision[30 * m_width + 79] = false;
    m_warps.push_back({79, 30, 7, 4, 25}); // To Sobaek Canyon (Map 7)
}

// -------------------------------------------------------------
// [Map 7] 소백산맥 대협곡길 (100 x 50)
// -------------------------------------------------------------
void Tilemap::initMap7_SobaekCanyon() {
    m_mapName = "제2구역: 소백산맥 대협곡길";
    m_width = 100;
    m_height = 50;
    m_tiles.assign(m_width * m_height, 0);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    for (int x = 2; x < m_width - 1; ++x) { m_tiles[25 * m_width + x] = 2; }
    for (int y = 12; y <= 25; ++y) { m_tiles[y * m_width + 35] = 2; }
    for (int y = 25; y <= 48; ++y) { m_tiles[y * m_width + 85] = 2; }

    // Sobaek Canyon Props: Doltaps & Beacon Banner
    m_tiles[22 * m_width + 25] = 13; m_collision[22 * m_width + 25] = true;
    m_tiles[22 * m_width + 70] = 13; m_collision[22 * m_width + 70] = true;
    m_tiles[23 * m_width + 88] = 24; m_collision[23 * m_width + 88] = true; // Dancheong Banner

    // Simmani Hut Entrance (Map 8) at (20, 20)
    m_tiles[20 * m_width + 20] = 4;
    m_warps.push_back({20, 20, 8, 10, 14});

    // Mine Entrance (Map 9) at (35, 12)
    m_tiles[12 * m_width + 35] = 9;
    m_warps.push_back({35, 12, 9, 25, 46});

    m_tiles[8 * m_width + 60] = 10;
    m_chests.push_back({9, 60, 8, "ART_BULL_IRON_RING", 600, 400, false});

    m_tiles[25 * m_width + 1] = 2; m_collision[25 * m_width + 1] = false;
    m_warps.push_back({1, 25, 6, 78, 30});

    m_tiles[49 * m_width + 85] = 2; m_collision[49 * m_width + 85] = false;
    m_warps.push_back({85, 49, 11, 30, 2}); // To Namhae Reeds (Map 11)

    m_tiles[10 * m_width + 90] = 24; m_collision[10 * m_width + 90] = false; // Beacon Ridge Banner (Map 32)
    m_warps.push_back({90, 10, 32, 0, 60}); // To Beacon Ridge (Map 32)
}

// -------------------------------------------------------------
// [Map 8] 심마니 오두막 실내 (20 x 16)
// -------------------------------------------------------------
void Tilemap::initMap8_SimmaniHut() {
    m_mapName = "소백산 심마니 오두막 (실내)";
    m_width = 20;
    m_height = 16;
    m_tiles.assign(m_width * m_height, 3);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    m_tiles[4 * m_width + 14] = 10;
    m_chests.push_back({10, 14, 4, "ART_DUDURI_HAMMER", 400, 300, false});

    m_tiles[15 * m_width + 10] = 2; m_collision[15 * m_width + 10] = false;
    m_warps.push_back({10, 15, 7, 20, 21});
}

// -------------------------------------------------------------
// [Map 9] 소백산 무쇠광산 상층 갱도 (50 x 50)
// -------------------------------------------------------------
void Tilemap::initMap9_IronMineUpper() {
    m_mapName = "소백산 무쇠광산 상층 갱도";
    m_width = 50;
    m_height = 50;
    m_tiles.assign(m_width * m_height, 2);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    for (int y = 10; y <= 40; y += 8) {
        for (int x = 10; x <= 40; x += 8) {
            m_tiles[y * m_width + x] = 16; m_collision[y * m_width + x] = true; // Iron Ore Vein
        }
    }

    // Deep Shaft Entrance (Map 10) at (25, 10)
    m_tiles[10 * m_width + 25] = 9;
    m_warps.push_back({25, 10, 10, 25, 46});

    m_tiles[8 * m_width + 42] = 10;
    m_chests.push_back({11, 42, 8, "ART_DEMON_TILE", 700, 500, false});

    m_tiles[49 * m_width + 25] = 2; m_collision[49 * m_width + 25] = false;
    m_warps.push_back({25, 49, 7, 35, 13});
}

// -------------------------------------------------------------
// [Map 10] 소백산 무쇠광산 최심부 (50 x 50)
// -------------------------------------------------------------
void Tilemap::initMap10_IronMineDeep() {
    m_mapName = "소백산 무쇠광산 최심부 (Ch.2 보스)";
    m_width = 50;
    m_height = 50;
    m_tiles.assign(m_width * m_height, 3);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    // Smelting Anvils & Iron Ore clusters
    m_tiles[20 * m_width + 15] = 16; m_collision[20 * m_width + 15] = true;
    m_tiles[20 * m_width + 35] = 16; m_collision[20 * m_width + 35] = true;

    m_tiles[12 * m_width + 25] = 9; // Boss Altar

    m_tiles[6 * m_width + 38] = 10;
    m_chests.push_back({12, 38, 6, "ART_BLACK_TIGER_TALON", 800, 600, false});

    m_tiles[49 * m_width + 25] = 2; m_collision[49 * m_width + 25] = false;
    m_warps.push_back({25, 49, 9, 25, 11});
}

// -------------------------------------------------------------
// [Map 11] 남해안 갈대밭 샛길 (60 x 60)
// -------------------------------------------------------------
void Tilemap::initMap11_NamhaeReeds() {
    m_mapName = "제3구역: 남해안 갈대밭 샛길";
    m_width = 60;
    m_height = 60;
    m_tiles.assign(m_width * m_height, 0);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    // Bamboo / Reed Groves (8)
    for (int y = 4; y < 56; ++y) {
        for (int x = 4; x < 56; ++x) {
            if ((x % 3 == 0) && (y % 3 != 0)) m_tiles[y * m_width + x] = 8;
        }
    }

    // Lotus Pond & Coastal Props
    m_tiles[15 * m_width + 20] = 22; m_collision[15 * m_width + 20] = true;
    m_tiles[15 * m_width + 21] = 22; m_collision[15 * m_width + 21] = true;
    m_tiles[40 * m_width + 45] = 21; m_collision[40 * m_width + 45] = true; // Rotten net/timber

    for (int y = 1; y < 59; ++y) { m_tiles[y * m_width + 30] = 2; }
    for (int x = 1; x < 59; ++x) { m_tiles[30 * m_width + x] = 2; }

    m_tiles[10 * m_width + 10] = 10;
    m_chests.push_back({13, 10, 10, "ART_GHOST_BELL", 700, 450, false});

    m_tiles[1 * m_width + 30] = 2; m_collision[1 * m_width + 30] = false;
    m_warps.push_back({30, 1, 7, 85, 48}); // Back to Sobaek Canyon (Map 7)

    m_tiles[59 * m_width + 30] = 2; m_collision[59 * m_width + 30] = false;
    m_warps.push_back({30, 59, 12, 40, 2}); // To Namhae Port (Map 12)

    m_tiles[50 * m_width + 10] = 23; m_collision[50 * m_width + 10] = false; // Secret Aqueduct Rim (Map 31)
    m_warps.push_back({10, 50, 31, 58, 30});
}

// -------------------------------------------------------------
// [Map 12] 남해안 갯벌 포구 (80 x 50)
// -------------------------------------------------------------
void Tilemap::initMap12_NamhaePort() {
    m_mapName = "제3구역: 남해안 갯벌 포구";
    m_width = 80;
    m_height = 50;
    m_tiles.assign(m_width * m_height, 0);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    for (int y = 35; y < 49; ++y) {
        for (int x = 1; x < m_width - 1; ++x) {
            m_tiles[y * m_width + x] = 6; m_collision[y * m_width + x] = true;
        }
    }
    for (int x = 15; x <= 65; ++x) {
        m_tiles[38 * m_width + x] = 7; m_collision[38 * m_width + x] = false;
    }

    for (int y = 1; y <= 38; ++y) { m_tiles[y * m_width + 40] = 2; }
    for (int x = 40; x <= 78; ++x) { m_tiles[25 * m_width + x] = 2; }

    // Harbor Market Props: Onggi pots & Fishing nets
    m_tiles[15 * m_width + 35] = 14; m_collision[15 * m_width + 35] = true;
    m_tiles[15 * m_width + 36] = 14; m_collision[15 * m_width + 36] = true;
    m_tiles[36 * m_width + 25] = 21; m_collision[36 * m_width + 25] = true;

    // Haenyeo Shelter (Map 13) at (50, 18)
    m_tiles[18 * m_width + 50] = 4;
    m_warps.push_back({50, 18, 13, 12, 16});

    // Ghost Ship (Map 14) at (20, 38)
    m_tiles[38 * m_width + 20] = 21; // Broken Hull Entry
    m_warps.push_back({20, 38, 14, 20, 26});

    m_tiles[42 * m_width + 68] = 10;
    m_chests.push_back({14, 68, 42, "ART_CENTIPEDE_EGG", 800, 600, false});

    m_tiles[1 * m_width + 40] = 2; m_collision[1 * m_width + 40] = false;
    m_warps.push_back({40, 1, 11, 30, 58});

    m_tiles[25 * m_width + 79] = 2; m_collision[25 * m_width + 79] = false;
    m_warps.push_back({79, 25, 16, 4, 30}); // To Jirisan Entry (Map 16)
}

// -------------------------------------------------------------
// [Map 13] 해녀의 쉼터 실내 (24 x 18)
// -------------------------------------------------------------
void Tilemap::initMap13_HaenyeoShelter() {
    m_mapName = "남해 포구 해녀의 쉼터 (실내)";
    m_width = 24;
    m_height = 18;
    m_tiles.assign(m_width * m_height, 3);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    m_tiles[4 * m_width + 18] = 10;
    m_chests.push_back({15, 18, 4, "ART_SUTTA_BELL", 600, 400, false});

    m_tiles[17 * m_width + 12] = 2; m_collision[17 * m_width + 12] = false;
    m_warps.push_back({12, 17, 12, 50, 19});
}

// -------------------------------------------------------------
// [Map 14] 남해 유령 난파선 갑판 (40 x 30)
// -------------------------------------------------------------
void Tilemap::initMap14_GhostShipUpper() {
    m_mapName = "남해 유령 난파선 갑판 상층";
    m_width = 40;
    m_height = 30;
    m_tiles.assign(m_width * m_height, 3);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    for (int y = 6; y <= 22; y += 4) {
        m_tiles[y * m_width + 10] = 1; m_collision[y * m_width + 10] = true;
        m_tiles[y * m_width + 30] = 1; m_collision[y * m_width + 30] = true;
    }

    // Hatch to Captain's Cabin (Map 15) at (20, 8)
    m_tiles[8 * m_width + 20] = 9;
    m_warps.push_back({20, 8, 15, 20, 26});

    m_tiles[6 * m_width + 6] = 10;
    m_chests.push_back({16, 6, 6, "", 1000, 800, false});

    m_tiles[29 * m_width + 20] = 2; m_collision[29 * m_width + 20] = false;
    m_warps.push_back({20, 29, 12, 20, 39});

    m_tiles[10 * m_width + 10] = 9; m_collision[10 * m_width + 10] = false;
    m_warps.push_back({10, 10, 33, 25, 2}); // To Sunken Grotto (Map 33)
}

// -------------------------------------------------------------
// [Map 15] 남해 유령 난파선 선장실 B1F (40 x 30)
// -------------------------------------------------------------
void Tilemap::initMap15_GhostShipDeep() {
    m_mapName = "남해 유령 난파선 선장실 (Ch.3 보스)";
    m_width = 40;
    m_height = 30;
    m_tiles.assign(m_width * m_height, 3);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    m_tiles[8 * m_width + 20] = 9; // Boss Altar

    m_tiles[6 * m_width + 34] = 10;
    m_chests.push_back({17, 34, 6, "ART_DRAGON_SCALE", 1200, 900, false});

    m_tiles[29 * m_width + 20] = 2; m_collision[29 * m_width + 20] = false;
    m_warps.push_back({20, 29, 14, 20, 9});

    m_tiles[25 * m_width + 35] = 9; m_collision[25 * m_width + 35] = false;
    m_warps.push_back({35, 25, 33, 25, 47}); // To Sunken Grotto (Map 33)
}

// -------------------------------------------------------------
// [Map 16] 지리산 안개 숲길 초입 (60 x 60)
// -------------------------------------------------------------
void Tilemap::initMap16_JirisanEntry() {
    m_mapName = "제4구역: 지리산 안개 숲길 초입";
    m_width = 60;
    m_height = 60;
    m_tiles.assign(m_width * m_height, 0);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    for (int x = 2; x < 58; ++x) { m_tiles[30 * m_width + x] = 2; }

    m_tiles[14 * m_width + 45] = 10;
    m_chests.push_back({18, 45, 14, "ART_CHEONGO_FAN", 900, 700, false});

    m_tiles[30 * m_width + 1] = 2; m_collision[30 * m_width + 1] = false;
    m_warps.push_back({1, 30, 12, 78, 25});

    m_tiles[30 * m_width + 59] = 2; m_collision[30 * m_width + 59] = false;
    m_warps.push_back({59, 30, 17, 4, 40}); // To Jirisan Forest (Map 17)

    m_tiles[5 * m_width + 55] = 9; m_collision[5 * m_width + 55] = false;
    m_warps.push_back({55, 5, 32, 67, 10}); // To Beacon Ridge (Map 32)
}

// -------------------------------------------------------------
// [Map 17] 지리산 대나무 밀림 미궁 (80 x 80)
// -------------------------------------------------------------
void Tilemap::initMap17_JirisanForest() {
    m_mapName = "제4구역: 지리산 대나무 밀림 미궁";
    m_width = 80;
    m_height = 80;
    m_tiles.assign(m_width * m_height, 0);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    for (int y = 4; y < 76; ++y) {
        for (int x = 4; x < 76; ++x) {
            if ((x % 4 == 0) && (y % 4 != 0)) m_tiles[y * m_width + x] = 8;
        }
    }
    for (int x = 2; x < 78; ++x) { m_tiles[40 * m_width + x] = 2; }
    for (int y = 20; y <= 60; ++y) { m_tiles[y * m_width + 55] = 2; }
    for (int y = 12; y <= 40; ++y) { m_tiles[y * m_width + 76] = 2; }

    // Dosa Hermitage (Map 18) at (30, 35)
    m_tiles[35 * m_width + 30] = 4;
    m_warps.push_back({30, 35, 18, 10, 14});

    // Fox Valley (Map 19) at (55, 20)
    m_tiles[20 * m_width + 55] = 9;
    m_warps.push_back({55, 20, 19, 30, 56});

    m_tiles[70 * m_width + 12] = 10;
    m_chests.push_back({19, 12, 70, "ART_FOX_MARBLE_SHARD", 1200, 1000, false});

    m_tiles[40 * m_width + 1] = 2; m_collision[40 * m_width + 1] = false;
    m_warps.push_back({1, 40, 16, 58, 30});

    m_tiles[12 * m_width + 79] = 2; m_collision[12 * m_width + 79] = false;
    m_warps.push_back({79, 12, 21, 4, 30}); // To Fortress Moat (Map 21)

    m_tiles[75 * m_width + 75] = 9; m_collision[75 * m_width + 75] = false;
    m_warps.push_back({75, 75, 34, 0, 30}); // To Fox Leyline Pass (Map 34)
}

// -------------------------------------------------------------
// [Map 18] 백발 도사의 산중 암자 (20 x 16)
// -------------------------------------------------------------
void Tilemap::initMap18_DosaHermitage() {
    m_mapName = "지리산 백발 도사의 산중 암자 (실내)";
    m_width = 20;
    m_height = 16;
    m_tiles.assign(m_width * m_height, 3);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    m_tiles[4 * m_width + 14] = 10;
    m_chests.push_back({20, 14, 4, "ART_EUMYANG_MIRROR", 1000, 800, false});

    m_tiles[15 * m_width + 10] = 2; m_collision[15 * m_width + 10] = false;
    m_warps.push_back({10, 15, 17, 30, 36});
}

// -------------------------------------------------------------
// [Map 19] 여우골 상층 숲 (60 x 60)
// -------------------------------------------------------------
void Tilemap::initMap19_FoxValleyForest() {
    m_mapName = "제4구역: 지리산 여우골 상층 숲";
    m_width = 60;
    m_height = 60;
    m_tiles.assign(m_width * m_height, 0);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    for (int y = 4; y < 58; ++y) { m_tiles[y * m_width + 30] = 2; }

    // Grotto Entrance (Map 20) at (30, 10)
    m_tiles[10 * m_width + 30] = 9;
    m_warps.push_back({30, 10, 20, 25, 46});

    m_tiles[20 * m_width + 15] = 10;
    m_chests.push_back({21, 15, 20, "ART_GUMIHO_TAIL_FUR", 1300, 1000, false});

    m_tiles[59 * m_width + 30] = 2; m_collision[59 * m_width + 30] = false;
    m_warps.push_back({30, 59, 17, 55, 21});
}

// -------------------------------------------------------------
// [Map 20] 여우골 영맥동굴 (50 x 50)
// -------------------------------------------------------------
void Tilemap::initMap20_FoxGrottoDungeon() {
    m_mapName = "여우골 영맥동굴 (Ch.4 보스)";
    m_width = 50;
    m_height = 50;
    m_tiles.assign(m_width * m_height, 2);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    for (int y = 10; y <= 40; y += 10) {
        m_tiles[y * m_width + 12] = 9;
        m_tiles[y * m_width + 38] = 9;
    }

    m_tiles[14 * m_width + 25] = 9; // Boss Altar

    m_tiles[8 * m_width + 40] = 10;
    m_chests.push_back({22, 40, 8, "ART_GOLDEN_BELL", 1500, 1200, false});

    m_tiles[49 * m_width + 25] = 2; m_collision[49 * m_width + 25] = false;
    m_warps.push_back({25, 49, 19, 30, 11});

    m_tiles[45 * m_width + 5] = 9; m_collision[45 * m_width + 5] = false;
    m_warps.push_back({5, 45, 34, 30, 59}); // To Fox Leyline Pass (Map 34)
}

// -------------------------------------------------------------
// [Map 21] 일식의 성채 외성 & 해자 (60 x 60)
// -------------------------------------------------------------
void Tilemap::initMap21_FortressMoat() {
    m_mapName = "제5구역: 일식의 성채 외성 & 해자";
    m_width = 60;
    m_height = 60;
    m_tiles.assign(m_width * m_height, 0);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    for (int y = 6; y < 54; ++y) {
        m_tiles[y * m_width + 28] = 6; m_collision[y * m_width + 28] = true;
        m_tiles[y * m_width + 29] = 6; m_collision[y * m_width + 29] = true;
    }
    m_tiles[30 * m_width + 28] = 7; m_collision[30 * m_width + 28] = false;
    m_tiles[30 * m_width + 29] = 7; m_collision[30 * m_width + 29] = false;

    for (int y = 10; y <= 50; ++y) {
        for (int x = 32; x <= 56; ++x) {
            m_tiles[y * m_width + x] = 3;
        }
    }

    // Portal to Fortress Corridor (Map 22) at (50, 30)
    m_tiles[30 * m_width + 50] = 9;
    m_warps.push_back({50, 30, 22, 4, 30});

    m_tiles[12 * m_width + 14] = 10;
    m_chests.push_back({23, 14, 12, "ART_SHADOW_INCENSE", 2000, 1500, false});

    m_tiles[30 * m_width + 1] = 2; m_collision[30 * m_width + 1] = false;
    m_warps.push_back({1, 30, 17, 78, 12});

    m_tiles[55 * m_width + 55] = 9; m_collision[55 * m_width + 55] = false;
    m_warps.push_back({55, 55, 34, 59, 5}); // To Fox Leyline Pass (Map 34)

    m_tiles[55 * m_width + 5] = 9; m_collision[55 * m_width + 5] = false;
    m_warps.push_back({5, 55, 35, 0, 30}); // To Secret Catacombs (Map 35)
}

// -------------------------------------------------------------
// [Map 22] 성채 내성 흑요석 회랑 (60 x 60)
// -------------------------------------------------------------
void Tilemap::initMap22_FortressCorridor() {
    m_mapName = "일식의 성채 흑요석 회랑";
    m_width = 60;
    m_height = 60;
    m_tiles.assign(m_width * m_height, 3);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    for (int x = 2; x < 58; ++x) { m_tiles[30 * m_width + x] = 2; }

    // Guardian Tower Entrance (Map 23) at (30, 10)
    m_tiles[10 * m_width + 30] = 9;
    m_warps.push_back({30, 10, 23, 20, 36});

    // Final Sanctum Entrance (Map 24) at (55, 30)
    m_tiles[30 * m_width + 55] = 9;
    m_warps.push_back({55, 30, 24, 25, 46});

    m_tiles[48 * m_width + 14] = 10;
    m_chests.push_back({24, 14, 48, "ART_REAPER_SCROLL", 2200, 1600, false});

    m_tiles[30 * m_width + 1] = 2; m_collision[30 * m_width + 1] = false;
    m_warps.push_back({1, 30, 21, 49, 30});
}

// -------------------------------------------------------------
// [Map 23] 음양당 사신수 결계탑 (40 x 40)
// -------------------------------------------------------------
void Tilemap::initMap23_GuardianTower() {
    m_mapName = "음양당 사신수 결계탑 (상층)";
    m_width = 40;
    m_height = 40;
    m_tiles.assign(m_width * m_height, 3);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    m_tiles[8 * m_width + 20] = 10;
    m_chests.push_back({25, 20, 8, "ART_CHILSEONG_SWORD", 3000, 2000, false});

    m_tiles[39 * m_width + 20] = 2; m_collision[39 * m_width + 20] = false;
    m_warps.push_back({20, 39, 22, 30, 11});

    m_tiles[35 * m_width + 35] = 9; m_collision[35 * m_width + 35] = false;
    m_warps.push_back({35, 35, 35, 30, 0}); // To Secret Catacombs (Map 35)
}

// -------------------------------------------------------------
// [Map 24] 천문음양전 중심 제단 (50 x 50)
// -------------------------------------------------------------
void Tilemap::initMap24_FinalSanctum() {
    m_mapName = "천문음양전 중심 제단 (최종 결전지)";
    m_width = 50;
    m_height = 50;
    m_tiles.assign(m_width * m_height, 3);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    for (int y = 8; y <= 40; y += 8) {
        m_tiles[y * m_width + 12] = 9;
        m_tiles[y * m_width + 38] = 9;
    }

    m_tiles[15 * m_width + 25] = 9; // Final Boss Altar

    // Portal to Origin Abyss (Map 25) at (25, 6)
    m_tiles[6 * m_width + 25] = 9;
    m_warps.push_back({25, 6, 25, 25, 46});

    m_tiles[6 * m_width + 42] = 10;
    m_chests.push_back({26, 42, 6, "ART_SUN_ORB", 5000, 3000, false});

    m_tiles[49 * m_width + 25] = 2; m_collision[49 * m_width + 25] = false;
    m_warps.push_back({25, 49, 22, 54, 30});
}

// -------------------------------------------------------------
// [Map 25] 태초의 영맥 심연 (50 x 50)
// -------------------------------------------------------------
void Tilemap::initMap25_OriginAbyss() {
    m_mapName = "태초의 영맥 심연 (진엔딩 신역)";
    m_width = 50;
    m_height = 50;
    m_tiles.assign(m_width * m_height, 3);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    m_tiles[15 * m_width + 25] = 9; // Legendary Spirit Altar

    m_tiles[6 * m_width + 25] = 10;
    m_chests.push_back({27, 25, 6, "ART_TAIJI_CORE", 10000, 5000, false});

    m_tiles[49 * m_width + 25] = 2; m_collision[49 * m_width + 25] = false;
    m_warps.push_back({25, 49, 24, 25, 7});

    // Celestial Gate to Map 36 (태초의 천상 신역 삼신단) at (25, 4)
    m_tiles[4 * m_width + 25] = 18; m_collision[4 * m_width + 25] = false; // Cloud Gate
    m_warps.push_back({25, 4, 36, 40, 78});
}

// -------------------------------------------------------------
// [Map 26] 한양 육조거리 & 경복궁 근정전 외곽 (90 x 60)
// -------------------------------------------------------------
void Tilemap::initMap26_HanyangBoulevard() {
    m_mapName = "한양 육조거리 & 경복궁 외곽";
    m_width = 90;
    m_height = 60;
    m_tiles.assign(m_width * m_height, 2);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    // Stone road across center
    for (int y = 0; y < m_height; ++y) {
        for (int x = 42; x <= 48; ++x) {
            m_tiles[y * m_width + x] = 2;
        }
    }

    // Royal Government Offices & Stone Pavilions & Dancheong Banners
    for (int y = 10; y <= 50; y += 15) {
        for (int x = 10; x <= 30; x += 10) {
            m_tiles[y * m_width + x] = 7;
            m_collision[y * m_width + x] = true;
        }
        for (int x = 60; x <= 80; x += 10) {
            m_tiles[y * m_width + x] = 7;
            m_collision[y * m_width + x] = true;
        }
        // Dancheong Banners along the boulevard
        m_tiles[y * m_width + 40] = 24; m_collision[y * m_width + 40] = true;
        m_tiles[y * m_width + 50] = 24; m_collision[y * m_width + 50] = true;
    }

    // Hongsalmun Gates at North and South
    m_tiles[5 * m_width + 45] = 11;
    m_tiles[55 * m_width + 45] = 11;

    // Chests in Treasury
    m_chests.push_back({28, 15, 12, "ART_SCHOLAR_BRUSH", 3000, 2000, false});
    m_chests.push_back({29, 75, 12, "ART_JADE_MIRROR", 5000, 3500, false});

    // Warp back to Map 0 (도선사 마을 입구)
    m_tiles[59 * m_width + 45] = 2;
    m_warps.push_back({45, 59, 0, 75, 30});

    // Warp to Geumgangsan (Map 27) at north gate
    m_tiles[0 * m_width + 45] = 2; m_collision[0 * m_width + 45] = false;
    m_warps.push_back({45, 0, 27, 40, 68});
}

// -------------------------------------------------------------
// [Map 27] 금강산 일만이천봉 선계 (80 x 70)
// -------------------------------------------------------------
void Tilemap::initMap27_Geumgangsan() {
    m_mapName = "금강산 일만이천봉 선계";
    m_width = 80;
    m_height = 70;
    m_tiles.assign(m_width * m_height, 0);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    // Mountain rock clusters & Celestial Cloud Platforms
    for (int y = 10; y < 60; y += 12) {
        for (int x = 15; x < 70; x += 14) {
            m_tiles[y * m_width + x] = 1;
            m_collision[y * m_width + x] = true;
            // Celestial cloud platform stepping stones
            m_tiles[(y + 4) * m_width + (x + 4)] = 18;
        }
    }

    // Hermit shrine & Dancheong Banners
    m_tiles[20 * m_width + 40] = 9;
    m_tiles[20 * m_width + 38] = 24; m_collision[20 * m_width + 38] = true;
    m_tiles[20 * m_width + 42] = 24; m_collision[20 * m_width + 42] = true;
    m_chests.push_back({30, 42, 20, "ART_HERB_GOURD", 4000, 2500, false});

    // Warp back to Hanyang (Map 26)
    m_tiles[69 * m_width + 40] = 2; m_collision[69 * m_width + 40] = false;
    m_warps.push_back({40, 69, 26, 45, 2});

    // Warp to Heuksan Dragon Palace (Map 28)
    m_tiles[5 * m_width + 75] = 9;
    m_warps.push_back({75, 5, 28, 35, 68});

    m_tiles[65 * m_width + 10] = 24; m_collision[65 * m_width + 10] = false; // Beacon Ridge Banner (Map 32)
    m_warps.push_back({10, 65, 32, 35, 0});
}

// -------------------------------------------------------------
// [Map 28] 흑산도 심해 용궁 (70 x 70)
// -------------------------------------------------------------
void Tilemap::initMap28_HeuksanDragonPalace() {
    m_mapName = "흑산도 심해 용궁";
    m_width = 70;
    m_height = 70;
    m_tiles.assign(m_width * m_height, 4);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    // Bioluminescent Coral columns & Sponges (17)
    for (int y = 15; y < 60; y += 15) {
        for (int x = 15; x < 60; x += 15) {
            m_tiles[y * m_width + x] = 17;
            m_collision[y * m_width + x] = true;
        }
    }

    // Dragon King's Pearl Chest
    m_chests.push_back({31, 35, 15, "ART_DRAGON_SCALE", 8000, 4500, false});

    // Warp back to Geumgangsan (Map 27)
    m_tiles[69 * m_width + 35] = 2; m_collision[69 * m_width + 35] = false;
    m_warps.push_back({35, 69, 27, 74, 6});

    // Warp to Baekdusan Sanctuary (Map 29)
    m_tiles[5 * m_width + 35] = 9;
    m_warps.push_back({35, 5, 29, 40, 78});

    m_tiles[35 * m_width + 10] = 17; m_collision[35 * m_width + 10] = false; // Coral Passage to Sunken Grotto (Map 33)
    m_warps.push_back({10, 35, 33, 49, 25});
}

// -------------------------------------------------------------
// [Map 29] 백두산 천지 백록담 신역 (80 x 80)
// -------------------------------------------------------------
void Tilemap::initMap29_BaekdusanSanctuary() {
    m_mapName = "백두산 천지 백록담 신역";
    m_width = 80;
    m_height = 80;
    m_tiles.assign(m_width * m_height, 0);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    // Sacred Caldera Lake in center with Ice Sheets (19)
    for (int y = 25; y <= 55; ++y) {
        for (int x = 25; x <= 55; ++x) {
            m_tiles[y * m_width + x] = (x == 25 || x == 55 || y == 25 || y == 55) ? 19 : 4;
            m_collision[y * m_width + x] = true;
        }
    }
    // Bridge to central sacred stone
    for (int x = 38; x <= 42; ++x) {
        m_tiles[40 * m_width + x] = 2;
        m_collision[40 * m_width + x] = false;
    }

    m_chests.push_back({32, 40, 39, "ART_TAIJI_CORE", 12000, 6000, false});

    // Warp back to Dragon Palace (Map 28)
    m_tiles[79 * m_width + 40] = 2; m_collision[79 * m_width + 40] = false;
    m_warps.push_back({40, 79, 28, 35, 6});

    // Warp to Pantheon Labyrinth (Map 30)
    m_tiles[5 * m_width + 40] = 9;
    m_warps.push_back({40, 5, 30, 50, 98});
}

// -------------------------------------------------------------
// [Map 30] 태고의 환상비무 미궁 (100 x 100)
// -------------------------------------------------------------
void Tilemap::initMap30_PantheonLabyrinth() {
    m_mapName = "태고의 환상비무 미궁 (궁극의 시련)";
    m_width = 100;
    m_height = 100;
    m_tiles.assign(m_width * m_height, 3);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    // Labyrinth obsidian pillars & Ward stones (20) & Arena Banners (24)
    for (int y = 10; y < 90; y += 10) {
        for (int x = 10; x < 90; x += 10) {
            if (x == 50 && y == 50) continue; // Keep center arena open
            m_tiles[y * m_width + x] = ((x + y) % 20 == 0) ? 20 : 9;
            m_collision[y * m_width + x] = true;
        }
    }

    // Central Arena Banners
    m_tiles[48 * m_width + 48] = 24; m_collision[48 * m_width + 48] = true;
    m_tiles[48 * m_width + 52] = 24; m_collision[48 * m_width + 52] = true;
    m_tiles[52 * m_width + 48] = 24; m_collision[52 * m_width + 48] = true;
    m_tiles[52 * m_width + 52] = 24; m_collision[52 * m_width + 52] = true;

    // Ultimate Celestial Chest
    m_chests.push_back({33, 50, 10, "ART_TAIJI_BELL", 20000, 10000, false});

    // Warp back to Baekdusan (Map 29)
    m_tiles[99 * m_width + 50] = 2; m_collision[99 * m_width + 50] = false;
    m_warps.push_back({50, 99, 29, 40, 6});

    m_tiles[90 * m_width + 10] = 20; m_collision[90 * m_width + 10] = false; // Obsidian Sigil to Catacombs (Map 35)
    m_warps.push_back({10, 90, 35, 59, 55});
}

// -------------------------------------------------------------
// [Map 31] 지하 암반 수로 (도선사-북한산-남해안 비밀 수로) (60 x 60)
// -------------------------------------------------------------
void Tilemap::initMap31_SubterraneanAqueduct() {
    m_mapName = "고대 지하 암반 수로 (비밀 통로)";
    m_width = 60;
    m_height = 60;
    m_tiles.assign(m_width * m_height, 2);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    // Underground canal water streams (6)
    for (int y = 5; y < 55; ++y) {
        m_tiles[y * m_width + 20] = 6; m_collision[y * m_width + 20] = true;
        m_tiles[y * m_width + 40] = 6; m_collision[y * m_width + 40] = true;
    }
    // Canal stone bridges
    for (int x = 18; x <= 22; ++x) { m_tiles[30 * m_width + x] = 7; m_collision[30 * m_width + x] = false; }
    for (int x = 38; x <= 42; ++x) { m_tiles[30 * m_width + x] = 7; m_collision[30 * m_width + x] = false; }

    // Ancient Waterway Chest
    m_chests.push_back({34, 30, 30, "ART_SUTTA_BELL", 2500, 1500, false});

    // North Warp to Map 0 (도선사 마을 우물)
    m_tiles[0 * m_width + 30] = 23; m_collision[0 * m_width + 30] = false; // Well Grate
    m_warps.push_back({30, 0, 0, 20, 50});

    // West Warp to Map 4 (북한산 고대 석굴)
    m_tiles[30 * m_width + 0] = 23; m_collision[30 * m_width + 0] = false;
    m_warps.push_back({0, 30, 4, 35, 18});

    // East Warp to Map 11 (남해안 갈대밭)
    m_tiles[30 * m_width + 59] = 23; m_collision[30 * m_width + 59] = false;
    m_warps.push_back({59, 30, 11, 10, 50});
}

// -------------------------------------------------------------
// [Map 32] 소백-지리 봉화대 능선 (70 x 70)
// -------------------------------------------------------------
void Tilemap::initMap32_BeaconRidge() {
    m_mapName = "소백-지리 봉화대 능선 (산마루 샛길)";
    m_width = 70;
    m_height = 70;
    m_tiles.assign(m_width * m_height, 0);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    // Mountain cliffs & rocky ridges & Doltaps (13)
    for (int y = 15; y < 55; y += 12) {
        for (int x = 15; x < 55; x += 12) {
            m_tiles[y * m_width + x] = 1;
            m_collision[y * m_width + x] = true;
            m_tiles[(y + 2) * m_width + (x + 2)] = 13; // Cairns along ridge
        }
    }

    // Signal Beacon Tower in center & Dancheong Banners (24)
    m_tiles[35 * m_width + 35] = 9;
    m_tiles[34 * m_width + 35] = 24; m_collision[34 * m_width + 35] = true;
    m_chests.push_back({35, 36, 35, "ART_SAMDU_FEATHER", 3500, 2000, false});

    // West Warp to Map 7 (소백산맥 대협곡)
    m_tiles[60 * m_width + 0] = 2; m_collision[60 * m_width + 0] = false;
    m_warps.push_back({0, 60, 7, 90, 10});

    // East Warp to Map 16 (지리산 안개 숲길)
    m_tiles[10 * m_width + 69] = 2; m_collision[10 * m_width + 69] = false;
    m_warps.push_back({69, 10, 16, 55, 5});

    // North Warp to Map 27 (금강산 선계)
    m_tiles[0 * m_width + 35] = 2; m_collision[0 * m_width + 35] = false;
    m_warps.push_back({35, 0, 27, 10, 65});
}

// -------------------------------------------------------------
// [Map 33] 난파선 침수 밀실 & 해저 회랑 (50 x 50)
// -------------------------------------------------------------
void Tilemap::initMap33_SunkenGrotto() {
    m_mapName = "난파선 침수 밀실 & 해저 회랑";
    m_width = 50;
    m_height = 50;
    m_tiles.assign(m_width * m_height, 3);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    // Coral pillars and water pools & Rotten Timber (21)
    for (int y = 12; y < 40; y += 10) {
        for (int x = 12; x < 40; x += 10) {
            m_tiles[y * m_width + x] = 17; // Coral
            m_collision[y * m_width + x] = true;
            m_tiles[(y + 2) * m_width + (x + 2)] = 21; // Timber debris
        }
    }

    m_chests.push_back({36, 25, 25, "ART_YONGWANG_PEARL", 4000, 2500, false});

    // North Warp to Map 14 (난파선 갑판 상층)
    m_tiles[0 * m_width + 25] = 21; m_collision[0 * m_width + 25] = false;
    m_warps.push_back({25, 0, 14, 10, 10});

    // South Warp to Map 15 (난파선 선장실)
    m_tiles[49 * m_width + 25] = 21; m_collision[49 * m_width + 25] = false;
    m_warps.push_back({25, 49, 15, 35, 25});

    // East Warp to Map 28 (흑산도 심해 용궁)
    m_tiles[25 * m_width + 49] = 17; m_collision[25 * m_width + 49] = false;
    m_warps.push_back({49, 25, 28, 10, 35});
}

// -------------------------------------------------------------
// [Map 34] 여우골 영맥 환상 샛길 (60 x 60)
// -------------------------------------------------------------
void Tilemap::initMap34_FoxLeylinePass() {
    m_mapName = "여우골 영맥 환상 샛길";
    m_width = 60;
    m_height = 60;
    m_tiles.assign(m_width * m_height, 0);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    // Glowing foxfire bamboo clusters & Dangsan trees (5)
    for (int y = 10; y < 50; y += 8) {
        for (int x = 10; x < 50; x += 8) {
            if ((x + y) % 16 == 0) {
                m_tiles[y * m_width + x] = 8;
                m_collision[y * m_width + x] = true;
            } else if ((x * y) % 24 == 0) {
                m_tiles[y * m_width + x] = 5; // Sacred Tree
                m_collision[y * m_width + x] = true;
            }
        }
    }

    m_chests.push_back({37, 30, 30, "ART_GUMIHO_TAIL_FUR", 6000, 3500, false});

    // West Warp to Map 17 (지리산 대나무 밀림)
    m_tiles[30 * m_width + 0] = 2; m_collision[30 * m_width + 0] = false;
    m_warps.push_back({0, 30, 17, 75, 75});

    // South Warp to Map 20 (여우골 영맥동굴)
    m_tiles[59 * m_width + 30] = 2; m_collision[59 * m_width + 30] = false;
    m_warps.push_back({30, 59, 20, 5, 45});

    // North-East Warp to Map 21 (성채 외성 비밀 침투로)
    m_tiles[5 * m_width + 59] = 20; m_collision[5 * m_width + 59] = false; // Rune ward
    m_warps.push_back({59, 5, 21, 55, 55});
}

// -------------------------------------------------------------
// [Map 35] 성채 지하 감옥 & 흑마술 비밀통로 (60 x 60)
// -------------------------------------------------------------
void Tilemap::initMap35_SecretCatacombs() {
    m_mapName = "성채 지하 감옥 & 흑마술 비밀통로";
    m_width = 60;
    m_height = 60;
    m_tiles.assign(m_width * m_height, 3);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    // Prison cell obsidian pillars & Rune Ward Stones (20)
    for (int y = 12; y < 50; y += 12) {
        for (int x = 12; x < 50; x += 12) {
            m_tiles[y * m_width + x] = 20;
            m_collision[y * m_width + x] = true;
        }
    }

    m_chests.push_back({38, 30, 30, "ART_SHADOW_INCENSE", 8000, 5000, false});

    // West Warp to Map 21 (성채 해자)
    m_tiles[30 * m_width + 0] = 2; m_collision[30 * m_width + 0] = false;
    m_warps.push_back({0, 30, 21, 5, 55});

    // North Warp to Map 23 (사신수 결계탑)
    m_tiles[0 * m_width + 30] = 2; m_collision[0 * m_width + 30] = false;
    m_warps.push_back({30, 0, 23, 35, 35});

    // South-East Warp to Map 30 (태고 환상비무 미궁)
    m_tiles[55 * m_width + 59] = 20; m_collision[55 * m_width + 59] = false;
    m_warps.push_back({59, 55, 30, 10, 90});
}

// -------------------------------------------------------------
// [Map 36] 태초의 천상 신역 삼신단 (80 x 80)
// -------------------------------------------------------------
void Tilemap::initMap36_SamshindanApex() {
    m_mapName = "태초의 천상 신역 삼신단 (2차 진엔딩 성역)";
    m_width = 80;
    m_height = 80;
    m_tiles.assign(m_width * m_height, 18); // Floating Cloud Platforms
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    // Golden Jade Hall floor (X=25~55, Y=25~55)
    for (int y = 25; y <= 55; ++y) {
        for (int x = 25; x <= 55; ++x) {
            m_tiles[y * m_width + x] = 3; // Golden Maru
        }
    }

    // Dancheong Banners around sacred apex
    m_tiles[28 * m_width + 28] = 24; m_collision[28 * m_width + 28] = true;
    m_tiles[28 * m_width + 52] = 24; m_collision[28 * m_width + 52] = true;
    m_tiles[52 * m_width + 28] = 24; m_collision[52 * m_width + 28] = true;
    m_tiles[52 * m_width + 52] = 24; m_collision[52 * m_width + 52] = true;

    // Sacred Samshin Sovereign Altar in Center
    m_tiles[40 * m_width + 40] = 9;

    // Supreme Celestial Relic Chest
    m_chests.push_back({39, 40, 28, "ART_TAIJI_BELL", 30000, 15000, false});

    // South Warp to Map 25 (태초의 영맥 심연)
    m_tiles[79 * m_width + 40] = 2; m_collision[79 * m_width + 40] = false;
    m_warps.push_back({40, 79, 25, 25, 6});
}

// -------------------------------------------------------------
// [Map 37] 마을 원로 훈장의 고택 실내 (24 x 18)
// -------------------------------------------------------------
void Tilemap::initMap37_ElderHanokInterior() {
    m_mapName = "도선사 마을 훈장 고택 (실내)";
    m_width = 24;
    m_height = 18;
    m_tiles.assign(m_width * m_height, 3); // Daecheong-maru wooden floor
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    // Ink landscape folding screen (30) on North wall
    for (int x = 6; x <= 17; ++x) {
        m_tiles[2 * m_width + x] = 30;
        m_collision[2 * m_width + x] = true;
    }

    // Tea Table & Porcelain Set (32) in front of screen
    m_tiles[5 * m_width + 11] = 32;
    m_tiles[5 * m_width + 12] = 32;
    m_collision[5 * m_width + 11] = true;
    m_collision[5 * m_width + 12] = true;

    // Ceramic Brazier & Firewood (31) on East side
    m_tiles[6 * m_width + 18] = 31;
    m_collision[6 * m_width + 18] = true;

    // Secret Antique Chest
    m_tiles[3 * m_width + 20] = 10;
    m_chests.push_back({40, 20, 3, "ART_SCHOLAR_BRUSH", 800, 400, false});

    // Southern Exit Door (29: Changhoji Door)
    m_tiles[17 * m_width + 12] = 29;
    m_collision[17 * m_width + 12] = false;
    m_warps.push_back({12, 17, 0, 32, 17}); // Return to Map 0 Village
}

// -------------------------------------------------------------
// [Map 38] 임진나루 도강지대 & 절벽 고저차 (90 x 60)
// -------------------------------------------------------------
void Tilemap::initMap38_ImjinFerryCrossing() {
    m_mapName = "제1구역: 임진나루 도강지대 & 절벽 험로";
    m_width = 90;
    m_height = 60;
    m_tiles.assign(m_width * m_height, 0); // Grass base
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    // 1. Wide Flowing River (X=38~52) dividing East and West
    for (int y = 1; y < m_height - 1; ++y) {
        int riverCenter = 45 + static_cast<int>(3.0 * std::sin(y * 0.12));
        for (int rx = riverCenter - 7; rx <= riverCenter + 7; ++rx) {
            if (rx >= 1 && rx < m_width - 1) {
                m_tiles[y * m_width + rx] = 6;
                m_collision[y * m_width + rx] = true;
            }
        }
    }

    // 2. Partially Collapsed Stone Arch Bridge at Y=10~15 (Broken Bridge Gimmick)
    for (int y = 10; y <= 15; ++y) {
        // East intact side
        for (int x = 50; x <= 56; ++x) {
            m_tiles[y * m_width + x] = 1;
            m_collision[y * m_width + x] = false;
        }
        // West intact side
        for (int x = 34; x <= 39; ++x) {
            m_tiles[y * m_width + x] = 1;
            m_collision[y * m_width + x] = false;
        }
        // Broken gap in middle (X=40~49 remains deep water!)
    }

    // 3. Wooden Ferry Piers & Ferry Boats (나루터 선착장 & 나룻배)
    // East Pier (X=52~55, Y=34~37)
    for (int y = 34; y <= 37; ++y) {
        for (int x = 52; x <= 55; ++x) {
            m_tiles[y * m_width + x] = 27; // Pier Dock
            m_collision[y * m_width + x] = false;
        }
    }
    m_tiles[35 * m_width + 51] = 28; // Ferry Boat moored

    // West Pier (X=36~39, Y=34~37)
    for (int y = 34; y <= 37; ++y) {
        for (int x = 36; x <= 39; ++x) {
            m_tiles[y * m_width + x] = 27;
            m_collision[y * m_width + x] = false;
        }
    }
    m_tiles[35 * m_width + 40] = 28; // Ferry Boat moored

    // 4. Multi-Tier High Cliff Elevation (X=1~30, Y=1~58) on West Bank
    // Tier 1 Cliff Ridge
    for (int y = 5; y <= 55; ++y) {
        m_tiles[y * m_width + 25] = 25; // Rock Cliff Wall
        m_collision[y * m_width + 25] = true;
    }
    // Stone Staircases (26) cutting through cliff at Y=20 and Y=45
    for (int y = 19; y <= 21; ++y) {
        m_tiles[y * m_width + 25] = 26;
        m_collision[y * m_width + 25] = false;
    }
    for (int y = 44; y <= 46; ++y) {
        m_tiles[y * m_width + 25] = 26;
        m_collision[y * m_width + 25] = false;
    }

    // Tier 2 High Summit Mountain Cliff (X=1~15)
    for (int y = 10; y <= 50; ++y) {
        m_tiles[y * m_width + 15] = 25;
        m_collision[y * m_width + 15] = true;
    }
    m_tiles[30 * m_width + 15] = 26; // High Summit Stairs
    m_collision[30 * m_width + 15] = false;

    // Mountain Top Ancient Shrine on High Cliff (X=4~10, Y=26~32)
    for (int y = 26; y <= 32; ++y) {
        for (int x = 4; x <= 10; ++x) {
            m_tiles[y * m_width + x] = (y == 26) ? 4 : 3;
            m_collision[y * m_width + x] = (y == 26 || x == 4 || x == 10 || (y == 32 && x != 7));
        }
    }
    m_tiles[29 * m_width + 7] = 9; // Ancient Spirit Altar inside summit shrine

    // 5. Ferry Tavern & Roadways
    // East Ferry Tavern (X=65~75, Y=30~36)
    for (int y = 30; y <= 36; ++y) {
        for (int x = 65; x <= 75; ++x) {
            m_tiles[y * m_width + x] = (y == 30) ? 15 : 3;
            m_collision[y * m_width + x] = (y == 30 || x == 65 || x == 75 || (y == 36 && x != 70));
        }
    }
    // Road connecting East Pier to East Boundary
    for (int x = 55; x < m_width - 1; ++x) {
        m_tiles[35 * m_width + x] = 2;
    }
    // Road connecting West Pier through cliff stairs
    for (int x = 25; x <= 36; ++x) {
        m_tiles[20 * m_width + x] = 2;
        m_tiles[45 * m_width + x] = 2;
    }
    for (int y = 20; y <= 45; ++y) {
        m_tiles[y * m_width + 36] = 2;
    }

    // Props: Willow groves, Doltap & Totems
    m_tiles[32 * m_width + 58] = 5; m_collision[32 * m_width + 58] = true; // Willow Tree
    m_tiles[38 * m_width + 58] = 5; m_collision[38 * m_width + 58] = true;
    m_tiles[35 * m_width + 60] = 12; m_collision[35 * m_width + 60] = true; // Ferry Jangseung
    m_tiles[35 * m_width + 33] = 13; m_collision[35 * m_width + 33] = true; // Wish Doltap

    // Hidden High Cliff Chest
    m_tiles[28 * m_width + 5] = 10;
    m_chests.push_back({41, 5, 28, "ART_BLACK_TIGER_TALON", 1200, 600, false});

    // Warps
    // East Warp back to Map 0 (도선사 주막마을)
    m_tiles[35 * m_width + 89] = 2; m_collision[35 * m_width + 89] = false;
    m_warps.push_back({89, 35, 0, 2, 28});
}

void Tilemap::render(Renderer& renderer, int cameraX, int cameraY) const {
    int startCol = cameraX / TILE_SIZE;
    int endCol = (cameraX + SCREEN_WIDTH) / TILE_SIZE + 1;
    int startRow = cameraY / TILE_SIZE;
    int endRow = (cameraY + SCREEN_HEIGHT) / TILE_SIZE + 1;

    startCol = std::max(0, startCol);
    endCol = std::min(m_width, endCol);
    startRow = std::max(0, startRow);
    endRow = std::min(m_height, endRow);

    for (int r = startRow; r < endRow; ++r) {
        for (int c = startCol; c < endCol; ++c) {
            int tileId = m_tiles[r * m_width + c];
            int screenX = c * TILE_SIZE - cameraX;
            int screenY = r * TILE_SIZE - cameraY;
            renderer.drawTileProcedural(screenX, screenY, tileId);
        }
    }
}

bool Tilemap::isSolid(int tileX, int tileY) const {
    if (tileX < 0 || tileX >= m_width || tileY < 0 || tileY >= m_height) return true;
    return m_collision[tileY * m_width + tileX];
}

const WarpTrigger* Tilemap::checkWarp(int tileX, int tileY) const {
    for (const auto& w : m_warps) {
        if (w.fromX == tileX && w.fromY == tileY) {
            return &w;
        }
    }
    return nullptr;
}

MapChest* Tilemap::checkChest(int tileX, int tileY) {
    for (auto& c : m_chests) {
        if (c.x == tileX && c.y == tileY && !c.opened) {
            return &c;
        }
    }
    return nullptr;
}

const MapChest* Tilemap::checkChest(int tileX, int tileY) const {
    for (const auto& c : m_chests) {
        if (c.x == tileX && c.y == tileY && !c.opened) {
            return &c;
        }
    }
    return nullptr;
}

} // namespace JoseonRPG
