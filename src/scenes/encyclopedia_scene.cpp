#include "encyclopedia_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/scene_stack.hpp"
#include "../ui/font_renderer.hpp"
#include <algorithm>

namespace JoseonRPG {

EncyclopediaScene::EncyclopediaScene(Encyclopedia& encyclopedia)
    : m_encyclopedia(encyclopedia) {}

void EncyclopediaScene::onEnter() {
    m_cursor = 1;
}

void EncyclopediaScene::handleInput() {
    if (Input::isRepeated(Key::Up)) {
        if (m_cursor > 1) m_cursor--;
    }
    if (Input::isRepeated(Key::Down)) {
        if (m_cursor < 108) m_cursor++;
    }
    if (Input::isRepeated(Key::Left)) {
        m_cursor = std::max(1, m_cursor - 10);
    }
    if (Input::isRepeated(Key::Right)) {
        m_cursor = std::min(108, m_cursor + 10);
    }

    if (Input::isPressed(Key::ActionB) || Input::isPressed(Key::ActionA)) {
        if (m_sceneStack) {
            m_sceneStack->popScene();
        }
    }
}

void EncyclopediaScene::update(float /*dt*/) {
}

void EncyclopediaScene::render(Renderer& renderer) {
    renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(18, 20, 26));

    // Header Banner
    renderer.drawPanel(4, 4, 312, 20, Color(28, 32, 42), Palette::Yellow);
    FontRenderer::drawText(renderer, 10, 8, "=== 108: 음양견문록 도감 (CODEX) ===", Palette::Yellow);

    int capCnt = m_encyclopedia.getCapturedCount();
    std::string rateStr = "수집: " + std::to_string(capCnt) + "/108 (" + std::to_string(static_cast<int>(capCnt * 100.0f / 108.0f)) + "%)";
    FontRenderer::drawText(renderer, 220, 8, rateStr, Palette::Jade);

    // Left List Panel
    renderer.drawPanel(4, 26, 130, 136, Color(16, 18, 24), Palette::MidGray);
    int startSlot = std::max(1, std::min(103, m_cursor - 2));

    for (int i = 0; i < 6; ++i) {
        int slotNum = startSlot + i;
        if (slotNum > 108) break;
        const auto* entry = m_encyclopedia.getEntry(slotNum);
        int ey = 32 + i * 21;

        if (m_cursor == slotNum) {
            renderer.drawPanel(6, ey - 2, 126, 19, Color(40, 44, 56), Palette::Yellow);
            FontRenderer::drawText(renderer, 8, ey + 3, ">", Palette::Yellow);
        }

        std::string numStr = std::string("#") + ((slotNum < 10) ? "00" : (slotNum < 100 ? "0" : "")) + std::to_string(slotNum);
        FontRenderer::drawText(renderer, 16, ey + 3, numStr, Palette::LightGray);

        if (entry) {
            if (entry->status == DiscoveryStatus::Captured) {
                FontRenderer::drawText(renderer, 50, ey + 3, entry->nameKo, Palette::White);
            } else if (entry->status == DiscoveryStatus::Seen) {
                FontRenderer::drawText(renderer, 50, ey + 3, entry->nameKo, Palette::MidGray);
            } else {
                FontRenderer::drawText(renderer, 50, ey + 3, "???", Palette::DarkGray);
            }
        }
    }

    // Right Detail Panel
    renderer.drawPanel(138, 26, 178, 136, Color(16, 18, 24), Palette::MidGray);
    const auto* selected = m_encyclopedia.getEntry(m_cursor);
    if (selected) {
        std::string titleNum = "No." + std::to_string(selected->number) + " " + selected->nameKo;
        FontRenderer::drawText(renderer, 144, 32, titleNum, Palette::Yellow);

        if (selected->status == DiscoveryStatus::Captured) {
            renderer.fillRect(144, 44, 55, 9, Palette::Jade);
            FontRenderer::drawText(renderer, 146, 45, "[계약완료]", Palette::Black);

            std::string gStr = "등급: Grade " + std::to_string(static_cast<int>(selected->baseGrade));
            FontRenderer::drawText(renderer, 206, 45, gStr, Palette::White);

            FontRenderer::drawText(renderer, 144, 58, "출처: " + selected->origin, Palette::LightGray);

            FontRenderer::drawText(renderer, 144, 72, "[전승 및 배경 설화]", Palette::Yellow);
            FontRenderer::drawText(renderer, 144, 84, selected->lore.substr(0, 24), Palette::White);
            if (selected->lore.length() > 24) {
                FontRenderer::drawText(renderer, 144, 96, selected->lore.substr(24, 24), Palette::White);
            }
        } else if (selected->status == DiscoveryStatus::Seen) {
            renderer.fillRect(144, 44, 55, 9, Palette::Blue);
            FontRenderer::drawText(renderer, 146, 45, "[조우함]", Palette::Black);
            FontRenderer::drawText(renderer, 144, 65, "전투에서 조우하였으나", Palette::LightGray);
            FontRenderer::drawText(renderer, 144, 78, "아직 계약을 맺지 못함.", Palette::LightGray);
        } else {
            renderer.fillRect(144, 44, 55, 9, Palette::DarkGray);
            FontRenderer::drawText(renderer, 146, 45, "[미확인]", Palette::Black);
            FontRenderer::drawText(renderer, 144, 70, "조선의 미지의 요괴입니다.", Palette::MidGray);
            FontRenderer::drawText(renderer, 144, 84, "야생에서 조우하십시오.", Palette::MidGray);
        }
    }

    // Bottom Help Bar
    renderer.fillRect(0, SCREEN_HEIGHT - 14, SCREEN_WIDTH, 14, Palette::Black);
    FontRenderer::drawText(renderer, 4, SCREEN_HEIGHT - 11, "방향키:스크롤 | 좌/우:10개 점프 | X/Z:닫기", Palette::White);
}

} // namespace JoseonRPG
