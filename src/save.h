// Save slots: <dir>/slot<N>.sav, one "key=value" per line (UTF-8). Unknown keys are ignored, missing keys keep defaults.
#pragma once
#include <string>

namespace yy {
constexpr int kSaveSlots = 3;
struct SaveInfo { bool exists = false; bool corrupt = false; std::string map_name; int day = 0; int playtime_min = 0; };
void save_set_dir(const std::string& dir);  // UTF-8; empty disables saving (tests, --shot)
bool save_allowed();                        // false during battle, transition, running script, dialog
bool save_game(int slot);                   // atomic: writes slot<N>.sav.tmp, then renames over the old file
bool load_game(int slot);                   // false on missing/corrupt; game state untouched on failure
SaveInfo save_info(int slot);
bool any_save();                            // some slot loads (title "continue")
}  // namespace yy
