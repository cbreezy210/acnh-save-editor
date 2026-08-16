# ACNH Save Editor (Switch homebrew)

A native Nintendo Switch homebrew save editor for Animal Crossing: New Horizons.

- Reads/writes the live save directly from/to NAND (no PC needed)
- Edits wallet & bank Bells (encrypted ints, healed Murmur3 hashes)
- Pocket item injector with full item-name lookup (items.txt from NHSE)
- Automatic SD backup before every NAND write

See [ROADMAP.md](ROADMAP.md) for what's coming next!

## Install

1. Download `acnh_editor.nro` and `items.txt` from the [Releases](https://github.com/cbreezy210/acnh-save-editor/releases) page.
2. Put both files in `/switch/acnh_editor/` on your SD card.
3. Fully close ACNH from the Home Menu.
4. Launch via the Homebrew Menu and select `acnh_editor`.

Built with devkitPro/libnx. Educational project — always keep backups!
