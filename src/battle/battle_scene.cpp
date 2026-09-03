#include "battle_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/scene_stack.hpp"
#include "../ui/font_renderer.hpp"
#include "../data/data_manager.hpp"
#include "../audio/audio_engine.hpp"
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
    m_sequencer.update(dt);
    m_skillFx.update(dt);

    Yokai* pYokai = m_battle->getActivePlayerYokai();
    if (pYokai) {
        m_playerHpBar.setTarget(pYokai->getStats().hp, pYokai->getStats().maxHp);
        m_playerQiBar.setTarget(pYokai->getStats().qi, pYokai->getStats().maxQi);
    }
    m_enemyHpBar.setTarget(m_battle->getEnemyYokai().getStats().hp, m_battle->getEnemyYokai().getStats().maxHp);

    m_playerHpBar.update(dt);
    m_playerQiBar.update(dt);
    m_enemyHpBar.update(dt);

    m_battle->update();
}

void BattleScene::render(Renderer& renderer) {
    // Top Half: Oriental Sky & Mountains
    renderer.fillRect(0, 0, SCREEN_WIDTH, 80, Color(14, 16, 24));

    // Ground: Traditional Tatami / Earth platform
    renderer.fillRect(0, 80, SCREEN_WIDTH, 30, Color(24, 30, 42));
    renderer.drawLine(0, 110, SCREEN_WIDTH, 110, Palette::MidGray);

    // Elemental Skill Particle FX Overlay
    m_skillFx.render(renderer);

    const Yokai* pYokai = m_battle->getActivePlayerYokai();
    const Yokai& eYokai = m_battle->getEnemyYokai();

    // 1. Enemy HUD Box (Top-Left)
    renderer.drawPanel(8, 4, 144, 46, Color(24, 26, 34, 230), m_isBoss ? Palette::Red : Palette::MidGray);
    std::string eGradeStr = " [G." + std::to_string(static_cast<int>(eYokai.getGrade())) + "]";
    FontRenderer::drawText(renderer, 14, 8, eYokai.getName() + " Lv." + std::to_string(eYokai.getLevel()), Palette::Red);
    FontRenderer::drawText(renderer, 112, 8, eGradeStr, Palette::Yellow);

    std::string eHpText = "HP " + std::to_string(m_enemyHpBar.getCurrentValue()) + "/" + std::to_string(eYokai.getStats().maxHp);
    FontRenderer::drawText(renderer, 14, 18, eHpText, Palette::White);
    m_enemyHpBar.render(renderer, 14, 27, 132, 5, Palette::Red);

    // Enemy Trait indicator
    if (eYokai.getTrait() != YokaiTrait::None) {
        FontRenderer::drawText(renderer, 14, 34, "[" + eYokai.getTraitName() + "]", Palette::Yellow);
    }

    if (eYokai.getStatus().effect != StatusEffect::None) {
        renderer.fillRect(80, 34, 60, 9, StatusEffectSystem::getStatusColor(eYokai.getStatus().effect));
        FontRenderer::drawText(renderer, 82, 35, StatusEffectSystem::getStatusName(eYokai.getStatus().effect), Palette::Black);
    }

    // Enemy Sprite with flashing support
    if (!m_sequencer.isEnemyFlashing()) {
        renderer.drawSprite(SCREEN_WIDTH - 65, 24, m_enemySpriteId, 0);
    }

    // 2. Player Yokai HUD Box
    if (pYokai) {
        if (!m_sequencer.isPlayerFlashing()) {
            renderer.drawSprite(35, 68, 0, 0);
        }

        renderer.drawPanel(165, 58, 147, 50, Color(24, 26, 34, 230), Palette::Blue);
        FontRenderer::drawText(renderer, 171, 62, pYokai->getName() + " Lv." + std::to_string(pYokai->getLevel()), Palette::Jade);

        std::string pHpText = "HP " + std::to_string(m_playerHpBar.getCurrentValue()) + "/" + std::to_string(pYokai->getStats().maxHp);
        FontRenderer::drawText(renderer, 171, 72, pHpText, Palette::White);
        m_playerHpBar.render(renderer, 171, 81, 135, 5, Palette::Green);

        std::string pQiText = "Qi " + std::to_string(m_playerQiBar.getCurrentValue()) + "/" + std::to_string(pYokai->getStats().maxQi);
        FontRenderer::drawText(renderer, 171, 88, pQiText, Palette::Jade);
        m_playerQiBar.render(renderer, 220, 89, 86, 4, Palette::Blue);

        // Player Trait indicator
        if (pYokai->getTrait() != YokaiTrait::None) {
            FontRenderer::drawText(renderer, 171, 96, "특성:" + pYokai->getTraitName(), Palette::Yellow);
        }
    }

    // 3. Command & Log Region
    renderer.drawPanel(0, 110, SCREEN_WIDTH, 70, Palette::Black, Palette::MidGray);

    // If Sequencer has active text message, display it cleanly in full width!
    if (!m_sequencer.isFinished() && m_sequencer.isCurrentCommandTextMessage()) {
        renderer.drawPanel(8, 114, 304, 62, Color(20, 24, 32), Palette::Yellow);
        FontRenderer::drawText(renderer, 16, 128, m_sequencer.getCurrentText(), Palette::White);
        if (m_sequencer.isWaitingForInput()) {
            FontRenderer::drawText(renderer, 290, 160, ">", Palette::Jade);
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
