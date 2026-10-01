#include "DefaultManualWeaponInputProvider.h"

// RE_Kenshi exposes the active input handler through the global `key` pointer.
// This provider intentionally remains separate so Emkejs Mod Core can replace it later.
#include <kenshi/Globals.h>

namespace ManualWeaponInput
{
    Action DefaultProvider::poll()
    {
        if (!key)
            return Action::None;

        if (key->isKeyState("AWT_PrimaryWeapon"))
            return Action::Primary;

        if (key->isKeyState("AWT_SecondaryWeapon"))
            return Action::Secondary;

        return Action::None;
    }
}
