#include "dialogue_box.hpp"
#include "font_renderer.hpp"
#include "../core/input.hpp"
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
    m_onCompleteCallback = std::move(onComplete);
}

void DialogueBox::close() {
    m_active = false;
    m_lines.clear();
    m_currentLineIndex = 0;
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
        m_charProgress += m_charsPerSecond * dt;
        if (m_charProgress >= static_cast<float>(currentLine.size())) {
            m_charProgress = static_cast<float>(currentLine.size());
            m_isLineFullyRevealed = true;
        }
    } else {
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

    if (Input::isPressed(Key::ActionA) || Input::isPressed(Key::ActionB)) {
        if (!m_isLineFullyRevealed) {
            // Instant reveal current line
            if (m_currentLineIndex < m_lines.size()) {
                m_charProgress = static_cast<float>(m_lines[m_currentLineIndex].size());
                m_isLineFullyRevealed = true;
            }
        } else {
            // Next line or close
            m_currentLineIndex++;
            if (m_currentLineIndex < m_lines.size()) {
                m_charProgress = 0.0f;
                m_isLineFullyRevealed = false;
            } else {
                close();
            }
        }
        return true;
    }

    return true; // Block other inputs while dialogue is active
}

void DialogueBox::render(Renderer& renderer) {
    if (!m_active || m_currentLineIndex >= m_lines.size()) return;

    // Main Dialogue Frame at bottom (320x180 resolution)
    int boxX = 6;
    int boxY = 110;
    int boxW = 308;
    int boxH = 64;

    renderer.drawPanel(boxX, boxY, boxW, boxH, Color(18, 22, 30, 245), Palette::Yellow);

    // Speaker Nameplate Header
    if (!m_speaker.empty()) {
        renderer.drawPanel(boxX + 6, boxY - 10, static_cast<int>(m_speaker.size() * 8 + 12), 14, Color(28, 34, 48), Palette::Yellow);
        FontRenderer::drawText(renderer, boxX + 12, boxY - 7, m_speaker, Palette::Yellow);
    }

    // Current Line with typewriter substring
    const std::string& currentLine = m_lines[m_currentLineIndex];
    size_t charCount = static_cast<size_t>(m_charProgress);
    std::string visibleText = currentLine.substr(0, std::min(charCount, currentLine.size()));

    // Draw text with word wrapping or multi-line if contains \n
    FontRenderer::drawText(renderer, boxX + 10, boxY + 12, visibleText, Palette::White);

    // Prompt Indicator (Blinking ▼ / [Z])
    if (m_isLineFullyRevealed && m_promptVisible) {
        FontRenderer::drawText(renderer, boxX + boxW - 20, boxY + boxH - 12, ">", Palette::Jade);
    }
}

} // namespace JoseonRPG
