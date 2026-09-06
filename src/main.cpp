#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#include <vector>
#include <fstream>
#include <string>
#include "battle_rules.h"
#include "status_rules.h"

namespace {
constexpr int W = 960, H = 540, SCALE = 2, TILE = 32;
std::vector<std::uint32_t> pixels(W * H);
bool running = true;
bool dialogue = false;
bool battle = false;
bool menu = false;
int save_slot = 1;
int battle_command = 0;
int player_hp = 100, enemy_hp = 80;
int player_qi = 30, skill_qi_cost = 0;
int player_level = 1, player_xp = 0;
int enemy_turns = 0;
int sealed_turns = 0;
int player_speed = 10, enemy_speed = 8;
int fight_power = 20;
int party_count = 0;
bool dokkaebi_contracted = false;
bool artifact_owned = false;
int party_ids[3] = {0, 0, 0};
int steps = 0;
int encounter_interval = 12;
bool player_priority = true;
using Status = status_rules::Kind;
Status enemy_status = Status::Burn;
int dialogue_page = 0;
const char* dialogue_lines[] = {"YIN YANG", "A SPIRIT WAITS", "PRESS ENTER"};
constexpr int dialogue_line_count = 3;
int player_x = 15, player_y = 8;
bool blocked(int x, int y) { return x < 6 || x > 23 || y < 4 || y > 12; }
bool near_shrine() { return (player_x == 15 && player_y == 8) || (player_x == 15 && player_y == 7); }
int json_int(const std::string& json, const char* key, int fallback) {
    const std::string needle = std::string("\"") + key + "\"";
    const auto pos = json.find(needle); if (pos == std::string::npos) return fallback;
    const auto colon = json.find(':', pos); if (colon == std::string::npos) return fallback;
    try { return std::stoi(json.substr(colon + 1)); } catch (...) { return fallback; }
}
std::string load_file(const char* path) {
    std::ifstream file(path, std::ios::binary); return std::string((std::istreambuf_iterator<char>(file)), {});
}
struct SaveData { std::uint32_t magic, version; int x, y, hp, qi, level, xp, party, ids[3]; std::uint8_t contracted, artifact; std::uint32_t checksum; };
std::uint32_t save_checksum(const SaveData& s) { return 0x108u ^ s.version ^ static_cast<std::uint32_t>(s.x * 31 + s.y * 17 + s.hp * 13 + s.qi * 11 + s.level * 9 + s.xp * 7 + s.party * 5 + s.ids[0] * 3 + s.ids[1] * 2 + s.ids[2] + s.contracted + s.artifact * 19); }
std::string slot_file() { return "save_slot_" + std::to_string(save_slot) + ".sav"; }
void save_game() { SaveData s{0x59435931u, 6u, player_x, player_y, player_hp, player_qi, player_level, player_xp, party_count, {party_ids[0],party_ids[1],party_ids[2]}, static_cast<std::uint8_t>(dokkaebi_contracted), static_cast<std::uint8_t>(artifact_owned), 0}; s.checksum=save_checksum(s); std::ofstream f(slot_file(), std::ios::binary); f.write(reinterpret_cast<const char*>(&s), sizeof(s)); }
void load_game() { SaveData s{}; std::ifstream f(slot_file(), std::ios::binary); if (f.read(reinterpret_cast<char*>(&s), sizeof(s)) && s.magic == 0x59435931u && s.version == 6u && s.checksum == save_checksum(s) && s.x >= 0 && s.x < W / TILE && s.y >= 0 && s.y < H / TILE) { player_x=s.x; player_y=s.y; player_hp=s.hp; player_qi=s.qi; player_level=s.level; player_xp=s.xp; party_count=s.party; for(int i=0;i<3;++i) party_ids[i]=s.ids[i]; dokkaebi_contracted=s.contracted != 0; artifact_owned=s.artifact != 0; } }
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
        text("FIGHT", 100, 350, 3); text("CAPTURE", 260, 350, 3); text("RUN", 500, 350, 3);
        for (int y=345;y<350;++y) for (int x=90+battle_command*160;x<100+battle_command*160;++x) pixels[y*W+x]=0xFFE8D8A0u;
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
            if (battle_command == 2) battle = false;
            else if (battle_command == 0 && enemy_hp > 0 && sealed_turns == 0 && player_qi >= skill_qi_cost) { player_qi -= skill_qi_cost; enemy_hp -= battle::damage({fight_power + (artifact_owned ? 3 : 0), 0, player_hp, player_qi, player_speed}, {0, 0, enemy_hp, 0, enemy_speed}, 0, false); }
            else if (battle_command == 1 && battle::can_capture(enemy_hp, 80, enemy_turns)) { enemy_hp = 0; dokkaebi_contracted = true; if (party_count < 3) party_ids[party_count++] = 1; }
            const bool frozen = status_rules::skips_action(enemy_status, enemy_turns) && enemy_status == Status::Freeze;
            const bool paralyzed = status_rules::skips_action(enemy_status, enemy_turns) && enemy_status == Status::Paralysis;
            if (enemy_hp > 0 && battle && !frozen && !paralyzed) {
                int damage = 8 + (enemy_turns++ % 3);
                damage = status_rules::adjusted_damage(enemy_status, damage);
                player_hp -= damage;
            } else if (enemy_hp > 0 && battle) ++enemy_turns;
            if (enemy_hp > 0 && battle) enemy_hp -= status_rules::tick_damage(enemy_status, 0);
            if (artifact_owned && battle) player_hp -= 2;
            if (sealed_turns > 0) --sealed_turns;
            if (enemy_turns > 0 && enemy_turns % 4 == 0) sealed_turns = 1;
            if (enemy_hp <= 0 || player_hp <= 0) { if (enemy_hp <= 0) { player_xp += 25; if (player_xp >= player_level * 100 && player_level < 50) { player_xp -= player_level * 100; ++player_level; fight_power += 2; player_qi += 5; } } battle = false; player_hp = 100 + player_level * 5; enemy_hp = 80; enemy_turns = 0; }
            render(); InvalidateRect(hwnd, nullptr, FALSE); return 0;
        }
        if (battle && (wp == VK_LEFT || wp == VK_RIGHT)) { battle_command = (battle_command + (wp == VK_RIGHT ? 1 : 2)) % 3; render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
        if (!dialogue && !battle && wp == 'B') { battle = true; player_priority = player_speed >= enemy_speed; render(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
        if (battle) return 0;
        if (dialogue && wp != VK_RETURN && wp != 'Z') return 0;
        int nx = player_x, ny = player_y;
        if (wp == VK_LEFT) --nx;
        if (wp == VK_RIGHT) ++nx;
        if (wp == VK_UP) --ny;
        if (wp == VK_DOWN) ++ny;
        if ((wp == VK_LEFT || wp == VK_RIGHT || wp == VK_UP || wp == VK_DOWN) && !blocked(nx, ny)) {
            player_x = nx; player_y = ny; ++steps;
            if (encounter_interval > 0 && steps % encounter_interval == 0) { battle = true; player_priority = player_speed >= enemy_speed; }
        }
        if (wp == VK_RETURN || wp == 'Z') {
            if (!dialogue && near_shrine()) { dialogue = true; dialogue_page = 0; }
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
    const std::string yokai = load_file("data/yokai_001_dokkaebi.json");
    const std::string skill = load_file("data/skill_001_basic_strike.json");
    const std::string encounters = load_file("data/encounter_village_edge.json");
    if (!yokai.empty()) { enemy_hp = json_int(yokai, "hp", enemy_hp); enemy_speed = json_int(yokai, "speed", enemy_speed); }
    if (!skill.empty()) { fight_power = json_int(skill, "power", fight_power) + 10; skill_qi_cost = json_int(skill, "qi_cost", skill_qi_cost); }
    if (!encounters.empty()) encounter_interval = json_int(encounters, "encounter_step_interval", encounter_interval);
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
