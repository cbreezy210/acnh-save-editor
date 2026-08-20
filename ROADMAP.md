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

## ✅ Shipped — v1.3.0
- [x] Native Item Search — Press (+) to open the Switch keyboard and search the 13,000+ item database by name
- [x] Optimized Item Database — Parsed into std::vector at boot for instant, lag-free name lookups
- [x] Expanded UI Panel — Increased to 900x480 for better text containment and a premium look
- [x] Centered Footer Text — Reorganized control guide into two clean, perfectly centered lines
- [x] Search Results Truncation — Long item names safely truncated to prevent horizontal overflow
- [x] Search Screen Pagination — Displays 6 items at a time with increased line height (38px)
- [x] Graceful App Exit — Added a dedicated "Quit App" option to the main menu

##  Ready to Ship — v1.3.1 (UI Polish & Bug Fixes)
*Target Release: Late August 2026*
- [x] Search Footer Overlap Fix — Reduced search results per page to prevent text clipping
- [x] Search Pagination Fix — Corrected page jump calculation to prevent skipping items
- [ ] **Release pending** — Scheduled for a few days after v1.3.0 launch

---

##  Coming Next — v1.4.x (Content, Safety & Deep Edit Update)
*Target Release: Q4 2026*

### 💰 Money & Progression
- [ ] Expanded Favorites — quick-select Cherry-blossom petals, Gold Roses, Bell Bags (10k, 30k, 99k), additional NMTs
- [ ] Turnip Price Overwrite — set your own weekly stalk market prices, force Big/Small Spike patterns
- [ ] Nook Miles & Achievements Unlocker — unlock all Nook Miles+ achievements and passport stamps

### 🌟 God-Mode Unlocks
- [ ] DIY Recipe Unlocker — learn every recipe, golden tools included (bitfield manipulation)

### 🏝️ Island & Environment
- [ ] Multi-Villager Support — edit Player 1–8 house blocks (Villager0–7, Base + player_index * 0x28A28)
- [ ] Villager Management — force specific villagers to move in, kick out unwanted villagers, edit catchphrases
- [ ] Player Customization — edit Player Name, Appearance, Passport info, and Catchphrase
- [ ] Island Infrastructure — max out all bridges/inclines, change Native Fruit, force K.K. Slider songs

### 📦 Inventory & Storage
- [ ] House Storage Editing — parse and edit the ~2,400+ slot storage array with pagination/search
- [ ] Custom Design Slot Management — clear all designs or inject popular design IDs

### 🛡️ Safety & Reliability
- [ ] Game Version Validator — warn if save offsets don't match your game version
- [ ] "Dry Run" Preview Mode — see exactly what will change before writing to NAND
- [ ] Rolling Backup Manager — keep the last 3 backups with timestamps and version tracking
- [ ] "Safe Mode" Toggle — restrict edits to offline-safe values to prevent console bans
- [ ] Sanity Checks — automatically prevent impossible values (e.g., negative Bells, invalid Item IDs)

### 🧪 Quality of Life & QA
- [ ] Crash Logging — write a `crash.log` to the SD card on unrecoverable SDL2 errors for easy debugging
- [ ] Beta Testing Channel — establish a Discord/GitHub Pre-release channel for power users to test features early

---

##  Long-Term — v2.0.0 (Architecture & Community Overhaul)
*Target Release: 2027*

### 🏆 Completion Cheats
- [ ] Museum Completion Flag — mark all fish, bugs, fossils, and art as donated
- [ ] Villager Friendship Control — max friendship to 10 stars for framed photos

### 🎨 UI & Graphics
- [ ] Advanced UI Navigation — tree-view or tabbed interface for cleaner feature access
- [ ] Save File "Diff" Viewer — compare backup and current save side-by-side before writing

### 🌍 Community & Localization
- [ ] Multi-Language UI — localized items.txt and UI strings (EN, ES, FR, DE, JA, KO) with boot-time selector
- [ ] Contributing.md — official guide for community devs to submit Pull Requests and translations

### 🛠️ Quality of Life
- [ ] Modular Plugin System — community devs can write JSON/Lua scripts for new offsets without recompiling
- [ ] Wi-Fi Cloud Backup — upload backups off-console for true cloud saves

---

Built with devkitPro/libnx. Educational project — always keep backups!