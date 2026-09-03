#include "battle_sequencer.hpp"
#include <algorithm>

namespace JoseonRPG {

void BattleSequencer::addTextMessage(const std::string& text) {
    BattleSequenceCommand cmd;
    cmd.type = BattleCommandType::TextMessage;
    cmd.text = text;
    m_commands.push_back(cmd);
}

void BattleSequencer::addFlash(bool isEnemy, int count) {
    BattleSequenceCommand cmd;
    cmd.type = BattleCommandType::FlashSprite;
    cmd.isEnemy = isEnemy;
    cmd.intVal1 = count;
    m_commands.push_back(cmd);
}

void BattleSequencer::addHpSlide(bool isEnemy, int targetHp, int maxHp) {
    BattleSequenceCommand cmd;
    cmd.type = BattleCommandType::SlideHpBar;
    cmd.isEnemy = isEnemy;
    cmd.intVal1 = targetHp;
    cmd.intVal2 = maxHp;
    m_commands.push_back(cmd);
}

void BattleSequencer::addCaptureShake(int shakes, bool success) {
    BattleSequenceCommand cmd;
    cmd.type = BattleCommandType::ShakeCapture;
    cmd.intVal1 = shakes;
    cmd.intVal2 = success ? 1 : 0;
    m_commands.push_back(cmd);
}

void BattleSequencer::addDelay(float durationSeconds) {
    BattleSequenceCommand cmd;
    cmd.type = BattleCommandType::WaitDelay;
    cmd.floatVal = durationSeconds;
    m_commands.push_back(cmd);
}

void BattleSequencer::addCallback(std::function<void()> callback) {
    BattleSequenceCommand cmd;
    cmd.type = BattleCommandType::CustomCallback;
    cmd.callback = std::move(callback);
    m_commands.push_back(cmd);
}

void BattleSequencer::clear() {
    m_commands.clear();
    m_currentActive = false;
    m_currentText.clear();
    m_waitingForInput = false;
    m_enemyFlashing = false;
    m_playerFlashing = false;
    m_captureActive = false;
}

bool BattleSequencer::isCurrentCommandTextMessage() const {
    return m_currentActive && m_currentCmd.type == BattleCommandType::TextMessage;
}

void BattleSequencer::advanceText() {
    if (m_currentActive && m_currentCmd.type == BattleCommandType::TextMessage && m_waitingForInput) {
        m_waitingForInput = false;
        m_currentActive = false;
    }
}

void BattleSequencer::update(float dt) {
    if (!m_currentActive) {
        if (m_commands.empty()) return;
        m_currentCmd = m_commands.front();
        m_commands.pop_front();
        m_currentActive = true;
        m_cmdTimer = 0.0f;

        switch (m_currentCmd.type) {
            case BattleCommandType::TextMessage:
                m_currentText = m_currentCmd.text;
                m_waitingForInput = true;
                break;
            case BattleCommandType::FlashSprite:
                if (m_currentCmd.isEnemy) m_enemyFlashing = true;
                else m_playerFlashing = true;
                m_flashCounter = m_currentCmd.intVal1 * 2;
                break;
            case BattleCommandType::ShakeCapture:
                m_captureActive = true;
                m_captureShakeCount = 0;
                break;
            case BattleCommandType::CustomCallback:
                if (m_currentCmd.callback) {
                    m_currentCmd.callback();
                }
                m_currentActive = false;
                break;
            default:
                break;
        }
        return;
    }

    // Process current active command
    switch (m_currentCmd.type) {
        case BattleCommandType::TextMessage:
            // Waits until user calls advanceText()
            break;

        case BattleCommandType::FlashSprite: {
            m_cmdTimer += dt;
            if (m_cmdTimer >= 0.08f) {
                m_cmdTimer = 0.0f;
                m_flashCounter--;
                if (m_currentCmd.isEnemy) m_enemyFlashing = (m_flashCounter % 2 == 1);
                else m_playerFlashing = (m_flashCounter % 2 == 1);

                if (m_flashCounter <= 0) {
                    m_enemyFlashing = false;
                    m_playerFlashing = false;
                    m_currentActive = false;
                }
            }
            break;
        }

        case BattleCommandType::SlideHpBar:
            // Small pause for HP bar slide
            m_cmdTimer += dt;
            if (m_cmdTimer >= 0.35f) {
                m_currentActive = false;
            }
            break;

        case BattleCommandType::WaitDelay:
            m_cmdTimer += dt;
            if (m_cmdTimer >= m_currentCmd.floatVal) {
                m_currentActive = false;
            }
            break;

        case BattleCommandType::ShakeCapture:
            m_cmdTimer += dt;
            if (m_cmdTimer >= 0.4f) {
                m_cmdTimer = 0.0f;
                m_captureShakeCount++;
                if (m_captureShakeCount >= m_currentCmd.intVal1) {
                    m_captureActive = false;
                    m_currentActive = false;
                }
            }
            break;

        case BattleCommandType::CustomCallback:
            m_currentActive = false;
            break;
    }
}

} // namespace JoseonRPG
