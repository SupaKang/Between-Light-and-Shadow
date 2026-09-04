# AI 기반 픽셀아트 에셋 제작 & 엔진 인제스트 파이프라인 가이드

본 문서는 **『108: 음양견문록 (108: Yin-Yang Chronicle)』**의 108종 요괴, NPC, 타일셋 및 UI 그래픽 에셋을 생성형 AI와 자동화 도구 체인을 통해 제작하고 1.44MB 바이너리에 패킹하는 표준 운영 절차(SOP)를 정의합니다.

---

## 1. 핵심 설계 철학 (8:2 분업 및 무손실 직렬화)

```
[1. AI 초안 생성] ────> [2. Aseprite 검수] ────> [3. CLI 자동 양자화] ────> [4. 4-bit RLE C++ 헤더] ────> [5. 런타임 렌더링]
(PixelLab/RetroDiff)     (외곽선·고증 보정)        (CIELAB 오방색 매핑)        (tools/yokai_asset_pipeline)    (Win32/Direct2D)
```

* **AI의 역할 (80%):** 고전 민담/전승 기반 108종 요괴의 다각도 시안, 전투 포즈, 4방향 모션 시트의 초고속 생성.
* **인간 & 스크립트의 역할 (20%):** Aseprite에서 전승 복식/장신구 고증 검수, CIELAB 16색 오방색 강제 양자화, 4-bit RLE 압축을 통한 극단적 용량 최적화.

---

## 2. 5단계 표준 작업 절차 (SOP)

### Step 1: 요괴 프롬프트 추출 (`data/yokai_ai_prompts.json`)
* `data/yokai_ai_prompts.json`에서 대상 요괴의 도감 번호(ID)를 검색합니다.
* 해당 요괴의 `pixel_lab_prompt`, `retro_diffusion_prompt`, `negative_prompt`를 복사합니다.

### Step 2: AI 픽셀아트 생성 (PixelLab / RetroDiffusion)
* **PixelLab 추천 설정**:
  - Model: `PixFlux` 또는 `BitForge`
  - Mode: `Side View Battle Stance` (전투 48x48) 또는 `4-Directional Top-Down` (필드 16x24)
  - Grid Snap: `Enabled` (Pixel Perfect 모드 활성화)
* **RetroDiffusion (Aseprite Extension) 추천 설정**:
  - Canvas: 48x48 (전투) / 16x24 (필드)
  - Model: `RD Plus` / `RD Tile`
  - Denoising Strength: 0.4 ~ 0.6 (img2img 보정 시)

### Step 3: Aseprite 검수 및 수동 디테일링 (Human-in-the-Loop)
* AI 결과물을 Aseprite에 로드 후 다음 항목을 점검합니다:
  1. **믹셀(Mixels) 및 안티앨리어싱 제거:** 1픽셀 단위의 선명한 경계 유지.
  2. **조선 전승 고증 추가:** 갓, 상투, 도포, 호패, 짚신, 부적, 도깨비 방망이 등 고유 소품 정돈.
  3. **1-Bit 실루엣 가독성:** 단색 실루엣으로 전환 시 요괴의 특징이 즉시 식별되는지 확인.

### Step 4: 파이썬 CLI 자동화 배치 실행
* 완성된 원본 스프라이트들을 프로젝트 표준 오방색/단청 16색 팔레트로 자동 양자화하고 C++ 코드로 빌드합니다.

```bash
# 1. 단일 이미지 색상 양자화 및 노이즈 정리
python tools/palette_quantizer.py raw_dokkaebi.png -o assets/sprites/dokkaebi_quant.png -p dancheong16 --cleanup

# 2. 폴더 내 모든 에셋을 4-bit RLE 압축 C++ 헤더로 일괄 변환
python tools/yokai_asset_pipeline.py -i assets/raw_sprites/ -o src/core/yokai_assets_gen.hpp -p dancheong16 -b 4
```

### Step 5: 게임 빌드 및 인게임 검증
```bash
# C++ 프로젝트 빌드 및 테스트 스위트 검증
cmake --build build --config Release
.\build\Release\run_tests.exe
```

---

## 3. 내장 툴체인 기능 및 CLI 레퍼런스

| 도구 파일 | 주요 기능 | 지원 옵션 |
| :--- | :--- | :--- |
| `tools/palette_quantizer.py` | CIELAB $\Delta E$ 기반 인지적 색상 양자화, 고립 픽셀 노이즈 제거 | `-p [dmg4\|dancheong16\|extended32]`, `--cleanup`, `--resize W H` |
| `tools/yokai_asset_pipeline.py` | 2bpp/4bpp 비트 패킹, RLE 압축, C++ `constexpr` 헤더 자동 생성 | `-i [input]`, `-o [header.hpp]`, `-b [2\|4]`, `--test` |
| `tools/generate_yokai_prompt_db.py` | 108종 요괴 전승 AI 프롬프트 데이터베이스 생성기 | `python tools/generate_yokai_prompt_db.py` |

---

## 4. 1.44MB 용량 절감 분석 (4-Bit RLE vs 원시 PNG)

| 에셋 종류 | 규격 | 비트 심도 | 압축 전 크기 | 4-bit RLE 압축 크기 | 108종 총 용량 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **필드 SD 캐릭터** | 16x24 (8프레임) | 2bpp / 4bpp | 3,072 B | **~420 B** | **~45 KB** |
| **전투 요괴 스프라이트** | 48x48 (1프레임) | 4bpp | 1,152 B | **~360 B** | **~39 KB** |
| **보스/컷신 포트레이트** | 64x64 (단일) | 4bpp | 2,048 B | **~680 B** | **~15 KB** |
| **합계 (전체 108종)** | - | - | - | - | **~99 KB** |

> **용량 판정:** 전체 108종 요괴 그래픽의 총 용량이 **약 99 KB**로, 전체 1.44MB 예산 중 그래픽 할당량(600 KB)의 **16.5%만 점유**하여 최적화 목표를 완벽히 초과 달성합니다.
