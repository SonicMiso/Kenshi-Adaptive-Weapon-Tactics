#pragma once

#include "ManualWeaponInputProvider.h"

namespace ManualWeaponInput
{
    class DefaultProvider final : public Provider
    {
    public:
        Action poll() override;
    };
}
