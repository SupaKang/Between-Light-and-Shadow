# PROCEDURAL ORGANIC MAP & INTERIOR ARCHITECTURE SPEC
## (비정형 유기적 마을/실내 맵 생성 알고리즘 및 장식 오브젝트 설계서)

---

## 1. 맵 구조적 요구사항 요약
- **단순 일자형 탈피**: 비정형 지형, 굴곡진 돌길, 사선 배치된 가옥, 마당, 실개천, 다리.
- **실내(Interior) 맵 연동**: 집/상점/주막 문으로 진입 시 아늑한 내부 맵(대청마루, 온돌방, 창호문, 병풍, 화로) 및 고유 NPC 배치.
- **풍부한 조선 민속 장식**: 장승(천하대장군/지하여장군), 당산나무와 오색천, 장독대, 짚단, 멍석, 청사초롱 등.
- **절차적 비정형 맵 생성 (Procedural Organic Map Generator)**: 고난수(PRNG + Noise) 기반 유기적 배치 알고리즘.

---

## 2. 절차적 비정형 맵 생성 알고리즘 (PCG Pipeline)

```text
[Step 1. 지형/고저차 생성] -> [Step 2. 메인/서브 도로망 형성] -> [Step 3. 비정형 건물 & 마당 배치] -> [Step 4. 장식 오브젝트 & NPC 스폰]
(Perlin/Simplex Noise)      (A* 최소비용 경로 곡선화)         (Poisson Disk Sampling)        (컨셉별 가중치 클러스터링)
```

### 2.1 단계별 알고리즘 상세

1. **지형 및 고저차 레이어 (Heightmap & Water Flow)**
   - 2D Simplex Noise 기반으로 자연스러운 완만한 언덕, 절벽 라인, 실개천(Water Stream) 흐름 생성.
   - 워터 블렌딩 타일로 강변 곡선 표현.

2. **유기적 도로망 생성 (Organic Path Network)**
   - 중심 랜드마크(예: 당산나무 광장, 주막 삼거리)를 시작 노드로 설정.
   - 각 건물 입구 및 맵 탈출구로 향하는 경로를 단순 직선이 아닌 **노이즈 변위가 적용된 A* 알고리즘**으로 구불구불한 흙길/돌길 타일로 연결.

3. **비정형 건물 & 마당 배치 (Building & Yard Layout)**
   - **Poisson Disk Sampling**: 건물 간 최소 이격 거리를 유지하면서도 격자에 갇히지 않는 유기적 좌표 선정.
   - 건물 크기/유형 무작위 배분: 초가집(3x2 ~ 4x3 타일), 기와집(5x4 타일), 주막(6x3 타일), 대장간/약방.
   - 건물 주변에 울타리(싸리나무 담장, 돌담)와 마당(흙바닥 타일) 생성.

4. **실내 맵 (Sub-Interior) 자동 매핑**
   - 건물의 문(Door) 타일에 상호작용 트리거 `WarpTrigger { targetMapId, spawnX, spawnY }` 자동 연결.
   - 실내 템플릿(초가 민가, 한옥 대청마루, 주막 객실, 도사 은거 사당)에 맞춰 가구(병풍, 화로, 장롱, 서책) 및 마을 주민/의뢰 NPC 배치.

5. **장식물 클러스터링 (Fringe Object Decoration)**
   - **마당 구역**: 장독대 3~5개 무리지어 배치, 널어놓은 빨래, 장작더미, 지게.
   - **마을 어귀**: 장승 쌍(천하대장군, 지하여장군), 솟대, 돌탑.
   - **광장/중심**: 거대 당산나무와 오색 금줄, 벤치 평상.
   - **주막 주변**: 걸려있는 붉은/청색 초롱, 야외 주막 평상과 막걸리 주전자.

---

## 3. C++ 데이터 구조 및 구현 설계

```cpp
namespace JoseonRPG {

enum class BuildingType : uint8_t {
    ThatchedCottage,   // 초가집
    TileHouse,         // 기와집
    Tavern,            // 주막
    Herbalist,         // 한약방/의원
    Shrine             // 사당/당집
};

struct IndoorMap {
    uint16_t indoorId;
    std::string name;
    uint8_t width = 12;
    uint8_t height = 8;
    std::vector<uint8_t> groundLayer;
    std::vector<uint8_t> obstacleLayer;
    std::vector<uint8_t> overheadLayer;
    std::vector<NPCData> npcs;
    Point returnFieldPos;
};

struct OrganicVillageGenerator {
    static void generateVillage(Map& outMap, std::vector<IndoorMap>& outInteriors, uint32_t seed);
};

} // namespace JoseonRPG
```

---

## 4. 용량 및 성능 최적화 보장

1. **절차적 맵 시드 생성**: 맵의 모든 타일을 하드코딩하지 않고, **알고리즘 코드 + 시드 번호(4바이트)**로 런타임에 맵을 생성할 수 있어 **바이너리 용량을 대폭 절약** (수십 개의 맵 데이터를 수 KB 이내로 압축).
2. **타일 아틀라스 인덱싱**: 공통 타일셋(자연물, 가옥 파츠, 실내 가구, 장식물)을 1장의 128x128 4bpp 아틀라스 시트로 집약하여 메모리 및 용량 최소화.
