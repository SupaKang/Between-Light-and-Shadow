#pragma once
#include "../core/scene.hpp"
#include "../core/types.hpp"
#include <string>
#include <vector>
#include <random>

namespace JoseonRPG {

class Renderer;

enum class YutResult {
    Do,     // 도 (1칸)
    Gae,    // 개 (2칸)
    Geol,   // 걸 (3칸)
    Yut,    // 윷 (4칸 + 추가 던지기)
    Mo,     // 모 (5칸 + 추가 던지기)
    BackDo  // 빽도 (-1칸)
};

class YutnoriScene : public IScene {
public:
    YutnoriScene(int& playerMoney);

    void onEnter() override;
    void handleInput() override;
    void update(float dt) override;
    void render(Renderer& renderer) override;

    bool isModal() const override { return true; }

private:
    void throwYut();
    void aiTurn();
    void checkGameEnd();

    int& m_playerMoney;
    int m_playerScore = 0; // 0..20
    int m_aiScore = 0;     // 0..20

    bool m_isPlayerTurn = true;
    bool m_gameOver = false;
    std::string m_gameMessage = "윷을 던지려면 [Z / Space] 키를 누르십시오!";
    std::string m_lastYutName;

    std::array<bool, 4> m_yutSticks{false, false, false, false}; // true = flat (front), false = round (back)
    std::mt19937 m_rng;
    float m_animTimer = 0.0f;
};

} // namespace JoseonRPG
