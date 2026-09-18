#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(USE_SDL2)
#include <SDL.h>
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
#include "korean_font.h"
#include "generated_sprites.h"

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

char enemy_name[64] = "도깨비";
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
char notice_msg[128] = "도선사 주변을 탐험하며 주막 주모와 대화하십시오.";
char battle_msg[128] = "야생 도깨비가 나타났다!";

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
    korean_font::draw_utf8_text(pixels.data(), W, H, s, x, y, scale, color);
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
    std::snprintf(notice_msg, sizeof(notice_msg), "슬롯 %d에 정상 저장되었습니다.", slot);
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
        std::snprintf(notice_msg, sizeof(notice_msg), "슬롯 %d에서 진행 데이터를 불러왔습니다.", slot);
        return true;
    }
    std::snprintf(notice_msg, sizeof(notice_msg), "슬롯 %d 데이터가 비어있거나 손상되었습니다.", slot);
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
        std::strncpy(enemy_name, "괴승 묘각", sizeof(enemy_name));
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
        std::snprintf(battle_msg, sizeof(battle_msg), "보스 전투: 음양당 괴승 묘각이 덤벼든다!");
    } else {
        std::strncpy(enemy_name, "도깨비", sizeof(enemy_name));
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
        std::snprintf(battle_msg, sizeof(battle_msg), "야생 도깨비가 나타났다! 행동을 선택하라.");
    }
}

