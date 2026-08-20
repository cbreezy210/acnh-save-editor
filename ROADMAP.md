# 🗺️ ACNH Save Editor — Roadmap

A native Nintendo Switch homebrew save editor for Animal Crossing: New Horizons — no PC required.

This file tracks what's shipped and what's coming next. Suggestions welcome — open an issue or find me on GBATemp/Reddit!

---

## ✅ Shipped — v1.0
- [x] Wallet & Bank Bells editing (encrypted ints, healed Murmur3 hashes)
- [x] Pocket item injector with live English item-name lookup
- [x] Automatic SD backup before every NAND write
- [x] GitHub Releases with ready-to-install .nro + items.txt
- [x] Submitted to the Homebrew App Store & sphaira++

## ✅ Shipped — v1.1
- [x] Full graphical UI (SDL2 renderer replaces the text console)
- [x] PNG icon rendering from the SD card (stb_image)
- [x] Crisp text via SDL2_ttf with bundled rounded font
- [x] Graphical menu with highlight navigation (Bells / Slot / Item / Count)

## ✅ Shipped — v1.2
- [x] Dual-file save engine (personal.dat + main.dat with dual hash healing)
- [x] House Loan editing (zero out Tom Nook's debt)
- [x] Nook Miles editing (max the miles balance)
- [x] Favorites Menu with visual Slot Picker (NMTs, Star Fragments, gold tools & nuggets)
- [x] Pocket Loadouts (save/load 5 named setups to the SD card)
- [x] Backup Restore (press ZL to rollback to the last SD backup)
- [x] Clear Slot (press Minus to empty a pocket)

## ✅ Shipped — v1.3.0 (Current)
- [x] Native Item Search — Press (+) to open the Switch keyboard and search the 13,000+ item database by name
- [x] Optimized Item Database — Parsed into std::vector at boot for instant, lag-free name lookups
- [x] Expanded UI Panel — Increased to 900x480 for better text containment and a premium look
- [x] Centered Footer Text — Reorganized control guide into two clean, perfectly centered lines
- [x] Search Results Truncation — Long item names safely truncated to prevent horizontal overflow
- [x] Search Screen Pagination — Displays 8 items at a time with increased line height (38px)
- [x] Graceful App Exit — Added a dedicated "Quit App" option to the main menu

---

## 🟡 Coming Next — v1.4.x (Content, Safety & Deep Edit Update)

### 💰 Money & Progression
- [ ] Expanded Favorites — quick-select Cherry-blossom petals, Gold Roses, Bell Bags (10k, 30k, 99k), additional NMTs
- [ ] Turnip Price Overwrite — set your own weekly stalk market prices, force Big/Small Spike patterns
- [ ] Nook Miles & Achievements Unlocker — unlock all Nook Miles+ achievements and passport stamps

### 🌟 God-Mode Unlocks
- [ ] DIY Recipe Unlocker — learn every recipe, golden tools included (bitfield manipulation)

### 🏝️ Island & Environment
- [ ] Multi-Villager Support — edit Player 1–8 house blocks (Villager0–7, Base + player_index * 0x28A28)
- [ ] Island "Flag" Edits — change Native Fruit and force specific K.K. Slider songs

### 📦 Inventory & Storage
- [ ] House Storage Editing — parse and edit the ~2,400+ slot storage array with pagination/search
- [ ] Custom Design Slot Management — clear all designs or inject popular design IDs

### 🛡️ Safety & Reliability
- [ ] Game Version Validator — warn if save offsets don't match your game version
- [ ] "Dry Run" Preview Mode — see exactly what will change before writing to NAND
- [ ] Rolling Backup Manager — keep the last 3 backups with timestamps and version tracking

---

## 🔴 Long-Term — v2.0.0 (Architecture & Completion Overhaul)

### 🏆 Completion Cheats
- [ ] Museum Completion Flag — mark all fish, bugs, fossils, and art as donated
- [ ] Villager Friendship Control — max friendship to 10 stars for framed photos

### 🎨 UI & Graphics
- [ ] Multi-Language UI — localized items.txt and UI strings (EN, ES, FR, DE, JA, KO) with boot-time selector
- [ ] Advanced UI Navigation — tree-view or tabbed interface for cleaner feature access
- [ ] Save File "Diff" Viewer — compare backup and current save side-by-side before writing

### 🛠️ Quality of Life
- [ ] Modular Plugin System — community devs can write JSON/Lua scripts for new offsets without recompiling
- [ ] Wi-Fi Cloud Backup — upload backups off-console for true cloud saves

---

Built with devkitPro/libnx. Educational project — always keep backups!