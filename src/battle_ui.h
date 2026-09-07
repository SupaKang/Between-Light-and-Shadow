#pragma once
#include "battle_command.h"
#include "skill_state.h"

namespace battle_ui {
constexpr int CommandCount = 6;

int move_command(int current, int direction, int count);
const char* command_label(int cmd, const skills::State& skills);
const char* command_label_ko(int cmd, const skills::State& skills);
void command_detail(int cmd, const skills::State& skills, int player_qi, int enemy_hp, int enemy_max_hp, int capture_rate, char* out_buf, int out_size);
void command_detail_ko(int cmd, const skills::State& skills, int player_qi, int enemy_hp, int enemy_max_hp, int capture_rate, char* out_buf, int out_size);
}
