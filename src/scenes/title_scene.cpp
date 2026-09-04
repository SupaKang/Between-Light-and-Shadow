#include "title_scene.hpp"
#include "art_inspector_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/scene_stack.hpp"
#include "../core/save_system.hpp"
#include "../ui/font_renderer.hpp"
#include "../data/data_manager.hpp"
#include "../world/world_scene.hpp"
#include "../audio/audio_engine.hpp"
#include <windows.h>

#include "intro_scene.hpp"

namespace JoseonRPG {

TitleScene::TitleScene(Party& party, ArtifactInventory& artifacts, int& money)
    : m_party(party), m_artifacts(artifacts), m_money(money) {}

void TitleScene::onEnter() {
    AudioEngine::playBgm(BgmTrack::Title);
    m_hasSave = SaveSystem::hasSaveFile(1);
    if (m_hasSave) {
        SaveSystem::getSaveSummary(1, m_saveSummary);
        m_cursor = 1; // Default to Continue if save exists
    } else {
        m_cursor = 0; // Default to New Game
    }
}

void TitleScene::startNewGame() {
    // Launch Prologue Cinematic Narrative Scene
    auto intro = std::make_unique<IntroScene>(m_party, m_artifacts, m_money);
    if (m_sceneStack) {
        m_sceneStack->clearAndSet(std::move(intro));
    }
}

void TitleScene::loadSavedGame() {
    if (!m_hasSave) return;

    GameRuntimeContext ctx;
    ctx.party = &m_party;
    ctx.artifacts = &m_artifacts;
    ctx.encyclopedia = &DataManager::getEncyclopedia();
    ctx.questManager = &DataManager::getQuestManager();

    if (SaveSystem::loadFromSlot(1, ctx)) {
        m_money = ctx.money;
        auto world = std::make_unique<WorldScene>(m_party, m_artifacts, m_money);
        world->setPlayerPosition(ctx.gridX, ctx.gridY, ctx.mapId);

        if (m_sceneStack) {
            m_sceneStack->clearAndSet(std::move(world));
        }
    }
}

void TitleScene::handleInput() {
    if (Input::isRepeated(Key::Up)) {
        if (m_cursor > 0) {
            m_cursor--;
            AudioEngine::playSfx(SfxId::MenuCursor);
        }
    }
    if (Input::isRepeated(Key::Down)) {
        if (m_cursor < 3) {
            m_cursor++;
            AudioEngine::playSfx(SfxId::MenuCursor);
        }
    }

    if (Input::isPressed(Key::ActionA)) {
        AudioEngine::playSfx(SfxId::MenuSelect);
        if (m_cursor == 0) {
            startNewGame();
        } else if (m_cursor == 1) {
            if (m_hasSave) {
                loadSavedGame();
            } else {
                startNewGame();
            }
        } else if (m_cursor == 2) {
            if (m_sceneStack) {
                m_sceneStack->pushScene(std::make_unique<ArtInspectorScene>());
            }
        } else if (m_cursor == 3) {
            PostQuitMessage(0);
        }
    }
}

void TitleScene::update(float dt) {
    m_animTimer += dt;
}

void TitleScene::render(Renderer& renderer) {
    // Pale DMG Background
    renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(224, 248, 208));

    // Title Logo Banner
    renderer.draw9SliceBox(20, 14, 280, 52, UITheme::Paper);
    FontRenderer::drawText(renderer, 58, 22, "108: 음양견문록", Color(8, 24, 32));
    FontRenderer::drawText(renderer, 44, 36, "Between Light and Shadow", Color(52, 104, 86));
    FontRenderer::drawText(renderer, 76, 48, "C++17 Standalone JRPG", Color(52, 104, 86));

    // Menu Options Box
    renderer.draw9SliceBox(60, 72, 200, 78, UITheme::Paper);

    const char* options[4] = {
        "1. 새로 시작 (New Game)",
        "2. 이어 하기 (Continue)",
        "3. 화첩 인스펙터 (Art Codex)",
        "4. 게임 종료 (Exit)"
    };
    for (int i = 0; i < 4; ++i) {
        int oy = 78 + i * 17;
        bool isSel = (m_cursor == i);

        if (isSel) {
            renderer.fillRect(66, oy - 2, 188, 15, Color(136, 192, 112));
            FontRenderer::drawText(renderer, 72, oy + 1, "▶", Color(8, 24, 32));
        }

        Color col = isSel ? Color(8, 24, 32) : Color(52, 104, 86);
        if (i == 1 && !m_hasSave) {
            col = Color(136, 192, 112);
        }

        FontRenderer::drawText(renderer, 84, oy + 1, options[i], col);
    }

    // Save Data Summary Preview
    if (m_hasSave) {
        renderer.draw9SliceBox(40, 154, 240, 18, UITheme::Paper);
        FontRenderer::drawText(renderer, 46, 158, "[저장 기록] " + m_saveSummary, Color(52, 104, 86));
    }

    // Bottom Footer
    renderer.draw9SliceBox(4, 166, 312, 12, UITheme::Inverted);
    FontRenderer::drawText(renderer, 10, 168, "방향키:선택 | Z/Space:확인 | 1.44MB Standalone", Color(224, 248, 208));
}

} // namespace JoseonRPG
