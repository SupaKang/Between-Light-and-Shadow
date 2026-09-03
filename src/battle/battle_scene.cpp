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

void BattleScene::handleInput() {
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
            m_battle->onConfirm();
            
            // Check if turn action was submitted
            if (m_battle->getState() == BattleState::ExecutingTurn) {
                m_playerLunge = 8.0f;
                m_enemyLunge = 8.0f;
                Yokai* py = m_battle->getActivePlayerYokai();
                if (py) {
                    m_skillFx.triggerSkillFx(py->getElement(), SCREEN_WIDTH - 65, 45);
                }
                m_skillFx.triggerSkillFx(m_battle->getEnemyYokai().getElement(), 50, 85);

                // Read latest combat log from battle and push to sequencer
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

    if (isFast && m_sequencer.isCurrentCommandTextMessage()) {
        m_sequencer.advanceText();
    }

    m_battleAnimTimer += simDt;
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

    // 1. Enemy HUD Box (Top-Left: X=10, Y=8, W=135, H=42)
    renderer.fillRect(10, 8, 135, 42, Color(224, 248, 208));
    renderer.drawRect(10, 8, 135, 42, Color(8, 24, 32));
    renderer.drawRect(12, 10, 131, 38, Color(52, 104, 86));

    std::string eGradeStr = " [G." + std::to_string(static_cast<int>(eYokai.getGrade())) + "]";
    FontRenderer::drawText(renderer, 16, 12, eYokai.getName() + " Lv." + std::to_string(eYokai.getLevel()), Color(8, 24, 32));
    FontRenderer::drawText(renderer, 106, 12, eGradeStr, Color(52, 104, 86));

    std::string eHpText = "HP " + std::to_string(m_enemyHpBar.getCurrentValue()) + "/" + std::to_string(eYokai.getStats().maxHp);
    FontRenderer::drawText(renderer, 16, 22, eHpText, Color(8, 24, 32));
    m_enemyHpBar.render(renderer, 16, 32, 120, 5, Color(52, 104, 86));

    if (eYokai.getStatus().effect != StatusEffect::None) {
        std::string statStr = std::string(StatusEffectSystem::getStatusName(eYokai.getStatus().effect));
        FontRenderer::drawText(renderer, 16, 38, statStr, Color(8, 24, 32));
    }

    // Dynamic Idle Breathing & Float + Lunge / Shake
    int eBounceY = static_cast<int>(std::sin(m_battleAnimTimer * 3.5f) * 1.5f);
    int pBounceY = static_cast<int>(std::cos(m_battleAnimTimer * 3.5f) * 1.5f);

    int pShakeX = (m_playerShake > 0.05f) ? static_cast<int>(std::sin(m_battleAnimTimer * 50.0f) * m_playerShake) : 0;
    int eShakeX = (m_enemyShake > 0.05f) ? static_cast<int>(std::sin(m_battleAnimTimer * 50.0f) * m_enemyShake) : 0;

    int pLungeX = static_cast<int>(m_playerLunge);
    int eLungeX = -static_cast<int>(m_enemyLunge);

    // Enemy Sprite (Top-Right: X=220, Y=12) - Gen 1 32x32 / 48x48 Battler
    int eBaseX = 220 + eShakeX + eLungeX;
    int eBaseY = 12 + eBounceY;

    if (!m_sequencer.isEnemyFlashing()) {
        if (eYokai.getId() == "YOKAI_001") {
            renderer.drawGen1Bitmap(eBaseX, eBaseY, 32, 32, Gen1Assets::DOKKAEBI_32x32, true);
        } else if (eYokai.getId() == "YOKAI_002") {
            renderer.drawGen1Bitmap(eBaseX - 8, eBaseY - 8, 48, 48, Gen1Assets::GUMIHO_48x48, true);
        } else {
            renderer.drawSprite(eBaseX, eBaseY, m_enemySpriteId, static_cast<int>(m_battleAnimTimer * 4.0f) % 2);
        }
    }

    // 2. Player Combatant (Bottom-Left: X=36, Y=56) & HUD Box (Bottom-Right: X=180, Y=56, W=132, H=46)
    int pBaseX = 36 + pShakeX + pLungeX;
    int pBaseY = 56 + pBounceY;

    if (pYokai) {
        int playerSprite = 0;
        if (pYokai->getId() == "YOKAI_001") {
            if (!m_sequencer.isPlayerFlashing()) renderer.drawGen1Bitmap(pBaseX, pBaseY, 32, 32, Gen1Assets::DOKKAEBI_32x32, true, true);
        } else if (pYokai->getId() == "YOKAI_002") {
            if (!m_sequencer.isPlayerFlashing()) renderer.drawGen1Bitmap(pBaseX, pBaseY, 48, 48, Gen1Assets::GUMIHO_48x48, true, true);
        } else {
            if (!m_sequencer.isPlayerFlashing()) renderer.drawSprite(pBaseX, pBaseY, playerSprite, static_cast<int>(m_battleAnimTimer * 4.0f) % 2);
        }

        // Player HUD Box (Bottom-Right: X=180, Y=56, W=132, H=46)
        renderer.fillRect(180, 56, 132, 46, Color(224, 248, 208));
        renderer.drawRect(180, 56, 132, 46, Color(8, 24, 32));
        renderer.drawRect(182, 58, 128, 42, Color(52, 104, 86));

        FontRenderer::drawText(renderer, 186, 60, pYokai->getName() + " Lv." + std::to_string(pYokai->getLevel()), Color(8, 24, 32));

        std::string pHpText = "HP " + std::to_string(m_playerHpBar.getCurrentValue()) + "/" + std::to_string(pYokai->getStats().maxHp);
        FontRenderer::drawText(renderer, 186, 72, pHpText, Color(8, 24, 32));
        m_playerHpBar.render(renderer, 186, 82, 118, 5, Color(52, 104, 86));

        std::string pQiText = "Qi " + std::to_string(m_playerQiBar.getCurrentValue()) + "/" + std::to_string(pYokai->getStats().maxQi);
        FontRenderer::drawText(renderer, 186, 90, pQiText, Color(52, 104, 86));
    } else {
        // Solo Exorcist Combatant (16x24 Back Sprite at X=36, Y=56)
        if (!m_sequencer.isPlayerFlashing()) {
            renderer.drawGen1Bitmap(pBaseX, pBaseY, 16, 24, Gen1Assets::PLAYER_16x24[2], true);
        }

        // Exorcist HUD Box (Bottom-Right: X=180, Y=56, W=132, H=46)
        renderer.fillRect(180, 56, 132, 46, Color(224, 248, 208));
        renderer.drawRect(180, 56, 132, 46, Color(8, 24, 32));
        renderer.drawRect(182, 58, 128, 42, Color(52, 104, 86));

        FontRenderer::drawText(renderer, 186, 60, "영술사 (혈혈단신)", Color(8, 24, 32));
        FontRenderer::drawText(renderer, 186, 74, "벽사의 부적 소지", Color(52, 104, 86));
        FontRenderer::drawText(renderer, 186, 88, "[2.계약] 가능", Color(8, 24, 32));
    }

    // 3. Command & Log Region (Bottom: Y=114, H=62)
    renderer.fillRect(0, 114, SCREEN_WIDTH, 66, Color(224, 248, 208));
    renderer.drawRect(0, 114, SCREEN_WIDTH, 66, Color(8, 24, 32));

    // If Sequencer has active text message, display it cleanly in full width!
    if (!m_sequencer.isFinished() && m_sequencer.isCurrentCommandTextMessage()) {
        renderer.fillRect(6, 118, 308, 56, Color(224, 248, 208));
        renderer.drawRect(6, 118, 308, 56, Color(8, 24, 32));
        FontRenderer::drawText(renderer, 14, 130, m_sequencer.getCurrentText(), Color(8, 24, 32));
        if (m_sequencer.isWaitingForInput()) {
            FontRenderer::drawText(renderer, 296, 156, ">", Color(8, 24, 32));
        }
        return;
    }

    if (m_battle->getState() == BattleState::PlayerCommand) {
        if (m_battle->getMenuState() == BattleMenuState::MainAction) {
            renderer.drawPanel(4, 114, 130, 62, Color(20, 22, 28), Palette::MidGray);

            const char* menuLabels[4] = {"1.기술 (Attack)", "2.계약 (Talisman)", "3.교체 (Party)", "4.도망 (Flee)"};
            int cur = m_battle->getMainCursor();

            for (int i = 0; i < 4; ++i) {
                int mx = (i % 2 == 0) ? 8 : 68;
                int my = (i < 2) ? 122 : 146;
                Color c = (cur == i) ? Palette::Yellow : Palette::White;
                if (cur == i) {
                    FontRenderer::drawText(renderer, mx - 4, my, ">", Palette::Yellow);
                }
                FontRenderer::drawText(renderer, mx, my, menuLabels[i], c);
            }

            // Combat Log Panel
            renderer.drawPanel(138, 114, 178, 62, Color(16, 18, 22), Palette::MidGray);
            const auto& log = m_battle->getCombatLog();
            int logY = 118;
            int startIdx = std::max(0, static_cast<int>(log.size()) - 4);
            for (size_t i = startIdx; i < log.size(); ++i) {
                FontRenderer::drawText(renderer, 142, logY, log[i], Palette::White);
                logY += 9;
            }

            float capRate = m_battle->calculateCaptureProbability();
            std::string capStr = "계약률: " + std::to_string(static_cast<int>(capRate * 100)) + "%";
            FontRenderer::drawText(renderer, 240, 162, capStr, Palette::Yellow);
        }
        else if (m_battle->getMenuState() == BattleMenuState::SkillSelect) {
            renderer.drawPanel(4, 114, 160, 62, Color(20, 22, 28), Palette::Yellow);

            if (pYokai) {
                const auto& skills = pYokai->getSkills();
                int sCur = m_battle->getSkillCursor();

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

                // Skill Detail Panel
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
        else if (m_battle->getMenuState() == BattleMenuState::PartySwapSelect) {
            renderer.drawPanel(4, 114, 312, 62, Color(20, 22, 28), Palette::Blue);
            FontRenderer::drawText(renderer, 10, 118, "=== 출전할 요괴를 선택하십시오 (X: 취소) ===", Palette::Yellow);

            int swapCur = m_battle->getSwapCursor();
            for (size_t i = 0; i < m_party.getSize(); ++i) {
                const Yokai* member = m_party.getYokai(i);
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
    else if (m_battle->getState() == BattleState::Victory) {
        renderer.drawPanel(30, 118, 260, 52, Color(16, 40, 24), Palette::Yellow);
        if (m_isBoss) {
            FontRenderer::drawText(renderer, 70, 126, "★ 음양당 보스 격파 성공! ★", Palette::Yellow);
            FontRenderer::drawText(renderer, 45, 140, "경험치 " + std::to_string(m_battle->getExpReward()) + " & 300냥 획득 및 퀘스트 완료!", Palette::White);
        } else {
            FontRenderer::drawText(renderer, 90, 126, "★ 전투 승리! ★", Palette::Yellow);
            FontRenderer::drawText(renderer, 50, 140, "경험치 " + std::to_string(m_battle->getExpReward()) + " 획득 및 도감 등록!", Palette::White);
        }
        FontRenderer::drawText(renderer, 70, 154, "[Z / Space 키를 눌러 필드로 복귀]", Palette::Jade);
    }
    else if (m_battle->getState() == BattleState::Defeat) {
        renderer.drawPanel(30, 118, 260, 52, Color(40, 16, 16), Palette::Red);
        FontRenderer::drawText(renderer, 90, 126, "☠ 파티 전멸 ☠", Palette::Red);
        FontRenderer::drawText(renderer, 60, 140, "모든 요괴가 기절했습니다...", Palette::White);
        FontRenderer::drawText(renderer, 70, 154, "[Z / Space 키를 눌러 주막으로 복귀]", Palette::Yellow);
    }
}

} // namespace JoseonRPG
