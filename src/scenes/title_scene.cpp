#include "title_scene.hpp"
#include "art_inspector_scene.hpp"
#include "settings_scene.hpp"
#include "intro_scene.hpp"
#include "../core/renderer.hpp"
#include "../core/input.hpp"
#include "../core/scene_stack.hpp"
#include "../core/save_system.hpp"
#include "../ui/font_renderer.hpp"
#include "../data/data_manager.hpp"
#include "../world/world_scene.hpp"
#include "../audio/audio_engine.hpp"
#include "../core/gen1_assets.hpp"
#include <algorithm>
#include <cmath>
#include <windows.h>

namespace JoseonRPG {

TitleScene::TitleScene(Party& party, ArtifactInventory& artifacts, int& money)
    : m_party(party), m_artifacts(artifacts), m_money(money) {
    initWisps();
}

void TitleScene::initWisps() {
    m_wisps.clear();
    const Color wispColors[3] = {
        Color(136, 240, 190), // Ethereal Cyan-Green
        Color(255, 225, 90),  // Golden Amber
        Color(224, 248, 208)  // Luminous Moonlight Cream
    };

    for (int i = 0; i < 24; ++i) {
        SpiritWisp w;
        w.x = static_cast<float>(rand() % SCREEN_WIDTH);
        w.y = static_cast<float>(rand() % SCREEN_HEIGHT);
        w.speed = 12.0f + static_cast<float>(rand() % 24);
        w.phase = static_cast<float>(rand() % 100) * 0.1f;
        w.size = (i % 3 == 0) ? 2.0f : (i % 7 == 0 ? 3.0f : 1.0f);
        w.color = wispColors[i % 3];
        m_wisps.push_back(w);
    }
}

void TitleScene::updateWisps(float dt) {
    for (auto& w : m_wisps) {
        w.y -= w.speed * dt;
        w.x += std::sin(w.phase + m_animTimer * 1.6f) * 14.0f * dt;
        if (w.y < -8.0f) {
            w.y = SCREEN_HEIGHT + 6.0f;
            w.x = static_cast<float>(rand() % SCREEN_WIDTH);
        }
        if (w.x < -4.0f) w.x = SCREEN_WIDTH + 4.0f;
        if (w.x > SCREEN_WIDTH + 4.0f) w.x = -4.0f;
    }
}

void TitleScene::onEnter() {
    AudioEngine::playBgm(BgmTrack::Title);
    m_hasSave = SaveSystem::hasSaveFile(1);
    if (m_hasSave) {
        SaveSystem::getSaveSummary(1, m_saveSummary);
        m_cursor = 1; // Default to Continue
    } else {
        m_cursor = 0; // Default to New Game
    }
    m_menuActivated = false;
    m_menuSlide = 0.0f;
}

void TitleScene::startNewGame() {
    auto intro = std::make_unique<IntroScene>(m_party, m_artifacts, m_money);
    if (m_sceneStack) {
        m_sceneStack->clearAndSet(std::move(intro));
    }
}

void TitleScene::loadSavedGame() {
    if (!m_hasSave) return;

    GameRuntimeContext ctx;
    ctx.party = &m_party;
    ctx.artifacts = &m_artifacts;
    ctx.encyclopedia = &DataManager::getEncyclopedia();
    ctx.questManager = &DataManager::getQuestManager();

    if (SaveSystem::loadFromSlot(1, ctx)) {
        m_money = ctx.money;
        auto world = std::make_unique<WorldScene>(m_party, m_artifacts, m_money);
        world->setPlayerPosition(ctx.gridX, ctx.gridY, ctx.mapId);

        if (m_sceneStack) {
            m_sceneStack->clearAndSet(std::move(world));
        }
    }
}

void TitleScene::handleInput() {
    if (!m_menuActivated) {
        // Press Any Key / Z / Enter to open menu
        if (Input::isPressed(Key::ActionA) || Input::isPressed(Key::Menu) || Input::isPressed(Key::Dash)) {
            AudioEngine::playSfx(SfxId::MenuSelect);
            m_menuActivated = true;
        }
        return;
    }

    // Inside Menu Navigation (5 Choices: 0:New, 1:Continue, 2:Codex, 3:Settings, 4:Exit)
    if (Input::isRepeated(Key::Up)) {
        if (m_cursor > 0) {
            m_cursor--;
            AudioEngine::playSfx(SfxId::MenuCursor);
        }
    }
    if (Input::isRepeated(Key::Down)) {
        if (m_cursor < 4) {
            m_cursor++;
            AudioEngine::playSfx(SfxId::MenuCursor);
        }
    }

    if (Input::isPressed(Key::ActionB)) {
        AudioEngine::playSfx(SfxId::MenuCancel);
        m_menuActivated = false;
        return;
    }

    if (Input::isPressed(Key::ActionA)) {
        AudioEngine::playSfx(SfxId::MenuSelect);
        if (m_cursor == 0) {
            startNewGame();
        } else if (m_cursor == 1) {
            if (m_hasSave) {
                loadSavedGame();
            } else {
                startNewGame();
            }
        } else if (m_cursor == 2) {
            if (m_sceneStack) {
                m_sceneStack->pushScene(std::make_unique<ArtInspectorScene>());
            }
        } else if (m_cursor == 3) {
            if (m_sceneStack) {
                m_sceneStack->pushScene(std::make_unique<SettingsScene>());
            }
        } else if (m_cursor == 4) {
            PostQuitMessage(0);
        }
    }
}

void TitleScene::update(float dt) {
    m_animTimer += dt;
    updateWisps(dt);

    m_promptBlink += dt;
    if (m_promptBlink >= 0.70f) {
        m_promptBlink = 0.0f;
    }

    if (m_menuActivated && m_menuSlide < 1.0f) {
        m_menuSlide = std::min(1.0f, m_menuSlide + dt * 4.5f);
    } else if (!m_menuActivated && m_menuSlide > 0.0f) {
        m_menuSlide = std::max(0.0f, m_menuSlide - dt * 4.5f);
    }
}

void TitleScene::renderBackgroundLandscape(Renderer& renderer) {
    // 1. Twilight Sky Gradient (Y=0..115)
    for (int y = 0; y < 115; ++y) {
        float factor = static_cast<float>(y) / 115.0f;
        uint8_t r = static_cast<uint8_t>(10 + factor * 70);
        uint8_t g = static_cast<uint8_t>(20 + factor * 95);
        uint8_t b = static_cast<uint8_t>(28 + factor * 72);
        renderer.drawLine(0, y, SCREEN_WIDTH, y, Color(r, g, b));
    }

    // 2. Starfield Twinkles
    const int stars[14][2] = {
        {18, 12}, {45, 26}, {82, 16}, {124, 28}, {160, 14}, {198, 22}, {280, 18},
        {35, 42}, {90, 52}, {145, 40}, {210, 58}, {295, 34}, {70, 32}, {260, 48}
    };
    for (int i = 0; i < 14; ++i) {
        int sx = stars[i][0];
        int sy = stars[i][1];
        float twinkle = std::sin(m_animTimer * 2.5f + static_cast<float>(i * 2));
        Color sCol = (twinkle > 0.2f) ? Color(224, 248, 208) : Color(90, 130, 110);
        renderer.setPixel(sx, sy, sCol);
    }

    // 3. Ethereal Yin-Yang Full Moon (Center X=242, Y=36, R=20)
    int moonX = 242;
    int moonY = 36;
    int r = 20;

    // Soft outer glow halo
    for (int dy = -26; dy <= 26; ++dy) {
        for (int dx = -26; dx <= 26; ++dx) {
            int distSq = dx * dx + dy * dy;
            if (distSq <= 26 * 26 && distSq > r * r) {
                if ((dx + dy) % 2 == 0) {
                    renderer.setPixel(moonX + dx, moonY + dy, Color(160, 210, 180, 80));
                }
            }
        }
    }

    // Moon Solid Body
    for (int dy = -r; dy <= r; ++dy) {
        for (int dx = -r; dx <= r; ++dx) {
            if (dx * dx + dy * dy <= r * r) {
                // Yin-Yang Swirling Disc
                bool isYin = (dx < 0 && dy < 0) || (dx > 0 && dy > 4 && dx * dx + (dy - 8) * (dy - 8) <= 64);
                if (dx * dx + (dy + 8) * (dy + 8) <= 16) isYin = false;
                if (dx * dx + (dy - 8) * (dy - 8) <= 4) isYin = true;

                Color mCol = isYin ? Color(32, 54, 58) : Color(236, 246, 222);
                renderer.setPixel(moonX + dx, moonY + dy, mCol);
            }
        }
    }
    // Moon Border
    for (int a = 0; a < 360; a += 5) {
        float rad = a * 0.0174533f;
        int bx = moonX + static_cast<int>(std::cos(rad) * r);
        int by = moonY + static_cast<int>(std::sin(rad) * r);
        renderer.setPixel(bx, by, Color(8, 24, 32));
    }

    // 4. Far Mountain Silhouette (Y=65..110)
    for (int x = 0; x < SCREEN_WIDTH; ++x) {
        float fx = static_cast<float>(x);
        int my = static_cast<int>(75.0f + std::sin(fx * 0.022f) * 12.0f + std::cos(fx * 0.05f) * 6.0f);
        for (int y = my; y < 125; ++y) {
            renderer.setPixel(x, y, Color(60, 94, 78));
        }
    }

    // 5. Mid Mountain Peaks & Morning Mist (Y=88..140)
    for (int x = 0; x < SCREEN_WIDTH; ++x) {
        float fx = static_cast<float>(x);
        int my = static_cast<int>(92.0f + std::cos(fx * 0.035f + 1.2f) * 16.0f + std::sin(fx * 0.08f) * 5.0f);
        for (int y = my; y < 145; ++y) {
            renderer.setPixel(x, y, Color(34, 60, 52));
        }
    }

    // Horizontal Mist Streamers
    for (int my = 108; my <= 126; my += 6) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            float wave = std::sin(x * 0.04f + m_animTimer * 1.8f + my);
            if (wave > 0.3f && (x % 2 == 0)) {
                renderer.setPixel(x, my, Color(136, 192, 112));
            }
        }
    }

    // 6. Foreground Rocky Crag Cliff (Left X=0..96, Y=112..180)
    for (int x = 0; x < 100; ++x) {
        int cliffTop = 114 + (x / 3) + static_cast<int>(std::sin(x * 0.2f) * 3.0f);
        for (int y = cliffTop; y < SCREEN_HEIGHT; ++y) {
            renderer.setPixel(x, y, Color(8, 20, 24));
        }
    }

    // Foreground Base Valley (Y=145..180)
    for (int y = 145; y < SCREEN_HEIGHT; ++y) {
        for (int x = 95; x < SCREEN_WIDTH; ++x) {
            renderer.setPixel(x, y, Color(12, 28, 30));
        }
    }

    // 7. Mythical Silhouette on Cliff (Gumiho 48x48 Silhouette on Left Crag)
    int foxX = 26;
    int foxY = 92;
    int tailSway = static_cast<int>(std::sin(m_animTimer * 2.2f) * 2.0f);
    renderer.drawGen1Bitmap(foxX, foxY + tailSway, 48, 48, Gen1Assets::GUMIHO_48x48, true);

    // 8. Spirit Wisps / Dokkaebi Fire
    for (const auto& w : m_wisps) {
        int wx = static_cast<int>(w.x);
        int wy = static_cast<int>(w.y);
        int sz = static_cast<int>(w.size);

        renderer.fillRect(wx, wy, sz, sz, w.color);
        if (sz >= 2) {
            renderer.setPixel(wx - 1, wy, Color(w.color.r, w.color.g, w.color.b, 120));
            renderer.setPixel(wx + sz, wy, Color(w.color.r, w.color.g, w.color.b, 120));
            renderer.setPixel(wx, wy - 1, Color(w.color.r, w.color.g, w.color.b, 120));
            renderer.setPixel(wx, wy + sz, Color(w.color.r, w.color.g, w.color.b, 120));
        }
    }
}

