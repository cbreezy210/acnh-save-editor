#include <switch.h>
#include <SDL2/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("ACNH SE", 0, 0, 1280, 720, SDL_WINDOW_FULLSCREEN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Load the icon PNG from the SD card
    SDL_Texture* icon = nullptr;
    int iw = 0, ih = 0, comp = 0;
    unsigned char* pixels = stbi_load("sdmc:/switch/acnh_editor/icon.png", &iw, &ih, &comp, 4);
    if (pixels) {
        icon = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, iw, ih);
        SDL_UpdateTexture(icon, nullptr, pixels, iw * 4);
        SDL_SetTextureBlendMode(icon, SDL_BLENDMODE_BLEND);
        stbi_image_free(pixels);
    }

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    int px = 608, py = 328;

    while (appletMainLoop()) {
        padUpdate(&pad);
        u64 k = padGetButtonsDown(&pad);
        if (k & HidNpadButton_Plus) break;
        if (k & HidNpadButton_Left)  px -= 16;
        if (k & HidNpadButton_Right) px += 16;
        if (k & HidNpadButton_Up)    py -= 16;
        if (k & HidNpadButton_Down)  py += 16;
        if (px < 0) px = 0;
        if (py < 0) py = 0;
        if (px > 1280 - 64) px = 1280 - 64;
        if (py > 720 - 64) py = 720 - 64;

        SDL_SetRenderDrawColor(ren, 0x7A, 0xC1, 0x43, 0xFF);
        SDL_RenderClear(ren);

        SDL_SetRenderDrawColor(ren, 0xF8, 0xF5, 0xEC, 0xFF);
        SDL_Rect panel = { 240, 140, 800, 440 };
        SDL_RenderFillRect(ren, &panel);

        SDL_SetRenderDrawColor(ren, 0x7A, 0x5C, 0x3E, 0xFF);
        SDL_Rect bar = { 240, 140, 800, 70 };
        SDL_RenderFillRect(ren, &bar);

        // Draw the icon — or a RED box if the PNG failed to load
        if (icon) {
            SDL_Rect dst = { 640 - 128, 360 - 100, 256, 256 };
            SDL_RenderCopy(ren, icon, nullptr, &dst);
        } else {
            SDL_SetRenderDrawColor(ren, 0xFF, 0x00, 0x00, 0xFF);
            SDL_Rect bad = { 640 - 128, 360 - 100, 256, 256 };
            SDL_RenderFillRect(ren, &bad);
        }

        SDL_SetRenderDrawColor(ren, 0x6A, 0xA8, 0x4F, 0xFF);
        SDL_Rect sq = { px, py, 64, 64 };
        SDL_RenderFillRect(ren, &sq);

        SDL_RenderPresent(ren);
    }

    if (icon) SDL_DestroyTexture(icon);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}