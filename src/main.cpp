#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <cstdint>
#include <vector>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstring>
#include <algorithm>

#include "battle_rules.h"
#include "status_rules.h"
#include "battle_state.h"
#include "battle_command.h"
#include "battle_ui.h"
#include "save_rules.h"
#include "skill_state.h"
#include "world_state.h"
#include "collection_state.h"
#include "data_registry.h"
#include "font.h"

namespace {

constexpr int W = 960, H = 540, SCALE = 2, TILE = 32;
using Status = status_rules::Kind;

std::vector<std::uint32_t> pixels(W * H);
bool running = true;
bool dialogue = false;
bool battle = false;
bool menu = false;
int save_slot = 1;

battle::State battle_state;
battle::Command battle_command = battle::Command::Skill1;
skills::State skill_state;
collection::State collection_state;
world::State world_state;

int player_hp = 100, player_max_hp = 100;
int player_qi = 30, player_max_qi = 30;
int player_attack = 12, player_defense = 9, player_speed = 10;
int player_level = 1, player_xp = 0;

int enemy_hp = 80, enemy_max_hp = 80;
int enemy_attack = 10, enemy_defense = 8, enemy_speed = 8;
int enemy_level = 1;
int enemy_turns = 0;
Status enemy_status = Status::None;
int enemy_status_turns = 0;

int artifact_attack_bonus = 3, artifact_battle_cost = 2;
bool artifact_owned = false;
int burn_damage = 3;
double fear_multiplier = 1.5;

int steps = 0;
int encounter_interval = 12;

char battle_msg[128] = "A WILD DOKKAEBI APPEARED!";
int dialogue_page = 0;
const char* dialogue_lines[] = {
    "ANCIENT SHRINE OF THE SPIRIT REALM",
    "THE YIN-YANG BALANCE IS WAVERING...",
    "BEWARE THE CORRUPTED ENCOUNTERS AHEAD"
};
constexpr int dialogue_line_count = 3;

int player_x = 15, player_y = 8;

bool blocked(int x, int y) {
    if (x < 2 || x > 27 || y < 3 || y > 14) return true;
    if (x == 15 && y == 7) return true; // Shrine object collision
    return false;
}

bool near_shrine() {
    return world_state.can_interact(player_x, player_y);
}

void text(const char* s, int x, int y, int scale = 2, std::uint32_t color = 0xFFE8D8A0u) {
    font::draw_text(pixels.data(), W, H, s, x, y, scale, color);
}

void fill_rect(int rx, int ry, int rw, int rh, std::uint32_t color) {
    int x0 = (std::max)(0, rx);
    int y0 = (std::max)(0, ry);
    int x1 = (std::min)(W, rx + rw);
    int y1 = (std::min)(H, ry + rh);
    for (int y = y0; y < y1; ++y) {
        for (int x = x0; x < x1; ++x) {
            pixels[y * W + x] = color;
        }
    }
}

void stroke_rect(int rx, int ry, int rw, int rh, int thick, std::uint32_t color) {
    fill_rect(rx, ry, rw, thick, color);
    fill_rect(rx, ry + rh - thick, rw, thick, color);
    fill_rect(rx, ry, thick, rh, color);
    fill_rect(rx + rw - thick, ry, thick, rh, color);
}

struct SaveData {
    std::uint32_t magic, version;
    int x, y, hp, qi, level, xp, party, ids[3];
    std::uint8_t contracted, artifact, shrine_seen;
    std::uint32_t checksum;
};

std::string slot_file() {
    return "save_slot_" + std::to_string(save_slot) + ".sav";
}

void save_game() {
    SaveData s{
        save_rules::Magic, save_rules::Version,
        player_x, player_y, player_hp, player_qi, player_level, player_xp,
        collection_state.party_count,
        {collection_state.party_ids[0], collection_state.party_ids[1], collection_state.party_ids[2]},
        static_cast<std::uint8_t>(collection_state.contains(1)),
        static_cast<std::uint8_t>(artifact_owned),
        static_cast<std::uint8_t>(world_state.shrine_event_seen),
        0
    };
    s.checksum = save_rules::checksum(
        s.version, s.x, s.y, s.hp, s.qi, s.level, s.xp, s.party, s.ids,
        s.contracted != 0, s.artifact != 0, s.shrine_seen != 0
    );
    std::ofstream f(slot_file(), std::ios::binary);
    f.write(reinterpret_cast<const char*>(&s), sizeof(s));
}

void load_game() {
    SaveData s{};
    std::ifstream f(slot_file(), std::ios::binary);
    if (f.read(reinterpret_cast<char*>(&s), sizeof(s)) &&
        s.magic == save_rules::Magic &&
        s.version == save_rules::Version &&
        s.checksum == save_rules::checksum(s.version, s.x, s.y, s.hp, s.qi, s.level, s.xp, s.party, s.ids, s.contracted != 0, s.artifact != 0, s.shrine_seen != 0) &&
        s.x >= 0 && s.x < W / TILE && s.y >= 0 && s.y < H / TILE) {
        player_x = s.x;
        player_y = s.y;
        player_hp = s.hp;
        player_qi = s.qi;
        player_level = s.level;
        player_xp = s.xp;
        collection_state.party_count = s.party;
        for (int i = 0; i < 3; ++i) collection_state.party_ids[i] = s.ids[i];
        world_state.shrine_event_seen = (s.shrine_seen != 0);
        if (s.contracted) collection_state.discover(1);
        artifact_owned = (s.artifact != 0);
    }
}

void begin_battle() {
    battle = true;
    battle_state.reset();
    battle_state.active = true;
    battle_state.enemy_level = enemy_level;
    enemy_max_hp = 80 + enemy_level * 5;
    enemy_hp = enemy_max_hp;
    enemy_turns = 0;
    enemy_status = Status::None;
    enemy_status_turns = 0;
    battle_command = battle::Command::Skill1;
    std::snprintf(battle_msg, sizeof(battle_msg), "A WILD DOKKAEBI APPEARED! CHOOSE ACTION");
}

void finish_battle(bool victory) {
    battle = false;
    battle_state.active = false;
    if (victory) {
        player_xp += enemy_level * 25;
        if (player_xp >= player_level * 100 && player_level < 50) {
            player_xp -= player_level * 100;
            ++player_level;
            player_max_hp += 5;
            player_max_qi += 2;
            player_attack += 2;
            player_defense += 1;
            player_hp = player_max_hp;
            player_qi = player_max_qi;
        }
    }
}

void init_game_data() {
    // 1. Load Yokai Definition
    const std::string yokai_raw = data::Registry::load_text("data/yokai_001_dokkaebi.json");
    if (!yokai_raw.empty()) {
        auto ydef = data::Registry::parse_yokai(yokai_raw);
        enemy_hp = ydef.hp;
        enemy_max_hp = ydef.hp;
        enemy_attack = ydef.attack;
        enemy_defense = ydef.defense;
        enemy_speed = ydef.speed;
    }

    // 2. Load 4 Skills
    const char* skill_files[4] = {
        "data/skill_001_basic_strike.json",
        "data/skill_002_ember.json",
        "data/skill_003_frost.json",
        "data/skill_004_seal.json"
    };
    const char* default_names[4] = {"STRIKE", "EMBER", "FROST", "SEAL"};
    int default_pwrs[4] = {20, 8, 6, 4};
    int default_qis[4] = {0, 3, 4, 5};
    int default_accs[4] = {100, 95, 90, 85};
    Status default_stats[4] = {Status::None, Status::Burn, Status::Freeze, Status::Seal};
    int default_seals[4] = {0, 0, 0, 1};

    for (int i = 0; i < 4; ++i) {
        std::string raw = data::Registry::load_text(skill_files[i]);
        if (!raw.empty()) {
            auto sdef = data::Registry::parse_skill(raw);
            skill_state.configure_slot(i, i + 1, default_names[i], sdef.power, sdef.qi_cost, sdef.accuracy, sdef.status, sdef.seal_turns);
        } else {
            skill_state.configure_slot(i, i + 1, default_names[i], default_pwrs[i], default_qis[i], default_accs[i], default_stats[i], default_seals[i]);
        }
    }

    // 3. Load Encounters
    const std::string enc_raw = data::Registry::load_text("data/encounter_village_edge.json");
    if (!enc_raw.empty()) {
        encounter_interval = data::Registry::integer(enc_raw, "encounter_step_interval", encounter_interval);
        enemy_level = data::Registry::integer(enc_raw, "min_level", enemy_level);
        enemy_max_hp = 80 + enemy_level * 5;
        enemy_hp = enemy_max_hp;
    }

    // 4. Load Artifact
    const std::string art_raw = data::Registry::load_text("data/artifact_001_shrine_shard.json");
    if (!art_raw.empty()) {
        auto adef = data::Registry::parse_artifact(art_raw);
        artifact_attack_bonus = adef.attack_bonus;
        artifact_battle_cost = adef.hp_loss_per_battle;
    }

    // 5. Load Statuses
    const std::string stat_raw = data::Registry::load_text("data/status_effects.json");
    if (!stat_raw.empty()) {
        burn_damage = data::Registry::integer(stat_raw, "damage_per_turn", burn_damage);
        fear_multiplier = data::Registry::number(stat_raw, "damage_taken_multiplier", fear_multiplier);
    }

    // Initialize player party with 1 Dokkaebi ally
    collection_state.add_contract(1);
}

void render_overworld() {
    // 1. Terrain Tiles (32x32)
    for (int ty = 0; ty < H / TILE; ++ty) {
        for (int tx = 0; tx < W / TILE; ++tx) {
            bool path = (tx >= 13 && tx <= 17) || (ty >= 7 && ty <= 9);
            std::uint32_t c = path ? 0xFF5D7A4Eu : 0xFF2A452Eu;
            if ((tx + ty) % 2 == 1) c += 0x00040404u;
            fill_rect(tx * TILE, ty * TILE, TILE, TILE, c);
        }
    }

    // 2. Shrine Monument at (15, 7)
    fill_rect(15 * TILE + 4, 7 * TILE + 4, 24, 24, 0xFF8A5D3Bu);
    fill_rect(15 * TILE + 8, 7 * TILE, 16, 6, 0xFFD9C47Au);
    stroke_rect(15 * TILE + 2, 7 * TILE + 2, 28, 28, 2, 0xFFE8D8A0u);

    // 3. Player Sprite (32x32 Exorcist Traveler)
    int px = player_x * TILE;
    int py = player_y * TILE;
    fill_rect(px + 10, py + 2, 12, 4, 0xFF101410u);  // Gat (Hat) brim
    fill_rect(px + 12, py + 6, 8, 4, 0xFF282C28u);   // Hat top
    fill_rect(px + 11, py + 10, 10, 8, 0xFFF0D4B2u); // Face
    fill_rect(px + 13, py + 13, 2, 2, 0xFF101010u);  // Eye L
    fill_rect(px + 17, py + 13, 2, 2, 0xFF101010u);  // Eye R
    fill_rect(px + 9, py + 18, 14, 12, 0xFFD8D2C2u); // Hanbok Robe
    fill_rect(px + 12, py + 20, 8, 3, 0xFF3D5A80u);  // Belt

    // 4. Top HUD Bar
    fill_rect(0, 0, W, 48, 0xDD121B14u);
    stroke_rect(0, 0, W, 48, 2, 0xFF3D5A3Du);

    char hud_buf[128];
    std::snprintf(hud_buf, sizeof(hud_buf), "HP:%d/%d", player_hp, player_max_hp);
    text(hud_buf, 20, 10, 2, 0xFF7DDA72u);
    fill_rect(140, 12, 120, 10, 0xFF1E2D20u);
    fill_rect(140, 12, (player_hp * 120) / player_max_hp, 10, 0xFF7DDA72u);

    std::snprintf(hud_buf, sizeof(hud_buf), "QI:%d/%d", player_qi, player_max_qi);
    text(hud_buf, 20, 26, 2, 0xFF6EB8EAu);
    fill_rect(140, 28, 120, 10, 0xFF1E2D20u);
    fill_rect(140, 28, (player_qi * 120) / player_max_qi, 10, 0xFF6EB8EAu);

    std::snprintf(hud_buf, sizeof(hud_buf), "LV.%d  XP:%d  STEPS:%d", player_level, player_xp, steps);
    text(hud_buf, 300, 16, 2, 0xFFE8D8A0u);

    // Party indicators at top right
    text("PARTY:", 620, 16, 2, 0xFFE8D8A0u);
    for (int i = 0; i < 3; ++i) {
        int sx = 720 + i * 44;
        bool has_member = (i < collection_state.party_count);
        fill_rect(sx, 10, 32, 28, has_member ? 0xFF8A5D3Bu : 0xFF243026u);
        stroke_rect(sx, 10, 32, 28, 2, 0xFFE8D8A0u);
        if (has_member) text("Y1", sx + 6, 16, 2, 0xFFE8D8A0u);
    }

    if (artifact_owned) {
        text("[ART:SHRINE SHARD]", 860, 16, 1, 0xFFB4A6D8u);
    }

    // 5. Bottom Controls Guide
    fill_rect(0, H - 28, W, 28, 0xEE121B14u);
    text("[ARROWS] MOVE   [ENTER/Z] EXAMINE   [B] BATTLE   [M] PARTY & CODEX   [S] SAVE   [L] LOAD", 40, H - 20, 2, 0xFF9EBAA0u);
}

void render_battle() {
    // 1. Dark Arena Background
    fill_rect(40, 20, W - 80, H - 40, 0xFF0E161Cu);
    stroke_rect(40, 20, W - 80, H - 40, 4, 0xFF3D5A40u);

    // 2. Enemy Box (Top Right)
    fill_rect(540, 50, 340, 140, 0xFF182428u);
    stroke_rect(540, 50, 340, 140, 2, 0xFF4A6860u);

    char enemy_info[64];
    std::snprintf(enemy_info, sizeof(enemy_info), "WILD DOKKAEBI  LV.%d", enemy_level);
    text(enemy_info, 560, 68, 2, 0xFFE8D8A0u);

    char hp_str[32];
    std::snprintf(hp_str, sizeof(hp_str), "HP: %d/%d", enemy_hp, enemy_max_hp);
    text(hp_str, 560, 96, 2, 0xFF7DDA72u);
    fill_rect(560, 120, 220, 14, 0xFF28352Au);
    int enemy_bar_w = (std::max)(0, (enemy_hp * 220) / enemy_max_hp);
    fill_rect(560, 120, enemy_bar_w, 14, 0xFFE26A5Au);

    if (enemy_status != Status::None) {
        text(status_rules::short_tag(enemy_status), 800, 118, 2, 0xFFFFCC00u);
    }

    // Enemy Sprite
    fill_rect(240, 80, 80, 80, 0xFF7D9A62u);
    fill_rect(255, 65, 10, 20, 0xFFD9C47Au); // Horn L
    fill_rect(295, 65, 10, 20, 0xFFD9C47Au); // Horn R
    fill_rect(260, 100, 8, 8, 0xFFB83020u);  // Eye L
    fill_rect(290, 100, 8, 8, 0xFFB83020u);  // Eye R
    fill_rect(270, 125, 20, 8, 0xFFE8D8A0u); // Fangs

    // 3. Player Ally Box (Middle Left)
    fill_rect(80, 180, 340, 130, 0xFF182428u);
    stroke_rect(80, 180, 340, 130, 2, 0xFF4A6860u);

    text("DOKKAEBI (ALLY)", 100, 195, 2, 0xFFE8D8A0u);
    std::snprintf(hp_str, sizeof(hp_str), "HP: %d/%d", player_hp, player_max_hp);
    text(hp_str, 100, 220, 2, 0xFF7DDA72u);
    fill_rect(100, 240, 200, 12, 0xFF28352Au);
    int player_bar_w = (std::max)(0, (player_hp * 200) / player_max_hp);
    fill_rect(100, 240, player_bar_w, 12, 0xFF7DDA72u);

    std::snprintf(hp_str, sizeof(hp_str), "QI: %d/%d", player_qi, player_max_qi);
    text(hp_str, 100, 260, 2, 0xFF6EB8EAu);
    fill_rect(100, 280, 200, 12, 0xFF28352Au);
    int qi_bar_w = (std::max)(0, (player_qi * 200) / player_max_qi);
    fill_rect(100, 280, qi_bar_w, 12, 0xFF6EB8EAu);

    // Player Sprite
    fill_rect(640, 210, 70, 70, 0xFFD9C47Au);
    fill_rect(650, 225, 6, 6, 0xFF101010u);
    fill_rect(680, 225, 6, 6, 0xFF101010u);

    // 4. Action Banner / Message Log
    fill_rect(80, 330, W - 160, 36, 0xFF1E2E28u);
    stroke_rect(80, 330, W - 160, 36, 2, 0xFF6E8D62u);
    text(battle_msg, 100, 340, 2, 0xFFFFFFFFu);

    // 5. Six Battle Commands
    const int cmd_w = 124, cmd_h = 44, start_x = 80, cmd_y = 380;
    int current_cmd = static_cast<int>(battle_command);

    for (int i = 0; i < 6; ++i) {
        int cx = start_x + i * 134;
        bool selected = (i == current_cmd);
        fill_rect(cx, cmd_y, cmd_w, cmd_h, selected ? 0xFF3D5A48u : 0xFF1A2620u);
        stroke_rect(cx, cmd_y, cmd_w, cmd_h, selected ? 3 : 1, selected ? 0xFFFFD700u : 0xFF4D6A58u);

        const char* label = battle_ui::command_label(i, skill_state);
        text(label, cx + 12, cmd_y + 14, 2, selected ? 0xFFFFD700u : 0xFFD8D2C2u);
    }

    // 6. Command Detail Sub-Panel
    fill_rect(80, 436, W - 160, 54, 0xFF121C18u);
    stroke_rect(80, 436, W - 160, 54, 1, 0xFF3D5A48u);

    int cap_rate = battle::capture_rate_percent(enemy_hp, enemy_max_hp, enemy_status != Status::None);
    char detail_buf[128];
    battle_ui::command_detail(current_cmd, skill_state, player_qi, enemy_hp, enemy_max_hp, cap_rate, detail_buf, sizeof(detail_buf));
    text(detail_buf, 100, 452, 2, 0xFFE8D8A0u);

    // Controls at bottom
    text("[LEFT/RIGHT] CHOOSE COMMAND    [ENTER/Z] EXECUTE    [ESC] SURRENDER", 100, 500, 2, 0xFF7D9A82u);
}

void render_menu() {
    fill_rect(60, 40, W - 120, H - 80, 0xFF142018u);
    stroke_rect(60, 40, W - 120, H - 80, 4, 0xFF7D9A62u);

    text("=== PARTY & 108 YOKAI CODEX ===", 100, 60, 3, 0xFFFFD700u);

    // Party section
    text("ACTIVE PARTY (MAX 3):", 100, 110, 2, 0xFFE8D8A0u);
    for (int i = 0; i < 3; ++i) {
        int y = 140 + i * 50;
        fill_rect(100, y, 760, 40, 0xFF1E2D22u);
        stroke_rect(100, y, 760, 40, 1, 0xFF4D6A58u);
        if (i < collection_state.party_count) {
            char pbuf[128];
            std::snprintf(pbuf, sizeof(pbuf), "SLOT %d: #%03d DOKKAEBI   GRADE: I   LV.%d   HP: %d/%d   QI: %d/%d",
                i + 1, collection_state.party_ids[i], player_level, player_hp, player_max_hp, player_qi, player_max_qi);
            text(pbuf, 120, y + 12, 2, 0xFFFFFFFFu);
        } else {
            char pbuf[64];
            std::snprintf(pbuf, sizeof(pbuf), "SLOT %d: [EMPTY SLOT]", i + 1);
            text(pbuf, 120, y + 12, 2, 0xFF6E8D72u);
        }
    }

    // Codex section
    text("108 YOKAI CODEX PROGRESS:", 100, 310, 2, 0xFFE8D8A0u);
    char codex_buf[128];
    std::snprintf(codex_buf, sizeof(codex_buf), "DISCOVERED: %d / 108    CONTRACTED: %d / 108",
        collection_state.discovered_count(), collection_state.contracted_count());
    text(codex_buf, 100, 335, 2, 0xFF7DDA72u);

    fill_rect(100, 360, 400, 16, 0xFF28352Au);
    int codex_w = (collection_state.discovered_count() * 400) / 108;
    fill_rect(100, 360, codex_w, 16, 0xFF7DDA72u);

    // Artifact section
    text("EQUIPPED ARTIFACT:", 100, 395, 2, 0xFFE8D8A0u);
    if (artifact_owned) {
        text("SHRINE SHARD (+3 ATK BONUS / -2 HP PER BATTLE)   [PRESS D TO DESTROY]", 100, 420, 2, 0xFFB4A6D8u);
    } else {
        text("NO ARTIFACT EQUIPPED (ACQUIRE AT SHRINE EVENTS)", 100, 420, 2, 0xFF6E8D72u);
    }

    text("[ESC / M] RETURN TO GAME", 100, 465, 2, 0xFF9EBAA0u);
}

void render_dialogue() {
    fill_rect(60, H - 160, W - 120, 130, 0xEE142018u);
    stroke_rect(60, H - 160, W - 120, 130, 3, 0xFFE8D8A0u);

    text("--- SHRINE RECORD ---", 90, H - 145, 2, 0xFFFFD700u);
    text(dialogue_lines[dialogue_page], 90, H - 110, 2, 0xFFFFFFFFu);
    text("[ENTER / Z] CONTINUE", 90, H - 65, 2, 0xFF7DDA72u);
}

void render() {
    render_overworld();
    if (dialogue) render_dialogue();
    if (battle) render_battle();
    if (menu) render_menu();
}

#ifdef _WIN32
LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_KEYDOWN) {
        if (wp == VK_ESCAPE) {
            if (dialogue) { dialogue = false; render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
            if (menu) { menu = false; render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
            if (battle) { finish_battle(false); render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
            running = false;
            PostQuitMessage(0);
            return 0;
        }

        // Menu Mode Handling
        if (menu) {
            if (wp == 'M' || wp == VK_ESCAPE) { menu = false; render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
            if (wp == 'D' && artifact_owned) {
                artifact_owned = false;
                render();
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            return 0;
        }

        // Dialogue Mode Handling
        if (dialogue) {
            if (wp == VK_RETURN || wp == 'Z') {
                if (++dialogue_page >= dialogue_line_count) {
                    dialogue = false;
                    dialogue_page = 0;
                    artifact_owned = true; // Gift shard upon reading shrine
                }
                render();
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        // Battle Mode Handling
        if (battle) {
            if (wp == VK_LEFT || wp == VK_RIGHT) {
                int cur = static_cast<int>(battle_command);
                int dir = (wp == VK_RIGHT) ? 1 : -1;
                battle_command = static_cast<battle::Command>(battle_ui::move_command(cur, dir, 6));
                render();
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }

            if (wp == VK_RETURN || wp == 'Z') {
                int cmd = static_cast<int>(battle_command);
                if (cmd == 5) { // Run
                    std::snprintf(battle_msg, sizeof(battle_msg), "SAFELY FLED FROM BATTLE!");
                    finish_battle(false);
                    render();
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }

                if (cmd == 4) { // Capture
                    int cap_rate = battle::capture_rate_percent(enemy_hp, enemy_max_hp, enemy_status != Status::None);
                    if (battle::can_capture(enemy_hp, enemy_max_hp, enemy_turns, enemy_status != Status::None)) {
                        collection_state.discover(1);
                        collection_state.add_contract(1);
                        std::snprintf(battle_msg, sizeof(battle_msg), "CONTRACT SUCCESS! DOKKAEBI JOINED PARTY!");
                        finish_battle(true);
                    } else {
                        std::snprintf(battle_msg, sizeof(battle_msg), "CONTRACT FAILED (%d%% CHANCE)! ENEMY RESISTED!", cap_rate);
                    }
                } else if (cmd >= 0 && cmd < 4) { // Skills 1-4
                    if (!skill_state.usable(cmd, player_qi)) {
                        if (skill_state.sealed_turns[cmd] > 0) {
                            std::snprintf(battle_msg, sizeof(battle_msg), "CANNOT USE: SKILL IS CURRENTLY SEALED!");
                        } else {
                            std::snprintf(battle_msg, sizeof(battle_msg), "CANNOT USE: INSUFFICIENT QI!");
                        }
                        render();
                        InvalidateRect(hwnd, nullptr, FALSE);
                        return 0;
                    }

                    // Spend Qi
                    player_qi -= skill_state.qi_costs[cmd];

                    // Calculate damage
                    int pwr = skill_state.powers[cmd] + (artifact_owned ? artifact_attack_bonus : 0);
                    int dmg = battle::damage(
                        {player_attack, player_defense, player_hp, player_qi, player_speed},
                        {enemy_attack, enemy_defense, enemy_hp, 0, enemy_speed},
                        pwr, false, enemy_status == Status::Fear
                    );
                    enemy_hp -= dmg;
                    enemy_status = status_rules::on_hit_effect(enemy_status); // Damage breaks freeze

                    // Apply Skill Status Effect
                    Status st = skill_state.statuses[cmd];
                    if (st != Status::None) {
                        enemy_status = st;
                        enemy_status_turns = status_rules::default_duration(st);
                    }

                    // Apply Seal to Skill if configured
                    if (skill_state.seal_duration[cmd] > 0) {
                        skill_state.seal(cmd, skill_state.seal_duration[cmd]);
                    }

                    std::snprintf(battle_msg, sizeof(battle_msg), "USED %s! DEALT %d DAMAGE TO ENEMY!",
                        skill_state.names[cmd], dmg);
                }

                // Check Enemy Defeat
                if (enemy_hp <= 0) {
                    std::snprintf(battle_msg, sizeof(battle_msg), "VICTORY! EARNED %d XP!", enemy_level * 25);
                    finish_battle(true);
                    render();
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }

                // Enemy Retaliation Turn
                ++enemy_turns;
                skill_state.tick();

                bool enemy_skips = status_rules::skips_action(enemy_status, enemy_turns);
                if (enemy_skips) {
                    std::snprintf(battle_msg, sizeof(battle_msg), "ENEMY IS %s AND CANNOT MOVE!",
                        status_rules::name(enemy_status));
                } else {
                    int edmg = battle::damage(
                        {enemy_attack, enemy_defense, enemy_hp, 0, enemy_speed},
                        {player_attack, player_defense, player_hp, player_qi, player_speed},
                        5, enemy_status == Status::Fear, false
                    );
                    player_hp -= edmg;
                }

                // Burn Tick Damage
                if (enemy_hp > 0 && enemy_status == Status::Burn) {
                    enemy_hp -= burn_damage;
                }

                // Artifact HP drawback
                if (artifact_owned && player_hp > 0) {
                    player_hp -= artifact_battle_cost;
                }

                // Status countdown
                if (enemy_status_turns > 0) {
                    --enemy_status_turns;
                    if (enemy_status_turns == 0) enemy_status = Status::None;
                }

                // Check Player Defeat
                if (player_hp <= 0) {
                    player_hp = player_max_hp;
                    std::snprintf(battle_msg, sizeof(battle_msg), "PLAYER COLLAPSED... RETURNED TO SHRINE");
                    finish_battle(false);
                }

                render();
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            return 0;
        }

        // Overworld Mode Handling
        if (!dialogue && !battle && !menu) {
            if (wp == 'M') { menu = true; render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
            if (wp == 'B') { begin_battle(); render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
            if (wp == 'S') { save_game(); render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
            if (wp == 'L') { load_game(); render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
            if (wp >= '1' && wp <= '3') { save_slot = static_cast<int>(wp - '0'); return 0; }

            int nx = player_x, ny = player_y;
            if (wp == VK_LEFT) --nx;
            if (wp == VK_RIGHT) ++nx;
            if (wp == VK_UP) --ny;
            if (wp == VK_DOWN) ++ny;

            if ((wp == VK_LEFT || wp == VK_RIGHT || wp == VK_UP || wp == VK_DOWN) && !blocked(nx, ny)) {
                player_x = nx;
                player_y = ny;
                ++steps;
                if (encounter_interval > 0 && (steps % encounter_interval == 0)) {
                    begin_battle();
                }
                render();
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }

            if (wp == VK_RETURN || wp == 'Z') {
                if (near_shrine()) {
                    dialogue = true;
                    dialogue_page = 0;
                    world_state.mark_shrine_seen();
                    render();
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }
            }
        }
        return 0;
    }

    if (msg == WM_DESTROY) {
        running = false;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
#endif

} // namespace

#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    init_game_data();

    const wchar_t* name = L"YinYangChronicleWindow";
    WNDCLASSW wc{};
    wc.hInstance = instance;
    wc.lpfnWndProc = wnd_proc;
    wc.lpszClassName = name;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassW(&wc);

    RECT rect{0, 0, W * SCALE, H * SCALE};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    HWND hwnd = CreateWindowExW(
        0, name, L"108: 음양견문록 (108: Yin-Yang Chronicle)", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
        nullptr, nullptr, instance, nullptr
    );
    if (!hwnd) return 1;

    ShowWindow(hwnd, show);
    render();

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = W;
    bmi.bmiHeader.biHeight = -H;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    MSG msg{};
    while (running) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        HDC dc = GetDC(hwnd);
        RECT client{};
        GetClientRect(hwnd, &client);
        StretchDIBits(dc, 0, 0, client.right, client.bottom, 0, 0, W, H, pixels.data(), &bmi, DIB_RGB_COLORS, SRCCOPY);
        ReleaseDC(hwnd, dc);
        Sleep(16);
    }
    return 0;
}
#else
int main() {
    init_game_data();
    render();
    return 0;
}
#endif
