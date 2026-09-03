#pragma once
#include "../core/scene.hpp"
#include "../core/types.hpp"
#include "../gameplay/party.hpp"
#include "../gameplay/artifact.hpp"
#include "../battle/battle.hpp"
#include "../battle/battle_sequencer.hpp"
#include "../ui/ui_widgets.hpp"
#include <memory>
#include <functional>

namespace JoseonRPG {

class BattleScene : public IScene {
public:
    BattleScene(Party& party, Yokai enemy, ArtifactInventory& artifacts, bool isBoss = false, std::function<void(bool won)> onBattleEnd = nullptr);

    void onEnter() override;
    void handleInput() override;
    void update(float dt) override;
    void render(Renderer& renderer) override;

    bool isModal() const override { return false; }

private:
    void setupTurnSequence(const TurnAction& playerAction, const TurnAction& enemyAction);
    void appendSkillActionToSequence(Yokai& attacker, Yokai& defender, int skillIndex, bool isPlayer);
    void appendCaptureActionToSequence();
    void appendSwapActionToSequence(int swapIndex);
    void appendFleeActionToSequence();

    Party& m_party;
    Yokai m_enemy;
    ArtifactInventory& m_artifacts;
    bool m_isBoss = false;
    std::function<void(bool won)> m_onBattleEnd = nullptr;

    std::unique_ptr<Battle> m_battle;
    BattleSequencer m_sequencer;

    // Smooth HUD Bars
    UISmoothBar m_playerHpBar;
    UISmoothBar m_playerQiBar;
    UISmoothBar m_enemyHpBar;

    int m_enemySpriteId = 1;
};

} // namespace JoseonRPG
