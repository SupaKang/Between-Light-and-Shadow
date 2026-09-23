// Game logic + scenes. Platform-free: main.cpp feeds Input and presents gfx::fb.
#pragma once
#include <string>

enum Key { K_UP, K_DOWN, K_LEFT, K_RIGHT, K_A, K_B, K_START, K_COUNT };
struct Input { bool held[K_COUNT]{}; bool pressed[K_COUNT]{}; };

// 1 real second = 1 game minute (1 real minute = 1 game hour).
struct Clock { int day = 1; float minute = 6 * 60; };
enum class Phase { Dawn, Day, Dusk, Night };
Phase phase_of(float minute);
void rest(Clock& c);  // always wakes on the next day at 06:00

void game_init(unsigned seed);
void game_update(const Input& in);  // fixed 60 Hz step
void game_render();
bool game_debug_scene(const std::string& name);  // fixtures for --shot screenshots

struct DebugInfo { int scene, map, x, y, quest; bool busy; };  // busy: dialog/transition/menu open
DebugInfo game_debug_info();
