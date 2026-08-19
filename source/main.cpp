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

// ================= v1.2 DUAL-FILE SAVE ENGINE =================
struct XorShift128 {
    uint32_t a, b, c, d;
    XorShift128(uint32_t seed) {
        uint32_t Mersenne = 0x6C078965;
        a = (Mersenne * (seed ^ (seed >> 30))) + 1;
        b = (Mersenne * (a   ^ (a   >> 30))) + 2;
        c = (Mersenne * (b   ^ (b   >> 30))) + 3;
        d = (Mersenne * (c   ^ (c   >> 30))) + 4;
    }
    uint32_t Next() { uint32_t t = a; a = b; b = c; c = d; t ^= t << 11; t ^= t >> 8; return d = t ^ d ^ (d >> 19); }
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
            s[c*4]   ^= x ^ xtime(a^b); s[c*4+1] ^= x ^ xtime(b^d);
            s[c*4+2] ^= x ^ xtime(d^e); s[c*4+3] ^= x ^ xtime(e^a);
        }
        for (int i = 0; i < 16; i++) s[i] ^= rk[r*16+i];
    }
    for (int i = 0; i < 16; i++) s[i] = sbox[s[i]];
    { uint8_t t; t=s[1]; s[1]=s[5]; s[5]=s[9]; s[9]=s[13]; s[13]=t;
      t=s[2]; s[2]=s[10]; s[10]=t; t=s[6]; s[6]=s[14]; s[14]=t;
      t=s[15]; s[15]=s[11]; s[11]=s[7]; s[7]=s[3]; s[3]=t; }
    for (int i = 0; i < 16; i++) out[i] = s[i] ^ rk[160+i];
}

void CryptACNH(const uint8_t* header, uint8_t* data, size_t dataLen) {
    const uint32_t* imp = (const uint32_t*)(header + 0x100);
    uint8_t key[16], ctr[16];
    GetParam(imp, 0, key, 16); GetParam(imp, 2, ctr, 16);
    uint8_t rk[176]; KeyExpansion(key, rk);
    uint8_t ek[16];
    for (size_t off = 0; off < dataLen; off += 16) {
        AesEncryptBlock(rk, ctr, ek);
        size_t bl = 16; if (off + bl > dataLen) bl = dataLen - off;
        for (size_t i = 0; i < bl; i++) data[off+i] ^= ek[i];
        for (int i = 15; i >= 0; i--) { if (++ctr[i] != 0) break; }
    }
}

