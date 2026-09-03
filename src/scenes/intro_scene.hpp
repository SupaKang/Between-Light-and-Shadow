#pragma once
#include "../core/scene_stack.hpp"
#include "../gameplay/party.hpp"
#include "../gameplay/artifact.hpp"
#include <string>
#include <vector>

namespace JoseonRPG {

class IntroScene : public IScene {
public:
    IntroScene(Party& party, ArtifactInventory& artifacts, int& money);
    ~IntroScene() override = default;

    void onEnter() override;
    void handleInput() override;
    void update(float dt) override;
    void render(Renderer& renderer) override;

private:
    Party& m_party;
    ArtifactInventory& m_artifacts;
    int& m_money;

    struct StorySlide {
        std::string title;
        std::vector<std::string> lines;
        int backgroundType; // 0=Genesis Chaos, 1=Joseon Eclipse, 2=Exorcist Order, 3=Yokai Bestiary
    };

    std::vector<StorySlide> m_slides;
    size_t m_currentSlide = 0;
    float m_slideTimer = 0.0f;
    float m_fadeAlpha = 1.0f;
    bool m_isFadingOut = false;
    
    void proceedToWorld();
};

} // namespace JoseonRPG