void finish_battle(bool victory) {
    battle = false;
    battle_state.active = false;
    if (victory) {
        if (is_boss_battle) {
            world_state.main_quest_step = 2;
            world_state.boss_defeated = true;
            std::snprintf(notice_msg, sizeof(notice_msg), "챕터 1 완료! 음양당 괴승을 처치하고 성지를 정화했습니다!");
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
    const std::string yokai_raw = data::Registry::load_text("data/yokai_001_dokkaebi.json");
    if (!yokai_raw.empty()) {
        auto ydef = data::Registry::parse_yokai(yokai_raw);
        enemy_hp = ydef.hp;
        enemy_max_hp = ydef.hp;
        enemy_attack = ydef.attack;
        enemy_defense = ydef.defense;
        enemy_speed = ydef.speed;
    }

    const char* skill_files[4] = {
        "data/skill_001_basic_strike.json",
        "data/skill_002_ember.json",
        "data/skill_003_frost.json",
        "data/skill_004_seal.json"
    };
    const char* default_names[4] = {"STRIKE", "EMBER", "FROST", "SEAL"};
    const char* default_names_ko[4] = {"기본 공격", "잔불 부적", "서리 숨결", "봉인 문양"};
    int default_pwrs[4] = {20, 8, 6, 4};
    int default_qis[4] = {0, 3, 4, 5};
    int default_accs[4] = {100, 95, 90, 85};
    Status default_stats[4] = {Status::None, Status::Burn, Status::Freeze, Status::Seal};
    int default_seals[4] = {0, 0, 0, 1};

    for (int i = 0; i < 4; ++i) {
        std::string raw = data::Registry::load_text(skill_files[i]);
        if (!raw.empty()) {
            auto sdef = data::Registry::parse_skill(raw);
            skill_state.configure_slot(i, i + 1, default_names[i], sdef.power, sdef.qi_cost, sdef.accuracy, sdef.status, sdef.seal_turns, default_names_ko[i]);
        } else {
            skill_state.configure_slot(i, i + 1, default_names[i], default_pwrs[i], default_qis[i], default_accs[i], default_stats[i], default_seals[i], default_names_ko[i]);
        }
    }

    const std::string enc_raw = data::Registry::load_text("data/encounter_village_edge.json");
    if (!enc_raw.empty()) {
        enemy_level = data::Registry::integer(enc_raw, "min_level", enemy_level);
        enemy_max_hp = 80 + enemy_level * 5;
        enemy_hp = enemy_max_hp;
    }

    const std::string art_raw = data::Registry::load_text("data/artifact_001_shrine_shard.json");
    if (!art_raw.empty()) {
        auto adef = data::Registry::parse_artifact(art_raw);
        artifact_attack_bonus = adef.attack_bonus;
        artifact_battle_cost = adef.hp_loss_per_battle;
    }

    const std::string stat_raw = data::Registry::load_text("data/status_effects.json");
    if (!stat_raw.empty()) {
        burn_damage = data::Registry::integer(stat_raw, "damage_per_turn", burn_damage);
        fear_multiplier = data::Registry::number(stat_raw, "damage_taken_multiplier", fear_multiplier);
    }

    collection_state.add_contract(1);
}

void render_overworld() {
    int cur_map = world_state.region_id;

    for (int ty = 0; ty < H / TILE; ++ty) {
        for (int tx = 0; tx < W / TILE; ++tx) {
            if (cur_map == 1) {
                bool path = (tx >= 13 && tx <= 16) || (ty >= 6 && ty <= 8);
                art::draw_rle_tile(pixels.data(), W, H, path ? art::SPRITE_tile_village_path : art::SPRITE_tile_village_grass, tx * TILE, ty * TILE);
            } else if (cur_map == 2) {
                bool path = (tx >= 12 && tx <= 17) && (ty >= 2 && ty <= 14);
                art::draw_rle_tile(pixels.data(), W, H, path ? art::SPRITE_tile_village_path : art::SPRITE_tile_mountain_rock, tx * TILE, ty * TILE);
            } else if (cur_map == 3) {
                bool courtyard = (tx >= 6 && tx <= 23 && ty >= 5 && ty <= 13);
                art::draw_rle_tile(pixels.data(), W, H, courtyard ? art::SPRITE_tile_temple_stone : art::SPRITE_tile_mountain_rock, tx * TILE, ty * TILE);
            }
        }
    }

    if (cur_map == 1) {
        fill_rect(6 * TILE, 4 * TILE, 5 * TILE, 2 * TILE, 0xFF6B4226u);
        stroke_rect(6 * TILE, 4 * TILE, 5 * TILE, 2 * TILE, 2, 0xFFD9C47Au);
        text("주막", 7 * TILE, 4 * TILE + 10, 2, 0xFFFFD700u);

        art::draw_rle_sprite(pixels.data(), W, H, art::SPRITE_jumo_field, 8 * TILE, 6 * TILE);
        text("주모", 8 * TILE + 4, 6 * TILE - 12, 1, 0xFFFFFFFFu);

        art::draw_rle_sprite(pixels.data(), W, H, art::SPRITE_shrine_field, 20 * TILE, 7 * TILE);
        text("고대 신목", 19 * TILE + 4, 7 * TILE - 12, 1, 0xFFE8D8A0u);

        stroke_rect(13 * TILE, 2 * TILE, 4 * TILE, 8, 2, 0xFFFFD700u);
        text("북한산 고갯길 ^", 13 * TILE, 2 * TILE + 12, 1, 0xFFFFD700u);
    } else if (cur_map == 2) {
        art::draw_rle_sprite(pixels.data(), W, H, art::SPRITE_signpost_field, 13 * TILE, 8 * TILE);
        text("안내판", 13 * TILE + 2, 8 * TILE - 10, 1, 0xFFFFFFFFu);

        text("도선사 대웅전 ^", 12 * TILE, 2 * TILE + 12, 1, 0xFFFFD700u);
        text("도선사 마을 v", 13 * TILE, 14 * TILE, 1, 0xFFFFD700u);
    } else if (cur_map == 3) {
        fill_rect(8 * TILE, 3 * TILE, 14 * TILE, 3 * TILE, 0xFF7A2020u);
        stroke_rect(8 * TILE, 3 * TILE, 14 * TILE, 3 * TILE, 3, 0xFFFFD700u);
        text("도선사 대웅전 경내", 9 * TILE, 3 * TILE + 16, 2, 0xFFFFD700u);

        if (!world_state.boss_defeated) {
            art::draw_rle_sprite(pixels.data(), W, H, art::SPRITE_monk_field, 15 * TILE, 6 * TILE);
            text("괴승 묘각 (보스)", 13 * TILE + 16, 6 * TILE - 14, 1, 0xFFFF5555u);
        } else {
            text("[성지 정화 완료]", 13 * TILE + 16, 6 * TILE + 8, 1, 0xFF7DDA72u);
        }

        text("북한산 고갯길 v", 13 * TILE, 14 * TILE, 1, 0xFFFFD700u);
    }

    int px = world_state.player_x * TILE;
    int py = world_state.player_y * TILE;
    art::draw_rle_sprite(pixels.data(), W, H, art::SPRITE_hero_field, px, py);

    fill_rect(0, 0, W, 48, 0xDD121B14u);
    stroke_rect(0, 0, W, 48, 2, 0xFF3D5A3Du);

    char hud_buf[128];
    std::snprintf(hud_buf, sizeof(hud_buf), "체력:%d/%d", player_hp, player_max_hp);
    text(hud_buf, 16, 10, 2, 0xFF7DDA72u);
    fill_rect(130, 12, 110, 10, 0xFF1E2D20u);
    fill_rect(130, 12, (player_hp * 110) / player_max_hp, 10, 0xFF7DDA72u);

    std::snprintf(hud_buf, sizeof(hud_buf), "영력:%d/%d", player_qi, player_max_qi);
    text(hud_buf, 16, 26, 2, 0xFF6EB8EAu);
    fill_rect(130, 28, 110, 10, 0xFF1E2D20u);
    fill_rect(130, 28, (player_qi * 110) / player_max_qi, 10, 0xFF6EB8EAu);

    const char* kr_map_name = (cur_map == 1) ? "도선사 마을" : (cur_map == 2 ? "북한산 고갯길" : "도선사 대웅전");
    std::snprintf(hud_buf, sizeof(hud_buf), "레벨.%d 경험치:%d [%s]", player_level, player_xp, kr_map_name);
    text(hud_buf, 260, 16, 2, 0xFFE8D8A0u);

    text("동행 요괴:", 640, 16, 2, 0xFFE8D8A0u);
    for (int i = 0; i < 3; ++i) {
        int sx = 740 + i * 42;
        bool has_member = (i < collection_state.party_count);
        fill_rect(sx, 10, 32, 28, has_member ? 0xFF8A5D3Bu : 0xFF243026u);
        stroke_rect(sx, 10, 32, 28, 2, 0xFFE8D8A0u);
        if (has_member) text("도", sx + 10, 16, 1, 0xFFE8D8A0u);
    }

    if (artifact_owned) {
        text("[신목의 파편]", 865, 16, 1, 0xFFB4A6D8u);
    }

    fill_rect(0, 48, W, 22, 0xAA0C1410u);
    text(notice_msg, 20, 52, 1, 0xFFFFD700u);

    fill_rect(0, H - 28, W, 28, 0xEE121B14u);
    text("[방향키] 이동  [ENTER/Z] 대화/조사  [M] 메뉴/도감  [B] 전투  [1-3] 슬롯  [S] 저장  [L] 불러오기", 20, H - 20, 2, 0xFF9EBAA0u);
}

void render_battle() {
    fill_rect(40, 20, W - 80, H - 40, 0xFF0E161Cu);
    stroke_rect(40, 20, W - 80, H - 40, 4, is_boss_battle ? 0xFF8A2020u : 0xFF3D5A40u);

    fill_rect(540, 50, 340, 140, 0xFF182428u);
    stroke_rect(540, 50, 340, 140, 2, is_boss_battle ? 0xFFB83020u : 0xFF4A6860u);

    char enemy_info[64];
    std::snprintf(enemy_info, sizeof(enemy_info), "%s  레벨.%d", enemy_name, enemy_level);
    text(enemy_info, 560, 68, 2, is_boss_battle ? 0xFFFF7777u : 0xFFE8D8A0u);

    char hp_str[32];
    std::snprintf(hp_str, sizeof(hp_str), "체력: %d/%d", enemy_hp, enemy_max_hp);
    text(hp_str, 560, 96, 2, 0xFF7DDA72u);
    fill_rect(560, 120, 220, 14, 0xFF28352Au);
    int enemy_bar_w = (std::max)(0, (enemy_hp * 220) / enemy_max_hp);
    fill_rect(560, 120, enemy_bar_w, 14, 0xFFE26A5Au);

    if (enemy_status != Status::None) {
        text(status_rules::short_tag_ko(enemy_status), 790, 118, 2, 0xFFFFCC00u);
    }

    if (is_boss_battle) {
        art::draw_rle_sprite(pixels.data(), W, H, art::SPRITE_boss_myogak_battle, 230, 65, 2);
    } else {
        art::draw_rle_sprite(pixels.data(), W, H, art::SPRITE_dokkaebi_battle, 230, 65, 2);
    }

    fill_rect(80, 180, 340, 130, 0xFF182428u);
    stroke_rect(80, 180, 340, 130, 2, 0xFF4A6860u);

    text("도깨비 (아군 요괴)", 100, 195, 2, 0xFFE8D8A0u);
    std::snprintf(hp_str, sizeof(hp_str), "체력: %d/%d", player_hp, player_max_hp);
    text(hp_str, 100, 220, 2, 0xFF7DDA72u);
    fill_rect(100, 240, 200, 12, 0xFF28352Au);
    int player_bar_w = (std::max)(0, (player_hp * 200) / player_max_hp);
    fill_rect(100, 240, player_bar_w, 12, 0xFF7DDA72u);

    std::snprintf(hp_str, sizeof(hp_str), "영력: %d/%d", player_qi, player_max_qi);
    text(hp_str, 100, 260, 2, 0xFF6EB8EAu);
    fill_rect(100, 280, 200, 12, 0xFF28352Au);
    int qi_bar_w = (std::max)(0, (player_qi * 200) / player_max_qi);
    fill_rect(100, 280, qi_bar_w, 12, 0xFF6EB8EAu);

    art::draw_rle_sprite(pixels.data(), W, H, art::SPRITE_dokkaebi_ally_battle, 625, 195, 2);

    fill_rect(80, 330, W - 160, 36, 0xFF1E2E28u);
    stroke_rect(80, 330, W - 160, 36, 2, 0xFF6E8D62u);
    text(battle_msg, 100, 340, 2, 0xFFFFFFFFu);

    const int cmd_w = 124, cmd_h = 44, start_x = 80, cmd_y = 380;
    int current_cmd = static_cast<int>(battle_command);

    for (int i = 0; i < 6; ++i) {
        int cx = start_x + i * 134;
        bool selected = (i == current_cmd);
        fill_rect(cx, cmd_y, cmd_w, cmd_h, selected ? 0xFF3D5A48u : 0xFF1A2620u);
        stroke_rect(cx, cmd_y, cmd_w, cmd_h, selected ? 3 : 1, selected ? 0xFFFFD700u : 0xFF4D6A58u);

        const char* label = battle_ui::command_label_ko(i, skill_state);
        text(label, cx + 16, cmd_y + 14, 2, selected ? 0xFFFFD700u : 0xFFD8D2C2u);
    }

    fill_rect(80, 436, W - 160, 54, 0xFF121C18u);
    stroke_rect(80, 436, W - 160, 54, 1, 0xFF3D5A48u);

    int cap_rate = battle::capture_rate_percent(enemy_hp, enemy_max_hp, enemy_status != Status::None);
    char detail_buf[128];
    battle_ui::command_detail_ko(current_cmd, skill_state, player_qi, enemy_hp, enemy_max_hp, cap_rate, detail_buf, sizeof(detail_buf));
    text(detail_buf, 100, 452, 2, 0xFFE8D8A0u);

    text("[좌우 방향키] 명령 선택    [ENTER/Z] 실행    [ESC] 도주/포기", 100, 500, 2, 0xFF7D9A82u);
}

void render_menu() {
    fill_rect(40, 20, W - 80, H - 40, 0xFF142018u);
    stroke_rect(40, 20, W - 80, H - 40, 4, 0xFF7D9A62u);

    text("=== 파티, 요괴도감 및 저장 슬롯 ===", 80, 36, 3, 0xFFFFD700u);

    text("동행 요괴 (최대 3마리):", 80, 75, 2, 0xFFE8D8A0u);
    for (int i = 0; i < 3; ++i) {
        int y = 98 + i * 36;
        fill_rect(80, y, 400, 30, 0xFF1E2D22u);
        stroke_rect(80, y, 400, 30, 1, 0xFF4D6A58u);
        if (i < collection_state.party_count) {
            char pbuf[128];
            std::snprintf(pbuf, sizeof(pbuf), "슬롯 %d: 도깨비 #%03d  레벨.%d  체력:%d/%d",
                i + 1, collection_state.party_ids[i], player_level, player_hp, player_max_hp);
            text(pbuf, 95, y + 8, 2, 0xFFFFFFFFu);
        } else {
            char pbuf[64];
            std::snprintf(pbuf, sizeof(pbuf), "슬롯 %d: [빈 슬롯]", i + 1);
            text(pbuf, 95, y + 8, 2, 0xFF6E8D72u);
        }
    }

    text("저장 슬롯 (1-3번 키 선택, S 저장, L 불러오기):", 510, 75, 2, 0xFFE8D8A0u);
    for (int i = 1; i <= 3; ++i) {
        int y = 98 + (i - 1) * 62;
        bool selected = (save_slot == i);
        fill_rect(510, y, 370, 54, selected ? 0xFF2B4232u : 0xFF1A2620u);
        stroke_rect(510, y, 370, 54, selected ? 2 : 1, selected ? 0xFFFFD700u : 0xFF4D6A58u);

        SaveData s{};
        if (inspect_slot(i, s)) {
            char sbuf[128];
            std::snprintf(sbuf, sizeof(sbuf), "슬롯 %d: 레벨.%d  체력:%d  동료:%d마리", i, s.level, s.hp, s.party);
            text(sbuf, 525, y + 10, 2, selected ? 0xFFFFD700u : 0xFFFFFFFFu);
            const char* rname = (s.region_id == 1) ? "도선사 마을" : (s.region_id == 2 ? "북한산 고갯길" : "대웅전");
            std::snprintf(sbuf, sizeof(sbuf), "위치: %s  임무:%d/2단계", rname, s.quest_step);
            text(sbuf, 525, y + 30, 2, 0xFF9EBAA0u);
        } else {
            char sbuf[64];
            std::snprintf(sbuf, sizeof(sbuf), "슬롯 %d: [데이터 없음 / 빈 슬롯]", i);
            text(sbuf, 525, y + 18, 2, 0xFF6E8D72u);
        }
    }

    text("108종 요괴도감 수집 현황:", 80, 220, 2, 0xFFE8D8A0u);
    char codex_buf[128];
    std::snprintf(codex_buf, sizeof(codex_buf), "발견: %d / 108    계약: %d / 108",
        collection_state.discovered_count(), collection_state.contracted_count());
    text(codex_buf, 80, 245, 2, 0xFF7DDA72u);

    fill_rect(80, 270, 400, 14, 0xFF28352Au);
    int codex_w = (collection_state.discovered_count() * 400) / 108;
    fill_rect(80, 270, codex_w, 14, 0xFF7DDA72u);

    text("현재 진행 임무:", 80, 305, 2, 0xFFE8D8A0u);
    if (world_state.main_quest_step == 0) {
        text("제 1 장: 도선사 어귀 마을의 주모를 찾아 대화하라", 80, 330, 2, 0xFFFFCC00u);
    } else if (world_state.main_quest_step == 1) {
        text("제 1 장: 북쪽 고갯길을 넘어 대웅전의 괴승 묘각을 격파하라", 80, 330, 2, 0xFFFFCC00u);
    } else {
        text("제 1 장 완료: 음양당 세력을 몰아내고 성지를 정화했습니다!", 80, 330, 2, 0xFF7DDA72u);
    }

    text("장착된 유물:", 80, 375, 2, 0xFFE8D8A0u);
    if (artifact_owned) {
        text("[신목의 파편] (완력+3 / 전투 시 체력-2)  [D키로 즉시 파괴]", 80, 400, 2, 0xFFB4A6D8u);
    } else {
        text("장착된 유물 없음 (고대 신목 비석에서 획득 가능)", 80, 400, 2, 0xFF6E8D72u);
    }

    fill_rect(80, 440, W - 160, 30, 0xFF0F1812u);
    text(notice_msg, 100, 448, 2, 0xFFFFD700u);

    text("[1-3] 슬롯 선택   [S] 저장   [L] 불러오기   [D] 유물 파괴   [ESC / M] 복귀", 80, 485, 2, 0xFF9EBAA0u);
}

void render_dialogue() {
    fill_rect(60, H - 160, W - 120, 130, 0xEE142018u);
    stroke_rect(60, H - 160, W - 120, 130, 3, 0xFFE8D8A0u);

    char title_buf[128];
    std::snprintf(title_buf, sizeof(title_buf), "--- %s ---", dialogue_speaker);
    text(title_buf, 90, H - 145, 2, 0xFFFFD700u);
    text(dialogue_pages[dialogue_page], 90, H - 110, 2, 0xFFFFFFFFu);
    text("[ENTER / Z] 계속", 90, H - 65, 2, 0xFF7DDA72u);
}

void render() {
    render_overworld();
    if (dialogue) render_dialogue();
    if (battle) render_battle();
    if (menu) render_menu();
}

enum AppKey {
    KEY_NONE = 0,
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_CONFIRM, KEY_CANCEL,
    KEY_MENU, KEY_BATTLE,
    KEY_SLOT1, KEY_SLOT2, KEY_SLOT3,
    KEY_SAVE, KEY_LOAD, KEY_DESTROY
};

void save_screenshot_bmp(const char* filepath, const std::uint32_t* fb, int w, int h) {
    std::ofstream f(filepath, std::ios::binary);
    if (!f.is_open()) return;
    std::uint32_t file_size = 54 + w * h * 4;
    std::uint32_t offset = 54;
    std::uint32_t header_size = 40;
    std::uint16_t planes = 1, bpp = 32;
    std::uint32_t zero = 0;
    f.put('B').put('M');
    f.write(reinterpret_cast<const char*>(&file_size), 4);
    f.write(reinterpret_cast<const char*>(&zero), 4);
    f.write(reinterpret_cast<const char*>(&offset), 4);
    f.write(reinterpret_cast<const char*>(&header_size), 4);
    f.write(reinterpret_cast<const char*>(&w), 4);
    int neg_h = -h;
    f.write(reinterpret_cast<const char*>(&neg_h), 4);
    f.write(reinterpret_cast<const char*>(&planes), 2);
    f.write(reinterpret_cast<const char*>(&bpp), 2);
    f.write(reinterpret_cast<const char*>(&zero), 4);
    f.write(reinterpret_cast<const char*>(&zero), 4);
    f.write(reinterpret_cast<const char*>(&zero), 4);
    f.write(reinterpret_cast<const char*>(&zero), 4);
    f.write(reinterpret_cast<const char*>(&zero), 4);
    f.write(reinterpret_cast<const char*>(&zero), 4);
    f.write(reinterpret_cast<const char*>(fb), w * h * 4);
}

void handle_input_key(AppKey k) {
    if (k == KEY_CANCEL) {
        if (dialogue) { dialogue = false; return; }
        if (menu) { menu = false; return; }
        if (battle) { finish_battle(false); return; }
        running = false;
        return;
    }

    if (menu) {
        if (k == KEY_SLOT1) { save_slot = 1; return; }
        if (k == KEY_SLOT2) { save_slot = 2; return; }
        if (k == KEY_SLOT3) { save_slot = 3; return; }
        if (k == KEY_SAVE) { save_game(save_slot); return; }
        if (k == KEY_LOAD) { load_game(save_slot); return; }
        if (k == KEY_DESTROY && artifact_owned) {
            artifact_owned = false;
            std::snprintf(notice_msg, sizeof(notice_msg), "유물을 파괴하여 인벤토리를 비웠습니다.");
            return;
        }
        if (k == KEY_MENU || k == KEY_CONFIRM) { menu = false; return; }
        return;
    }

    if (dialogue) {
        if (k == KEY_CONFIRM) {
            if (++dialogue_page >= current_dialogue_count) {
                dialogue = false;
                dialogue_page = 0;
            }
        }
        return;
    }

    if (battle) {
        if (k == KEY_LEFT || k == KEY_RIGHT) {
            int cur = static_cast<int>(battle_command);
            int dir = (k == KEY_RIGHT) ? 1 : -1;
            battle_command = static_cast<battle::Command>(battle_ui::move_command(cur, dir, 6));
            return;
        }

        if (k == KEY_CONFIRM) {
            int cmd = static_cast<int>(battle_command);
            if (cmd == 5) { // Run
                if (is_boss_battle) {
                    std::snprintf(battle_msg, sizeof(battle_msg), "보스 전투에서는 도주할 수 없습니다!");
                } else {
                    std::snprintf(battle_msg, sizeof(battle_msg), "전투에서 무사히 도주했습니다!");
                    finish_battle(false);
                }
                return;
            }

            if (cmd == 4) { // Capture
                if (is_boss_battle) {
                    std::snprintf(battle_msg, sizeof(battle_msg), "오염된 보스 요괴는 계약할 수 없습니다!");
                } else {
                    int cap_rate = battle::capture_rate_percent(enemy_hp, enemy_max_hp, enemy_status != Status::None);
                    if (battle::can_capture(enemy_hp, enemy_max_hp, enemy_turns, enemy_status != Status::None)) {
                        collection_state.discover(1);
                        collection_state.add_contract(1);
                        std::snprintf(battle_msg, sizeof(battle_msg), "계약 성공! 도깨비가 동료로 합류했다!");
                        finish_battle(true);
                    } else {
                        std::snprintf(battle_msg, sizeof(battle_msg), "계약 실패(%d%% 성공률)! 도깨비가 저항했다!", cap_rate);
                    }
                }
            } else if (cmd >= 0 && cmd < 4) {
                if (!skill_state.usable(cmd, player_qi)) {
                    if (skill_state.sealed_turns[cmd] > 0) {
                        std::snprintf(battle_msg, sizeof(battle_msg), "사용 불가: 해당 기술은 현재 봉인되어 있습니다!");
                    } else {
                        std::snprintf(battle_msg, sizeof(battle_msg), "사용 불가: 영력이 부족합니다!");
                    }
                    return;
                }

                player_qi -= skill_state.qi_costs[cmd];

                int pwr = skill_state.powers[cmd] + (artifact_owned ? artifact_attack_bonus : 0);
                int dmg = battle::damage(
                    {player_attack, player_defense, player_hp, player_qi, player_speed},
                    {enemy_attack, enemy_defense, enemy_hp, 0, enemy_speed},
                    pwr, false, enemy_status == Status::Fear
                );
                enemy_hp -= dmg;
                enemy_status = status_rules::on_hit_effect(enemy_status);

                Status st = skill_state.statuses[cmd];
                if (st != Status::None) {
                    enemy_status = st;
                    enemy_status_turns = status_rules::default_duration(st);
                }

                if (skill_state.seal_duration[cmd] > 0) {
                    skill_state.seal(cmd, skill_state.seal_duration[cmd]);
                }

                std::snprintf(battle_msg, sizeof(battle_msg), "%s 사용! 적에게 %d의 피해를 입혔다!",
                    skill_state.names_ko[cmd], dmg);
            }

            if (enemy_hp <= 0) {
                std::snprintf(battle_msg, sizeof(battle_msg), "%s 격파 승리! %d 경험치 획득!",
                    enemy_name, enemy_level * 25);
                finish_battle(true);
                return;
            }

            ++enemy_turns;
            skill_state.tick();

            bool enemy_skips = status_rules::skips_action(enemy_status, enemy_turns);
            if (enemy_skips) {
                std::snprintf(battle_msg, sizeof(battle_msg), "적은 %s 상태여서 행동하지 못했다!",
                    status_rules::name_ko(enemy_status));
            } else {
                int edmg = battle::damage(
                    {enemy_attack, enemy_defense, enemy_hp, 0, enemy_speed},
                    {player_attack, player_defense, player_hp, player_qi, player_speed},
                    is_boss_battle ? 10 : 5, enemy_status == Status::Fear, false
                );
                player_hp -= edmg;
            }

            if (enemy_hp > 0 && enemy_status == Status::Burn) {
                enemy_hp -= burn_damage;
            }

            if (artifact_owned && player_hp > 0) {
                player_hp -= artifact_battle_cost;
            }

            if (enemy_status_turns > 0) {
                --enemy_status_turns;
                if (enemy_status_turns == 0) enemy_status = Status::None;
            }

            if (player_hp <= 0) {
                player_hp = player_max_hp;
                world_state.change_map(1, 14, 8);
                std::snprintf(notice_msg, sizeof(notice_msg), "기력을 다하여 도선사 주막으로 후송되었습니다.");
                finish_battle(false);
            }
            return;
        }
        return;
    }

    if (!dialogue && !battle && !menu) {
        if (k == KEY_MENU) { menu = true; return; }
        if (k == KEY_BATTLE) { begin_battle(false); return; }
        if (k == KEY_SAVE) { save_game(save_slot); return; }
        if (k == KEY_LOAD) { load_game(save_slot); return; }
        if (k == KEY_SLOT1) { save_slot = 1; std::snprintf(notice_msg, sizeof(notice_msg), "저장 슬롯 1번이 선택되었습니다."); return; }
        if (k == KEY_SLOT2) { save_slot = 2; std::snprintf(notice_msg, sizeof(notice_msg), "저장 슬롯 2번이 선택되었습니다."); return; }
        if (k == KEY_SLOT3) { save_slot = 3; std::snprintf(notice_msg, sizeof(notice_msg), "저장 슬롯 3번이 선택되었습니다."); return; }

        int nx = world_state.player_x, ny = world_state.player_y;
        if (k == KEY_LEFT) --nx;
        if (k == KEY_RIGHT) ++nx;
        if (k == KEY_UP) --ny;
        if (k == KEY_DOWN) ++ny;

        if (k == KEY_LEFT || k == KEY_RIGHT || k == KEY_UP || k == KEY_DOWN) {
            int target_map = 0, target_x = 0, target_y = 0;
            if (world_state.check_portal(nx, ny, target_map, target_x, target_y)) {
                world_state.change_map(target_map, target_x, target_y);
                const char* kname = (target_map == 1) ? "도선사 마을" : (target_map == 2 ? "북한산 고갯길" : "도선사 대웅전");
                std::snprintf(notice_msg, sizeof(notice_msg), "[%s]에 도착했습니다.", kname);
                return;
            }

            if (!world_state.is_blocked(nx, ny)) {
                world_state.player_x = nx;
                world_state.player_y = ny;
                ++steps;
                int rate = world_state.map_encounter_rate();
                if (rate > 0 && (steps % rate == 0)) {
                    begin_battle(false);
                }
                return;
            }
        }

        if (k == KEY_CONFIRM) {
            int npc = world_state.check_npc_interaction(world_state.player_x, world_state.player_y);
            if (npc == 1) { // Tavern Jumo
                player_hp = player_max_hp;
                player_qi = player_max_qi;
                if (world_state.main_quest_step == 0) {
                    world_state.advance_quest();
                    trigger_dialogue("주막 주모", {
                        "주막에 오신 것을 환영하오! 편히 쉬어가시게.",
                        "음양당의 요기가 북쪽 도선사에 번지고 있소!",
                        "북쪽 고갯길을 조심히 넘어 균형을 되찾아주시오."
                    });
                } else {
                    trigger_dialogue("주막 주모", {
                        "체력과 영력이 모두 회복되었소.",
                        "조상의 영령이 그대의 앞길을 지켜줄 것이오."
                    });
                }
                std::snprintf(notice_msg, sizeof(notice_msg), "주막 휴식: 체력과 영력이 완전 회복되었습니다.");
                return;
            } else if (npc == 2) { // Ancient Shrine
                world_state.mark_shrine_seen();
                artifact_owned = true;
                trigger_dialogue("고대 신목", {
                    "영맥의 균열로 흩어진 요괴들이 날뛰고 있구나...",
                    "[신목의 파편] 유물을 획득했다! (완력+3 / 전투 체력-2)",
                    "사악한 음양학파를 저지하여 대지를 치유하라."
                });
                std::snprintf(notice_msg, sizeof(notice_msg), "고대 유물 [신목의 파편]을 획득했습니다.");
                return;
            } else if (npc == 3) { // Mountain Signpost
                trigger_dialogue("고갯길 안내판", {
                    "--- 북한산 고갯길 안내 ---",
                    "북쪽: 도선사 대웅전 (음양당 괴승 점거)",
                    "남쪽: 도선사 어귀 마을 (주막 및 고대 신목)"
                });
                return;
            } else if (npc == 4) { // Corrupt Monk Myogak Boss
                if (!world_state.boss_defeated) {
                    trigger_dialogue("괴승 묘각", {
                        "감히 음양당의 금지된 음기 도술을 방해하려 들다니!",
                        "음기의 영맥으로 너의 혼을 삼켜주마!",
                        "어둠의 심연 속으로 떨어져라!"
                    });
                    begin_battle(true);
                } else {
                    trigger_dialogue("도선사 대웅전", {
                        "사악한 요기가 물러가고 대웅전이 정화되었습니다.",
                        "조선의 영맥이 조금씩 안정을 되찾고 있습니다."
                    });
                }
                return;
            }
        }
    }
}

#ifdef _WIN32
LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_KEYDOWN) {
        AppKey k = KEY_NONE;
        switch (wp) {
            case VK_ESCAPE: k = KEY_CANCEL; break;
            case VK_LEFT: k = KEY_LEFT; break;
            case VK_RIGHT: k = KEY_RIGHT; break;
            case VK_UP: k = KEY_UP; break;
            case VK_DOWN: k = KEY_DOWN; break;
            case VK_RETURN:
            case 'Z': k = KEY_CONFIRM; break;
            case 'M': k = KEY_MENU; break;
            case 'B': k = KEY_BATTLE; break;
            case 'S': k = KEY_SAVE; break;
            case 'L': k = KEY_LOAD; break;
            case 'D': k = KEY_DESTROY; break;
            case '1': k = KEY_SLOT1; break;
            case '2': k = KEY_SLOT2; break;
            case '3': k = KEY_SLOT3; break;
            default: break;
        }
        if (k != KEY_NONE) {
            handle_input_key(k);
            if (!running) PostQuitMessage(0);
            render();
            InvalidateRect(hwnd, nullptr, FALSE);
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

#if defined(USE_SDL2)
AppKey map_sdl_key(SDL_Keycode sym) {
    switch (sym) {
        case SDLK_UP: return KEY_UP;
        case SDLK_DOWN: return KEY_DOWN;
        case SDLK_LEFT: return KEY_LEFT;
        case SDLK_RIGHT: return KEY_RIGHT;
        case SDLK_RETURN:
        case SDLK_z: return KEY_CONFIRM;
        case SDLK_ESCAPE: return KEY_CANCEL;
        case SDLK_m: return KEY_MENU;
        case SDLK_b: return KEY_BATTLE;
        case SDLK_s: return KEY_SAVE;
        case SDLK_l: return KEY_LOAD;
        case SDLK_d: return KEY_DESTROY;
        case SDLK_1: return KEY_SLOT1;
        case SDLK_2: return KEY_SLOT2;
        case SDLK_3: return KEY_SLOT3;
        default: return KEY_NONE;
    }
}
#endif

void generate_snapshots() {
    render();
    save_screenshot_bmp("screenshot_overworld.bmp", pixels.data(), W, H);

    begin_battle(false);
    render();
    save_screenshot_bmp("screenshot_battle.bmp", pixels.data(), W, H);
    finish_battle(false);

    begin_battle(true);
    render();
    save_screenshot_bmp("screenshot_boss_battle.bmp", pixels.data(), W, H);
    finish_battle(false);

    menu = true;
    render();
    save_screenshot_bmp("screenshot_menu.bmp", pixels.data(), W, H);
    menu = false;

    render();
}

} // namespace

#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    init_game_data();
    generate_snapshots();

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
#elif defined(USE_SDL2)
int main(int, char*[]) {
    init_game_data();
    generate_snapshots();

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "108: \xEC\x9D\x8C\xEC\x96\x91\xEA\xB2\xAC\xEB\xAC\xB8\xEB\xA1\x9D (108: Yin-Yang Chronicle)",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        W * SCALE, H * SCALE,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );
    if (!window) {
        std::printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        W, H
    );

    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_KEYDOWN) {
                AppKey k = map_sdl_key(e.key.keysym.sym);
                if (k != KEY_NONE) {
                    handle_input_key(k);
                    render();
                }
            }
        }
        SDL_UpdateTexture(texture, nullptr, pixels.data(), W * sizeof(std::uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
#else
int main() {
    init_game_data();
    render();
    save_screenshot_bmp("screenshot_overworld.bmp", pixels.data(), W, H);
    return 0;
}
#endif
