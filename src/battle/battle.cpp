#include "battle.hpp"
#include <random>
#include <algorithm>

namespace JoseonRPG {

static std::mt19937 s_battleRng(1337);

Battle::Battle(Party& playerParty, Yokai wildYokai, ArtifactInventory& artifacts)
    : m_playerParty(playerParty), m_enemyYokai(std::move(wildYokai)), m_artifacts(artifacts) {
    m_combatLog.push_back("A wild " + m_enemyYokai.getName() + " appeared!");
}

const Yokai* Battle::getActivePlayerYokai() const {
    return m_playerParty.getActiveYokai();
}

float Battle::calculateCaptureProbability() const {
    float hpRatio = static_cast<float>(m_enemyYokai.getStats().hp) / m_enemyYokai.getStats().maxHp;
    float baseChance = (1.0f - hpRatio) * 0.60f; // Up to 60% from weakening

    // Status effect bonus (+25%)
    if (m_enemyYokai.getStatus().effect != StatusEffect::None) {
        baseChance += 0.25f;
    }

    // Grade modifier
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

bool Battle::attemptCapture() {
    float prob = calculateCaptureProbability();
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    bool success = dist(s_battleRng) <= prob;

    if (success) {
        m_combatLog.push_back("Contract Successful! " + m_enemyYokai.getName() + " joined you!");
        m_playerParty.addYokai(m_enemyYokai);
        m_state = BattleState::Victory;
    } else {
        m_combatLog.push_back("The contract talisman failed! " + m_enemyYokai.getName() + " broke free!");
    }
    return success;
}

int Battle::calculateDamage(const Yokai& attacker, const Yokai& defender, const Skill& skill, bool isPlayerAttacker) {
    int effectiveAtk = attacker.getStats().atk;
    int effectiveDef = defender.getStats().def;

    if (isPlayerAttacker) {
        effectiveAtk = static_cast<int>(effectiveAtk * m_artifacts.getMagicAtkMultiplier());
    } else {
        effectiveDef = static_cast<int>(effectiveDef * m_artifacts.getDefMultiplier());
    }

    // Standard formula: (ATK * Skill.Power) / (DEF * 2)
    float baseDmg = (static_cast<float>(effectiveAtk * skill.power) / std::max(1.0f, static_cast<float>(effectiveDef * 2)));

    // Artifact damage reduction on player receiving damage
    if (!isPlayerAttacker) {
        baseDmg *= m_artifacts.getDamageReductionMultiplier();
    }

    int finalDmg = std::max(1, static_cast<int>(baseDmg));
    return finalDmg;
}

void Battle::selectAction(PlayerAction action, int targetParam) {
    if (m_state != BattleState::PlayerCommand) return;
    executeTurn(action, targetParam);
}

void Battle::executeTurn(PlayerAction playerAction, int targetParam) {
    Yokai* playerYokai = m_playerParty.getActiveYokai();
    if (!playerYokai || playerYokai->isFainted()) {
        m_state = BattleState::Defeat;
        return;
    }

    if (playerAction == PlayerAction::Contract) {
        if (!attemptCapture()) {
            executeEnemyTurn();
        }
        return;
    }

    if (playerAction == PlayerAction::SwapYokai) {
        m_playerParty.swapYokai(0, targetParam);
        m_combatLog.push_back("Swapped active Yokai!");
        executeEnemyTurn();
        return;
    }

    if (playerAction == PlayerAction::Flee) {
        m_combatLog.push_back("Escaped safely!");
        m_state = BattleState::Victory;
        return;
    }

    // Skill execution
    int skillIdx = static_cast<int>(playerAction);
    const auto& skills = playerYokai->getSkills();
    if (skillIdx >= 0 && skillIdx < static_cast<int>(skills.size())) {
        const Skill& skill = skills[skillIdx];
        
        // Sealed check
        if (playerYokai->getStatus().effect == StatusEffect::Seal &&
            playerYokai->getStatus().sealedSkillIndex == skillIdx) {
            m_combatLog.push_back("This skill is [SEALED] and cannot be used!");
            return;
        }

        std::string turnLog;
        if (!StatusEffectSystem::onTurnStart(*playerYokai, turnLog)) {
            m_combatLog.push_back(turnLog);
        } else {
            // Player attacks enemy
            m_combatLog.push_back(playerYokai->getName() + " uses " + skill.name + "!");
            StatusEffectSystem::onActionUsed(*playerYokai, true, turnLog);
            if (!turnLog.empty()) m_combatLog.push_back(turnLog);

            int dmg = calculateDamage(*playerYokai, m_enemyYokai, skill, true);
            dmg = StatusEffectSystem::onDamageReceived(m_enemyYokai, dmg, turnLog);
            if (!turnLog.empty()) m_combatLog.push_back(turnLog);

            m_enemyYokai.takeDamage(dmg);
            m_combatLog.push_back("Dealt " + std::to_string(dmg) + " damage!");

            // Apply status if triggered
            if (skill.statusEffect != StatusEffect::None) {
                std::uniform_int_distribution<int> dist(1, 100);
                if (dist(s_battleRng) <= skill.statusChance) {
                    StatusEffectSystem::applyStatus(m_enemyYokai, skill.statusEffect, 3, m_lastUsedEnemySkill);
                    m_combatLog.push_back("Inflicted [" + std::string(StatusEffectSystem::getStatusName(skill.statusEffect)) + "]!");
                }
            }

            m_lastUsedPlayerSkill = skillIdx;
        }

        if (m_enemyYokai.isFainted()) {
            m_combatLog.push_back("Enemy " + m_enemyYokai.getName() + " was subdued!");
            playerYokai->gainExp(100);
            m_state = BattleState::Victory;
            return;
        }
    }

    executeEnemyTurn();
}

void Battle::executeEnemyTurn() {
    Yokai* playerYokai = m_playerParty.getActiveYokai();
    if (!playerYokai) return;

    std::string turnLog;
    if (!StatusEffectSystem::onTurnStart(m_enemyYokai, turnLog)) {
        m_combatLog.push_back(turnLog);
        return;
    }

    const auto& enemySkills = m_enemyYokai.getSkills();
    if (!enemySkills.empty()) {
        std::uniform_int_distribution<size_t> dist(0, enemySkills.size() - 1);
        size_t chosenIdx = dist(s_battleRng);
        const Skill& skill = enemySkills[chosenIdx];

        m_combatLog.push_back("Enemy " + m_enemyYokai.getName() + " uses " + skill.name + "!");
        StatusEffectSystem::onActionUsed(m_enemyYokai, true, turnLog);
        if (!turnLog.empty()) m_combatLog.push_back(turnLog);

        int dmg = calculateDamage(m_enemyYokai, *playerYokai, skill, false);
        dmg = StatusEffectSystem::onDamageReceived(*playerYokai, dmg, turnLog);
        if (!turnLog.empty()) m_combatLog.push_back(turnLog);

        playerYokai->takeDamage(dmg);
        m_combatLog.push_back("Took " + std::to_string(dmg) + " damage!");

        // Status effect chance
        if (skill.statusEffect != StatusEffect::None) {
            // Check burn immunity from artifact
            if (skill.statusEffect == StatusEffect::Burn && m_artifacts.hasBurnImmunity()) {
                m_combatLog.push_back("Burn prevented by Artifact effect!");
            } else {
                std::uniform_int_distribution<int> statusRoll(1, 100);
                if (statusRoll(s_battleRng) <= skill.statusChance) {
                    StatusEffectSystem::applyStatus(*playerYokai, skill.statusEffect, 3, m_lastUsedPlayerSkill);
                    m_combatLog.push_back("Afflicted with [" + std::string(StatusEffectSystem::getStatusName(skill.statusEffect)) + "]!");
                }
            }
        }
        m_lastUsedEnemySkill = static_cast<int>(chosenIdx);
    }

    if (playerYokai->isFainted()) {
        m_combatLog.push_back(playerYokai->getName() + " fainted!");
        if (m_playerParty.isAllFainted()) {
            m_combatLog.push_back("All party members fainted...");
            m_state = BattleState::Defeat;
        }
    }
}

} // namespace JoseonRPG
