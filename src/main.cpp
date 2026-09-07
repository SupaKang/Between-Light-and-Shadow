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
bool is_boss_battle = false;
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

char enemy_name[64] = "DOKKAEBI";
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
char notice_msg[128] = "EXPLORE DOSEONSA AND SPEAK WITH VILLAGERS";
char battle_msg[128] = "A WILD DOKKAEBI APPEARED!";

int dialogue_page = 0;
int current_dialogue_count = 0;
char dialogue_speaker[64] = "";
char dialogue_pages[4][128];

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

void text(const char* s, int x, int y, int scale = 2, std::uint32_t color = 0xFFE8D8A0u) {
    font::draw_text(pixels.data(), W, H, s, x, y, scale, color);
}

struct SaveData {
    std::uint32_t magic, version;
    int x, y, hp, qi, level, xp, party, ids[3];
    std::uint8_t contracted, artifact, shrine_seen;
    int region_id, quest_step;
    std::uint32_t checksum;
};

std::string slot_file(int slot) {
    return "save_slot_" + std::to_string(slot) + ".sav";
}

bool inspect_slot(int slot, SaveData& out_s) {
    std::ifstream f(slot_file(slot), std::ios::binary);
    if (!f.is_open()) return false;
    if (f.read(reinterpret_cast<char*>(&out_s), sizeof(out_s)) &&
        out_s.magic == save_rules::Magic &&
        out_s.version == save_rules::Version) {
        std::uint32_t expected = save_rules::checksum(
            out_s.version, out_s.x, out_s.y, out_s.hp, out_s.qi, out_s.level, out_s.xp,
            out_s.party, out_s.ids, out_s.contracted != 0, out_s.artifact != 0, out_s.shrine_seen != 0,
            out_s.region_id, out_s.quest_step
        );
        return out_s.checksum == expected;
    }
    return false;
}

void save_game(int slot) {
    SaveData s{
        save_rules::Magic, save_rules::Version,
        world_state.player_x, world_state.player_y, player_hp, player_qi, player_level, player_xp,
        collection_state.party_count,
        {collection_state.party_ids[0], collection_state.party_ids[1], collection_state.party_ids[2]},
        static_cast<std::uint8_t>(collection_state.contains(1)),
        static_cast<std::uint8_t>(artifact_owned),
        static_cast<std::uint8_t>(world_state.shrine_event_seen),
        world_state.region_id, world_state.main_quest_step,
        0
    };
    s.checksum = save_rules::checksum(
        s.version, s.x, s.y, s.hp, s.qi, s.level, s.xp, s.party, s.ids,
        s.contracted != 0, s.artifact != 0, s.shrine_seen != 0,
        s.region_id, s.quest_step
    );
    std::ofstream f(slot_file(slot), std::ios::binary);
    f.write(reinterpret_cast<const char*>(&s), sizeof(s));
    std::snprintf(notice_msg, sizeof(notice_msg), "SAVED PROGRESS TO SLOT %d", slot);
}

bool load_game(int slot) {
    SaveData s{};
    if (inspect_slot(slot, s)) {
        world_state.player_x = s.x;
        world_state.player_y = s.y;
        world_state.region_id = s.region_id;
        world_state.main_quest_step = s.quest_step;
        player_hp = s.hp;
        player_qi = s.qi;
        player_level = s.level;
        player_xp = s.xp;
        collection_state.party_count = s.party;
        for (int i = 0; i < 3; ++i) collection_state.party_ids[i] = s.ids[i];
        world_state.shrine_event_seen = (s.shrine_seen != 0);
        if (s.contracted) collection_state.discover(1);
        artifact_owned = (s.artifact != 0);
        std::snprintf(notice_msg, sizeof(notice_msg), "LOADED PROGRESS FROM SLOT %d", slot);
        return true;
    }
    std::snprintf(notice_msg, sizeof(notice_msg), "SLOT %d IS EMPTY OR CORRUPTED", slot);
    return false;
}

void trigger_dialogue(const char* speaker, const std::vector<std::string>& lines) {
    dialogue = true;
    dialogue_page = 0;
    current_dialogue_count = static_cast<int>(lines.size());
    std::strncpy(dialogue_speaker, speaker, sizeof(dialogue_speaker) - 1);
    for (int i = 0; i < current_dialogue_count && i < 4; ++i) {
        std::strncpy(dialogue_pages[i], lines[i].c_str(), sizeof(dialogue_pages[i]) - 1);
    }
}

