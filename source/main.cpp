#include <switch.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ================= v1.0 SAVE ENGINE (unchanged) =================
struct XorShift128 {
    uint32_t a, b, c, d;
    XorShift128(uint32_t seed) {
        uint32_t Mersenne = 0x6C078965;
        a = (Mersenne * (seed ^ (seed >> 30))) + 1;
        b = (Mersenne * (a   ^ (a   >> 30))) + 2;
        c = (Mersenne * (b   ^ (b   >> 30))) + 3;
        d = (Mersenne * (c   ^ (c   >> 30))) + 4;
    }
    uint32_t Next() {
        uint32_t t = a; a = b; b = c; c = d;
        t ^= t << 11; t ^= t >> 8;
        return d = t ^ d ^ (d >> 19);
    }
    uint64_t Next64() { return ((uint64_t)Next() << 32) | Next(); }
};

void GetParam(const uint32_t* data, int index, uint8_t* result, int len) {
    uint32_t seed = data[data[index] & 0x7F];
    XorShift128 rand(seed);
    uint32_t prms = data[data[index + 1] & 0x7F] & 0x7F;
    int rndRollCount = (prms & 0xF) + 1;
    for (int i = 0; i < rndRollCount; i++) rand.Next64();
    for (int i = 0; i < len; i++) result[i] = (uint8_t)(rand.Next() >> 24);
}

static const uint8_t sbox[256] = {
  0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
  0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
  0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
  0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
  0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
  0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
  0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
  0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
  0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
  0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
  0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
  0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
  0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
  0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
  0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
  0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};
static const uint8_t rcon[11] = {0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36};
static uint8_t xtime(uint8_t x) { return (x << 1) ^ (((x >> 7) & 1) * 0x1b); }
static void KeyExpansion(const uint8_t* key, uint8_t* rk) {
    for (int i = 0; i < 16; i++) rk[i] = key[i];
    for (int i = 4; i < 44; i++) {
        uint8_t t[4];
        for (int j = 0; j < 4; j++) t[j] = rk[(i-1)*4+j];
        if (i % 4 == 0) { uint8_t q = t[0]; t[0] = sbox[t[1]] ^ rcon[i/4]; t[1] = sbox[t[2]]; t[2] = sbox[t[3]]; t[3] = sbox[q]; }
        for (int j = 0; j < 4; j++) rk[i*4+j] = rk[(i-4)*4+j] ^ t[j];
    }
}
static void AesEncryptBlock(const uint8_t* rk, const uint8_t* in, uint8_t* out) {
    uint8_t s[16];
    for (int i = 0; i < 16; i++) s[i] = in[i];
    for (int i = 0; i < 16; i++) s[i] ^= rk[i];
    for (int r = 1; r < 10; r++) {
        for (int i = 0; i < 16; i++) s[i] = sbox[s[i]];
        uint8_t t;
        t=s[1]; s[1]=s[5]; s[5]=s[9]; s[9]=s[13]; s[13]=t;
        t=s[2]; s[2]=s[10]; s[10]=t; t=s[6]; s[6]=s[14]; s[14]=t;
        t=s[15]; s[15]=s[11]; s[11]=s[7]; s[7]=s[3]; s[3]=t;
        for (int c = 0; c < 4; c++) {
            uint8_t a=s[c*4], b=s[c*4+1], d=s[c*4+2], e=s[c*4+3], x=a^b^d^e;
            s[c*4]   ^= x ^ xtime(a^b);
            s[c*4+1] ^= x ^ xtime(b^d);
            s[c*4+2] ^= x ^ xtime(d^e);
            s[c*4+3] ^= x ^ xtime(e^a);
        }
        for (int i = 0; i < 16; i++) s[i] ^= rk[r*16+i];
    }
    for (int i = 0; i < 16; i++) s[i] = sbox[s[i]];
    { uint8_t t;
      t=s[1]; s[1]=s[5]; s[5]=s[9]; s[9]=s[13]; s[13]=t;
      t=s[2]; s[2]=s[10]; s[10]=t; t=s[6]; s[6]=s[14]; s[14]=t;
      t=s[15]; s[15]=s[11]; s[11]=s[7]; s[7]=s[3]; s[3]=t; }
    for (int i = 0; i < 16; i++) out[i] = s[i] ^ rk[160+i];
}

