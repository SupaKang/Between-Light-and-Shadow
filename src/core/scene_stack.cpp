#include "scene_stack.hpp"
#include <algorithm>

namespace JoseonRPG {

SceneStack::SceneStack() = default;

SceneStack::~SceneStack() {
    while (!m_scenes.empty()) {
        m_scenes.back()->onExit();
        m_scenes.pop_back();
    }
}

void SceneStack::pushScene(std::unique_ptr<IScene> scene) {
    if (!scene) return;
    m_pendingAction = PendingAction::Push;
    m_pendingScene = std::move(scene);
}

void SceneStack::popScene() {
    if (m_scenes.empty()) return;
    m_pendingAction = PendingAction::Pop;
}

void SceneStack::changeScene(std::unique_ptr<IScene> scene, bool withFade) {
    if (!scene) return;

    if (withFade) {
        startTransition(0.3f, [this, sc = std::shared_ptr<IScene>(scene.release())]() mutable {
            changeScene(std::unique_ptr<IScene>(sc.get()));
            // Note: Transfer ownership back to unique_ptr safely
        });
    } else {
        m_pendingAction = PendingAction::Change;
        m_pendingScene = std::move(scene);
    }
}

void SceneStack::clearAndSet(std::unique_ptr<IScene> scene) {
    if (!scene) return;
    m_pendingAction = PendingAction::ClearAndSet;
    m_pendingScene = std::move(scene);
}

void SceneStack::applyPendingChanges() {
    if (m_pendingAction == PendingAction::None) return;

    switch (m_pendingAction) {
        case PendingAction::Push: {
            if (!m_scenes.empty()) {
                m_scenes.back()->onPause();
            }
            m_pendingScene->setSceneStack(this);
            m_pendingScene->onEnter();
            m_scenes.push_back(std::move(m_pendingScene));
            break;
        }
        case PendingAction::Pop: {
            if (!m_scenes.empty()) {
                m_scenes.back()->onExit();
                m_scenes.pop_back();
                if (!m_scenes.empty()) {
                    m_scenes.back()->onResume();
                }
            }
            break;
        }
        case PendingAction::Change: {
            if (!m_scenes.empty()) {
                m_scenes.back()->onExit();
                m_scenes.pop_back();
            }
            m_pendingScene->setSceneStack(this);
            m_pendingScene->onEnter();
            m_scenes.push_back(std::move(m_pendingScene));
            break;
        }
        case PendingAction::ClearAndSet: {
            while (!m_scenes.empty()) {
                m_scenes.back()->onExit();
                m_scenes.pop_back();
            }
            m_pendingScene->setSceneStack(this);
            m_pendingScene->onEnter();
            m_scenes.push_back(std::move(m_pendingScene));
            break;
        }
        case PendingAction::None:
            break;
    }

    m_pendingAction = PendingAction::None;
    m_pendingScene = nullptr;
}

IScene* SceneStack::getTopScene() const {
    if (m_scenes.empty()) return nullptr;
    return m_scenes.back().get();
}

void SceneStack::handleInput() {
    applyPendingChanges();
    if (m_fadeState != FadeState::None) {
        // Block user input during screen fade transitions
        return;
    }
    if (!m_scenes.empty()) {
        m_scenes.back()->handleInput();
    }
}

void SceneStack::update(float dt) {
    applyPendingChanges();

    // Update screen fade transition
    if (m_fadeState == FadeState::FadingOut) {
        m_fadeAlpha += m_fadeSpeed * dt;
        if (m_fadeAlpha >= 1.0f) {
            m_fadeAlpha = 1.0f;
            m_fadeState = FadeState::FadingIn;
            if (m_fadeMidpointCallback) {
                m_fadeMidpointCallback();
                m_fadeMidpointCallback = nullptr;
                applyPendingChanges();
            }
        }
    } else if (m_fadeState == FadeState::FadingIn) {
        m_fadeAlpha -= m_fadeSpeed * dt;
        if (m_fadeAlpha <= 0.0f) {
            m_fadeAlpha = 0.0f;
            m_fadeState = FadeState::None;
        }
    }

    if (!m_scenes.empty()) {
        m_scenes.back()->update(dt);
    }
}

void SceneStack::render(Renderer& renderer) {
    if (m_scenes.empty()) return;

    // Find the bottom-most scene that needs to be rendered (in case of modal dialogs/menus)
    int firstRenderIdx = static_cast<int>(m_scenes.size()) - 1;
    while (firstRenderIdx > 0 && m_scenes[firstRenderIdx]->isModal()) {
        firstRenderIdx--;
    }

    for (size_t i = firstRenderIdx; i < m_scenes.size(); ++i) {
        m_scenes[i]->render(renderer);
    }

    // Render screen fade overlay if active
    if (m_fadeState != FadeState::None && m_fadeAlpha > 0.001f) {
        uint8_t alpha = static_cast<uint8_t>(std::clamp(m_fadeAlpha * 255.0f, 0.0f, 255.0f));
        Color fadeOverlay(m_fadeColor.r, m_fadeColor.g, m_fadeColor.b, alpha);
        renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, fadeOverlay);
    }
}

void SceneStack::startTransition(float fadeDuration, std::function<void()> onMidpoint) {
    m_fadeState = FadeState::FadingOut;
    m_fadeAlpha = 0.0f;
    m_fadeSpeed = (fadeDuration > 0.0f) ? (1.0f / (fadeDuration * 0.5f)) : 10.0f;
    m_fadeColor = Palette::Black;
    m_fadeMidpointCallback = std::move(onMidpoint);
}

} // namespace JoseonRPG