void begin_battle(bool boss = false) {
    battle = true;
    is_boss_battle = boss;
    battle_state.reset();
    battle_state.active = true;

    if (boss) {
        std::strncpy(enemy_name, "CORRUPT MONK MYOGAK", sizeof(enemy_name));
        enemy_level = 5;
        enemy_max_hp = 120;
        enemy_hp = 120;
        enemy_attack = 16;
        enemy_defense = 12;
        enemy_speed = 12;
        enemy_turns = 0;
        enemy_status = Status::None;
        enemy_status_turns = 0;
        battle_command = battle::Command::Skill1;
        std::snprintf(battle_msg, sizeof(battle_msg), "BOSS BATTLE: CORRUPT MONK MYOGAK ENGAGES!");
    } else {
        std::strncpy(enemy_name, "DOKKAEBI", sizeof(enemy_name));
        enemy_level = 1 + (world_state.region_id == 2 ? 1 : 0);
        enemy_max_hp = 80 + enemy_level * 5;
        enemy_hp = enemy_max_hp;
        enemy_attack = 10 + enemy_level * 2;
        enemy_defense = 8 + enemy_level;
        enemy_speed = 8 + enemy_level;
        enemy_turns = 0;
        enemy_status = Status::None;
        enemy_status_turns = 0;
        battle_command = battle::Command::Skill1;
        std::snprintf(battle_msg, sizeof(battle_msg), "A WILD DOKKAEBI APPEARED! CHOOSE ACTION");
    }
}

