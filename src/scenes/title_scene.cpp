#include "title_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/scene_stack.hpp"
#include "../core/save_system.hpp"
#include "../ui/font_renderer.hpp"
#include "../data/data_manager.hpp"
#include "../world/world_scene.hpp"
#include "../audio/audio_engine.hpp"
#include <windows.h>

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
    // Initialize default starter party
    m_party.clear();
    Yokai p1 = DataManager::createYokaiById("YOKAI_001"); // Dokkaebi
    p1.gainExp(1500); // Level 6
    m_party.addYokai(p1);

    Yokai p2 = DataManager::createYokaiById("YOKAI_002"); // Gumiho
    p2.gainExp(2500); // Level 8
    m_party.addYokai(p2);

    Yokai p3 = DataManager::createYokaiById("YOKAI_003"); // Bulgasari
    p3.gainExp(800);  // Level 4
    m_party.addYokai(p3);

    m_artifacts.clear();
    m_artifacts.addArtifact(DataManager::createArtifactById("ART_DOKKAEBI_HAT"));
    m_artifacts.addArtifact(DataManager::createArtifactById("ART_DEMON_TILE"));

    m_money = 500;

    auto world = std::make_unique<WorldScene>(m_party, m_artifacts, m_money);
    world->setPlayerPosition(7, 6, 0);

    if (m_sceneStack) {
        m_sceneStack->clearAndSet(std::move(world));
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
        if (m_cursor < 2) {
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
            PostQuitMessage(0);
        }
    }
}

void TitleScene::update(float dt) {
    m_animTimer += dt;
}

void TitleScene::render(Renderer& renderer) {
    // Dark Oriental Backdrop
    renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(12, 14, 20));

    // Title Logo Banner
    renderer.drawPanel(20, 16, 280, 52, Color(24, 28, 40), Palette::Yellow);
    FontRenderer::drawText(renderer, 58, 24, "108: 음양견문록", Palette::Yellow);
    FontRenderer::drawText(renderer, 44, 38, "Between Light and Shadow", Palette::White);
    FontRenderer::drawText(renderer, 76, 50, "C++17 Standalone JRPG", Palette::LightGray);

    // Menu Options Box
    renderer.drawPanel(60, 78, 200, 68, Color(20, 24, 32), Palette::MidGray);

    const char* options[3] = {"1. 새로 시작 (New Game)", "2. 이어 하기 (Continue)", "3. 게임 종료 (Exit)"};
    for (int i = 0; i < 3; ++i) {
        int oy = 86 + i * 18;
        bool isSel = (m_cursor == i);

        if (isSel) {
            renderer.fillRect(66, oy - 2, 188, 16, Color(40, 48, 64));
            FontRenderer::drawText(renderer, 72, oy + 1, ">", Palette::Yellow);
        }

        Color col = isSel ? Palette::Yellow : Palette::White;
        if (i == 1 && !m_hasSave) {
            col = Palette::DarkGray;
        }

        FontRenderer::drawText(renderer, 84, oy + 1, options[i], col);
    }

    // Save Data Summary Preview
    if (m_hasSave) {
        renderer.drawPanel(40, 150, 240, 18, Color(16, 24, 20), Palette::Jade);
        FontRenderer::drawText(renderer, 46, 155, "[저장 기록] " + m_saveSummary, Palette::Jade);
    }

    // Bottom Footer
    renderer.fillRect(0, SCREEN_HEIGHT - 10, SCREEN_WIDTH, 10, Palette::Black);
    FontRenderer::drawText(renderer, 4, SCREEN_HEIGHT - 8, "방향키:선택 | Z/Space:확인 | 1.44MB Standalone", Palette::MidGray);
}

} // namespace JoseonRPG
