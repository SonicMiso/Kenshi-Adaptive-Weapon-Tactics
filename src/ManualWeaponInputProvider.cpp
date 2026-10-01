#include "ManualWeaponInputProvider.h"

namespace ManualWeaponInput
{
    static Provider* currentProvider = nullptr;

    void setProvider(Provider* provider)
    {
        currentProvider = provider;
    }

    Action poll()
    {
        if (!currentProvider)
            return Action::None;

        return currentProvider->poll();
    }
}
