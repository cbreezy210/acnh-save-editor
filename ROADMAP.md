# 🗺️ ACNH Save Editor — Roadmap

A native Nintendo Switch homebrew save editor for *Animal Crossing: New Horizons* — no PC required.

This file tracks what's shipped and what's coming next. Suggestions welcome — open an issue or find me on GBATemp/Reddit!

## ✅ Shipped — v1.0

- [x] Wallet & Bank Bells editing (encrypted ints, healed Murmur3 hashes)
- [x] Pocket item injector with live English item-name lookup
- [x] Automatic SD backup before every NAND write
- [x] GitHub Releases with ready-to-install `.nro` + `items.txt`
- [x] Submitted to the Homebrew App Store & sphaira++

## ✅ Shipped — v1.1

- [x] Full graphical UI (SDL2 renderer replaces the text console)
- [x] PNG icon rendering from the SD card (stb_image)
- [x] Crisp text via SDL2_ttf with bundled rounded font
- [x] Graphical menu with highlight navigation (Bells / Slot / Item / Count)

## ✅ Shipped — v1.2

- [x] Dual-file save engine (`personal.dat` + `main.dat` with dual hash healing)
- [x] House Loan editing (zero out Tom Nook's debt)
- [x] Nook Miles editing (max the miles balance)
- [x] Favorites Menu with visual Slot Picker (NMTs, Star Fragments, gold tools & nuggets)
- [x] Pocket Loadouts (save/load 5 named setups to the SD card)
- [x] Backup Restore (press ZL to rollback to the last SD backup)
- [x] Clear Slot (press Minus to empty a pocket)

## 🎨 UI & Graphics

- [ ] **Per-item Pictures** — show each item's sprite next to its name (graphics engine shipped in v1.1)
- [ ] **Dark Mode** — sleek theme for late-night editing sessions

## 💰 Money & Progression

- [ ] **Turnip Price Overwrite** — set your own weekly stalk market prices

## 🌟 God-Mode Unlocks

- [ ] **DIY Recipe Unlocker** — learn every recipe, golden tools included
- [ ] **Nook Shopping Catalog Completion** — order any item from the Nook Stop
- [ ] **All Reactions Unlocked**

## 🏝️ Island & Environment

- [ ] **Instant 5-Star Island Rating** — unlock terraforming & the golden watering can
- [ ] **Meteor Shower / Weather Forcer** — star fragment farming on demand

## 💖 Villager Tools

- [ ] **Multi-Villager Support** — edit any island resident (Villager0–7)
- [ ] **Max Friendship Levels** — photos, catchphrases & clothing control
- [ ] **Move-Out Trigger** — choose who gets the "thinking about moving" bubble

## 🛠️ Quality of Life

- [ ] **Wi-Fi Cloud Backup** — upload backups off-console for true cloud saves

---

*Built with devkitPro/libnx. Educational project — always keep backups!*