void finish_battle(bool victory) {
    battle = false;
    battle_state.active = false;
    if (victory) {
        if (is_boss_battle) {
            world_state.main_quest_step = 2;
            world_state.boss_defeated = true;
            std::snprintf(notice_msg, sizeof(notice_msg), "CHAPTER 1 CLEAR! DOKKAEBI SECT QUELLED!");
        }
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
    is_boss_battle = false;
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

    collection_state.add_contract(1);
}

void render_overworld() {
    int cur_map = world_state.region_id;

    // 1. Terrain Tiles (32x32) based on Map
    for (int ty = 0; ty < H / TILE; ++ty) {
        for (int tx = 0; tx < W / TILE; ++tx) {
            std::uint32_t c = 0xFF2A452Eu; // Default Grass
            if (cur_map == 1) { // Village Edge
                bool path = (tx >= 13 && tx <= 16) || (ty >= 6 && ty <= 8);
                c = path ? 0xFF6B8250u : 0xFF2F4F35u;
            } else if (cur_map == 2) { // Mountain Pass
                bool path = (tx >= 12 && tx <= 17) && (ty >= 2 && ty <= 14);
                c = path ? 0xFF4A443Cu : 0xFF2A2E28u;
            } else if (cur_map == 3) { // Daewoongjeon Temple
                bool courtyard = (tx >= 6 && tx <= 23 && ty >= 5 && ty <= 13);
                c = courtyard ? 0xFF4A4E54u : 0xFF22262Cu;
            }
            if ((tx + ty) % 2 == 1) c += 0x00040404u;
            fill_rect(tx * TILE, ty * TILE, TILE, TILE, c);
        }
    }

    // 2. Map-Specific Landmark Objects & NPCs
    if (cur_map == 1) {
        // Tavern House (x: 6..10, y: 4..5)
        fill_rect(6 * TILE, 4 * TILE, 5 * TILE, 2 * TILE, 0xFF6B4226u);
        stroke_rect(6 * TILE, 4 * TILE, 5 * TILE, 2 * TILE, 2, 0xFFD9C47Au);
        text("TAVERN", 6 * TILE + 8, 4 * TILE + 10, 2, 0xFFFFD700u);

        // Tavern Keeper NPC at (8, 6)
        fill_rect(8 * TILE + 8, 6 * TILE + 4, 16, 24, 0xFFB84A39u);
        text("JUMO", 8 * TILE, 6 * TILE - 12, 1, 0xFFFFFFFFu);

        // Ancient Shrine at (20, 7)
        fill_rect(20 * TILE + 4, 7 * TILE + 4, 24, 24, 0xFF8A5D3Bu);
        fill_rect(20 * TILE + 8, 7 * TILE, 16, 6, 0xFFD9C47Au);
        stroke_rect(20 * TILE + 2, 7 * TILE + 2, 28, 28, 2, 0xFFE8D8A0u);
        text("SHRINE", 20 * TILE - 6, 7 * TILE - 12, 1, 0xFFE8D8A0u);

        // North Gate to Mountain Pass
        stroke_rect(13 * TILE, 2 * TILE, 4 * TILE, 8, 2, 0xFFFFD700u);
        text("NORTH PASS ^", 13 * TILE, 2 * TILE + 12, 1, 0xFFFFD700u);
    } else if (cur_map == 2) {
        // Mountain Pass Signpost at (13, 8)
        fill_rect(13 * TILE + 12, 8 * TILE + 4, 8, 24, 0xFF8A5D3Bu);
        fill_rect(13 * TILE + 4, 8 * TILE + 2, 24, 12, 0xFFD9C47Au);
        text("SIGN", 13 * TILE + 4, 8 * TILE - 10, 1, 0xFFFFFFFFu);

        // Giant Boulder at (14, 8)
        fill_rect(14 * TILE + 2, 8 * TILE + 2, 28, 28, 0xFF606468u);

        // North & South Gates
        text("TEMPLE GROUNDS ^", 12 * TILE, 2 * TILE + 12, 1, 0xFFFFD700u);
        text("VILLAGE EDGE v", 13 * TILE, 14 * TILE, 1, 0xFFFFD700u);
    } else if (cur_map == 3) {
        // Temple Hall (x: 8..21, y: 3..5)
        fill_rect(8 * TILE, 3 * TILE, 14 * TILE, 3 * TILE, 0xFF7A2020u);
        stroke_rect(8 * TILE, 3 * TILE, 14 * TILE, 3 * TILE, 3, 0xFFFFD700u);
        text("DAEWOONGJEON MAIN TEMPLE", 9 * TILE, 3 * TILE + 16, 2, 0xFFFFD700u);

        // Boss Corrupt Monk Myogak at (15, 6)
        if (!world_state.boss_defeated) {
            fill_rect(15 * TILE + 6, 6 * TILE + 2, 20, 26, 0xFF3D2054u);
            text("MYOGAK (BOSS)", 14 * TILE, 6 * TILE - 14, 1, 0xFFFF5555u);
        } else {
            text("[PURIFIED]", 14 * TILE + 6, 6 * TILE + 8, 1, 0xFF7DDA72u);
        }

        // South Gate
        text("MOUNTAIN PASS v", 13 * TILE, 14 * TILE, 1, 0xFFFFD700u);
    }

    // 3. Player Sprite (32x32 Exorcist Traveler)
    int px = world_state.player_x * TILE;
    int py = world_state.player_y * TILE;
    fill_rect(px + 10, py + 2, 12, 4, 0xFF101410u);  // Gat brim
    fill_rect(px + 12, py + 6, 8, 4, 0xFF282C28u);   // Hat top
    fill_rect(px + 11, py + 10, 10, 8, 0xFFF0D4B2u); // Face
    fill_rect(px + 13, py + 13, 2, 2, 0xFF101010u);  // Eye L
    fill_rect(px + 17, py + 13, 2, 2, 0xFF101010u);  // Eye R
    fill_rect(px + 9, py + 18, 14, 12, 0xFFD8D2C2u); // Hanbok
    fill_rect(px + 12, py + 20, 8, 3, 0xFF3D5A80u);  // Belt

    // 4. Top HUD Bar
    fill_rect(0, 0, W, 48, 0xDD121B14u);
    stroke_rect(0, 0, W, 48, 2, 0xFF3D5A3Du);

    char hud_buf[128];
    std::snprintf(hud_buf, sizeof(hud_buf), "HP:%d/%d", player_hp, player_max_hp);
    text(hud_buf, 16, 10, 2, 0xFF7DDA72u);
    fill_rect(130, 12, 110, 10, 0xFF1E2D20u);
    fill_rect(130, 12, (player_hp * 110) / player_max_hp, 10, 0xFF7DDA72u);

    std::snprintf(hud_buf, sizeof(hud_buf), "QI:%d/%d", player_qi, player_max_qi);
    text(hud_buf, 16, 26, 2, 0xFF6EB8EAu);
    fill_rect(130, 28, 110, 10, 0xFF1E2D20u);
    fill_rect(130, 28, (player_qi * 110) / player_max_qi, 10, 0xFF6EB8EAu);

    std::snprintf(hud_buf, sizeof(hud_buf), "LV.%d XP:%d [MAP: %s]", player_level, player_xp, world_state.current_map_name());
    text(hud_buf, 260, 16, 2, 0xFFE8D8A0u);

    // Party Indicators
    text("PARTY:", 660, 16, 2, 0xFFE8D8A0u);
    for (int i = 0; i < 3; ++i) {
        int sx = 740 + i * 42;
        bool has_member = (i < collection_state.party_count);
        fill_rect(sx, 10, 32, 28, has_member ? 0xFF8A5D3Bu : 0xFF243026u);
        stroke_rect(sx, 10, 32, 28, 2, 0xFFE8D8A0u);
        if (has_member) text("Y1", sx + 6, 16, 2, 0xFFE8D8A0u);
    }

    if (artifact_owned) {
        text("[SHARD]", 875, 16, 1, 0xFFB4A6D8u);
    }

    // 5. Notice Banner
    fill_rect(0, 48, W, 22, 0xAA0C1410u);
    text(notice_msg, 20, 52, 1, 0xFFFFD700u);

    // 6. Bottom Controls Guide
    fill_rect(0, H - 28, W, 28, 0xEE121B14u);
    text("[ARROWS] MOVE   [ENTER/Z] TALK/INSPECT   [M] MENU/SLOTS   [B] BATTLE   [1-3] SLOTS   [S] SAVE   [L] LOAD", 20, H - 20, 2, 0xFF9EBAA0u);
}

void render_battle() {
    // 1. Dark Arena Background
    fill_rect(40, 20, W - 80, H - 40, 0xFF0E161Cu);
    stroke_rect(40, 20, W - 80, H - 40, 4, is_boss_battle ? 0xFF8A2020u : 0xFF3D5A40u);

    // 2. Enemy Box (Top Right)
    fill_rect(540, 50, 340, 140, 0xFF182428u);
    stroke_rect(540, 50, 340, 140, 2, is_boss_battle ? 0xFFB83020u : 0xFF4A6860u);

    char enemy_info[64];
    std::snprintf(enemy_info, sizeof(enemy_info), "%s  LV.%d", enemy_name, enemy_level);
    text(enemy_info, 560, 68, 2, is_boss_battle ? 0xFFFF7777u : 0xFFE8D8A0u);

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
    if (is_boss_battle) {
        fill_rect(240, 70, 80, 90, 0xFF542572u); // Corrupt Monk Robes
        fill_rect(255, 80, 50, 40, 0xFFD8C4A0u); // Face
        fill_rect(265, 95, 6, 6, 0xFFFF0000u);  // Red glowing eyes
        fill_rect(285, 95, 6, 6, 0xFFFF0000u);
    } else {
        fill_rect(240, 80, 80, 80, 0xFF7D9A62u); // Dokkaebi Goblin
        fill_rect(255, 65, 10, 20, 0xFFD9C47Au); // Horn L
        fill_rect(295, 65, 10, 20, 0xFFD9C47Au); // Horn R
        fill_rect(260, 100, 8, 8, 0xFFB83020u);  // Eye L
        fill_rect(290, 100, 8, 8, 0xFFB83020u);  // Eye R
        fill_rect(270, 125, 20, 8, 0xFFE8D8A0u); // Fangs
    }

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

    text("[LEFT/RIGHT] CHOOSE COMMAND    [ENTER/Z] EXECUTE    [ESC] SURRENDER", 100, 500, 2, 0xFF7D9A82u);
}

void render_menu() {
    fill_rect(40, 20, W - 80, H - 40, 0xFF142018u);
    stroke_rect(40, 20, W - 80, H - 40, 4, 0xFF7D9A62u);

    text("=== PARTY, CODEX & SAVE SLOTS ===", 80, 36, 3, 0xFFFFD700u);

    // 1. Party section
    text("ACTIVE PARTY:", 80, 75, 2, 0xFFE8D8A0u);
    for (int i = 0; i < 3; ++i) {
        int y = 98 + i * 36;
        fill_rect(80, y, 400, 30, 0xFF1E2D22u);
        stroke_rect(80, y, 400, 30, 1, 0xFF4D6A58u);
        if (i < collection_state.party_count) {
            char pbuf[128];
            std::snprintf(pbuf, sizeof(pbuf), "SLOT %d: DOKKAEBI #%03d  LV.%d  HP:%d/%d",
                i + 1, collection_state.party_ids[i], player_level, player_hp, player_max_hp);
            text(pbuf, 95, y + 8, 2, 0xFFFFFFFFu);
        } else {
            char pbuf[64];
            std::snprintf(pbuf, sizeof(pbuf), "SLOT %d: [EMPTY SLOT]", i + 1);
            text(pbuf, 95, y + 8, 2, 0xFF6E8D72u);
        }
    }

    // 2. Save Slots section (Visual Save/Load preview)
    text("SAVE/LOAD SLOTS (PRESS 1-3 TO SELECT, S TO SAVE, L TO LOAD):", 510, 75, 2, 0xFFE8D8A0u);
    for (int i = 1; i <= 3; ++i) {
        int y = 98 + (i - 1) * 62;
        bool selected = (save_slot == i);
        fill_rect(510, y, 370, 54, selected ? 0xFF2B4232u : 0xFF1A2620u);
        stroke_rect(510, y, 370, 54, selected ? 2 : 1, selected ? 0xFFFFD700u : 0xFF4D6A58u);

        SaveData s{};
        if (inspect_slot(i, s)) {
            char sbuf[128];
            std::snprintf(sbuf, sizeof(sbuf), "SLOT %d: LV.%d  HP:%d  PARTY:%d", i, s.level, s.hp, s.party);
            text(sbuf, 525, y + 10, 2, selected ? 0xFFFFD700u : 0xFFFFFFFFu);
            const char* rname = (s.region_id == 1) ? "VILLAGE" : (s.region_id == 2 ? "MOUNTAIN PASS" : "DAEWOONGJEON");
            std::snprintf(sbuf, sizeof(sbuf), "MAP: %s  QUEST:%d/2", rname, s.quest_step);
            text(sbuf, 525, y + 30, 2, 0xFF9EBAA0u);
        } else {
            char sbuf[64];
            std::snprintf(sbuf, sizeof(sbuf), "SLOT %d: [EMPTY / NO DATA]", i);
            text(sbuf, 525, y + 18, 2, 0xFF6E8D72u);
        }
    }

    // 3. Codex progress
    text("108 YOKAI CODEX PROGRESS:", 80, 220, 2, 0xFFE8D8A0u);
    char codex_buf[128];
    std::snprintf(codex_buf, sizeof(codex_buf), "DISCOVERED: %d / 108    CONTRACTED: %d / 108",
        collection_state.discovered_count(), collection_state.contracted_count());
    text(codex_buf, 80, 245, 2, 0xFF7DDA72u);

    fill_rect(80, 270, 400, 14, 0xFF28352Au);
    int codex_w = (collection_state.discovered_count() * 400) / 108;
    fill_rect(80, 270, codex_w, 14, 0xFF7DDA72u);

    // 4. Quest & Artifact Status
    text("CURRENT MISSION:", 80, 305, 2, 0xFFE8D8A0u);
    if (world_state.main_quest_step == 0) {
        text("CHAPTER 1: VISIT THE TAVERN JUMO AT DOSEONSA VILLAGE", 80, 330, 2, 0xFFFFCC00u);
    } else if (world_state.main_quest_step == 1) {
        text("CHAPTER 1: TRAVEL NORTH AND QUELL CORRUPT MONK MYOGAK", 80, 330, 2, 0xFFFFCC00u);
    } else {
        text("CHAPTER 1: COMPLETE! YIN-YANG SECT HAS BEEN EXPELLED!", 80, 330, 2, 0xFF7DDA72u);
    }

    text("EQUIPPED ARTIFACT:", 80, 375, 2, 0xFFE8D8A0u);
    if (artifact_owned) {
        text("SHRINE SHARD (+3 ATK / -2 HP PER BATTLE)  [PRESS D TO DESTROY]", 80, 400, 2, 0xFFB4A6D8u);
    } else {
        text("NO ARTIFACT EQUIPPED (ACQUIRE AT ANCIENT SHRINE)", 80, 400, 2, 0xFF6E8D72u);
    }

    // Action feedback inside menu
    fill_rect(80, 440, W - 160, 30, 0xFF0F1812u);
    text(notice_msg, 100, 448, 2, 0xFFFFD700u);

    text("[1-3] SELECT SLOT   [S] SAVE   [L] LOAD   [D] DESTROY ARTIFACT   [ESC / M] EXIT", 80, 485, 2, 0xFF9EBAA0u);
}

void render_dialogue() {
    fill_rect(60, H - 160, W - 120, 130, 0xEE142018u);
    stroke_rect(60, H - 160, W - 120, 130, 3, 0xFFE8D8A0u);

    char title_buf[128];
    std::snprintf(title_buf, sizeof(title_buf), "--- %s ---", dialogue_speaker);
    text(title_buf, 90, H - 145, 2, 0xFFFFD700u);
    text(dialogue_pages[dialogue_page], 90, H - 110, 2, 0xFFFFFFFFu);
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
            if (wp >= '1' && wp <= '3') {
                save_slot = static_cast<int>(wp - '0');
                render(); InvalidateRect(hwnd, nullptr, FALSE); return 0;
            }
            if (wp == 'S') { save_game(save_slot); render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
            if (wp == 'L') { load_game(save_slot); render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
            if (wp == 'D' && artifact_owned) {
                artifact_owned = false;
                std::snprintf(notice_msg, sizeof(notice_msg), "DESTROYED ARTIFACT FOR INVENTORY BALANCE");
                render(); InvalidateRect(hwnd, nullptr, FALSE); return 0;
            }
            if (wp == 'M' || wp == VK_RETURN) { menu = false; render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
            return 0;
        }

        // Dialogue Mode Handling
        if (dialogue) {
            if (wp == VK_RETURN || wp == 'Z') {
                if (++dialogue_page >= current_dialogue_count) {
                    dialogue = false;
                    dialogue_page = 0;
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
                    if (is_boss_battle) {
                        std::snprintf(battle_msg, sizeof(battle_msg), "CANNOT FLEE FROM BOSS BATTLE!");
                    } else {
                        std::snprintf(battle_msg, sizeof(battle_msg), "SAFELY FLED FROM BATTLE!");
                        finish_battle(false);
                    }
                    render();
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }

                if (cmd == 4) { // Capture
                    if (is_boss_battle) {
                        std::snprintf(battle_msg, sizeof(battle_msg), "CANNOT CAPTURE A CORRUPTED BOSS!");
                    } else {
                        int cap_rate = battle::capture_rate_percent(enemy_hp, enemy_max_hp, enemy_status != Status::None);
                        if (battle::can_capture(enemy_hp, enemy_max_hp, enemy_turns, enemy_status != Status::None)) {
                            collection_state.discover(1);
                            collection_state.add_contract(1);
                            std::snprintf(battle_msg, sizeof(battle_msg), "CONTRACT SUCCESS! DOKKAEBI JOINED PARTY!");
                            finish_battle(true);
                        } else {
                            std::snprintf(battle_msg, sizeof(battle_msg), "CONTRACT FAILED (%d%% CHANCE)! ENEMY RESISTED!", cap_rate);
                        }
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
                    std::snprintf(battle_msg, sizeof(battle_msg), "VICTORY OVER %s! GAINED %d XP!",
                        enemy_name, enemy_level * 25);
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
                        is_boss_battle ? 10 : 5, enemy_status == Status::Fear, false
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
                    world_state.change_map(1, 14, 8); // Return to Village Tavern
                    std::snprintf(notice_msg, sizeof(notice_msg), "PLAYER COLLAPSED... RETREATED TO VILLAGE");
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
            if (wp == 'B') { begin_battle(false); render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
            if (wp == 'S') { save_game(save_slot); render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
            if (wp == 'L') { load_game(save_slot); render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
            if (wp >= '1' && wp <= '3') {
                save_slot = static_cast<int>(wp - '0');
                std::snprintf(notice_msg, sizeof(notice_msg), "SELECTED SAVE SLOT %d", save_slot);
                render(); InvalidateRect(hwnd, nullptr, FALSE); return 0;
            }

            int nx = world_state.player_x, ny = world_state.player_y;
            if (wp == VK_LEFT) --nx;
            if (wp == VK_RIGHT) ++nx;
            if (wp == VK_UP) --ny;
            if (wp == VK_DOWN) ++ny;

            if (wp == VK_LEFT || wp == VK_RIGHT || wp == VK_UP || wp == VK_DOWN) {
                // Check map transition portal
                int target_map = 0, target_x = 0, target_y = 0;
                if (world_state.check_portal(nx, ny, target_map, target_x, target_y)) {
                    world_state.change_map(target_map, target_x, target_y);
                    std::snprintf(notice_msg, sizeof(notice_msg), "ARRIVED AT: %s", world_state.current_map_name());
                    render(); InvalidateRect(hwnd, nullptr, FALSE); return 0;
                }

                if (!world_state.is_blocked(nx, ny)) {
                    world_state.player_x = nx;
                    world_state.player_y = ny;
                    ++steps;
                    int rate = world_state.map_encounter_rate();
                    if (rate > 0 && (steps % rate == 0)) {
                        begin_battle(false);
                    }
                    render(); InvalidateRect(hwnd, nullptr, FALSE); return 0;
                }
            }

            if (wp == VK_RETURN || wp == 'Z') {
                int npc = world_state.check_npc_interaction(world_state.player_x, world_state.player_y);
                if (npc == 1) { // Tavern Jumo
                    player_hp = player_max_hp;
                    player_qi = player_max_qi;
                    if (world_state.main_quest_step == 0) {
                        world_state.advance_quest();
                        trigger_dialogue("TAVERN JUMO", {
                            "WELCOME TRAVELER! REST YOUR SPIRIT AT OUR INN.",
                            "RUMORS SAY YIN-YANG SECT HAS CORRUPTED THE NORTH TEMPLE!",
                            "TAKE THE NORTH PASS CAREFULLY AND RESTORE BALANCE."
                        });
                    } else {
                        trigger_dialogue("TAVERN JUMO", {
                            "YOU RESTED WELL! HP AND QI ARE FULLY RESTORED.",
                            "MAY THE ANCESTRAL SPIRITS PROTECT YOUR JOURNEY."
                        });
                    }
                    std::snprintf(notice_msg, sizeof(notice_msg), "RESTED AT INN: FULL HP AND QI RECOVERED");
                    render(); InvalidateRect(hwnd, nullptr, FALSE); return 0;
                } else if (npc == 2) { // Ancient Shrine
                    world_state.mark_shrine_seen();
                    artifact_owned = true;
                    trigger_dialogue("ANCIENT SHRINE", {
                        "ANCIENT SHRINE OF THE SPIRIT REALM...",
                        "YOU RECEIVED [SHRINE SHARD] (+3 ATK / -2 HP DRAWBACK)!",
                        "PURIFY THE CORRUPTED SECT TO HEAL THE LAND."
                    });
                    std::snprintf(notice_msg, sizeof(notice_msg), "OBTAINED ARTIFACT: SHRINE SHARD");
                    render(); InvalidateRect(hwnd, nullptr, FALSE); return 0;
                } else if (npc == 3) { // Mountain Signpost
                    trigger_dialogue("MOUNTAIN SIGNPOST", {
                        "--- BUKHANSAN PASS GUIDANCE ---",
                        "NORTH: DAEWOONGJEON TEMPLE (HAUNTED BY YIN-YANG MONK)",
                        "SOUTH: DOSEONSA VILLAGE EDGE (INN & SHRINE)"
                    });
                    render(); InvalidateRect(hwnd, nullptr, FALSE); return 0;
                } else if (npc == 4) { // Corrupt Monk Myogak Boss
                    if (!world_state.boss_defeated) {
                        trigger_dialogue("CORRUPT MONK MYOGAK", {
                            "YOU DARE INTRUDE UPON OUR DARK FORBIDDEN RITUAL?!",
                            "THE YIN ENERGY SHALL CONSUME YOUR FLESH AND SOUL!",
                            "PREPARE TO PERISH IN THE SHADOWS!"
                        });
                        begin_battle(true); // Trigger Boss Battle!
                    } else {
                        trigger_dialogue("PURIFIED TEMPLE", {
                            "THE CORRUPTED INCENSE HAS BURNED AWAY.",
                            "THE TEMPLE OF DAEWOONGJEON IS AT PEACE ONCE MORE."
                        });
                    }
                    render(); InvalidateRect(hwnd, nullptr, FALSE); return 0;
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
