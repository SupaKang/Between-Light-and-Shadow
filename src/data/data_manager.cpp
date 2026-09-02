#include "data_manager.hpp"

namespace JoseonRPG {

std::vector<Yokai> DataManager::s_yokaiDatabase;
std::vector<Skill> DataManager::s_skillDatabase;
std::vector<Artifact> DataManager::s_artifactDatabase;
std::vector<NPC> DataManager::s_npcDatabase;
Encyclopedia DataManager::s_encyclopedia;
QuestManager DataManager::s_questManager;

void DataManager::init() {
    // 1. Skills Database
    Skill clubStrike{"SKL_CLUB_STRIKE", "Club Strike", 45, 10, Element::Physical, 95, StatusEffect::None, 0};
    Skill goblinFire{"SKL_GOBLIN_FIRE", "Goblin Fire", 55, 18, Element::Fire, 90, StatusEffect::Burn, 35};
    Skill taunt{"SKL_TAUNT", "Taunt Grin", 20, 12, Element::Dark, 100, StatusEffect::Fear, 60};
    Skill earthSlam{"SKL_EARTH_SLAM", "Earth Slam", 60, 25, Element::Earth, 85, StatusEffect::None, 0};

    Skill foxFire{"SKL_FOX_FIRE", "Fox Fire", 65, 22, Element::Fire, 90, StatusEffect::Burn, 40};
    Skill fearGaze{"SKL_FEAR_GAZE", "Fearful Gaze", 30, 15, Element::Dark, 100, StatusEffect::Fear, 80};
    Skill spiritDrain{"SKL_SPIRIT_DRAIN", "Spirit Drain", 40, 20, Element::Dark, 95, StatusEffect::None, 0};
    Skill illusionBurst{"SKL_ILLUSION_BURST", "Illusion Burst", 75, 35, Element::Light, 85, StatusEffect::None, 0};

    Skill chillWail{"SKL_CHILL_WAIL", "Chill Wail", 50, 20, Element::Water, 85, StatusEffect::Freeze, 35};
    Skill talismanSeal{"SKL_TALISMAN_SEAL", "Talisman Seal", 25, 25, Element::Light, 95, StatusEffect::Seal, 100};
    Skill thunderBolt{"SKL_THUNDER_BOLT", "Thunder Bolt", 70, 30, Element::Physical, 85, StatusEffect::Paralysis, 45};
    Skill ironCharge{"SKL_IRON_CHARGE", "Iron Charge", 55, 15, Element::Earth, 90, StatusEffect::None, 0};
    Skill flameBreath{"SKL_FLAME_BREATH", "Flame Breath", 60, 24, Element::Fire, 90, StatusEffect::Burn, 50};

    Skill deathDecree{"SKL_DEATH_DECREE", "Death Decree", 65, 28, Element::Dark, 90, StatusEffect::Fear, 60};
    Skill holyPurge{"SKL_HOLY_PURGE", "Holy Purge", 70, 32, Element::Light, 90, StatusEffect::Seal, 75};

    s_skillDatabase = {
        clubStrike, goblinFire, taunt, earthSlam,
        foxFire, fearGaze, spiritDrain, illusionBurst,
        chillWail, talismanSeal, thunderBolt, ironCharge, flameBreath,
        deathDecree, holyPurge
    };

    // 2. Yokai Templates (Core Folklore Representative Samples)
    // #001: Dokkaebi
    Yokai dokkaebi(1, "YOKAI_001", "Dokkaebi", YokaiGrade::Grade2, Element::Fire, {95, 95, 60, 60, 22, 18, 14},
                  "KOREAN_FOLKLORE", "방망이와 씨름을 좋아하는 익살맞은 한국의 대표 요괴.");
    dokkaebi.addSkill(clubStrike);
    dokkaebi.addSkill(goblinFire);
    dokkaebi.addSkill(taunt);
    dokkaebi.addSkill(earthSlam);

    // #002: Gumiho
    Yokai gumiho(2, "YOKAI_002", "Gumiho", YokaiGrade::Grade4, Element::Fire, {110, 110, 100, 100, 28, 16, 26},
                "KOREAN_FOLKLORE", "천 년 묵은 아홉 꼬리 여우. 매혹과 여우불을 다룬다.");
    gumiho.addSkill(foxFire);
    gumiho.addSkill(fearGaze);
    gumiho.addSkill(spiritDrain);
    gumiho.addSkill(illusionBurst);

    // #003: Bulgasari
    Yokai bulgasari(3, "YOKAI_003", "Bulgasari", YokaiGrade::Grade3, Element::Earth, {140, 140, 50, 50, 24, 28, 10},
                   "KOREAN_FOLKLORE", "쇠를 먹고 자라나는 전설의 괴수. 불로만 제어 가능.");
    bulgasari.addSkill(ironCharge);
    bulgasari.addSkill(flameBreath);
    bulgasari.addSkill(clubStrike);
    bulgasari.addSkill(earthSlam);

    // #004: Geuseundae
    Yokai geuseundae(4, "YOKAI_004", "Geuseundae", YokaiGrade::Grade3, Element::Dark, {120, 120, 70, 70, 25, 19, 15},
                    "KOREAN_FOLKLORE", "쳐다볼수록 키가 거대해져 상대를 짓누르는 어둠의 요괴.");
    geuseundae.addSkill(fearGaze);
    geuseundae.addSkill(clubStrike);
    geuseundae.addSkill(earthSlam);
    geuseundae.addSkill(spiritDrain);

    // #005: Cheonyeogwishin (Maiden Ghost)
    Yokai maidenGhost(5, "YOKAI_005", "Cheonyeogwishin", YokaiGrade::Grade2, Element::Water, {85, 85, 80, 80, 20, 14, 22},
                     "KOREAN_FOLKLORE", "소복을 입고 한을 품은 처녀 귀신. 빙결 한기를 뿜는다.");
    maidenGhost.addSkill(chillWail);
    maidenGhost.addSkill(fearGaze);
    maidenGhost.addSkill(talismanSeal);
    maidenGhost.addSkill(spiritDrain);

    // #006: Samdugumijo
    Yokai samdu(6, "YOKAI_006", "Samdugumijo", YokaiGrade::Grade4, Element::Light, {105, 105, 90, 90, 26, 17, 25},
               "KOREAN_FOLKLORE", "삼재를 쫓는 세 머리와 아홉 꼬리를 가진 신령한 새.");
    samdu.addSkill(holyPurge);
    samdu.addSkill(talismanSeal);
    samdu.addSkill(thunderBolt);
    samdu.addSkill(illusionBurst);

    // #007: Baektaek (Baize)
    Yokai baize(7, "YOKAI_007", "Baize", YokaiGrade::Grade5, Element::Light, {160, 160, 120, 120, 32, 30, 25},
               "EAST_ASIAN_FOLKLORE", "만물의 요괴와 신령의 이름을 아는 전설의 영수.");
    baize.addSkill(holyPurge);
    baize.addSkill(thunderBolt);
    baize.addSkill(illusionBurst);
    baize.addSkill(talismanSeal);

    // #008: Nue
    Yokai nue(8, "YOKAI_008", "Thunder Nue", YokaiGrade::Grade3, Element::Dark, {105, 105, 75, 75, 26, 17, 21},
             "JAPANESE_FOLKLORE", "원숭이 머리, 호랑이 몸통, 뱀 꼬리를 한 흉조.");
    nue.addSkill(thunderBolt);
    nue.addSkill(fearGaze);
    nue.addSkill(clubStrike);
    nue.addSkill(talismanSeal);

    // #009: Jeoseungsaja
    Yokai reaper(9, "YOKAI_009", "Jeoseungsaja", YokaiGrade::Grade4, Element::Dark, {115, 115, 95, 95, 29, 20, 23},
                 "KOREAN_FOLKLORE", "검은 갓과 도포를 입고 망자를 인도하는 저승의 차사.");
    reaper.addSkill(deathDecree);
    reaper.addSkill(talismanSeal);
    reaper.addSkill(fearGaze);
    reaper.addSkill(spiritDrain);

    // #010: Eoduksini
    Yokai eoduk(10, "YOKAI_010", "Eoduksini", YokaiGrade::Grade2, Element::Dark, {90, 90, 60, 60, 21, 15, 16},
               "KOREAN_FOLKLORE", "사람의 두려움을 먹고 커지는 어둠의 그림자 도깨비.");
    eoduk.addSkill(fearGaze);
    eoduk.addSkill(taunt);
    eoduk.addSkill(clubStrike);
    eoduk.addSkill(spiritDrain);

    // Chapter 1 Boss: Red Berserk Dokkaebi
    Yokai bossDokkaebi(1, "YOKAI_BOSS_01", "폭주한 붉은 도깨비", YokaiGrade::Grade3, Element::Fire, {160, 160, 90, 90, 32, 22, 18},
                       "BOSS", "음양당 괴승 묘각의 주술로 흑화하여 거대화된 붉은 도깨비.");
    bossDokkaebi.addSkill(goblinFire);
    bossDokkaebi.addSkill(clubStrike);
    bossDokkaebi.addSkill(earthSlam);
    bossDokkaebi.addSkill(fearGaze);

    s_yokaiDatabase = {
        dokkaebi, gumiho, bulgasari, geuseundae, maidenGhost,
        samdu, baize, nue, reaper, eoduk, bossDokkaebi
    };

    // 3. Register All 108 Slots in Encyclopedia
    for (const auto& y : s_yokaiDatabase) {
        if (y.getId() == "YOKAI_BOSS_01") continue;
        s_encyclopedia.registerTemplate(y.getNumber(), y.getId(), y.getName(), y.getName(),
                                        y.getGrade(), y.getElement(), y.getOrigin(), y.getLore());
    }

    for (int i = 11; i <= 108; ++i) {
        std::string numStr = (i < 100) ? ((i < 10) ? "00" : "0") + std::to_string(i) : std::to_string(i);
        std::string slotId = "YOKAI_" + numStr;
        std::string name = "미확인 요괴 #" + numStr;
        s_encyclopedia.registerTemplate(i, slotId, name, "Unknown Yokai", YokaiGrade::Grade1, Element::Physical, "UNKNOWN", "아직 기록되지 않은 조선의 미확인 요괴.");
    }

    s_encyclopedia.markCaptured("YOKAI_001");
    s_encyclopedia.markCaptured("YOKAI_002");
    s_encyclopedia.markCaptured("YOKAI_003");
    s_encyclopedia.markSeen("YOKAI_005");

    // 4. Artifacts Database
    Artifact dokkaebiHat{"ART_DOKKAEBI_HAT", "도깨비 감투", ArtifactBuffType::CritRateBoost, 30, ArtifactDebuffType::QiDrainPerTurn, 5, "쓰면 투명해지나 영혼의 기운을 갉아먹는 도깨비 털모자."};
    Artifact centipedeEgg{"ART_CENTIPEDE_EGG", "백년 묵은 지네의 알", ArtifactBuffType::ImmunityBurn, 1, ArtifactDebuffType::MaxHpReduction, 20, "독기를 막아주나 소지자의 기혈을 쇠약하게 만든다."};
    Artifact foxShard{"ART_FOX_MARBLE_SHARD", "깨진 여우구슬", ArtifactBuffType::MagicAtkBoost, 40, ArtifactDebuffType::DefReduction, 35, "엄청난 영술의 힘을 주지만 육신을 무방비로 만든다."};
    Artifact demonTile{"ART_DEMON_TILE", "귀면와 (도깨비 기와)", ArtifactBuffType::DamageReduction, 20, ArtifactDebuffType::SpdReduction, 15, "잡귀의 타격을 튕겨내지만 무거워서 신법이 둔해진다."};
    Artifact jadeMirror{"ART_JADE_MIRROR", "벽사 옥경 (玉鏡)", ArtifactBuffType::ImmunityFreezeFear, 1, ArtifactDebuffType::HpDrainPerTurn, 3, "빙결과 공포를 막아주나 매 턴 생명력을 소모한다."};
    Artifact goldenBell{"ART_GOLDEN_BELL", "신령 무당방울", ArtifactBuffType::CaptureRateBoost, 15, ArtifactDebuffType::ExpPenalty, 20, "요괴의 넋을 홀려 계약을 쉽게 하나 수련 경험치가 감소한다."};
    Artifact shadowIncense{"ART_SHADOW_INCENSE", "음기도방의 향로", ArtifactBuffType::StatusChanceBoost, 50, ArtifactDebuffType::DefReduction, 15, "상태이상을 쉽게 걸리게 하지만 방어력을 떨어뜨린다."};
    Artifact reaperScroll{"ART_REAPER_SCROLL", "저승사자의 명부 조각", ArtifactBuffType::MagicAtkBoost, 50, ArtifactDebuffType::QiDrainPerTurn, 10, "망자의 힘을 빌려 절대적인 위력을 내나 영력을 크게 소모한다."};
    
    s_artifactDatabase = {
        dokkaebiHat, centipedeEgg, foxShard, demonTile,
        jadeMirror, goldenBell, shadowIncense, reaperScroll
    };

    // 5. NPCs Database
    NPC jumoh{
        "NPC_001", 0, 3, 3, 2, "주모 월선", "도선사 주막 주모",
        {
            "어서 오시게! 북한산 꼭대기 도선사에 수상한 음양당 놈들이",
            "들어앉더니 온 산천에 흉흉한 요기가 돌고 있다오.",
            "따뜻한 장터국밥 한 그릇 말아드릴 테니 기운 차리시게!"
        },
        NPCActionType::TavernRest, "MQ_001"
    };

    NPC merchant{
        "NPC_002", 0, 14, 3, 3, "벽사 상인 박 서방", "관상감 공인 부적 상인",
        {
            "관상감에서 내려온 영술사님이시군요!",
            "음양당 놈들이 부리는 요괴들은 벽사 부적으로 계약할 수 있습니다.",
            "주막 아래쪽 길로 나가면 북한산 고갯길로 이어집니다."
        },
        NPCActionType::TalismanShop, ""
    };

    NPC bride{
        "NPC_003", 0, 11, 7, 2, "방귀쟁이 며느리", "근심 가득한 며느리",
        {
            "흑흑... 제 몸에 깃든 바람 요괴 때문에",
            "시아버지 앞에서 큰 방귀 소리를 내어 쫓겨날 판입니다.",
            "고갯길의 바람 요괴를 진정시켜 주실 수 있을까요?"
        },
        NPCActionType::QuestTrigger, "SQ_001"
    };

    NPC bossMyogak{
        "NPC_004", 2, 9, 3, 4, "괴승 묘각", "음양당 북악방주",
        {
            "크크크... 관상감의 애송이 영술사가 여기까지 기어들어왔구나!",
            "천지음양부는 이미 깨어졌고, 조선의 108 요괴는 우리 음양당의 손에 들어갈 것이다!",
            "폭주한 붉은 도깨비여, 저 놈을 찢어발겨라!"
        },
        NPCActionType::BossEncounter, "MQ_001"
    };

    s_npcDatabase = {jumoh, merchant, bride, bossMyogak};

    // 6. Quests Database
    Quest mainQuest1{
        "MQ_001", QuestType::Main, "벽사의 부름: 도선사의 요기", 1,
        "관상감 벽사청의 명을 받아 도선사 인근에 번지는 음양당의 사악한 기운을 조사하라.",
        {
            "도선사 주막 주모와 대화하여 정보 수집",
            "북한산 고갯길을 지나 도선사 대웅전 진입",
            "음양당 하수인 괴승 묘각 격파"
        },
        0, {500, 300, "ART_DOKKAEBI_HAT"}, QuestState::InProgress
    };

    Quest sideQuest1{
        "SQ_001", QuestType::Side, "방귀쟁이 며느리의 하소연", 0,
        "너무 강한 영적 바람(방귀)으로 인해 쫓겨날 위기에 처한 며느리를 위해 바람의 영수를 진정시켜라.",
        {
            "주막 뒤뜰 며느리와 대화 후 바람 요괴 수색",
            "바람 요괴와 전투 후 계약 완료"
        },
        0, {250, 150, "ART_DEMON_TILE"}, QuestState::NotStarted
    };

    s_questManager.registerQuest(mainQuest1);
    s_questManager.registerQuest(sideQuest1);
}

const std::vector<Yokai>& DataManager::getAllYokaiTemplates() {
    return s_yokaiDatabase;
}

const std::vector<Skill>& DataManager::getAllSkills() {
    return s_skillDatabase;
}

const std::vector<Artifact>& DataManager::getAllArtifacts() {
    return s_artifactDatabase;
}

const std::vector<NPC>& DataManager::getAllNPCs() {
    return s_npcDatabase;
}

std::vector<NPC> DataManager::getNPCsForMap(int mapId) {
    std::vector<NPC> list;
    for (const auto& n : s_npcDatabase) {
        if (n.mapId == mapId) {
            list.push_back(n);
        }
    }
    return list;
}

Yokai DataManager::createYokaiById(std::string_view id) {
    for (const auto& y : s_yokaiDatabase) {
        if (y.getId() == id) return y;
    }
    return s_yokaiDatabase.empty() ? Yokai{} : s_yokaiDatabase[0];
}

Yokai DataManager::createYokaiByNumber(int number) {
    for (const auto& y : s_yokaiDatabase) {
        if (y.getNumber() == number) return y;
    }
    return s_yokaiDatabase.empty() ? Yokai{} : s_yokaiDatabase[0];
}

Artifact DataManager::createArtifactById(std::string_view id) {
    for (const auto& a : s_artifactDatabase) {
        if (a.id == id) return a;
    }
    return s_artifactDatabase.empty() ? Artifact{} : s_artifactDatabase[0];
}

} // namespace JoseonRPG
