# Runtime integration contract

## Confirmed RE_Kenshi plugin model

The public RE_Kenshi/KenshiLib examples use native C++ DLL plugins with an exported `startPlugin()` entry point. The repository's current `src/decision.lua` is therefore not a directly loadable RE_Kenshi plugin; it is a standalone decision prototype. Do not add a Lua runtime or Lua-to-C++ bridge unless a concrete need and supported runtime are established.

Verified from the published KenshiLib headers/examples (KenshiLib
`1ce4a163739bbdf17254259039d7aa74fcc11cc0`, KenshiLib_Examples
`548b3eaf779c1b2feb25416f1db757320d04ec6c`, and RE_Kenshi
`c8dfbc9621b9263863722852f819c4c0f440475a`):

- `startPlugin()` is the plugin entry point.
- `ou->player->getAllPlayerCharacters()` exposes the player-controlled character list in example code.
- `GameWorld::getCharactersWithinSphere(...)` exists for nearby-object queries.
- `Character::getCurrentWeapon()` reads the weapon currently in use. The example
  falls back to `getThePreferredWeapon()` only when the current weapon is null.
- `Inventory::getPrimaryWeapon()` and `getSecondaryWeapon()` directly expose the
  two equipped weapon candidates. This is the smallest reliable way to identify
  the main and secondary weapons; do not infer them from a section name.
- `Inventory::getEquippedWeapons()` returns all equipped weapons. For a generic
  fallback, `getAllSectionsOfType(..., AttachSlot::ATTACH_WEAPON)` lists weapon
  sections, and `InventorySection::getItems()` exposes their contents.
- `Item::isWeapon()` identifies a weapon. `InventoryItemBase` also stores the
  runtime `inventorySection`, `slotType`, and `isEquipped` fields for diagnostics.
- Human characters implement `drawWeapon(Item*, std::string lastSection)` and
  return a success flag. The public headers do not document the meaning of
  `lastSection`, so it must be supplied from the live current item and validated
  in-game before this project calls it.
- `InputHandler::addCommand(...)` and `InputHandler::isKeyState(...)` support
  registered controls, but hotkeys are deferred until a switch is proven safe.

The RE_Kenshi project notes that plugins can use precompiled RE_Kenshi/KenshiLib binaries. Building the library itself has legacy Visual Studio 2010 x64 toolchain requirements. The plugin examples target native C++.

## Adapter responsibilities

1. Enumerate player-controlled characters and maintain per-character enabled state.
2. Read the active weapon slot and the two equipped weapon items.
3. Query nearby hostile characters and calculate distance.
4. Normalize weapon damage, reach, attack speed, armor interaction, indoor constraints, and character weapon skill into decision inputs.
5. Determine whether the character is conscious and whether relevant limbs are injured or missing.
6. Run the decision logic at a safe, bounded cadence.
7. Invoke `drawWeapon(desired, current->inventorySection)` only after a live
   verification proves that the second parameter is the expected prior section;
   call it only when desired differs from `getCurrentWeapon()`, check its `bool`
   return value, then confirm with `getCurrentWeapon()`.
8. Support manual modes: `auto`, `primary`, and `secondary`.
9. Expose per-character and party-wide automation toggles.

## Important safety rules

- Do not issue a switch command during an attack animation unless the verified API explicitly supports it.
- Do not switch if the desired slot is empty, inaccessible, or unusable due to limb loss.
- Avoid changing inventory items; the intended feature only switches between already-equipped weapons.
- Treat `getPrimaryWeapon()` and `getSecondaryWeapon()` as the slot mapping. Do
  not hard-code inventory section names: `AttachSlot` only distinguishes
  `ATTACH_WEAPON` and `ATTACH_BACK`, not a documented primary/secondary pair.
- Do not call `equipItem`, `unequipItem`, item removal, or item transfer to
  change weapons; those are inventory-mutating operations and are unnecessary
  while `drawWeapon` exists.
- Clear or suspend manual state when a character leaves the player party.
- If required data is unavailable, keep the current weapon and log a concise diagnostic.

## Validation checklist

- [x] Confirm RE_Kenshi native plugin entry point and find player-party enumeration.
- [x] Locate a nearby-character query API.
- [x] Verify active weapon read (`Character::getCurrentWeapon()`).
- [x] Identify equipped primary/secondary weapons (`Inventory::getPrimaryWeapon()` / `getSecondaryWeapon()`).
- [ ] Build and run `src/WeaponProbe.cpp` against the matching KenshiLib SDK; record the one-time debug output for a character with two melee weapons.
- [ ] Verify `CharacterHuman::drawWeapon(desired, current->inventorySection)` in a live game and confirm the postcondition.
- [ ] Verify live weapon stats and armor values.
- [ ] Verify indoor detection and weapon indoor constraints.
- [ ] Verify hostile filtering excludes allies, prisoners, and neutral NPCs.
- [ ] Test one character with two valid weapons.
- [ ] Test missing secondary weapon and injured/absent arms.
- [ ] Test indoor fights and multiple enemies.
- [ ] Test manual mode precedence, cooldown, and party membership changes.
- [ ] Test save/load behavior.

**Status:** still a prototype, not a ready-to-install mod. The headers now show
the intended minimal read path and a plausible non-mutating switch entry point,
but `drawWeapon`'s `lastSection` contract has no source-level documentation.
The immediate blocker is one live-game validation with two equipped melee weapons
before implementing a native plugin that invokes it.
