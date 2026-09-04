#include "battle.hpp"
#include "../data/data_manager.hpp"
#include "../audio/audio_engine.hpp"
#include <random>
#include <algorithm>

namespace JoseonRPG {

static std::mt19937 s_battleRng(2026);

Battle::Battle(Party& playerParty, Yokai wildYokai, ArtifactInventory& artifacts)
    : m_playerParty(playerParty), m_enemyYokai(std::move(wildYokai)), m_artifacts(artifacts) {
    m_expReward = static_cast<int>(m_enemyYokai.getLevel() * 35 * static_cast<int>(m_enemyYokai.getGrade()));
    DataManager::getEncyclopedia().markSeen(m_enemyYokai.getId());
    m_combatLog.push_back("야생의 [" + m_enemyYokai.getName() + "] 조우!");

    // FoxCharm Trait Check on battle start
    Yokai* playerYokai = getActivePlayerYokai();
    if (playerYokai && playerYokai->getTrait() == YokaiTrait::FoxCharm) {
        std::uniform_int_distribution<int> roll(1, 100);
        if (roll(s_battleRng) <= 30) {
            StatusEffectSystem::applyStatus(m_enemyYokai, StatusEffect::Fear, 2);
            m_combatLog.push_back("[" + playerYokai->getName() + "]의 [구미호의 매혹] 발동! 적이 공포에 빠집니다.");
        }
    }
    m_combatLog.push_back("행동을 선택하십시오.");
}

Yokai* Battle::getActivePlayerYokai() {
    return m_playerParty.getActiveYokai();
}

const Yokai* Battle::getActivePlayerYokai() const {
    return m_playerParty.getActiveYokai();
}

void Battle::update() {
    // Battle logic update tick if needed
}

void Battle::onNavigateUp() {
    if (m_state != BattleState::PlayerCommand) return;
    if (m_menuState == BattleMenuState::MainAction) {
        if (m_mainCursor >= 2) m_mainCursor -= 2;
    } else if (m_menuState == BattleMenuState::SkillSelect) {
        if (m_skillCursor >= 2) m_skillCursor -= 2;
    } else if (m_menuState == BattleMenuState::PartySwapSelect) {
        if (m_swapCursor > 0) m_swapCursor--;
    }
}

void Battle::onNavigateDown() {
    if (m_state != BattleState::PlayerCommand) return;
    if (m_menuState == BattleMenuState::MainAction) {
        if (m_mainCursor <= 1) m_mainCursor += 2;
    } else if (m_menuState == BattleMenuState::SkillSelect) {
        Yokai* playerYokai = getActivePlayerYokai();
        if (playerYokai && m_skillCursor + 2 < static_cast<int>(playerYokai->getSkills().size())) {
            m_skillCursor += 2;
        }
    } else if (m_menuState == BattleMenuState::PartySwapSelect) {
        if (m_swapCursor + 1 < static_cast<int>(m_playerParty.getSize())) {
            m_swapCursor++;
        }
    }
}

void Battle::onNavigateLeft() {
    if (m_state != BattleState::PlayerCommand) return;
    if (m_menuState == BattleMenuState::MainAction) {
        if (m_mainCursor % 2 == 1) m_mainCursor--;
    } else if (m_menuState == BattleMenuState::SkillSelect) {
        if (m_skillCursor % 2 == 1) m_skillCursor--;
    }
}

void Battle::onNavigateRight() {
    if (m_state != BattleState::PlayerCommand) return;
    if (m_menuState == BattleMenuState::MainAction) {
        if (m_mainCursor % 2 == 0) m_mainCursor++;
    } else if (m_menuState == BattleMenuState::SkillSelect) {
        Yokai* playerYokai = getActivePlayerYokai();
        if (playerYokai && m_skillCursor + 1 < static_cast<int>(playerYokai->getSkills().size())) {
            m_skillCursor++;
        }
    }
}

void Battle::onConfirm() {
    if (m_state == BattleState::Victory || m_state == BattleState::Defeat) {
        return;
    }

    if (m_menuState == BattleMenuState::MainAction) {
        switch (m_mainCursor) {
            case 0: // 기술 선택
                m_menuState = BattleMenuState::SkillSelect;
                m_skillCursor = 0;
                break;
            case 1: // 계약 시도
                executePlayerCapture();
                break;
            case 2: // 요괴 교체
                m_menuState = BattleMenuState::PartySwapSelect;
                m_swapCursor = 0;
                break;
            case 3: // 도망
                executePlayerFlee();
                break;
        }
    } else if (m_menuState == BattleMenuState::SkillSelect) {
        executePlayerSkill(m_skillCursor);
    } else if (m_menuState == BattleMenuState::PartySwapSelect) {
        executePlayerSwap(m_swapCursor);
    }
}

void Battle::onCancel() {
    if (m_menuState == BattleMenuState::SkillSelect || m_menuState == BattleMenuState::PartySwapSelect) {
        m_menuState = BattleMenuState::MainAction;
    }
}

float Battle::calculateCaptureProbability() const {
    float hpRatio = static_cast<float>(m_enemyYokai.getStats().hp) / std::max(1, m_enemyYokai.getStats().maxHp);
    float baseChance = (1.0f - hpRatio) * 0.60f; // Up to 60% based on remaining HP

    // Status Effect Bonus (+25%)
    if (m_enemyYokai.getStatus().effect != StatusEffect::None) {
        baseChance += 0.25f;
    }

    // Grade Modifier
    float gradeMod = 0.0f;
    switch (m_enemyYokai.getGrade()) {
        case YokaiGrade::Grade1: gradeMod = 0.15f; break;
        case YokaiGrade::Grade2: gradeMod = 0.05f; break;
        case YokaiGrade::Grade3: gradeMod = -0.05f; break;
        case YokaiGrade::Grade4: gradeMod = -0.15f; break;
        case YokaiGrade::Grade5: gradeMod = -0.30f; break;
    }

    return std::clamp(baseChance + gradeMod, 0.05f, 0.95f);
}

int Battle::calculateDamage(const Yokai& attacker, const Yokai& defender, const Skill& skill, bool isPlayerAttacker) {
    int effectiveAtk = attacker.getStats().atk;
    int effectiveDef = defender.getStats().def;

    if (isPlayerAttacker) {
        effectiveAtk = static_cast<int>(effectiveAtk * m_artifacts.getMagicAtkMultiplier());
    } else {
        effectiveDef = static_cast<int>(effectiveDef * m_artifacts.getDefMultiplier());
    }

    // Trait Buff: GrimGaze (ATK +40% when target HP <= 30%)
    if (attacker.getTrait() == YokaiTrait::GrimGaze) {
        if (defender.getStats().hp * 100 / std::max(1, defender.getStats().maxHp) <= 30) {
            effectiveAtk = static_cast<int>(effectiveAtk * 1.40f);
        }
    }

    // Damage Formula: (ATK * Skill.Power) / (DEF * 1.35)
    float baseDmg = (static_cast<float>(effectiveAtk * skill.power) / std::max(1.0f, static_cast<float>(effectiveDef * 1.35f)));

    // Trait Defense: DuduriProtection (-10% incoming damage)
    if (defender.getTrait() == YokaiTrait::DuduriProtection) {
        baseDmg *= 0.90f;
    }
    // Trait Defense: IronDiet (-15% physical damage)
    if (defender.getTrait() == YokaiTrait::IronDiet && skill.element == Element::Physical) {
        baseDmg *= 0.85f;
    }

    // Artifact bonus / reduction & Critical calculation
    if (isPlayerAttacker) {
        int critRate = 5 + m_artifacts.getCritRateBonus();
        if (attacker.getTrait() == YokaiTrait::DokkaebiPower) {
            critRate += 15; // Trait: Dokkaebi power +15%
        }
        std::uniform_int_distribution<int> critRoll(1, 100);
        if (critRoll(s_battleRng) <= critRate) {
            baseDmg *= 1.5f;
            m_combatLog.push_back(">> 치명타(Crit) 발생! <<");
        }
    } else {
        baseDmg *= m_artifacts.getDamageReductionMultiplier();
    }

    return std::max(1, static_cast<int>(baseDmg));
}

void Battle::executePlayerSkill(int skillIndex) {
    Yokai* playerYokai = getActivePlayerYokai();
    if (!playerYokai || playerYokai->isFainted()) return;

    const auto& skills = playerYokai->getSkills();
    if (skillIndex < 0 || skillIndex >= static_cast<int>(skills.size())) return;

    const Skill& skill = skills[skillIndex];

    // Qi check
    if (playerYokai->getStats().qi < skill.qiCost) {
        m_combatLog.push_back("영력(Qi)이 부족하여 시전할 수 없습니다!");
        return;
    }

    // Sealed check
    if (playerYokai->getStatus().effect == StatusEffect::Seal &&
        playerYokai->getStatus().sealedSkillIndex == skillIndex) {
        m_combatLog.push_back("해당 기술은 [봉인]되어 사용할 수 없습니다!");
        return;
    }

    TurnAction playerAct;
    playerAct.isPlayer = true;
    playerAct.skillIndex = skillIndex;
    playerAct.speed = playerYokai->getStats().spd + std::uniform_int_distribution<int>(-2, 2)(s_battleRng);

    TurnAction enemyAct = decideEnemyAction();
    resolveTurnActions(playerAct, enemyAct);
}

void Battle::executePlayerCapture() {
    TurnAction playerAct;
    playerAct.isPlayer = true;
    playerAct.isCapture = true;
    playerAct.speed = 999; // Top priority

    TurnAction enemyAct = decideEnemyAction();
    resolveTurnActions(playerAct, enemyAct);
}

void Battle::executePlayerSwap(int targetIndex) {
    if (targetIndex < 0 || targetIndex >= static_cast<int>(m_playerParty.getSize())) return;
    if (targetIndex == 0) {
        m_combatLog.push_back("이미 출전 중인 요괴입니다.");
        return;
    }

    Yokai* targetYokai = m_playerParty.getYokai(targetIndex);
    if (!targetYokai || targetYokai->isFainted()) {
        m_combatLog.push_back("기절한 요괴는 출전할 수 없습니다.");
        return;
    }

    TurnAction playerAct;
    playerAct.isPlayer = true;
    playerAct.isSwap = true;
    playerAct.swapIndex = targetIndex;
    playerAct.speed = 999; // Top priority

    TurnAction enemyAct = decideEnemyAction();
    resolveTurnActions(playerAct, enemyAct);
}

void Battle::executePlayerFlee() {
    m_combatLog.push_back("무사히 전장을 이탈했습니다.");
    m_state = BattleState::Victory;
}

TurnAction Battle::decideEnemyAction() {
    TurnAction act;
    act.isPlayer = false;
    act.speed = m_enemyYokai.getStats().spd + std::uniform_int_distribution<int>(-2, 2)(s_battleRng);

    const auto& enemySkills = m_enemyYokai.getSkills();
    if (!enemySkills.empty()) {
        std::uniform_int_distribution<int> dist(0, static_cast<int>(enemySkills.size()) - 1);
        act.skillIndex = dist(s_battleRng);
    }
    return act;
}

void Battle::resolveTurnActions(const TurnAction& playerAction, const TurnAction& enemyAction) {
    m_menuState = BattleMenuState::MainAction;

    // Handle Artifact Qi drain and HolyAura at turn start
    Yokai* playerYokai = getActivePlayerYokai();
    if (playerYokai && !playerYokai->isFainted()) {
        int qiDrain = m_artifacts.getQiDrainPerTurn();
        if (qiDrain > 0) {
            playerYokai->consumeQi(qiDrain);
            m_combatLog.push_back("유물 대가로 영력 " + std::to_string(qiDrain) + " 소모.");
        }

        // Trait: HolyAura (5% HP heal)
        if (playerYokai->getTrait() == YokaiTrait::HolyAura) {
            int healAmt = std::max(1, playerYokai->getStats().maxHp * 5 / 100);
            playerYokai->healHp(healAmt);
            m_combatLog.push_back("[" + playerYokai->getName() + "]의 [벽사의 영기]로 HP +" + std::to_string(healAmt) + " 회복!");
        }
    }

    // 1. Capture Action Check
    if (playerAction.isCapture) {
        float rate = calculateCaptureProbability();
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float roll = dist(s_battleRng);

        m_lastCaptureResult.attempted = true;
        m_combatLog.push_back("벽사 봉인 부적을 던졌습니다! (성공률: " + std::to_string(static_cast<int>(rate * 100)) + "%)");

        if (roll <= rate) {
            m_lastCaptureResult.success = true;
            m_lastCaptureResult.targetShakes = 3;
            m_combatLog.push_back("계약 성공! [" + m_enemyYokai.getName() + "]과 영혼의 계약을 맺었습니다!");
            DataManager::getEncyclopedia().markCaptured(m_enemyYokai.getId());
            m_playerParty.addYokai(m_enemyYokai);
            m_state = BattleState::Victory;
            return;
        } else {
            m_lastCaptureResult.success = false;
            if (roll < rate + 0.15f) m_lastCaptureResult.targetShakes = 2;
            else if (roll < rate + 0.35f) m_lastCaptureResult.targetShakes = 1;
            else m_lastCaptureResult.targetShakes = 0;

            m_combatLog.push_back("부적이 튕겨져 나갔습니다! 계약 실패.");
            // Enemy attacks after failed capture
            if (playerYokai && !playerYokai->isFainted()) {
                performSkillAction(m_enemyYokai, *playerYokai, enemyAction.skillIndex, false);
            }
            checkBattleOutcome();
            return;
        }
    }

    // 2. Swap Action Check
    if (playerAction.isSwap) {
        AudioEngine::playSfx(SfxId::MenuSelect);
        m_playerParty.swapYokai(0, playerAction.swapIndex);
        playerYokai = getActivePlayerYokai();
        m_combatLog.push_back("가랏! [" + playerYokai->getName() + "] 교체 출전!");
        // Enemy attacks incoming swapped yokai
        if (playerYokai && !playerYokai->isFainted()) {
            performSkillAction(m_enemyYokai, *playerYokai, enemyAction.skillIndex, false);
        }
        checkBattleOutcome();
        return;
    }

    // 3. Initiative-based Turn Resolution
    bool playerFirst = (playerAction.speed >= enemyAction.speed);

    if (playerFirst) {
        // Player attacks first
        if (playerYokai && !playerYokai->isFainted()) {
            performSkillAction(*playerYokai, m_enemyYokai, playerAction.skillIndex, true);
        }
        if (m_enemyYokai.isFainted()) {
            checkBattleOutcome();
            return;
        }
        // Enemy attacks second
        if (playerYokai && !playerYokai->isFainted()) {
            performSkillAction(m_enemyYokai, *playerYokai, enemyAction.skillIndex, false);
        }
    } else {
        // Enemy attacks first
        if (playerYokai && !playerYokai->isFainted()) {
            performSkillAction(m_enemyYokai, *playerYokai, enemyAction.skillIndex, false);
        }
        if (playerYokai && playerYokai->isFainted()) {
            checkBattleOutcome();
            return;
        }
        // Player attacks second
        if (playerYokai && !playerYokai->isFainted()) {
            performSkillAction(*playerYokai, m_enemyYokai, playerAction.skillIndex, true);
        }
    }

    // End of Turn Status Decrement
    std::string dummyLog;
    if (playerYokai) StatusEffectSystem::onTurnEnd(*playerYokai, dummyLog);
    StatusEffectSystem::onTurnEnd(m_enemyYokai, dummyLog);

    checkBattleOutcome();
}

void Battle::performSkillAction(Yokai& attacker, Yokai& defender, int skillIndex, bool isPlayer) {
    const auto& skills = attacker.getSkills();
    if (skillIndex < 0 || skillIndex >= static_cast<int>(skills.size())) return;
    const Skill& skill = skills[skillIndex];

    int actualCost = skill.qiCost;
    if (attacker.getTrait() == YokaiTrait::AquaSurge && skill.element == Element::Water) {
        actualCost = std::max(1, actualCost * 75 / 100);
    }
    attacker.consumeQi(actualCost);

    m_combatLog.push_back("[" + attacker.getName() + "]의 " + skill.name + "!");

    // Accuracy Check
    std::uniform_int_distribution<int> accRoll(1, 100);
    if (accRoll(s_battleRng) > skill.accuracy) {
        m_combatLog.push_back("공격이 빗나갔습니다!");
        return;
    }

    int dmg = calculateDamage(attacker, defender, skill, isPlayer);
    defender.takeDamage(dmg);

    if (skill.element == Element::Physical) {
        AudioEngine::playSfx(SfxId::HitPhysical);
    } else {
        AudioEngine::playSfx(SfxId::HitMagic);
    }

    m_combatLog.push_back("[" + defender.getName() + "]에게 " + std::to_string(dmg) + " 피해!");

    // Trait: FlameBody check on physical contact
    if (defender.getTrait() == YokaiTrait::FlameBody && skill.element == Element::Physical) {
        std::uniform_int_distribution<int> roll(1, 100);
        if (roll(s_battleRng) <= 30 && attacker.getStatus().effect == StatusEffect::None) {
            StatusEffectSystem::applyStatus(attacker, StatusEffect::Burn, 3);
            m_combatLog.push_back("[" + defender.getName() + "]의 [타오르는 화기운]으로 화상을 입었습니다!");
        }
    }

    // Status effect apply check
    if (skill.statusEffect != StatusEffect::None && defender.getStatus().effect == StatusEffect::None) {
        std::uniform_int_distribution<int> statusRoll(1, 100);
        if (statusRoll(s_battleRng) <= skill.statusChance) {
            StatusEffectSystem::applyStatus(defender, skill.statusEffect, 3);
            AudioEngine::playSfx(SfxId::StatusAfflict);
            m_combatLog.push_back("[" + defender.getName() + "] 상태이상 부여 성공!");
        }
    }

    if (isPlayer) {
        m_lastUsedPlayerSkill = skillIndex;
    } else {
        m_lastUsedEnemySkill = skillIndex;
    }
}

void Battle::checkBattleOutcome() {
    if (m_enemyYokai.isFainted()) {
        m_combatLog.push_back("적 [" + m_enemyYokai.getName() + "]을(를) 격파했습니다!");
        m_combatLog.push_back("경험치 " + std::to_string(m_expReward) + " 획득!");
        Yokai* pYokai = getActivePlayerYokai();
        if (pYokai) {
            int prevLv = pYokai->getLevel();
            pYokai->gainExp(m_expReward);
            if (pYokai->getLevel() > prevLv) {
                m_combatLog.push_back("[" + pYokai->getName() + "] 레벨 상승! Lv." + std::to_string(pYokai->getLevel()));
            }
        }
        m_state = BattleState::Victory;
        return;
    }

    Yokai* playerYokai = getActivePlayerYokai();
    if (playerYokai && playerYokai->isFainted()) {
        m_combatLog.push_back("[" + playerYokai->getName() + "] 기절!");
        if (m_playerParty.isAllFainted()) {
            m_combatLog.push_back("파티 전원 전투 불능... 눈앞이 캄캄해집니다.");
            m_state = BattleState::Defeat;
        } else {
            m_combatLog.push_back("다른 요괴를 출전시켜야 합니다 (교체 선택).");
            m_menuState = BattleMenuState::PartySwapSelect;
        }
    }
}

} // namespace JoseonRPG
