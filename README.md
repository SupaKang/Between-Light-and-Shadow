# 108:음양견문록 (108: Yin-Yang Chronicle)

> **1.44MB 플로피 디스크 용량 제한**을 고려하여 설계된 초소형 C++17 Windows 독립 실행 조선 민속 턴제 요괴 수집 RPG.

---

## 📌 프로젝트 개요

| 항목 | 명세 |
| :--- | :--- |
| **프로젝트명** | **108:음양견문록** (108: Yin-Yang Chronicle) |
| **장르** | 조선시대 민속 판타지 턴제 요괴 수집 RPG |
| **대상 플랫폼** | Windows (x64 / x86 Standalone Executable) |
| **기술 스택** | C++17, Native Win32 API, Software Framebuffer / Direct2D |
| **해상도** | 320 x 180 (16:9 정수 스케일링 지원) |
| **타일 규격** | 16 x 16 Pixel Tilemap |
| **용량 한계** | **최대 1.44MB (1,474,560 Bytes)** 이하 독립 실행 파일 |
| **의존성** | 무설치, 노 서버, 노 브라우저, 외부 대용량 런타임 의존성 0% |

---

## 🎮 핵심 게임플레이 루프

```text
[16x16 타일 탐험] ➔ [야생 요괴 조우] ➔ [1v1 턴제 전투] ➔ [상태이상/약화]
         ▲                                                       │
         │                                                       ▼
[새로운 지역 해금] ◀── [파티 3마리 편성] ◀── [성장 및 승급] ◀── [요괴 계약/포획]
```

- **요괴 108종**: 한국 전승 70% + 해외 전승 30% 기반의 108종 요괴 도감.
- **5종 핵심 상태이상**: 화상, 빙결, 봉인, 마비, 공포의 전략적 연계.
- **양날의 검 유물(Artifact)**: 강력한 패시브 버프와 치명적인 디버프가 공존하는 유물 수집 & 즉시 파괴 메커니즘.
- **메인 캠페인**: 영맥을 왜곡하고 금지된 도술을 부리는 사설 음양학파 **음양당(陰陽堂)** 처치.
- **서브 퀘스트**: 조선 야사 및 민담 기반의 독립 단발 설화 퀘스트.

---

## 📂 디렉토리 구조

```text
joseon-yokai-rpg/
├── AGENTS.md                 # Codex / AI 에이전트 작업 지침 및 프로토콜
├── README.md                 # 프로젝트 개요 및 빌드 안내
├── CMakeLists.txt            # C++17 빌드 시스템
├── docs/                     # 기획 및 기술 명세서 (SSOT)
│   ├── PROJECT_MASTER.md     # 단일 진실 공급원 (SSOT)
│   ├── GAME_OVERVIEW.md      # 게임 개요 및 콘셉트
│   ├── CORE_PILLARS.md       # 4대 핵심 개발 기둥
│   ├── DESIGN_DIRECTION.md   # 비주얼/사운드/시스템 디자인 방향
│   ├── world/                # 월드 맵 및 구역 명세
│   ├── yokai/                # 108종 요괴 데이터 및 5종 상태이상 설계
│   ├── battle/               # 1v1 턴제 전투 시스템 및 수식
│   ├── growth/               # 레벨(1~50) 및 Ⅰ~Ⅴ 등급 승급 시스템
│   ├── artifact/             # 유물 시스템 (버프+디버프 및 즉시 파괴)
│   ├── campaign/             # 음양당 메인 스토리 및 민담 서브퀘스트
│   ├── ui/                   # 320x180 픽셀 UI 레이아웃
│   ├── tech/                 # 아키텍처, 1.44MB 용량 예산표, 스파이크 계획
│   ├── art/                  # 도트 그래픽 및 오방색 팔레트 규격
│   └── release/              # 릴리즈 및 패키징 체크리스트
├── data/                     # 게임 런타임 데이터 (JSON/바이너리)
│   ├── yokai/                # 요괴 스탯/도감 데이터
│   ├── skills/               # 기술 및 영력 소모 데이터
│   ├── artifacts/            # 유물 버프/디버프 데이터
│   ├── quests/               # 메인/서브 퀘스트 데이터
│   └── maps/                 # 16x16 타일맵 데이터
├── src/                      # C++17 소스 코드
│   ├── core/                 # 윈도우, 입력, 320x180 소프트웨어 렌더러
│   ├── world/                # 타일맵 및 2D 카메라
│   ├── battle/               # 턴제 전투 엔진 및 상태이상 처리
│   ├── data/                 # 데이터 매니저
│   ├── gameplay/             # 요괴, 파티(3마리), 유물 인벤토리
│   ├── ui/                   # 비트맵 폰트 및 UI 렌더링
│   └── main.cpp              # 메인 엔트리포인트 (Phase 1 Spike)
├── assets/                   # 소스 리소스 (용량 최적화 도트 에셋)
│   ├── sprites/
│   ├── tiles/
│   └── audio/
└── tests/                    # 유닛 테스트 및 수식 검증
    ├── test_main.cpp
    └── test_battle.cpp
```

---

## 🛠️ 빌드 및 실행 방법

### 요구 사항
- Windows 10/11
- CMake 3.20 이상
- Visual Studio 2022 C++ Build Tools (MSVC v143) 또는 Clang/GCC

### 빌드 명령어 (Release - 크기 최적화)

```powershell
# 빌드 디렉토리 생성 및 설정
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# 빌드 실행
cmake --build build --config Release

# 실행 및 바이너리 용량 확인
.\build\Release\joseon_yokai_rpg.exe
```

---

## 🚀 로드맵 (Development Roadmap)

- [x] **Phase 0: Repository Bootstrap** — SSOT 문서 체계 및 디렉토리/스키마 구축
- [x] **Phase 1: Technical Spike** — 320x180 렌더러, 타일 이동, Release 크기 측정 (87KB / 6.04%)
- [x] **Phase 2: Battle Prototype** — 1v1 턴제, HP/영력, 4기술, 5종 상태이상, 속도 선공권, 포획 수식 (109KB / 7.57%)
- [x] **Phase 3: Collection Prototype** — 108종 도감(Codex) UI, 3마리 파티, Lv.1~50 성장 & Ⅰ~Ⅴ 승급, 야생 인카운터 (131KB / 9.10%)
- [x] **Phase 4: Artifact System** — 8종 양날의 검 유물, 버프/디버프 공존, 즉시 파괴 & 영기 환원 (142KB / 9.86%)
- [x] **Phase 5: Vertical Slice** — 도선사 주막/북한산/대웅전 3개 맵 전환, NPC 대화/주막 완치, 퀘스트 추적, 챕터 1 음양당 보스전 (169KB / 11.74%)
- [ ] **Phase 6: Full Content** — 108종 요괴, 음양당 캠페인, 민담 퀘스트
