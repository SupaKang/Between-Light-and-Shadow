#pragma once
#include "scene.hpp"
#include "renderer.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace JoseonRPG {

enum class FadeState {
    None,
    FadingOut,
    FadingIn
};

class SceneStack {
public:
    SceneStack();
    ~SceneStack();

    void pushScene(std::unique_ptr<IScene> scene);
    void popScene();
    void changeScene(std::unique_ptr<IScene> scene, bool withFade = false);
    void clearAndSet(std::unique_ptr<IScene> scene);

    void handleInput();
    void update(float dt);
    void render(Renderer& renderer);

    bool isEmpty() const { return m_scenes.empty() && m_pendingAction == PendingAction::None; }
    IScene* getTopScene() const;
    size_t getSceneCount() const { return m_scenes.size(); }

    void startTransition(float fadeDuration, std::function<void()> onMidpoint);

private:
    void applyPendingChanges();

    enum class PendingAction {
        None,
        Push,
        Pop,
        Change,
        ClearAndSet
    };

    std::vector<std::unique_ptr<IScene>> m_scenes;
    PendingAction m_pendingAction = PendingAction::None;
    std::unique_ptr<IScene> m_pendingScene = nullptr;

    // Screen Fade Transition
    FadeState m_fadeState = FadeState::None;
    float m_fadeAlpha = 0.0f;
    float m_fadeSpeed = 3.0f;
    Color m_fadeColor = Palette::Black;
    std::function<void()> m_fadeMidpointCallback = nullptr;
};

} // namespace JoseonRPG
