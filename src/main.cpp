#include "core/types.hpp"
#include "core/window.hpp"
#include "core/renderer.hpp"
#include "core/input.hpp"
#include "ui/font_renderer.hpp"
#include "world/tilemap.hpp"
#include "world/camera.hpp"
#include "gameplay/yokai.hpp"
#include "gameplay/party.hpp"
#include "gameplay/artifact.hpp"
#include "battle/battle.hpp"
#include "data/data_manager.hpp"

#include <chrono>
#include <thread>
#include <memory>
#include <sstream>
#include <iomanip>

using namespace JoseonRPG;

enum class GameState {
    WorldExploration,
    BattleView,
    ArtifactMenu
};

int main() {
    // 1. Initialize Data & Subsystems
    DataManager::init();

    Window window("Joseon Yokai RPG - Phase 1 Technical Spike (320x180)", 3);
    if (!window.init()) {
        return -1;
    }

    Renderer renderer;
    Tilemap tilemap;
    Camera camera;

    // 2. Player State
    int playerGridX = 7;
    int playerGridY = 6;
    int playerPixelX = playerGridX * TILE_SIZE;
    int playerPixelY = playerGridY * TILE_SIZE;
    int playerAnimFrame = 0;
    int animTimer = 0;

    // 3. Gameplay State
    Party playerParty;
    playerParty.addYokai(DataManager::createYokaiById("YOKAI_001")); // Dokkaebi
    playerParty.addYokai(DataManager::createYokaiById("YOKAI_002")); // Gumiho

    ArtifactInventory artifacts;
    artifacts.addArtifact(DataManager::createArtifactById("ART_DOKKAEBI_HAT"));

    std::unique_ptr<Battle> currentBattle = nullptr;
    GameState gameState = GameState::WorldExploration;

    // 4. Main Game Loop (Fixed 60 FPS Target)
    auto prevTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    int fps = 60;
    auto fpsTimer = prevTime;

    while (window.isOpen()) {
        Input::update();
        if (!window.processMessages()) {
            break;
        }

        // Calculate FPS
        frameCount++;
        auto now = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - fpsTimer).count() >= 1000) {
            fps = frameCount;
            frameCount = 0;
            fpsTimer = now;
        }

        // --- UPDATE LOGIC ---
        if (gameState == GameState::WorldExploration) {
            // Player movement
            int dx = 0;
            int dy = 0;
            if (Input::isPressed(Key::Up)) dy -= 1;
            if (Input::isPressed(Key::Down)) dy += 1;
            if (Input::isPressed(Key::Left)) dx -= 1;
            if (Input::isPressed(Key::Right)) dx += 1;

            if (dx != 0 || dy != 0) {
                int targetX = playerGridX + dx;
                int targetY = playerGridY + dy;
                if (!tilemap.isSolid(targetX, targetY)) {
                    playerGridX = targetX;
                    playerGridY = targetY;
                    playerPixelX = playerGridX * TILE_SIZE;
                    playerPixelY = playerGridY * TILE_SIZE;
                    playerAnimFrame++;
                }
            }

            camera.update(playerPixelX, playerPixelY, tilemap.getWidth(), tilemap.getHeight());

            // Trigger Battle Spike Demo with ActionA (Z key)
            if (Input::isPressed(Key::ActionA)) {
                Yokai wildEnemy = DataManager::createYokaiById("YOKAI_005"); // Maiden Ghost
                currentBattle = std::make_unique<Battle>(playerParty, wildEnemy, artifacts);
                gameState = GameState::BattleView;
            }

            // Open Artifact Menu with ActionC (C key)
            if (Input::isPressed(Key::ActionC)) {
                gameState = GameState::ArtifactMenu;
            }
        }
        else if (gameState == GameState::BattleView) {
            if (currentBattle) {
                if (currentBattle->getState() == BattleState::PlayerCommand) {
                    if (Input::isPressed(Key::Up) || Input::isPressed(Key::ActionA)) {
                        // Use skill 0
                        currentBattle->selectAction(PlayerAction::Skill0);
                    } else if (Input::isPressed(Key::Down)) {
                        // Use skill 1
                        currentBattle->selectAction(PlayerAction::Skill1);
                    } else if (Input::isPressed(Key::Left)) {
                        // Contract / Capture attempt
                        currentBattle->selectAction(PlayerAction::Contract);
                    } else if (Input::isPressed(Key::Right) || Input::isPressed(Key::ActionB)) {
                        // Return to field
                        gameState = GameState::WorldExploration;
                    }
                } else if (currentBattle->getState() == BattleState::Victory ||
                           currentBattle->getState() == BattleState::Defeat) {
                    if (Input::isPressed(Key::ActionA) || Input::isPressed(Key::ActionB)) {
                        gameState = GameState::WorldExploration;
                    }
                }
            }
        }
        else if (gameState == GameState::ArtifactMenu) {
            if (Input::isPressed(Key::ActionB) || Input::isPressed(Key::ActionC)) {
                gameState = GameState::WorldExploration;
            }
            if (Input::isPressed(Key::ActionA) && artifacts.getCount() > 0) {
                // Destroy first artifact demo
                artifacts.destroyArtifact(0);
            }
        }

        // --- RENDER LOGIC (320x180 Framebuffer) ---
        renderer.clear(Palette::Black);

        if (gameState == GameState::WorldExploration) {
            // Render 16x16 Tilemap
            tilemap.render(renderer, camera.getX(), camera.getY());

            // Render Player Sprite
            int screenPX = playerPixelX - camera.getX();
            int screenPY = playerPixelY - camera.getY();
            renderer.drawSprite(screenPX, screenPY, 0, playerAnimFrame);

            // World HUD Overlay
            renderer.fillRect(0, 0, SCREEN_WIDTH, 12, Color(18, 18, 20, 210));
            FontRenderer::drawText(renderer, 4, 2, "JOSEON YOKAI RPG - FIELD SPIKE", Palette::Yellow);
            
            std::string fpsStr = "FPS: " + std::to_string(fps);
            FontRenderer::drawText(renderer, SCREEN_WIDTH - 64, 2, fpsStr, Palette::Jade);

            // Bottom controls hint
            renderer.fillRect(0, SCREEN_HEIGHT - 12, SCREEN_WIDTH, 12, Color(18, 18, 20, 210));
            FontRenderer::drawText(renderer, 4, SCREEN_HEIGHT - 10, "ARROWS:Move | Z:Battle Spike | C:Bag", Palette::White);
        }
        else if (gameState == GameState::BattleView) {
            // Battle Background
            renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(24, 26, 32));
            renderer.drawLine(0, 110, SCREEN_WIDTH, 110, Palette::MidGray);

            if (currentBattle) {
                const Yokai* pYokai = currentBattle->getActivePlayerYokai();
                const Yokai& eYokai = currentBattle->getEnemyYokai();

                // Enemy Info & Sprite
                FontRenderer::drawText(renderer, 10, 10, eYokai.getName() + " Lv." + std::to_string(eYokai.getLevel()), Palette::Red);
                std::string eHp = "HP: " + std::to_string(eYokai.getStats().hp) + "/" + std::to_string(eYokai.getStats().maxHp);
                FontRenderer::drawText(renderer, 10, 20, eHp, Palette::White);
                
                if (eYokai.getStatus().effect != StatusEffect::None) {
                    FontRenderer::drawText(renderer, 10, 30, StatusEffectSystem::getStatusName(eYokai.getStatus().effect), StatusEffectSystem::getStatusColor(eYokai.getStatus().effect));
                }

                renderer.drawSprite(SCREEN_WIDTH - 60, 25, 1, 0); // Enemy sprite

                // Player Info & Sprite
                if (pYokai) {
                    renderer.drawSprite(40, 75, 0, 0); // Player sprite
                    FontRenderer::drawText(renderer, 120, 70, pYokai->getName() + " Lv." + std::to_string(pYokai->getLevel()), Palette::Blue);
                    std::string pHp = "HP: " + std::to_string(pYokai->getStats().hp) + "/" + std::to_string(pYokai->getStats().maxHp);
                    FontRenderer::drawText(renderer, 120, 80, pHp, Palette::White);
                    std::string pQi = "Qi: " + std::to_string(pYokai->getStats().qi) + "/" + std::to_string(pYokai->getStats().maxQi);
                    FontRenderer::drawText(renderer, 120, 90, pQi, Palette::Jade);
                }

                // Battle Menu / Log Box
                renderer.fillRect(0, 112, SCREEN_WIDTH, SCREEN_HEIGHT - 112, Palette::Black);
                renderer.drawRect(2, 114, SCREEN_WIDTH - 4, SCREEN_HEIGHT - 116, Palette::MidGray);

                const auto& log = currentBattle->getCombatLog();
                int logY = 118;
                int startIdx = std::max(0, static_cast<int>(log.size()) - 3);
                for (size_t i = startIdx; i < log.size(); ++i) {
                    FontRenderer::drawText(renderer, 8, logY, log[i], Palette::White);
                    logY += 9;
                }

                // Command Hints
                std::string capProbStr = "Capture: " + std::to_string(static_cast<int>(currentBattle->calculateCaptureProbability() * 100)) + "%";
                FontRenderer::drawText(renderer, SCREEN_WIDTH - 110, SCREEN_HEIGHT - 22, capProbStr, Palette::Yellow);
                FontRenderer::drawText(renderer, SCREEN_WIDTH - 110, SCREEN_HEIGHT - 12, "Z:Atk | L:Talisman", Palette::Jade);
            }
        }
        else if (gameState == GameState::ArtifactMenu) {
            renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Palette::DarkGray);
            FontRenderer::drawText(renderer, 10, 10, "=== ARTIFACT INVENTORY ===", Palette::Yellow);

            int artY = 30;
            const auto& artList = artifacts.getArtifacts();
            if (artList.empty()) {
                FontRenderer::drawText(renderer, 10, artY, "No artifacts in possession.", Palette::LightGray);
            } else {
                for (size_t i = 0; i < artList.size(); ++i) {
                    FontRenderer::drawText(renderer, 10, artY, std::to_string(i + 1) + ". " + artList[i].name, Palette::White);
                    FontRenderer::drawText(renderer, 20, artY + 10, artList[i].lore, Palette::MidGray);
                    artY += 24;
                }
            }

            renderer.fillRect(0, SCREEN_HEIGHT - 16, SCREEN_WIDTH, 16, Palette::Black);
            FontRenderer::drawText(renderer, 4, SCREEN_HEIGHT - 12, "Z: Instantly Destroy #1 | X: Close", Palette::White);
        }

        // Present Framebuffer to Win32 Window
        window.present(renderer);

        // Frame pacing (~60 FPS)
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}
