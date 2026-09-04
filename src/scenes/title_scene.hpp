#pragma once
#include "../core/scene.hpp"
#include "../core/types.hpp"
#include "../gameplay/party.hpp"
#include "../gameplay/artifact.hpp"
#include <string>
#include <vector>

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

    void initWisps();
    void updateWisps(float dt);
    void renderBackgroundLandscape(Renderer& renderer);
    void renderTitleLogo(Renderer& renderer);
    void renderMenu(Renderer& renderer);

    Party& m_party;
    ArtifactInventory& m_artifacts;
    int& m_money;

    int m_cursor = 0;
    bool m_hasSave = false;
    std::string m_saveSummary;
    float m_animTimer = 0.0f;
    float m_promptBlink = 0.0f;
    bool m_menuActivated = false;
    float m_menuSlide = 0.0f;

    struct SpiritWisp {
        float x = 0.0f;
        float y = 0.0f;
        float speed = 0.0f;
        float phase = 0.0f;
        float size = 0.0f;
        Color color;
    };
    std::vector<SpiritWisp> m_wisps;
};

} // namespace JoseonRPG
