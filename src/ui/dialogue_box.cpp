#include "dialogue_box.hpp"
#include "font_renderer.hpp"
#include "../core/input.hpp"
#include "../core/gen1_assets.hpp"
#include "../scenes/settings_scene.hpp"
#include "../audio/audio_engine.hpp"
#include <algorithm>

namespace JoseonRPG {

static size_t getUtf8Length(std::string_view str) {
    size_t count = 0;
    for (size_t i = 0; i < str.size();) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        if (c < 0x80) i += 1;
        else if ((c & 0xE0) == 0xC0) i += 2;
        else if ((c & 0xF0) == 0xE0) i += 3;
        else if ((c & 0xF8) == 0xF0) i += 4;
        else i += 1;
        count++;
    }
    return count;
}

static std::string getUtf8Substr(std::string_view str, size_t codepoints) {
    size_t count = 0;
    size_t byteIdx = 0;
    while (byteIdx < str.size() && count < codepoints) {
        unsigned char c = static_cast<unsigned char>(str[byteIdx]);
        if (c < 0x80) byteIdx += 1;
        else if ((c & 0xE0) == 0xC0) byteIdx += 2;
        else if ((c & 0xF0) == 0xE0) byteIdx += 3;
        else if ((c & 0xF8) == 0xF0) byteIdx += 4;
        else byteIdx += 1;
        count++;
    }
    return std::string(str.substr(0, byteIdx));
}

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
    size_t totalChars = getUtf8Length(currentLine);

    if (!m_isLineFullyRevealed) {
        int speedMode = SettingsScene::getGlobalSettings().textSpeed;
        float curSpeed = (speedMode == 0) ? 25.0f : (speedMode == 1 ? 55.0f : 9999.0f);
        m_charProgress += curSpeed * dt;
        if (m_charProgress >= static_cast<float>(totalChars)) {
            m_charProgress = static_cast<float>(totalChars);
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
            if (m_selectedOptionIndex > 0) {
                m_selectedOptionIndex--;
                AudioEngine::playSfx(SfxId::MenuCursor);
            }
        }
        if (Input::isPressed(Key::Down)) {
            if (m_selectedOptionIndex + 1 < static_cast<int>(m_options.size())) {
                m_selectedOptionIndex++;
                AudioEngine::playSfx(SfxId::MenuCursor);
            }
        }
        if (Input::isPressed(Key::ActionA)) {
            AudioEngine::playSfx(SfxId::MenuSelect);
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
                m_charProgress = static_cast<float>(getUtf8Length(m_lines[m_currentLineIndex]));
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
                AudioEngine::playSfx(SfxId::MenuCursor);
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

    // Main Dialogue Frame at bottom (320x180 resolution) - 9-Slice Paper Frame
    int boxX = 6;
    int boxY = 112;
    int boxW = 308;
    int boxH = 64;

    renderer.draw9SliceBox(boxX, boxY, boxW, boxH, UITheme::Paper);

    // Portrait (48x48 Gen 1 Bust)
    int textStartX = boxX + 10;
    const uint8_t* portraitData = nullptr;

    if (m_speaker.find("구미호") != std::string::npos || m_speaker.find("설화") != std::string::npos || m_speaker.find("여우") != std::string::npos) {
        portraitData = Gen1Assets::PORTRAIT_GUMIHO_48x48;
    } else if (m_speaker.find("주인공") != std::string::npos || m_speaker.find("영술사") != std::string::npos || m_speaker.find("퇴마사") != std::string::npos || m_speaker.find("자신") != std::string::npos) {
        portraitData = Gen1Assets::PORTRAIT_PROTAGONIST_48x48;
    } else if (m_speaker.find("도사") != std::string::npos || m_speaker.find("성현") != std::string::npos || m_speaker.find("도선") != std::string::npos || m_speaker.find("제조관") != std::string::npos || m_speaker.find("스승") != std::string::npos || m_speaker.find("낭인") != std::string::npos || m_speaker.find("선사") != std::string::npos || m_speaker.find("박문수") != std::string::npos || m_speaker.find("어사") != std::string::npos) {
        portraitData = Gen1Assets::PORTRAIT_DOSA_48x48;
    } else if (m_speaker.find("주모") != std::string::npos || m_speaker.find("주막") != std::string::npos || m_speaker.find("아낙") != std::string::npos || m_speaker.find("어머니") != std::string::npos || m_speaker.find("며느리") != std::string::npos || m_speaker.find("해녀") != std::string::npos || m_speaker.find("진주") != std::string::npos) {
        portraitData = Gen1Assets::PORTRAIT_JUMO_48x48;
    } else if (m_speaker.find("음양당") != std::string::npos || m_speaker.find("주술사") != std::string::npos || m_speaker.find("묵영") != std::string::npos || m_speaker.find("괴승") != std::string::npos || m_speaker.find("배극") != std::string::npos || m_speaker.find("흑사") != std::string::npos || m_speaker.find("탈주") != std::string::npos) {
        portraitData = Gen1Assets::PORTRAIT_CULTIST_48x48;
    }

    if (portraitData) {
        renderer.fillRect(boxX + 7, boxY + 7, 50, 50, Color(8, 24, 32));
        renderer.drawGen1Bitmap(boxX + 8, boxY + 8, 48, 48, portraitData, false);
        renderer.drawRect(boxX + 7, boxY + 7, 50, 50, Color(52, 104, 86));
        textStartX = boxX + 64;
    }

    // Speaker Nameplate Header
    if (!m_speaker.empty()) {
        int spkW = static_cast<int>(m_speaker.size() * 8 + 16);
        renderer.draw9SliceBox(boxX + 8, boxY - 11, spkW, 15, UITheme::Inverted);
        FontRenderer::drawText(renderer, boxX + 14, boxY - 9, m_speaker, Color(224, 248, 208));
    }

    // Current Line with typewriter substring
    const std::string& currentLine = m_lines[m_currentLineIndex];
    size_t charCount = static_cast<size_t>(m_charProgress);
    std::string visibleText = getUtf8Substr(currentLine, charCount);

    // Draw text in Darkest Ink Black (Line height 14px)
    FontRenderer::drawText(renderer, textStartX, boxY + 12, visibleText, Color(8, 24, 32));

    // Render Option Choice Panel when active (Top-Right Pop-up)
    if (m_isChoosingOption && !m_options.empty()) {
        int optH = static_cast<int>(m_options.size()) * 16 + 10;
        int optW = 180;
        int optX = boxX + boxW - optW - 2;
        int optY = boxY - optH - 3;
        if (optY < 2) optY = 2; // Prevent going off-screen

        renderer.draw9SliceBox(optX, optY, optW, optH, UITheme::Paper);

        for (size_t i = 0; i < m_options.size(); ++i) {
            int oy = optY + 6 + static_cast<int>(i) * 16;
            bool isCur = (m_selectedOptionIndex == static_cast<int>(i));
            if (isCur) {
                renderer.fillRect(optX + 4, oy - 1, optW - 8, 14, Color(136, 192, 112));
                FontRenderer::drawText(renderer, optX + 6, oy, ">", Color(8, 24, 32));
                FontRenderer::drawText(renderer, optX + 16, oy, m_options[i], Color(8, 24, 32));
            } else {
                FontRenderer::drawText(renderer, optX + 16, oy, m_options[i], Color(52, 104, 86));
            }
        }
    } else {
        // Prompt Indicator (Blinking ▼)
        if (m_isLineFullyRevealed && m_promptVisible) {
            FontRenderer::drawText(renderer, boxX + boxW - 16, boxY + boxH - 16, "▼", Color(8, 24, 32));
        }
    }
}


} // namespace JoseonRPG
