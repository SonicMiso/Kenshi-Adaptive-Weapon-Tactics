#pragma once

#include "ManualWeaponInput.h"

namespace ManualWeaponInput
{
    class Provider
    {
    public:
        virtual ~Provider() = default;
        virtual Action poll() = 0;
    };

    void setProvider(Provider* provider);
}
