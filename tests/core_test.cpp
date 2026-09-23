// Minimal self-checks for platform-free core logic.
#undef NDEBUG  // asserts must run in Release builds too
#include <cassert>
#include <cstdio>
#include "../src/game.h"
#include "../src/gfx.h"
#include "../src/data.h"
#include "../src/maps_api.h"

int main() {
    // Clock phases and rest rule: rest always wakes on the next day at 06:00.
    assert(phase_of(6 * 60) == Phase::Dawn);
    assert(phase_of(12 * 60) == Phase::Day);
    assert(phase_of(18 * 60) == Phase::Dusk);
    assert(phase_of(23 * 60) == Phase::Night);
    assert(phase_of(2 * 60) == Phase::Night);
    Clock c{3, 23 * 60 + 30};
    rest(c);
    assert(c.day == 4 && c.minute == 6 * 60);

    // Korean text: Hangul advance is 12px, wrap keeps lines inside the box.
    assert(gfx::text_width("가") == 12);
    assert(gfx::cp_count("음양사 abc") == 7);
    auto lines = gfx::wrap("산천의 영맥이 뒤엉켜, 백팔 가지 요괴가 밤낮을 가리지 않고 날뛰었지.", 120);
    assert(lines.size() > 1);
    for (auto& l : lines) assert(gfx::text_width(l) <= 120);

    // Every debug scene builds and renders without crashing.
    for (const char* s : {"title", "prologue", "card", "wake", "village", "night", "talk", "rest", "menu", "shop",
                          "battle", "battle_list", "battle_msg", "encounter", "choice"}) {
        game_init(1);
        assert(game_debug_scene(s));
        Input none;
        game_update(none);
        game_render();
    }
    // Scripted playthrough: title -> prologue -> wake -> walk to the gate -> win battle -> quest done.
    game_init(7);
    auto frame = [](int key, bool press) {
        Input in;
        if (key >= 0) { in.held[key] = true; in.pressed[key] = press; }
        game_update(in);
        game_render();
    };
    auto tap = [&](int key) { frame(key, true); frame(-1, false); };
    auto until = [&](auto cond, auto step, int cap) { while (!cond() && cap--) step(); return cond(); };
    auto info = game_debug_info;
    const int FIELD = 3, BATTLE = 4;
    tap(K_A);
    assert(until([&] { return info().scene == FIELD && !info().busy; }, [&] { tap(K_A); }, 3000));
    assert(info().map == "tavern_room");
    auto walk = [&](int key, auto cond) { return until(cond, [&] { frame(key, false); }, 2000); };
    assert(walk(K_RIGHT, [&] { return info().x == 3 && !info().busy; }));
    assert(walk(K_DOWN, [&] { return info().map == "village" && !info().busy; }));
    assert(walk(K_DOWN, [&] { return info().y == 14 && !info().busy; }));
    assert(walk(K_RIGHT, [&] { return info().x == 12 && !info().busy; }));
    assert(walk(K_UP, [&] { return info().y == 2; }));
    assert(until([&] { return info().scene == BATTLE; }, [&] { tap(K_A); }, 3000));
    assert(until([&] { return info().scene == FIELD && info().quest == 1 && !info().busy; }, [&] { tap(K_A); }, 6000));

    // Baked game data: 108 yokai with five-phase elements, items with prices.
    assert(data::yokai_count() == 108);
    assert(data::find_yokai("yokai_076")->element == data::Element::Wood);   // 장승 목신
    assert(data::find_yokai("yokai_006")->element == data::Element::Water);  // 해태: fire ward
    assert(data::overcomes(data::Element::Metal, data::Element::Wood));
    assert(!data::overcomes(data::Element::Wood, data::Element::Metal));
    assert(data::find_item("contract_talisman")->price == 40);
    assert(data::find_item("nope") == nullptr);
    int korean = 0;
    for (int i = 0; i < data::yokai_count(); ++i) korean += data::yokai_at(i).korean;
    assert(korean == 76);  // AGENTS: 70% Korean folklore

    // Baked maps: same layout as the old in-code maps; bad warps are rejected.
    const MapDef* v = find_map("village");
    assert(v && v->w == 28 && v->h == 19 && !v->indoor);
    assert(v->tiles[1 * v->w + 11] == 'G');  // left jangseung
    assert(find_map("tavern_room")->indoor);
    assert(warp_valid("tavern_room", 3, 4));
    assert(!warp_valid("tavern_room", 0, 0));  // wall
    assert(!warp_valid("no_such_map", 1, 1));
    assert(!warp_valid("village", 99, 99));    // out of bounds

    std::puts("core_test ok");
}
