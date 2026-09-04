#include "quest_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/scene_stack.hpp"
#include "../ui/font_renderer.hpp"
#include <algorithm>

namespace JoseonRPG {

QuestScene::QuestScene(QuestManager& questManager)
    : m_questManager(questManager) {}

void QuestScene::onEnter() {
    m_cursor = 0;
}

void QuestScene::handleInput() {
    auto allQuests = m_questManager.getAllQuests();
    if (Input::isRepeated(Key::Up)) {
        if (m_cursor > 0) m_cursor--;
    }
    if (Input::isRepeated(Key::Down)) {
        if (m_cursor + 1 < static_cast<int>(allQuests.size())) m_cursor++;
    }

    if (Input::isPressed(Key::ActionB) || Input::isPressed(Key::ActionA) || Input::isPressed(Key::ActionE)) {
        if (m_sceneStack) {
            m_sceneStack->popScene();
        }
    }
}

void QuestScene::update(float /*dt*/) {}

void QuestScene::render(Renderer& renderer) {
    renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(224, 248, 208));

    // Header Banner
    renderer.draw9SliceBox(4, 4, 312, 20, UITheme::Paper);
    FontRenderer::drawText(renderer, 10, 8, "=== 임무 일지 (QUEST JOURNAL) ===", Color(8, 24, 32));

    auto allQuests = m_questManager.getAllQuests();

    // Left Quest List
    renderer.draw9SliceBox(4, 26, 134, 136, UITheme::Paper);
    for (size_t i = 0; i < allQuests.size() && i < 6; ++i) {
        int qy = 32 + static_cast<int>(i) * 22;
        const auto& q = allQuests[i];
        bool isCur = (m_cursor == static_cast<int>(i));

        if (isCur) {
            renderer.fillRect(8, qy - 2, 126, 20, Color(136, 192, 112));
            FontRenderer::drawText(renderer, 10, qy + 4, "▶", Color(8, 24, 32));
        }

        Color titleCol = Color(8, 24, 32);
        FontRenderer::drawText(renderer, 20, qy + 1, q.titleKo.substr(0, 14), titleCol);

        std::string stStr = (q.state == QuestState::Completed) ? "[완료]" : (q.state == QuestState::InProgress ? "[진행중]" : "[미수주]");
        Color stCol = (q.state == QuestState::Completed) ? Color(136, 192, 112) : (q.state == QuestState::InProgress ? Color(52, 104, 86) : Color(136, 192, 112));
        FontRenderer::drawText(renderer, 20, qy + 11, stStr, stCol);
    }

    // Right Quest Details
    renderer.draw9SliceBox(142, 26, 174, 136, UITheme::Paper);
    if (m_cursor < static_cast<int>(allQuests.size())) {
        const auto& q = allQuests[m_cursor];
        FontRenderer::drawText(renderer, 148, 30, q.titleKo, Color(8, 24, 32));

        std::string typeStr = (q.type == QuestType::Main) ? "구분: 메인 스토리" : "구분: 조선 설화 서브퀘스트";
        FontRenderer::drawText(renderer, 148, 44, typeStr, Color(52, 104, 86));

        FontRenderer::drawText(renderer, 148, 58, "[현재 목표]", Color(52, 104, 86));
        FontRenderer::drawText(renderer, 148, 70, q.getCurrentObjective(), Color(8, 24, 32));

        FontRenderer::drawText(renderer, 148, 94, "[보상 내역]", Color(52, 104, 86));
        std::string rwdStr = "엽전 " + std::to_string(q.reward.money) + "냥 + EXP " + std::to_string(q.reward.exp);
        FontRenderer::drawText(renderer, 148, 106, rwdStr, Color(8, 24, 32));
        if (!q.reward.artifactId.empty()) {
            FontRenderer::drawText(renderer, 148, 118, "유물: " + q.reward.artifactId, Color(8, 24, 32));
        }
    }

    // Bottom Help Bar
    renderer.draw9SliceBox(4, 164, 312, 14, UITheme::Inverted);
    FontRenderer::drawText(renderer, 10, 166, "방향키:임무선택 | X/Q:닫기", Color(224, 248, 208));
}

} // namespace JoseonRPG