void CryptACNH(const uint8_t* header, uint8_t* data, size_t dataLen) {
    const uint32_t* imp = (const uint32_t*)(header + 0x100);
    uint8_t key[16], ctr[16];
    GetParam(imp, 0, key, 16);
    GetParam(imp, 2, ctr, 16);
    uint8_t rk[176];
    KeyExpansion(key, rk);
    uint8_t ek[16];
    for (size_t off = 0; off < dataLen; off += 16) {
        AesEncryptBlock(rk, ctr, ek);
        size_t bl = 16; if (off + bl > dataLen) bl = dataLen - off;
        for (size_t i = 0; i < bl; i++) data[off+i] ^= ek[i];
        for (int i = 15; i >= 0; i--) { if (++ctr[i] != 0) break; }
    }
}

static uint32_t Scramble(uint32_t value) {
    value = (value * 0x16A88000) | ((value * 0xCC9E2D51) >> 17);
    value *= 0x1B873593;
    return value;
}
static uint32_t Advance(uint32_t checksum, uint32_t value) {
    checksum ^= Scramble(value);
    checksum = (checksum >> 19) | (checksum << 13);
    checksum = (checksum * 5) + 0xE6546B64;
    return checksum;
}
static uint32_t Finalize(uint32_t checksum, uint32_t length) {
    checksum ^= length;
    checksum ^= checksum >> 16;
    checksum *= 0x85EBCA6B;
    checksum ^= checksum >> 13;
    checksum *= 0xC2B2AE35;
    checksum ^= checksum >> 16;
    return checksum;
}
static uint32_t Murmur3Hash(const uint8_t* data, size_t len) {
    uint32_t checksum = 0;
    const uint32_t* u32 = (const uint32_t*)data;
    for (size_t i = 0; i < len / 4; i++) checksum = Advance(checksum, u32[i]);
    return Finalize(checksum, (uint32_t)len);
}
static int GetLength(const uint8_t* data, size_t maxLen, uint32_t expect) {
    uint32_t checksum = 0;
    const uint32_t* u32 = (const uint32_t*)data;
    for (size_t i = 0; i < maxLen / 4; i++) {
        checksum = Advance(checksum, u32[i]);
        uint32_t length = (uint32_t)((i + 1) * 4);
        if (Finalize(checksum, length) == expect) return (int)length;
    }
    return -1;
}

static uint32_t LE32(const uint8_t* p) { return (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24); }
static void WriteLE32(uint8_t* p, uint32_t v) { p[0]=v&0xFF; p[1]=(v>>8)&0xFF; p[2]=(v>>16)&0xFF; p[3]=(v>>24)&0xFF; }
static uint16_t LE16(const uint8_t* p) { return (uint16_t)p[0] | ((uint16_t)p[1]<<8); }
static void WriteLE16(uint8_t* p, uint16_t v) { p[0]=v&0xFF; p[1]=(v>>8)&0xFF; }
static uint8_t CalcChecksum(uint32_t v) { uint32_t s = v + (v>>16) + (v>>24) + (v>>8); return (uint8_t)(s - 0x2D); }
static uint32_t rotr32(uint32_t x, int r) { r &= 31; return (x >> r) | (x << ((32 - r) & 31)); }
static uint32_t rotl32(uint32_t x, int r) { r &= 31; return (x << r) | (x >> ((32 - r) & 31)); }
static uint32_t DecInt(uint32_t enc, uint8_t shift, uint16_t adjust) { return rotr32(enc, shift + 3) + 0x80E32B11u - adjust; }
static uint32_t EncInt(uint32_t value, uint8_t shift, uint16_t adjust) { return rotl32(value + adjust - 0x80E32B11u, shift + 3); }

struct Region { uint32_t off; uint32_t len; };
const uint32_t WALLET_OFF = 0x37D60;
const uint32_t BANK_OFF   = 0x6527C;
const uint32_t POCKETS_BASE = 0x37CA8;

