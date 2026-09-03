#pragma once
#include "types.hpp"

namespace JoseonRPG {

class Renderer;
class SceneStack;

class IScene {
public:
    virtual ~IScene() = default;

    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void onPause() {}
    virtual void onResume() {}

    virtual void handleInput() = 0;
    virtual void update(float dt) = 0;
    virtual void render(Renderer& renderer) = 0;

    // If true, the scene below this one in the stack will still be rendered
    virtual bool isModal() const { return false; }

    void setSceneStack(SceneStack* stack) { m_sceneStack = stack; }
    SceneStack* getSceneStack() const { return m_sceneStack; }

protected:
    SceneStack* m_sceneStack = nullptr;
};

} // namespace JoseonRPG
