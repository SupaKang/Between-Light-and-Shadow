#pragma once
#include "../core/scene.hpp"
#include "../core/types.hpp"
#include "../gameplay/party.hpp"
#include "../gameplay/artifact.hpp"
#include <string>

namespace JoseonRPG {

class TitleScene : public IScene {
public:
    TitleScene(Party& party, ArtifactInventory& artifacts, int& money);

    void onEnter() override;
    void handleInput() override;
    void update(float dt) override;
    void render(Renderer& renderer) override;

    bool isModal() const override { return false; }

private:
    void startNewGame();
    void loadSavedGame();

    Party& m_party;
    ArtifactInventory& m_artifacts;
    int& m_money;

    int m_cursor = 0;
    bool m_hasSave = false;
    std::string m_saveSummary;
    float m_animTimer = 0.0f;
};

} // namespace JoseonRPG
