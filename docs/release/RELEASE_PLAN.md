# RELEASE PLAN (릴리즈 및 배포 계획)

## 1. 릴리즈 목표 바이너리 형태
- **파일명**: `JoseonYokaiRPG.exe` (또는 `joseon_yokai_rpg.exe`)
- **형태**: 단일 독립 실행 파일 (Single Standalone Executable)
- **크기**: 고정 제한 없음. Release 패키지 크기 측정
- **설치 방식**: No-Install (다운로드 후 즉시 더블클릭 실행)

## 2. 품질 검증 게이트 (Quality Gates)
1. **용량 게이트**: 비정상적인 크기 증가와 누락된 의존성을 검토할 것.
2. **독립성 게이트**: VC++ 재배포 패키지나 DirectX 등 추가 설치 없이 순수 Windows 10/11 기본 환경에서 바로 실행될 것.
3. **완결성 게이트**: 108종 요괴 전종 도감 수집 및 음양당 메인 캠페인 엔딩까지 버그 없이 진행 가능할 것.
