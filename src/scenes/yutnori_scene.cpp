#include "yutnori_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/scene_stack.hpp"
#include "../ui/font_renderer.hpp"
#include "../audio/audio_engine.hpp"
#include <algorithm>

namespace JoseonRPG {

YutnoriScene::YutnoriScene(int& playerMoney)
    : m_playerMoney(playerMoney), m_rng(2026) {}

void YutnoriScene::onEnter() {
    m_playerScore = 0;
    m_aiScore = 0;
    m_isPlayerTurn = true;
    m_gameOver = false;
    m_gameMessage = "윷놀이 판돈: 100냥 | [Z/Space] 던지기";
    m_lastYutName = "-";
}

void YutnoriScene::throwYut() {
    if (m_gameOver || !m_isPlayerTurn) return;

    // Deduct entry bet if starting
    if (m_playerScore == 0 && m_aiScore == 0 && m_playerMoney >= 100) {
        m_playerMoney -= 100;
    }

    AudioEngine::playSfx(SfxId::HitPhysical);

    int flatCount = 0;
    std::uniform_int_distribution<int> coin(0, 1);
    for (int i = 0; i < 4; ++i) {
        m_yutSticks[i] = (coin(m_rng) == 1);
        if (m_yutSticks[i]) flatCount++;
    }

    int steps = 0;
    bool extraThrow = false;

    if (flatCount == 1) {
        // 15% chance of Back-Do if stick 0 is flat
        if (m_yutSticks[0]) {
            m_lastYutName = "빽도! (-1칸)";
            steps = -1;
        } else {
            m_lastYutName = "도! (+1칸)";
            steps = 1;
        }
    } else if (flatCount == 2) {
        m_lastYutName = "개! (+2칸)";
        steps = 2;
    } else if (flatCount == 3) {
        m_lastYutName = "걸! (+3칸)";
        steps = 3;
    } else if (flatCount == 4) {
        m_lastYutName = "윷! (+4칸 & 한번 더!)";
        steps = 4;
        extraThrow = true;
    } else if (flatCount == 0) {
        m_lastYutName = "모! (+5칸 & 한번 더!)";
        steps = 5;
        extraThrow = true;
    }

    m_playerScore = std::max(0, m_playerScore + steps);
    m_gameMessage = "플레이어: " + m_lastYutName;

    checkGameEnd();

    if (!m_gameOver) {
        if (extraThrow) {
            m_gameMessage += " -> 추가 던지기 기회!";
        } else {
            m_isPlayerTurn = false;
        }
    }
}

void YutnoriScene::aiTurn() {
    if (m_gameOver || m_isPlayerTurn) return;

    int flatCount = 0;
    std::uniform_int_distribution<int> coin(0, 1);
    for (int i = 0; i < 4; ++i) {
        m_yutSticks[i] = (coin(m_rng) == 1);
        if (m_yutSticks[i]) flatCount++;
    }

    int steps = 0;
    bool extraThrow = false;

    if (flatCount == 1) {
        m_lastYutName = "도! (+1칸)";
        steps = 1;
    } else if (flatCount == 2) {
        m_lastYutName = "개! (+2칸)";
        steps = 2;
    } else if (flatCount == 3) {
        m_lastYutName = "걸! (+3칸)";
        steps = 3;
    } else if (flatCount == 4) {
        m_lastYutName = "윷! (+4칸 & 한번 더!)";
        steps = 4;
        extraThrow = true;
    } else if (flatCount == 0) {
        m_lastYutName = "모! (+5칸 & 한번 더!)";
        steps = 5;
        extraThrow = true;
    }

    m_aiScore = std::max(0, m_aiScore + steps);
    m_gameMessage = "주모 월선: " + m_lastYutName;

    checkGameEnd();

    if (!m_gameOver) {
        if (!extraThrow) {
            m_isPlayerTurn = true;
        }
    }
}

void YutnoriScene::checkGameEnd() {
    if (m_playerScore >= 20) {
        m_gameOver = true;
        m_playerMoney += 300;
        AudioEngine::playSfx(SfxId::LevelUp);
        m_gameMessage = "★ 윷놀이 승리! 상금 300냥을 획득하였습니다! ★";
    } else if (m_aiScore >= 20) {
        m_gameOver = true;
        AudioEngine::playSfx(SfxId::MenuCancel);
        m_gameMessage = "주모 월선 승리! 다음 기회에 도전하십시오...";
    }
}

void YutnoriScene::handleInput() {
    if (Input::isPressed(Key::ActionA)) {
        if (m_gameOver) {
            if (m_sceneStack) m_sceneStack->popScene();
        } else if (m_isPlayerTurn) {
            throwYut();
        } else {
            aiTurn();
        }
    }

    if (Input::isPressed(Key::ActionB)) {
        if (m_sceneStack) m_sceneStack->popScene();
    }
}

void YutnoriScene::update(float dt) {
    m_animTimer += dt;
}

void YutnoriScene::render(Renderer& renderer) {
    // Backdrop
    renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(24, 20, 16));

