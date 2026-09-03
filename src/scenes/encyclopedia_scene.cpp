#include "encyclopedia_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/scene_stack.hpp"
#include "../ui/font_renderer.hpp"
#include "../data/data_manager.hpp"
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
        std::string numPrefix = std::string("#") + ((selected->number < 10) ? "00" : (selected->number < 100 ? "0" : "")) + std::to_string(selected->number);
        std::string titleNum = numPrefix + " " + ((selected->status != DiscoveryStatus::Unseen) ? selected->nameKo : "???");
        FontRenderer::drawText(renderer, 144, 30, titleNum, Palette::Yellow);

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

        auto getElemCol = [](Element el) -> Color {
            switch (el) {
                case Element::Fire: return Palette::CinnabarRed;
                case Element::Water: return Palette::IndigoBlue;
                case Element::Earth: return Palette::GardeniaYellow;
                case Element::Light: return Palette::GoldHalo;
                case Element::Dark: return Palette::RoyalPurple;
                default: return Palette::LightGray;
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
            renderer.fillRect(144, 42, 48, 9, Palette::Jade);
            FontRenderer::drawText(renderer, 146, 43, "[계약완료]", Palette::Black);

            renderer.fillRect(196, 42, 38, 9, getElemCol(selected->element));
            FontRenderer::drawText(renderer, 198, 43, getElemName(selected->element), Palette::Black);

            std::string gStr = "G." + std::to_string(static_cast<int>(selected->baseGrade));
            FontRenderer::drawText(renderer, 240, 43, gStr, Palette::White);

            FontRenderer::drawText(renderer, 144, 54, getHabitat(selected->number), Palette::Jade);

            // Fetch template Yokai for trait and skills
            Yokai tempYokai = DataManager::createYokaiById(selected->id);
            if (tempYokai.getTrait() != YokaiTrait::None) {
                std::string trStr = "특성: " + tempYokai.getTraitName();
                FontRenderer::drawText(renderer, 144, 65, trStr, Palette::Yellow);
            } else {
                FontRenderer::drawText(renderer, 144, 65, "출처: " + selected->origin, Palette::LightGray);
            }

            // Skills Preview
            const auto& skls = tempYokai.getSkills();
            std::string sklPreview = "기술: ";
            for (size_t k = 0; k < skls.size() && k < 2; ++k) {
                if (k > 0) sklPreview += ", ";
                sklPreview += skls[k].name;
            }
            FontRenderer::drawText(renderer, 144, 76, sklPreview, Palette::White);

            // Lore / Folklore description
            FontRenderer::drawText(renderer, 144, 88, "[전승 및 배경 설화]", Palette::Yellow);
            FontRenderer::drawText(renderer, 144, 98, selected->lore.substr(0, 24), Palette::LightGray);
            if (selected->lore.length() > 24) {
                FontRenderer::drawText(renderer, 144, 108, selected->lore.substr(24, 24), Palette::LightGray);
            }
            if (selected->lore.length() > 48) {
                FontRenderer::drawText(renderer, 144, 118, selected->lore.substr(48, 24), Palette::LightGray);
            }
        } else if (selected->status == DiscoveryStatus::Seen) {
            renderer.fillRect(144, 42, 44, 9, Palette::Blue);
            FontRenderer::drawText(renderer, 146, 43, "[조우함]", Palette::Black);

            renderer.fillRect(192, 42, 38, 9, getElemCol(selected->element));
            FontRenderer::drawText(renderer, 194, 43, getElemName(selected->element), Palette::Black);

            FontRenderer::drawText(renderer, 144, 56, "[주요 서식지]", Palette::Yellow);
            FontRenderer::drawText(renderer, 144, 68, getHabitat(selected->number), Palette::Jade);

            FontRenderer::drawText(renderer, 144, 84, "전투에서 조우하였으나", Palette::LightGray);
            FontRenderer::drawText(renderer, 144, 96, "아직 계약을 맺지 못함.", Palette::LightGray);
            FontRenderer::drawText(renderer, 144, 110, "야생에서 다시 찾아", Palette::White);
            FontRenderer::drawText(renderer, 144, 120, "체력을 깎고 계약하십시오.", Palette::Yellow);
        } else {
            renderer.fillRect(144, 42, 44, 9, Palette::DarkGray);
            FontRenderer::drawText(renderer, 146, 43, "[미확인]", Palette::Black);

            FontRenderer::drawText(renderer, 144, 60, "[주요 서식지]", Palette::MidGray);
            FontRenderer::drawText(renderer, 144, 72, getHabitat(selected->number), Palette::MidGray);

            FontRenderer::drawText(renderer, 144, 92, "조선의 미지의 요괴입니다.", Palette::MidGray);
            FontRenderer::drawText(renderer, 144, 106, "해당 권역을 탐험하여", Palette::MidGray);
            FontRenderer::drawText(renderer, 144, 118, "새로운 요괴를 마주하십시오.", Palette::MidGray);
        }
    }

    // Bottom Help Bar
    renderer.fillRect(0, SCREEN_HEIGHT - 14, SCREEN_WIDTH, 14, Palette::Black);
    FontRenderer::drawText(renderer, 4, SCREEN_HEIGHT - 11, "방향키:스크롤 | 좌/우:10개 점프 | X/Z:닫기", Palette::White);
}

} // namespace JoseonRPG
