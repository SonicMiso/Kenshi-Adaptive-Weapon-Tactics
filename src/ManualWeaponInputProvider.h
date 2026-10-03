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

    extern bool primaryCommand;
    extern bool secondaryCommand;

    void setProvider(Provider* provider);
}
