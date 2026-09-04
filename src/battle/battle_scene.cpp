#include "battle_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/scene_stack.hpp"
#include "../ui/font_renderer.hpp"
#include "../data/data_manager.hpp"
#include "../audio/audio_engine.hpp"
#include "../core/gen1_assets.hpp"
#include <algorithm>

namespace JoseonRPG {

BattleScene::BattleScene(Party& party, Yokai enemy, ArtifactInventory& artifacts, bool isBoss, std::function<void(bool won)> onBattleEnd)
    : m_party(party), m_enemy(enemy), m_artifacts(artifacts), m_isBoss(isBoss), m_onBattleEnd(std::move(onBattleEnd)) {
    m_battle = std::make_unique<Battle>(m_party, m_enemy, m_artifacts);
}

void BattleScene::onEnter() {
    AudioEngine::playBgm(m_isBoss ? BgmTrack::BossBattle : BgmTrack::WildBattle);

    Yokai* pYokai = m_battle->getActivePlayerYokai();
    if (pYokai) {
        m_playerHpBar.setTarget(pYokai->getStats().hp, pYokai->getStats().maxHp, true);
        m_playerQiBar.setTarget(pYokai->getStats().qi, pYokai->getStats().maxQi, true);
    }
    m_enemyHpBar.setTarget(m_battle->getEnemyYokai().getStats().hp, m_battle->getEnemyYokai().getStats().maxHp, true);

    // Determine enemy sprite ID
    const auto& e = m_battle->getEnemyYokai();
    if (e.getId() == "YOKAI_002") m_enemySpriteId = 2; // Gumiho
    else if (e.getId() == "YOKAI_005") m_enemySpriteId = 3; // Maiden Ghost
    else if (e.getId() == "YOKAI_003") m_enemySpriteId = 4; // Bulgasari
    else if (e.getId() == "YOKAI_007") m_enemySpriteId = 5; // Jeoseungsaja
    else if (e.getId() == "YOKAI_031") m_enemySpriteId = 6; // Mountain Tiger
    else if (e.getId() == "YOKAI_020") m_enemySpriteId = 7; // Imoogi
    else if (e.getId() == "YOKAI_022") m_enemySpriteId = 8; // Centipede Demon
    else if (e.getId() == "YOKAI_048") m_enemySpriteId = 9; // Tree God
    else if (e.getId() == "YOKAI_BOSS_01") m_enemySpriteId = 1;
    else if (e.getId() == "YOKAI_BOSS_02") m_enemySpriteId = 4;
    else if (e.getId() == "YOKAI_BOSS_03") m_enemySpriteId = 7;
    else if (e.getId() == "YOKAI_BOSS_04") m_enemySpriteId = 2;
    else if (e.getId() == "YOKAI_BOSS_05") m_enemySpriteId = 11;
    else {
        if (e.getElement() == Element::Fire) m_enemySpriteId = 1;
        else if (e.getElement() == Element::Water) m_enemySpriteId = 3;
        else if (e.getElement() == Element::Earth) m_enemySpriteId = 4;
        else if (e.getElement() == Element::Dark) m_enemySpriteId = 5;
        else if (e.getElement() == Element::Light) m_enemySpriteId = 2;
        else m_enemySpriteId = 6;
    }

    // Intro message in sequencer
    std::string introMsg = m_isBoss ? ("음양당 보스 [" + e.getName() + "] 출현!") : ("야생의 [" + e.getName() + "] 조우!");
    m_sequencer.addTextMessage(introMsg);
}

void BattleScene::spawnCaptureBurst(int cx, int cy, bool isGold) {
    m_captureParticles.clear();
    for (int i = 0; i < 24; ++i) {
        CaptureParticle p;
        p.x = static_cast<float>(cx);
        p.y = static_cast<float>(cy);
        float angle = (i / 24.0f) * 6.283185f;
        float spd = 25.0f + static_cast<float>(rand() % 45);
        p.vx = std::cos(angle) * spd;
        p.vy = std::sin(angle) * spd;
        p.life = 0.0f;
        p.maxLife = 0.35f + static_cast<float>(rand() % 20) * 0.01f;
        if (isGold) {
            p.color = (i % 2 == 0) ? Color(240, 200, 40) : Color(255, 240, 150);
        } else {
            p.color = (i % 2 == 0) ? Color(220, 60, 40) : Color(80, 80, 80);
        }
        m_captureParticles.push_back(p);
    }
}

void BattleScene::updateCaptureAnimation(float dt) {
    for (auto& p : m_captureParticles) {
        p.life += dt;
        p.x += p.vx * dt;
        p.y += p.vy * dt;
    }
    m_captureParticles.erase(
        std::remove_if(m_captureParticles.begin(), m_captureParticles.end(), [](const CaptureParticle& p) {
            return p.life >= p.maxLife;
        }),
        m_captureParticles.end()
    );

    if (m_captureAnimState == CaptureAnimState::None) return;

    if (m_captureAnimState == CaptureAnimState::Throwing) {
        m_captureAnimTimer += dt;
        float progress = m_captureAnimTimer / 0.45f;
        if (progress >= 1.0f) {
            m_captureAnimState = CaptureAnimState::Shaking;
            m_captureAnimTimer = 0.0f;
            m_currentShakeCount = 0;
            m_talismanX = 228.0f;
            m_talismanY = 16.0f;
        } else {
            m_talismanX = 40.0f + progress * (228.0f - 40.0f);
            float arc = std::sin(progress * 3.14159f) * 40.0f;
            m_talismanY = (65.0f + progress * (16.0f - 65.0f)) - arc;
        }
    } else if (m_captureAnimState == CaptureAnimState::Shaking) {
        m_captureAnimTimer += dt;
        float shakeInterval = 0.65f;
        if (m_captureAnimTimer >= shakeInterval) {
            m_captureAnimTimer = 0.0f;
            if (m_currentShakeCount < m_targetShakeCount) {
                m_currentShakeCount++;
                AudioEngine::playSfx(SfxId::CapturePulse);
                m_enemyShake = 5.0f + static_cast<float>(m_currentShakeCount) * 2.0f;
                m_sequencer.addTextMessage("벽사 부적 " + std::to_string(m_currentShakeCount) + "차 영기 결속... (두근!)");
            } else {
                if (m_captureWillSucceed) {
                    m_captureAnimState = CaptureAnimState::SuccessBurst;
                    m_captureAnimTimer = 0.0f;
                    AudioEngine::playSfx(SfxId::CaptureSuccess);
                    spawnCaptureBurst(232, 28, true);
                    m_sequencer.addTextMessage("★ 계약 성공! [" + m_battle->getEnemyYokai().getName() + "]과 영혼의 계약을 맺었습니다! ★");
                } else {
                    m_captureAnimState = CaptureAnimState::Breakout;
                    m_captureAnimTimer = 0.0f;
                    AudioEngine::playSfx(SfxId::CaptureBreak);
                    spawnCaptureBurst(232, 28, false);
                    m_enemyShake = 12.0f;
                    m_sequencer.addTextMessage("부적이 찢어지며 [" + m_battle->getEnemyYokai().getName() + "]이(가) 결계를 깨고 탈출했습니다!");
                }
            }
        }
    } else if (m_captureAnimState == CaptureAnimState::SuccessBurst) {
        m_captureAnimTimer += dt;
        if (m_captureAnimTimer >= 0.8f) {
            m_captureAnimState = CaptureAnimState::None;
        }
    } else if (m_captureAnimState == CaptureAnimState::Breakout) {
        m_captureAnimTimer += dt;
        if (m_captureAnimTimer >= 0.6f) {
            m_captureAnimState = CaptureAnimState::None;
        }
    }
}

void BattleScene::renderTalismanCapture(Renderer& renderer) {
    if (m_captureAnimState == CaptureAnimState::Throwing || m_captureAnimState == CaptureAnimState::Shaking) {
        int tx = static_cast<int>(m_talismanX);
        int ty = static_cast<int>(m_talismanY);

        bool glow = (m_captureAnimState == CaptureAnimState::Shaking && (static_cast<int>(m_battleAnimTimer * 10.0f) % 2 == 0));
        Color bgCol = glow ? Color(255, 240, 100) : Color(240, 195, 45);
        renderer.fillRect(tx, ty, 8, 12, bgCol);
        renderer.drawRect(tx, ty, 8, 12, Color(8, 24, 32));

        renderer.setPixel(tx + 3, ty + 2, Color(200, 30, 30));
        renderer.setPixel(tx + 4, ty + 2, Color(200, 30, 30));
        renderer.drawLine(tx + 2, ty + 4, tx + 5, ty + 4, Color(200, 30, 30));
        renderer.drawLine(tx + 3, ty + 5, tx + 3, ty + 9, Color(200, 30, 30));
        renderer.drawLine(tx + 2, ty + 7, tx + 5, ty + 7, Color(200, 30, 30));
        renderer.setPixel(tx + 3, ty + 10, Color(200, 30, 30));
        renderer.setPixel(tx + 4, ty + 10, Color(200, 30, 30));
    }

    for (const auto& p : m_captureParticles) {
        renderer.setPixel(static_cast<int>(p.x), static_cast<int>(p.y), p.color);
    }
}

void BattleScene::handleInput() {
    if (m_captureAnimState != CaptureAnimState::None) {
        return;
    }

    if (!m_sequencer.isFinished()) {
        if (Input::isPressed(Key::ActionA) || Input::isPressed(Key::ActionB)) {
            m_sequencer.advanceText();
        }
        return;
    }

    if (m_battle->getState() == BattleState::PlayerCommand) {
        if (Input::isPressed(Key::Up)) {
            m_battle->onNavigateUp();
            AudioEngine::playSfx(SfxId::MenuCursor);
        }
        if (Input::isPressed(Key::Down)) {
            m_battle->onNavigateDown();
            AudioEngine::playSfx(SfxId::MenuCursor);
        }
        if (Input::isPressed(Key::Left)) {
            m_battle->onNavigateLeft();
            AudioEngine::playSfx(SfxId::MenuCursor);
        }
        if (Input::isPressed(Key::Right)) {
            m_battle->onNavigateRight();
            AudioEngine::playSfx(SfxId::MenuCursor);
        }

        if (Input::isPressed(Key::ActionA)) {
            AudioEngine::playSfx(SfxId::MenuSelect);
            bool wasCapture = (m_battle->getMenuState() == BattleMenuState::MainAction && m_battle->getMainCursor() == 1);

            m_battle->onConfirm();
            
            if (wasCapture) {
                auto capRes = m_battle->getLastCaptureResult();
                if (capRes.attempted) {
                    m_captureAnimState = CaptureAnimState::Throwing;
                    m_captureAnimTimer = 0.0f;
                    m_currentShakeCount = 0;
                    m_targetShakeCount = capRes.targetShakes;
                    m_captureWillSucceed = capRes.success;
                    m_talismanX = 40.0f;
                    m_talismanY = 65.0f;
                    AudioEngine::playSfx(SfxId::CaptureThrow);
                    m_sequencer.addTextMessage("벽사 봉인 부적을 투척했습니다!");
                }
            } else if (m_battle->getState() == BattleState::ExecutingTurn) {
                m_playerLunge = 8.0f;
                m_enemyLunge = 8.0f;
                Yokai* py = m_battle->getActivePlayerYokai();
                if (py) {
                    m_skillFx.triggerSkillFx(py->getElement(), SCREEN_WIDTH - 65, 45);
                }
                m_skillFx.triggerSkillFx(m_battle->getEnemyYokai().getElement(), 50, 85);

                const auto& log = m_battle->getCombatLog();
                for (const auto& line : log) {
                    m_sequencer.addTextMessage(line);
                }
                m_sequencer.addFlash(false, 2);
                m_sequencer.addFlash(true, 2);
            }
        }
        if (Input::isPressed(Key::ActionB)) {
            AudioEngine::playSfx(SfxId::MenuCancel);
            m_battle->onCancel();
        }
    } else if (m_battle->getState() == BattleState::Victory) {
        if (Input::isPressed(Key::ActionA) || Input::isPressed(Key::ActionB)) {
            if (m_onBattleEnd) m_onBattleEnd(true);
            if (m_sceneStack) m_sceneStack->popScene();
        }
    } else if (m_battle->getState() == BattleState::Defeat) {
        if (Input::isPressed(Key::ActionA) || Input::isPressed(Key::ActionB)) {
            if (m_onBattleEnd) m_onBattleEnd(false);
            if (m_sceneStack) m_sceneStack->popScene();
        }
    }
}

void BattleScene::update(float dt) {
    bool isFast = Input::isDown(Key::Dash) || Input::isDown(Key::ActionA);
    float simDt = isFast ? dt * 2.2f : dt;

    if (m_introSlideTimer > 0.0f) {
        m_introSlideTimer = std::max(0.0f, m_introSlideTimer - simDt);
    }

    if (isFast && m_sequencer.isCurrentCommandTextMessage() && m_captureAnimState == CaptureAnimState::None) {
        m_sequencer.advanceText();
    }

    m_battleAnimTimer += simDt;
    updateCaptureAnimation(simDt);
    m_sequencer.update(simDt);
    m_skillFx.update(simDt);

    if (m_playerLunge > 0.0f) m_playerLunge = std::max(0.0f, m_playerLunge - simDt * 25.0f);
    if (m_enemyLunge > 0.0f) m_enemyLunge = std::max(0.0f, m_enemyLunge - simDt * 25.0f);

    if (m_sequencer.isPlayerFlashing()) m_playerShake = 3.5f;
    if (m_sequencer.isEnemyFlashing()) m_enemyShake = 3.5f;

    if (m_playerShake > 0.0f) m_playerShake = std::max(0.0f, m_playerShake - simDt * 15.0f);
    if (m_enemyShake > 0.0f) m_enemyShake = std::max(0.0f, m_enemyShake - simDt * 15.0f);

    Yokai* pYokai = m_battle->getActivePlayerYokai();
    if (pYokai) {
        m_playerHpBar.setTarget(pYokai->getStats().hp, pYokai->getStats().maxHp);
        m_playerQiBar.setTarget(pYokai->getStats().qi, pYokai->getStats().maxQi);
    }
    m_enemyHpBar.setTarget(m_battle->getEnemyYokai().getStats().hp, m_battle->getEnemyYokai().getStats().maxHp);

    m_playerHpBar.update(simDt);
    m_playerQiBar.update(simDt);
    m_enemyHpBar.update(simDt);

    m_battle->update();
}

void BattleScene::render(Renderer& renderer) {
    // Top Half: Oriental Sky & Mountains in GB DMG 4-Shade
    renderer.fillRect(0, 0, SCREEN_WIDTH, 112, Color(224, 248, 208)); // Pale Off-White DMG ground
    renderer.fillRect(0, 0, SCREEN_WIDTH, 50, Color(136, 192, 112));   // Light Olive sky

    // Ground platform line
    renderer.drawLine(0, 112, SCREEN_WIDTH, 112, Color(8, 24, 32));

    // Fast-Forward HUD Badge
    if (Input::isDown(Key::Dash) || Input::isDown(Key::ActionA)) {
        renderer.fillRect(2, 2, 70, 10, Color(8, 24, 32, 200));
        FontRenderer::drawText(renderer, 4, 3, ">> 2.2x 배속", Color(224, 248, 208));
    }

    // Elemental Skill Particle FX Overlay
    m_skillFx.render(renderer);

    const Yokai* pYokai = m_battle->getActivePlayerYokai();
    const Yokai& eYokai = m_battle->getEnemyYokai();

    int slideOffset = (m_introSlideTimer > 0.0f) ? static_cast<int>((m_introSlideTimer / 0.5f) * 120.0f) : 0;

    // 1. Enemy HUD Box (Top-Left: X=8, Y=8, W=138, H=44)
    renderer.draw9SliceBox(8 - slideOffset, 8, 138, 44, UITheme::Paper);

    std::string eGradeStr = " [G." + std::to_string(static_cast<int>(eYokai.getGrade())) + "]";
    FontRenderer::drawText(renderer, 14 - slideOffset, 12, eYokai.getName() + " Lv." + std::to_string(eYokai.getLevel()), Color(8, 24, 32));
    FontRenderer::drawText(renderer, 106 - slideOffset, 12, eGradeStr, Color(52, 104, 86));

    std::string eHpText = "HP " + std::to_string(m_enemyHpBar.getCurrentValue()) + "/" + std::to_string(eYokai.getStats().maxHp);
    FontRenderer::drawText(renderer, 14 - slideOffset, 24, eHpText, Color(8, 24, 32));
    renderer.drawGaugeBar(14 - slideOffset, 36, 126, 6, m_enemyHpBar.getCurrentValue(), eYokai.getStats().maxHp, Color(52, 104, 86));

    if (eYokai.getStatus().effect != StatusEffect::None) {
        std::string statStr = "[" + std::string(StatusEffectSystem::getStatusName(eYokai.getStatus().effect)) + "]";
        FontRenderer::drawText(renderer, 90 - slideOffset, 24, statStr, Color(180, 40, 40));
    }

    // Dynamic Idle Breathing & Float + Lunge / Shake
    int eBounceY = static_cast<int>(std::sin(m_battleAnimTimer * 3.5f) * 1.5f);
    int pBounceY = static_cast<int>(std::cos(m_battleAnimTimer * 3.5f) * 1.5f);

    int pShakeX = (m_playerShake > 0.05f) ? static_cast<int>(std::sin(m_battleAnimTimer * 50.0f) * m_playerShake) : 0;
    int eShakeX = (m_enemyShake > 0.05f) ? static_cast<int>(std::sin(m_battleAnimTimer * 50.0f) * m_enemyShake) : 0;

    int pLungeX = static_cast<int>(m_playerLunge);
    int eLungeX = -static_cast<int>(m_enemyLunge);

    // Enemy Sprite (Top-Right: X=216, Y=14) - Gen 1 32x32 / 48x48 Battler
    int eBaseX = 216 + eShakeX + eLungeX + slideOffset;
    int eBaseY = 14 + eBounceY;

    if (!m_sequencer.isEnemyFlashing()) {
        if (eYokai.getId() == "YOKAI_002") {
            renderer.drawGen1Bitmap(eBaseX - 8, eBaseY - 8, 48, 48, Gen1Assets::GUMIHO_48x48, true);
        } else {
            int yNum = 1;
            if (eYokai.getId().rfind("YOKAI_", 0) == 0) {
                try {
                    yNum = std::stoi(eYokai.getId().substr(6));
                } catch (...) {
                    yNum = 1;
                }
            }
            int clampedNum = std::clamp(yNum, 1, 108);
            renderer.drawGen1Bitmap(eBaseX, eBaseY, 32, 32, Gen1Assets::YOKAI_BATTLERS_32x32[clampedNum - 1], true);
        }
    }

    // 2. Player Combatant (Bottom-Left: X=32, Y=56) & HUD Box (Bottom-Right: X=174, Y=58, W=138, H=48)
    int pBaseX = 32 + pShakeX + pLungeX - slideOffset;
    int pBaseY = 56 + pBounceY;

    if (pYokai) {
        if (!m_sequencer.isPlayerFlashing()) {
            if (pYokai->getId() == "YOKAI_002") {
                renderer.drawGen1Bitmap(pBaseX, pBaseY, 48, 48, Gen1Assets::GUMIHO_48x48, true, true);
            } else {
                int yNum = 1;
                if (pYokai->getId().rfind("YOKAI_", 0) == 0) {
                    try {
                        yNum = std::stoi(pYokai->getId().substr(6));
                    } catch (...) {
                        yNum = 1;
                    }
                }
                int clampedNum = std::clamp(yNum, 1, 108);
                renderer.drawGen1Bitmap(pBaseX, pBaseY, 32, 32, Gen1Assets::YOKAI_BATTLERS_32x32[clampedNum - 1], true, true);
            }
        }

        // Player HUD Box (Bottom-Right: X=174, Y=58, W=138, H=48)
        renderer.draw9SliceBox(174 + slideOffset, 58, 138, 48, UITheme::Paper);

        FontRenderer::drawText(renderer, 180 + slideOffset, 62, pYokai->getName() + " Lv." + std::to_string(pYokai->getLevel()), Color(8, 24, 32));

        std::string pHpText = "HP " + std::to_string(m_playerHpBar.getCurrentValue()) + "/" + std::to_string(pYokai->getStats().maxHp);
        FontRenderer::drawText(renderer, 180 + slideOffset, 74, pHpText, Color(8, 24, 32));
        renderer.drawGaugeBar(180 + slideOffset, 84, 126, 5, m_playerHpBar.getCurrentValue(), pYokai->getStats().maxHp, Color(52, 104, 86));

        std::string pQiText = "Qi " + std::to_string(m_playerQiBar.getCurrentValue()) + "/" + std::to_string(pYokai->getStats().maxQi);
        FontRenderer::drawText(renderer, 180 + slideOffset, 92, pQiText, Color(52, 104, 86));
        renderer.drawGaugeBar(240 + slideOffset, 94, 66, 4, m_playerQiBar.getCurrentValue(), pYokai->getStats().maxQi, Color(136, 192, 112));
    } else {
        if (!m_sequencer.isPlayerFlashing()) {
            renderer.drawGen1Bitmap(pBaseX, pBaseY, 16, 24, Gen1Assets::PLAYER_16x24[2], true);
        }

        renderer.draw9SliceBox(174 + slideOffset, 58, 138, 48, UITheme::Paper);

        FontRenderer::drawText(renderer, 180 + slideOffset, 62, "영술사 (혈혈단신)", Color(8, 24, 32));
        FontRenderer::drawText(renderer, 180 + slideOffset, 76, "벽사의 부적 소지", Color(52, 104, 86));
        FontRenderer::drawText(renderer, 180 + slideOffset, 90, "[2.계약] 가능", Color(8, 24, 32));
    }

    // Talisman & Capture Particles Overlay
    renderTalismanCapture(renderer);

    // 3. Command & Log Region (Bottom: Y=112, H=64)
    // If Sequencer has active text message, display it cleanly in full width!
    if (!m_sequencer.isFinished() && m_sequencer.isCurrentCommandTextMessage()) {
        renderer.draw9SliceBox(6, 112, 308, 64, UITheme::Paper);
        FontRenderer::drawText(renderer, 16, 126, m_sequencer.getCurrentText(), Color(8, 24, 32));
        if (m_sequencer.isWaitingForInput()) {
            FontRenderer::drawText(renderer, 296, 156, "▼", Color(8, 24, 32));
        }
        return;
    }

    if (m_battle->getState() == BattleState::PlayerCommand) {
        if (m_battle->getMenuState() == BattleMenuState::MainAction) {
            // Left Command Panel
            renderer.draw9SliceBox(6, 112, 134, 64, UITheme::Paper);

            const char* menuLabels[4] = {"1.기술", "2.계약", "3.교체", "4.도망"};
            int cur = m_battle->getMainCursor();

            for (int i = 0; i < 4; ++i) {
                int mx = (i % 2 == 0) ? 12 : 72;
                int my = (i < 2) ? 120 : 138;
                bool isCur = (cur == i);
                if (isCur) {
                    renderer.fillRect(mx - 2, my - 1, 56, 14, Color(136, 192, 112));
                    FontRenderer::drawText(renderer, mx, my, menuLabels[i], Color(8, 24, 32));
                } else {
                    FontRenderer::drawText(renderer, mx, my, menuLabels[i], Color(52, 104, 86));
                }
            }

            float capRate = m_battle->calculateCaptureProbability();
            std::string capStr = "계약률: " + std::to_string(static_cast<int>(capRate * 100)) + "%";
            FontRenderer::drawText(renderer, 14, 156, capStr, Color(8, 24, 32));

            // Right Combat Log Panel
            renderer.draw9SliceBox(144, 112, 170, 64, UITheme::Paper);
            const auto& log = m_battle->getCombatLog();
            int logY = 118;
            int startIdx = std::max(0, static_cast<int>(log.size()) - 3);
            for (size_t i = startIdx; i < log.size(); ++i) {
                FontRenderer::drawText(renderer, 150, logY, log[i], Color(8, 24, 32));
                logY += 13;
            }
        }
        else if (m_battle->getMenuState() == BattleMenuState::SkillSelect) {
            // Left Skill Select Panel
            renderer.draw9SliceBox(6, 112, 148, 64, UITheme::Paper);

            if (pYokai) {
                const auto& skills = pYokai->getSkills();
                int sCur = m_battle->getSkillCursor();

                for (size_t i = 0; i < 4 && i < skills.size(); ++i) {
                    int sx = (i % 2 == 0) ? 12 : 80;
                    int sy = (i < 2) ? 120 : 142;
                    bool isCur = (sCur == static_cast<int>(i));
                    if (isCur) {
                        renderer.fillRect(sx - 2, sy - 1, 64, 18, Color(136, 192, 112));
                        FontRenderer::drawText(renderer, sx, sy, skills[i].name, Color(8, 24, 32));
                        FontRenderer::drawText(renderer, sx, sy + 10, "Qi:" + std::to_string(skills[i].qiCost), Color(8, 24, 32));
                    } else {
                        FontRenderer::drawText(renderer, sx, sy, skills[i].name, Color(52, 104, 86));
                        FontRenderer::drawText(renderer, sx, sy + 10, "Qi:" + std::to_string(skills[i].qiCost), Color(52, 104, 86));
                    }
                }

                // Right Skill Detail Panel
                renderer.draw9SliceBox(158, 112, 156, 64, UITheme::Paper);
                if (sCur < static_cast<int>(skills.size())) {
                    const auto& curSkl = skills[sCur];
                    FontRenderer::drawText(renderer, 164, 118, "위력(Pwr): " + std::to_string(curSkl.power), Color(8, 24, 32));
                    FontRenderer::drawText(renderer, 164, 130, "명중(Acc): " + std::to_string(curSkl.accuracy) + "%", Color(8, 24, 32));
                    if (curSkl.statusEffect != StatusEffect::None) {
                        FontRenderer::drawText(renderer, 164, 142, StatusEffectSystem::getStatusName(curSkl.statusEffect), Color(180, 40, 40));
                    } else {
                        FontRenderer::drawText(renderer, 164, 142, "상태이상: 없음", Color(52, 104, 86));
                    }
                    FontRenderer::drawText(renderer, 164, 156, "[X: 뒤로가기]", Color(52, 104, 86));
                }
            }
        }
        else if (m_battle->getMenuState() == BattleMenuState::PartySwapSelect) {
            renderer.draw9SliceBox(6, 112, 308, 64, UITheme::Paper);
            FontRenderer::drawText(renderer, 14, 118, "=== 출전할 요괴 선택 (X: 취소) ===", Color(52, 104, 86));

            int swapCur = m_battle->getSwapCursor();
            for (size_t i = 0; i < m_party.getSize(); ++i) {
                const Yokai* member = m_party.getYokai(i);
                if (!member) continue;
                int my = 132 + static_cast<int>(i) * 14;
                bool isCur = (swapCur == static_cast<int>(i));
                if (isCur) {
                    renderer.fillRect(10, my - 1, 300, 13, Color(136, 192, 112));
                    FontRenderer::drawText(renderer, 12, my, "▶", Color(8, 24, 32));
                }
                std::string slotInfo = std::to_string(i + 1) + ". " + member->getName() + " Lv." + std::to_string(member->getLevel()) +
                                       " (HP:" + std::to_string(member->getStats().hp) + "/" + std::to_string(member->getStats().maxHp) + ")";
                FontRenderer::drawText(renderer, 26, my, slotInfo, Color(8, 24, 32));
            }
        }
    }
    else if (m_battle->getState() == BattleState::Victory) {
        renderer.draw9SliceBox(20, 112, 280, 64, UITheme::Paper);
        if (m_isBoss) {
            FontRenderer::drawText(renderer, 70, 120, "★ 음양당 보스 격파 성공! ★", Color(8, 24, 32));
            FontRenderer::drawText(renderer, 36, 136, "경험치 " + std::to_string(m_battle->getExpReward()) + " & 300냥 획득 및 퀘스트 완료!", Color(52, 104, 86));
        } else {
            FontRenderer::drawText(renderer, 90, 120, "★ 전투 승리! ★", Color(8, 24, 32));
            FontRenderer::drawText(renderer, 48, 136, "경험치 " + std::to_string(m_battle->getExpReward()) + " 획득 및 도감 등록!", Color(52, 104, 86));
        }
        FontRenderer::drawText(renderer, 70, 154, "[Z / Space 키를 눌러 필드로 복귀]", Color(8, 24, 32));
    }
    else if (m_battle->getState() == BattleState::Defeat) {
        renderer.draw9SliceBox(20, 112, 280, 64, UITheme::Inverted);
        FontRenderer::drawText(renderer, 90, 120, "☠ 파티 전멸 ☠", Color(224, 248, 208));
        FontRenderer::drawText(renderer, 60, 136, "모든 요괴가 기절했습니다...", Color(136, 192, 112));
        FontRenderer::drawText(renderer, 64, 154, "[Z / Space 키를 눌러 주막으로 복귀]", Color(224, 248, 208));
    }
}

} // namespace JoseonRPG
