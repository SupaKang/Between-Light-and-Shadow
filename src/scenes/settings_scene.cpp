#include "settings_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/window.hpp"
#include "../core/scene_stack.hpp"
#include "../ui/font_renderer.hpp"
#include "../audio/audio_engine.hpp"
#include <algorithm>

namespace JoseonRPG {

GameSettings SettingsScene::s_settings;

SettingsScene::SettingsScene() : m_cursor(0) {}

void SettingsScene::onEnter() {}

GameSettings& SettingsScene::getGlobalSettings() {
    return s_settings;
}

void SettingsScene::handleInput() {
    if (Input::isRepeated(Key::Up)) {
        if (m_cursor > 0) {
            m_cursor--;
            AudioEngine::playSfx(SfxId::MenuCursor);
        }
    }
    if (Input::isRepeated(Key::Down)) {
        if (m_cursor < 8) {
            m_cursor++;
            AudioEngine::playSfx(SfxId::MenuCursor);
        }
    }

    if (Input::isRepeated(Key::Left)) {
        AudioEngine::playSfx(SfxId::MenuCursor);
        switch (m_cursor) {
            case 0: // Master Volume
                s_settings.masterVolume = std::max(0.0f, s_settings.masterVolume - 0.1f);
                AudioEngine::setMasterVolume(s_settings.masterVolume);
                break;
            case 1: // BGM Volume
                s_settings.bgmVolume = std::max(0.0f, s_settings.bgmVolume - 0.1f);
                AudioEngine::setBgmVolume(s_settings.bgmVolume);
                break;
            case 2: // SFX Volume
                s_settings.sfxVolume = std::max(0.0f, s_settings.sfxVolume - 0.1f);
                AudioEngine::setSfxVolume(s_settings.sfxVolume);
                break;
            case 3: // Mute
                s_settings.isMuted = !s_settings.isMuted;
                AudioEngine::toggleMute();
                break;
            case 4: // Display Scale
                if (s_settings.displayScale > 3) {
                    s_settings.displayScale--;
                    if (Window::getInstance()) {
                        if (s_settings.displayScale <= 5) {
                            Window::getInstance()->setScale(s_settings.displayScale);
                        }
                    }
                }
                break;
            case 5: // CRT Filter
                s_settings.crtFilter = !s_settings.crtFilter;
                break;
            case 6: // Vignette
                s_settings.vignette = !s_settings.vignette;
                break;
            case 7: // Text Speed
                if (s_settings.textSpeed > 0) s_settings.textSpeed--;
                break;
        }
    }

    if (Input::isRepeated(Key::Right)) {
        AudioEngine::playSfx(SfxId::MenuCursor);
        switch (m_cursor) {
            case 0: // Master Volume
                s_settings.masterVolume = std::min(1.0f, s_settings.masterVolume + 0.1f);
                AudioEngine::setMasterVolume(s_settings.masterVolume);
                break;
            case 1: // BGM Volume
                s_settings.bgmVolume = std::min(1.0f, s_settings.bgmVolume + 0.1f);
                AudioEngine::setBgmVolume(s_settings.bgmVolume);
                break;
            case 2: // SFX Volume
                s_settings.sfxVolume = std::min(1.0f, s_settings.sfxVolume + 0.1f);
                AudioEngine::setSfxVolume(s_settings.sfxVolume);
                break;
            case 3: // Mute
                s_settings.isMuted = !s_settings.isMuted;
                AudioEngine::toggleMute();
                break;
            case 4: // Display Scale
                if (s_settings.displayScale < 6) {
                    s_settings.displayScale++;
                    if (Window::getInstance()) {
                        if (s_settings.displayScale == 6) {
                            Window::getInstance()->setFullscreen(true);
                        } else {
                            Window::getInstance()->setScale(s_settings.displayScale);
                        }
                    }
                }
                break;
            case 5: // CRT Filter
                s_settings.crtFilter = !s_settings.crtFilter;
                break;
            case 6: // Vignette
                s_settings.vignette = !s_settings.vignette;
                break;
            case 7: // Text Speed
                if (s_settings.textSpeed < 2) s_settings.textSpeed++;
                break;
        }
    }

    if (Input::isPressed(Key::ActionA)) {
        AudioEngine::playSfx(SfxId::MenuSelect);
        if (m_cursor == 8) {
            if (m_sceneStack) m_sceneStack->popScene();
        } else if (m_cursor == 3) {
            s_settings.isMuted = !s_settings.isMuted;
            AudioEngine::toggleMute();
        } else if (m_cursor == 4) {
            if (s_settings.displayScale < 6) {
                s_settings.displayScale = 6;
                if (Window::getInstance()) Window::getInstance()->setFullscreen(true);
            } else {
                s_settings.displayScale = 3;
                if (Window::getInstance()) Window::getInstance()->setScale(3);
            }
        } else if (m_cursor == 5) {
            s_settings.crtFilter = !s_settings.crtFilter;
        } else if (m_cursor == 6) {
            s_settings.vignette = !s_settings.vignette;
        }
    }

    if (Input::isPressed(Key::ActionB)) {
        AudioEngine::playSfx(SfxId::MenuCancel);
        if (m_sceneStack) m_sceneStack->popScene();
    }
}

void SettingsScene::update(float /*dt*/) {}

void SettingsScene::render(Renderer& renderer) {
    // Dim background
    renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(16, 18, 24, 230));

