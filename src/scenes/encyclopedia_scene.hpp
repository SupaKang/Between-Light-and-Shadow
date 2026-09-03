#pragma once
#include "../core/scene.hpp"
#include "../core/types.hpp"
#include "../gameplay/encyclopedia.hpp"

namespace JoseonRPG {

class EncyclopediaScene : public IScene {
public:
    EncyclopediaScene(Encyclopedia& encyclopedia);

    void onEnter() override;
    void handleInput() override;
    void update(float dt) override;
    void render(Renderer& renderer) override;

    bool isModal() const override { return false; }

private:
    Encyclopedia& m_encyclopedia;
    int m_cursor = 1; // 1..108
};

} // namespace JoseonRPG
