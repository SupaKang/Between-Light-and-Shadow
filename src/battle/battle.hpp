#pragma once
#include "../core/types.hpp"
#include "../gameplay/yokai.hpp"
#include "../gameplay/party.hpp"
#include "../gameplay/artifact.hpp"
#include <string>
#include <vector>

namespace JoseonRPG {

enum class BattleMenuState {
    MainAction,      // 0: 기술(Attack), 1: 계약(Contract), 2: 교체(Party), 3: 도망(Flee)
    SkillSelect,     // Select from 4 active skills
    PartySwapSelect  // Select from party members
};

enum class BattleState {
    Intro,
    PlayerCommand,
    ExecutingTurn,
    Victory,
    Defeat
};

struct TurnAction {
    bool isPlayer = true;
    bool isSwap = false;
    bool isCapture = false;
    int skillIndex = -1;
    int swapIndex = -1;
    int speed = 0;
};

class Battle {
public:
    Battle(Party& playerParty, Yokai wildYokai, ArtifactInventory& artifacts);

    void update();

    // Input handlers for menu navigation
    void onNavigateUp();
    void onNavigateDown();
    void onNavigateLeft();
    void onNavigateRight();
    void onConfirm();
    void onCancel();

    // Direct action execution
    void executePlayerSkill(int skillIndex);
    void executePlayerCapture();
    void executePlayerSwap(int targetIndex);
    void executePlayerFlee();

    // State getters
    BattleState getState() const { return m_state; }
    BattleMenuState getMenuState() const { return m_menuState; }
    int getMainCursor() const { return m_mainCursor; }
    int getSkillCursor() const { return m_skillCursor; }
    int getSwapCursor() const { return m_swapCursor; }

    Yokai* getActivePlayerYokai();
    const Yokai* getActivePlayerYokai() const;
    const Yokai& getEnemyYokai() const { return m_enemyYokai; }
    Yokai& getEnemyYokai() { return m_enemyYokai; }
    Party& getPlayerParty() { return m_playerParty; }
    const std::vector<std::string>& getCombatLog() const { return m_combatLog; }
    int getExpReward() const { return m_expReward; }

    // Formulas
    float calculateCaptureProbability() const;
    int calculateDamage(const Yokai& attacker, const Yokai& defender, const Skill& skill, bool isPlayerAttacker);

private:
    void resolveTurnActions(const TurnAction& playerAction, const TurnAction& enemyAction);
    void performSkillAction(Yokai& attacker, Yokai& defender, int skillIndex, bool isPlayer);
    TurnAction decideEnemyAction();
    void checkBattleOutcome();

    Party& m_playerParty;
    Yokai m_enemyYokai;
    ArtifactInventory& m_artifacts;

    BattleState m_state = BattleState::PlayerCommand;
    BattleMenuState m_menuState = BattleMenuState::MainAction;

    int m_mainCursor = 0;   // 0: Attack, 1: Contract, 2: Swap, 3: Flee
    int m_skillCursor = 0;  // 0..3
    int m_swapCursor = 0;   // 0..2

    std::vector<std::string> m_combatLog;
    int m_lastUsedPlayerSkill = -1;
    int m_lastUsedEnemySkill = -1;
    int m_expReward = 0;
};

} // namespace JoseonRPG
