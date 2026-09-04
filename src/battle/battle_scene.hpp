#pragma once
#include "../core/scene.hpp"
#include "../core/types.hpp"
#include "../gameplay/party.hpp"
#include "../gameplay/artifact.hpp"
#include "../battle/battle.hpp"
#include "../battle/battle_sequencer.hpp"
#include "../battle/skill_fx_system.hpp"
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

    const SkillFxSystem& getSkillFxSystem() const { return m_skillFx; }

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
    SkillFxSystem m_skillFx;

    // Smooth HUD Bars
    UISmoothBar m_playerHpBar;
    UISmoothBar m_playerQiBar;
    UISmoothBar m_enemyHpBar;

    int m_enemySpriteId = 1;
    float m_battleAnimTimer = 0.0f;
    float m_introSlideTimer = 0.5f;
    float m_playerLunge = 0.0f;
    float m_enemyLunge = 0.0f;
    float m_playerShake = 0.0f;
    float m_enemyShake = 0.0f;

    // 3-Stage Talisman Capture Animation State
    enum class CaptureAnimState { None, Throwing, Shaking, SuccessBurst, Breakout };
    CaptureAnimState m_captureAnimState = CaptureAnimState::None;
    float m_captureAnimTimer = 0.0f;
    int m_currentShakeCount = 0;
    int m_targetShakeCount = 3;
    bool m_captureWillSucceed = false;
    float m_talismanX = 0.0f;
    float m_talismanY = 0.0f;

    struct CaptureParticle {
        float x = 0.0f, y = 0.0f;
        float vx = 0.0f, vy = 0.0f;
        float life = 0.0f, maxLife = 0.5f;
        Color color;
    };
    std::vector<CaptureParticle> m_captureParticles;
    void spawnCaptureBurst(int cx, int cy, bool isGold);
    void updateCaptureAnimation(float dt);
    void renderTalismanCapture(Renderer& renderer);
};

} // namespace JoseonRPG