    // Yut Board Panel (Left side)
    renderer.drawPanel(8, 8, 160, 140, Color(40, 32, 24), Palette::Yellow);
    FontRenderer::drawText(renderer, 44, 14, "조선 전통 윷놀이판", Palette::Yellow);

    // Draw 20-node board tracks
    constexpr int BX = 24;
    constexpr int BY = 32;
    constexpr int BW = 128;
    constexpr int BH = 104;

    renderer.drawRect(BX, BY, BW, BH, Palette::MidGray);
    renderer.drawLine(BX, BY, BX + BW, BY + BH, Palette::DarkGray);
    renderer.drawLine(BX + BW, BY, BX, BY + BH, Palette::DarkGray);

    // Player position token (Red)
    float pRatio = std::min(1.0f, static_cast<float>(m_playerScore) / 20.0f);
    int pX = BX + static_cast<int>(pRatio * (BW - 8));
    int pY = BY + BH - 6;
    renderer.fillRect(pX, pY, 6, 6, Palette::Red);

    // AI position token (Blue)
    float aiRatio = std::min(1.0f, static_cast<float>(m_aiScore) / 20.0f);
    int aiX = BX + static_cast<int>(aiRatio * (BW - 8));
    int aiY = BY + 2;
    renderer.fillRect(aiX, aiY, 6, 6, Palette::Blue);

    // Right Side: 4 Yut Sticks Display
    renderer.drawPanel(174, 8, 138, 140, Color(32, 26, 20), Palette::MidGray);
    FontRenderer::drawText(renderer, 210, 14, "윷가락 (4개)", Palette::White);

    for (int i = 0; i < 4; ++i) {
        int yx = 186 + i * 30;
        int yy = 36;
        bool isFlat = m_yutSticks[i];

        if (isFlat) {
            // Flat belly (white wood with inscribed Xs)
            renderer.fillRect(yx, yy, 16, 50, Color(240, 230, 210));
            renderer.drawRect(yx, yy, 16, 50, Color(80, 50, 30));
            FontRenderer::drawText(renderer, yx + 4, yy + 20, "X", Palette::DarkGray);
        } else {
            // Round bark (dark chestnut wood)
            renderer.fillRect(yx, yy, 16, 50, Color(100, 60, 30));
            renderer.drawRect(yx, yy, 16, 50, Color(40, 20, 10));
            renderer.drawLine(yx + 4, yy, yx + 4, yy + 50, Color(130, 80, 40));
        }
    }

    // Score status
    FontRenderer::drawText(renderer, 180, 96, "플레이어 말: " + std::to_string(m_playerScore) + "/20칸", Palette::Red);
    FontRenderer::drawText(renderer, 180, 110, "주모 월선 말: " + std::to_string(m_aiScore) + "/20칸", Palette::Blue);
    FontRenderer::drawText(renderer, 180, 126, "보유 엽전: " + std::to_string(m_playerMoney) + "냥", Palette::Yellow);

    // Bottom Message Panel
    renderer.drawPanel(8, 152, 304, 24, Color(16, 16, 20), Palette::MidGray);
    FontRenderer::drawText(renderer, 14, 158, m_gameMessage, Palette::White);
    FontRenderer::drawText(renderer, 250, 158, "[X: 나가기]", Palette::LightGray);
}

} // namespace JoseonRPG
