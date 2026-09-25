// Lua event runner: story events are Lua functions run as coroutines, one at a time.
// say/ask/battle/warp/rest/shop yield; the engine resumes the event when the dialog, battle or fade ends.
#pragma once
#include <string>

void script_init();  // fresh lua_State with every baked script loaded (game_init calls it)
bool script_start(const std::string& event, int x = -1, int y = -1);  // false if unknown or one is already running
void script_resume(int value);  // dialog choice (1-based) / battle result (1 win, 0 lose); no-op when idle
bool script_busy();
std::string script_last_error();
int script_flag(const std::string& name);  // integer story flag (0 when unset)
