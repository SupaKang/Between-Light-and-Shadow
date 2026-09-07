#include "battle_ui.h"
#include <cstdio>
#include <cstring>

namespace battle_ui {

int move_command(int current, int direction, int count) {
    if (count <= 0) return 0;
    int next = (current + direction) % count;
    return next < 0 ? next + count : next;
}

const char* command_label(int cmd, const skills::State& skills) {
    if (cmd >= 0 && cmd < skills::State::MaxSlots) {
        if (skills.ids[cmd] > 0) return skills.names[cmd];
        return "---";
    }
    if (cmd == 4) return "CAPTURE";
    if (cmd == 5) return "RUN";
    return "???";
}

const char* command_label_ko(int cmd, const skills::State& skills) {
    if (cmd >= 0 && cmd < skills::State::MaxSlots) {
        if (skills.ids[cmd] > 0) return skills.names_ko[cmd];
        return "---";
    }
    if (cmd == 4) return "계약";
    if (cmd == 5) return "도주";
    return "???";
}

void command_detail(int cmd, const skills::State& skills, int player_qi, int enemy_hp, int enemy_max_hp, int capture_rate, char* out_buf, int out_size) {
    if (!out_buf || out_size <= 0) return;
    if (cmd >= 0 && cmd < skills::State::MaxSlots) {
        if (skills.ids[cmd] <= 0) {
            std::snprintf(out_buf, out_size, "NO SKILL EQUIPPED IN THIS SLOT");
            return;
        }
        if (skills.sealed_turns[cmd] > 0) {
            std::snprintf(out_buf, out_size, "[SEALED] LOCKED FOR %d MORE TURN(S)", skills.sealed_turns[cmd]);
            return;
        }
        if (player_qi < skills.qi_costs[cmd]) {
            std::snprintf(out_buf, out_size, "[QI DEFICIT] NEEDS %d QI (HAVE %d)", skills.qi_costs[cmd], player_qi);
            return;
        }
        const char* status_name = status_rules::name(skills.statuses[cmd]);
        if (skills.statuses[cmd] != status_rules::Kind::None) {
            std::snprintf(out_buf, out_size, "PWR:%d  QI:%d  ACC:%d%%  EFFECT:%s",
                skills.powers[cmd], skills.qi_costs[cmd], skills.accuracies[cmd], status_name);
        } else {
            std::snprintf(out_buf, out_size, "PWR:%d  QI:%d  ACC:%d%%  STANDARD ATTACK",
                skills.powers[cmd], skills.qi_costs[cmd], skills.accuracies[cmd]);
        }
        return;
    }
    if (cmd == 4) {
        if (enemy_hp > enemy_max_hp / 2) {
            std::snprintf(out_buf, out_size, "CONTRACT: ENEMY HP MUST BE UNDER 50%%");
        } else {
            std::snprintf(out_buf, out_size, "CONTRACT CHANCE: %d%% - SEAL SPIRIT PACT", capture_rate);
        }
        return;
    }
    if (cmd == 5) {
        std::snprintf(out_buf, out_size, "FLEE FROM BATTLE - RETURN TO OVERWORLD");
        return;
    }
    std::snprintf(out_buf, out_size, "UNKNOWN COMMAND");
}

void command_detail_ko(int cmd, const skills::State& skills, int player_qi, int enemy_hp, int enemy_max_hp, int capture_rate, char* out_buf, int out_size) {
    if (!out_buf || out_size <= 0) return;
    if (cmd >= 0 && cmd < skills::State::MaxSlots) {
        if (skills.ids[cmd] <= 0) {
            std::snprintf(out_buf, out_size, "장착된 기술이 없습니다");
            return;
        }
        if (skills.sealed_turns[cmd] > 0) {
            std::snprintf(out_buf, out_size, "[봉인됨] %d턴 동안 사용 불가", skills.sealed_turns[cmd]);
            return;
        }
        if (player_qi < skills.qi_costs[cmd]) {
            std::snprintf(out_buf, out_size, "[영력 부족] 필요 영력: %d (보유: %d)", skills.qi_costs[cmd], player_qi);
            return;
        }
        const char* status_name = status_rules::name_ko(skills.statuses[cmd]);
        if (skills.statuses[cmd] != status_rules::Kind::None) {
            std::snprintf(out_buf, out_size, "위력:%d  영력:%d  명중:%d%%  효과:%s",
                skills.powers[cmd], skills.qi_costs[cmd], skills.accuracies[cmd], status_name);
        } else {
            std::snprintf(out_buf, out_size, "위력:%d  영력:%d  명중:%d%%  일반 공격",
                skills.powers[cmd], skills.qi_costs[cmd], skills.accuracies[cmd]);
        }
        return;
    }
    if (cmd == 4) {
        if (enemy_hp > enemy_max_hp / 2) {
            std::snprintf(out_buf, out_size, "[계약 불가] 적 체력을 50%% 이하로 깎아야 함");
        } else {
            std::snprintf(out_buf, out_size, "[계약] 성공률: %d%% - 요괴와 영맥 계약 체결", capture_rate);
        }
        return;
    }
    if (cmd == 5) {
        std::snprintf(out_buf, out_size, "[도주] 전투를 이탈하여 필드로 복귀");
        return;
    }
    std::snprintf(out_buf, out_size, "알 수 없는 명령");
}

}
