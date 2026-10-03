# Adaptive Weapon Tactics (Kenshi)

**Manual Weapon Switch v0.1.0**

A RE_Kenshi plugin for Kenshi that lets the player manually switch the selected character between their equipped primary and secondary weapons.

## Current status

The `manual-version` branch is the first runnable plugin implementation. It is intentionally limited to manual switching; the previous adaptive/AI weapon-selection design is not part of this version.

### Controls

- **F7** — switch the selected character to the primary weapon.
- **F8** — switch the selected character to the secondary weapon.

The input layer is separated from the weapon-switching logic so a future Emkejs Mod Core keybind/settings UI can replace the default F7/F8 provider without rewriting the weapon logic.

## Mod package

The Kenshi mod package consists of:

```text
Adaptive Weapon Tactics/
├── Adaptive Weapon Tactics.mod
├── RE_Kenshi.json
└── AdaptiveWeaponTactics.dll
```

The `.mod` file is a zero-record carrier file used so Kenshi recognizes the mod directory. RE_Kenshi reads `RE_Kenshi.json` and loads the DLL listed in its `Plugins` array. This is the standard RE_Kenshi plugin layout. citeturn0search0turn0search8

### Installation

1. Install a compatible **RE_Kenshi** release.
2. Create `Kenshi/mods/Adaptive Weapon Tactics/`.
3. Copy these three files into that directory:
   - `Adaptive Weapon Tactics.mod`
   - `RE_Kenshi.json`
   - `AdaptiveWeaponTactics.dll`
4. Enable **Adaptive Weapon Tactics** in Kenshi's mod list.
5. Load a game and test F7/F8 with the selected character carrying both primary and secondary weapons.

RE_Kenshi's plugin loader processes `RE_Kenshi.json` for active mods and loads DLLs from the mod directory. citeturn0search4

## Build

The Visual Studio project targets x64 and links against KenshiLib. The official RE_Kenshi/KenshiLib documentation requires the Visual C++ 2010 x64 compiler ABI (`v100`) for production-compatible builds; the GitHub Actions workflow currently uses `v143` only as a hosted CI compilation check because the standard runner does not provide v100. citeturn0search4

Set:

- `KENSHILIB_DIR` — KenshiLib SDK root.
- `BOOST_INCLUDE_PATH` — Boost 1.60 root.
- `BOOST_LIBRARY_PATH` — Boost 1.60 `stage/lib`.

Then build `Release|x64`.

## Repository layout

- `src/ManualWeaponSwitch.cpp` — selected-character weapon switching and RE_Kenshi hooks.
- `src/ManualWeaponInput.*` — input abstraction.
- `src/DefaultManualWeaponInputProvider.*` — current F7/F8 InputHandler provider.
- `AdaptiveWeaponTactics.vcxproj` — x64 plugin project.
- `RE_Kenshi.json` — plugin loader manifest.
- `Adaptive Weapon Tactics.mod` — Kenshi mod carrier file.

## License

MIT. See `LICENSE`.
