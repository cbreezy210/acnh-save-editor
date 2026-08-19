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

---

## 🟡 Coming Next — v1.3.x (Content, Safety & UX Update)

### 💰 Money & Progression
- [ ] Expanded Favorites — quick-select Cherry-blossom petals, Gold Roses, Bell Bags, NMTs
- [ ] Turnip Price Overwrite — set your own weekly stalk market prices
- [ ] Nook Miles & Achievements Unlocker — unlock all Nook Miles+ achievements and passport stamps

### 🌟 God-Mode Unlocks
- [ ] DIY Recipe Unlocker — learn every recipe, golden tools included

### 🏝️ Island & Environment
- [ ] Multi-Villager Support — edit Player 1–8 house blocks (Villager0–7)

### 🛡️ Safety & Reliability
- [ ] Game Version Validator — warn if save offsets don't match your game version
- [ ] "Dry Run" Preview Mode — see exactly what will change before writing to NAND
- [ ] Rolling Backup Manager — keep the last 3 backups with timestamps and version tracking

---

## 🟠 Mid-Term — v1.4.x (Deep Edit & Storage Update)

### 📦 Inventory & Storage
- [ ] House Storage Editing — parse and edit the ~2,400+ slot storage array with pagination/search
- [ ] Custom Design Slot Management — clear all designs or inject popular design IDs

### 🏝️ Island & Environment
- [ ] Island "Flag" Edits — change Native Fruit (Peaches to Pears) and force K.K. Slider songs
- [ ] Villager & Player Customization — edit Player Name, Appearance, Catchphrase; force villagers to move in
- [ ] Island Infrastructure — max out shop upgrades; unlock terraforming without the app

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
