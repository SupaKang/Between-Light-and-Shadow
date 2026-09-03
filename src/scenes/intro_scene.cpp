#include "intro_scene.hpp"
#include "../world/world_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../ui/font_renderer.hpp"
#include "../data/data_manager.hpp"
#include "../audio/audio_engine.hpp"
#include <algorithm>

namespace JoseonRPG {

IntroScene::IntroScene(Party& party, ArtifactInventory& artifacts, int& money)
    : m_party(party), m_artifacts(artifacts), m_money(money) {}

void IntroScene::onEnter() {
    AudioEngine::playBgm(BgmTrack::Title);

    m_slides = {
        {
            "【 삿갓 쓴 낭인의 옛이야기 】",
            {
                "주막 평상에 걸터앉은 삿갓 쓴 낭인이",
                "막걸리 사발을 기울이며 옛일을 읊조린다.",
                "",
                "\"이보게, 젊은이... 귀신과 요괴가 헛것인 줄 아는가?",
                "삼천리 강산에 백여덟 신령이 터를 잡고 살았지.",
                "사람과 요괴가 산천의 순리를 지켰더랬네.\""
            },
            0
        },
        {
            "【 피빛 일식과 요괴들의 비명 】",
            {
                "\"허나... 음양당 놈들이 금지된 음기 도술로",
                "백두대간의 영맥을 끊고 정기를 뒤흔들었지.",
                "",
                "순박하던 도깨비도, 산을 지키던 신령들도",
                "모두 미쳐 날뛰는 괴물로 전락해 버렸네.\""
            },
            1
        },
        {
            "【 영술사, 홀로 길을 나서다 】",
            {
                "\"조정에서는 요기를 다스릴 인재를 찾지 못했으나,",
                "자네는 호위 무사도 없이 홀로 길을 나섰더군.",
                "",
                "오직 품속의 '벽사의 부적'과 흑립 갓 하나로",
                "험악한 도선사 고갯길에 발을 디뎠네...\""
            },
            2
        },
        {
            "【 108: 음양견문록의 시작 】",
            {
                "\"홀몸으로 나서는 길이니 목숨을 걸게나.",
                "날뛰는 야생 요괴를 부적으로 제압하고 계약하게.",
                "백여덟 요괴를 거느려 음양당을 꺾을지는",
                "오직 자네 손에 달렸네.\"",
                "",
                "  [ Z 키를 눌러 여정을 시작하십시오 ]"
            },
            3
        }
    };

    m_currentSlide = 0;
    m_slideTimer = 0.0f;
    m_fadeAlpha = 1.0f;
    m_isFadingOut = false;
}

void IntroScene::proceedToWorld() {
    // 3번 요청 반영: 스타팅 요괴 지급 전면 삭제! 음양사 혈혈단신(솔로)으로 모험 시작!
    m_party.clear();
    // Exorcist starts as a pure solo talisman user (No starter monsters in party)
    
    m_artifacts.clear();
    m_money = 300; // Starter money for talismans and tavern

    // Spawn inside Map 1 (도선사 주막 본채 실내 12, 12) after hearing Ronin's tale
    auto world = std::make_unique<WorldScene>(m_party, m_artifacts, m_money);
    world->setPlayerPosition(12, 12, 1);

    if (m_sceneStack) {
        m_sceneStack->clearAndSet(std::move(world));
    }
}

void IntroScene::handleInput() {
    if (Input::isPressed(Key::ActionA) || Input::isPressed(Key::ActionB) || Input::isPressed(Key::Menu)) {
        AudioEngine::playSfx(SfxId::MenuSelect);
        if (m_currentSlide + 1 < m_slides.size()) {
            m_currentSlide++;
            m_fadeAlpha = 0.8f;
        } else {
            proceedToWorld();
        }
    }
}

void IntroScene::update(float dt) {
    if (m_fadeAlpha > 0.0f) {
        m_fadeAlpha = std::max(0.0f, m_fadeAlpha - dt * 2.0f);
    }
    m_slideTimer += dt;
}

void IntroScene::render(Renderer& renderer) {
    // 1. Authentic Gen 1 DMG 4-Shade Cinematic Canvas
    renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(8, 24, 32)); // Darkest DMG Base

    if (m_currentSlide >= m_slides.size()) return;
    const auto& slide = m_slides[m_currentSlide];

    // Background Decorative Graphic (Traditional Mountain / Celestial Frame)
    renderer.fillRect(8, 8, SCREEN_WIDTH - 16, SCREEN_HEIGHT - 16, Color(52, 104, 86));
    renderer.fillRect(12, 12, SCREEN_WIDTH - 24, SCREEN_HEIGHT - 24, Color(8, 24, 32));
    renderer.drawRect(10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20, Color(224, 248, 208));

    // Slide Header Title
    FontRenderer::drawText(renderer, 24, 22, slide.title, Color(224, 248, 208));
    renderer.drawLine(24, 36, SCREEN_WIDTH - 24, 36, Color(136, 192, 112));

    // Slide Body Lines
    int curY = 48;
    for (const auto& line : slide.lines) {
        if (!line.empty()) {
            FontRenderer::drawText(renderer, 26, curY, line, Color(224, 248, 208));
        }
        curY += 16;
    }

    // Bottom Navigation Prompt
    std::string pageStr = "[" + std::to_string(m_currentSlide + 1) + "/" + std::to_string(m_slides.size()) + "] Z:다음 (Space:스킵)";
    FontRenderer::drawText(renderer, SCREEN_WIDTH - 186, SCREEN_HEIGHT - 22, pageStr, Color(136, 192, 112));

    // Screen Fade Transition
    if (m_fadeAlpha > 0.001f) {
        renderer.applyFade(1.0f - m_fadeAlpha);
    }
}

} // namespace JoseonRPG
