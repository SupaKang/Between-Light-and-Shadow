#pragma once
#include "../core/scene.hpp"
#include "../core/types.hpp"
#include <string>

namespace JoseonRPG {

class Renderer;

struct GameSettings {
    float masterVolume = 1.0f;
    float bgmVolume = 1.0f;
    float sfxVolume = 1.0f;
    bool isMuted = false;
    bool crtFilter = true;
    bool vignette = true;
    int textSpeed = 1; // 0: Normal, 1: Fast, 2: Instant
    int displayScale = 3; // 3: 3X, 4: 4X, 5: 5X, 6: Fullscreen
};

class SettingsScene : public IScene {
public:
    SettingsScene();

    void onEnter() override;
    void handleInput() override;
    void update(float dt) override;
    void render(Renderer& renderer) override;

    bool isModal() const override { return true; }

    static GameSettings& getGlobalSettings();

private:
    int m_cursor = 0;
    static GameSettings s_settings;
};

} // namespace JoseonRPG
