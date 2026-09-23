// Render regression: every debug scene must hash to the recorded value.
// Refresh deliberately with:  ./build/v1/RenderHashTest --print > tests/render_hashes.inc
#include <cstdio>
#include <cstring>
#include "../src/game.h"
#include "../src/gfx.h"

struct Expect { const char* scene; unsigned long long hash; };
static const Expect kExpected[] = {
#include "render_hashes.inc"
};

static const char* kScenes[] = {"title", "prologue", "choice", "card", "wake", "village", "yard", "gate", "dusk",
                                "night", "talk", "rest", "menu", "shop", "encounter", "battle", "battle_list", "battle_msg"};

static unsigned long long render(const char* s) {
    game_init(1);
    game_debug_scene(s);
    Input none;
    for (int i = 0; i < 3; ++i) game_update(none);
    game_render();
    return gfx::frame_hash();
}

int main(int argc, char** argv) {
    if (argc > 1 && !std::strcmp(argv[1], "--print")) {
        for (const char* s : kScenes) std::printf("{\"%s\", 0x%016llxull},\n", s, render(s));
        return 0;
    }
    int bad = 0;
    for (const Expect& e : kExpected) {
        unsigned long long h = render(e.scene);
        if (h != e.hash) { std::printf("MISMATCH %s: got 0x%016llx\n", e.scene, h); ++bad; }
    }
    std::printf(bad ? "render_hash_test FAILED (%d)\n" : "render_hash_test ok\n", bad);
    return bad ? 1 : 0;
}
