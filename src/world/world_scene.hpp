#pragma once
#include "../core/scene.hpp"
#include "../core/types.hpp"
#include "../world/tilemap.hpp"
#include "../world/camera.hpp"
#include "../world/grid_controller.hpp"
#include "../world/npc.hpp"
#include "../gameplay/party.hpp"
#include "../gameplay/artifact.hpp"
#include "../ui/dialogue_box.hpp"
#include "../world/weather_system.hpp"
#include <random>
#include <string>

namespace JoseonRPG {

class WorldScene : public IScene {
public:
    WorldScene(Party& party, ArtifactInventory& artifacts, int& money);

    void onEnter() override;
    void onResume() override;
    void handleInput() override;
    void update(float dt) override;
    void render(Renderer& renderer) override;

    void setPlayerPosition(int gridX, int gridY, int mapId = -1);
    void triggerBossBattle(const std::string& bossId, const std::string& questIdToComplete, const std::string& nextQuestId, int rewardMoney, const std::string& victoryNotice);

    const WeatherSystem& getWeatherSystem() const { return m_weather; }

    void openStartMenu();
    void closeStartMenu();

private:
    void interactWithNPC();
    void checkStepEvents(int newGridX, int newGridY);
    void saveGame();

    Party& m_party;
    ArtifactInventory& m_artifacts;
    int& m_money;

    Tilemap m_tilemap;
    Camera m_camera;
    GridController m_gridController;
    DialogueBox m_dialogueBox;
    WeatherSystem m_weather;

    std::mt19937 m_rng;
    std::string m_noticeMsg;
    float m_fadeAlpha = 0.0f;

    struct DustParticle {
        float x = 0.0f;
        float y = 0.0f;
        float vx = 0.0f;
        float vy = 0.0f;
        float life = 0.0f;
        float maxLife = 0.18f;
    };
    std::vector<DustParticle> m_dustParticles;
    float m_dustTimer = 0.0f;

    // Active Dialogue NPC state
    const NPC* m_activeNPC = nullptr;

    // Pokemon Gold Style Start Menu
    bool m_menuOpen = false;
    int m_menuCursor = 0;
    void handleStartMenuInput();
    void renderStartMenu(Renderer& renderer);

    // Battle Intro Screen Transition Cutscene State
    bool m_inBattleTransition = false;
    float m_battleTransitionTimer = 0.0f;
    std::unique_ptr<IScene> m_pendingBattleScene = nullptr;
    void startBattleTransition(std::unique_ptr<IScene> battleScene);
};

} // namespace JoseonRPG