static void applyEdit(uint8_t* p, uint32_t off, uint32_t value) {
    uint16_t adjust = (uint16_t)(p[off+4] | (p[off+5] << 8));
    uint8_t shift = p[off+6];
    uint32_t enc = EncInt(value, shift, adjust);
    WriteLE32(p + off, enc);
    p[off+7] = CalcChecksum(enc);
}

static uint8_t* readSaveFile(FsFileSystem* fs, const char* path, size_t& outSize) {
    FsFile f;
    if (R_FAILED(fsFsOpenFile(fs, path, FsOpenMode_Read, &f))) return nullptr;
    s64 sz = 0;
    fsFileGetSize(&f, &sz);
    uint8_t* buf = (uint8_t*)malloc((size_t)sz);
    if (buf) { u64 rd = 0; fsFileRead(&f, 0, buf, (u64)sz, FsReadOption_None, &rd); }
    fsFileClose(&f);
    outSize = (size_t)sz;
    return buf;
}

static char bakMsg[96];
static bool backupToSd(const uint8_t* data, size_t size) {
    FsFileSystem sd;
    Result rc = fsOpenSdCardFileSystem(&sd);
    if (R_FAILED(rc)) { snprintf(bakMsg, sizeof(bakMsg), "BAK sd open 0x%X", rc); return false; }
    const char* path = "/switch/acnh_editor/backup_test.dat";
    fsFsDeleteFile(&sd, path);
    Result cr = fsFsCreateFile(&sd, path, size, 0);
    if (R_FAILED(cr)) { snprintf(bakMsg, sizeof(bakMsg), "BAK create 0x%X", cr); fsFsClose(&sd); return false; }
    FsFile f;
    Result orr = fsFsOpenFile(&sd, path, FsOpenMode_Write, &f);
    if (R_FAILED(orr)) { snprintf(bakMsg, sizeof(bakMsg), "BAK open 0x%X", orr); fsFsClose(&sd); return false; }
    Result wr = fsFileWrite(&f, 0, data, size, FsWriteOption_Flush);
    fsFileClose(&f);
    if (R_FAILED(wr)) { snprintf(bakMsg, sizeof(bakMsg), "BAK write 0x%X", wr); fsFsClose(&sd); return false; }
    fsFsClose(&sd);
    return true;
}

static char* itemText = nullptr;
static const char* itemName(int id) {
    static char buf[96];
    buf[0] = 0;
    if (!itemText || id < 0) return buf;
    const char* p = itemText;
    for (int cur = 0; cur < id && *p; ) {
        if (*p == '\n') cur++;
        p++;
    }
    int i = 0;
    while (*p && *p != '\n' && *p != '\r' && i < 95) buf[i++] = *p++;
    buf[i] = 0;
    return buf;
}

// ================= GRAPHICS SHELL (M3) =================
static SDL_Renderer* g_ren = nullptr;
static TTF_Font* fTitle = nullptr;
static TTF_Font* fLine = nullptr;
static TTF_Font* fSmall = nullptr;

static SDL_Texture* makeText(TTF_Font* f, const char* s, SDL_Color c) {
    if (!f) return nullptr;
    SDL_Surface* surf = TTF_RenderUTF8_Blended(f, s, c);
    if (!surf) return nullptr;
    SDL_Texture* t = SDL_CreateTextureFromSurface(g_ren, surf);
    SDL_FreeSurface(surf);
    return t;
}

struct UiItem { SDL_Texture* t; int x, y; };
static std::vector<UiItem> ui;
static SDL_Rect hl = { 0, 0, 0, 0 };
static bool hlOn = false;

// UI-relevant state (shared with buildUI)
static uint32_t g_vals[2] = { 0, 0 };
static int g_slot = 1;
static int32_t g_newId = 0, g_newCount = 1;
static int g_sel = 0;
static const char* g_status = "Ready.";

static void addText(TTF_Font* f, const char* s, SDL_Color c, int x, int y) {
    SDL_Texture* t = makeText(f, s, c);
    if (t) ui.push_back({ t, x, y });
}

