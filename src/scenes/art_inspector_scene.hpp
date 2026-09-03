#pragma once
#include "../core/scene.hpp"
#include "../core/types.hpp"
#include <string>
#include <vector>

namespace JoseonRPG {

enum class InspectorTab {
    Yokai108 = 0,
    NPCs29 = 1,
    Artifacts27 = 2,
    Tiles24 = 3
};

class ArtInspectorScene : public IScene {
public:
    ArtInspectorScene();

    void onEnter() override;
    void handleInput() override;
    void update(float dt) override;
    void render(Renderer& renderer) override;

    bool isModal() const override { return false; }

private:
    void renderYokaiTab(Renderer& renderer);
    void renderNpcTab(Renderer& renderer);
    void renderArtifactTab(Renderer& renderer);
    void renderTileTab(Renderer& renderer);

    InspectorTab m_currentTab = InspectorTab::Yokai108;
    int m_yokaiIndex = 0;       // 0 ~ 107
    int m_npcIndex = 0;         // 0 ~ 28
    int m_artifactIndex = 0;    // 0 ~ 26
    int m_tileIndex = 0;        // 0 ~ 24

    int m_zoomScale = 2;        // 1x, 2x, 4x, 8x
    bool m_silhouetteMode = false; // 1-bit monochrome silhouette testing
    float m_animTimer = 0.0f;
    int m_walkFrame = 0;
};

} // namespace JoseonRPG
