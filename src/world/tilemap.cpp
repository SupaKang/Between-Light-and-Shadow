#include "tilemap.hpp"
#include "../core/renderer.hpp"
#include <algorithm>

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
// [Map 0] 도선사 주막마을 (80 x 60)
// -------------------------------------------------------------
void Tilemap::initMap0_Village() {
    m_mapName = "제1구역: 도선사 주막마을";
    m_width = 80;
    m_height = 60;
    m_tiles.assign(m_width * m_height, 0);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision);

    // Stream on X=55 with bridge at Y=30
    for (int y = 1; y < m_height - 1; ++y) {
        m_tiles[y * m_width + 55] = 6; m_collision[y * m_width + 55] = true;
    }
    m_tiles[30 * m_width + 55] = 7; m_collision[30 * m_width + 55] = false;

    // Roads
    for (int x = 2; x < m_width - 1; ++x) {
        if (x != 55) m_tiles[30 * m_width + x] = 2;
    }
    for (int y = 2; y < m_height - 1; ++y) {
        m_tiles[y * m_width + 20] = 2;
        m_tiles[y * m_width + 40] = 2;
        m_tiles[y * m_width + 68] = 2;
    }

    // Tavern Building (X=10~18, Y=12~18)
    for (int y = 12; y <= 18; ++y) {
        for (int x = 10; x <= 18; ++x) {
            m_tiles[y * m_width + x] = (y == 12) ? 4 : 3;
            m_collision[y * m_width + x] = (y == 12 || (x == 10 && y < 18) || (x == 18 && y < 18));
        }
    }
    m_tiles[18 * m_width + 14] = 3;

    // Exorcist Bureau Building (X=24~32, Y=12~18)
    for (int y = 12; y <= 18; ++y) {
        for (int x = 24; x <= 32; ++x) {
            m_tiles[y * m_width + x] = (y == 12) ? 4 : 3;
            m_collision[y * m_width + x] = (y == 12 || (x == 24 && y < 18) || (x == 32 && y < 18));
        }
    }
    m_tiles[18 * m_width + 28] = 3;

    // Dangsan Tree
    m_tiles[15 * m_width + 68] = 5; m_collision[15 * m_width + 68] = true;

    // Chests
    m_tiles[8 * m_width + 10] = 10;
    m_chests.push_back({1, 10, 8, "", 200, 100, false});

    m_tiles[12 * m_width + 65] = 10;
    m_chests.push_back({2, 65, 12, "", 300, 150, false});

    // Warps
    m_warps.push_back({14, 18, 1, 12, 16}); // To Tavern Interior (Map 1)
    m_warps.push_back({28, 18, 2, 12, 16}); // To Exorcist Bureau (Map 2)

    m_tiles[30 * m_width + 78] = 2; m_collision[30 * m_width + 78] = false;
    m_tiles[30 * m_width + 79] = 2; m_collision[30 * m_width + 79] = false;
    m_warps.push_back({79, 30, 3, 20, 2});  // To Mountain Pass (Map 3)
    m_warps.push_back({78, 30, 3, 20, 2});

    m_tiles[59 * m_width + 75] = 2; m_collision[59 * m_width + 75] = false;
    m_warps.push_back({75, 59, 26, 45, 58}); // To Hanyang Boulevard (Map 26)
}

// -------------------------------------------------------------
// [Map 1] 주막 본채 실내 (24 x 18)
// -------------------------------------------------------------
void Tilemap::initMap1_TavernInterior() {
    m_mapName = "도선사 주막 본채 (실내)";
    m_width = 24;
    m_height = 18;
    m_tiles.assign(m_width * m_height, 3);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    for (int y = 2; y <= 8; ++y) { m_tiles[y * m_width + 8] = 1; m_collision[y * m_width + 8] = true; }
    for (int x = 2; x <= 8; ++x) { m_tiles[8 * m_width + x] = 1; m_collision[8 * m_width + x] = true; }
    m_tiles[8 * m_width + 5] = 3; m_collision[8 * m_width + 5] = false;

    m_tiles[17 * m_width + 12] = 2; m_collision[17 * m_width + 12] = false;
    m_warps.push_back({12, 17, 0, 14, 19});
}

