# 🍃 ACNH Save Editor (Native Switch)

A 100% native Nintendo Switch homebrew application for editing *Animal Crossing: New Horizons* save files directly on your console. No PC, no pulling the SD card, no complicated dumping tools required.

![Version](https://img.shields.io/badge/version-1.2.0-green)
![License](https://img.shields.io/badge/license-MIT-blue)

## ✨ Features (v1.2.0)

*   **Dual-File Engine:** Edits both `personal.dat` (Wallet, Bank, Pockets, Nook Miles) and `main.dat` (House Loan) simultaneously.
*   **Graphical UI:** Built with SDL2, featuring a clean, highlight-bar navigation system.
*   **Favorites Menu:** Press **X** to quickly inject high-value items (NMTs, Star Fragments, Gold Tools, Nuggets).
*   **Visual Slot Picker:** Choose exactly which pocket slot to inject items into without guessing.
*   **Pocket Loadouts:** Press **Y** to save and load up to 5 custom pocket setups (Mining, Fishing, Terraforming) to your SD card.
*   **Safety First:** Automatic SD card backups before every single write. Press **ZL** on the main menu to instantly rollback to your last backup.
*   **Automatic Hash Healing:** Recalculates Murmur3 hashes and EncryptedInt32 checksums so the game never detects corruption.

## 📥 Installation

1. Ensure your Switch is running Custom Firmware (Atmosphere).
2. Download the latest `acnh_editor_vX.X.X.zip` from the [Releases page](../../releases).
3. Extract the `switch` folder to the root of your SD card.
4. Launch the "ACNH Save Editor" from your Homebrew Menu (do **not** run ACNH at the same time!).

## 🎮 Controls

| Button | Action |
| :--- | :--- |
| **Up / Down** | Navigate menus / Select items |
| **Left / Right** | Step values (+/- 1) or navigate columns |
| **L / R** | Big step values (+/- 10 or 100,000) |
| **A** | Backup to SD & Save changes to NAND |
| **X** | Open Favorites Menu |
| **Y** | Open Loadout Manager |
| **Minus (-)** | Clear the currently selected pocket slot |
| **ZL** | Restore from last SD backup (Emergency Rollback) |
| **Plus (+)** | Exit app |

## 🛠️ Building from Source

Requires [devkitPro](https://devkitpro.org/) with `switch-dev` installed.
```bash
pacman -S switch-sdl2 switch-sdl2_ttf switch-sdl2_image
git clone https://github.com/cbreezy210/acnh-save-editor.git
cd acnh-save-editor
make
⚠️ Disclaimer
This is an educational project. Always keep backups of your save files. Modifying save data always carries a risk. I am not responsible for corrupted islands or banned consoles. Use at your own risk!
