#include <switch.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static SDL_Texture* makeText(SDL_Renderer* ren, TTF_Font* font, const char* str, Uint8 r, Uint8 g, Uint8 b) {
    SDL_Color col = { r, g, b, 255 };
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, str, col);
    if (!surf) return nullptr;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
    return tex;
}

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("ACNH SE", 0, 0, 1280, 720, SDL_WINDOW_FULLSCREEN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    plInitialize(PlServiceType_System);
    int ttfInitOk = (TTF_Init() == 0);

    TTF_Font* ttf = nullptr;
    bool sharedOk = false, sdOk = false, memOk = false;
    static unsigned char* fontBuf = nullptr;
    PlFontData font;

    // Path A: system shared font
    if (ttfInitOk && R_SUCCEEDED(plGetSharedFontByType(&font, PlSharedFontType_Standard))) {
        SDL_RWops* rw = SDL_RWFromMem(font.address, (int)font.size);
        ttf = TTF_OpenFontRW(rw, 1, 42);
        sharedOk = (ttf != nullptr);
    }
    // Path B: SDL opens the file itself
    if (ttfInitOk && !ttf) {
        ttf = TTF_OpenFont("sdmc:/switch/acnh_editor/font.ttf", 42);
        sdOk = (ttf != nullptr);
    }
    // Path C: WE read the bytes with stdio, hand them over from memory
    if (ttfInitOk && !ttf) {
        FILE* f = fopen("sdmc:/switch/acnh_editor/font.ttf", "rb");
        if (f) {
            fseek(f, 0, SEEK_END);
            long sz = ftell(f);
            fseek(f, 0, SEEK_SET);
            fontBuf = (unsigned char*)malloc(sz);
            if (fontBuf && fread(fontBuf, 1, sz, f) == (size_t)sz) {
                SDL_RWops* rw = SDL_RWFromMem(fontBuf, (int)sz);
                ttf = TTF_OpenFontRW(rw, 1, 42);
                memOk = (ttf != nullptr);
            }
            fclose(f);
        }
    }

    FILE* dbg = fopen("sdmc:/switch/acnh_editor/debug.txt", "w");
    if (dbg) {
        fprintf(dbg, "ttf_init=%d shared=%d sd=%d mem=%d\n",
                ttfInitOk, sharedOk ? 1 : 0, sdOk ? 1 : 0, memOk ? 1 : 0);
        fprintf(dbg, "ttf_error=%s\n", TTF_GetError());
        fclose(dbg);
    }

    SDL_Texture* title = ttf ? makeText(ren, ttf, "ACNH Save Editor", 0xF8, 0xF5, 0xEC) : nullptr;
    SDL_Texture* sub   = ttf ? makeText(ren, ttf, "graphics mode: ONLINE", 0x7A, 0x5C, 0x3E) : nullptr;

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

    int px = 608, py = 620;

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

        if (title) {
            int w = 0, h = 0;
            SDL_QueryTexture(title, nullptr, nullptr, &w, &h);
            SDL_Rect dst = { 640 - w / 2, 140 + (70 - h) / 2, w, h };
            SDL_RenderCopy(ren, title, nullptr, &dst);
        }
        if (sub) {
            int w = 0, h = 0;
            SDL_QueryTexture(sub, nullptr, nullptr, &w, &h);
            SDL_Rect dst = { 640 - w / 2, 225, w, h };
            SDL_RenderCopy(ren, sub, nullptr, &dst);
        }
        if (icon) {
            SDL_Rect dst = { 640 - 128, 290, 256, 256 };
            SDL_RenderCopy(ren, icon, nullptr, &dst);
        }

        // LEDs: 1=shared 2=SDL-file 3=our-memory 4=title
        SDL_SetRenderDrawColor(ren, sharedOk ? 0x00 : 0xFF, sharedOk ? 0xFF : 0x00, 0x00, 0xFF);
        SDL_Rect l1 = { 40, 40, 40, 40 };
        SDL_RenderFillRect(ren, &l1);
        SDL_SetRenderDrawColor(ren, sdOk ? 0x00 : 0xFF, sdOk ? 0xFF : 0x00, 0x00, 0xFF);
        SDL_Rect l2 = { 90, 40, 40, 40 };
        SDL_RenderFillRect(ren, &l2);
        SDL_SetRenderDrawColor(ren, memOk ? 0x00 : 0xFF, memOk ? 0xFF : 0x00, 0x00, 0xFF);
        SDL_Rect l3 = { 140, 40, 40, 40 };
        SDL_RenderFillRect(ren, &l3);
        SDL_SetRenderDrawColor(ren, title ? 0x00 : 0xFF, title ? 0xFF : 0x00, 0x00, 0xFF);
        SDL_Rect l4 = { 190, 40, 40, 40 };
        SDL_RenderFillRect(ren, &l4);

        SDL_SetRenderDrawColor(ren, 0x6A, 0xA8, 0x4F, 0xFF);
        SDL_Rect sq = { px, py, 64, 64 };
        SDL_RenderFillRect(ren, &sq);

        SDL_RenderPresent(ren);
    }

    if (title) SDL_DestroyTexture(title);
    if (sub) SDL_DestroyTexture(sub);
    if (icon) SDL_DestroyTexture(icon);
    if (ttf) TTF_CloseFont(ttf);
    TTF_Quit();
    plExit();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}