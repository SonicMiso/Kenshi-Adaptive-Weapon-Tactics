# Adaptive Weapon Tactics (Kenshi)

Adaptive Weapon Tactics is a prototype design and decision-engine scaffold for Kenshi. Its goal is to choose between a character's already-equipped primary and secondary weapons using nearby-enemy count, armor, indoor/outdoor context, distance, and the character's ability to use each weapon. It also defines manual primary/secondary locks and a return-to-auto mode.

> **Status: prototype / not yet a game-ready RE_Kenshi mod.** This repository currently contains the decision engine and integration contract, not a verified in-game plugin. The Kenshi/RE_Kenshi runtime API for reading combatants and switching the active weapon must be implemented and tested before this can be installed as a working mod. No claim is made that the included Lua module can run directly inside Kenshi.

## Initial scope

- Party-wide feature with per-character opt-in.
- Choose only between equipped primary and secondary weapons.
- Balanced scoring rather than rigid weapon-type rules.
- Manual modes: force primary, force secondary, or resume automatic selection.
- Hysteresis and cooldown to prevent rapid toggling.
- Safe fallback when weapon or combat data is unavailable.

## Repository layout

- `src/decision.lua` — runtime-independent weapon scoring and switching decision logic.
- `config/defaults.json` — tunable thresholds and weights.
- `docs/INTEGRATION.md` — game-runtime adapter contract and validation checklist.

## Current limitation

The decision engine expects normalized data from an adapter. It does not itself inspect Kenshi entities, infer armor values from game objects, register hotkeys, or change a character's weapon. Those tasks require verified RE_Kenshi/KenshiLua API bindings and must be implemented in the integration layer.

## First runtime check

`src/WeaponProbe.cpp` is a deliberately read-only RE_Kenshi plugin source for
the first game-side check. Build it against the matching KenshiLib SDK, load it
through RE_Kenshi, then load a save with a player character equipped with two
melee weapons. The debug log should identify the current weapon as `primary` or
`secondary` and print both runtime section names. It does not call
`drawWeapon`, register hotkeys, or mutate inventory.

## Planned controls

The intended default controls are:
- F6: toggle automation for the selected character (party-wide control is planned as a separate setting).
- F7: force primary.
- F8: force secondary.
- F9: resume automatic selection.

These bindings are proposals, not active hotkeys in this prototype.

## License

MIT. See `LICENSE`.
