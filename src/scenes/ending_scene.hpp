#pragma once
#include "../core/scene.hpp"
#include "../core/types.hpp"
#include "../gameplay/party.hpp"
#include "../gameplay/artifact.hpp"
#include "../gameplay/encyclopedia.hpp"
#include <vector>
#include <string>

namespace JoseonRPG {

class EndingScene : public IScene {
public:
    EndingScene(const Party& party, const ArtifactInventory& artifacts, const Encyclopedia& encyclopedia);

    void onEnter() override;
    void handleInput() override;
    void update(float dt) override;
    void render(Renderer& renderer) override;

private:
    const Party& m_party;
    const ArtifactInventory& m_artifacts;
    const Encyclopedia& m_encyclopedia;

    float m_scrollOffset = 0.0f;
    std::vector<std::string> m_creditLines;
    bool m_finished = false;
};

} // namespace JoseonRPG