// -------------------------------------------------------------
// [Map 2] 관상감 벽사청 북악출장소 (24 x 18)
// -------------------------------------------------------------
void Tilemap::initMap2_ExorcistBureau() {
    m_mapName = "관상감 벽사청 북악출장소 (실내)";
    m_width = 24;
    m_height = 18;
    m_tiles.assign(m_width * m_height, 3);
    m_collision.assign(m_width * m_height, false);

    makeBoundaryWalls(m_width, m_height, m_tiles, m_collision, 1);

    m_tiles[6 * m_width + 12] = 9;  // Altar
    m_tiles[6 * m_width + 18] = 10; // Chest
    m_chests.push_back({3, 18, 6, "ART_HEONGSAL_WOOD", 250, 200, false});

    m_tiles[17 * m_width + 12] = 2; m_collision[17 * m_width + 12] = false;
    m_warps.push_back({12, 17, 0, 28, 19});
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
            m_tiles[y * m_width + x] = 1; m_collision[y * m_width + x] = true;
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
    for (int y = 2; y < 58; ++y) { m_tiles[y * m_width + 30] = 2; }

    m_tiles[15 * m_width + 12] = 10;
    m_chests.push_back({13, 12, 15, "ART_YONGWANG_PEARL", 750, 500, false});

    m_tiles[1 * m_width + 30] = 2; m_collision[1 * m_width + 30] = false;
    m_warps.push_back({30, 1, 7, 85, 48});

    m_tiles[59 * m_width + 30] = 2; m_collision[59 * m_width + 30] = false;
    m_warps.push_back({30, 59, 12, 40, 2}); // To Namhae Port (Map 12)
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

    // Haenyeo Shelter (Map 13) at (50, 18)
    m_tiles[18 * m_width + 50] = 4;
    m_warps.push_back({50, 18, 13, 12, 16});

    // Ghost Ship (Map 14) at (20, 38)
    m_tiles[38 * m_width + 20] = 9;
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

    // Royal Government Offices & Stone Pavilions
    for (int y = 10; y <= 50; y += 15) {
        for (int x = 10; x <= 30; x += 10) {
            m_tiles[y * m_width + x] = 7;
            m_collision[y * m_width + x] = true;
        }
        for (int x = 60; x <= 80; x += 10) {
            m_tiles[y * m_width + x] = 7;
            m_collision[y * m_width + x] = true;
        }
    }

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

    // Mountain rock clusters
    for (int y = 10; y < 60; y += 12) {
        for (int x = 15; x < 70; x += 14) {
            m_tiles[y * m_width + x] = 1;
            m_collision[y * m_width + x] = true;
        }
    }

    // Hermit shrine
    m_tiles[20 * m_width + 40] = 9;
    m_chests.push_back({30, 42, 20, "ART_HERB_GOURD", 4000, 2500, false});

    // Warp back to Hanyang (Map 26)
    m_tiles[69 * m_width + 40] = 2; m_collision[69 * m_width + 40] = false;
    m_warps.push_back({40, 69, 26, 45, 2});

    // Warp to Heuksan Dragon Palace (Map 28)
    m_tiles[5 * m_width + 75] = 9;
    m_warps.push_back({75, 5, 28, 35, 68});
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

    // Coral columns
    for (int y = 15; y < 60; y += 15) {
        for (int x = 15; x < 60; x += 15) {
            m_tiles[y * m_width + x] = 9;
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

    // Sacred Caldera Lake in center
    for (int y = 25; y <= 55; ++y) {
        for (int x = 25; x <= 55; ++x) {
            m_tiles[y * m_width + x] = 4;
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

    // Labyrinth stone pillars
    for (int y = 10; y < 90; y += 10) {
        for (int x = 10; x < 90; x += 10) {
            if (x == 50 && y == 50) continue; // Keep center arena open
            m_tiles[y * m_width + x] = 9;
            m_collision[y * m_width + x] = true;
        }
    }

    // Ultimate Celestial Chest
    m_chests.push_back({33, 50, 10, "ART_TAIJI_BELL", 20000, 10000, false});

    // Warp back to Baekdusan (Map 29)
    m_tiles[99 * m_width + 50] = 2; m_collision[99 * m_width + 50] = false;
    m_warps.push_back({50, 99, 29, 40, 6});
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
