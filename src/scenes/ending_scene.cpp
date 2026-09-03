#include "ending_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/scene_stack.hpp"
#include "../ui/font_renderer.hpp"
#include "../audio/audio_engine.hpp"

namespace JoseonRPG {

EndingScene::EndingScene(const Party& party, const ArtifactInventory& artifacts, const Encyclopedia& encyclopedia)
    : m_party(party), m_artifacts(artifacts), m_encyclopedia(encyclopedia), m_scrollOffset(0.0f) {}

void EndingScene::onEnter() {
    AudioEngine::playBgm(BgmTrack::Title);

    int capturedCount = m_encyclopedia.getCapturedCount();
    int seenCount = m_encyclopedia.getSeenCount();
    int artCount = static_cast<int>(m_artifacts.getCount());

    m_creditLines = {
        "=== 108: 음양견문록 ===",
        "Between Light and Shadow",
        "",
        "★ 조선의 평화가 마침내 회복되었습니다 ★",
        "",
        "--- [플레이어 최종 결산 기록] ---",
        "108 요괴도감 포획: " + std::to_string(capturedCount) + " / 108 종",
        "108 요괴도감 조우: " + std::to_string(seenCount) + " / 108 종",
        "조선 설화 유물 수집: " + std::to_string(artCount) + " / 24 점",
        "출전 파티 정예 요괴 수: " + std::to_string(m_party.getSize()) + " 마리",
        "",
        "--- [원작 및 개발 기획] ---",
        "기획 & 시나리오: 조선 설화 연구회",
        "게임 시스템 설계: 1v1 턴제 전략 음양 배틀 엔진",
        "사운드 엔지니어링: 4채널 국악 5음계 칩튠 신디사이저",
        "도감 & 그래픽스: 320x180 8-Bit 레트로 엔진",
        "",
        "--- [기술적 성과] ---",
        "순수 C++17 스탠드얼론 아키텍처",
        "1.44MB 플로피 디스크 용량 완벽 수용 (380KB대)",
        "외부 종속성 0개 (Zero Dependencies)",
        "",
        "플레이해 주셔서 대단히 감사합니다!",
        "",
        "[ Z / Space 키를 눌러 타이틀로 복귀 ]"
    };
}

void EndingScene::handleInput() {
    if (Input::isPressed(Key::ActionA) || Input::isPressed(Key::ActionB)) {
        if (m_sceneStack) {
            m_sceneStack->popScene();
        }
    }
}

void EndingScene::update(float dt) {
    m_scrollOffset += dt * 16.0f; // Scroll speed
}

void EndingScene::render(Renderer& renderer) {
    // Midnight Sky backdrop
    renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(10, 12, 18));

    // Draw scrolling lines
    for (size_t i = 0; i < m_creditLines.size(); ++i) {
        int y = static_cast<int>(SCREEN_HEIGHT + i * 16 - m_scrollOffset);
        if (y < -16 || y > SCREEN_HEIGHT) continue;

        const auto& line = m_creditLines[i];
        int textWidth = static_cast<int>(line.length()) * 6; // Approximate font width
        int x = std::max(10, (SCREEN_WIDTH - textWidth) / 2);

        Color col = Palette::White;
        if (line.find("===") != std::string::npos || line.find("★") != std::string::npos) {
            col = Palette::Yellow;
        } else if (line.find("---") != std::string::npos) {
            col = Palette::Jade;
        } else if (line.find("감사합니다") != std::string::npos) {
            col = Palette::Yellow;
        }

        FontRenderer::drawText(renderer, x, y, line, col);
    }
}

} // namespace JoseonRPG
