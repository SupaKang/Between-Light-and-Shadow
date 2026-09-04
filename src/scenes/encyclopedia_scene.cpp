#include "encyclopedia_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/scene_stack.hpp"
#include "../ui/font_renderer.hpp"
#include "../data/data_manager.hpp"
#include "../core/gen1_assets.hpp"
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
    renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(224, 248, 208));

    // Header Banner
    renderer.draw9SliceBox(4, 4, 312, 20, UITheme::Paper);
    FontRenderer::drawText(renderer, 10, 8, "=== 108: 음양견문록 도감 ===", Color(8, 24, 32));

    int capCnt = m_encyclopedia.getCapturedCount();
    std::string rateStr = "수집: " + std::to_string(capCnt) + "/108";
    FontRenderer::drawText(renderer, 228, 8, rateStr, Color(52, 104, 86));

    // Left List Panel
    renderer.draw9SliceBox(4, 26, 134, 136, UITheme::Paper);
    int startSlot = std::max(1, std::min(103, m_cursor - 2));

    for (int i = 0; i < 6; ++i) {
        int slotNum = startSlot + i;
        if (slotNum > 108) break;
        const auto* entry = m_encyclopedia.getEntry(slotNum);
        int ey = 32 + i * 21;

        if (m_cursor == slotNum) {
            renderer.fillRect(8, ey - 1, 126, 18, Color(136, 192, 112));
            FontRenderer::drawText(renderer, 10, ey + 3, ">", Color(8, 24, 32));
        }

        std::string numStr = std::string("#") + ((slotNum < 10) ? "00" : (slotNum < 100 ? "0" : "")) + std::to_string(slotNum);
        FontRenderer::drawText(renderer, 18, ey + 3, numStr, Color(52, 104, 86));

        if (entry) {
            if (entry->status == DiscoveryStatus::Captured) {
                FontRenderer::drawText(renderer, 52, ey + 3, entry->nameKo, Color(8, 24, 32));
            } else if (entry->status == DiscoveryStatus::Seen) {
                FontRenderer::drawText(renderer, 52, ey + 3, entry->nameKo, Color(52, 104, 86));
            } else {
                FontRenderer::drawText(renderer, 52, ey + 3, "???", Color(136, 192, 112));
            }
        }
    }

    // Right Detail Panel
    renderer.draw9SliceBox(142, 26, 174, 136, UITheme::Paper);
    const auto* selected = m_encyclopedia.getEntry(m_cursor);
    if (selected) {
        std::string numPrefix = std::string("#") + ((selected->number < 10) ? "00" : (selected->number < 100 ? "0" : "")) + std::to_string(selected->number);
        std::string titleNum = numPrefix + " " + ((selected->status != DiscoveryStatus::Unseen) ? selected->nameKo : "???");
        FontRenderer::drawText(renderer, 148, 30, titleNum, Color(8, 24, 32));

        // Draw 32x32 Yokai Battler preview if seen/captured
        if (selected->status != DiscoveryStatus::Unseen && selected->number >= 1 && selected->number <= 108) {
            renderer.drawGen1Bitmap(276, 30, 32, 32, Gen1Assets::YOKAI_BATTLERS_32x32[selected->number - 1], true);
        }

        auto getElemName = [](Element el) -> const char* {
            switch (el) {
                case Element::Fire: return "화(火)";
                case Element::Water: return "수(水)";
                case Element::Earth: return "토(土)";
                case Element::Light: return "광(光)";
                case Element::Dark: return "암(暗)";
                default: return "무(無)";
            }
        };

        auto getHabitat = [](int num) -> const char* {
            if (num <= 25) return "제1구역: 한양 북한산 & 도선사";
            if (num <= 55) return "제2구역: 소백산맥 & 죽령 험로";
            if (num <= 80) return "제3구역: 남해안 & 유령 난파선";
            if (num <= 100) return "제4구역: 지리산 & 여우골";
            return "제5구역: 일식의 성채 & 영맥 심연";
        };

        if (selected->status == DiscoveryStatus::Captured) {
            renderer.fillRect(148, 44, 48, 12, Color(8, 24, 32));
            FontRenderer::drawText(renderer, 150, 45, "[계약완료]", Color(224, 248, 208));

            renderer.fillRect(200, 44, 38, 12, Color(52, 104, 86));
            FontRenderer::drawText(renderer, 202, 45, getElemName(selected->element), Color(224, 248, 208));

            std::string gStr = "G." + std::to_string(static_cast<int>(selected->baseGrade));
            FontRenderer::drawText(renderer, 244, 45, gStr, Color(8, 24, 32));

            FontRenderer::drawText(renderer, 148, 60, getHabitat(selected->number), Color(52, 104, 86));

            // Fetch template Yokai for trait and skills
            Yokai tempYokai = DataManager::createYokaiById(selected->id);
            if (tempYokai.getTrait() != YokaiTrait::None) {
                std::string trStr = "특성: " + tempYokai.getTraitName();
                FontRenderer::drawText(renderer, 148, 72, trStr, Color(8, 24, 32));
            } else {
                FontRenderer::drawText(renderer, 148, 72, "출처: " + selected->origin, Color(52, 104, 86));
            }

            // Skills Preview
            const auto& skls = tempYokai.getSkills();
            std::string sklPreview = "기술: ";
            for (size_t k = 0; k < skls.size() && k < 2; ++k) {
                if (k > 0) sklPreview += ", ";
                sklPreview += skls[k].name;
            }
            FontRenderer::drawText(renderer, 148, 84, sklPreview, Color(8, 24, 32));

            // Lore / Folklore description
            FontRenderer::drawText(renderer, 148, 98, "[전승 및 배경 설화]", Color(52, 104, 86));
            FontRenderer::drawText(renderer, 148, 110, selected->lore.substr(0, 24), Color(8, 24, 32));
            if (selected->lore.length() > 24) {
                FontRenderer::drawText(renderer, 148, 122, selected->lore.substr(24, 24), Color(8, 24, 32));
            }
            if (selected->lore.length() > 48) {
                FontRenderer::drawText(renderer, 148, 134, selected->lore.substr(48, 24), Color(8, 24, 32));
            }
        } else if (selected->status == DiscoveryStatus::Seen) {
            renderer.fillRect(148, 44, 44, 12, Color(52, 104, 86));
            FontRenderer::drawText(renderer, 150, 45, "[조우함]", Color(224, 248, 208));

            renderer.fillRect(196, 44, 38, 12, Color(8, 24, 32));
            FontRenderer::drawText(renderer, 198, 45, getElemName(selected->element), Color(224, 248, 208));

            FontRenderer::drawText(renderer, 148, 62, "[주요 서식지]", Color(8, 24, 32));
            FontRenderer::drawText(renderer, 148, 74, getHabitat(selected->number), Color(52, 104, 86));

            FontRenderer::drawText(renderer, 148, 92, "전투에서 조우하였으나", Color(52, 104, 86));
            FontRenderer::drawText(renderer, 148, 106, "아직 계약을 맺지 못함.", Color(52, 104, 86));
            FontRenderer::drawText(renderer, 148, 122, "체력을 깎고 계약하십시오.", Color(8, 24, 32));
        } else {
            renderer.fillRect(148, 44, 44, 12, Color(8, 24, 32));
            FontRenderer::drawText(renderer, 150, 45, "[미확인]", Color(224, 248, 208));

            FontRenderer::drawText(renderer, 148, 64, "[주요 서식지]", Color(52, 104, 86));
            FontRenderer::drawText(renderer, 148, 76, getHabitat(selected->number), Color(52, 104, 86));

            FontRenderer::drawText(renderer, 148, 98, "조선의 미지의 요괴입니다.", Color(52, 104, 86));
            FontRenderer::drawText(renderer, 148, 114, "해당 권역을 탐험하여", Color(52, 104, 86));
            FontRenderer::drawText(renderer, 148, 128, "새로운 요괴를 마주하십시오.", Color(8, 24, 32));
        }
    }

    // Bottom Help Bar
    renderer.draw9SliceBox(4, 164, 312, 14, UITheme::Inverted);
    FontRenderer::drawText(renderer, 10, 166, "방향키:스크롤 | 좌/우:10개 점프 | X/Z:닫기", Color(224, 248, 208));
}

} // namespace JoseonRPG
