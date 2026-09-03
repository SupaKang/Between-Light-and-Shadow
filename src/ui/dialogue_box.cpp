#include "dialogue_box.hpp"
#include "font_renderer.hpp"
#include "../core/input.hpp"
#include "../core/gen1_assets.hpp"
#include "../scenes/settings_scene.hpp"
#include <algorithm>

namespace JoseonRPG {

DialogueBox::DialogueBox() = default;

void DialogueBox::startDialogue(const std::string& speaker, const std::vector<std::string>& lines, std::function<void()> onComplete) {
    if (lines.empty()) return;

    m_speaker = speaker;
    m_lines = lines;
    m_currentLineIndex = 0;
    m_charProgress = 0.0f;
    m_isLineFullyRevealed = false;
    m_promptBlinkTimer = 0.0f;
    m_promptVisible = true;
    m_active = true;
    m_options.clear();
    m_selectedOptionIndex = 0;
    m_isChoosingOption = false;
    m_onCompleteCallback = std::move(onComplete);
    m_onOptionSelectedCallback = nullptr;
}

void DialogueBox::startDialogueWithOptions(const std::string& speaker, const std::vector<std::string>& lines, const std::vector<std::string>& options, std::function<void(int optionIndex)> onSelectOption) {
    if (lines.empty()) return;

    m_speaker = speaker;
    m_lines = lines;
    m_currentLineIndex = 0;
    m_charProgress = 0.0f;
    m_isLineFullyRevealed = false;
    m_promptBlinkTimer = 0.0f;
    m_promptVisible = true;
    m_active = true;
    m_options = options;
    m_selectedOptionIndex = 0;
    m_isChoosingOption = false;
    m_onCompleteCallback = nullptr;
    m_onOptionSelectedCallback = std::move(onSelectOption);
}

void DialogueBox::close() {
    m_active = false;
    m_lines.clear();
    m_options.clear();
    m_currentLineIndex = 0;
    m_isChoosingOption = false;
    if (m_onCompleteCallback) {
        auto cb = m_onCompleteCallback;
        m_onCompleteCallback = nullptr;
        cb();
    }
}

void DialogueBox::update(float dt) {
    if (!m_active || m_currentLineIndex >= m_lines.size()) return;

    const std::string& currentLine = m_lines[m_currentLineIndex];
    if (!m_isLineFullyRevealed) {
        int speedMode = SettingsScene::getGlobalSettings().textSpeed;
        float curSpeed = (speedMode == 0) ? 35.0f : (speedMode == 1 ? 75.0f : 9999.0f);
        m_charProgress += curSpeed * dt;
        if (m_charProgress >= static_cast<float>(currentLine.size())) {
            m_charProgress = static_cast<float>(currentLine.size());
            m_isLineFullyRevealed = true;
            if (m_currentLineIndex + 1 == m_lines.size() && !m_options.empty()) {
                m_isChoosingOption = true;
            }
        }
    } else {
        if (!m_isChoosingOption && m_currentLineIndex + 1 == m_lines.size() && !m_options.empty()) {
            m_isChoosingOption = true;
        }

        // Blink prompt indicator
        m_promptBlinkTimer += dt;
        if (m_promptBlinkTimer >= 0.35f) {
            m_promptBlinkTimer = 0.0f;
            m_promptVisible = !m_promptVisible;
        }
    }
}

bool DialogueBox::handleInput() {
    if (!m_active) return false;

    if (m_isChoosingOption) {
        if (Input::isPressed(Key::Up)) {
            if (m_selectedOptionIndex > 0) m_selectedOptionIndex--;
        }
        if (Input::isPressed(Key::Down)) {
            if (m_selectedOptionIndex + 1 < static_cast<int>(m_options.size())) m_selectedOptionIndex++;
        }
        if (Input::isPressed(Key::ActionA)) {
            int chosen = m_selectedOptionIndex;
            auto cb = m_onOptionSelectedCallback;
            close();
            if (cb) {
                cb(chosen);
            }
            return true;
        }
        return true;
    }

    if (Input::isPressed(Key::ActionA) || Input::isPressed(Key::ActionB)) {
        if (!m_isLineFullyRevealed) {
            // Instant reveal current line
            if (m_currentLineIndex < m_lines.size()) {
                m_charProgress = static_cast<float>(m_lines[m_currentLineIndex].size());
                m_isLineFullyRevealed = true;
                if (m_currentLineIndex + 1 == m_lines.size() && !m_options.empty()) {
                    m_isChoosingOption = true;
                }
            }
        } else {
            // Next line or close
            if (m_currentLineIndex + 1 < m_lines.size()) {
                m_currentLineIndex++;
                m_charProgress = 0.0f;
                m_isLineFullyRevealed = false;
            } else {
                if (!m_options.empty()) {
                    m_isChoosingOption = true;
                } else {
                    close();
                }
            }
        }
        return true;
    }

    return true; // Block other inputs while dialogue is active
}

void DialogueBox::render(Renderer& renderer) {
    if (!m_active || m_currentLineIndex >= m_lines.size()) return;

    // Main Dialogue Frame at bottom (320x180 resolution) - 1세대 GB 스타일
    int boxX = 6;
    int boxY = 112;
    int boxW = 308;
    int boxH = 62;

    renderer.fillRect(boxX, boxY, boxW, boxH, Color(224, 248, 208)); // Pale off-white DMG background
    renderer.drawRect(boxX, boxY, boxW, boxH, Color(8, 24, 32));      // Darkest DMG border
    renderer.drawRect(boxX + 2, boxY + 2, boxW - 4, boxH - 4, Color(52, 104, 86));

    // Portrait (48x48 Gen 1 Bust)
    int textStartX = boxX + 10;
    bool hasPortrait = (m_speaker.find("구미호") != std::string::npos || m_speaker.find("설화") != std::string::npos || m_speaker.find("여우") != std::string::npos);
    if (hasPortrait) {
        renderer.drawGen1Bitmap(boxX + 6, boxY + 7, 48, 48, Gen1Assets::PORTRAIT_GUMIHO_48x48, false);
        renderer.drawRect(boxX + 5, boxY + 6, 50, 50, Color(8, 24, 32));
        textStartX = boxX + 62;
    }

    // Speaker Nameplate Header
    if (!m_speaker.empty()) {
        int spkW = static_cast<int>(m_speaker.size() * 8 + 12);
        renderer.fillRect(boxX + 6, boxY - 10, spkW, 14, Color(8, 24, 32));
        renderer.drawRect(boxX + 6, boxY - 10, spkW, 14, Color(224, 248, 208));
        FontRenderer::drawText(renderer, boxX + 12, boxY - 7, m_speaker, Color(224, 248, 208));
    }

    // Current Line with typewriter substring
    const std::string& currentLine = m_lines[m_currentLineIndex];
    size_t charCount = static_cast<size_t>(m_charProgress);
    std::string visibleText = currentLine.substr(0, std::min(charCount, currentLine.size()));

    // Draw text in Darkest Ink Black
    FontRenderer::drawText(renderer, textStartX, boxY + 14, visibleText, Color(8, 24, 32));

    // Render Option Choice Panel when active
    if (m_isChoosingOption && !m_options.empty()) {
        int optH = static_cast<int>(m_options.size()) * 16 + 10;
        int optW = 180;
        int optX = boxX + boxW - optW - 4;
        int optY = boxY - optH - 2;
        if (optY < 2) optY = 2; // Prevent going off-screen

        renderer.fillRect(optX, optY, optW, optH, Color(224, 248, 208));
        renderer.drawRect(optX, optY, optW, optH, Color(8, 24, 32));
        renderer.drawRect(optX + 2, optY + 2, optW - 4, optH - 4, Color(52, 104, 86));

        for (size_t i = 0; i < m_options.size(); ++i) {
            int oy = optY + 6 + static_cast<int>(i) * 16;
            bool isCur = (m_selectedOptionIndex == static_cast<int>(i));
            if (isCur) {
                FontRenderer::drawText(renderer, optX + 6, oy, ">", Color(8, 24, 32));
                FontRenderer::drawText(renderer, optX + 16, oy, m_options[i], Color(8, 24, 32));
            } else {
                FontRenderer::drawText(renderer, optX + 16, oy, m_options[i], Color(52, 104, 86));
            }
        }
    } else {
        // Prompt Indicator (Blinking ▼)
        if (m_isLineFullyRevealed && m_promptVisible) {
            FontRenderer::drawText(renderer, boxX + boxW - 16, boxY + boxH - 14, "▼", Color(8, 24, 32));
        }
    }
}

} // namespace JoseonRPG
