# Runtime integration contract

This document defines what a future RE_Kenshi/KenshiLua integration adapter must provide. The exact game API names are intentionally not guessed.

## Adapter responsibilities

1. Enumerate player-controlled characters and store a per-character enabled flag.
2. Read the current active weapon slot and the equipped primary/secondary weapon stats.
3. Query nearby hostile combatants, with stable identity and distance.
4. Normalize armor, cut/blunt resistance, robot classification, weapon reach/damage/speed/cleave, and indoor/space restrictions into the fields documented in `src/decision.lua`.
5. Detect consciousness and injured arms.
6. Call `decision.decide(snapshot, config)` on a timer.
7. If the returned slot differs from the current slot, invoke the verified game weapon-switch action and confirm the result.
8. Register manual controls and maintain `manual_mode` per character:
   - `auto`: use scoring engine.
   - `primary`: force primary while available.
   - `secondary`: force secondary while available.
9. Expose per-character and party-wide automation toggles.

## Important safety rules

- Do not issue a switch command during an attack animation unless the verified API explicitly supports it.
- Do not switch if the desired slot is empty, inaccessible, or unusable due to limb loss.
- Avoid changing equipment items; this prototype only switches between already-equipped slots.
- Clear or suspend manual state when a character is removed from the player party.
- Log missing or unsupported data and fall back to keeping the current weapon.

## Validation checklist

- [ ] Confirm RE_Kenshi runtime and supported plugin/script entrypoint.
- [ ] Confirm whether KenshiLua is required and which Lua version it embeds.
- [ ] Verify active weapon-slot read and switch calls in a disposable save.
- [ ] Verify indoor detection and weapon indoor penalty representation.
- [ ] Verify armor and damage values are read from live game objects.
- [ ] Verify hostile filtering does not include allies, prisoners, or neutral NPCs.
- [ ] Test one character with two valid weapons.
- [ ] Test missing secondary weapon and injured/absent arms.
- [ ] Test indoor fights and multiple enemies.
- [ ] Test manual lock precedence and cooldown.
- [ ] Test save/load persistence of settings.

Until these checks are complete, this repository should be treated as a prototype decision engine, not a ready-to-install mod.
