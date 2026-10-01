# Manual Weapon Switch v0.1 Test Checklist

## Goal

Verify that the plugin can switch the currently selected character between already equipped primary and secondary weapons without modifying inventory.

## Test setup

1. Build the RE_Kenshi DLL.
2. Start Kenshi with the plugin loaded.
3. Load a save with a selected character that has:
   - primary weapon equipped
   - secondary weapon equipped
4. Open the game log.

## Expected runtime flow

Input source:

```
InputHandler
    -> ManualWeaponInput Provider
    -> ManualWeaponSwitch
    -> Character::drawWeapon()
```

## Verify

- Selecting a character works.
- Primary weapon switch draws the primary weapon.
- Secondary weapon switch draws the secondary weapon.
- Inventory contents do not change.
- Switching to the already active weapon does not cause errors.

## Known pending items

- Default key binding registration.
- Emkejs Mod Core keybind integration.
- UI button integration.
