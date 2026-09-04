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
    renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(224, 248, 208));

    // Header Banner
    renderer.draw9SliceBox(4, 4, 312, 20, UITheme::Paper);
    FontRenderer::drawText(renderer, 10, 8, "=== 사역 요괴 관리 및 승급 (PARTY & GROWTH) ===", Color(8, 24, 32));

    // Left Party Member List
    renderer.draw9SliceBox(4, 26, 134, 136, UITheme::Paper);
    for (size_t i = 0; i < m_party.getSize(); ++i) {
        const Yokai* member = m_party.getYokai(i);
        if (!member) continue;
        int py = 32 + static_cast<int>(i) * 42;
        bool isCur = (m_cursor == static_cast<int>(i));

        if (isCur) {
            renderer.fillRect(8, py - 2, 126, 40, Color(136, 192, 112));
            FontRenderer::drawText(renderer, 10, py + 2, "▶", Color(8, 24, 32));
        }

        FontRenderer::drawText(renderer, 20, py + 2, member->getName() + " Lv." + std::to_string(member->getLevel()), Color(8, 24, 32));
        std::string gStr = "G." + std::to_string(static_cast<int>(member->getGrade()));
        FontRenderer::drawText(renderer, 96, py + 2, gStr, Color(52, 104, 86));

        std::string hpStr = "HP " + std::to_string(member->getStats().hp) + "/" + std::to_string(member->getStats().maxHp);
        FontRenderer::drawText(renderer, 20, py + 14, hpStr, Color(52, 104, 86));
        renderer.drawGaugeBar(20, py + 24, 104, 4, member->getStats().hp, member->getStats().maxHp, Color(52, 104, 86));

        int expNext = member->getExpToNextLevel();
        std::string expStr = "EXP " + std::to_string(member->getExp()) + "/" + std::to_string(expNext);
        FontRenderer::drawText(renderer, 20, py + 30, expStr, Color(8, 24, 32));
    }

    // Right Member Details
    renderer.draw9SliceBox(142, 26, 174, 136, UITheme::Paper);
    const Yokai* selectedMember = m_party.getYokai(m_cursor);
    if (selectedMember) {
        FontRenderer::drawText(renderer, 148, 30, "== " + selectedMember->getName() + " 상세 능력치 ==", Color(8, 24, 32));
        FontRenderer::drawText(renderer, 148, 44, "완력(ATK): " + std::to_string(selectedMember->getStats().atk), Color(8, 24, 32));
        FontRenderer::drawText(renderer, 226, 44, "호신(DEF): " + std::to_string(selectedMember->getStats().def), Color(8, 24, 32));
        FontRenderer::drawText(renderer, 148, 56, "신법(SPD): " + std::to_string(selectedMember->getStats().spd), Color(8, 24, 32));
        FontRenderer::drawText(renderer, 226, 56, "영력(Qi) : " + std::to_string(selectedMember->getStats().maxQi), Color(52, 104, 86));

        FontRenderer::drawText(renderer, 148, 72, "[장착 기술 목록]", Color(52, 104, 86));
        const auto& skills = selectedMember->getSkills();
        for (size_t s = 0; s < skills.size() && s < 4; ++s) {
            int sky = 84 + static_cast<int>(s) * 12;
            FontRenderer::drawText(renderer, 148, sky, std::to_string(s + 1) + "." + skills[s].name, Color(8, 24, 32));
        }

        renderer.draw9SliceBox(148, 132, 162, 24, selectedMember->canPromote() ? UITheme::Paper : UITheme::Inverted);
        if (selectedMember->canPromote()) {
            FontRenderer::drawText(renderer, 156, 139, "[Z키: 등급 승급(Promotion)!]", Color(8, 24, 32));
        } else {
            FontRenderer::drawText(renderer, 154, 139, "승급 조건 미충족 (Lv부족)", Color(224, 248, 208));
        }
    }

    if (!m_feedbackMsg.empty()) {
        renderer.draw9SliceBox(6, 144, 130, 16, UITheme::Inverted);
        FontRenderer::drawText(renderer, 10, 147, m_feedbackMsg, Color(224, 248, 208));
    }

    // Bottom Help Bar
    renderer.draw9SliceBox(4, 164, 312, 14, UITheme::Inverted);
    FontRenderer::drawText(renderer, 10, 166, "방향키:요괴선택 | Z:승급 | X/V:닫기", Color(224, 248, 208));
}

} // namespace JoseonRPG
