#pragma once
#include "../core/scene.hpp"
#include "../core/types.hpp"
#include "../gameplay/party.hpp"
#include "../gameplay/artifact.hpp"
#include "../gameplay/encyclopedia.hpp"
#include <vector>
#include <string>

namespace JoseonRPG {

enum class EndingType {
    FirstEnding = 0,            // 1차 엔딩 (당주 묵영 격파, 1부 완결)
    TrueEndingAscension = 1,    // 2차 진엔딩 A (108 요괴 성불 & 신선 등선)
    TrueEndingCoexistence = 2   // 2차 진엔딩 B (108 요괴 이승 공존 & 조선 수호)
};

class EndingScene : public IScene {
public:
    EndingScene(const Party& party, const ArtifactInventory& artifacts, const Encyclopedia& encyclopedia, EndingType endingType = EndingType::FirstEnding);

    void onEnter() override;
    void handleInput() override;
    void update(float dt) override;
    void render(Renderer& renderer) override;

    bool isModal() const override { return false; }

private:
    void buildCredits();

    const Party& m_party;
    const ArtifactInventory& m_artifacts;
    const Encyclopedia& m_encyclopedia;
    EndingType m_endingType;

    float m_scrollOffset = 0.0f;
    std::vector<std::string> m_creditLines;
    bool m_choosingTrueEnding = false;
    int m_choiceCursor = 0;
};

} // namespace JoseonRPG
