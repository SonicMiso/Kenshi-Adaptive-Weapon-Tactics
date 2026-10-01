#pragma once

namespace ManualWeaponInput
{
    enum class Action
    {
        None,
        Primary,
        Secondary
    };

    // v0.1 default provider.
    // This layer is intentionally isolated so future Emkejs Mod Core
    // keybind integration can replace the source of actions.
    Action poll();
}
