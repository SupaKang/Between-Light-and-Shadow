# 콘텐츠 데이터 규칙

- 파일 하나에 요괴 또는 기술 하나만 저장한다.
- `id`는 중복 금지이며 참조 대상은 문자열 ID로 연결한다.
- `origin`은 `VERIFY_TRADITIONAL_KOREAN_FOLKLORE`, `VERIFY_FOREIGN_FOLKLORE`, `ORIGINAL` 중 하나로 분류한다.
- 확인되지 않은 전승·설정은 `VERIFY`를 포함해 표시한다.
- 요괴 레벨은 1~50, 등급은 I~V, 기술 수는 최대 4개로 제한한다.
- 수치 변경은 코드가 아니라 데이터 파일과 설계 문서를 함께 갱신한다.
- 지역별 조우는 `encounter_*.json`으로 분리하고 가중치·레벨 범위를 명시한다.
- 상태이상 수치는 `status_effects.json`에서 관리하며 핵심 상태 5종을 유지한다.
- 유물은 패시브와 디버프를 함께 기록하며 `destroyable`이 true인 경우 인벤토리에서 즉시 파괴할 수 있다.
- 전투 명령 문구와 활성 상태는 `battle_commands.json`에서 관리한다.