void TitleScene::renderTitleLogo(Renderer& renderer) {
    int floatY = static_cast<int>(std::sin(m_animTimer * 1.8f) * 1.5f);
    int logoX = 18;
    int logoY = 12 + floatY;
    int logoW = 284;
    int logoH = 50;

    // Paper 9-Slice Frame
    renderer.draw9SliceBox(logoX, logoY, logoW, logoH, UITheme::Paper);

    // Gold Brass Corner Insets
    renderer.fillRect(logoX + 2, logoY + 2, 4, 4, Color(240, 200, 40));
    renderer.fillRect(logoX + logoW - 6, logoY + 2, 4, 4, Color(240, 200, 40));
    renderer.fillRect(logoX + 2, logoY + logoH - 6, 4, 4, Color(240, 200, 40));
    renderer.fillRect(logoX + logoW - 6, logoY + logoH - 6, 4, 4, Color(240, 200, 40));

    // Sweeping Golden Shimmer Gleam Line
    float gleamPhase = std::fmod(m_animTimer, 4.0f);
    if (gleamPhase < 1.0f) {
        int gleamX = logoX + static_cast<int>(gleamPhase * (logoW + 30)) - 15;
        for (int dy = 0; dy < logoH - 4; ++dy) {
            int gx = gleamX + (dy / 3);
            if (gx >= logoX + 2 && gx < logoX + logoW - 2) {
                renderer.setPixel(gx, logoY + 2 + dy, Color(255, 245, 180));
            }
        }
    }

    // Grand Main Title Typography
    FontRenderer::drawText(renderer, logoX + 16, logoY + 8, "108: 음양견문록", Color(8, 24, 32));

    // Red Royal Cinnabar Seal Stamp: [陰陽]
    int sealX = logoX + 234;
    int sealY = logoY + 7;
    renderer.fillRect(sealX, sealY, 34, 18, Color(190, 35, 35));
    renderer.drawRect(sealX, sealY, 34, 18, Color(8, 24, 32));
    FontRenderer::drawText(renderer, sealX + 4, sealY + 3, "陰 陽", Color(245, 230, 180));

    // Calligraphy Subtitle Ribbons
    FontRenderer::drawText(renderer, logoX + 16, logoY + 23, "Between Light and Shadow", Color(52, 104, 86));
    FontRenderer::drawText(renderer, logoX + 16, logoY + 35, "조선시대 민속 판타지 턴제 수집 RPG", Color(80, 130, 100));
}

