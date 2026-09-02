# SIZE BUDGET (1.44MB 용량 예산표 및 최적화 전략)

## 1. 예산 할당 요약 (Total Target: ≤ 1,474,560 Bytes)

| 영역 | 할당 용량 | 세부 항목 | 사용 기법 |
| :--- | :--- | :--- | :--- |
| **바이너리 코드 (.text, .rdata)** | 250 KB | 엔진, 렌더러, 전투, UI, 상태머신 | `/O1` or `/O2`, `/GL`, `/OPT:REF`, `/OPT:ICF` |
| **정적 데이터 (.data, tables)** | 150 KB | 108종 요괴, 스킬, 유물, 퀘스트, 맵 | 구조체 바이너리 직렬화 / 콤팩트 바이트 테이블 |
| **그래픽 에셋 (도트 스프라이트)** | 600 KB | 캐릭터 108종 x 2프레임, 타일셋 5종 | 4bpp / 8bpp 팔레트 인덱싱, RLE/Deflate 압축 |
| **사운드 & 음악 (BGM/SFX)** | 250 KB | BGM 8곡, 효과음 20종 | 4채널 Chiptune 사운드폰트 합성 / 트래커 모듈 |
| **안전 마진 (System Headroom)** | 194.5 KB | 예비 용량 및 실행 오버헤드 | - |
| **합계** | **1,444.5 KB** | **1.44MB 한도 100% 만족** | - |

## 2. MSVC 링커 최적화 옵션
```cmake
# CMake 용량 최적화 플래그
set(CMAKE_CXX_FLAGS_RELEASE "/O2 /GL /Gy /GF /DNDEBUG")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/LTCG /OPT:REF /OPT:ICF /MANIFEST:NO")
```
