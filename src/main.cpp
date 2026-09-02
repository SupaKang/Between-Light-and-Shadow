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
#include "gameplay/encyclopedia.hpp"
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
    ArtifactMenu,
    EncyclopediaView,
    PartyManagementView
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

    // 3. Gameplay State (3 Party Members & Artifacts)
    Party playerParty;
    Yokai p1 = DataManager::createYokaiById("YOKAI_001"); // Dokkaebi
    p1.gainExp(1200); // Level 5
    playerParty.addYokai(p1);

    Yokai p2 = DataManager::createYokaiById("YOKAI_002"); // Gumiho
    p2.gainExp(2500); // Level 8
    playerParty.addYokai(p2);

    Yokai p3 = DataManager::createYokaiById("YOKAI_003"); // Bulgasari
    p3.gainExp(800);  // Level 4
    playerParty.addYokai(p3);

    ArtifactInventory artifacts;
    artifacts.addArtifact(DataManager::createArtifactById("ART_DOKKAEBI_HAT"));
    artifacts.addArtifact(DataManager::createArtifactById("ART_DEMON_TILE"));

    std::unique_ptr<Battle> currentBattle = nullptr;
    GameState gameState = GameState::WorldExploration;

    // UI View State
    int codexCursor = 1;      // 1..108
    int partyViewCursor = 0;  // 0..2
    std::string partyFeedbackMsg = "";
    int artCursor = 0;        // 0..7
    std::string artFeedbackMsg = "";

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

                    // Wild Encounter check on grass (Tile 0)
                    std::uniform_int_distribution<int> encRoll(1, 100);
                    if (encRoll(s_mainRng) <= 12) { // 12% encounter chance per step
                        const auto& pool = DataManager::getAllYokaiTemplates();
                        std::uniform_int_distribution<size_t> poolDist(0, pool.size() - 1);
                        Yokai wildEnemy = pool[poolDist(s_mainRng)];

                        currentBattle = std::make_unique<Battle>(playerParty, wildEnemy, artifacts);
                        gameState = GameState::BattleView;
                    }
                }
            }

            camera.update(playerPixelX, playerPixelY, tilemap.getWidth(), tilemap.getHeight());

            // Manual Battle Trigger with ActionA (Z key)
            if (Input::isPressed(Key::ActionA)) {
                const auto& pool = DataManager::getAllYokaiTemplates();
                std::uniform_int_distribution<size_t> poolDist(0, pool.size() - 1);
                Yokai wildEnemy = pool[poolDist(s_mainRng)];

                currentBattle = std::make_unique<Battle>(playerParty, wildEnemy, artifacts);
                gameState = GameState::BattleView;
            }

            // Open 108 Yokai Encyclopedia with ActionB (X key)
            if (Input::isPressed(Key::ActionB)) {
                gameState = GameState::EncyclopediaView;
            }

            // Open Artifact Menu with ActionC (C key)
            if (Input::isPressed(Key::ActionC)) {
                gameState = GameState::ArtifactMenu;
            }

            // Open Party Management with Debug/F1 key
            if (Input::isPressed(Key::Debug)) {
                partyFeedbackMsg = "";
                gameState = GameState::PartyManagementView;
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
        else if (gameState == GameState::EncyclopediaView) {
            if (Input::isPressed(Key::Up)) {
                if (codexCursor > 1) codexCursor--;
            }
            if (Input::isPressed(Key::Down)) {
                if (codexCursor < 108) codexCursor++;
            }
            if (Input::isPressed(Key::Left)) {
                codexCursor = std::max(1, codexCursor - 10);
            }
            if (Input::isPressed(Key::Right)) {
                codexCursor = std::min(108, codexCursor + 10);
            }
            if (Input::isPressed(Key::ActionB) || Input::isPressed(Key::ActionA)) {
                gameState = GameState::WorldExploration;
            }
        }
        else if (gameState == GameState::PartyManagementView) {
            if (Input::isPressed(Key::Up)) {
                if (partyViewCursor > 0) partyViewCursor--;
            }
            if (Input::isPressed(Key::Down)) {
                if (partyViewCursor + 1 < static_cast<int>(playerParty.getSize())) partyViewCursor++;
            }
            if (Input::isPressed(Key::ActionA)) {
                // Promote Grade attempt
                Yokai* selected = playerParty.getYokai(partyViewCursor);
                if (selected) {
                    if (selected->canPromote()) {
                        selected->promoteGrade();
                        partyFeedbackMsg = selected->getName() + " 등급 승급 성공! (Grade " +
                                           std::to_string(static_cast<int>(selected->getGrade())) + ")";
                    } else {
                        partyFeedbackMsg = "승급 레벨 조건 미충족 (필요: Lv.10/20/35/50)";
                    }
                }
            }
            if (Input::isPressed(Key::ActionB) || Input::isPressed(Key::Debug)) {
                gameState = GameState::WorldExploration;
            }
        }
        else if (gameState == GameState::ArtifactMenu) {
            if (Input::isPressed(Key::Up)) {
                if (artCursor > 0) artCursor--;
            }
            if (Input::isPressed(Key::Down)) {
                if (artCursor + 1 < static_cast<int>(artifacts.getCount())) artCursor++;
            }
            if (Input::isPressed(Key::ActionA) && artifacts.getCount() > 0) {
                artifacts.destroyArtifact(artCursor, playerParty.getActiveYokai(), &artFeedbackMsg);
                if (artCursor >= static_cast<int>(artifacts.getCount()) && artCursor > 0) {
                    artCursor--;
                }
            }
            if (Input::isPressed(Key::ActionB) || Input::isPressed(Key::ActionC)) {
                gameState = GameState::WorldExploration;
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
            
            std::string colRateStr = "도감: " + std::to_string(DataManager::getEncyclopedia().getCapturedCount()) + "/108";
            FontRenderer::drawText(renderer, SCREEN_WIDTH - 120, 2, colRateStr, Palette::Jade);

            std::string fpsStr = "FPS:" + std::to_string(fps);
            FontRenderer::drawText(renderer, SCREEN_WIDTH - 42, 2, fpsStr, Palette::LightGray);

            // Bottom controls hint
            renderer.fillRect(0, SCREEN_HEIGHT - 12, SCREEN_WIDTH, 12, Color(18, 18, 20, 210));
            FontRenderer::drawText(renderer, 4, SCREEN_HEIGHT - 10, "ARROWS:Move | Z:Encounter | X:Codex | C:Bag | F1:Party", Palette::White);
        }
        else if (gameState == GameState::BattleView) {
            // Battle Background
            renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(16, 20, 28));
            renderer.fillRect(0, 80, SCREEN_WIDTH, 30, Color(24, 30, 42));
            renderer.drawLine(0, 110, SCREEN_WIDTH, 110, Palette::MidGray);

            if (currentBattle) {
                const Yokai* pYokai = currentBattle->getActivePlayerYokai();
                const Yokai& eYokai = currentBattle->getEnemyYokai();

                // 1. Enemy HUD Box (Top-Left)
                renderer.drawPanel(8, 6, 140, 42, Color(24, 26, 34, 230), Palette::MidGray);
                std::string eGradeStr = " [G." + std::to_string(static_cast<int>(eYokai.getGrade())) + "]";
                FontRenderer::drawText(renderer, 14, 10, eYokai.getName() + " Lv." + std::to_string(eYokai.getLevel()), Palette::Red);
                FontRenderer::drawText(renderer, 108, 10, eGradeStr, Palette::Yellow);

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

                // 3. Command & Log Region (Bottom Area: Y 110..180)
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
                    FontRenderer::drawText(renderer, 50, 140, "경험치 " + std::to_string(currentBattle->getExpReward()) + " 획득 및 도감 등록!", Palette::White);
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
        else if (gameState == GameState::EncyclopediaView) {
            // 108 Yokai Encyclopedia View
            renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(20, 22, 28));
            
            // Header
            renderer.drawPanel(4, 4, 312, 20, Color(30, 34, 44), Palette::Yellow);
            FontRenderer::drawText(renderer, 10, 8, "=== 108: 음양견문록 도감 (CODEX) ===", Palette::Yellow);
            
            int capCnt = DataManager::getEncyclopedia().getCapturedCount();
            std::string rateStr = "수집: " + std::to_string(capCnt) + "/108 (" + std::to_string(static_cast<int>(capCnt * 100.0f / 108.0f)) + "%)";
            FontRenderer::drawText(renderer, 220, 8, rateStr, Palette::Jade);

            // Left: 6-Slot Scroll List
            renderer.drawPanel(4, 26, 130, 136, Color(16, 18, 24), Palette::MidGray);
            int startSlot = std::max(1, std::min(103, codexCursor - 2));

            for (int i = 0; i < 6; ++i) {
                int slotNum = startSlot + i;
                if (slotNum > 108) break;
                const auto* entry = DataManager::getEncyclopedia().getEntry(slotNum);
                int ey = 32 + i * 21;

                Color c = (codexCursor == slotNum) ? Palette::Yellow : Palette::White;
                if (codexCursor == slotNum) {
                    renderer.drawPanel(6, ey - 2, 126, 19, Color(40, 44, 56), Palette::Yellow);
                    FontRenderer::drawText(renderer, 8, ey + 3, ">", Palette::Yellow);
                }

                std::string numStr = std::string("#") + ((slotNum < 10) ? "00" : (slotNum < 100 ? "0" : "")) + std::to_string(slotNum);
                FontRenderer::drawText(renderer, 16, ey + 3, numStr, Palette::LightGray);

                if (entry) {
                    if (entry->status == DiscoveryStatus::Captured) {
                        FontRenderer::drawText(renderer, 50, ey + 3, entry->nameKo, Palette::White);
                    } else if (entry->status == DiscoveryStatus::Seen) {
                        FontRenderer::drawText(renderer, 50, ey + 3, entry->nameKo, Palette::MidGray);
                    } else {
                        FontRenderer::drawText(renderer, 50, ey + 3, "???", Palette::DarkGray);
                    }
                }
            }

            // Right: Detail Panel
            renderer.drawPanel(138, 26, 178, 136, Color(16, 18, 24), Palette::MidGray);
            const auto* selected = DataManager::getEncyclopedia().getEntry(codexCursor);
            if (selected) {
                std::string titleNum = "No." + std::to_string(selected->number) + " " + selected->nameKo;
                FontRenderer::drawText(renderer, 144, 32, titleNum, Palette::Yellow);

                if (selected->status == DiscoveryStatus::Captured) {
                    renderer.fillRect(144, 44, 55, 9, Palette::Jade);
                    FontRenderer::drawText(renderer, 146, 45, "[계약완료]", Palette::Black);

                    std::string gStr = "등급: Grade " + std::to_string(static_cast<int>(selected->baseGrade));
                    FontRenderer::drawText(renderer, 206, 45, gStr, Palette::White);

                    FontRenderer::drawText(renderer, 144, 58, "출처: " + selected->origin, Palette::LightGray);
                    
                    // Multi-line lore
                    FontRenderer::drawText(renderer, 144, 72, "[전승 및 배경 설화]", Palette::Yellow);
                    FontRenderer::drawText(renderer, 144, 84, selected->lore.substr(0, 24), Palette::White);
                    if (selected->lore.length() > 24) {
                        FontRenderer::drawText(renderer, 144, 96, selected->lore.substr(24, 24), Palette::White);
                    }
                } else if (selected->status == DiscoveryStatus::Seen) {
                    renderer.fillRect(144, 44, 55, 9, Palette::Blue);
                    FontRenderer::drawText(renderer, 146, 45, "[조우함]", Palette::Black);
                    FontRenderer::drawText(renderer, 144, 65, "전투에서 조우하였으나", Palette::LightGray);
                    FontRenderer::drawText(renderer, 144, 78, "아직 계약을 맺지 못함.", Palette::LightGray);
                } else {
                    renderer.fillRect(144, 44, 55, 9, Palette::DarkGray);
                    FontRenderer::drawText(renderer, 146, 45, "[미확인]", Palette::Black);
                    FontRenderer::drawText(renderer, 144, 70, "조선의 미지의 요괴입니다.", Palette::MidGray);
                    FontRenderer::drawText(renderer, 144, 84, "야생에서 조우하십시오.", Palette::MidGray);
                }
            }

            // Footer
            renderer.fillRect(0, SCREEN_HEIGHT - 14, SCREEN_WIDTH, 14, Palette::Black);
            FontRenderer::drawText(renderer, 4, SCREEN_HEIGHT - 11, "ARROWS:Scroll | LEFT/RIGHT:Skip 10 | X/Z:Back", Palette::White);
        }
        else if (gameState == GameState::PartyManagementView) {
            // Party Management & Promotion View
            renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(20, 24, 32));
            renderer.drawPanel(4, 4, 312, 20, Color(30, 36, 48), Palette::Blue);
            FontRenderer::drawText(renderer, 10, 8, "=== 파티 관리 및 요괴 승급 (PARTY & GROWTH) ===", Palette::Yellow);

            // Left: 3 Party Slots
            renderer.drawPanel(4, 26, 140, 136, Color(16, 20, 28), Palette::MidGray);
            for (size_t i = 0; i < playerParty.getSize(); ++i) {
                const Yokai* member = playerParty.getYokai(i);
                if (!member) continue;
                int py = 32 + static_cast<int>(i) * 42;

                if (partyViewCursor == static_cast<int>(i)) {
                    renderer.drawPanel(6, py - 2, 136, 40, Color(36, 44, 60), Palette::Yellow);
                    FontRenderer::drawText(renderer, 8, py + 2, ">", Palette::Yellow);
                }

                FontRenderer::drawText(renderer, 16, py + 2, member->getName() + " Lv." + std::to_string(member->getLevel()), Palette::White);
                std::string gStr = "Grade " + std::to_string(static_cast<int>(member->getGrade()));
                FontRenderer::drawText(renderer, 92, py + 2, gStr, Palette::Yellow);

                std::string hpStr = "HP " + std::to_string(member->getStats().hp) + "/" + std::to_string(member->getStats().maxHp);
                FontRenderer::drawText(renderer, 16, py + 14, hpStr, Palette::Jade);

                int expNext = member->getExpToNextLevel();
                std::string expStr = "EXP " + std::to_string(member->getExp()) + "/" + std::to_string(expNext);
                FontRenderer::drawText(renderer, 16, py + 24, expStr, Palette::LightGray);
            }

            // Right: Detailed Stats & Promotion Box
            renderer.drawPanel(148, 26, 168, 136, Color(16, 20, 28), Palette::MidGray);
            const Yokai* selectedMember = playerParty.getYokai(partyViewCursor);
            if (selectedMember) {
                FontRenderer::drawText(renderer, 154, 32, "== " + selectedMember->getName() + " 상세 능력치 ==", Palette::Yellow);
                FontRenderer::drawText(renderer, 154, 46, "완력(ATK): " + std::to_string(selectedMember->getStats().atk), Palette::White);
                FontRenderer::drawText(renderer, 230, 46, "호신(DEF): " + std::to_string(selectedMember->getStats().def), Palette::White);
                FontRenderer::drawText(renderer, 154, 58, "신법(SPD): " + std::to_string(selectedMember->getStats().spd), Palette::White);
                FontRenderer::drawText(renderer, 230, 58, "영력(Qi) : " + std::to_string(selectedMember->getStats().maxQi), Palette::Jade);

                // Skills summary
                FontRenderer::drawText(renderer, 154, 74, "[장착 기술 목록]", Palette::LightGray);
                const auto& skills = selectedMember->getSkills();
                for (size_t s = 0; s < skills.size() && s < 4; ++s) {
                    int sky = 86 + static_cast<int>(s) * 11;
                    FontRenderer::drawText(renderer, 154, sky, std::to_string(s + 1) + "." + skills[s].name, Palette::White);
                }

                // Promotion Button
                renderer.drawPanel(154, 132, 156, 24, selectedMember->canPromote() ? Color(20, 60, 30) : Color(40, 40, 40), Palette::Yellow);
                if (selectedMember->canPromote()) {
                    FontRenderer::drawText(renderer, 162, 139, "[Z키: 등급 승급(Promotion)!]", Palette::Yellow);
                } else {
                    FontRenderer::drawText(renderer, 160, 139, "승급 조건 미충족 (Lv부족)", Palette::MidGray);
                }
            }

            // Feedback / Footer
            if (!partyFeedbackMsg.empty()) {
                renderer.fillRect(4, 148, 140, 12, Color(20, 50, 30));
                FontRenderer::drawText(renderer, 8, 150, partyFeedbackMsg, Palette::Yellow);
            }

            renderer.fillRect(0, SCREEN_HEIGHT - 14, SCREEN_WIDTH, 14, Palette::Black);
            FontRenderer::drawText(renderer, 4, SCREEN_HEIGHT - 11, "UP/DOWN:Select | Z:Promote Grade | F1/X:Close", Palette::White);
        }
        else if (gameState == GameState::ArtifactMenu) {
            // Artifact Inventory Dual-Panel View
            renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(20, 24, 28));
            
            // Header
            renderer.drawPanel(4, 4, 312, 20, Color(32, 38, 44), Palette::Yellow);
            FontRenderer::drawText(renderer, 10, 8, "=== 유물 보관함 (ARTIFACT INVENTORY) ===", Palette::Yellow);
            std::string slotCountStr = "보유: " + std::to_string(artifacts.getCount()) + "/8";
            FontRenderer::drawText(renderer, 248, 8, slotCountStr, Palette::Jade);

            // Left: Artifact List (8 Slots)
            renderer.drawPanel(4, 26, 130, 136, Color(16, 18, 22), Palette::MidGray);
            const auto& artList = artifacts.getArtifacts();

            if (artList.empty()) {
                FontRenderer::drawText(renderer, 10, 36, "보유 유물 없음", Palette::LightGray);
            } else {
                for (size_t i = 0; i < artList.size(); ++i) {
                    int ay = 30 + static_cast<int>(i) * 16;
                    Color c = (artCursor == static_cast<int>(i)) ? Palette::Yellow : Palette::White;
                    if (artCursor == static_cast<int>(i)) {
                        renderer.drawPanel(6, ay - 2, 126, 15, Color(36, 42, 52), Palette::Yellow);
                        FontRenderer::drawText(renderer, 8, ay + 1, ">", Palette::Yellow);
                    }
                    FontRenderer::drawText(renderer, 16, ay + 1, std::to_string(i + 1) + ". " + artList[i].name, c);
                }
            }

            // Right: Artifact Details (Buff & Debuff Coexistence Panel)
            renderer.drawPanel(138, 26, 178, 136, Color(16, 18, 22), Palette::MidGray);
            const auto* selectedArt = artifacts.getArtifact(artCursor);
            if (selectedArt) {
                FontRenderer::drawText(renderer, 144, 30, selectedArt->name, Palette::Yellow);
                
                // Buff Box (Green)
                renderer.fillRect(144, 42, 40, 9, Palette::Green);
                FontRenderer::drawText(renderer, 146, 43, "[이점]", Palette::Black);
                FontRenderer::drawText(renderer, 144, 54, ArtifactInventory::getBuffDescription(*selectedArt), Palette::Jade);

                // Debuff Box (Red)
                renderer.fillRect(144, 68, 40, 9, Palette::Red);
                FontRenderer::drawText(renderer, 146, 69, "[대가]", Palette::Black);
                FontRenderer::drawText(renderer, 144, 80, ArtifactInventory::getDebuffDescription(*selectedArt), Palette::Red);

                // Lore
                FontRenderer::drawText(renderer, 144, 98, "[배경 설화]", Palette::LightGray);
                FontRenderer::drawText(renderer, 144, 108, selectedArt->lore.substr(0, 24), Palette::MidGray);

                // Destruction Action Button
                renderer.drawPanel(144, 134, 168, 22, Color(45, 20, 20), Palette::Red);
                FontRenderer::drawText(renderer, 148, 140, "[Z키: 즉시 파괴 (영기 환원)]", Palette::Yellow);
            } else {
                FontRenderer::drawText(renderer, 144, 40, "선택된 유물이 없습니다.", Palette::MidGray);
            }

            // Feedback Message
            if (!artFeedbackMsg.empty()) {
                renderer.fillRect(4, 148, 130, 12, Color(20, 50, 30));
                FontRenderer::drawText(renderer, 8, 150, "파괴 완료! 영기 환원됨", Palette::Jade);
            }

            // Footer
            renderer.fillRect(0, SCREEN_HEIGHT - 14, SCREEN_WIDTH, 14, Palette::Black);
            FontRenderer::drawText(renderer, 4, SCREEN_HEIGHT - 11, "UP/DOWN:Select | Z:Instant Destroy | X/C:Close", Palette::White);
        }

        // Present Framebuffer to Win32 Window
        window.present(renderer);

        // Frame pacing (~60 FPS)
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}
