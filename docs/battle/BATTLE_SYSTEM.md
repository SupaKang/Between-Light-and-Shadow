# BATTLE SYSTEM (1v1 턴제 전투 시스템 명세)

## 1. 전투 플로우
1. **진입 (Encounter)**: 필드에서 야생 요괴 조우 또는 음양당 술사와 조우
2. **선공권 계산**:
   $$\text{Initiative} = \text{Yokai.SPD} + \text{RNG}(-2, +2)$$
3. **명령 입력 단계 (Command Phase)**:
   - **기술 (Skill)**: 4개 기술 중 1개 선택 (영력 소모)
   - **계약 (Contract)**: 야생 요괴를 향해 봉인 부적 투척
   - **교체 (Swap)**: 파티 내 생존한 다른 요괴(최대 3마리)와 교체 (1턴 소모)
   - **도망 (Flee)**: 야생 전투 이탈 시도 (보스전 불가)
4. **턴 실행 단계 (Execution Phase)**:
   - 선공 요괴 행동 ➔ 상태이상 판정 (화상/마비/빙결 체크) ➔ 데미지/효과 적용
   - 후공 요괴 행동 (생존 시) ➔ 상태이상 판정 ➔ 데미지/효과 적용
5. **턴 종료 단계 (End of Turn)**:
   - 턴 수 카운트다운, 상태이상 턴 감소, 패시브 재생 및 도트 틱 적용
6. **결과 판정 (Resolution)**:
   - 아군 전원 기절 ➔ 주막 패배 후 부활
   - 적 기절 / 계약 성공 ➔ 경험치, 엽전, 유물 드랍 획득

## 2. 수식 체계

### 2.1 데미지 계산식
$$\text{BaseDamage} = \left( \frac{\text{Attacker.ATK} \times \text{Skill.Power}}{\text{Defender.DEF} \times 2} \right)$$
$$\text{FinalDamage} = \text{BaseDamage} \times \text{CritMultiplier} \times \text{FearModifier} \times \text{ArtifactBonus}$$

- 기본 최소 데미지: 1 (DEF가 아무리 높아도 최소 1 데미지 보장)
- 치명타(Crit): 1.5배 피해 (기본 확률 5%)

### 2.2 계약(포획) 성공률 공식
$$\text{Rate} = \left( 1.0 - \frac{\text{CurrentHP}}{\text{MaxHP}} \right) \times 0.60 + (\text{HasStatusEffect} ? 0.25 : 0.0) + \text{GradeModifier}$$
- `GradeModifier`:
  - Ⅰ등급: +0.15
  - Ⅱ등급: +0.05
  - Ⅲ등급: -0.05
  - Ⅳ등급: -0.15
  - Ⅴ등급: -0.30
