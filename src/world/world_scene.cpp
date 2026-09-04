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
        m_fadeAlpha = 1.0f;
    }
    // Clamping to valid map boundary and safety from collision
    int clampedX = std::clamp(gridX, 1, m_tilemap.getWidth() - 2);
    int clampedY = std::clamp(gridY, 1, m_tilemap.getHeight() - 2);
    if (m_tilemap.isSolid(clampedX, clampedY)) {
        const int dx[] = {0, 1, -1, 0, 1, -1, 1, -1};
        const int dy[] = {1, 0, 0, -1, 1, 1, -1, -1};
        for (int i = 0; i < 8; ++i) {
            int nx = clampedX + dx[i];
            int ny = clampedY + dy[i];
            if (!m_tilemap.isSolid(nx, ny)) {
                clampedX = nx;
                clampedY = ny;
                break;
            }
        }
    }
    m_gridController.setPosition(clampedX, clampedY);
}

void WorldScene::checkStepEvents(int newGridX, int newGridY) {
    // 1. Warp Trigger Check
    const WarpTrigger* warp = m_tilemap.checkWarp(newGridX, newGridY);
    if (warp) {
        AudioEngine::playSfx(SfxId::MapWarp);
        setPlayerPosition(warp->targetX, warp->targetY, warp->targetMapId);
        m_noticeMsg = "[" + m_tilemap.getMapName() + "] 진입";
        return;
    }

    // 2. Wild Encounter Check across dangerous maps (15% roll)
    int mapId = m_tilemap.getMapId();
    // Exclude safe havens: 0 (Village), 1 (Tavern), 2 (Bureau), 8 (Hut), 12 (Port), 13 (Shelter), 18 (Hermitage), 26 (Hanyang)
    bool isSafeZone = (mapId == 0 || mapId == 1 || mapId == 2 || mapId == 8 || mapId == 12 || mapId == 13 || mapId == 18 || mapId == 26);
    if (!isSafeZone) {
        std::uniform_int_distribution<int> encRoll(1, 100);
        if (encRoll(m_rng) <= 15) {
            const auto& pool = DataManager::getAllYokaiTemplates();
            size_t minIdx = 0;
            size_t maxIdx = 25;
            int wildLevel = 3;

            if (mapId >= 3 && mapId <= 5) {
                // Zone 1: 북한산 & 고대 석굴 & 대웅전 (Lv 3 ~ 9)
                minIdx = 0; maxIdx = 25;
                wildLevel = 3 + (mapId - 3) * 2;
            } else if (mapId >= 6 && mapId <= 10) {
                // Zone 2: 죽령 옛고개 & 소백산맥 & 무쇠광산 (Lv 10 ~ 19)
                minIdx = 20; maxIdx = 55;
                wildLevel = 10 + (mapId == 6 ? 0 : (mapId == 7 ? 2 : (mapId == 9 ? 5 : 8)));
            } else if (mapId >= 11 && mapId <= 15) {
                // Zone 3: 남해안 갈대밭 & 유령 난파선 (Lv 20 ~ 28)
                minIdx = 45; maxIdx = 80;
                wildLevel = 20 + (mapId == 11 ? 0 : (mapId == 14 ? 3 : 7));
            } else if (mapId >= 16 && mapId <= 20) {
                // Zone 4: 지리산 원시림 & 여우골 (Lv 28 ~ 37)
                minIdx = 65; maxIdx = 100;
                wildLevel = 28 + (mapId == 16 ? 0 : (mapId == 17 ? 3 : (mapId == 19 ? 5 : 8)));
            } else if (mapId >= 21 && mapId <= 25) {
                // Zone 5: 음양당 성채 & 수호탑 & 본당 & 태초 심연 (Lv 38 ~ 48)
                minIdx = 85; maxIdx = std::min<size_t>(107, pool.size() - 6);
                wildLevel = 38 + (mapId - 21) * 2;
            } else if (mapId == 27) {
                // Zone 6A: 금강산 일만이천봉 선계 (Lv 40 ~ 44)
                minIdx = 50; maxIdx = 90;
                wildLevel = 42;
            } else if (mapId == 28) {
                // Zone 6B: 흑산도 심해 용궁 (Lv 43 ~ 46)
                minIdx = 60; maxIdx = 95;
                wildLevel = 45;
            } else if (mapId == 29) {
                // Zone 6C: 백두산 천지 백록담 신역 (Lv 46 ~ 49)
                minIdx = 75; maxIdx = 105;
                wildLevel = 48;
            } else if (mapId == 30) {
                // Zone 6D: 태고의 환상비무 미궁 (Lv 48 ~ 50)
                minIdx = 85; maxIdx = 107;
                wildLevel = 50;
            } else if (mapId == 31) {
                // Connecting Passage A: 지하 암반 수로 (Lv 15 ~ 22)
                minIdx = 10; maxIdx = 45;
                wildLevel = 18;
            } else if (mapId == 32) {
                // Connecting Passage B: 소백-지리 봉화대 능선 (Lv 25 ~ 32)
                minIdx = 30; maxIdx = 70;
                wildLevel = 28;
            } else if (mapId == 33) {
                // Connecting Passage C: 난파선 침수 밀실 (Lv 30 ~ 38)
                minIdx = 40; maxIdx = 85;
                wildLevel = 34;
            } else if (mapId == 34) {
                // Connecting Passage D: 여우골 영맥 환상 샛길 (Lv 35 ~ 44)
                minIdx = 60; maxIdx = 95;
                wildLevel = 40;
            } else if (mapId == 35) {
                // Connecting Passage E: 성채 지하 감옥 & 흑마술 비밀통로 (Lv 42 ~ 48)
                minIdx = 80; maxIdx = 106;
                wildLevel = 46;
            }

            maxIdx = std::min(maxIdx, pool.size() - 6);
            if (minIdx > maxIdx) minIdx = 0;

            std::uniform_int_distribution<size_t> poolDist(minIdx, maxIdx);
            Yokai wildEnemy = pool[poolDist(m_rng)];
            wildEnemy.setLevel(wildLevel);

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

            auto handleNPCAction = [this, n, px](int optIdx) {
                if (n.nameKo == "주모 월선") {
                    if (optIdx == 0) {
                        AudioEngine::playSfx(SfxId::TavernHeal);
                        m_party.healAll();
                        m_noticeMsg = "주막에서 하룻밤 묵었습니다. 파티 전원의 체력/영력 완치!";
                        DataManager::getQuestManager().advanceQuest("MQ_001");
                    } else if (optIdx == 1 && m_sceneStack) {
                        m_sceneStack->pushScene(std::make_unique<YutnoriScene>(m_money));
                    }
                }
                else if (n.nameKo == "훈장 이 생원" || n.nameKo == "해녀대장 진주") {
                    if (optIdx == 0 && !n.associatedQuestId.empty()) {
                        DataManager::getQuestManager().startQuest(n.associatedQuestId);
                        m_noticeMsg = "퀘스트 [" + n.associatedQuestId + "] 수주 완료!";
                    } else if (optIdx == 1 && m_sceneStack) {
                        m_sceneStack->pushScene(std::make_unique<YutnoriScene>(m_money));
                    }
                }
                else if (n.nameKo == "혜민서 의원") {
                    AlchemySystem::brewPotion(AlchemyRecipeId::VitalityDecoction, m_party, m_money, m_noticeMsg);
                }
                else if (n.nameKo == "천명영호") {
                    triggerBossBattle("YOKAI_108", "", "", 10000, "★ [환상비무대 제패!] 전설의 영수 천명영호를 꺾고 조선 제일의 음양사로 등극하셨습니다! ★");
                }
                else if (n.nameKo == "환상비무대 수호령") {
                    if (optIdx == 0) {
                        if (!n.associatedQuestId.empty()) DataManager::getQuestManager().startQuest(n.associatedQuestId);
                        triggerBossBattle("YOKAI_108", "SQ_010", "", 20000, "★ [태고 환상미궁 제패!] 5대 보스를 모두 격파하고 궁극의 천부인 요령을 획득하셨습니다! ★");
                    }
                }
                else if (n.actionType == NPCActionType::TavernRest) {
                    AudioEngine::playSfx(SfxId::TavernHeal);
                    m_party.healAll();
                    m_noticeMsg = "주막에서 하룻밤 묵었습니다. 파티 전원의 체력/영력 완치!";
                    DataManager::getQuestManager().advanceQuest("MQ_001");
                }
                else if (n.actionType == NPCActionType::QuestTrigger) {
                    if (optIdx == 0 && !n.associatedQuestId.empty()) {
                        DataManager::getQuestManager().startQuest(n.associatedQuestId);
                        m_noticeMsg = "퀘스트 [" + n.associatedQuestId + "] 수주 완료!";
                    }
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
                        bossId = "YOKAI_BOSS_05"; questId = "MQ_005"; nextQ = "MQ_006"; rMoney = 5000;
                        vNotice = "★ [1차 엔딩 달성] 음양당 궤멸 완료! 108 요괴도감을 완성하여 2차 진엔딩을 해금하십시오! ★";
                    } else if (n.associatedQuestId == "MQ_006") {
                        // Check 108 Yokai Full Codex Requirement for 2nd Ending
                        int cap = DataManager::getEncyclopedia().getCapturedCount();
                        if (cap < 108) {
                            m_noticeMsg = "★ [천명의 봉인] 108종 요괴도감을 전종 완성(108/108)해야만 시험에 응할 수 있습니다! (현재: " + std::to_string(cap) + "/108) ★";
                            m_activeNPC = nullptr;
                            return;
                        }
                    }
                    triggerBossBattle(bossId, questId, nextQ, rMoney, vNotice);
                }
                else if (n.actionType == NPCActionType::FerryCrossing) {
                    if (optIdx == 0) {
                        if (m_money >= 30) {
                            m_money -= 30;
                            AudioEngine::playSfx(SfxId::MapWarp);
                            if (px >= 50) {
                                // Cross from East to West
                                setPlayerPosition(38, 35, 38);
                                m_noticeMsg = "★ [나룻배 도강] 임진강을 건너 서안 절벽 선착장에 도착했습니다! (30냥 지불) ★";
                            } else {
                                // Cross from West to East
                                setPlayerPosition(55, 35, 38);
                                m_noticeMsg = "★ [나룻배 도강] 임진강을 건너 동안 포구 선착장에 도착했습니다! (30냥 지불) ★";
                            }
                        } else {
                            m_noticeMsg = "뱃삯 30냥이 부족하여 나룻배를 탈 수 없습니다!";
                        }
                    }
                }
                m_activeNPC = nullptr;
            };

            if (!n.options.empty()) {
                m_dialogueBox.startDialogueWithOptions(speakerTitle, n.dialogue, n.options, handleNPCAction);
            } else {
                m_dialogueBox.startDialogue(speakerTitle, n.dialogue, [handleNPCAction]() {
                    handleNPCAction(0);
                });
            }
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
                if (m_artifacts.addArtifact(art)) {
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
    if (bossId == "YOKAI_BOSS_01") bossYokai.setLevel(8);
    else if (bossId == "YOKAI_BOSS_02") bossYokai.setLevel(16);
    else if (bossId == "YOKAI_BOSS_03") bossYokai.setLevel(24);
    else if (bossId == "YOKAI_BOSS_04") bossYokai.setLevel(34);
    else if (bossId == "YOKAI_BOSS_05") bossYokai.setLevel(44);
    else if (bossId == "YOKAI_BOSS_06") bossYokai.setLevel(55);
    else if (bossId == "YOKAI_108") bossYokai.setLevel(50);

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
                        // 1st Ending (Normal Campaign Clear)
                        m_sceneStack->pushScene(std::make_unique<EndingScene>(m_party, m_artifacts, DataManager::getEncyclopedia(), EndingType::FirstEnding));
                    } else if (bossId == "YOKAI_BOSS_06" && m_sceneStack) {
                        // 2nd True Ending with Branching Philosophical Choice
                        m_dialogueBox.startDialogueWithOptions(
                            "태초의 삼신제석 [천명의 심판]",
                            std::vector<std::string>{"그대는 108 번뇌의 모든 요괴를 품고 시험을 완수하였다. 이제 천명을 선택하라."},
                            {"[1] 108 요괴 성불 및 신선 등선 (Nirvana Ascension)", "[2] 108 요괴와 이승 공존 및 조선 수호 (Earthly Coexistence)"},
                            [this](int choiceIdx) {
                                EndingType et = (choiceIdx == 0) ? EndingType::TrueEndingAscension : EndingType::TrueEndingCoexistence;
                                if (m_sceneStack) {
                                    m_sceneStack->pushScene(std::make_unique<EndingScene>(m_party, m_artifacts, DataManager::getEncyclopedia(), et));
                                }
                            }
                        );
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

void WorldScene::openStartMenu() {
    m_menuOpen = true;
    m_menuCursor = 0;
    AudioEngine::playSfx(SfxId::MenuSelect);
}

void WorldScene::closeStartMenu() {
    m_menuOpen = false;
    AudioEngine::playSfx(SfxId::MenuCancel);
}

void WorldScene::handleStartMenuInput() {
    if (Input::isPressed(Key::Up)) {
        if (m_menuCursor > 0) {
            m_menuCursor--;
            AudioEngine::playSfx(SfxId::MenuCursor);
        }
    }
    if (Input::isPressed(Key::Down)) {
        if (m_menuCursor < 6) {
            m_menuCursor++;
            AudioEngine::playSfx(SfxId::MenuCursor);
        }
    }

    if (Input::isPressed(Key::ActionA)) {
        AudioEngine::playSfx(SfxId::MenuSelect);
        int choice = m_menuCursor;
        m_menuOpen = false; // Close menu on selection
        switch (choice) {
            case 0: // 요괴도감 (POKEDEX)
                if (m_sceneStack) m_sceneStack->pushScene(std::make_unique<EncyclopediaScene>(DataManager::getEncyclopedia()));
                break;
            case 1: // 사역요괴 (POKEMON)
                if (m_sceneStack) m_sceneStack->pushScene(std::make_unique<PartyScene>(m_party));
                break;
            case 2: // 유물/가방 (PACK/BAG)
                if (m_sceneStack) m_sceneStack->pushScene(std::make_unique<ArtifactScene>(m_artifacts, m_party));
                break;
            case 3: // 임무록 (QUEST/GEAR)
                if (m_sceneStack) m_sceneStack->pushScene(std::make_unique<QuestScene>(DataManager::getQuestManager()));
                break;
            case 4: // 기록하기 (SAVE)
                saveGame();
                break;
            case 5: // 환경설정 (OPTION)
                if (m_sceneStack) m_sceneStack->pushScene(std::make_unique<SettingsScene>());
                break;
            case 6: // 닫기 (EXIT)
                break;
        }
        return;
    }

    if (Input::isPressed(Key::ActionB) || Input::isPressed(Key::Menu)) {
        closeStartMenu();
    }
}

void WorldScene::handleInput() {
    if (m_dialogueBox.isActive()) {
        m_dialogueBox.handleInput();
        return;
    }

    if (m_menuOpen) {
        handleStartMenuInput();
        return;
    }

    // Open Pokemon Gold Style Start Menu (ESC or Enter/Start/Menu key)
    if (Input::isPressed(Key::Menu)) {
        openStartMenu();
        return;
    }

    m_gridController.setRunning(Input::isDown(Key::Dash));

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

    // Interaction (Z / Space)
    if (Input::isPressed(Key::ActionA)) {
        interactWithNPC();
    }

    // Quick shortcuts for convenience
    if (Input::isPressed(Key::ActionB)) {
        if (m_sceneStack) m_sceneStack->pushScene(std::make_unique<EncyclopediaScene>(DataManager::getEncyclopedia()));
    }
    if (Input::isPressed(Key::ActionC)) {
        if (m_sceneStack) m_sceneStack->pushScene(std::make_unique<ArtifactScene>(m_artifacts, m_party));
    }
    if (Input::isPressed(Key::ActionD) || Input::isPressed(Key::Debug)) {
        if (m_sceneStack) m_sceneStack->pushScene(std::make_unique<PartyScene>(m_party));
    }
    if (Input::isPressed(Key::ActionE)) {
        if (m_sceneStack) m_sceneStack->pushScene(std::make_unique<QuestScene>(DataManager::getQuestManager()));
    }
    if (Input::isPressed(Key::Save)) {
        saveGame();
    }
}

void WorldScene::update(float dt) {
    if (m_dialogueBox.isActive()) {
        m_dialogueBox.update(dt);
    }

    if (m_fadeAlpha > 0.0f) {
        m_fadeAlpha = std::max(0.0f, m_fadeAlpha - dt * 3.5f);
    }

    if (!m_dialogueBox.isActive() && !m_menuOpen) {
        m_gridController.update(dt);
    }

    // Smooth Camera Follow
    m_camera.update(m_gridController.getPixelX(), m_gridController.getPixelY(),
                    m_tilemap.getWidth(), m_tilemap.getHeight());

    m_weather.update(dt);

    // Dust particles
    if (m_gridController.isRunning() && m_gridController.isMoving()) {
        m_dustTimer += dt;
        if (m_dustTimer >= 0.035f) {
            m_dustTimer = 0.0f;
            DustParticle p;
            p.x = static_cast<float>(m_gridController.getPixelX() + 8);
            p.y = static_cast<float>(m_gridController.getPixelY() + 14);

            std::uniform_real_distribution<float> jitter(-4.0f, 4.0f);
            switch (m_gridController.getFacing()) {
                case Direction::North: p.vy = 14.0f; p.vx = jitter(m_rng); break;
                case Direction::South: p.vy = -14.0f; p.vx = jitter(m_rng); break;
                case Direction::West:  p.vx = 14.0f; p.vy = jitter(m_rng); break;
                case Direction::East:  p.vx = -14.0f; p.vy = jitter(m_rng); break;
            }
            p.life = 0.0f;
            p.maxLife = 0.16f;
            m_dustParticles.push_back(p);
        }
    }

    for (auto& p : m_dustParticles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.life += dt;
    }
    m_dustParticles.erase(
        std::remove_if(m_dustParticles.begin(), m_dustParticles.end(), [](const DustParticle& p) {
            return p.life >= p.maxLife;
        }),
        m_dustParticles.end()
    );
}

void WorldScene::renderStartMenu(Renderer& renderer) {
    // 30% Dim background overlay
    renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(8, 16, 20, 80));

    // Pokemon Gold Style Right-Side Popup Menu Frame (9-Slice Paper Frame)
    int menuW = 96;
    int menuH = 126;
    int menuX = SCREEN_WIDTH - menuW - 6;
    int menuY = 6;

    renderer.draw9SliceBox(menuX, menuY, menuW, menuH, UITheme::Paper);

    const std::string menuItems[7] = {
        "1. 도  감",
        "2. 요  괴",
        "3. 가  방",
        "4. 임  무",
        "5. 기  록",
        "6. 설  정",
        "7. 닫  기"
    };

    for (int i = 0; i < 7; ++i) {
        int iy = menuY + 8 + i * 16;
        bool isSel = (m_menuCursor == i);

        if (isSel) {
            renderer.fillRect(menuX + 4, iy - 1, menuW - 8, 14, Color(136, 192, 112));
            FontRenderer::drawText(renderer, menuX + 6, iy, "▶", Color(8, 24, 32));
            FontRenderer::drawText(renderer, menuX + 18, iy, menuItems[i], Color(8, 24, 32));
        } else {
            FontRenderer::drawText(renderer, menuX + 18, iy, menuItems[i], Color(52, 104, 86));
        }
    }

    // Money & Area Info Sub-Window
    int infoY = menuY + menuH + 4;
    int infoH = 38;
    renderer.draw9SliceBox(menuX, infoY, menuW, infoH, UITheme::Paper);
    FontRenderer::drawText(renderer, menuX + 8, infoY + 6, "소지금", Color(52, 104, 86));
    FontRenderer::drawText(renderer, menuX + 8, infoY + 20, std::to_string(m_money) + " 냥", Color(8, 24, 32));
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

    // 3. Render Sprint Dust Particles
    for (const auto& p : m_dustParticles) {
        int sx = static_cast<int>(p.x) - m_camera.getX();
        int sy = static_cast<int>(p.y) - m_camera.getY();
        float t = p.life / p.maxLife;
        Color dustColor = (t < 0.5f) ? Palette::LightGray : Palette::MidGray;
        int size = (t < 0.5f) ? 2 : 1;
        renderer.fillRect(sx - size / 2, sy - size / 2, size, size, dustColor);
    }

    // 4. Render Player Sprite (16x24 crisp bitmapped)
    int screenPX = m_gridController.getPixelX() - m_camera.getX();
    int screenPY = m_gridController.getPixelY() - m_camera.getY();
    renderer.drawSprite(screenPX, screenPY, 0, m_gridController.getAnimFrame());

    // 5. Regional Ambient Weather
    m_weather.render(renderer);

    // 6. Screen Fade Transition
    if (m_fadeAlpha > 0.001f) {
        renderer.applyFade(1.0f - m_fadeAlpha);
    }

    // 7. Minimalist Location Banner (Only when noticeMsg exists)
    if (!m_noticeMsg.empty() && !m_dialogueBox.isActive() && !m_menuOpen) {
        int notW = static_cast<int>(m_noticeMsg.size() * 8 + 18);
        renderer.draw9SliceBox(4, 4, notW, 18, UITheme::Paper);
        FontRenderer::drawText(renderer, 8, 7, m_noticeMsg, Color(8, 24, 32));
    }

    // 8. Render Start Menu overlay if active
    if (m_menuOpen) {
        renderStartMenu(renderer);
    }

    // 9. Render Dialogue Box
    if (m_dialogueBox.isActive()) {
        m_dialogueBox.render(renderer);
    }
}

} // namespace JoseonRPG
