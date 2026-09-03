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
    renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(22, 24, 30));

    // Header Banner
    renderer.drawPanel(4, 4, 312, 20, Color(34, 38, 48), Palette::Jade);
    FontRenderer::drawText(renderer, 10, 8, "=== 임무 일지 (QUEST JOURNAL) ===", Palette::Yellow);

    auto allQuests = m_questManager.getAllQuests();

    // Left Quest List
    renderer.drawPanel(4, 26, 130, 136, Color(16, 18, 24), Palette::MidGray);
    for (size_t i = 0; i < allQuests.size() && i < 6; ++i) {
        int qy = 32 + static_cast<int>(i) * 22;
        const auto& q = allQuests[i];

        if (m_cursor == static_cast<int>(i)) {
            renderer.drawPanel(6, qy - 2, 126, 20, Color(36, 42, 54), Palette::Yellow);
            FontRenderer::drawText(renderer, 8, qy + 4, ">", Palette::Yellow);
        }

        Color titleCol = Palette::White;
        if (q.state == QuestState::Completed) titleCol = Palette::LightGray;
        else if (q.state == QuestState::InProgress) titleCol = Palette::Yellow;
        else titleCol = Palette::DarkGray;

        FontRenderer::drawText(renderer, 16, qy + 1, q.titleKo.substr(0, 14), titleCol);

        std::string stStr = (q.state == QuestState::Completed) ? "[완료]" : (q.state == QuestState::InProgress ? "[진행중]" : "[미수주]");
        Color stCol = (q.state == QuestState::Completed) ? Palette::LightGray : (q.state == QuestState::InProgress ? Palette::Jade : Palette::DarkGray);
        FontRenderer::drawText(renderer, 16, qy + 11, stStr, stCol);
    }

    // Right Quest Details
    renderer.drawPanel(138, 26, 178, 136, Color(16, 18, 24), Palette::MidGray);
    if (m_cursor < static_cast<int>(allQuests.size())) {
        const auto& q = allQuests[m_cursor];
        FontRenderer::drawText(renderer, 144, 32, q.titleKo, Palette::Yellow);

        std::string typeStr = (q.type == QuestType::Main) ? "구분: 메인 스토리" : "구분: 조선 설화 서브퀘스트";
        FontRenderer::drawText(renderer, 144, 46, typeStr, Palette::LightGray);

        FontRenderer::drawText(renderer, 144, 60, "[현재 목표]", Palette::Jade);
        FontRenderer::drawText(renderer, 144, 72, q.getCurrentObjective(), Palette::White);

        FontRenderer::drawText(renderer, 144, 94, "[보상 내역]", Palette::Yellow);
        std::string rwdStr = "엽전 " + std::to_string(q.reward.money) + "냥 + EXP " + std::to_string(q.reward.exp);
        FontRenderer::drawText(renderer, 144, 106, rwdStr, Palette::White);
        if (!q.reward.artifactId.empty()) {
            FontRenderer::drawText(renderer, 144, 116, "유물: " + q.reward.artifactId, Palette::Jade);
        }
    }

    // Bottom Help Bar
    renderer.fillRect(0, SCREEN_HEIGHT - 14, SCREEN_WIDTH, 14, Palette::Black);
    FontRenderer::drawText(renderer, 4, SCREEN_HEIGHT - 11, "방향키:임무선택 | X/Q:닫기", Palette::White);
}

} // namespace JoseonRPG
