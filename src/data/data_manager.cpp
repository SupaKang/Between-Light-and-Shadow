#include "data_manager.hpp"

namespace JoseonRPG {

std::vector<Yokai> DataManager::s_yokaiDatabase;
std::vector<Skill> DataManager::s_skillDatabase;
std::vector<Artifact> DataManager::s_artifactDatabase;
std::vector<NPC> DataManager::s_npcDatabase;
Encyclopedia DataManager::s_encyclopedia;
QuestManager DataManager::s_questManager;

void DataManager::init() {
    // 1. Skills Database (Comprehensive 20+ Skills Across 7 Elements)
    Skill clubStrike{"SKL_CLUB_STRIKE", "Club Strike", 45, 10, Element::Physical, 95, StatusEffect::None, 0};
    Skill ironCharge{"SKL_IRON_CHARGE", "Iron Charge", 55, 15, Element::Earth, 90, StatusEffect::None, 0};
    Skill earthSlam{"SKL_EARTH_SLAM", "Earth Slam", 65, 25, Element::Earth, 85, StatusEffect::None, 0};
    
    Skill goblinFire{"SKL_GOBLIN_FIRE", "Goblin Fire", 55, 18, Element::Fire, 90, StatusEffect::Burn, 35};
    Skill foxFire{"SKL_FOX_FIRE", "Fox Fire", 65, 22, Element::Fire, 90, StatusEffect::Burn, 45};
    Skill flameBreath{"SKL_FLAME_BREATH", "Flame Breath", 75, 30, Element::Fire, 85, StatusEffect::Burn, 60};

    Skill chillWail{"SKL_CHILL_WAIL", "Chill Wail", 50, 20, Element::Water, 85, StatusEffect::Freeze, 40};
    Skill aquaTide{"SKL_AQUA_TIDE", "Aqua Tide", 60, 24, Element::Water, 90, StatusEffect::None, 0};
    Skill absoluteZero{"SKL_ABSOLUTE_ZERO", "Absolute Zero", 80, 40, Element::Water, 80, StatusEffect::Freeze, 70};

    Skill thunderBolt{"SKL_THUNDER_BOLT", "Thunder Bolt", 70, 30, Element::Physical, 85, StatusEffect::Paralysis, 50};
    Skill stormStrike{"SKL_STORM_STRIKE", "Storm Strike", 55, 18, Element::Physical, 95, StatusEffect::Paralysis, 30};

    Skill talismanSeal{"SKL_TALISMAN_SEAL", "Talisman Seal", 25, 25, Element::Light, 95, StatusEffect::Seal, 100};
    Skill holyPurge{"SKL_HOLY_PURGE", "Holy Purge", 75, 35, Element::Light, 90, StatusEffect::Seal, 60};
    Skill divineSanction{"SKL_DIVINE_SANCTION", "Divine Sanction", 90, 50, Element::Light, 85, StatusEffect::Seal, 80};

    Skill taunt{"SKL_TAUNT", "Taunt Grin", 20, 12, Element::Dark, 100, StatusEffect::Fear, 60};
    Skill fearGaze{"SKL_FEAR_GAZE", "Fearful Gaze", 35, 18, Element::Dark, 95, StatusEffect::Fear, 80};
    Skill spiritDrain{"SKL_SPIRIT_DRAIN", "Spirit Drain", 50, 22, Element::Dark, 95, StatusEffect::None, 0};
    Skill illusionBurst{"SKL_ILLUSION_BURST", "Illusion Burst", 80, 38, Element::Light, 85, StatusEffect::None, 0};
    Skill deathDecree{"SKL_DEATH_DECREE", "Death Decree", 75, 32, Element::Dark, 90, StatusEffect::Fear, 70};
    Skill voidCollapse{"SKL_VOID_COLLAPSE", "Void Collapse", 100, 60, Element::Dark, 85, StatusEffect::Fear, 90};

    s_skillDatabase = {
        clubStrike, ironCharge, earthSlam, goblinFire, foxFire, flameBreath,
        chillWail, aquaTide, absoluteZero, thunderBolt, stormStrike,
        talismanSeal, holyPurge, divineSanction, taunt, fearGaze,
        spiritDrain, illusionBurst, deathDecree, voidCollapse
    };

    // 2. 108 Official Folklore Yokai Database
    struct YokaiMeta {
        int num;
        const char* id;
        const char* nameKo;
        const char* nameEn;
        YokaiGrade grade;
        Element elem;
        Stats stats;
        const char* origin;
        const char* lore;
    };

    static const YokaiMeta k108Roster[108] = {
        {1, "YOKAI_001", "도깨비", "Dokkaebi", YokaiGrade::Grade2, Element::Fire, {95, 95, 60, 60, 22, 18, 14}, "KOREAN_FOLKLORE", "방망이와 씨름을 좋아하는 익살맞은 한국의 대표 요괴."},
        {2, "YOKAI_002", "구미호", "Gumiho", YokaiGrade::Grade4, Element::Fire, {110, 110, 100, 100, 28, 16, 26}, "KOREAN_FOLKLORE", "천 년 묵은 아홉 꼬리 여우. 매혹과 여우불을 다룬다."},
        {3, "YOKAI_003", "불가사리", "Bulgasari", YokaiGrade::Grade3, Element::Earth, {140, 140, 50, 50, 24, 28, 10}, "KOREAN_FOLKLORE", "쇠를 먹고 자라나는 전설의 괴수. 불로만 제어 가능."},
        {4, "YOKAI_004", "그슨대", "Geuseundae", YokaiGrade::Grade3, Element::Dark, {120, 120, 70, 70, 25, 19, 15}, "KOREAN_FOLKLORE", "쳐다볼수록 키가 거대해져 상대를 짓누르는 어둠의 요괴."},
        {5, "YOKAI_005", "처녀귀신", "Cheonyeogwishin", YokaiGrade::Grade2, Element::Water, {85, 85, 80, 80, 20, 14, 22}, "KOREAN_FOLKLORE", "소복을 입고 한을 품은 처녀 귀신. 빙결 한기를 뿜는다."},
        {6, "YOKAI_006", "삼두구미조", "Samdugumijo", YokaiGrade::Grade4, Element::Light, {105, 105, 90, 90, 26, 17, 25}, "KOREAN_FOLKLORE", "삼재를 쫓는 세 머리와 아홉 꼬리를 가진 신령한 새."},
        {7, "YOKAI_007", "저승사자", "Jeoseungsaja", YokaiGrade::Grade4, Element::Dark, {115, 115, 95, 95, 29, 20, 23}, "KOREAN_FOLKLORE", "검은 갓과 도포를 입고 망자를 인도하는 저승의 차사."},
        {8, "YOKAI_008", "어둑시니", "Eoduksini", YokaiGrade::Grade2, Element::Dark, {90, 90, 60, 60, 21, 15, 16}, "KOREAN_FOLKLORE", "사람의 두려움을 먹고 커지는 어둠의 그림자 도깨비."},
        {9, "YOKAI_009", "두두리", "Dodu-uri", YokaiGrade::Grade3, Element::Earth, {110, 110, 65, 65, 23, 22, 14}, "SAMGUK_YUSA", "경주 일대에서 목신(木神)으로 숭배받던 방망이 신령."},
        {10, "YOKAI_010", "두억시니", "Dueoksini", YokaiGrade::Grade4, Element::Physical, {130, 130, 60, 60, 30, 24, 16}, "KOREAN_FOLKLORE", "사납고 흉포하여 마주치면 머리가 쪼개진다는 야차."},
        {11, "YOKAI_011", "장산범", "Jangsan Tiger", YokaiGrade::Grade4, Element::Light, {115, 115, 80, 80, 27, 18, 25}, "KOREAN_FOLKLORE", "하얀 털로 사람의 목소리를 흉내 내어 홀리는 맹수."},
        {12, "YOKAI_012", "강철이", "Gangcheoli", YokaiGrade::Grade4, Element::Fire, {125, 125, 85, 85, 28, 22, 18}, "KOREAN_FOLKLORE", "지나가는 곳마다 가뭄과 열풍을 몰고 오는 용의 일종."},
        {13, "YOKAI_013", "달걀귀신", "Egg Ghost", YokaiGrade::Grade1, Element::Dark, {70, 70, 50, 50, 16, 12, 18}, "KOREAN_FOLKLORE", "이목구비가 없어 보기만 해도 공포를 주는 귀신."},
        {14, "YOKAI_014", "몽달귀신", "Mongdal Ghost", YokaiGrade::Grade2, Element::Dark, {80, 80, 55, 55, 19, 14, 16}, "KOREAN_FOLKLORE", "혼인을 치르지 못하고 죽어 한을 품은 총각 귀신."},
        {15, "YOKAI_015", "수살귀", "Drowning Ghost", YokaiGrade::Grade2, Element::Water, {85, 85, 75, 75, 20, 15, 19}, "KOREAN_FOLKLORE", "물가에서 사람을 끌어당겨 익사시키는 물귀신."},
        {16, "YOKAI_016", "총각귀신", "Bachelor Ghost", YokaiGrade::Grade2, Element::Earth, {85, 85, 60, 60, 20, 16, 15}, "KOREAN_FOLKLORE", "노총각으로 세상을 떠나 장난을 치는 귀신."},
        {17, "YOKAI_017", "신립의 원혼", "General Shin Spirit", YokaiGrade::Grade4, Element::Physical, {120, 120, 80, 80, 28, 24, 20}, "KOREAN_HISTORY", "탄금대에서 전사한 맹장의 굽히지 않는 무혼."},
        {18, "YOKAI_018", "삼목구", "Three-Eyed Dog", YokaiGrade::Grade3, Element::Light, {95, 95, 70, 70, 23, 17, 24}, "KOREAN_FOLKLORE", "귀신을 꿰뚫어 보는 세 번째 눈을 가진 신령한 개."},
        {19, "YOKAI_019", "불개", "Fire Hound", YokaiGrade::Grade3, Element::Fire, {100, 100, 75, 75, 24, 16, 25}, "KOREAN_FOLKLORE", "까막나라에서 해와 달을 베어 물러 내려온 불사견."},
        {20, "YOKAI_020", "이무기", "Imoogi", YokaiGrade::Grade4, Element::Water, {135, 135, 90, 90, 28, 25, 18}, "KOREAN_FOLKLORE", "천 년 동안 물속에서 여의주를 품고 승천을 기다리는 영수."},
        {21, "YOKAI_021", "백사", "White Snake", YokaiGrade::Grade3, Element::Light, {90, 90, 85, 85, 22, 18, 22}, "KOREAN_FOLKLORE", "신비로운 기운으로 만병을 고친다는 흰 뱀 신령."},
        {22, "YOKAI_022", "지네귀신", "Centipede Demon", YokaiGrade::Grade3, Element::Dark, {110, 110, 70, 70, 25, 20, 19}, "KOREAN_FOLKLORE", "수많은 다리와 맹독으로 두꺼비와 맞서는 거대 지네."},
        {23, "YOKAI_023", "거구귀", "Giant Mouth Ghost", YokaiGrade::Grade3, Element::Dark, {115, 115, 60, 60, 26, 21, 13}, "KOREAN_FOLKLORE", "입이 하늘까지 닿아 무엇이든 삼켜버리는 거대한 귀신."},
        {24, "YOKAI_024", "도섭귀", "Trickster Ghost", YokaiGrade::Grade2, Element::Light, {80, 80, 70, 70, 18, 14, 23}, "KOREAN_FOLKLORE", "사람을 홀려 길을 잃게 만드는 짓궂은 도깨비."},
        {25, "YOKAI_025", "비형랑의 귀", "Bihyeong Ghost", YokaiGrade::Grade4, Element::Light, {120, 120, 90, 90, 27, 22, 24}, "SAMGUK_YUSA", "신라 진지왕의 혼백과 여인 사이에서 태어난 반귀반인."},
        {26, "YOKAI_026", "길달", "Gildal Spirit", YokaiGrade::Grade3, Element::Dark, {100, 100, 70, 70, 24, 18, 22}, "SAMGUK_YUSA", "비형랑을 섬기다 여우로 변해 달아난 영특한 도깨비."},
        {27, "YOKAI_027", "묘두사", "Cat-Headed Snake", YokaiGrade::Grade3, Element::Dark, {95, 95, 80, 80, 23, 17, 23}, "KOREAN_FOLKLORE", "고양이 머리를 한 뱀. 사람의 마음을 꿰뚫어 본다."},
        {28, "YOKAI_028", "야광귀", "Shoe Stealer Ghost", YokaiGrade::Grade1, Element::Light, {70, 70, 50, 50, 16, 12, 21}, "KOREAN_FOLKLORE", "설날 밤 신발을 훔쳐가며 체의 구멍을 세다 도망치는 귀신."},
        {29, "YOKAI_029", "손각시", "Virgin Ghost", YokaiGrade::Grade2, Element::Water, {85, 85, 80, 80, 20, 14, 21}, "KOREAN_FOLKLORE", "처녀가 죽어 원혼이 된 서러운 손각시."},
        {30, "YOKAI_030", "창귀", "Changgwi", YokaiGrade::Grade3, Element::Dark, {95, 95, 70, 70, 25, 15, 24}, "KOREAN_FOLKLORE", "호랑이에게 물려 죽어 다른 사람을 유인하는 호랑이의 종."},
        {31, "YOKAI_031", "호랑이 산신", "Mountain Lord Tiger", YokaiGrade::Grade5, Element::Physical, {150, 150, 100, 100, 32, 28, 26}, "KOREAN_MYTH", "조선 팔도의 산하를 지키는 백수의 제왕 산군(山君)."},
        {32, "YOKAI_032", "돗가비", "Dotgabi", YokaiGrade::Grade2, Element::Physical, {90, 90, 55, 55, 21, 17, 15}, "KOREAN_FOLKLORE", "오래된 절구와 빗자루에서 깃든 장난꾸러기 돗가비."},
        {33, "YOKAI_033", "금돼지", "Golden Boar", YokaiGrade::Grade4, Element::Earth, {130, 130, 70, 70, 27, 26, 15}, "KOREAN_FOLKLORE", "최치원의 탄생 설화에 등장하는 금빛 털의 신비한 멧돼지."},
        {34, "YOKAI_034", "녹둔도 둔갑여우", "Nokdundo Fox", YokaiGrade::Grade3, Element::Fire, {100, 100, 85, 85, 24, 16, 24}, "KOREAN_FOLKLORE", "북방 녹둔도 갈대밭에서 군사들을 홀리던 천년 여우."},
        {35, "YOKAI_035", "첩지귀", "Concubine Ghost", YokaiGrade::Grade2, Element::Dark, {85, 85, 75, 75, 20, 15, 20}, "KOREAN_FOLKLORE", "질투와 원한으로 구천을 떠도는 양반가의 원혼."},
        {36, "YOKAI_036", "처용의 역신", "Plague Spirit", YokaiGrade::Grade4, Element::Dark, {115, 115, 90, 90, 28, 20, 22}, "SAMGUK_YUSA", "처용의 노래와 춤에 감복하여 물러난 역병의 신."},
        {37, "YOKAI_037", "호구별상", "Smallpox Deity", YokaiGrade::Grade3, Element::Dark, {100, 100, 80, 80, 23, 17, 20}, "KOREAN_FOLKLORE", "마마 자국을 남기며 사람을 시험하는 무서운 마마신."},
        {38, "YOKAI_038", "노구화호", "Old Crone Fox", YokaiGrade::Grade3, Element::Fire, {105, 105, 85, 85, 25, 17, 23}, "KOREAN_FOLKLORE", "늙은 노파로 변신하여 효심 깊은 아들을 시험하는 여우."},
        {39, "YOKAI_039", "신구미호", "Divine Fox", YokaiGrade::Grade5, Element::Light, {140, 140, 110, 110, 30, 22, 28}, "KOREAN_FOLKLORE", "인간을 해치지 않고 도를 깨달아 신선이 된 황금 구미호."},
        {40, "YOKAI_040", "삼두응", "Three-Headed Falcon", YokaiGrade::Grade4, Element::Light, {105, 105, 95, 95, 27, 18, 27}, "KOREAN_FOLKLORE", "부적에 그려 삼재를 막는 세 머리의 신령한 매."},
        {41, "YOKAI_041", "두억진기", "Dueok Goblin", YokaiGrade::Grade3, Element::Physical, {115, 115, 65, 65, 26, 21, 16}, "KOREAN_FOLKLORE", "도깨비 무리의 우두머리로 힘이 장사라는 요괴."},
        {42, "YOKAI_042", "무척귀", "Footless Ghost", YokaiGrade::Grade2, Element::Dark, {85, 85, 65, 65, 19, 15, 22}, "KOREAN_FOLKLORE", "발이 없어 공중에 떠다니며 서늘한 바람을 일으키는 귀신."},
        {43, "YOKAI_043", "짚신요괴", "Straw Shoe Yokai", YokaiGrade::Grade1, Element::Earth, {70, 70, 45, 45, 17, 13, 16}, "KOREAN_FOLKLORE", "버려진 짚신에 영기가 깃들어 밤마다 뛰어다니는 물건요괴."},
        {44, "YOKAI_044", "도깨비불", "Will-o'-the-Wisp", YokaiGrade::Grade2, Element::Fire, {75, 75, 80, 80, 22, 12, 25}, "KOREAN_FOLKLORE", "푸른 빛을 띠며 밤길 나그네를 홀려 구덩이로 빠뜨리는 불."},
        {45, "YOKAI_045", "삼태기요괴", "Basket Yokai", YokaiGrade::Grade1, Element::Earth, {75, 75, 50, 50, 18, 15, 14}, "KOREAN_FOLKLORE", "흙을 나르던 삼태기에 혼이 깃든 순박한 요괴."},
        {46, "YOKAI_046", "빗자루요괴", "Broom Yokai", YokaiGrade::Grade1, Element::Wind, {70, 70, 55, 55, 17, 13, 18}, "KOREAN_FOLKLORE", "피 묻은 싸리 빗자루가 변하여 도깨비 둔갑술을 쓰는 요괴."},
        {47, "YOKAI_047", "솥뚜껑요괴", "Cauldron Lid Spirit", YokaiGrade::Grade2, Element::Earth, {100, 100, 45, 45, 19, 23, 10}, "KOREAN_FOLKLORE", "무쇠 솥뚜껑에 영기가 깃들어 방어력이 엄청난 요괴."},
        {48, "YOKAI_048", "목신", "Tree God", YokaiGrade::Grade3, Element::Wind, {120, 120, 80, 80, 23, 24, 14}, "KOREAN_MYTH", "마을 입구 당산나무에 깃들어 액운을 막아주는 수호목."},
        {49, "YOKAI_049", "터주신", "Land Guardian", YokaiGrade::Grade3, Element::Earth, {115, 115, 75, 75, 22, 25, 13}, "KOREAN_MYTH", "집터와 대지를 관장하며 집안의 번영을 지키는 가신."},
        {50, "YOKAI_050", "성주신", "House Lord God", YokaiGrade::Grade4, Element::Light, {125, 125, 85, 85, 26, 24, 18}, "KOREAN_MYTH", "가옥의 대들보를 지키며 가내 평안을 관장하는 으뜸 가신."},
        {51, "YOKAI_051", "조왕신", "Hearth Goddess", YokaiGrade::Grade3, Element::Fire, {95, 95, 90, 90, 24, 18, 20}, "KOREAN_MYTH", "부엌 아궁이와 불씨를 관장하는 가신의 어머니."},
        {52, "YOKAI_052", "측간귀신", "Outhouse Maiden", YokaiGrade::Grade2, Element::Dark, {85, 85, 75, 75, 21, 14, 21}, "KOREAN_FOLKLORE", "머리카락을 세며 깜짝 놀라게 만드는 뒤깐의 여신."},
        {53, "YOKAI_053", "삼신할미", "Birth Grandmother", YokaiGrade::Grade4, Element::Light, {110, 110, 100, 100, 24, 20, 22}, "KOREAN_MYTH", "아이의 점지와 생명을 돌보는 거룩한 생명의 신령."},
        {54, "YOKAI_054", "업구렁이", "Wealth Serpent", YokaiGrade::Grade3, Element::Earth, {110, 110, 70, 70, 23, 22, 17}, "KOREAN_FOLKLORE", "집안의 복과 재물을 지켜주는 온순한 큰 구렁이 신령."},
        {55, "YOKAI_055", "지신", "Earth Spirit", YokaiGrade::Grade3, Element::Earth, {115, 115, 70, 70, 24, 23, 14}, "KOREAN_MYTH", "풍물패가 지신밟기를 할 때 깨어나는 땅의 신령."},
        {56, "YOKAI_056", "굴타귀", "Cave Chimney Ghost", YokaiGrade::Grade2, Element::Fire, {85, 85, 70, 70, 22, 16, 17}, "KOREAN_FOLKLORE", "굴뚝 연기를 타고 내려와 장난을 치는 앙증맞은 귀신."},
        {57, "YOKAI_057", "독각귀", "One-Legged Goblin", YokaiGrade::Grade2, Element::Physical, {90, 90, 60, 60, 23, 16, 19}, "KOREAN_FOLKLORE", "외다리로 껑충껑충 뛰며 전염병을 옮긴다는 도깨비."},
        {58, "YOKAI_058", "우산귀신", "Umbrella Ghost", YokaiGrade::Grade1, Element::Water, {70, 70, 55, 55, 17, 13, 20}, "KOREAN_FOLKLORE", "비 오는 날 홀로 펼쳐져 사람을 쫓아오는 우산 요괴."},
        {59, "YOKAI_059", "삿갓귀신", "Bamboo Hat Ghost", YokaiGrade::Grade2, Element::Dark, {85, 85, 65, 65, 20, 15, 19}, "KOREAN_FOLKLORE", "커다란 삿갓 아래로 붉은 눈을 번뜩이는 방랑 귀신."},
        {60, "YOKAI_060", "망태할아범", "Basket Old Man", YokaiGrade::Grade3, Element::Dark, {105, 105, 70, 70, 24, 19, 18}, "KOREAN_FOLKLORE", "말 안 듣는 아이를 망태기에 담아간다는 설화의 노인."},
        {61, "YOKAI_061", "홍살귀", "Red Gate Ghost", YokaiGrade::Grade3, Element::Fire, {100, 100, 80, 80, 25, 18, 20}, "KOREAN_FOLKLORE", "홍살문 위에서 잡귀의 침입을 감시하는 붉은 귀신."},
        {62, "YOKAI_062", "청의동자", "Blue Robed Youth", YokaiGrade::Grade3, Element::Light, {90, 90, 90, 90, 23, 17, 24}, "KOREAN_MYTH", "용왕과 산신의 심부름을 하는 영특한 푸른 옷 동자."},
        {63, "YOKAI_063", "삼족구", "Three-Legged Dog", YokaiGrade::Grade3, Element::Light, {95, 95, 70, 70, 23, 17, 25}, "KOREAN_FOLKLORE", "여우를 귀신같이 때려잡는 다리 셋 달린 영견."},
        {64, "YOKAI_064", "화용", "Fire Dragon", YokaiGrade::Grade4, Element::Fire, {135, 135, 95, 95, 30, 22, 22}, "KOREAN_MYTH", "불꽃을 품고 화마를 다스리는 전설의 화룡."},
        {65, "YOKAI_065", "목두기", "Wooden Head Ghost", YokaiGrade::Grade2, Element::Wind, {90, 90, 60, 60, 21, 18, 15}, "KOREAN_FOLKLORE", "나무토막에서 생겨나 묵직한 힘을 자랑하는 요괴."},
        {66, "YOKAI_066", "취귀", "Drunken Ghost", YokaiGrade::Grade2, Element::Water, {85, 85, 70, 70, 19, 14, 18}, "KOREAN_FOLKLORE", "주막의 술독에 빠져 죽어 술 냄새를 풍기는 취한 귀신."},
        {67, "YOKAI_067", "사령귀", "Command Ghost", YokaiGrade::Grade3, Element::Dark, {100, 100, 80, 80, 25, 18, 21}, "KOREAN_FOLKLORE", "조선 관아의 포졸 복장을 하고 원혼들을 잡아들이는 귀신."},
        {68, "YOKAI_068", "백호", "White Tiger", YokaiGrade::Grade5, Element::Physical, {150, 150, 100, 100, 33, 26, 27}, "EAST_ASIAN_MYTH", "서방을 수호하는 쇠의 기운을 다스리는 사신(四神)."},
        {69, "YOKAI_069", "청룡", "Azure Dragon", YokaiGrade::Grade5, Element::Wind, {155, 155, 110, 110, 31, 27, 26}, "EAST_ASIAN_MYTH", "동방을 수호하는 나무와 비바람의 기운을 다스리는 사신."},
        {70, "YOKAI_070", "주작", "Vermilion Bird", YokaiGrade::Grade5, Element::Fire, {140, 140, 120, 120, 34, 22, 29}, "EAST_ASIAN_MYTH", "남방을 수호하는 정화의 불꽃을 다스리는 사신."},
        {71, "YOKAI_071", "현무", "Black Tortoise", YokaiGrade::Grade5, Element::Water, {165, 165, 95, 95, 27, 34, 15}, "EAST_ASIAN_MYTH", "북방을 수호하는 물과 얼음의 기운을 다스리는 사신."},
        {72, "YOKAI_072", "황룡", "Yellow Dragon", YokaiGrade::Grade5, Element::Earth, {170, 170, 120, 120, 33, 30, 24}, "EAST_ASIAN_MYTH", "중앙을 관장하며 사신을 거느리는 지고의 황금룡."},
        {73, "YOKAI_073", "삼족오", "Three-Legged Crow", YokaiGrade::Grade5, Element::Fire, {145, 145, 115, 115, 33, 23, 29}, "KOREAN_MYTH", "태양 속에 살며 하늘과 땅을 잇는 고구려의 신조."},
        {74, "YOKAI_074", "해태", "Haetae", YokaiGrade::Grade4, Element::Light, {130, 130, 85, 85, 27, 28, 20}, "KOREAN_MYTH", "선악을 구별하고 화재와 액운을 물리치는 정의의 영수."},
        {75, "YOKAI_075", "기린", "Qilin", YokaiGrade::Grade5, Element::Light, {145, 145, 110, 110, 30, 28, 26}, "EAST_ASIAN_MYTH", "성인이 태어날 때 나타난다는 자비롭고 신령한 영수."},
        {76, "YOKAI_076", "웅녀의 영수", "Ungnyeo Guardian", YokaiGrade::Grade4, Element::Earth, {135, 135, 80, 80, 28, 26, 17}, "KOREAN_MYTH", "단군 신화의 쑥과 마늘의 정기를 품은 신성한 곰 영수."},
        // East Asian & Folklore (77..108)
        {77, "YOKAI_077", "백택", "Baize", YokaiGrade::Grade5, Element::Light, {160, 160, 120, 120, 32, 30, 25}, "SHAN_HAI_JING", "만물의 요괴와 신령의 이름을 아는 전설의 백택."},
        {78, "YOKAI_078", "누에", "Thunder Nue", YokaiGrade::Grade3, Element::Dark, {105, 105, 75, 75, 26, 17, 21}, "JAPANESE_FOLKLORE", "원숭이 머리, 호랑이 몸통, 뱀 꼬리를 한 흉조."},
        {79, "YOKAI_079", "카파", "Kappa", YokaiGrade::Grade2, Element::Water, {85, 85, 70, 70, 20, 16, 21}, "JAPANESE_FOLKLORE", "머리의 접시에 물이 담겨야 힘을 내는 수중 요괴."},
        {80, "YOKAI_080", "텐구", "Tengu", YokaiGrade::Grade4, Element::Physical, {115, 115, 85, 85, 28, 19, 26}, "JAPANESE_FOLKLORE", "부채로 폭풍을 일으키는 산속의 까마귀 도사."},
        {81, "YOKAI_081", "강시", "Jiangshi", YokaiGrade::Grade3, Element::Dark, {120, 120, 60, 60, 26, 23, 14}, "CHINESE_FOLKLORE", "팔을 뻗고 껑충 뛰며 피를 쫓는 강시."},
        {82, "YOKAI_082", "구미옥조", "Kyubi Fox", YokaiGrade::Grade4, Element::Fire, {110, 110, 95, 95, 28, 17, 26}, "EAST_ASIAN_MYTH", "요술로 대륙의 왕조를 어지럽히던 전설의 요호."},
        {83, "YOKAI_083", "주작환수", "Vermilion Beast", YokaiGrade::Grade4, Element::Fire, {120, 120, 90, 90, 29, 20, 25}, "EAST_ASIAN_MYTH", "주작의 날개에서 떨어진 불씨로 태어난 환수."},
        {84, "YOKAI_084", "기린환수", "Jade Qilin", YokaiGrade::Grade4, Element::Light, {125, 125, 90, 90, 27, 25, 22}, "EAST_ASIAN_MYTH", "옥빛 뿔을 가진 자비로운 신수."},
        {85, "YOKAI_085", "혼돈", "Hundun", YokaiGrade::Grade5, Element::Dark, {165, 165, 115, 115, 33, 28, 20}, "SHAN_HAI_JING", "얼굴과 구멍이 없는 고대 사흉(四凶) 중 으뜸."},
        {86, "YOKAI_086", "궁기", "Qiongqi", YokaiGrade::Grade5, Element::Physical, {150, 150, 100, 100, 35, 24, 28}, "SHAN_HAI_JING", "날개 달린 식인 호랑이 형상의 고대 사흉."},
        {87, "YOKAI_087", "도올", "Taowu", YokaiGrade::Grade5, Element::Earth, {160, 160, 95, 95, 32, 31, 17}, "SHAN_HAI_JING", "흉포하고 완고하여 물러서지 않는 멧돼지 사흉."},
        {88, "YOKAI_088", "도철", "Taotie", YokaiGrade::Grade5, Element::Dark, {170, 170, 90, 90, 34, 29, 16}, "SHAN_HAI_JING", "무엇이든 집어삼키는 탐욕의 고대 사흉."},
        {89, "YOKAI_089", "응룡", "Yinglong", YokaiGrade::Grade5, Element::Water, {155, 155, 110, 110, 32, 28, 25}, "SHAN_HAI_JING", "날개를 달고 비구름을 부르는 황제의 용."},
        {90, "YOKAI_090", "치우의 혼백", "Chiyou Spirit", YokaiGrade::Grade5, Element::Physical, {160, 160, 100, 100, 35, 30, 22}, "EAST_ASIAN_MYTH", "동두철액(銅頭鐵額)의 군신 치우 천왕의 무혼."},
        {91, "YOKAI_091", "풍백", "Wind Master", YokaiGrade::Grade4, Element::Physical, {110, 110, 95, 95, 27, 19, 27}, "KOREAN_MYTH", "단군 조선의 삼사(三師) 중 바람을 관장하는 신령."},
        {92, "YOKAI_092", "우사", "Rain Master", YokaiGrade::Grade4, Element::Water, {115, 115, 95, 95, 26, 21, 23}, "KOREAN_MYTH", "단군 조선의 삼사 중 비를 다스리는 자비로운 신령."},
        {93, "YOKAI_093", "운사", "Cloud Master", YokaiGrade::Grade4, Element::Light, {110, 110, 100, 100, 27, 20, 25}, "KOREAN_MYTH", "단군 조선의 삼사 중 구름을 모아 그늘을 드리우는 신령."},
        {94, "YOKAI_094", "설녀", "Yuki-onna", YokaiGrade::Grade3, Element::Water, {95, 95, 90, 90, 24, 16, 24}, "JAPANESE_FOLKLORE", "눈보라 속에서 얼어붙은 숨결을 내뿜는 설녀."},
        {95, "YOKAI_095", "야차", "Yaksha", YokaiGrade::Grade4, Element::Physical, {125, 125, 80, 80, 30, 22, 23}, "BUDDHIST_FOLKLORE", "불법을 수호하며 날렵하게 악귀를 베는 신장."},
        {96, "YOKAI_096", "아수라", "Asura", YokaiGrade::Grade5, Element::Fire, {145, 145, 100, 100, 34, 25, 26}, "BUDDHIST_FOLKLORE", "여섯 개의 팔과 세 개의 얼굴로 싸움을 숭상하는 투신."},
        {97, "YOKAI_097", "가루다", "Garuda", YokaiGrade::Grade5, Element::Light, {140, 140, 105, 105, 33, 24, 28}, "BUDDHIST_FOLKLORE", "독사를 삼키고 황금 날개로 천공을 가르는 금시조."},
        {98, "YOKAI_098", "간다르바", "Gandharva", YokaiGrade::Grade3, Element::Light, {95, 95, 95, 95, 23, 17, 24}, "BUDDHIST_FOLKLORE", "향기를 먹고 아름다운 천상의 음악을 연주하는 악신."},
        {99, "YOKAI_099", "킨나라", "Kinnara", YokaiGrade::Grade3, Element::Light, {95, 95, 90, 90, 24, 18, 23}, "BUDDHIST_FOLKLORE", "사람과 새의 형상을 한 천상의 가객."},
        {100, "YOKAI_100", "마후라가", "Mahoraga", YokaiGrade::Grade4, Element::Earth, {130, 130, 85, 85, 29, 25, 18}, "BUDDHIST_FOLKLORE", "거대한 구렁이의 형상으로 대지를 지키는 팔부신중."},
        {101, "YOKAI_101", "용왕의 사자", "Dragon Envoy", YokaiGrade::Grade4, Element::Water, {120, 120, 95, 95, 27, 23, 23}, "KOREAN_FOLKLORE", "동해 용궁의 보물과 서찰을 전하는 신령한 거북."},
        {102, "YOKAI_102", "바다이무기", "Sea Serpent", YokaiGrade::Grade4, Element::Water, {135, 135, 90, 90, 28, 24, 20}, "KOREAN_FOLKLORE", "거센 파도와 해일을 일으키는 바다의 대형 이무기."},
        {103, "YOKAI_103", "염라의 옥졸", "Underworld Guard", YokaiGrade::Grade3, Element::Dark, {115, 115, 75, 75, 26, 21, 18}, "BUDDHIST_FOLKLORE", "지옥에서 쇠방망이를 들고 죄인을 다스리는 옥졸."},
        {104, "YOKAI_104", "우두나찰", "Ox-Headed Demon", YokaiGrade::Grade4, Element::Earth, {140, 140, 75, 75, 30, 26, 16}, "BUDDHIST_FOLKLORE", "소의 머리를 하고 지옥문을 지키는 강력한 나찰."},
        {105, "YOKAI_105", "마면나찰", "Horse-Headed Demon", YokaiGrade::Grade4, Element::Physical, {130, 130, 80, 80, 29, 23, 24}, "BUDDHIST_FOLKLORE", "말의 머리를 하고 혼백을 쫓는 날쌘 나찰."},
        {106, "YOKAI_106", "천지음양수", "Yin-Yang Beast", YokaiGrade::Grade5, Element::Light, {160, 160, 120, 120, 32, 29, 25}, "EAST_ASIAN_MYTH", "조선의 음양 영맥이 뭉쳐 태어난 태극의 영수."},
        {107, "YOKAI_107", "태극진령", "Taiji True Spirit", YokaiGrade::Grade5, Element::Light, {170, 170, 130, 130, 34, 31, 26}, "KOREAN_MYTH", "천지음양부의 본원 정기가 깃든 최고위 신령."},
        {108, "YOKAI_108", "천명영호", "Destined Spirit Fox", YokaiGrade::Grade5, Element::Light, {180, 180, 140, 140, 36, 32, 30}, "KOREAN_MYTH", "108번째 음양의 조화를 완성하는 전설의 영수."}
    };

    s_yokaiDatabase.clear();
    s_yokaiDatabase.reserve(120);

    for (int i = 0; i < 108; ++i) {
        const auto& m = k108Roster[i];
        Yokai y(m.num, m.id, m.nameKo, m.grade, m.elem, m.stats, m.origin, m.lore);
        
        // Assign diverse starter skills based on element
        y.addSkill(s_skillDatabase[0]); // Club strike
        if (m.elem == Element::Fire) {
            y.addSkill(s_skillDatabase[3]); // Goblin fire
            y.addSkill(s_skillDatabase[4]); // Fox fire
            y.addSkill(s_skillDatabase[5]); // Flame breath
        } else if (m.elem == Element::Water) {
            y.addSkill(s_skillDatabase[6]); // Chill wail
            y.addSkill(s_skillDatabase[7]); // Aqua tide
            y.addSkill(s_skillDatabase[8]); // Absolute zero
        } else if (m.elem == Element::Light) {
            y.addSkill(s_skillDatabase[11]); // Talisman seal
            y.addSkill(s_skillDatabase[12]); // Holy purge
            y.addSkill(s_skillDatabase[17]); // Illusion burst
        } else if (m.elem == Element::Dark) {
            y.addSkill(s_skillDatabase[14]); // Taunt
            y.addSkill(s_skillDatabase[15]); // Fear gaze
            y.addSkill(s_skillDatabase[16]); // Spirit drain
        } else {
            y.addSkill(s_skillDatabase[1]); // Iron charge
            y.addSkill(s_skillDatabase[2]); // Earth slam
            y.addSkill(s_skillDatabase[9]); // Thunder bolt
        }

        s_yokaiDatabase.push_back(y);
        s_encyclopedia.registerTemplate(m.num, m.id, m.nameKo, m.nameEn, m.grade, m.elem, m.origin, m.lore);
    }

    // Boss Yokai Entries
    Yokai boss1(1, "YOKAI_BOSS_01", "폭주한 붉은 도깨비", YokaiGrade::Grade3, Element::Fire, {160, 160, 90, 90, 32, 22, 18}, "BOSS", "음양당 괴승 묘각의 주술로 흑화된 붉은 도깨비.");
    boss1.addSkill(s_skillDatabase[3]); boss1.addSkill(s_skillDatabase[0]); boss1.addSkill(s_skillDatabase[2]); boss1.addSkill(s_skillDatabase[15]);

    Yokai boss2(3, "YOKAI_BOSS_02", "천년 무쇠 불가사리", YokaiGrade::Grade4, Element::Earth, {220, 220, 100, 100, 36, 32, 14}, "BOSS", "소백산맥의 쇠를 집어삼키고 폭주하는 거대 괴수.");
    boss2.addSkill(s_skillDatabase[1]); boss2.addSkill(s_skillDatabase[2]); boss2.addSkill(s_skillDatabase[5]); boss2.addSkill(s_skillDatabase[9]);

    Yokai boss3(15, "YOKAI_BOSS_03", "남해 수살귀 왕", YokaiGrade::Grade4, Element::Water, {200, 200, 120, 120, 34, 25, 24}, "BOSS", "남해안 갯벌을 안개로 뒤덮고 영혼을 삼키는 수살귀의 수장.");
    boss3.addSkill(s_skillDatabase[6]); boss3.addSkill(s_skillDatabase[7]); boss3.addSkill(s_skillDatabase[8]); boss3.addSkill(s_skillDatabase[16]);

    Yokai boss4(2, "YOKAI_BOSS_04", "백발 백호 구미호", YokaiGrade::Grade5, Element::Fire, {240, 240, 140, 140, 38, 26, 30}, "BOSS", "음양당 좌호법이 조종하는 결계의 수호 백발 구미호.");
    boss4.addSkill(s_skillDatabase[4]); boss4.addSkill(s_skillDatabase[5]); boss4.addSkill(s_skillDatabase[15]); boss4.addSkill(s_skillDatabase[17]);

    Yokai boss5(85, "YOKAI_BOSS_05", "태고의 혼돈 흉수", YokaiGrade::Grade5, Element::Dark, {300, 300, 160, 160, 42, 34, 28}, "BOSS", "음양당 당주 묵영이 일식의 성채에서 깨워낸 태고의 혼돈 흉수.");
    boss5.addSkill(s_skillDatabase[18]); boss5.addSkill(s_skillDatabase[19]); boss5.addSkill(s_skillDatabase[13]); boss5.addSkill(s_skillDatabase[5]);

    s_yokaiDatabase.push_back(boss1);
    s_yokaiDatabase.push_back(boss2);
    s_yokaiDatabase.push_back(boss3);
    s_yokaiDatabase.push_back(boss4);
    s_yokaiDatabase.push_back(boss5);

    // Initial Encyclopedia Discoveries
    s_encyclopedia.markCaptured("YOKAI_001");
    s_encyclopedia.markCaptured("YOKAI_002");
    s_encyclopedia.markCaptured("YOKAI_003");
    s_encyclopedia.markSeen("YOKAI_005");

    // 3. Artifacts (12 Dual-Trait Folklore Artifacts)
    Artifact dokkaebiHat{"ART_DOKKAEBI_HAT", "도깨비 감투", ArtifactBuffType::CritRateBoost, 30, ArtifactDebuffType::QiDrainPerTurn, 5, "쓰면 투명해지나 영혼의 기운을 갉아먹는 도깨비 털모자."};
    Artifact centipedeEgg{"ART_CENTIPEDE_EGG", "백년 묵은 지네의 알", ArtifactBuffType::ImmunityBurn, 1, ArtifactDebuffType::MaxHpReduction, 20, "독기를 막아주나 소지자의 기혈을 쇠약하게 만든다."};
    Artifact foxShard{"ART_FOX_MARBLE_SHARD", "깨진 여우구슬", ArtifactBuffType::MagicAtkBoost, 40, ArtifactDebuffType::DefReduction, 35, "엄청난 영술의 힘을 주지만 육신을 무방비로 만든다."};
    Artifact demonTile{"ART_DEMON_TILE", "귀면와 (도깨비 기와)", ArtifactBuffType::DamageReduction, 20, ArtifactDebuffType::SpdReduction, 15, "잡귀의 타격을 튕겨내지만 무거워서 신법이 둔해진다."};
    Artifact jadeMirror{"ART_JADE_MIRROR", "벽사 옥경 (玉鏡)", ArtifactBuffType::ImmunityFreezeFear, 1, ArtifactDebuffType::HpDrainPerTurn, 3, "빙결과 공포를 막아주나 매 턴 생명력을 소모한다."};
    Artifact goldenBell{"ART_GOLDEN_BELL", "신령 무당방울", ArtifactBuffType::CaptureRateBoost, 15, ArtifactDebuffType::ExpPenalty, 20, "요괴의 넋을 홀려 계약을 쉽게 하나 수련 경험치가 감소한다."};
    Artifact shadowIncense{"ART_SHADOW_INCENSE", "음기도방의 향로", ArtifactBuffType::StatusChanceBoost, 50, ArtifactDebuffType::DefReduction, 15, "상태이상을 쉽게 걸리게 하지만 방어력을 떨어뜨린다."};
    Artifact reaperScroll{"ART_REAPER_SCROLL", "저승사자의 명부 조각", ArtifactBuffType::MagicAtkBoost, 50, ArtifactDebuffType::QiDrainPerTurn, 10, "망자의 힘을 빌려 절대적인 위력을 내나 영력을 크게 소모한다."};
    Artifact samduFeather{"ART_SAMDU_FEATHER", "삼두구미조의 깃털", ArtifactBuffType::ImmunityBurn, 1, ArtifactDebuffType::SpdReduction, 10, "삼재를 막아주는 신비한 아홉 빛깔 깃털."};
    Artifact baizeTusk{"ART_BAIZE_TUSK", "백택의 뿔 조각", ArtifactBuffType::CaptureRateBoost, 25, ArtifactDebuffType::MaxHpReduction, 15, "만물의 이름을 통달하여 요괴 포획을 극대화한다."};
    Artifact dragonScale{"ART_DRAGON_SCALE", "청룡의 역린", ArtifactBuffType::DamageReduction, 30, ArtifactDebuffType::QiDrainPerTurn, 8, "절대적인 방호벽을 펼치는 신령한 푸른 비늘."};
    Artifact sunOrb{"ART_SUN_ORB", "삼족오의 일륜석", ArtifactBuffType::CritRateBoost, 45, ArtifactDebuffType::HpDrainPerTurn, 5, "태양의 흑점을 담아 치명타를 폭발시키는 붉은 옥구슬."};

    s_artifactDatabase = {
        dokkaebiHat, centipedeEgg, foxShard, demonTile,
        jadeMirror, goldenBell, shadowIncense, reaperScroll,
        samduFeather, baizeTusk, dragonScale, sunOrb
    };

    // 4. NPCs Database (Across All 5 Regions)
    NPC jumoh{"NPC_001", 0, 3, 3, 2, "주모 월선", "도선사 주막 주모",
             {"어서 오시게! 북한산 꼭대기 도선사에 수상한 음양당 놈들이", "들어앉더니 온 산천에 흉흉한 요기가 돌고 있다오.", "따뜻한 장터국밥 한 그릇 말아드릴 테니 기운 차리시게!"},
             NPCActionType::TavernRest, "MQ_001"};

    NPC merchant{"NPC_002", 0, 14, 3, 3, "벽사 상인 박 서방", "관상감 공인 부적 상인",
                {"관상감에서 내려온 영술사님이시군요!", "음양당 놈들이 부리는 요괴들은 벽사 부적으로 계약할 수 있습니다.", "주막 아래쪽 길로 나가면 북한산 고갯길로 이어집니다."},
                NPCActionType::TalismanShop, ""};

    NPC bride{"NPC_003", 0, 11, 7, 2, "방귀쟁이 며느리", "근심 가득한 며느리",
             {"흑흑... 제 몸에 깃든 바람 요괴 때문에", "시아버지 앞에서 큰 방귀 소리를 내어 쫓겨날 판입니다.", "고갯길의 바람 요괴를 진정시켜 주실 수 있을까요?"},
             NPCActionType::QuestTrigger, "SQ_001"};

    NPC bossMyogak{"NPC_004", 2, 9, 3, 4, "괴승 묘각", "음양당 북악방주",
                  {"크크크... 관상감의 애송이 영술사가 여기까지 기어들어왔구나!", "천지음양부는 이미 깨어졌고, 조선의 108 요괴는 우리 음양당의 손에 들어갈 것이다!", "폭주한 붉은 도깨비여, 저 놈을 찢어발겨라!"},
                  NPCActionType::BossEncounter, "MQ_001"};

    NPC sobaekElder{"NPC_005", 3, 4, 3, 2, "소백산 심마니", "산길 안내인",
                   {"조심하게나! 소백산 무쇠광산에 거대 불가사리가 나타나", "온 산의 쇠를 먹어치우며 폭주하고 있다네!"},
                   NPCActionType::QuestTrigger, "MQ_002"};

    NPC finalBoss{"NPC_006", 4, 9, 3, 4, "당주 묵영", "음양당 총수",
                 {"벽사청의 애송이가 마침내 일식의 성채까지 도달했군.", "허나 이미 늦었다. 108 요괴의 원혼으로 태고의 혼돈을 부활시켰다!", "조선의 하늘 아래 새로운 음양의 질서를 세우리라!"},
                 NPCActionType::BossEncounter, "MQ_005"};

    s_npcDatabase = {jumoh, merchant, bride, bossMyogak, sobaekElder, finalBoss};

    // 5. 5 Main Campaign Chapters & 5 Side Quests
    Quest mq1{"MQ_001", QuestType::Main, "제1장: 도선사의 붉은 안개", 1, "관상감 벽사청의 명을 받아 도선사 인근에 번지는 음양당의 요기를 조사하라.",
             {"도선사 주막 주모와 대화하여 정보 수집", "북한산 고갯길을 지나 도선사 대웅전 진입", "음양당 하수인 괴승 묘각 격파"}, 0, {500, 300, "ART_DOKKAEBI_HAT"}, QuestState::InProgress};

    Quest mq2{"MQ_002", QuestType::Main, "제2장: 소백산의 무쇠 괴수", 2, "소백산맥의 쇠를 먹고 폭주하는 거대 불가사리와 음양당 철포방주를 진정시켜라.",
             {"소백산 심마니와 대화", "소백산맥 무쇠 광산 험로 돌파", "천년 무쇠 불가사리 격파 및 정화"}, 0, {800, 500, "ART_DEMON_TILE"}, QuestState::NotStarted};

    Quest mq3{"MQ_003", QuestType::Main, "제3장: 남해 포구의 침묵", 3, "남해안 갯벌을 뒤덮은 안개와 수살귀 군단을 이끄는 음양당 수로방주를 궤멸하라.",
             {"남해안 포구 주모와 대화", "귀신 갯벌 폐선박 조사", "남해 수살귀 왕 격파"}, 0, {1200, 700, "ART_CENTIPEDE_EGG"}, QuestState::NotStarted};

    Quest mq4{"MQ_004", QuestType::Main, "제4장: 지리산 여우골의 미궁", 4, "지리산 대나무 밀림에 세워진 음양결계를 파괴하고 백발 구미호를 해방하라.",
             {"지리산 천년 사찰 조사", "여우골 동굴 미궁 돌파", "음양당 좌호법 및 백발 구미호 정화"}, 0, {1800, 1000, "ART_FOX_MARBLE_SHARD"}, QuestState::NotStarted};

    Quest mq5{"MQ_005", QuestType::Main, "최종장: 일식의 성채 (천지음양의 밤)", 5, "음양당 본거지 일식의 성채에서 당주 묵영과 태고의 혼돈 흉수를 분쇄하고 천지음양부를 복원하라.",
             {"일식의 성채 결계탑 해제", "천문음양전 중심 제단 진입", "음양당 당주 묵영 및 태고의 혼돈 격파"}, 0, {3000, 2000, "ART_SUN_ORB"}, QuestState::NotStarted};

    Quest sq1{"SQ_001", QuestType::Side, "방귀쟁이 며느리의 하소연", 0, "너무 강한 영적 바람으로 쫓겨날 위기에 처한 며느리를 위해 바람 요괴를 진정시켜라.",
             {"주막 뒤뜰 며느리와 대화", "바람 요괴와 전투 후 계약 완료"}, 0, {250, 150, "ART_DEMON_TILE"}, QuestState::NotStarted};

    Quest sq2{"SQ_002", QuestType::Side, "전우치의 도술 비급", 0, "조선 최고의 도사 전우치가 남긴 영술 비급을 찾아 팔도 요괴를 통달하라.",
             {"도선사 탑 뒤편 비석 조사", "환영 요괴와 비무 승리"}, 0, {400, 250, "ART_JADE_MIRROR"}, QuestState::NotStarted};

    Quest sq3{"SQ_003", QuestType::Side, "두두리의 목신제", 0, "경주 고목나무에 깃든 두두리 신령에게 제를 올려 축복을 받아라.",
             {"당산나무에 공물 바치기", "두두리 신령과 친선 대련"}, 0, {600, 350, "ART_SAMDU_FEATHER"}, QuestState::NotStarted};

    Quest sq4{"SQ_004", QuestType::Side, "어둑시니의 두려움", 0, "사람들의 공포를 먹고 괴로워하는 어둑시니를 빛의 부적으로 정화하라.",
             {"어두운 동굴 탐색", "어둑시니 진정 및 계약"}, 0, {700, 400, "ART_GOLDEN_BELL"}, QuestState::NotStarted};

    Quest sq5{"SQ_005", QuestType::Side, "108 요괴 전종 도감의 완성", 0, "조선 팔도에 흩어진 108종의 모든 요괴와 계약하여 음양견문록을 완성하라.",
             {"108종 요괴 전원 도감 계약 달성"}, 0, {5000, 5000, "ART_SUN_ORB"}, QuestState::InProgress};

    s_questManager.registerQuest(mq1);
    s_questManager.registerQuest(mq2);
    s_questManager.registerQuest(mq3);
    s_questManager.registerQuest(mq4);
    s_questManager.registerQuest(mq5);
    s_questManager.registerQuest(sq1);
    s_questManager.registerQuest(sq2);
    s_questManager.registerQuest(sq3);
    s_questManager.registerQuest(sq4);
    s_questManager.registerQuest(sq5);
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
        if (y.getNumber() == number && y.getId().find("BOSS") == std::string::npos) return y;
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
