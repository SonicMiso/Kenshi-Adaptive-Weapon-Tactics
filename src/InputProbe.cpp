#include <Debug.h>
#include <kenshi/InputHandler.h>
#include <kenshi/Globals.h>

// Temporary probe for v0.1 Manual Weapon Switch.
//
// This file intentionally does not register hotkeys yet.
// The goal is to verify how the live InputHandler instance is accessed
// before adding F7/F8/F9 commands.

namespace ManualWeaponSwitchInputProbe
{
    static bool initialized = false;

    void probe(InputHandler* handler)
    {
        if (initialized || !handler)
            return;

        initialized = true;
        DebugLog("Manual Weapon Switch: InputHandler found");
    }
}
