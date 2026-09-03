#pragma once
#include "../core/scene.hpp"
#include "../core/types.hpp"
#include "../gameplay/quest.hpp"

namespace JoseonRPG {

class QuestScene : public IScene {
public:
    QuestScene(QuestManager& questManager);

    void onEnter() override;
    void handleInput() override;
    void update(float dt) override;
    void render(Renderer& renderer) override;

    bool isModal() const override { return false; }

private:
    QuestManager& m_questManager;
    int m_cursor = 0;
};

} // namespace JoseonRPG
