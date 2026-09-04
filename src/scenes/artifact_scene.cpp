#include "artifact_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/scene_stack.hpp"
#include "../ui/font_renderer.hpp"
#include "../audio/audio_engine.hpp"
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
        AudioEngine::playSfx(SfxId::ArtifactDestroy);
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
    renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(224, 248, 208));

    // Header Banner
    renderer.draw9SliceBox(4, 4, 312, 20, UITheme::Paper);
    FontRenderer::drawText(renderer, 10, 8, "=== 유물 보관함 (양날의 검) ===", Color(8, 24, 32));
    std::string slotCountStr = "보유: " + std::to_string(m_artifacts.getCount()) + "/8";
    FontRenderer::drawText(renderer, 240, 8, slotCountStr, Color(52, 104, 86));

    // Left List
    renderer.draw9SliceBox(4, 26, 134, 136, UITheme::Paper);
    const auto& artList = m_artifacts.getArtifacts();

    if (artList.empty()) {
        FontRenderer::drawText(renderer, 12, 36, "보유 유물 없음", Color(52, 104, 86));
    } else {
        for (size_t i = 0; i < artList.size(); ++i) {
            int ay = 30 + static_cast<int>(i) * 16;
            bool isCur = (m_cursor == static_cast<int>(i));
            if (isCur) {
                renderer.fillRect(8, ay - 1, 126, 15, Color(136, 192, 112));
                FontRenderer::drawText(renderer, 10, ay + 1, "▶", Color(8, 24, 32));
                FontRenderer::drawText(renderer, 20, ay + 1, std::to_string(i + 1) + ". " + artList[i].name, Color(8, 24, 32));
            } else {
                FontRenderer::drawText(renderer, 20, ay + 1, std::to_string(i + 1) + ". " + artList[i].name, Color(52, 104, 86));
            }
        }
    }

    // Right Details
    renderer.draw9SliceBox(142, 26, 174, 136, UITheme::Paper);
    const auto* selectedArt = m_artifacts.getArtifact(m_cursor);
    if (selectedArt) {
        FontRenderer::drawText(renderer, 148, 30, selectedArt->name, Color(8, 24, 32));

        // Buff Box
        renderer.draw9SliceBox(148, 44, 162, 24, UITheme::Paper);
        FontRenderer::drawText(renderer, 152, 46, "[축복/Buff (+)]", Color(52, 104, 86));
        FontRenderer::drawText(renderer, 152, 56, ArtifactInventory::getBuffDescription(*selectedArt), Color(8, 24, 32));

        // Debuff Box
        renderer.draw9SliceBox(148, 72, 162, 24, UITheme::Paper);
        FontRenderer::drawText(renderer, 152, 74, "[저주/Debuff (-)]", Color(180, 40, 40));
        FontRenderer::drawText(renderer, 152, 84, ArtifactInventory::getDebuffDescription(*selectedArt), Color(8, 24, 32));

        FontRenderer::drawText(renderer, 148, 100, "[설화 및 유래]", Color(52, 104, 86));
        FontRenderer::drawText(renderer, 148, 112, selectedArt->lore.substr(0, 22), Color(8, 24, 32));
        if (selectedArt->lore.length() > 22) {
            FontRenderer::drawText(renderer, 148, 124, selectedArt->lore.substr(22, 22), Color(8, 24, 32));
        }

        renderer.draw9SliceBox(148, 138, 162, 20, UITheme::Inverted);
        FontRenderer::drawText(renderer, 154, 143, "[Z키: 유물 즉시 파괴]", Color(224, 248, 208));
    }

    if (!m_feedbackMsg.empty()) {
        renderer.draw9SliceBox(6, 144, 130, 16, UITheme::Inverted);
        FontRenderer::drawText(renderer, 10, 147, m_feedbackMsg, Color(224, 248, 208));
    }

    // Bottom Help Bar
    renderer.draw9SliceBox(4, 164, 312, 14, UITheme::Inverted);
    FontRenderer::drawText(renderer, 10, 166, "방향키:유물선택 | Z:즉시파괴 | X/C:닫기", Color(224, 248, 208));
}

} // namespace JoseonRPG