static uint32_t Scramble(uint32_t v) { v = (v * 0x16A88000) | ((v * 0xCC9E2D51) >> 17); v *= 0x1B873593; return v; }
static uint32_t Advance(uint32_t c, uint32_t v) { c ^= Scramble(v); c = (c >> 19) | (c << 13); c = (c * 5) + 0xE6546B64; return c; }
static uint32_t Finalize(uint32_t c, uint32_t len) { c ^= len; c ^= c >> 16; c *= 0x85EBCA6B; c ^= c >> 13; c *= 0xC2B2AE35; c ^= c >> 16; return c; }
static uint32_t Murmur3Hash(const uint8_t* data, size_t len) {
    uint32_t c = 0; const uint32_t* u32 = (const uint32_t*)data;
    for (size_t i = 0; i < len / 4; i++) c = Advance(c, u32[i]);
    return Finalize(c, (uint32_t)len);
}
static int GetLength(const uint8_t* data, size_t maxLen, uint32_t expect) {
    uint32_t c = 0; const uint32_t* u32 = (const uint32_t*)data;
    for (size_t i = 0; i < maxLen / 4; i++) {
        c = Advance(c, u32[i]);
        uint32_t l = (uint32_t)((i + 1) * 4);
        if (Finalize(c, l) == expect) return (int)l;
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
const uint32_t MILES_OFF  = 0x12718;
const uint32_t LOAN_OFF   = 0x33CBF0;
const uint32_t POCKETS_BASE = 0x37CA8;

struct Fav { int id; const char* label; };
static const Fav FAVS[] = {
    { 5851, "Nook Miles Ticket" },
    { 5983, "Star Fragment" },
    { 5984, "Large Star Fragment" },
    { 2505, "Gold Nugget" },
    { 2511, "Iron Nugget" },
    { 2513, "Customization Kit" },
    { 8179, "Golden Net" },
    { 8533, "Golden Watering Can" },
    { 8574, "Golden Shovel" },
    { 8578, "Golden Slingshot" },
    { 8660, "Golden Rod" },
    { 9617, "Golden Axe" },
};
static const int FAV_COUNT = 12;

static const char* LOADOUT_NAMES[] = {"Mining", "Fishing", "Terraforming", "Custom 1", "Custom 2"};
static const int LOADOUT_COUNT = 5;
static bool g_loadoutExists[5] = {false};
static int g_loadoutCount[5] = {0};
static int g_loadoutSel = 0;

static void applyEdit(uint8_t* p, uint32_t off, uint32_t value) {
    uint16_t adjust = (uint16_t)(p[off+4] | (p[off+5] << 8));
    uint8_t shift = p[off+6];
    uint32_t enc = EncInt(value, shift, adjust);
    WriteLE32(p + off, enc);
    p[off+7] = CalcChecksum(enc);
}

static uint8_t* readSaveFile(FsFileSystem* fs, const char* path, size_t& outSize) {
    FsFile f; if (R_FAILED(fsFsOpenFile(fs, path, FsOpenMode_Read, &f))) return nullptr;
    s64 sz = 0; fsFileGetSize(&f, &sz);
    uint8_t* buf = (uint8_t*)malloc((size_t)sz);
    if (buf) { u64 rd = 0; fsFileRead(&f, 0, buf, (u64)sz, FsReadOption_None, &rd); }
    fsFileClose(&f); outSize = (size_t)sz; return buf;
}

static char bakMsg[128];
static char loMsg[128];
static bool backupToSd(const uint8_t* pers, size_t pSz, const uint8_t* main, size_t mSz) {
    FsFileSystem sd; Result rc = fsOpenSdCardFileSystem(&sd);
    if (R_FAILED(rc)) { snprintf(bakMsg, sizeof(bakMsg), "BAK sd open 0x%X", rc); return false; }
    auto writeOne = [&](const char* p, const uint8_t* d, size_t s) -> bool {
        fsFsDeleteFile(&sd, p);
        if (R_FAILED(fsFsCreateFile(&sd, p, s, 0))) return false;
        FsFile f; if (R_FAILED(fsFsOpenFile(&sd, p, FsOpenMode_Write, &f))) return false;
        Result wr = fsFileWrite(&f, 0, d, s, FsWriteOption_Flush);
        fsFileClose(&f); return R_SUCCEEDED(wr);
    };
    bool ok = writeOne("/switch/acnh_editor/backup_personal.dat", pers, pSz) &&
              writeOne("/switch/acnh_editor/backup_main.dat", main, mSz);
    fsFsClose(&sd);
    if (!ok) snprintf(bakMsg, sizeof(bakMsg), "BAK write failed!");
    return ok;
}

static char* itemText = nullptr;
static const char* itemName(int id) {
    static char buf[96]; buf[0] = 0;
    if (!itemText || id < 0) return buf;
    const char* p = itemText;
    for (int cur = 0; cur < id && *p; ) { if (*p == '\n') cur++; p++; }
    int i = 0; while (*p && *p != '\n' && *p != '\r' && i < 95) buf[i++] = *p++;
    buf[i] = 0; return buf;
}

// ================= GRAPHICS SHELL =================
static SDL_Renderer* g_ren = nullptr;
static TTF_Font* fTitle = nullptr, *fLine = nullptr, *fSmall = nullptr;
static SDL_Texture* makeText(TTF_Font* f, const char* s, SDL_Color c) {
    if (!f) return nullptr;
    SDL_Surface* surf = TTF_RenderUTF8_Blended(f, s, c);
    if (!surf) return nullptr;
    SDL_Texture* t = SDL_CreateTextureFromSurface(g_ren, surf);
    SDL_FreeSurface(surf); return t;
}

struct UiItem { SDL_Texture* t; int x, y; };
static std::vector<UiItem> ui;
static SDL_Rect hl = { 0, 0, 0, 0 };
static bool hlOn = false;

static uint32_t g_vals[4] = { 0, 0, 0, 0 };
static int g_slot = 1;
static int32_t g_newId = 0, g_newCount = 1;
static int g_sel = 0;
static int g_screen = 0;
static int g_favSel = 0;
static int g_favCount = 1;
static const char* g_status = "Ready. ZL restores SD backup.";

static void addText(TTF_Font* f, const char* s, SDL_Color c, int x, int y) {
    SDL_Texture* t = makeText(f, s, c); if (t) ui.push_back({ t, x, y });
}

static void buildUI(uint8_t* personal) {
    for (auto& it : ui) SDL_DestroyTexture(it.t);
    ui.clear();
    hlOn = false;
    SDL_Color cream = { 0xF8, 0xF5, 0xEC, 255 }, brown = { 0x7A, 0x5C, 0x3E, 255 };
    SDL_Color red = { 0xC0, 0x39, 0x2B, 255 }, okc = { 0x2E, 0x7D, 0x32, 255 };
    char buf[192];

    if (g_screen == 0) {
        int y = 236; const int LH = 40;
        auto line = [&](int idx, const char* text) {
            if (idx == g_sel) { hl = { 260, y - 6, 760, 36 }; hlOn = true; addText(fLine, text, cream, 284, y); }
            else { addText(fLine, text, brown, 284, y); }
            y += LH;
        };
        snprintf(buf, sizeof buf, "Wallet: %u", (unsigned)g_vals[0]); line(0, buf);
        snprintf(buf, sizeof buf, "Bank:   %u", (unsigned)g_vals[1]); line(1, buf);
        snprintf(buf, sizeof buf, "Miles:  %u", (unsigned)g_vals[2]); line(2, buf);
        snprintf(buf, sizeof buf, "Loan:   %u", (unsigned)g_vals[3]); line(3, buf);
        snprintf(buf, sizeof buf, "Slot:   %d/20", g_slot); line(4, buf);
        const char* nm = itemName(g_newId);
        if (g_newId == 0xFFFE) nm = "(empty slot)"; else if (!nm[0]) nm = "(unnamed)";
        snprintf(buf, sizeof buf, "Item:   %04X  %s", (unsigned)(uint16_t)g_newId, nm); line(5, buf);
        snprintf(buf, sizeof buf, "Count:  %d", (int)g_newCount); line(6, buf);
        addText(fSmall, "Up/Dn sel < > step L R big A save - clear X favs Y loadouts + exit", brown, 284, 548);
    } else if (g_screen == 1) {
        int y = 222; const int LH = 27;
        for (int i = 0; i < FAV_COUNT; i++) {
            if (i == g_favSel) { hl = { 260, y - 4, 760, 28 }; hlOn = true; addText(fSmall, FAVS[i].label, cream, 284, y); }
            else { addText(fSmall, FAVS[i].label, brown, 284, y); }
            y += LH;
        }
        snprintf(buf, sizeof buf, "Count: %d   |   < > count   A pick slot   X back   + exit", g_favCount);
        addText(fSmall, buf, brown, 284, 550);
    } else if (g_screen == 2) {
        int y0 = 222; const int LH = 30;
        for (int s = 1; s <= 20; s++) {
            int col = (s <= 10) ? 0 : 1;
            int row = (s <= 10) ? (s - 1) : (s - 11);
            int x = col ? 660 : 284;
            int y = y0 + row * LH;
            const uint8_t* rec = personal + POCKETS_BASE + (s - 1) * 8;
            uint16_t id = LE16(rec); uint16_t cnt = LE16(rec + 4);
            char linebuf[64];
            if (id == 0xFFFE) snprintf(linebuf, sizeof linebuf, "%2d (empty)", s);
            else snprintf(linebuf, sizeof linebuf, "%2d %-16.16s x%d", s, itemName(id), cnt);
            if (s == g_slot) { hl = { x - 8, y - 4, 360, 28 }; hlOn = true; addText(fSmall, linebuf, cream, x, y); }
            else addText(fSmall, linebuf, brown, x, y);
        }
        const char* pnm = itemName(g_newId);
        snprintf(buf, sizeof buf, "Inject: %-16.16s x%d   |   X back   A save   + exit", pnm, (int)g_newCount);
        addText(fSmall, buf, brown, 284, 550);
    } else {
        int y = 222; const int LH = 40;
        for (int i = 0; i < LOADOUT_COUNT; i++) {
            char linebuf[80];
            if (g_loadoutExists[i]) snprintf(linebuf, sizeof(linebuf), "%-12s [Saved - %d/20 items]", LOADOUT_NAMES[i], g_loadoutCount[i]);
            else snprintf(linebuf, sizeof(linebuf), "%-12s [Empty]", LOADOUT_NAMES[i]);
            if (i == g_loadoutSel) { hl = { 260, y - 6, 760, 36 }; hlOn = true; addText(fLine, linebuf, cream, 284, y); }
            else { addText(fLine, linebuf, brown, 284, y); }
            y += LH;
        }
        addText(fSmall, "Up/Dn select  A Save to SD  B Load from SD  X back  + exit", brown, 284, 548);
    }

    SDL_Color sc = cream;
    if (strstr(g_status, "fail") || strstr(g_status, "BAK") || strstr(g_status, "Mount") || strstr(g_status, "FAILED") || strstr(g_status, "ERR")) sc = red;
    else if (strncmp(g_status, "SAVED", 5) == 0 || strncmp(g_status, "Saved", 5) == 0 || strncmp(g_status, "Loaded", 6) == 0 || strncmp(g_status, "RESTORED", 8) == 0) sc = okc;
    addText(fSmall, g_status, sc, 284, 640);
}

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("ACNH SE", 0, 0, 1280, 720, SDL_WINDOW_FULLSCREEN);
    g_ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    TTF_Init();
    fTitle = TTF_OpenFont("sdmc:/switch/acnh_editor/font.ttf", 40);
    fLine  = TTF_OpenFont("sdmc:/switch/acnh_editor/font.ttf", 28);
    fSmall = TTF_OpenFont("sdmc:/switch/acnh_editor/font.ttf", 24);

    SDL_Color creamC = { 0xF8, 0xF5, 0xEC, 255 };
    SDL_Texture* title = makeText(fTitle, "ACNH Save Editor v1.2", creamC);
    int titleW = 0, titleH = 0;
    if (title) SDL_QueryTexture(title, nullptr, nullptr, &titleW, &titleH);

    SDL_Texture* icon = nullptr;
    { int iw=0,ih=0,c=0; unsigned char* px = stbi_load("sdmc:/switch/acnh_editor/icon.png", &iw, &ih, &c, 4);
      if (px) { icon = SDL_CreateTexture(g_ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, iw, ih);
                SDL_UpdateTexture(icon, nullptr, px, iw * 4); SDL_SetTextureBlendMode(icon, SDL_BLENDMODE_BLEND); stbi_image_free(px); } }

    FILE* it = fopen("sdmc:/switch/acnh_editor/items.txt", "rb");
    if (it) { fseek(it,0,SEEK_END); long tsz=ftell(it); fseek(it,0,SEEK_SET);
              itemText=(char*)malloc(tsz+1); if(itemText){fread(itemText,1,tsz,it); itemText[tsz]=0;} fclose(it); }
    if (!itemText) g_status = "No items.txt on SD!";

    FsFileSystem fs; FsSaveDataAttribute attr; memset(&attr, 0, sizeof(attr));
    attr.application_id = 0x01006F8002326000ull; attr.save_data_type = 3;
    Result rc = fsOpenSaveDataFileSystem(&fs, FsSaveDataSpaceId_User, &attr);

    size_t pSz=0, pHdrSz=0, mSz=0, mHdrSz=0;
    uint8_t *personal=nullptr, *pHdr=nullptr, *origP=nullptr;
    uint8_t *mainData=nullptr, *mHdr=nullptr, *origM=nullptr;

    if (R_SUCCEEDED(rc)) {
        personal = readSaveFile(&fs, "/Villager0/personal.dat", pSz);
        pHdr = readSaveFile(&fs, "/Villager0/personalHeader.dat", pHdrSz);
        mainData = readSaveFile(&fs, "/main.dat", mSz);
        mHdr = readSaveFile(&fs, "/mainHeader.dat", mHdrSz);
        if (personal && mainData) {
            origP = (uint8_t*)malloc(pSz); if(origP) memcpy(origP, personal, pSz);
            origM = (uint8_t*)malloc(mSz); if(origM) memcpy(origM, mainData, mSz);
        }
    } else { g_status = "Mount failed! Close ACNH fully first."; }

    Region pReg[16], mReg[32]; int pRegC=0, mRegC=0;

    if (personal && pHdr && mainData && mHdr) {
        CryptACNH(pHdr, personal, pSz); CryptACNH(mHdr, mainData, mSz);

        auto findRegions = [&](uint8_t* buf, size_t sz, Region* arr, int& cnt, int maxR) {
            size_t off = 0x100;
            while (off + 4 < sz && cnt < maxR) {
                uint32_t hash = LE32(buf + off);
                int len = GetLength(buf + off + 4, sz - off - 4, hash);
                if (len == -1) { off += 0x10; continue; }
                arr[cnt].off = (uint32_t)off; arr[cnt].len = (uint32_t)len; cnt++; off += 4 + len;
            }
        };
        findRegions(personal, pSz, pReg, pRegC, 16);
        findRegions(mainData, mSz, mReg, mRegC, 32);

        g_vals[0] = DecInt(LE32(personal + WALLET_OFF), personal[WALLET_OFF+6], LE16(personal + WALLET_OFF + 4));
        g_vals[1] = DecInt(LE32(personal + BANK_OFF), personal[BANK_OFF+6], LE16(personal + BANK_OFF + 4));
        g_vals[2] = DecInt(LE32(personal + MILES_OFF), personal[MILES_OFF+6], LE16(personal + MILES_OFF + 4));
        g_vals[3] = DecInt(LE32(mainData + LOAN_OFF), mainData[LOAN_OFF+6], LE16(mainData + LOAN_OFF + 4));

        const uint8_t* rec = personal + POCKETS_BASE;
        g_newId = LE16(rec); g_newCount = LE16(rec + 4);
    } else if (R_SUCCEEDED(rc)) { g_status = "FAILED to read NAND files!"; }

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad; padInitializeDefault(&pad);
    bool needDraw = true; int hf = 0;

    auto checkLoadouts = [&]() {
        for (int i = 0; i < LOADOUT_COUNT; i++) {
            char path[96]; snprintf(path, sizeof(path), "sdmc:/switch/acnh_editor/l_%d.dat", i);
            g_loadoutExists[i] = false;
            g_loadoutCount[i] = 0;
            FILE* f = fopen(path, "rb");
            if (f) {
                g_loadoutExists[i] = true;
                uint8_t tmp[160];
                size_t rd = fread(tmp, 1, 160, f);
                if (rd == 160) {
                    int n = 0;
                    for (int s2 = 0; s2 < 20; s2++) {
                        uint16_t idv = LE16(tmp + s2 * 8);
                        if (idv != 0xFFFE) n++;
                    }
                    g_loadoutCount[i] = n;
                }
                fclose(f);
            }
        }
    };

    auto doSave = [&]() {
        if (!backupToSd(origP, pSz, origM, mSz)) { g_status = bakMsg; }
        else {
            uint8_t* outP = (uint8_t*)malloc(pSz);
            uint8_t* outM = (uint8_t*)malloc(mSz);
            if (outP && outM) {
                memcpy(outP, personal, pSz); memcpy(outM, mainData, mSz);
                applyEdit(outP, WALLET_OFF, g_vals[0]);
                applyEdit(outP, BANK_OFF, g_vals[1]);
                applyEdit(outP, MILES_OFF, g_vals[2]);
                applyEdit(outM, LOAN_OFF, g_vals[3]);

                uint8_t* r = outP + POCKETS_BASE + (g_slot - 1) * 8;
                WriteLE16(r, (uint16_t)g_newId); WriteLE16(r + 4, (uint16_t)g_newCount);

                for (int i = 0; i < pRegC; i++) WriteLE32(outP + pReg[i].off, Murmur3Hash(outP + pReg[i].off + 4, pReg[i].len));
                for (int i = 0; i < mRegC; i++) WriteLE32(outM + mReg[i].off, Murmur3Hash(outM + mReg[i].off + 4, mReg[i].len));

                CryptACNH(pHdr, outP, pSz); CryptACNH(mHdr, outM, mSz);

                auto writeNand = [&](const char* path, uint8_t* buf, size_t sz) -> bool {
                    FsFile f; if (R_FAILED(fsFsOpenFile(&fs, path, FsOpenMode_Write, &f))) return false;
                    Result w2 = fsFileWrite(&f, 0, buf, sz, FsWriteOption_Flush); fsFileClose(&f);
                    return R_SUCCEEDED(w2);
                };

                if (writeNand("/Villager0/personal.dat", outP, pSz) && writeNand("/main.dat", outM, mSz)) {
                    fsFsCommit(&fs);
                    memcpy(origP, outP, pSz); memcpy(personal, outP, pSz); CryptACNH(pHdr, personal, pSz);
                    memcpy(origM, outM, mSz); memcpy(mainData, outM, mSz); CryptACNH(mHdr, mainData, mSz);
                    uint8_t* pr = personal + POCKETS_BASE + (g_slot - 1) * 8;
                    WriteLE16(pr, (uint16_t)g_newId); WriteLE16(pr + 4, (uint16_t)g_newCount);
                    g_status = "SAVED personal + main to NAND!";
                } else g_status = "NAND write failed!";
            } else g_status = "malloc failed!";
            if (outP) free(outP);
            if (outM) free(outM);
        }
    };

    while (appletMainLoop()) {
        padUpdate(&pad); u64 k = padGetButtonsDown(&pad); u64 held = padGetButtons(&pad);
        if (k & HidNpadButton_Plus) break;

        if (held & (HidNpadButton_Left|HidNpadButton_Right|HidNpadButton_L|HidNpadButton_R)) {
            hf++; if (hf > 20 && (hf % 5 == 0)) k |= held & (HidNpadButton_Left|HidNpadButton_Right|HidNpadButton_L|HidNpadButton_R);
        } else hf = 0;

        if (personal && pHdr && mainData && mHdr && origP && origM) {
            if (k & HidNpadButton_X) {
                if (g_screen == 0) g_screen = 1;
                else if (g_screen == 1) g_screen = 0;
                else if (g_screen == 2) g_screen = 1;
                else g_screen = 0;
                needDraw = true;
            }
            if (k & HidNpadButton_Y && g_screen == 0) {
                g_screen = 3;
                checkLoadouts();
                needDraw = true;
            }

            int64_t d = 0, big = 0;
            if (k & HidNpadButton_Left) d = -1;
            if (k & HidNpadButton_Right) d = +1;
            if (k & HidNpadButton_L) big = -1;
            if (k & HidNpadButton_R) big = +1;

            if (g_screen == 0) {
                if (k & HidNpadButton_Up)   { g_sel = (g_sel + 6) % 7; needDraw = true; }
                if (k & HidNpadButton_Down) { g_sel = (g_sel + 1) % 7; needDraw = true; }

                if (k & HidNpadButton_Minus) { // Clear current pocket slot
                    g_newId = 0xFFFE;
                    g_newCount = 0;
                    uint8_t* pr = personal + POCKETS_BASE + (g_slot - 1) * 8;
                    WriteLE16(pr, 0xFFFE);
                    WriteLE16(pr + 4, 0);
                    g_status = "Slot cleared! Press A to save to NAND.";
                    needDraw = true;
                }

                if (k & HidNpadButton_ZL) { // Restore from SD backup
                    FILE* fp = fopen("sdmc:/switch/acnh_editor/backup_personal.dat", "rb");
                    FILE* fm = fopen("sdmc:/switch/acnh_editor/backup_main.dat", "rb");
                    if (!fp || !fm) {
                        g_status = "Restore failed: backup files missing!";
                    } else {
                        fseek(fp, 0, SEEK_END); long psz2 = ftell(fp); fseek(fp, 0, SEEK_SET);
                        fseek(fm, 0, SEEK_END); long msz2 = ftell(fm); fseek(fm, 0, SEEK_SET);
                        if ((size_t)psz2 != pSz || (size_t)msz2 != mSz) {
                            g_status = "Restore failed: backup size mismatch!";
                        } else {
                            uint8_t* bp = (uint8_t*)malloc(pSz);
                            uint8_t* bm = (uint8_t*)malloc(mSz);
                            bool okRead = bp && bm &&
                                fread(bp, 1, pSz, fp) == pSz &&
                                fread(bm, 1, mSz, fm) == mSz;
                            if (!okRead) {
                                g_status = "Restore failed: backup read error!";
                            } else {
                                auto writeNand2 = [&](const char* path, uint8_t* buf, size_t sz) -> bool {
                                    FsFile f; if (R_FAILED(fsFsOpenFile(&fs, path, FsOpenMode_Write, &f))) return false;
                                    Result w2 = fsFileWrite(&f, 0, buf, sz, FsWriteOption_Flush); fsFileClose(&f);
                                    return R_SUCCEEDED(w2);
                                };
                                if (writeNand2("/Villager0/personal.dat", bp, pSz) && writeNand2("/main.dat", bm, mSz)) {
                                    fsFsCommit(&fs);
                                    memcpy(origP, bp, pSz);
                                    memcpy(origM, bm, mSz);
                                    memcpy(personal, bp, pSz);
                                    memcpy(mainData, bm, mSz);
                                    CryptACNH(pHdr, personal, pSz);
                                    CryptACNH(mHdr, mainData, mSz);
                                    g_vals[0] = DecInt(LE32(personal + WALLET_OFF), personal[WALLET_OFF+6], LE16(personal + WALLET_OFF + 4));
                                    g_vals[1] = DecInt(LE32(personal + BANK_OFF), personal[BANK_OFF+6], LE16(personal + BANK_OFF + 4));
                                    g_vals[2] = DecInt(LE32(personal + MILES_OFF), personal[MILES_OFF+6], LE16(personal + MILES_OFF + 4));
                                    g_vals[3] = DecInt(LE32(mainData + LOAN_OFF), mainData[LOAN_OFF+6], LE16(mainData + LOAN_OFF + 4));
                                    const uint8_t* rec2 = personal + POCKETS_BASE + (g_slot - 1) * 8;
                                    g_newId = LE16(rec2); g_newCount = LE16(rec2 + 4);
                                    g_status = "RESTORED from SD backup!";
                                } else {
                                    g_status = "Restore failed: NAND write failed!";
                                }
                            }
                            if (bp) free(bp);
                            if (bm) free(bm);
                        }
                    }
                    if (fp) fclose(fp);
                    if (fm) fclose(fm);
                    needDraw = true;
                }

                if (d || big) {
                    if (g_sel <= 3) {
                        int64_t maxv = 999999999;
                        if (g_sel == 2 || g_sel == 3) maxv = 9999999;
                        int64_t nv = (int64_t)g_vals[g_sel] + d * 1000 + big * 100000;
                        if (nv < 0) nv = 0;
                        if (nv > maxv) nv = maxv;
                        g_vals[g_sel] = (uint32_t)nv;
                    } else if (g_sel == 4) {
                        int ns = g_slot + (int)(d + big * 5); if (ns < 1) ns = 1; if (ns > 20) ns = 20;
                        if (ns != g_slot) { g_slot = ns; const uint8_t* r2 = personal + POCKETS_BASE + (g_slot - 1) * 8; g_newId = LE16(r2); g_newCount = LE16(r2 + 4); }
                    } else if (g_sel == 5) {
                        int64_t nv = g_newId + d + big * 256; if (nv < 0) nv = 0; if (nv > 65534) nv = 65534; g_newId = (int32_t)nv;
                    } else {
                        int64_t nv = g_newCount + d + big * 10; if (nv < 1) nv = 1; if (nv > 99) nv = 99; g_newCount = (int32_t)nv;
                    }
                    needDraw = true;
                }
                if (k & HidNpadButton_A) { doSave(); needDraw = true; }
            } else if (g_screen == 1) {
                if (k & HidNpadButton_Up)   { g_favSel = (g_favSel + FAV_COUNT - 1) % FAV_COUNT; needDraw = true; }
                if (k & HidNpadButton_Down) { g_favSel = (g_favSel + 1) % FAV_COUNT; needDraw = true; }
                if (d || big) {
                    int nv = g_favCount + (int)(d + big * 10);
                    if (nv < 1) nv = 1;
                    if (nv > 99) nv = 99;
                    g_favCount = nv;
                    needDraw = true;
                }
                if (k & HidNpadButton_A) {
                    g_newId = FAVS[g_favSel].id;
                    g_newCount = g_favCount;
                    g_screen = 2;
                    needDraw = true;
                }
            } else if (g_screen == 2) {
                if (k & HidNpadButton_Up)   { if (g_slot > 1) g_slot--; needDraw = true; }
                if (k & HidNpadButton_Down) { if (g_slot < 20) g_slot++; needDraw = true; }
                if (d || big) {
                    int ns = g_slot + (int)(d + big) * 10;
                    if (ns < 1) ns = 1;
                    if (ns > 20) ns = 20;
                    if (ns != g_slot) { g_slot = ns; needDraw = true; }
                }
                if (k & HidNpadButton_A) { doSave(); needDraw = true; }
            } else {
                if (k & HidNpadButton_Up)   { g_loadoutSel = (g_loadoutSel + LOADOUT_COUNT - 1) % LOADOUT_COUNT; needDraw = true; }
                if (k & HidNpadButton_Down) { g_loadoutSel = (g_loadoutSel + 1) % LOADOUT_COUNT; needDraw = true; }

                if (k & HidNpadButton_A) { // Save Loadout (stdio)
                    char path[96]; snprintf(path, sizeof(path), "sdmc:/switch/acnh_editor/l_%d.dat", g_loadoutSel);
                    FILE* f = fopen(path, "wb");
                    if (f) {
                        size_t wr = fwrite(personal + POCKETS_BASE, 1, 160, f);
                        fclose(f);
                        g_status = (wr == 160) ? "Saved loadout to SD!" : "Loadout write short!";
                    } else {
                        g_status = "LO ERR fopen failed!";
                    }
                    checkLoadouts();
                    needDraw = true;
                }
                if (k & HidNpadButton_B) { // Load Loadout (stdio)
                    char path[96]; snprintf(path, sizeof(path), "sdmc:/switch/acnh_editor/l_%d.dat", g_loadoutSel);
                    FILE* f = fopen(path, "rb");
                    if (f) {
                        size_t rd = fread(personal + POCKETS_BASE, 1, 160, f);
                        fclose(f);
                        if (rd == 160) {
                            g_newId = LE16(personal + POCKETS_BASE + (g_slot - 1) * 8);
                            g_newCount = LE16(personal + POCKETS_BASE + (g_slot - 1) * 8 + 4);
                            g_status = "Loaded loadout! Press A to save to NAND.";
                            g_screen = 0;
                        } else {
                            g_status = "Loadout file too small!";
                        }
                    } else {
                        g_status = "Loadout file not found!";
                    }
                    needDraw = true;
                }
            }
        }
        svcSleepThread(16000000);
        if (needDraw) { needDraw = false; buildUI(personal); }

        SDL_SetRenderDrawColor(g_ren, 0x7A, 0xC1, 0x43, 0xFF); SDL_RenderClear(g_ren);
        SDL_SetRenderDrawColor(g_ren, 0xF8, 0xF5, 0xEC, 0xFF);
        SDL_Rect panel = { 240, 140, 800, 440 }; SDL_RenderFillRect(g_ren, &panel);
        SDL_SetRenderDrawColor(g_ren, 0x7A, 0x5C, 0x3E, 0xFF);
        SDL_Rect bar = { 240, 140, 800, 70 }; SDL_RenderFillRect(g_ren, &bar);
        if (title) { SDL_Rect dst = { 640 - titleW / 2, 140 + (70 - titleH) / 2, titleW, titleH }; SDL_RenderCopy(g_ren, title, nullptr, &dst); }
        if (icon) { SDL_Rect dst = { 976, 147, 56, 56 }; SDL_RenderCopy(g_ren, icon, nullptr, &dst); }
        if (hlOn) { SDL_SetRenderDrawColor(g_ren, 0x6A, 0xA8, 0x4F, 0xFF); SDL_RenderFillRect(g_ren, &hl); }
        for (auto& u : ui) { SDL_Rect dst = { u.x, u.y, 0, 0 }; SDL_QueryTexture(u.t, nullptr, nullptr, &dst.w, &dst.h); SDL_RenderCopy(g_ren, u.t, nullptr, &dst); }
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
    if (pHdr) free(pHdr);
    if (origP) free(origP);
    if (mainData) free(mainData);
    if (mHdr) free(mHdr);
    if (origM) free(origM);
    if (itemText) free(itemText);
    if (R_SUCCEEDED(rc)) fsFsClose(&fs);
    SDL_DestroyRenderer(g_ren); SDL_DestroyWindow(win); SDL_Quit();
    return 0;
}