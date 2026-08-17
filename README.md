# ACNH Save Editor (Switch homebrew)

A native Nintendo Switch homebrew save editor for Animal Crossing: New Horizons, with a full graphical UI.

- Reads/writes the live save directly from/to NAND (no PC needed)
- Edits wallet & bank Bells (encrypted ints, healed Murmur3 hashes)
- Pocket item injector with full item-name lookup (items.txt from NHSE)
- Automatic SD backup before every NAND write
- v1.1: SDL2 graphical interface with icon + crisp text rendering

See [ROADMAP.md](ROADMAP.md) for what's coming next!

## Install

1. Download `acnh_editor_v1.1.zip` from the [Releases](https://github.com/cbreezy210/acnh-save-editor/releases) page.
2. Extract it to the root of your SD card (creates `/switch/acnh_editor/` with the `.nro`, `items.txt`, `font.ttf` and `icon.png`).
3. Fully close ACNH from the Home Menu.
4. Launch via the Homebrew Menu and select `acnh_editor`.

Built with devkitPro/libnx, SDL2, stb_image and SDL2_ttf. Educational project — always keep backups!
