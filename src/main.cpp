// SDL2 platform layer: window, integer scaling, input, fixed 60 Hz loop.
// Usage: YinYangChronicle [--shot <scene> <out.bmp>]
#include <SDL.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include "game.h"
#include "gfx.h"
#include "save.h"

static int shot(const char* scene, const char* path) {
    game_init(1);
    if (!game_debug_scene(scene)) { std::fprintf(stderr, "unknown scene: %s\n", scene); return 1; }
    Input none;
    for (int i = 0; i < 3; ++i) game_update(none);
    game_render();
    const int s = 3;  // 640x360 x3 = 1920x1080, same as the Full HD target
    SDL_Surface* out = SDL_CreateRGBSurfaceWithFormat(0, gfx::UW * s, gfx::UH * s, 32, SDL_PIXELFORMAT_XRGB8888);
    auto* px = static_cast<Uint32*>(out->pixels);
    for (int y = 0; y < gfx::UH * s; ++y)
        for (int x = 0; x < gfx::UW * s; ++x) px[y * out->pitch / 4 + x] = gfx::out[(y / s) * gfx::UW + x / s];
    int rc = SDL_SaveBMP(out, path);
    SDL_FreeSurface(out);
    return rc;
}

int main(int argc, char** argv) {
    if (argc == 4 && !std::strcmp(argv[1], "--shot")) return shot(argv[2], argv[3]);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
        std::fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    if (char* p = SDL_GetPrefPath("supakang", "YinYangChronicle")) { yy::save_set_dir(p); SDL_free(p); }
    else yy::save_set_dir(".");
    SDL_Rect usable{0, 0, 1920, 1080};
    SDL_GetDisplayUsableBounds(0, &usable);
    int scale = SDL_max(1, SDL_min(usable.w / gfx::UW, (usable.h - 40) / gfx::UH));
    SDL_Window* win = SDL_CreateWindow("108: 음양견문록", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       gfx::UW * scale, gfx::UH * scale, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_RenderSetLogicalSize(ren, gfx::UW, gfx::UH);
    SDL_RenderSetIntegerScale(ren, SDL_TRUE);
    SDL_Texture* tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, gfx::UW, gfx::UH);

    game_init((unsigned)std::time(nullptr));
    SDL_GameController* pad = nullptr;
    bool held[K_COUNT] = {}, prev[K_COUNT] = {};
    Uint64 freq = SDL_GetPerformanceFrequency(), last = SDL_GetPerformanceCounter();
    double acc = 0;

    for (bool running = true; running;) {
        for (SDL_Event e; SDL_PollEvent(&e);) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_CONTROLLERDEVICEADDED && !pad) pad = SDL_GameControllerOpen(e.cdevice.which);
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F11)
                SDL_SetWindowFullscreen(win, SDL_GetWindowFlags(win) & SDL_WINDOW_FULLSCREEN_DESKTOP ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
        }
        const Uint8* ks = SDL_GetKeyboardState(nullptr);
        auto btn = [&](SDL_GameControllerButton b) { return pad && SDL_GameControllerGetButton(pad, b); };
        held[K_UP] = ks[SDL_SCANCODE_UP] || btn(SDL_CONTROLLER_BUTTON_DPAD_UP);
        held[K_DOWN] = ks[SDL_SCANCODE_DOWN] || btn(SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        held[K_LEFT] = ks[SDL_SCANCODE_LEFT] || btn(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        held[K_RIGHT] = ks[SDL_SCANCODE_RIGHT] || btn(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        held[K_A] = ks[SDL_SCANCODE_Z] || ks[SDL_SCANCODE_SPACE] || btn(SDL_CONTROLLER_BUTTON_A);
        held[K_B] = ks[SDL_SCANCODE_X] || ks[SDL_SCANCODE_ESCAPE] || btn(SDL_CONTROLLER_BUTTON_B);
        held[K_START] = ks[SDL_SCANCODE_RETURN] || btn(SDL_CONTROLLER_BUTTON_START);

        Uint64 now = SDL_GetPerformanceCounter();
        acc += (double)(now - last) / freq;
        last = now;
        if (acc > 0.25) acc = 0.25;
        while (acc >= 1.0 / 60) {
            Input in;
            for (int k = 0; k < K_COUNT; ++k) { in.held[k] = held[k]; in.pressed[k] = held[k] && !prev[k]; prev[k] = held[k]; }
            game_update(in);
            acc -= 1.0 / 60;
        }
        game_render();
        SDL_UpdateTexture(tex, nullptr, gfx::out, gfx::UW * 4);
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, nullptr, nullptr);
        SDL_RenderPresent(ren);
    }
    SDL_Quit();
    return 0;
}
