#include "core/types.hpp"
#include "core/window.hpp"
#include "core/renderer.hpp"
#include "core/input.hpp"
#include "core/task_engine.hpp"
#include "core/scene_stack.hpp"
#include "data/data_manager.hpp"
#include "gameplay/party.hpp"
#include "gameplay/artifact.hpp"
#include "scenes/title_scene.hpp"

#include <chrono>
#include <thread>
#include <memory>

using namespace JoseonRPG;

int main() {
    // 1. Initialize Subsystems & Static Databases
    DataManager::init();

    Window window("108: Eumyang Gyeonmunrok (Between Light and Shadow) - 320x180", 3);
    if (!window.init()) {
        return -1;
    }

    Renderer renderer;
    SceneStack sceneStack;

    // 2. Global Game State containers (Party, Artifacts, Money)
    Party playerParty;
    ArtifactInventory artifacts;
    int playerMoney = 500;

    // 3. Start with Title Scene
    sceneStack.pushScene(std::make_unique<TitleScene>(playerParty, artifacts, playerMoney));

    // 4. Fixed Timestep Game Loop (Target: 60 FPS)
    constexpr float TARGET_DT = 1.0f / 60.0f;
    auto prevTime = std::chrono::high_resolution_clock::now();

    while (window.isOpen() && !sceneStack.isEmpty()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float>(currentTime - prevTime).count();
        prevTime = currentTime;

        // Clamp delta time to prevent spiral of death during debugging or hiccups
        float dt = std::min(frameTime, 0.05f);

        // Input Polling & Window Messages
        Input::update(dt);
        if (!window.processMessages()) {
            break;
        }

        // Global Asynchronous Tasks
        TaskEngine::update(dt);

        // Scene Stack Lifecycle
        sceneStack.handleInput();
        sceneStack.update(dt);

        // Render Framebuffer
        renderer.clear(Palette::Black);
        sceneStack.render(renderer);
        window.present(renderer);

        // Frame rate limiter
        auto frameEndTime = std::chrono::high_resolution_clock::now();
        float elapsed = std::chrono::duration<float>(frameEndTime - currentTime).count();
        if (elapsed < TARGET_DT) {
            auto sleepDuration = std::chrono::duration<float>(TARGET_DT - elapsed);
            std::this_thread::sleep_for(std::chrono::duration_cast<std::chrono::milliseconds>(sleepDuration));
        }
    }

    return 0;
}
