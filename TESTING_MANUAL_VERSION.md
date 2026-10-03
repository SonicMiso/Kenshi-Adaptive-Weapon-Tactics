# Manual Weapon Switch v0.1 Test Checklist

## Goal

Verify that the plugin can switch the currently selected character between already equipped primary and secondary weapons without modifying inventory.

## Test setup

1. Build the RE_Kenshi DLL.
2. Install the three-file mod package:
   - `Adaptive Weapon Tactics.mod`
   - `RE_Kenshi.json`
   - `AdaptiveWeaponTactics.dll`
3. Start Kenshi with RE_Kenshi enabled.
4. Enable **Adaptive Weapon Tactics** in the Kenshi mod list.
5. Load a save with a selected character that has both a primary and secondary weapon equipped.
6. Open the RE_Kenshi/game log.

## Expected runtime flow

```text
InputHandler
    -> ManualWeaponInput Provider
    -> ManualWeaponSwitch
    -> Character::drawWeapon()
```

## Verify

- The plugin loads without a RE_Kenshi plugin initialization error.
- Selecting a character works.
- Pressing **F7** switches the selected character to the primary weapon.
- Pressing **F8** switches the selected character to the secondary weapon.
- Inventory contents do not change.
- Switching to the already active weapon does not cause errors.
- Switching repeatedly between F7 and F8 does not create duplicate items or alter inventory sections.
- The plugin does not attempt adaptive/AI weapon selection.

## Known pending items

- Replace the fixed F7/F8 bindings with Emkejs Mod Core settings/keybind integration.
- Add optional UI controls if needed.
- Produce and validate a production-compatible `v100` build on a matching Visual C++ 2010 x64 environment.
