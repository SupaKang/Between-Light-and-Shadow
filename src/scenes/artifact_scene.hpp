#pragma once
#include "../core/scene.hpp"
#include "../core/types.hpp"
#include "../gameplay/artifact.hpp"
#include "../gameplay/party.hpp"
#include <string>

namespace JoseonRPG {

class ArtifactScene : public IScene {
public:
    ArtifactScene(ArtifactInventory& artifacts, Party& party);

    void onEnter() override;
    void handleInput() override;
    void update(float dt) override;
    void render(Renderer& renderer) override;

    bool isModal() const override { return false; }

private:
    ArtifactInventory& m_artifacts;
    Party& m_party;
    int m_cursor = 0;
    std::string m_feedbackMsg;
};

} // namespace JoseonRPG
