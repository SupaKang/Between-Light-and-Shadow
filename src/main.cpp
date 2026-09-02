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
#include <random>

using namespace JoseonRPG;

enum class GameState {
    WorldExploration,
    BattleView,
    ArtifactMenu
};

static std::mt19937 s_mainRng(2026);

int main() {
    // 1. Initialize Data & Subsystems
    DataManager::init();

    Window window("108: Eumyang Gyeonmunrok (108: Yin-Yang Chronicle) - 320x180", 3);
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

    // 3. Gameplay State
    Party playerParty;
    playerParty.addYokai(DataManager::createYokaiById("YOKAI_001")); // Dokkaebi
    playerParty.addYokai(DataManager::createYokaiById("YOKAI_002")); // Gumiho
    playerParty.addYokai(DataManager::createYokaiById("YOKAI_003")); // Bulgasari

    ArtifactInventory artifacts;
    artifacts.addArtifact(DataManager::createArtifactById("ART_DOKKAEBI_HAT"));
    artifacts.addArtifact(DataManager::createArtifactById("ART_DEMON_TILE"));

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

            // Trigger Battle with ActionA (Z key)
            if (Input::isPressed(Key::ActionA)) {
                // Random wild yokai selection
                const auto& pool = DataManager::getAllYokaiTemplates();
                std::uniform_int_distribution<size_t> poolDist(0, pool.size() - 1);
                Yokai wildEnemy = pool[poolDist(s_mainRng)];

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
                    if (Input::isPressed(Key::Up)) currentBattle->onNavigateUp();
                    if (Input::isPressed(Key::Down)) currentBattle->onNavigateDown();
                    if (Input::isPressed(Key::Left)) currentBattle->onNavigateLeft();
                    if (Input::isPressed(Key::Right)) currentBattle->onNavigateRight();
                    if (Input::isPressed(Key::ActionA)) currentBattle->onConfirm();
                    if (Input::isPressed(Key::ActionB)) currentBattle->onCancel();
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
            FontRenderer::drawText(renderer, 4, 2, "108: EUMYANG GYEONMUNROK", Palette::Yellow);
            
            std::string fpsStr = "FPS: " + std::to_string(fps);
            FontRenderer::drawText(renderer, SCREEN_WIDTH - 64, 2, fpsStr, Palette::Jade);

            // Bottom controls hint
            renderer.fillRect(0, SCREEN_HEIGHT - 12, SCREEN_WIDTH, 12, Color(18, 18, 20, 210));
            FontRenderer::drawText(renderer, 4, SCREEN_HEIGHT - 10, "ARROWS:Move | Z:Encounter | C:Artifacts", Palette::White);
        }
        else if (gameState == GameState::BattleView) {
            // Battle Background (Moonlit Night Atmosphere)
            renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(16, 20, 28));
            renderer.fillRect(0, 80, SCREEN_WIDTH, 30, Color(24, 30, 42));
            renderer.drawLine(0, 110, SCREEN_WIDTH, 110, Palette::MidGray);

            if (currentBattle) {
                const Yokai* pYokai = currentBattle->getActivePlayerYokai();
                const Yokai& eYokai = currentBattle->getEnemyYokai();

                // 1. Enemy HUD Box (Top-Left)
                renderer.drawPanel(8, 6, 140, 42, Color(24, 26, 34, 230), Palette::MidGray);
                std::string eGradeStr = " [Grade " + std::to_string(static_cast<int>(eYokai.getGrade())) + "]";
                FontRenderer::drawText(renderer, 14, 10, eYokai.getName() + " Lv." + std::to_string(eYokai.getLevel()), Palette::Red);
                FontRenderer::drawText(renderer, 100, 10, eGradeStr, Palette::Yellow);

                // Enemy HP Bar
                std::string eHpText = "HP " + std::to_string(eYokai.getStats().hp) + "/" + std::to_string(eYokai.getStats().maxHp);
                FontRenderer::drawText(renderer, 14, 20, eHpText, Palette::White);
                renderer.drawHealthBar(14, 29, 128, 5, eYokai.getStats().hp, eYokai.getStats().maxHp, Palette::Red);

                // Enemy Status Badge
                if (eYokai.getStatus().effect != StatusEffect::None) {
                    renderer.fillRect(14, 36, 60, 9, StatusEffectSystem::getStatusColor(eYokai.getStatus().effect));
                    FontRenderer::drawText(renderer, 16, 37, StatusEffectSystem::getStatusName(eYokai.getStatus().effect), Palette::Black);
                }

                // Enemy Sprite (Top-Right)
                int eSpriteId = 1;
                if (eYokai.getId() == "YOKAI_002") eSpriteId = 2; // Gumiho
                else if (eYokai.getId() == "YOKAI_005") eSpriteId = 3; // Maiden Ghost
                else if (eYokai.getId() == "YOKAI_003") eSpriteId = 4; // Bulgasari
                renderer.drawSprite(SCREEN_WIDTH - 65, 24, eSpriteId, 0);

                // 2. Player Yokai HUD Box (Bottom-Right)
                if (pYokai) {
                    renderer.drawSprite(35, 68, 0, 0); // Player active yokai sprite

                    renderer.drawPanel(165, 60, 147, 46, Color(24, 26, 34, 230), Palette::Blue);
                    FontRenderer::drawText(renderer, 171, 64, pYokai->getName() + " Lv." + std::to_string(pYokai->getLevel()), Palette::Jade);
                    
                    // Player HP Bar
                    std::string pHpText = "HP " + std::to_string(pYokai->getStats().hp) + "/" + std::to_string(pYokai->getStats().maxHp);
                    FontRenderer::drawText(renderer, 171, 74, pHpText, Palette::White);
                    renderer.drawHealthBar(171, 83, 135, 5, pYokai->getStats().hp, pYokai->getStats().maxHp, Palette::Green);

                    // Player Qi Bar
                    std::string pQiText = "Qi " + std::to_string(pYokai->getStats().qi) + "/" + std::to_string(pYokai->getStats().maxQi);
                    FontRenderer::drawText(renderer, 171, 90, pQiText, Palette::Jade);
                    renderer.drawHealthBar(220, 91, 86, 4, pYokai->getStats().qi, pYokai->getStats().maxQi, Palette::Blue);
                }

                // 3. Command & Log Region (Bottom Area: Y 112..180)
                renderer.drawPanel(0, 110, SCREEN_WIDTH, 70, Palette::Black, Palette::MidGray);

                if (currentBattle->getState() == BattleState::PlayerCommand) {
                    if (currentBattle->getMenuState() == BattleMenuState::MainAction) {
                        // 4-Button Main Menu Panel (Left)
                        renderer.drawPanel(4, 114, 130, 62, Color(20, 22, 28), Palette::MidGray);
                        
                        const char* menuLabels[4] = {"1.기술 (Attack)", "2.계약 (Talisman)", "3.교체 (Party)", "4.도망 (Flee)"};
                        int cur = currentBattle->getMainCursor();

                        for (int i = 0; i < 4; ++i) {
                            int mx = (i % 2 == 0) ? 8 : 68;
                            int my = (i < 2) ? 122 : 146;
                            Color c = (cur == i) ? Palette::Yellow : Palette::White;
                            if (cur == i) {
                                FontRenderer::drawText(renderer, mx - 4, my, ">", Palette::Yellow);
                            }
                            FontRenderer::drawText(renderer, mx, my, menuLabels[i], c);
                        }

                        // Combat Log & Status Panel (Right)
                        renderer.drawPanel(138, 114, 178, 62, Color(16, 18, 22), Palette::MidGray);
                        const auto& log = currentBattle->getCombatLog();
                        int logY = 118;
                        int startIdx = std::max(0, static_cast<int>(log.size()) - 4);
                        for (size_t i = startIdx; i < log.size(); ++i) {
                            FontRenderer::drawText(renderer, 142, logY, log[i], Palette::White);
                            logY += 9;
                        }

                        // Contract chance indicator
                        float capRate = currentBattle->calculateCaptureProbability();
                        std::string capStr = "계약률: " + std::to_string(static_cast<int>(capRate * 100)) + "%";
                        FontRenderer::drawText(renderer, 240, 162, capStr, Palette::Yellow);
                    }
                    else if (currentBattle->getMenuState() == BattleMenuState::SkillSelect) {
                        // 4-Skill Select Submenu (Left)
                        renderer.drawPanel(4, 114, 160, 62, Color(20, 22, 28), Palette::Yellow);
                        
                        if (pYokai) {
                            const auto& skills = pYokai->getSkills();
                            int sCur = currentBattle->getSkillCursor();

                            for (size_t i = 0; i < 4 && i < skills.size(); ++i) {
                                int sx = (i % 2 == 0) ? 8 : 84;
                                int sy = (i < 2) ? 122 : 144;
                                Color col = (sCur == static_cast<int>(i)) ? Palette::Yellow : Palette::White;
                                if (sCur == static_cast<int>(i)) {
                                    FontRenderer::drawText(renderer, sx - 4, sy, ">", Palette::Yellow);
                                }
                                FontRenderer::drawText(renderer, sx, sy, skills[i].name, col);
                                FontRenderer::drawText(renderer, sx, sy + 8, "Qi:" + std::to_string(skills[i].qiCost), Palette::Jade);
                            }

                            // Skill Detail Panel (Right)
                            renderer.drawPanel(168, 114, 148, 62, Color(16, 18, 22), Palette::MidGray);
                            if (sCur < static_cast<int>(skills.size())) {
                                const auto& curSkl = skills[sCur];
                                FontRenderer::drawText(renderer, 172, 118, "위력(Pwr): " + std::to_string(curSkl.power), Palette::White);
                                FontRenderer::drawText(renderer, 172, 128, "명중(Acc): " + std::to_string(curSkl.accuracy) + "%", Palette::White);
                                if (curSkl.statusEffect != StatusEffect::None) {
                                    FontRenderer::drawText(renderer, 172, 138, StatusEffectSystem::getStatusName(curSkl.statusEffect), StatusEffectSystem::getStatusColor(curSkl.statusEffect));
                                } else {
                                    FontRenderer::drawText(renderer, 172, 138, "상태이상: 없음", Palette::LightGray);
                                }
                                FontRenderer::drawText(renderer, 172, 156, "[X키: 뒤로가기]", Palette::MidGray);
                            }
                        }
                    }
                    else if (currentBattle->getMenuState() == BattleMenuState::PartySwapSelect) {
                        // Party Swap Submenu
                        renderer.drawPanel(4, 114, 312, 62, Color(20, 22, 28), Palette::Blue);
                        FontRenderer::drawText(renderer, 10, 118, "=== 출전할 요괴를 선택하십시오 (X: 취소) ===", Palette::Yellow);

                        int swapCur = currentBattle->getSwapCursor();
                        for (size_t i = 0; i < playerParty.getSize(); ++i) {
                            const Yokai* member = playerParty.getYokai(i);
                            if (!member) continue;
                            int my = 130 + static_cast<int>(i) * 12;
                            Color c = (swapCur == static_cast<int>(i)) ? Palette::Yellow : Palette::White;
                            if (swapCur == static_cast<int>(i)) {
                                FontRenderer::drawText(renderer, 8, my, ">", Palette::Yellow);
                            }
                            std::string slotInfo = std::to_string(i + 1) + ". " + member->getName() + " Lv." + std::to_string(member->getLevel()) +
                                                   " (HP:" + std::to_string(member->getStats().hp) + "/" + std::to_string(member->getStats().maxHp) + ")";
                            FontRenderer::drawText(renderer, 16, my, slotInfo, c);
                        }
                    }
                }
                else if (currentBattle->getState() == BattleState::Victory) {
                    renderer.drawPanel(30, 118, 260, 52, Color(16, 40, 24), Palette::Yellow);
                    FontRenderer::drawText(renderer, 90, 126, "★ 전투 승리! ★", Palette::Yellow);
                    FontRenderer::drawText(renderer, 50, 140, "경험치 " + std::to_string(currentBattle->getExpReward()) + " 획득 및 요괴 진정 완료!", Palette::White);
                    FontRenderer::drawText(renderer, 70, 154, "[Z / Space 키를 눌러 필드로 복귀]", Palette::Jade);
                }
                else if (currentBattle->getState() == BattleState::Defeat) {
                    renderer.drawPanel(30, 118, 260, 52, Color(40, 16, 16), Palette::Red);
                    FontRenderer::drawText(renderer, 90, 126, "☠ 파티 전멸 ☠", Palette::Red);
                    FontRenderer::drawText(renderer, 60, 140, "모든 요괴가 기절했습니다...", Palette::White);
                    FontRenderer::drawText(renderer, 70, 154, "[Z / Space 키를 눌러 주막으로 복귀]", Palette::Yellow);
                }
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