    // Main Settings Panel
    renderer.drawPanel(16, 6, 288, 168, Color(24, 28, 36), Palette::Yellow);
    FontRenderer::drawText(renderer, 92, 11, "=== 시스템 환경 설정 ===", Palette::Yellow);

    std::string textSpeedNames[3] = {"보통 (Normal)", "빠름 (Fast)", "즉시 (Instant)"};

    std::string scaleName = "3X (960x540)";
    if (s_settings.displayScale == 4) scaleName = "4X (1280x720)";
    else if (s_settings.displayScale == 5) scaleName = "5X (1600x900)";
    else if (s_settings.displayScale == 6) scaleName = "전체화면 (F11/Alt+Enter)";

    std::string labels[9] = {
        "마스터 음량 (Master Vol): " + std::to_string(static_cast<int>(s_settings.masterVolume * 100)) + "%",
        "배경음 음량 (BGM Vol):    " + std::to_string(static_cast<int>(s_settings.bgmVolume * 100)) + "%",
        "효과음 음량 (SFX Vol):    " + std::to_string(static_cast<int>(s_settings.sfxVolume * 100)) + "%",
        "음소거 모드 (Mute Audio): " + std::string(s_settings.isMuted ? "[ON]" : "[OFF]"),
        "화면 크기 (Display Scale): " + scaleName,
        "CRT 레트로 스캔라인:      " + std::string(s_settings.crtFilter ? "[ON]" : "[OFF]"),
        "화면 외곽 비네트 (암실):   " + std::string(s_settings.vignette ? "[ON]" : "[OFF]"),
        "대사 출력 속도:           " + textSpeedNames[s_settings.textSpeed],
        "[ 설정 완료 및 게임 복귀 ]"
    };

    for (int i = 0; i < 9; ++i) {
        int oy = 25 + i * 14;
        bool isSel = (m_cursor == i);

        if (isSel) {
            renderer.fillRect(22, oy - 2, 276, 12, Color(40, 50, 70));
            FontRenderer::drawText(renderer, 25, oy, ">", Palette::Yellow);
        }

        Color col = isSel ? Palette::Yellow : Palette::White;
        if (i == 8) col = isSel ? Palette::Green : Palette::Jade;

        FontRenderer::drawText(renderer, 34, oy, labels[i], col);
    }

    FontRenderer::drawText(renderer, 24, 157, "방향키:조절 | Z:결정 | X:닫기 | F11:전체화면", Palette::MidGray);
}

} // namespace JoseonRPG
