#include "ending_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/scene_stack.hpp"
#include "../ui/font_renderer.hpp"
#include "../audio/audio_engine.hpp"

namespace JoseonRPG {

EndingScene::EndingScene(const Party& party, const ArtifactInventory& artifacts, const Encyclopedia& encyclopedia, EndingType endingType)
    : m_party(party), m_artifacts(artifacts), m_encyclopedia(encyclopedia), m_endingType(endingType), m_scrollOffset(0.0f) {}

void EndingScene::onEnter() {
    AudioEngine::playBgm(BgmTrack::Title);
    buildCredits();
}

void EndingScene::buildCredits() {
    int capturedCount = m_encyclopedia.getCapturedCount();
    int seenCount = m_encyclopedia.getSeenCount();
    int artCount = static_cast<int>(m_artifacts.getCount());

    m_creditLines.clear();

    if (m_endingType == EndingType::FirstEnding) {
        m_creditLines = {
            "=== 108: 음양견문록 ===",
            "Between Light and Shadow",
            "",
            "★ [1차 엔딩: 음양당의 몰락과 조선의 여명] ★",
            "",
            "음양당 당주 묵영의 야망이 분쇄되고 일식의 성채가 정화되었습니다.",
            "사악한 흑마술의 안개가 걷히고, 한양 도성에 평화로운 새벽빛이 밝아옵니다.",
            "",
            "--- [1차 캠페인 결산 기록] ---",
            "108 요괴도감 계약: " + std::to_string(capturedCount) + " / 108 종",
            "108 요괴도감 조우: " + std::to_string(seenCount) + " / 108 종",
            "조선 설화 유물 수집: " + std::to_string(artCount) + " / 27 점",
            "정예 출전 요괴: " + std::to_string(m_party.getSize()) + " 마리",
            "",
            "--- [2차 진엔딩 에필로그 안내] ---",
            "하지만 아직 삼천리 강토에 108 요괴의 영맥이 흩어져 있습니다.",
            "진정한 조선의 평화와 영혼들의 구원을 위해서는",
            "★ 108종 모든 요괴와 도감 계약을 완수해야 합니다! ★",
            "",
            "[2차 진엔딩 해금 조건]:",
            "1. 108종 요괴 전종 포획 (108/108 달성)",
            "2. 태초의 천상 신역 삼신단 (Map 36) 진입",
            "3. 태초의 삼신제석 격파 후 [성불 등선] vs [이승 공존] 선택",
            "",
            "--- [제작 및 엔진] ---",
            "기획 & 시나리오: 조선 설화 연구회",
            "게임 시스템: 1v1 턴제 전략 음양 배틀 엔진",
            "사운드: 4채널 국악 5음계 칩튠 신디사이저",
            "렌더러: 320x240 8-Bit 순수 절차적 도트 엔진 (1.44MB)",
            "",
            "1차 엔딩 달성을 진심으로 축하합니다!",
            "[ Z / Space / X 키를 눌러 모험 계속하기 ]"
        };
    } else if (m_endingType == EndingType::TrueEndingAscension) {
        m_creditLines = {
            "=== 108: 음양견문록 ===",
            "Between Light and Shadow",
            "",
            "★ [2차 진엔딩: 108 성불(成佛)과 영겁의 등선(登仙)] ★",
            "",
            "소슬은 천상 신역 삼신단에서 108 번뇌의 넋을 모두 해탈의 빛으로 인도하였다.",
            "팔도의 요괴들은 은하수 같은 빛무리로 승화하여 북두칠성의 성좌가 되었고,",
            "소슬은 육신의 번뇌를 벗어나 조선을 영원히 굽어살피는 [천상 국선(國仙)]으로 등선하였다.",
            "",
            "--- [조선 인물들의 찬사] ---",
            "도선사 도선국사: '나무아미타불... 108 번뇌가 스러져 삼천리에 백옥 정토가 열렸구려.'",
            "금강산 백운선사: '허허! 속세의 연을 털고 마침내 선계의 진정한 도반이 되었군!'",
            "암행어사 박문수: '하늘에 빛나는 108 성좌가 조선 만백성을 영원히 굽어살필 것이오.'",
            "백두산 백록령: '순백의 천지 정기가 국선의 거룩한 등선을 영원히 찬미합니다.'",
            "",
            "--- [궁극의 위업 달성 결산] ---",
            "108 요괴도감 계약: 108 / 108 종 (100.0% 완전 성불)",
            "조선 설화 유물 수집: " + std::to_string(artCount) + " / 27 점",
            "최종 엔딩 칭호: [태초의 천상 국선 (Supreme Immortal)]",
            "",
            "--- [제작 및 감사] ---",
            "기획 & 시나리오: 조선 설화 연구회",
            "완전 무결 스탠드얼론 1.44MB 레트로 JRPG",
            "Zero Dependencies / Pure C++17 Engine",
            "",
            "108: 음양견문록의 모든 전설을 완성해 주셔서 진심으로 감사합니다!",
            "[ Z / Space / X 키를 눌러 타이틀로 복귀 ]"
        };
    } else { // TrueEndingCoexistence
        m_creditLines = {
            "=== 108: 음양견문록 ===",
            "Between Light and Shadow",
            "",
            "★ [2차 진엔딩: 만백성과 요괴의 영원한 벗 (이승 공존)] ★",
            "",
            "소슬은 천상의 등선을 사양하고, 108 요괴와 함께 조선의 산천에 머물기로 결단하였다.",
            "도깨비는 백성의 장작을 패고, 불가사리는 전란의 흉기를 삼키며,",
            "구미호는 선비들의 길잡이가 되어 사람과 요괴가 어우러지는 태평성대가 열렸다.",
            "소슬은 백성들에게 영원히 사랑받는 [조선 제일 대영술사(大靈術士)]로 역사에 남았다.",
            "",
            "--- [조선 인물들의 찬사] ---",
            "주막마을 주모: '영술사 나리 덕분에 우리 주막 도깨비가 잔칫날 가마솥을 날라준다오!'",
            "흑산도 해녀 진주: '심해 요괴들이 거친 파도로부터 우리 해녀들의 숨비소리를 지켜줘요!'",
            "암행어사 박문수: '사람과 영물이 화합하는 세상, 이것이야말로 진정한 조선의 이상향이오!'",
            "관상감 류 영감: '팔도강산에 만수무강의 기운이 넘치니, 대영술사님의 위대한 덕택입니다.'",
            "",
            "--- [궁극의 위업 달성 결산] ---",
            "108 요괴도감 계약: 108 / 108 종 (100.0% 영원한 동반)",
            "조선 설화 유물 수집: " + std::to_string(artCount) + " / 27 점",
            "최종 엔딩 칭호: [조선 제일 대영술사 (Grand Folk Exorcist)]",
            "",
            "--- [제작 및 감사] ---",
            "기획 & 시나리오: 조선 설화 연구회",
            "완전 무결 스탠드얼론 1.44MB 레트로 JRPG",
            "Zero Dependencies / Pure C++17 Engine",
            "",
            "108: 음양견문록의 대장정을 함께해 주셔서 대단히 감사합니다!",
            "[ Z / Space / X 키를 눌러 타이틀로 복귀 ]"
        };
    }
}

void EndingScene::handleInput() {
    if (Input::isPressed(Key::ActionA) || Input::isPressed(Key::ActionB)) {
        if (m_sceneStack) {
            m_sceneStack->popScene();
        }
    }
}

void EndingScene::update(float dt) {
    m_scrollOffset += dt * 18.0f; // Smooth scroll speed
}

void EndingScene::render(Renderer& renderer) {
    // Midnight Sky backdrop with subtle star field
    renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(10, 12, 18));

    // Draw scrolling lines
    for (size_t i = 0; i < m_creditLines.size(); ++i) {
        int y = static_cast<int>(SCREEN_HEIGHT + i * 16 - m_scrollOffset);
        if (y < -16 || y > SCREEN_HEIGHT) continue;

        const auto& line = m_creditLines[i];
        int textWidth = static_cast<int>(line.length()) * 6;
        int x = std::max(10, (SCREEN_WIDTH - textWidth) / 2);

        Color col = Palette::White;
        if (line.find("===") != std::string::npos || line.find("★") != std::string::npos) {
            col = Palette::Yellow;
        } else if (line.find("---") != std::string::npos) {
            col = Palette::Jade;
        } else if (line.find("감사합니다") != std::string::npos || line.find("축하합니다") != std::string::npos) {
            col = Palette::Yellow;
        } else if (line.find("칭호:") != std::string::npos) {
            col = Palette::BichuiJade;
        }

        FontRenderer::drawText(renderer, x, y, line, col);
    }
}

} // namespace JoseonRPG
