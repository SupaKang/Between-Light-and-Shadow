#pragma once
#include "../core/types.hpp"
#include <vector>
#include <string>

namespace JoseonRPG {

class Renderer;

struct WarpTrigger {
    int fromX = 0;
    int fromY = 0;
    int targetMapId = 0;
    int targetX = 0;
    int targetY = 0;
};

struct MapChest {
    int id = 0;
    int x = 0;
    int y = 0;
    std::string artifactId;
    int money = 0;
    int exp = 0;
    bool opened = false;
};

class Tilemap {
public:
    Tilemap();

    void loadMap(int mapId);
    void render(Renderer& renderer, int cameraX, int cameraY) const;
    bool isSolid(int tileX, int tileY) const;
    const WarpTrigger* checkWarp(int tileX, int tileY) const;
    MapChest* checkChest(int tileX, int tileY);
    const MapChest* checkChest(int tileX, int tileY) const;

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getMapId() const { return m_mapId; }
    const std::string& getMapName() const { return m_mapName; }
    const std::vector<MapChest>& getChests() const { return m_chests; }

    static int getTotalMapCount() { return 36; }

private:
    // Chapter 1: 한양 북악산 & 도선사 (6 Maps)
    void initMap0_Village();             // 80x60 마을 외경
    void initMap1_TavernInterior();      // 24x18 주막 실내
    void initMap2_ExorcistBureau();      // 24x18 관상감 벽사청 임시 지부
    void initMap3_MountainPass();        // 40x120 북한산 대협곡 종단 도로
    void initMap4_MountainCave();        // 36x36 북한산 고대 석굴
    void initMap5_BossTemple();          // 48x36 도선사 대웅전 (Ch.1 괴승 묘각)

    // Chapter 2: 소백산맥 & 죽령 험로 (5 Maps)
    void initMap6_JungryeongPass();      // 80x60 죽령 옛길 초입
    void initMap7_SobaekCanyon();        // 100x50 소백산맥 대협곡길
    void initMap8_SimmaniHut();          // 20x16 심마니 오두막 실내
    void initMap9_IronMineUpper();       // 50x50 무쇠광산 상층 갱도
    void initMap10_IronMineDeep();       // 50x50 무쇠광산 최심부 (Ch.2 철포방주 배극)

    // Chapter 3: 남해안 & 다도해 (5 Maps)
    void initMap11_NamhaeReeds();        // 60x60 남해안 갈대밭 샛길
    void initMap12_NamhaePort();         // 80x50 남해안 갯벌 포구
    void initMap13_HaenyeoShelter();     // 24x18 해녀의 쉼터 실내
    void initMap14_GhostShipUpper();     // 40x30 남해 유령 난파선 갑판
    void initMap15_GhostShipDeep();      // 40x30 남해 유령 난파선 선장실 (Ch.3 수로방주 흑사)

    // Chapter 4: 지리산 & 천왕봉 (5 Maps)
    void initMap16_JirisanEntry();       // 60x60 지리산 안개 숲길 초입
    void initMap17_JirisanForest();      // 80x80 지리산 대나무 밀림 미궁
    void initMap18_DosaHermitage();      // 20x16 백발 도사의 산중 암자
    void initMap19_FoxValleyForest();    // 60x60 여우골 상층 숲
    void initMap20_FoxGrottoDungeon();   // 50x50 여우골 영맥동굴 (Ch.4 좌호법 설화)

    // Chapter 5: 일식의 성채 & 음양당 본거지 (5 Maps)
    void initMap21_FortressMoat();       // 60x60 일식의 성채 외성 & 해자
    void initMap22_FortressCorridor();   // 60x60 성채 내성 흑요석 회랑
    void initMap23_GuardianTower();      // 40x40 음양당 사신수 결계탑
    void initMap24_FinalSanctum();       // 50x50 천문음양전 중심 제단 (최종보스 당주 묵영)
    void initMap25_OriginAbyss();        // 50x50 태초의 영맥 심연 (진엔딩 신역)

    // Expanded Endgame & Secret Mythic Regions (5 Maps)
    void initMap26_HanyangBoulevard();   // 90x60 한양 육조거리 & 경복궁 근정전 외곽
    void initMap27_Geumgangsan();        // 80x70 금강산 일만이천봉 선계
    void initMap28_HeuksanDragonPalace();// 70x70 흑산도 심해 용궁
    void initMap29_BaekdusanSanctuary(); // 80x80 백두산 천지 백록담 신역
    void initMap30_PantheonLabyrinth();  // 100x100 태고의 환상비무 미궁 (보스러시)

    // Dynamic Connecting Shortcuts & Secret Loop Passages (5 Maps)
    void initMap31_SubterraneanAqueduct(); // 60x60 지하 암반 수로 (도선사-북한산-남해안 연결)
    void initMap32_BeaconRidge();          // 70x70 소백-지리 봉화대 능선 (소백산-지리산-금강산 연결)
    void initMap33_SunkenGrotto();         // 50x50 난파선 침수 밀실 (난파선-용궁 연결)
    void initMap34_FoxLeylinePass();       // 60x60 여우골 영맥 환상 샛길 (지리산-성채 비밀 침투로)
    void initMap35_SecretCatacombs();      // 60x60 성채 지하 감옥 (성채-결계탑-미궁 비밀통로)

    int m_mapId = 0;
    std::string m_mapName;
    int m_width = 80;
    int m_height = 60;
    std::vector<int> m_tiles;
    std::vector<bool> m_collision;
    std::vector<WarpTrigger> m_warps;
    std::vector<MapChest> m_chests;
};

} // namespace JoseonRPG
