#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#include <vector>
#include <fstream>
#include <string>
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

namespace {
constexpr int W = 960, H = 540, SCALE = 2, TILE = 32;
using Status = status_rules::Kind;
std::vector<std::uint32_t> pixels(W * H);
bool running = true;
bool dialogue = false;
bool battle = false;
 battle::State battle_state;
bool menu = false;
int save_slot = 1;
battle::Command battle_command = battle::Command::Skill1;
int player_hp = 100, enemy_hp = 80;
int player_qi = 30, skill_qi_cost = 0;
int player_level = 1, player_xp = 0;
int enemy_turns = 0;
int sealed_turns = 0;
int player_speed = 10, enemy_speed = 8, enemy_level = 1;
int fight_power = 20;
int skill_power[4] = {20, 8, 6, 4};
int skill_cost[4] = {0, 3, 4, 5};
int skill_accuracy[4] = {100, 95, 90, 85};
int skill_seal_turns[4] = {0, 0, 0, 1};
Status skill_status[4] = {Status::None, Status::Burn, Status::Freeze, Status::Seal};
int artifact_attack_bonus = 3, artifact_battle_cost = 2;
int burn_damage = 3;
double fear_multiplier = 1.5;
int party_count = 0;
bool dokkaebi_contracted = false;
bool artifact_owned = false;
int party_ids[3] = {0, 0, 0};
collection::State collection_state;
skills::State skill_state;
world::State world_state;
int steps = 0;
int encounter_interval = 12;
bool player_priority = true;
Status enemy_status = Status::Burn;
int enemy_status_turns = 0;
int status_duration(Status status) { switch(status){case Status::Burn:return 3; case Status::Freeze:return 2; case Status::Seal:return 1; case Status::Paralysis:return 4; case Status::Fear:return 3; default:return 0;} }
void begin_battle() { battle=true; battle_state.reset(); battle_state.active=true; battle_state.player_hp=player_hp; battle_state.player_qi=player_qi; battle_state.enemy_hp=enemy_hp; battle_state.enemy_max_hp=80 + enemy_level * 5; battle_state.enemy_level=enemy_level; battle_state.enemy_status=enemy_status; player_priority=player_speed>=enemy_speed; }
void finish_battle() { battle=false; battle_state.active=false; battle_state.player_hp=player_hp; battle_state.player_qi=player_qi; }
int dialogue_page = 0;
const char* dialogue_lines[] = {"YIN YANG", "A SPIRIT WAITS", "PRESS ENTER"};
constexpr int dialogue_line_count = 3;
int player_x = 15, player_y = 8;
bool blocked(int x, int y) { return x < 6 || x > 23 || y < 4 || y > 12; }
bool near_shrine() { return world_state.can_interact(player_x, player_y); }
struct SaveData { std::uint32_t magic, version; int x, y, hp, qi, level, xp, party, ids[3]; std::uint8_t contracted, artifact, shrine_seen; std::uint32_t checksum; };
std::uint32_t save_checksum(const SaveData& s) { return save_rules::checksum(s.version,s.x,s.y,s.hp,s.qi,s.level,s.xp,s.party,s.ids,s.contracted != 0,s.artifact != 0,s.shrine_seen != 0); }
std::string slot_file() { return "save_slot_" + std::to_string(save_slot) + ".sav"; }
void save_game() { SaveData s{save_rules::Magic, save_rules::Version, player_x, player_y, player_hp, player_qi, player_level, player_xp, party_count, {party_ids[0],party_ids[1],party_ids[2]}, static_cast<std::uint8_t>(dokkaebi_contracted), static_cast<std::uint8_t>(artifact_owned), static_cast<std::uint8_t>(world_state.shrine_event_seen), 0}; s.checksum=save_checksum(s); std::ofstream f(slot_file(), std::ios::binary); f.write(reinterpret_cast<const char*>(&s), sizeof(s)); }
void load_game() { SaveData s{}; std::ifstream f(slot_file(), std::ios::binary); if (f.read(reinterpret_cast<char*>(&s), sizeof(s)) && s.magic == save_rules::Magic && s.version == save_rules::Version && s.checksum == save_checksum(s) && s.x >= 0 && s.x < W / TILE && s.y >= 0 && s.y < H / TILE) { player_x=s.x; player_y=s.y; player_hp=s.hp; player_qi=s.qi; player_level=s.level; player_xp=s.xp; party_count=s.party; for(int i=0;i<3;++i) { party_ids[i]=s.ids[i]; collection_state.party_ids[i]=s.ids[i]; } collection_state.party_count=party_count; world_state.shrine_event_seen=s.shrine_seen != 0; dokkaebi_contracted=s.contracted != 0; artifact_owned=s.artifact != 0; } }
void glyph(char c, int ox, int oy, int scale) {
    static const char* rows[] = {"01110","10001","10001","11111","10001","10001","10001"};
    if (c == ' ') return;
    for (int y=0;y<7;++y) for (int x=0;x<5;++x) if (rows[y][x]=='1')
        for (int py=0;py<scale;++py) for (int px=0;px<scale;++px)
            pixels[(oy+y*scale+py)*W+ox+x*scale+px]=0xFFE8D8A0u;
}
void text(const char* s, int x, int y, int scale) { for (;*s;++s,x+=6*scale) glyph(*s,x,y,scale); }

void render() {
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            const std::uint8_t shade = static_cast<std::uint8_t>(18 + ((x / 32 + y / 32) & 1) * 8);
            pixels[y * W + x] = 0xFF000000u | (shade << 16) | (shade << 8) | (shade / 2);
        }
    }
    for (int ty = 0; ty < H / TILE; ++ty) for (int tx = 0; tx < W / TILE; ++tx) {
        const bool path = (tx > 12 && tx < 18) || (ty > 7 && ty < 11);
        const std::uint32_t color = path ? 0xFF7D9A62u : 0xFF294B35u;
        for (int py = 0; py < TILE; ++py) for (int px = 0; px < TILE; ++px)
            pixels[(ty * TILE + py) * W + tx * TILE + px] = color;
    }
    for (int py = 4; py < 28; ++py) for (int px = 7; px < 25; ++px)
        pixels[(player_y * TILE + py) * W + player_x * TILE + px] = 0xFFD9C47Au;
    for (int y=12;y<22;++y) for (int x=24;x<224;++x) pixels[y*W+x]=0xFF17251Bu;
    for (int y=12;y<22;++y) for (int x=24;x<24+player_hp*2;++x) pixels[y*W+x]=0xFFD9C47Au;
    for (int y=28;y<38;++y) for (int x=24;x<184;++x) pixels[y*W+x]=0xFF17251Bu;
    for (int y=28;y<38;++y) for (int x=24;x<24+player_qi*2;++x) pixels[y*W+x]=0xFF7D9A62u;
    for (int slot=0;slot<3;++slot) {
        const int sx=760+slot*48;
        for (int y=12;y<44;++y) for (int x=sx;x<sx+32;++x) pixels[y*W+x]=(slot<party_count)?0xFFD9C47Au:0xFF17251Bu;
    }
    if (dialogue) {
        for (int y = 390; y < 520; ++y) for (int x = 40; x < 920; ++x)
            pixels[y * W + x] = (x == 40 || x == 919 || y == 390 || y == 519) ? 0xFFE8D8A0u : 0xFF17251Bu;
        for (int y = 420; y < 424; ++y) for (int x = 80; x < 850; ++x) pixels[y * W + x] = 0xFF6E8D62u;
        text(dialogue_lines[dialogue_page], 80, 445, 4);
        text("ENTER", 80, 485, 3);
    }
    if (battle) {
        for (int y=80;y<460;++y) for (int x=80;x<880;++x) pixels[y*W+x]=0xFF101820u;
        for (int y=120;y<220;++y) for (int x=610;x<760;++x) pixels[y*W+x]=0xFF7D9A62u;
        for (int y=300;y<400;++y) for (int x=200;x<350;++x) pixels[y*W+x]=0xFFD9C47Au;
        text("BATTLE", 100, 100, 5);
        text("HP", 200, 270, 3);
        for (int y=285;y<297;++y) for (int x=200;x<400;++x) pixels[y*W+x]=0xFF28352Au;
        for (int y=285;y<297;++y) for (int x=200;x<200+player_hp*2;++x) pixels[y*W+x]=0xFFD9C47Au;
        text("HP", 610, 240, 3);
        for (int y=255;y<267;++y) for (int x=610;x<770;++x) pixels[y*W+x]=0xFF28352Au;
        for (int y=255;y<267;++y) for (int x=610;x<610+enemy_hp*2;++x) pixels[y*W+x]=0xFF7D9A62u;
        const std::uint32_t status_color = enemy_status == Status::Burn ? 0xFFB86B4Bu : (enemy_status == Status::Fear ? 0xFFB4A6D8u : 0xFF6E8D62u);
        for (int y=280;y<304;++y) for (int x=610;x<634;++x) pixels[y*W+x]=status_color;
        text("FIGHT", 100, 350, 3); text("CAPTURE", 260, 350, 3); text("RUN", 500, 350, 3);
        const int command_index = static_cast<int>(battle_command);
        for (int y=345;y<350;++y) for (int x=90+command_index*160;x<100+command_index*160;++x) pixels[y*W+x]=0xFFE8D8A0u;
        text("ENTER", 100, 430, 3);
    }
    if (menu) {
        for (int y=60;y<480;++y) for (int x=100;x<860;++x) pixels[y*W+x]=0xFF17251Bu;
        text("PARTY", 150, 100, 5);
        text("YIN YANG", 150, 180, 4);
        text("ENTER", 150, 400, 3);
    }
}

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_KEYDOWN) {
        if (wp == VK_ESCAPE) { if (dialogue) { dialogue = false; render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; } if (menu) { menu = false; render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; } running = false; PostQuitMessage(0); return 0; }
        if (menu && wp == VK_ESCAPE) { menu = false; render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
        if (menu && wp == VK_RETURN) { artifact_owned = false; render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
        if (!dialogue && !battle && wp == 'M') { menu = true; render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
        if (menu) return 0;
        if (!dialogue && !battle && wp == 'S') { save_game(); return 0; }
        if (!dialogue && !battle && wp == 'A') { artifact_owned = true; return 0; }
        if (!dialogue && !battle && wp == 'L') { load_game(); render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
        if (!dialogue && !battle && wp >= '1' && wp <= '3') { save_slot = static_cast<int>(wp - '0'); return 0; }
        if (battle && wp == VK_RETURN) {
            if (!player_priority && enemy_hp > 0 && player_hp > 0) player_hp -= 8;
            if (battle_command == battle::Command::Run) battle = false;
            else if (static_cast<int>(battle_command) <= 3 && enemy_hp > 0 && sealed_turns == 0 && skill_state.usable(static_cast<int>(battle_command), player_qi) && player_qi >= skill_cost[static_cast<int>(battle_command)]) { const int slot=static_cast<int>(battle_command); skill_state.selected=slot; player_qi -= skill_cost[slot]; if (((enemy_turns * 37) % 100) < skill_accuracy[slot]) { enemy_hp -= battle::damage({skill_power[slot] + (artifact_owned ? artifact_attack_bonus : 0), 0, player_hp, player_qi, player_speed}, {0, 0, enemy_hp, 0, enemy_speed}, 0, false); enemy_status=skill_status[slot]; enemy_status_turns=status_duration(enemy_status); if(skill_seal_turns[slot]>0) sealed_turns=skill_seal_turns[slot]; } skill_state.tick(); }
            else if (battle_command == battle::Command::Capture && battle::can_capture(enemy_hp, 80 + enemy_level * 5, enemy_turns)) { collection_state.discover(1); if (collection_state.add_contract(1)) { enemy_hp = 0; dokkaebi_contracted = true; party_count = collection_state.party_count; for (int i=0;i<3;++i) party_ids[i] = collection_state.party_ids[i]; } }
            const bool frozen = status_rules::skips_action(enemy_status, enemy_turns) && enemy_status == Status::Freeze;
            const bool paralyzed = status_rules::skips_action(enemy_status, enemy_turns) && enemy_status == Status::Paralysis;
            if (enemy_hp > 0 && battle && !frozen && !paralyzed) {
                int damage = 8 + (enemy_turns++ % 3);
                damage = status_rules::adjusted_damage(enemy_status, damage, fear_multiplier);
                player_hp -= damage;
            } else if (enemy_hp > 0 && battle) ++enemy_turns;
            if (enemy_hp > 0 && battle) enemy_hp -= status_rules::tick_damage(enemy_status, burn_damage);
            if (artifact_owned && battle) player_hp -= artifact_battle_cost;
            if (sealed_turns > 0) --sealed_turns;
            if (enemy_status_turns > 0) { --enemy_status_turns; if (enemy_status_turns == 0) enemy_status = Status::None; }
            if (enemy_turns > 0 && enemy_turns % 4 == 0) sealed_turns = 1;
            if (enemy_hp <= 0 || player_hp <= 0) { if (enemy_hp <= 0) { player_xp += enemy_level * 25; if (player_xp >= player_level * 100 && player_level < 50) { player_xp -= player_level * 100; ++player_level; fight_power += 2; player_qi += 5; } } player_hp = 100 + player_level * 5; enemy_hp = 80 + enemy_level * 5; enemy_turns = 0; finish_battle(); }
            render(); InvalidateRect(hwnd, nullptr, FALSE); return 0;
        }
        if (battle && (wp == VK_LEFT || wp == VK_RIGHT)) { const int current = static_cast<int>(battle_command); const int direction = wp == VK_RIGHT ? 1 : -1; battle_command = static_cast<battle::Command>(battle_ui::move_command(current, direction, 6)); render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
        if (!dialogue && !battle && wp == 'B') { begin_battle(); render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
        if (battle) return 0;
        if (dialogue && wp != VK_RETURN && wp != 'Z') return 0;
        int nx = player_x, ny = player_y;
        if (wp == VK_LEFT) --nx;
        if (wp == VK_RIGHT) ++nx;
        if (wp == VK_UP) --ny;
        if (wp == VK_DOWN) ++ny;
        if ((wp == VK_LEFT || wp == VK_RIGHT || wp == VK_UP || wp == VK_DOWN) && !blocked(nx, ny)) {
            player_x = nx; player_y = ny; ++steps;
            if (encounter_interval > 0 && steps % encounter_interval == 0) { begin_battle(); }
        }
        if (wp == VK_RETURN || wp == 'Z') {
            if (!dialogue && near_shrine()) { dialogue = true; dialogue_page = 0; world_state.mark_shrine_seen(); }
            else if (++dialogue_page >= dialogue_line_count) { dialogue = false; dialogue_page = 0; }
        }
        if (dialogue) { render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
        render(); InvalidateRect(hwnd, nullptr, FALSE); return 0;
    }
    if (msg == WM_DESTROY) { running = false; PostQuitMessage(0); return 0; }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    const std::string yokai = data::Registry::load_text("data/yokai_001_dokkaebi.json");
    const std::string skill = data::Registry::load_text("data/skill_001_basic_strike.json");
    const std::string encounters = data::Registry::load_text("data/encounter_village_edge.json");
    const std::string artifact = data::Registry::load_text("data/artifact_001_shrine_shard.json");
    const std::string statuses = data::Registry::load_text("data/status_effects.json");
    if (!yokai.empty()) { enemy_hp = data::Registry::integer(yokai, "hp", enemy_hp); enemy_speed = data::Registry::integer(yokai, "speed", enemy_speed); }
    if (!skill.empty()) { fight_power = data::Registry::integer(skill, "power", fight_power) + 10; skill_qi_cost = data::Registry::integer(skill, "qi_cost", skill_qi_cost); }
    for (int i=0;i<4;++i) { const std::string path = "data/skill_00" + std::to_string(i+1) + (i==0 ? "_basic_strike.json" : i==1 ? "_ember.json" : i==2 ? "_frost.json" : "_seal.json"); const std::string content=data::Registry::load_text(path.c_str()); skill_state.ids[i]=i+1; if(!content.empty()){skill_power[i]=data::Registry::integer(content,"power",skill_power[i]); skill_cost[i]=data::Registry::integer(content,"qi_cost",skill_cost[i]); skill_accuracy[i]=data::Registry::integer(content,"accuracy",skill_accuracy[i]); skill_seal_turns[i]=data::Registry::integer(content,"seal_turns",skill_seal_turns[i]); const std::string status=data::Registry::string_value(content,"status",""); if(status=="burn") skill_status[i]=Status::Burn; else if(status=="freeze") skill_status[i]=Status::Freeze; else if(status=="seal") skill_status[i]=Status::Seal; } }
    if (!encounters.empty()) { encounter_interval = data::Registry::integer(encounters, "encounter_step_interval", encounter_interval); enemy_level = data::Registry::integer(encounters, "min_level", enemy_level); enemy_hp = 80 + enemy_level * 5; }
    if (!artifact.empty()) { artifact_attack_bonus = data::Registry::integer(artifact, "attack_bonus", artifact_attack_bonus); artifact_battle_cost = data::Registry::integer(artifact, "hp_loss_per_battle", artifact_battle_cost); }
    if (!statuses.empty()) burn_damage = data::Registry::integer(statuses, "damage_per_turn", burn_damage);
    if (!statuses.empty()) fear_multiplier = data::Registry::number(statuses, "damage_taken_multiplier", fear_multiplier);
    const wchar_t* name = L"YinYangChronicleWindow";
    WNDCLASSW wc{}; wc.hInstance = instance; wc.lpfnWndProc = wnd_proc; wc.lpszClassName = name;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW); RegisterClassW(&wc);
    RECT rect{0, 0, W * SCALE, H * SCALE}; AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    HWND hwnd = CreateWindowExW(0, name, L"108: 음양견문록 — Technical Spike", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
        nullptr, nullptr, instance, nullptr);
    if (!hwnd) return 1;
    ShowWindow(hwnd, show); render();
    BITMAPINFO bmi{}; bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); bmi.bmiHeader.biWidth = W;
    bmi.bmiHeader.biHeight = -H; bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    MSG msg{};
    while (running) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
        HDC dc = GetDC(hwnd); RECT client{}; GetClientRect(hwnd, &client);
        StretchDIBits(dc, 0, 0, client.right, client.bottom, 0, 0, W, H, pixels.data(), &bmi, DIB_RGB_COLORS, SRCCOPY);
        ReleaseDC(hwnd, dc); Sleep(16);
    }
    return 0;
}
