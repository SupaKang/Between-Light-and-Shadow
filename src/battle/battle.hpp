#pragma once
#include "../core/types.hpp"
#include "../gameplay/yokai.hpp"
#include "../gameplay/party.hpp"
#include "../gameplay/artifact.hpp"
#include <string>
#include <vector>

namespace JoseonRPG {

enum class BattleState {
    Start,
    PlayerCommand,
    TurnResolution,
    CaptureResolution,
    Victory,
    Defeat
};

enum class PlayerAction {
    Skill0,
    Skill1,
    Skill2,
    Skill3,
    Contract,
    SwapYokai,
    Flee
};

class Battle {
public:
    Battle(Party& playerParty, Yokai wildYokai, ArtifactInventory& artifacts);

    void update();
    void selectAction(PlayerAction action, int targetParam = 0);

    BattleState getState() const { return m_state; }
    const Yokai* getActivePlayerYokai() const;
    const Yokai& getEnemyYokai() const { return m_enemyYokai; }
    const std::vector<std::string>& getCombatLog() const { return m_combatLog; }
    
    // Capture probability calculation
    float calculateCaptureProbability() const;
    bool attemptCapture();

    // Damage formula
    int calculateDamage(const Yokai& attacker, const Yokai& defender, const Skill& skill, bool isPlayerAttacker);

private:
    void executeTurn(PlayerAction playerAction, int targetParam);
    void executeEnemyTurn();

    Party& m_playerParty;
    Yokai m_enemyYokai;
    ArtifactInventory& m_artifacts;

    BattleState m_state = BattleState::PlayerCommand;
    std::vector<std::string> m_combatLog;
    int m_lastUsedPlayerSkill = -1;
    int m_lastUsedEnemySkill = -1;
};

} // namespace JoseonRPG
