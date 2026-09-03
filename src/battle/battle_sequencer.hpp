#pragma once
#include "../core/types.hpp"
#include <string>
#include <vector>
#include <deque>
#include <functional>
#include <memory>

namespace JoseonRPG {

enum class BattleCommandType {
    TextMessage,
    FlashSprite,
    SlideHpBar,
    ApplyStatus,
    ShakeCapture,
    WaitDelay,
    CustomCallback
};

struct BattleSequenceCommand {
    BattleCommandType type = BattleCommandType::TextMessage;
    std::string text;
    bool isEnemy = false;
    int intVal1 = 0;
    int intVal2 = 0;
    float floatVal = 0.0f;
    std::function<void()> callback = nullptr;
};

class BattleSequencer {
public:
    BattleSequencer() = default;

    void addTextMessage(const std::string& text);
    void addFlash(bool isEnemy, int count = 3);
    void addHpSlide(bool isEnemy, int targetHp, int maxHp);
    void addCaptureShake(int shakes, bool success);
    void addDelay(float durationSeconds);
    void addCallback(std::function<void()> callback);

    void clear();
    bool isFinished() const { return m_commands.empty() && !m_currentActive; }
    
    // Process sequencer queue
    void update(float dt);

    bool isCurrentCommandTextMessage() const;
    const std::string& getCurrentText() const { return m_currentText; }
    bool isWaitingForInput() const { return m_waitingForInput; }
    void advanceText();

    bool isEnemyFlashing() const { return m_enemyFlashing; }
    bool isPlayerFlashing() const { return m_playerFlashing; }
    int getCaptureShakeCount() const { return m_captureShakeCount; }
    bool isCaptureActive() const { return m_captureActive; }

private:
    std::deque<BattleSequenceCommand> m_commands;
    bool m_currentActive = false;
    BattleSequenceCommand m_currentCmd;

    // State for current command execution
    std::string m_currentText;
    bool m_waitingForInput = false;
    float m_cmdTimer = 0.0f;

    bool m_enemyFlashing = false;
    bool m_playerFlashing = false;
    int m_flashCounter = 0;

    bool m_captureActive = false;
    int m_captureShakeCount = 0;
};

} // namespace JoseonRPG
