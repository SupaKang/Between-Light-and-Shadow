#include "world_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/scene_stack.hpp"
#include "../core/save_system.hpp"
#include "../ui/font_renderer.hpp"
#include "../data/data_manager.hpp"
#include "../audio/audio_engine.hpp"
#include "../battle/battle_scene.hpp"
#include "../scenes/encyclopedia_scene.hpp"
#include "../scenes/party_scene.hpp"
#include "../scenes/artifact_scene.hpp"
#include "../scenes/quest_scene.hpp"
#include "../scenes/yutnori_scene.hpp"
#include "../scenes/settings_scene.hpp"
#include "../scenes/ending_scene.hpp"
#include "../gameplay/alchemy.hpp"
#include "field_obstacle.hpp"
#include <algorithm>

namespace JoseonRPG {

WorldScene::WorldScene(Party& party, ArtifactInventory& artifacts, int& money)
    : m_party(party), m_artifacts(artifacts), m_money(money), m_gridController(7, 6, Direction::South), m_rng(2026) {
    
    m_gridController.setOnStepComplete([this](int gx, int gy) {
        checkStepEvents(gx, gy);
    });
}

void WorldScene::onEnter() {
    AudioEngine::playBgm(BgmTrack::Village);
    FieldObstacleManager::init();
    m_tilemap.loadMap(0);
    m_weather.setWeatherForMap(0);
    m_gridController.setPosition(7, 6);
    m_noticeMsg = "[" + m_tilemap.getMapName() + "] 진입";
}

void WorldScene::onResume() {
    AudioEngine::playBgm(BgmTrack::Village);
    m_weather.setWeatherForMap(m_tilemap.getMapId());
    m_activeNPC = nullptr;
}

void WorldScene::setPlayerPosition(int gridX, int gridY, int mapId) {
    if (mapId >= 0 && mapId != m_tilemap.getMapId()) {
        m_tilemap.loadMap(mapId);
        m_weather.setWeatherForMap(mapId);
    }
    m_gridController.setPosition(gridX, gridY);
}

void WorldScene::checkStepEvents(int newGridX, int newGridY) {
    // 1. Warp Trigger Check
    const WarpTrigger* warp = m_tilemap.checkWarp(newGridX, newGridY);
    if (warp) {
        m_tilemap.loadMap(warp->targetMapId);
        m_weather.setWeatherForMap(warp->targetMapId);
        m_gridController.setPosition(warp->targetX, warp->targetY);
        m_noticeMsg = "[" + m_tilemap.getMapName() + "] 진입";
        return;
    }

    // 2. Wild Encounter Check across dangerous maps (15% roll)
    if (m_tilemap.getMapId() == 1 || m_tilemap.getMapId() == 3 || m_tilemap.getMapId() == 4) {
        std::uniform_int_distribution<int> encRoll(1, 100);
        if (encRoll(m_rng) <= 15) {
            const auto& pool = DataManager::getAllYokaiTemplates();
            size_t maxIdx = 107;
            size_t minIdx = 0;
            if (m_tilemap.getMapId() == 3) { minIdx = 20; maxIdx = 70; }
            else if (m_tilemap.getMapId() == 4) { minIdx = 50; maxIdx = 107; }
            else { minIdx = 0; maxIdx = 35; }

            std::uniform_int_distribution<size_t> poolDist(minIdx, std::min(maxIdx, pool.size() - 6));
            Yokai wildEnemy = pool[poolDist(m_rng)];

            int wildLevel = 3;
            if (m_tilemap.getMapId() == 3) wildLevel = 15;
            else if (m_tilemap.getMapId() == 4) wildLevel = 30;
            wildEnemy.gainExp(wildLevel * 250);

            if (m_sceneStack) {
                m_sceneStack->pushScene(std::make_unique<BattleScene>(
                    m_party, wildEnemy, m_artifacts, false, [this](bool won) {
                        if (!won) {
                            // Respawn at Doseonsa Tavern on defeat
                            setPlayerPosition(7, 6, 0);
                            m_party.healAll();
                            m_noticeMsg = "도선사 주막으로 후송되어 응급 처치를 받았습니다.";
                        }
                    }
                ));
            }
        }
    }
}

void WorldScene::saveGame() {
    GameRuntimeContext ctx;
    ctx.mapId = m_tilemap.getMapId();
    ctx.gridX = m_gridController.getGridX();
    ctx.gridY = m_gridController.getGridY();
    ctx.facing = static_cast<int>(m_gridController.getFacing());
    ctx.money = m_money;
    ctx.party = &m_party;
    ctx.artifacts = &m_artifacts;
    ctx.encyclopedia = &DataManager::getEncyclopedia();
    ctx.questManager = &DataManager::getQuestManager();

    if (SaveSystem::saveToSlot(1, ctx)) {
        m_noticeMsg = "★ [저장 완료] 1번 슬롯에 진행 상황이 안전하게 저장되었습니다! ★";
    } else {
        m_noticeMsg = "저장 실패!";
    }
}

void WorldScene::interactWithNPC() {
    int px = m_gridController.getGridX();
    int py = m_gridController.getGridY();

    // 1. Check NPC Interaction first
    auto currentNPCs = DataManager::getNPCsForMap(m_tilemap.getMapId());
    for (const auto& n : currentNPCs) {
        int dist = std::abs(n.gridX - px) + std::abs(n.gridY - py);
        if (dist <= 1) {
            m_activeNPC = &n;
            std::string speakerTitle = n.nameKo + " [" + n.titleKo + "]";

            m_dialogueBox.startDialogue(speakerTitle, n.dialogue, [this, n]() {
                // Actions after dialogue finishes
                if (n.nameKo == "주모 월선" || n.nameKo == "이 생원") {
                    if (m_sceneStack) {
                        m_sceneStack->pushScene(std::make_unique<YutnoriScene>(m_money));
                    }
                }
                else if (n.nameKo == "혜민서 의원") {
                    AlchemySystem::brewPotion(AlchemyRecipeId::VitalityDecoction, m_party, m_money, m_noticeMsg);
                }
                else if (n.nameKo == "천명영호") {
                    triggerBossBattle("YOKAI_108", "", "", 10000, "★ [환상비무대 제패!] 전설의 영수 천명영호를 꺾고 조선 제일의 음양사로 등극하셨습니다! ★");
                }
                else if (n.actionType == NPCActionType::TavernRest) {
                    m_party.healAll();
                    m_noticeMsg = "주막에서 하룻밤 묵었습니다. 파티 전원의 체력/영력 완치!";
                    DataManager::getQuestManager().advanceQuest("MQ_001");
                }
                else if (n.actionType == NPCActionType::QuestTrigger) {
                    DataManager::getQuestManager().startQuest(n.associatedQuestId);
                    m_noticeMsg = "퀘스트 [" + n.associatedQuestId + "] 수주 완료!";
                }
                else if (n.actionType == NPCActionType::BossEncounter) {
                    std::string bossId = "YOKAI_BOSS_01";
                    std::string questId = "MQ_001";
                    std::string nextQ = "MQ_002";
                    int rMoney = 500;
                    std::string vNotice = "★ [챕터 1 클리어] 음양당 괴승 묘각 격파! 제2구역 소백산맥 개방! ★";

                    if (n.associatedQuestId == "MQ_002") {
                        bossId = "YOKAI_BOSS_02"; questId = "MQ_002"; nextQ = "MQ_003"; rMoney = 800;
                        vNotice = "★ [챕터 2 클리어] 철포방주 격파! 남해 포구 개방! ★";
                    } else if (n.associatedQuestId == "MQ_003") {
                        bossId = "YOKAI_BOSS_03"; questId = "MQ_003"; nextQ = "MQ_004"; rMoney = 1200;
                        vNotice = "★ [챕터 3 클리어] 수로방주 격파! 지리산 여우골 개방! ★";
                    } else if (n.associatedQuestId == "MQ_004") {
                        bossId = "YOKAI_BOSS_04"; questId = "MQ_004"; nextQ = "MQ_005"; rMoney = 1800;
                        vNotice = "★ [챕터 4 클리어] 음양좌호법 격파! 최종 성채 개방! ★";
                    } else if (n.associatedQuestId == "MQ_005") {
                        bossId = "YOKAI_BOSS_05"; questId = "MQ_005"; nextQ = ""; rMoney = 5000;
                        vNotice = "★ [축하합니다! 전 챕터 엔딩] 음양당 궤멸 및 조선의 평화 수호 완료! ★";
                    }

                    triggerBossBattle(bossId, questId, nextQ, rMoney, vNotice);
                }
                m_activeNPC = nullptr;
            });
            return;
        }
    }

    // 2. Check Treasure Chest Interaction (adjacent 4 tiles)
    const int dxs[4] = {0, 0, -1, 1};
    const int dys[4] = {-1, 1, 0, 0};
    for (int i = 0; i < 4; ++i) {
        int cx = px + dxs[i];
        int cy = py + dys[i];
        MapChest* chest = m_tilemap.checkChest(cx, cy);
        if (chest && !chest->opened) {
            chest->opened = true;
            AudioEngine::playSfx(SfxId::ChestOpen);
            m_money += chest->money;
            std::string rewardSummary = "엽전 " + std::to_string(chest->money) + "냥";

            if (!chest->artifactId.empty()) {
                Artifact art = DataManager::createArtifactById(chest->artifactId);
                if (!art.id.empty()) {
                    m_artifacts.addArtifact(art);
                    rewardSummary += " & 유물 [" + art.name + "]";
                }
            }

            if (chest->exp > 0 && m_party.getActiveYokai()) {
                m_party.getActiveYokai()->gainExp(chest->exp);
                rewardSummary += " & EXP " + std::to_string(chest->exp);
            }

            m_noticeMsg = "★ [보물 상자 개방!] " + rewardSummary + " 획득! ★";
            return;
        }
    }

    // 3. Check Field Obstacle Interaction (adjacent 4 tiles)
    for (int i = 0; i < 4; ++i) {
        int ox = px + dxs[i];
        int oy = py + dys[i];
        FieldObstacle* obs = FieldObstacleManager::getObstacleAt(m_tilemap.getMapId(), ox, oy);
        if (obs) {
            FieldObstacleManager::tryClearObstacle(*obs, m_party, m_noticeMsg);
            return;
        }
    }
}

void WorldScene::triggerBossBattle(const std::string& bossId, const std::string& questIdToComplete, const std::string& nextQuestId, int rewardMoney, const std::string& victoryNotice) {
    Yokai bossYokai = DataManager::createYokaiById(bossId);
    if (m_sceneStack) {
        m_sceneStack->pushScene(std::make_unique<BattleScene>(
            m_party, bossYokai, m_artifacts, true, [this, bossId, questIdToComplete, nextQuestId, rewardMoney, victoryNotice](bool won) {
                if (won) {
                    if (!questIdToComplete.empty()) {
                        DataManager::getQuestManager().completeQuest(questIdToComplete);
                    }
                    if (!nextQuestId.empty()) {
                        DataManager::getQuestManager().startQuest(nextQuestId);
                    }
                    m_money += rewardMoney;
                    m_noticeMsg = victoryNotice;

                    if (bossId == "YOKAI_BOSS_05" && m_sceneStack) {
                        m_sceneStack->pushScene(std::make_unique<EndingScene>(m_party, m_artifacts, DataManager::getEncyclopedia()));
                    }
                } else {
                    setPlayerPosition(7, 6, 0);
                    m_party.healAll();
                    m_noticeMsg = "도선사 주막으로 후송되어 응급 처치를 받았습니다.";
                }
            }
        ));
    }
}

void WorldScene::handleInput() {
    if (m_dialogueBox.isActive()) {
        m_dialogueBox.handleInput();
        return;
    }

    if (!m_gridController.isMoving()) {
        auto walkableCheck = [this](int tx, int ty) -> bool {
            if (m_tilemap.isSolid(tx, ty)) return false;
            if (FieldObstacleManager::getObstacleAt(m_tilemap.getMapId(), tx, ty)) return false;
            auto npcs = DataManager::getNPCsForMap(m_tilemap.getMapId());
            for (const auto& n : npcs) {
                if (n.gridX == tx && n.gridY == ty) return false;
            }
            return true;
        };

        if (Input::isDown(Key::Up)) m_gridController.tryMove(Direction::North, walkableCheck);
        else if (Input::isDown(Key::Down)) m_gridController.tryMove(Direction::South, walkableCheck);
        else if (Input::isDown(Key::Left)) m_gridController.tryMove(Direction::West, walkableCheck);
        else if (Input::isDown(Key::Right)) m_gridController.tryMove(Direction::East, walkableCheck);
    }

    // NPC Interaction
    if (Input::isPressed(Key::ActionA)) {
        interactWithNPC();
    }

    // Open Codex (X key)
    if (Input::isPressed(Key::ActionB)) {
        if (m_sceneStack) {
            m_sceneStack->pushScene(std::make_unique<EncyclopediaScene>(DataManager::getEncyclopedia()));
        }
    }

    // Open Artifacts (C key)
    if (Input::isPressed(Key::ActionC)) {
        if (m_sceneStack) {
            m_sceneStack->pushScene(std::make_unique<ArtifactScene>(m_artifacts, m_party));
        }
    }

    // Open Party (V / P / F1 key)
    if (Input::isPressed(Key::ActionD) || Input::isPressed(Key::Debug)) {
        if (m_sceneStack) {
            m_sceneStack->pushScene(std::make_unique<PartyScene>(m_party));
        }
    }

    // Open Quest Log (Q / L key)
    if (Input::isPressed(Key::ActionE)) {
        if (m_sceneStack) {
            m_sceneStack->pushScene(std::make_unique<QuestScene>(DataManager::getQuestManager()));
        }
    }

    // Open Settings (ESC / Menu key)
    if (Input::isPressed(Key::Menu)) {
        if (m_sceneStack) {
            m_sceneStack->pushScene(std::make_unique<SettingsScene>());
        }
    }

    // Quick Save (F5 / S key)
    if (Input::isPressed(Key::Save)) {
        saveGame();
    }
}

void WorldScene::update(float dt) {
    if (m_dialogueBox.isActive()) {
        m_dialogueBox.update(dt);
    }

    m_gridController.update(dt);
    m_camera.update(m_gridController.getPixelX(), m_gridController.getPixelY(), m_tilemap.getWidth(), m_tilemap.getHeight());
    m_weather.update(dt);
}

void WorldScene::render(Renderer& renderer) {
    // 1. Render Tilemap
    m_tilemap.render(renderer, m_camera.getX(), m_camera.getY());

    // 2. Render NPCs
    auto mapNPCs = DataManager::getNPCsForMap(m_tilemap.getMapId());
    for (const auto& n : mapNPCs) {
        int nx = n.gridX * TILE_SIZE - m_camera.getX();
        int ny = n.gridY * TILE_SIZE - m_camera.getY();
        renderer.drawSprite(nx, ny, n.spriteId, 0);
    }

    // 3. Render Player Sprite with walk animation
    int screenPX = m_gridController.getPixelX() - m_camera.getX();
    int screenPY = m_gridController.getPixelY() - m_camera.getY();
    renderer.drawSprite(screenPX, screenPY, 0, m_gridController.getAnimFrame());

    // 4. Regional Ambient Weather and Atmosphere Overlay
    m_weather.render(renderer);

    // 5. World Top HUD Overlay
    renderer.fillRect(0, 0, SCREEN_WIDTH, 12, Color(18, 18, 22, 220));
    FontRenderer::drawText(renderer, 4, 2, m_tilemap.getMapName(), Palette::Yellow);

    const Quest* mq = DataManager::getQuestManager().getQuest("MQ_001");
    if (mq && mq->state == QuestState::InProgress) {
        std::string qStr = "[임무] " + mq->getCurrentObjective();
        FontRenderer::drawText(renderer, 96, 2, qStr, Palette::Jade);
    }

    std::string moneyStr = std::to_string(m_money) + "냥";
    FontRenderer::drawText(renderer, SCREEN_WIDTH - 46, 2, moneyStr, Palette::Yellow);

    // 5. Bottom Controls Hint / System Notice
    renderer.fillRect(0, SCREEN_HEIGHT - 12, SCREEN_WIDTH, 12, Color(18, 18, 22, 220));
    if (!m_noticeMsg.empty()) {
        FontRenderer::drawText(renderer, 4, SCREEN_HEIGHT - 10, m_noticeMsg, Palette::Yellow);
    } else {
        FontRenderer::drawText(renderer, 4, SCREEN_HEIGHT - 10, "방향키:이동 | Z:대화 | X:도감 | C:유물 | V:파티 | Q:임무 | F5:저장", Palette::White);
    }

    // 6. Dialogue Box Overlay
    if (m_dialogueBox.isActive()) {
        m_dialogueBox.render(renderer);
    }
}

} // namespace JoseonRPG
