# TECH STACK (기술 스택 및 아키텍처 명세)

## 1. 코어 기술 스택
- **언어 표준**: C++17 (`std::byte`, `std::string_view`, `std::optional`, `std::array`, 구조적 바인딩)
- **대상 OS**: Windows 10, 11 (Win32 x64/x86)
- **렌더러**: 순수 소프트웨어 프레임버퍼 (320x180x32bpp) ➔ Win32 `StretchDIBits` / GDI 하드웨어 가속 비트맵 복사
- **사운드**: Win32 WaveOut / DirectSound 미니멀 드라이버 또는 내장 프로그래머블 PSG(Programmable Sound Generator)
- **입력**: Win32 `GetAsyncKeyState` / `WM_KEYDOWN` 비동기 논블로킹 입력
- **메모리 정책**: 동적 할당 최소화, 정적 풀(Static Object Pool) 및 스택 메모리 적극 활용

## 2. 모듈 아키텍처

```text
[WinMain / Application Loop]
         │
 ┌───────┴───────┐
 ▼               ▼
[Input]       [Timer / Fixed 60FPS]
 │               │
 └───────┬───────┘
         ▼
[Game Engine State Machine]
 (Title ➔ Field ➔ Battle ➔ Menu ➔ Dialog)
         │
 ┌───────┼───────────────────────────┐
 ▼       ▼                           ▼
[World] [Battle / StatusEffects]   [Gameplay: Yokai / Party / Artifacts]
 │       │                           │
 └───────┼───────────────────────────┘
         ▼
[Software Renderer (320x180 ARGB)]
         │
         ▼
[Win32 StretchDIBits Blit to Window]
```