static void buildUI() {
    for (auto& it : ui) SDL_DestroyTexture(it.t);
    ui.clear();
    hlOn = false;
    SDL_Color cream = { 0xF8, 0xF5, 0xEC, 255 };
    SDL_Color brown = { 0x7A, 0x5C, 0x3E, 255 };
    SDL_Color red   = { 0xC0, 0x39, 0x2B, 255 };
    SDL_Color okc   = { 0x2E, 0x7D, 0x32, 255 };
    char buf[192];
    int y = 246;
    const int LH = 54;

    auto line = [&](int idx, const char* text) {
        if (idx == g_sel) {
            hl = { 260, y - 8, 760, 46 };
            hlOn = true;
            addText(fLine, text, cream, 284, y);
        } else {
            addText(fLine, text, brown, 284, y);
        }
        y += LH;
    };

    snprintf(buf, sizeof buf, "Wallet: %u", (unsigned)g_vals[0]); line(0, buf);
    snprintf(buf, sizeof buf, "Bank:   %u", (unsigned)g_vals[1]); line(1, buf);
    snprintf(buf, sizeof buf, "Slot:   %d/20", g_slot); line(2, buf);
    const char* nm = itemName(g_newId);
    if (g_newId == 0xFFFE) nm = "(empty slot)";
    else if (!nm[0]) nm = "(unnamed/internal)";
    snprintf(buf, sizeof buf, "Item:   %04X  %s", (unsigned)(uint16_t)g_newId, nm); line(3, buf);
    snprintf(buf, sizeof buf, "Count:  %d", (int)g_newCount); line(4, buf);

    addText(fSmall, "Up/Dn select   < > step   L R big   A backup+save   + exit", brown, 284, 546);

    SDL_Color sc = cream;
    if (strstr(g_status, "fail") || strstr(g_status, "BAK") || strstr(g_status, "Mount") || strstr(g_status, "FAILED")) sc = red;
    else if (strncmp(g_status, "SAVED", 5) == 0) sc = okc;
    addText(fSmall, g_status, sc, 284, 640);
}

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("ACNH SE", 0, 0, 1280, 720, SDL_WINDOW_FULLSCREEN);
    g_ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    TTF_Init();
    fTitle = TTF_OpenFont("sdmc:/switch/acnh_editor/font.ttf", 40);
    fLine  = TTF_OpenFont("sdmc:/switch/acnh_editor/font.ttf", 30);
    fSmall = TTF_OpenFont("sdmc:/switch/acnh_editor/font.ttf", 24);

    SDL_Texture* title = makeText(fTitle, "ACNH Save Editor v1.1", (SDL_Color){ 0xF8, 0xF5, 0xEC, 255 });
    int titleW = 0, titleH = 0;
    if (title) SDL_QueryTexture(title, nullptr, nullptr, &titleW, &titleH);

    SDL_Texture* icon = nullptr;
    {
        int iw = 0, ih = 0, comp = 0;
        unsigned char* pixels = stbi_load("sdmc:/switch/acnh_editor/icon.png", &iw, &ih, &comp, 4);
        if (pixels) {
            icon = SDL_CreateTexture(g_ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, iw, ih);
            SDL_UpdateTexture(icon, nullptr, pixels, iw * 4);
            SDL_SetTextureBlendMode(icon, SDL_BLENDMODE_BLEND);
            stbi_image_free(pixels);
        }
    }

    // ---- v1.0 save loading ----
    FILE* it = fopen("sdmc:/switch/acnh_editor/items.txt", "rb");
    if (it) {
        fseek(it, 0, SEEK_END);
        long tsz = ftell(it);
        fseek(it, 0, SEEK_SET);
        itemText = (char*)malloc((size_t)tsz + 1);
        if (itemText) {
            fread(itemText, 1, (size_t)tsz, it);
            itemText[tsz] = 0;
        }
        fclose(it);
    }
    if (!itemText) g_status = "No items.txt on SD!";

    FsFileSystem fs;
    FsSaveDataAttribute attr;
    memset(&attr, 0, sizeof(attr));
    attr.application_id = 0x01006F8002326000ull;
    attr.save_data_type = 3;
    Result rc = fsOpenSaveDataFileSystem(&fs, FsSaveDataSpaceId_User, &attr);

    size_t size = 0, hdrSize = 0;
    uint8_t* personal = nullptr;
    uint8_t* header = nullptr;
    uint8_t* original = nullptr;

    if (R_SUCCEEDED(rc)) {
        personal = readSaveFile(&fs, "/Villager0/personal.dat", size);
        header = readSaveFile(&fs, "/Villager0/personalHeader.dat", hdrSize);
        if (personal) {
            original = (uint8_t*)malloc(size);
            if (original) memcpy(original, personal, size);
        }
    } else {
        g_status = "Mount failed! Close ACNH fully first.";
    }

    Region regions[16];
    int regionCount = 0;

    if (personal && header) {
        CryptACNH(header, personal, size);

        size_t offset = 0x100;
        while (offset + 4 < size && regionCount < 16) {
            uint32_t hash = LE32(personal + offset);
            int len = GetLength(personal + offset + 4, size - offset - 4, hash);
            if (len == -1) { offset += 0x10; continue; }
            regions[regionCount].off = (uint32_t)offset;
            regions[regionCount].len = (uint32_t)len;
            regionCount++;
            offset += 4 + len;
        }

        {
            uint32_t e = LE32(personal + WALLET_OFF);
            uint16_t a = (uint16_t)(personal[WALLET_OFF+4] | (personal[WALLET_OFF+5] << 8));
            g_vals[0] = DecInt(e, personal[WALLET_OFF+6], a);
        }
        {
            uint32_t e = LE32(personal + BANK_OFF);
            uint16_t a = (uint16_t)(personal[BANK_OFF+4] | (personal[BANK_OFF+5] << 8));
            g_vals[1] = DecInt(e, personal[BANK_OFF+6], a);
        }

        const uint8_t* rec = personal + POCKETS_BASE;
        g_newId = LE16(rec);
        g_newCount = LE16(rec + 4);
    } else if (R_SUCCEEDED(rc)) {
        g_status = "FAILED to read NAND files!";
    }

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    bool needDraw = true;
    int hf = 0;

    while (appletMainLoop()) {
        padUpdate(&pad);
        u64 k = padGetButtonsDown(&pad);
        u64 held = padGetButtons(&pad);
        if (k & HidNpadButton_Plus) break;

        if (held & (HidNpadButton_Left|HidNpadButton_Right|HidNpadButton_L|HidNpadButton_R)) {
            hf++;
            if (hf > 20 && (hf % 5 == 0))
                k |= held & (HidNpadButton_Left|HidNpadButton_Right|HidNpadButton_L|HidNpadButton_R);
        } else hf = 0;

        if (personal && header && original) {
            if (k & HidNpadButton_Up)   { g_sel = (g_sel + 4) % 5; needDraw = true; }
            if (k & HidNpadButton_Down) { g_sel = (g_sel + 1) % 5; needDraw = true; }

            int64_t d = 0, big = 0;
            if (k & HidNpadButton_Left)  d = -1;
            if (k & HidNpadButton_Right) d = +1;
            if (k & HidNpadButton_L)     big = -1;
            if (k & HidNpadButton_R)     big = +1;
            if (d || big) {
                if (g_sel == 0 || g_sel == 1) {
                    int64_t nv = (int64_t)g_vals[g_sel] + d * 1000 + big * 100000;
                    if (nv < 0) nv = 0;
                    if (nv > 999999999) nv = 999999999;
                    g_vals[g_sel] = (uint32_t)nv;
                } else if (g_sel == 2) {
                    int ns = g_slot + (int)(d + big * 5);
                    if (ns < 1) ns = 1;
                    if (ns > 20) ns = 20;
                    if (ns != g_slot) {
                        g_slot = ns;
                        const uint8_t* r2 = personal + POCKETS_BASE + (g_slot - 1) * 8;
                        g_newId = LE16(r2);
                        g_newCount = LE16(r2 + 4);
                    }
                } else if (g_sel == 3) {
                    int64_t nv = g_newId + d + big * 256;
                    if (nv < 0) nv = 0;
                    if (nv > 65534) nv = 65534;
                    g_newId = (int32_t)nv;
                } else {
                    int64_t nv = g_newCount + d + big * 10;
                    if (nv < 1) nv = 1;
                    if (nv > 99) nv = 99;
                    g_newCount = (int32_t)nv;
                }
                needDraw = true;
            }

            if (k & HidNpadButton_A) {
                if (!backupToSd(original, size)) {
                    g_status = bakMsg;
                } else {
                    uint8_t* out = (uint8_t*)malloc(size);
                    if (out) {
                        memcpy(out, personal, size);
                        applyEdit(out, WALLET_OFF, g_vals[0]);
                        applyEdit(out, BANK_OFF, g_vals[1]);
                        uint8_t* r = out + POCKETS_BASE + (g_slot - 1) * 8;
                        WriteLE16(r, (uint16_t)g_newId);
                        WriteLE16(r + 4, (uint16_t)g_newCount);
                        for (int i = 0; i < regionCount; i++)
                            WriteLE32(out + regions[i].off, Murmur3Hash(out + regions[i].off + 4, regions[i].len));
                        CryptACNH(header, out, size);

                        FsFile f;
                        Result wr = fsFsOpenFile(&fs, "/Villager0/personal.dat", FsOpenMode_Write, &f);
                        if (R_SUCCEEDED(wr)) {
                            Result w2 = fsFileWrite(&f, 0, out, size, FsWriteOption_Flush);
                            fsFileClose(&f);
                            if (R_SUCCEEDED(w2)) {
                                fsFsCommit(&fs);
                                memcpy(original, out, size);
                                uint8_t* pr = personal + POCKETS_BASE + (g_slot - 1) * 8;
                                WriteLE16(pr, (uint16_t)g_newId);
                                WriteLE16(pr + 4, (uint16_t)g_newCount);
                                g_status = "SAVED to NAND + SD backup ok.";
                            } else g_status = "write failed!";
                        } else g_status = "open-for-write failed!";
                        free(out);
                    } else g_status = "malloc failed!";
                }
                needDraw = true;
            }
        }

        svcSleepThread(16000000);

        if (needDraw) {
            needDraw = false;
            buildUI();
        }

        SDL_SetRenderDrawColor(g_ren, 0x7A, 0xC1, 0x43, 0xFF);
        SDL_RenderClear(g_ren);

        SDL_SetRenderDrawColor(g_ren, 0xF8, 0xF5, 0xEC, 0xFF);
        SDL_Rect panel = { 240, 140, 800, 440 };
        SDL_RenderFillRect(g_ren, &panel);

        SDL_SetRenderDrawColor(g_ren, 0x7A, 0x5C, 0x3E, 0xFF);
        SDL_Rect bar = { 240, 140, 800, 70 };
        SDL_RenderFillRect(g_ren, &bar);

        if (title) {
            SDL_Rect dst = { 640 - titleW / 2, 140 + (70 - titleH) / 2, titleW, titleH };
            SDL_RenderCopy(g_ren, title, nullptr, &dst);
        }
        if (icon) {
            SDL_Rect dst = { 976, 147, 56, 56 };
            SDL_RenderCopy(g_ren, icon, nullptr, &dst);
        }
        if (hlOn) {
            SDL_SetRenderDrawColor(g_ren, 0x6A, 0xA8, 0x4F, 0xFF);
            SDL_RenderFillRect(g_ren, &hl);
        }
        for (auto& u : ui) {
            SDL_Rect dst = { u.x, u.y, 0, 0 };
            SDL_QueryTexture(u.t, nullptr, nullptr, &dst.w, &dst.h);
            SDL_RenderCopy(g_ren, u.t, nullptr, &dst);
        }

        SDL_RenderPresent(g_ren);
    }

    for (auto& u : ui) SDL_DestroyTexture(u.t);
    if (title) SDL_DestroyTexture(title);
    if (icon) SDL_DestroyTexture(icon);
    if (fTitle) TTF_CloseFont(fTitle);
    if (fLine) TTF_CloseFont(fLine);
    if (fSmall) TTF_CloseFont(fSmall);
    TTF_Quit();
    if (personal) free(personal);
    if (header) free(header);
    if (original) free(original);
    if (itemText) free(itemText);
    if (R_SUCCEEDED(rc)) fsFsClose(&fs);
    SDL_DestroyRenderer(g_ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}