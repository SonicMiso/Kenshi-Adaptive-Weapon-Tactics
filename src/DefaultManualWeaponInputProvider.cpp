#include "DefaultManualWeaponInputProvider.h"

#include <kenshi/Globals.h>
#include <kenshi/InputHandler.h>

namespace ManualWeaponInput
{
    bool primaryCommand = false;
    bool secondaryCommand = false;

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
