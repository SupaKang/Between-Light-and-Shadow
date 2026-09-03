#include "party_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/scene_stack.hpp"
#include "../ui/font_renderer.hpp"
#include <algorithm>

namespace JoseonRPG {

PartyScene::PartyScene(Party& party)
    : m_party(party) {}

void PartyScene::onEnter() {
    m_cursor = 0;
    m_feedbackMsg.clear();
}

void PartyScene::handleInput() {
    if (Input::isRepeated(Key::Up)) {
        if (m_cursor > 0) m_cursor--;
    }
    if (Input::isRepeated(Key::Down)) {
        if (m_cursor + 1 < static_cast<int>(m_party.getSize())) m_cursor++;
    }

    if (Input::isPressed(Key::ActionA)) {
        Yokai* selected = m_party.getYokai(m_cursor);
        if (selected) {
            if (selected->canPromote()) {
                selected->promoteGrade();
                m_feedbackMsg = selected->getName() + " 승급 성공! (Grade " +
                                std::to_string(static_cast<int>(selected->getGrade())) + ")";
            } else {
                m_feedbackMsg = "승급 조건 미충족 (필요: Lv.10/20/35/50)";
            }
        }
    }

    if (Input::isPressed(Key::ActionB) || Input::isPressed(Key::ActionD) || Input::isPressed(Key::Debug)) {
        if (m_sceneStack) {
            m_sceneStack->popScene();
        }
    }
}

void PartyScene::update(float /*dt*/) {}

void PartyScene::render(Renderer& renderer) {
    renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(18, 22, 30));

    // Header Banner
    renderer.drawPanel(4, 4, 312, 20, Color(26, 32, 44), Palette::Blue);
    FontRenderer::drawText(renderer, 10, 8, "=== 파티 관리 및 요괴 승급 (PARTY & GROWTH) ===", Palette::Yellow);

    // Left Party Member List
    renderer.drawPanel(4, 26, 140, 136, Color(16, 20, 28), Palette::MidGray);
    for (size_t i = 0; i < m_party.getSize(); ++i) {
        const Yokai* member = m_party.getYokai(i);
        if (!member) continue;
        int py = 32 + static_cast<int>(i) * 42;

        if (m_cursor == static_cast<int>(i)) {
            renderer.drawPanel(6, py - 2, 136, 40, Color(36, 44, 60), Palette::Yellow);
            FontRenderer::drawText(renderer, 8, py + 2, ">", Palette::Yellow);
        }

        FontRenderer::drawText(renderer, 16, py + 2, member->getName() + " Lv." + std::to_string(member->getLevel()), Palette::White);
        std::string gStr = "Grade " + std::to_string(static_cast<int>(member->getGrade()));
        FontRenderer::drawText(renderer, 92, py + 2, gStr, Palette::Yellow);

        std::string hpStr = "HP " + std::to_string(member->getStats().hp) + "/" + std::to_string(member->getStats().maxHp);
        FontRenderer::drawText(renderer, 16, py + 14, hpStr, Palette::Jade);

        int expNext = member->getExpToNextLevel();
        std::string expStr = "EXP " + std::to_string(member->getExp()) + "/" + std::to_string(expNext);
        FontRenderer::drawText(renderer, 16, py + 24, expStr, Palette::LightGray);
    }

    // Right Member Details
    renderer.drawPanel(148, 26, 168, 136, Color(16, 20, 28), Palette::MidGray);
    const Yokai* selectedMember = m_party.getYokai(m_cursor);
    if (selectedMember) {
        FontRenderer::drawText(renderer, 154, 32, "== " + selectedMember->getName() + " 상세 능력치 ==", Palette::Yellow);
        FontRenderer::drawText(renderer, 154, 46, "완력(ATK): " + std::to_string(selectedMember->getStats().atk), Palette::White);
        FontRenderer::drawText(renderer, 230, 46, "호신(DEF): " + std::to_string(selectedMember->getStats().def), Palette::White);
        FontRenderer::drawText(renderer, 154, 58, "신법(SPD): " + std::to_string(selectedMember->getStats().spd), Palette::White);
        FontRenderer::drawText(renderer, 230, 58, "영력(Qi) : " + std::to_string(selectedMember->getStats().maxQi), Palette::Jade);

        FontRenderer::drawText(renderer, 154, 74, "[장착 기술 목록]", Palette::LightGray);
        const auto& skills = selectedMember->getSkills();
        for (size_t s = 0; s < skills.size() && s < 4; ++s) {
            int sky = 86 + static_cast<int>(s) * 11;
            FontRenderer::drawText(renderer, 154, sky, std::to_string(s + 1) + "." + skills[s].name, Palette::White);
        }

        renderer.drawPanel(154, 132, 156, 24, selectedMember->canPromote() ? Color(20, 60, 30) : Color(40, 40, 40), Palette::Yellow);
        if (selectedMember->canPromote()) {
            FontRenderer::drawText(renderer, 162, 139, "[Z키: 등급 승급(Promotion)!]", Palette::Yellow);
        } else {
            FontRenderer::drawText(renderer, 160, 139, "승급 조건 미충족 (Lv부족)", Palette::MidGray);
        }
    }

    if (!m_feedbackMsg.empty()) {
        renderer.fillRect(4, 148, 140, 12, Color(20, 50, 30));
        FontRenderer::drawText(renderer, 8, 150, m_feedbackMsg, Palette::Yellow);
    }

    // Bottom Help Bar
    renderer.fillRect(0, SCREEN_HEIGHT - 14, SCREEN_WIDTH, 14, Palette::Black);
    FontRenderer::drawText(renderer, 4, SCREEN_HEIGHT - 11, "방향키:요괴선택 | Z:승급 | X/V:닫기", Palette::White);
}

} // namespace JoseonRPG
