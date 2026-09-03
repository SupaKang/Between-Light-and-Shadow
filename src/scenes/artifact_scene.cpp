#include "artifact_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/scene_stack.hpp"
#include "../ui/font_renderer.hpp"
#include <algorithm>

namespace JoseonRPG {

ArtifactScene::ArtifactScene(ArtifactInventory& artifacts, Party& party)
    : m_artifacts(artifacts), m_party(party) {}

void ArtifactScene::onEnter() {
    m_cursor = 0;
    m_feedbackMsg.clear();
}

void ArtifactScene::handleInput() {
    if (Input::isRepeated(Key::Up)) {
        if (m_cursor > 0) m_cursor--;
    }
    if (Input::isRepeated(Key::Down)) {
        if (m_cursor + 1 < static_cast<int>(m_artifacts.getCount())) m_cursor++;
    }

    if (Input::isPressed(Key::ActionA) && m_artifacts.getCount() > 0) {
        m_artifacts.destroyArtifact(m_cursor, m_party.getActiveYokai(), &m_feedbackMsg);
        if (m_cursor >= static_cast<int>(m_artifacts.getCount()) && m_cursor > 0) {
            m_cursor--;
        }
    }

    if (Input::isPressed(Key::ActionB) || Input::isPressed(Key::ActionC)) {
        if (m_sceneStack) {
            m_sceneStack->popScene();
        }
    }
}

void ArtifactScene::update(float /*dt*/) {}

void ArtifactScene::render(Renderer& renderer) {
    renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(20, 24, 28));

    // Header Banner
    renderer.drawPanel(4, 4, 312, 20, Color(32, 38, 44), Palette::Yellow);
    FontRenderer::drawText(renderer, 10, 8, "=== 유물 보관함 (ARTIFACT INVENTORY) ===", Palette::Yellow);
    std::string slotCountStr = "보유: " + std::to_string(m_artifacts.getCount()) + "/8";
    FontRenderer::drawText(renderer, 248, 8, slotCountStr, Palette::Jade);

    // Left List
    renderer.drawPanel(4, 26, 130, 136, Color(16, 18, 22), Palette::MidGray);
    const auto& artList = m_artifacts.getArtifacts();

    if (artList.empty()) {
        FontRenderer::drawText(renderer, 10, 36, "보유 유물 없음", Palette::LightGray);
    } else {
        for (size_t i = 0; i < artList.size(); ++i) {
            int ay = 30 + static_cast<int>(i) * 16;
            Color c = (m_cursor == static_cast<int>(i)) ? Palette::Yellow : Palette::White;
            if (m_cursor == static_cast<int>(i)) {
                renderer.drawPanel(6, ay - 2, 126, 15, Color(36, 42, 52), Palette::Yellow);
                FontRenderer::drawText(renderer, 8, ay + 1, ">", Palette::Yellow);
            }
            FontRenderer::drawText(renderer, 16, ay + 1, std::to_string(i + 1) + ". " + artList[i].name, c);
        }
    }

    // Right Details
    renderer.drawPanel(138, 26, 178, 136, Color(16, 18, 22), Palette::MidGray);
    const auto* selectedArt = m_artifacts.getArtifact(m_cursor);
    if (selectedArt) {
        FontRenderer::drawText(renderer, 144, 30, selectedArt->name, Palette::Yellow);

        renderer.drawPanel(144, 44, 166, 22, Color(20, 36, 24), Palette::Jade);
        FontRenderer::drawText(renderer, 148, 46, "[축복/Buff]", Palette::Jade);
        FontRenderer::drawText(renderer, 148, 54, ArtifactInventory::getBuffDescription(*selectedArt), Palette::White);

        renderer.drawPanel(144, 70, 166, 22, Color(36, 20, 20), Palette::Red);
        FontRenderer::drawText(renderer, 148, 72, "[저주/Debuff]", Palette::Red);
        FontRenderer::drawText(renderer, 148, 80, ArtifactInventory::getDebuffDescription(*selectedArt), Palette::White);

        FontRenderer::drawText(renderer, 144, 96, "[설화 및 유래]", Palette::Yellow);
        FontRenderer::drawText(renderer, 144, 108, selectedArt->lore.substr(0, 22), Palette::LightGray);
        if (selectedArt->lore.length() > 22) {
            FontRenderer::drawText(renderer, 144, 118, selectedArt->lore.substr(22, 22), Palette::LightGray);
        }

        renderer.drawPanel(144, 134, 166, 20, Color(40, 20, 20), Palette::Red);
        FontRenderer::drawText(renderer, 150, 139, "[Z키: 유물 즉시 파괴]", Palette::Red);
    }

    if (!m_feedbackMsg.empty()) {
        renderer.fillRect(4, 148, 130, 12, Color(40, 50, 20));
        FontRenderer::drawText(renderer, 6, 150, m_feedbackMsg, Palette::Yellow);
    }

    // Bottom Help Bar
    renderer.fillRect(0, SCREEN_HEIGHT - 14, SCREEN_WIDTH, 14, Palette::Black);
    FontRenderer::drawText(renderer, 4, SCREEN_HEIGHT - 11, "방향키:유물선택 | Z:즉시파괴 | X/C:닫기", Palette::White);
}

} // namespace JoseonRPG