void TitleScene::renderMenu(Renderer& renderer) {
    if (!m_menuActivated) {
        // Blinking Press Start Prompt
        int promptW = 216;
        int promptH = 20;
        int promptX = (SCREEN_WIDTH - promptW) / 2;
        int promptY = 142;

        renderer.draw9SliceBox(promptX, promptY, promptW, promptH, UITheme::Inverted);
        if (m_promptBlink < 0.45f) {
            FontRenderer::drawText(renderer, promptX + 12, promptY + 5, "◆ PRESS Z / ENTER TO START ◆", Color(224, 248, 208));
        }

        // Bottom Footer Info
        renderer.draw9SliceBox(4, 166, 312, 12, UITheme::Inverted);
        FontRenderer::drawText(renderer, 10, 168, "© 2026 Joseon Folklore RPG · 1.44MB Standalone C++17", Color(136, 192, 112));
    } else {
        // Sliding 9-Slice Menu Box (X=55, Y=66, W=210, H=100)
        int menuW = 210;
        int menuH = 98;
        int menuX = (SCREEN_WIDTH - menuW) / 2;
        int menuY = static_cast<int>(66.0f + (1.0f - m_menuSlide) * 15.0f);

        renderer.draw9SliceBox(menuX, menuY, menuW, menuH, UITheme::Paper);

        // Header Title
        FontRenderer::drawText(renderer, menuX + 64, menuY + 6, "【 여 정 선 택 】", Color(8, 24, 32));

        const char* menuOptions[5] = {
            "1. 새로운 여정 (New Game)",
            "2. 이어 하기 (Continue)",
            "3. 요괴 화첩 (Art Codex)",
            "4. 환경 설정 (Settings)",
            "5. 유람 종료 (Exit)"
        };

        for (int i = 0; i < 5; ++i) {
            int oy = menuY + 20 + i * 15;
            bool isSel = (m_cursor == i);

            if (isSel) {
                renderer.fillRect(menuX + 6, oy - 1, menuW - 12, 13, Color(136, 192, 112));
                FontRenderer::drawText(renderer, menuX + 8, oy, "▶", Color(8, 24, 32));
                FontRenderer::drawText(renderer, menuX + 18, oy, menuOptions[i], Color(8, 24, 32));
            } else {
                Color c = (i == 1 && !m_hasSave) ? Color(136, 192, 112) : Color(52, 104, 86);
                FontRenderer::drawText(renderer, menuX + 18, oy, menuOptions[i], c);
            }
        }

        // Bottom Footer
        renderer.draw9SliceBox(4, 166, 312, 12, UITheme::Inverted);
        if (m_cursor == 1 && m_hasSave) {
            FontRenderer::drawText(renderer, 10, 168, "[저장 기록] " + m_saveSummary, Color(224, 248, 208));
        } else {
            FontRenderer::drawText(renderer, 10, 168, "[Z]: 확인 | [X]: 닫기 | [방향키]: 이동", Color(224, 248, 208));
        }
    }
}

void TitleScene::render(Renderer& renderer) {
    // 1. Render Atmospheric Landscape, Mountains & Moon
    renderBackgroundLandscape(renderer);

    // 2. Render Title Logo Header
    renderTitleLogo(renderer);

    // 3. Render Interactive Menu / Start Prompt
    renderMenu(renderer);
}

} // namespace JoseonRPG

