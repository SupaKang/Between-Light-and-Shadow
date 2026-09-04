#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <cmath>

#include "../src/core/types.hpp"
#include "../src/core/renderer.hpp"
#include "../src/ui/font_renderer.hpp"
#include "../src/data/data_manager.hpp"
#include "../src/gameplay/party.hpp"
#include "../src/gameplay/artifact.hpp"
#include "../src/world/world_scene.hpp"
#include "../src/battle/battle_scene.hpp"
#include "../src/scenes/intro_scene.hpp"
#include "../src/scenes/encyclopedia_scene.hpp"
#include "../src/scenes/settings_scene.hpp"
#include "../src/scenes/quest_scene.hpp"
#include "../src/scenes/artifact_scene.hpp"
#include "../src/scenes/party_scene.hpp"
#include "../src/scenes/ending_scene.hpp"
#include "../src/scenes/art_inspector_scene.hpp"

using namespace JoseonRPG;

// Save 320x180 ARGB Framebuffer as uncompressed 24-bit/32-bit BMP file
static bool saveFramebufferToBMP(const uint32_t* fb, int width, int height, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;

    uint32_t rowSize = ((width * 3 + 3) / 4) * 4;
    uint32_t imageSize = rowSize * height;
    uint32_t fileSize = 54 + imageSize;

    // BMP Header (14 bytes)
    uint8_t bmpHeader[14] = {
        'B', 'M',
        static_cast<uint8_t>(fileSize), static_cast<uint8_t>(fileSize >> 8), static_cast<uint8_t>(fileSize >> 16), static_cast<uint8_t>(fileSize >> 24),
        0, 0, 0, 0,
        54, 0, 0, 0
    };
    file.write(reinterpret_cast<const char*>(bmpHeader), 14);

    // DIB Header (40 bytes)
    uint8_t dibHeader[40] = {
        40, 0, 0, 0,
        static_cast<uint8_t>(width), static_cast<uint8_t>(width >> 8), static_cast<uint8_t>(width >> 16), static_cast<uint8_t>(width >> 24),
        static_cast<uint8_t>(height), static_cast<uint8_t>(height >> 8), static_cast<uint8_t>(height >> 16), static_cast<uint8_t>(height >> 24),
        1, 0,
        24, 0,
        0, 0, 0, 0,
        static_cast<uint8_t>(imageSize), static_cast<uint8_t>(imageSize >> 8), static_cast<uint8_t>(imageSize >> 16), static_cast<uint8_t>(imageSize >> 24),
        0x13, 0x0B, 0, 0,
        0x13, 0x0B, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    };
    file.write(reinterpret_cast<const char*>(dibHeader), 40);

    // Pixel data (bottom-up in BMP format)
    std::vector<uint8_t> row(rowSize, 0);
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            uint32_t pixel = fb[y * width + x];
            uint8_t b = pixel & 0xFF;
            uint8_t g = (pixel >> 8) & 0xFF;
            uint8_t r = (pixel >> 16) & 0xFF;
            row[x * 3 + 0] = b;
            row[x * 3 + 1] = g;
            row[x * 3 + 2] = r;
        }
        file.write(reinterpret_cast<const char*>(row.data()), rowSize);
    }

    file.close();
    std::cout << "[CAPTURED SCREENSHOT] -> " << filename << " (" << width << "x" << height << ")" << std::endl;
    return true;
}

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << " [108: 음양견문록] AUTOMATED SCREENSHOT GENERATOR " << std::endl;
    std::cout << "==================================================" << std::endl;

    DataManager::init();

    Party party;
    ArtifactInventory artifacts;
    int money = 500;

    Renderer renderer;

    // 1. Capture Intro Monologue Slide
    {
        IntroScene intro(party, artifacts, money);
        intro.onEnter();
        intro.update(0.6f); // Clear fade
        renderer.clear();
        intro.render(renderer);
        saveFramebufferToBMP(renderer.getFramebuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, "screen_01_intro.bmp");
    }

    // 2. Capture Tavern Room (Map 1 Interior 10x8 Scale)
    {
        WorldScene world(party, artifacts, money);
        world.onEnter();
        world.setPlayerPosition(5, 6, 1); // Inside Map 1 Tavern
        world.update(0.6f); // Clear fade
        renderer.clear();
        world.render(renderer);
        saveFramebufferToBMP(renderer.getFramebuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, "screen_02_tavern_room.bmp");
    }

    // 3. Capture Village Plaza & Start Menu (Map 0 with 4x3 Hanok & Dangsan Tree)
    {
        WorldScene world(party, artifacts, money);
        world.onEnter();
        world.setPlayerPosition(36, 28, 0); // At Dangsan Tree plaza
        world.update(0.6f); // Clear fade
        renderer.clear();
        world.render(renderer);
        saveFramebufferToBMP(renderer.getFramebuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, "screen_03_village_field.bmp");

        // Open Start Menu
        world.openStartMenu();
        renderer.clear();
        world.render(renderer);
        saveFramebufferToBMP(renderer.getFramebuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, "screen_04_start_menu.bmp");
    }

    // 4. Capture 1 vs 1 Battle Scene (Solo Exorcist vs Wild Dokkaebi)
    {
        Yokai wildEnemy = DataManager::createYokaiById("YOKAI_001"); // Dokkaebi
        wildEnemy.setLevel(5);

        BattleScene battle(party, wildEnemy, artifacts, false);
        battle.onEnter();
        battle.update(0.016f);
        renderer.clear();
        battle.render(renderer);
        saveFramebufferToBMP(renderer.getFramebuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, "screen_05_battle.bmp");
    }

    // 5. Capture 108 Codex Encyclopedia Scene
    {
        EncyclopediaScene codex(DataManager::getEncyclopedia());
        codex.onEnter();
        renderer.clear();
        codex.render(renderer);
        saveFramebufferToBMP(renderer.getFramebuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, "screen_06_codex.bmp");
    }

    // 6. Capture Artifacts Bag Scene
    {
        Artifact hat = DataManager::createArtifactById("ART_DOKKAEBI_HAT");
        artifacts.addArtifact(hat);
        ArtifactScene artScene(artifacts, party);
        artScene.onEnter();
        renderer.clear();
        artScene.render(renderer);
        saveFramebufferToBMP(renderer.getFramebuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, "screen_07_artifacts.bmp");
    }

    // 7. Capture Art Inspector Scene (108 Yokai Pixel Asset Viewer)
    {
        ArtInspectorScene inspector;
        inspector.onEnter();
        renderer.clear();
        inspector.render(renderer);
        saveFramebufferToBMP(renderer.getFramebuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, "screen_08_art_inspector.bmp");
    }

    std::cout << "\n[ALL SCREENSHOTS GENERATED SUCCESSFULLY!]" << std::endl;
    return 0;
}

