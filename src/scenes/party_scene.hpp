#pragma once
#include "../core/scene.hpp"
#include "../core/types.hpp"
#include "../gameplay/party.hpp"
#include <string>

namespace JoseonRPG {

class PartyScene : public IScene {
public:
    PartyScene(Party& party);

    void onEnter() override;
    void handleInput() override;
    void update(float dt) override;
    void render(Renderer& renderer) override;

    bool isModal() const override { return false; }

private:
    Party& m_party;
    int m_cursor = 0;
    std::string m_feedbackMsg;
};

} // namespace JoseonRPG
