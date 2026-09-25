#include "save.h"
#include <filesystem>
#include <fstream>
#include "state.h"

namespace fs = std::filesystem;

namespace yy {

static fs::path save_dir;

void save_set_dir(const std::string& dir) { save_dir = dir.empty() ? fs::path() : fs::u8path(dir); }

static fs::path slot_path(int slot) { return save_dir / ("slot" + std::to_string(slot) + ".sav"); }
static bool usable(int slot) { return !save_dir.empty() && slot >= 0 && slot < kSaveSlots; }

bool save_allowed() {
    return g.scene == Scene::Field && g.trans_t < 0 && g.enc_t == 0 && g.dq.empty() && !script_busy() && !g.moving;
}

bool save_game(int slot) {
    if (!usable(slot) || !save_allowed()) return false;
    fs::path dst = slot_path(slot), tmp = dst;
    tmp += ".tmp";
    {
        std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
        const char dirs[] = "DULR";
        out << "version=1\nmap=" << g.map_id << "\nx=" << g.px << "\ny=" << g.py << "\ndir=" << dirs[g.dir]
            << "\nday=" << g.clock.day << "\nminute=" << (int)g.clock.minute << "\nhp=" << g.hp << "\nng=" << g.ng
            << "\nlevel=" << g.level << "\nmoney=" << g.money << "\nplaytime=" << g.playtime_f / 3600 << "\n";
        for (auto& [id, n] : g.items) if (n > 0) out << "item." << id << "=" << n << "\n";
        for (auto& [k, v] : g.flags) out << "flag." << k << "=" << v << "\n";
        if (!out.flush()) return false;
    }
    std::error_code ec;
    fs::rename(tmp, dst, ec);  // replaces dst on POSIX and Windows (MoveFileEx)
    if (ec) { std::fprintf(stderr, "save: %s\n", ec.message().c_str()); fs::remove(tmp, ec); return false; }
    return true;
}

// Reads a slot into t (on top of Game{} defaults). False if missing, unreadable, or invalid.
static bool parse(int slot, Game& t) {
    if (!usable(slot)) return false;
    std::ifstream in(slot_path(slot), std::ios::binary);
    if (!in) return false;
    t.items.clear();  // a save lists every item it has
    std::string line;
    int version = 0;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string k = line.substr(0, eq), v = line.substr(eq + 1);
        int n = std::atoi(v.c_str());
        if (k == "version") version = n;
        else if (k == "map") t.map_id = v;
        else if (k == "x") t.px = n;
        else if (k == "y") t.py = n;
        else if (k == "dir") t.dir = dir_of(v.empty() ? 'D' : v[0]);
        else if (k == "day") t.clock.day = std::max(1, n);
        else if (k == "minute") t.clock.minute = (float)std::clamp(n, 0, 1439);
        else if (k == "hp") t.hp = n;
        else if (k == "ng") t.ng = n;
        else if (k == "level") t.level = std::clamp(n, 1, 50);
        else if (k == "money") t.money = std::max(0, n);
        else if (k == "playtime") t.playtime_f = std::max(0, n) * 3600;
        else if (k.rfind("item.", 0) == 0) { if (n > 0 && data::find_item(k.substr(5))) t.items[k.substr(5)] = n; }
        else if (k.rfind("flag.", 0) == 0) t.flags[k.substr(5)] = n;
    }
    t.hp = std::clamp(t.hp, 1, t.hp_max);
    t.ng = std::clamp(t.ng, 0, t.ng_max);
    return version == 1 && warp_valid(t.map_id, t.px, t.py);
}

bool load_game(int slot) {
    Game t{};
    if (!parse(slot, t)) return false;  // g untouched
    t.rng = g.rng;
    t.trans_t = g.trans_t;  // loading inside a fade keeps the fade-in
    t.scene = Scene::Field;
    t.last_phase = phase_of(t.clock.minute);
    g = std::move(t);
    load_map(g.map_id);
    script_init();  // no event survives a load
    return true;
}

SaveInfo save_info(int slot) {
    SaveInfo s;
    std::error_code ec;
    if (!usable(slot) || !fs::exists(slot_path(slot), ec)) return s;
    s.exists = true;
    Game t{};
    if (!parse(slot, t)) { s.corrupt = true; return s; }
    s.map_name = find_map(t.map_id)->name_ko;
    s.day = t.clock.day;
    s.playtime_min = t.playtime_f / 3600;
    return s;
}

bool any_save() {
    for (int i = 0; i < kSaveSlots; ++i) {
        SaveInfo s = save_info(i);
        if (s.exists && !s.corrupt) return true;
    }
    return false;
}

}  // namespace yy
