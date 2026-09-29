# Runtime integration contract

## Confirmed RE_Kenshi plugin model

The public RE_Kenshi/KenshiLib examples use native C++ DLL plugins with an exported `startPlugin()` entry point. The repository's current `src/decision.lua` is therefore not a directly loadable RE_Kenshi plugin; it is a standalone decision prototype. Do not add a Lua runtime or Lua-to-C++ bridge unless a concrete need and supported runtime are established.

Verified from the published KenshiLib headers/examples:

- `startPlugin()` is the plugin entry point.
- `ou->player->getAllPlayerCharacters()` exposes the player-controlled character list in example code.
- `GameWorld::getCharactersWithinSphere(...)` exists for nearby-object queries.
- Inventory APIs expose sections and items, but the exact safe operation for switching the active weapon has not yet been verified.

The RE_Kenshi project notes that plugins can use precompiled RE_Kenshi/KenshiLib binaries. Building the library itself has legacy Visual Studio 2010 x64 toolchain requirements. The plugin examples target native C++.

## Adapter responsibilities

1. Enumerate player-controlled characters and maintain per-character enabled state.
2. Read the active weapon slot and the two equipped weapon items.
3. Query nearby hostile characters and calculate distance.
4. Normalize weapon damage, reach, attack speed, armor interaction, indoor constraints, and character weapon skill into decision inputs.
5. Determine whether the character is conscious and whether relevant limbs are injured or missing.
6. Run the decision logic at a safe, bounded cadence.
7. Invoke a verified weapon-switch operation only when the desired slot differs from the active slot, then confirm the result.
8. Support manual modes: `auto`, `primary`, and `secondary`.
9. Expose per-character and party-wide automation toggles.

## Important safety rules

- Do not issue a switch command during an attack animation unless the verified API explicitly supports it.
- Do not switch if the desired slot is empty, inaccessible, or unusable due to limb loss.
- Avoid changing inventory items; the intended feature only switches between already-equipped weapons.
- Clear or suspend manual state when a character leaves the player party.
- If required data is unavailable, keep the current weapon and log a concise diagnostic.

## Validation checklist

- [x] Confirm RE_Kenshi native plugin entry point and find player-party enumeration.
- [x] Locate a nearby-character query API.
- [ ] Verify active weapon-slot read and safe switch operation.
- [ ] Identify how equipped weapon slots map to inventory sections/items.
- [ ] Verify live weapon stats and armor values.
- [ ] Verify indoor detection and weapon indoor constraints.
- [ ] Verify hostile filtering excludes allies, prisoners, and neutral NPCs.
- [ ] Test one character with two valid weapons.
- [ ] Test missing secondary weapon and injured/absent arms.
- [ ] Test indoor fights and multiple enemies.
- [ ] Test manual mode precedence, cooldown, and party membership changes.
- [ ] Test save/load behavior.

**Status:** still a prototype, not a ready-to-install mod. The immediate blocker is confirming the game's actual active-weapon switching mechanism before implementing a native plugin.
