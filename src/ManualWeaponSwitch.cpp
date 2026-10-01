#include <Debug.h>
#include <core/Functions.h>
#include <kenshi/Character.h>
#include <kenshi/GameWorld.h>
#include <kenshi/Globals.h>
#include <kenshi/PlayerInterface.h>

// Manual Weapon Switch runtime
//
// v0.1 goal:
// Switch only between weapons that are already equipped.
// Inventory is never modified.

static void (*mainLoopOriginal)(GameWorld*, float);

namespace ManualWeaponSwitch
{
    enum class Mode
    {
        Auto,
        Primary,
        Secondary
    };

    static Mode currentMode = Mode::Auto;

    static Character* getSelectedCharacter()
    {
        if (!ou || !ou->player)
            return nullptr;

        return ou->player->getSelectedCharacter();
    }

    static bool switchWeapon(Character* character, Item* weapon)
    {
        if (!character || !weapon)
            return false;

        Item* current = character->getCurrentWeapon();
        if (current == weapon)
            return true;

        // drawWeapon is the native weapon draw/switch path.
        // The previous inventory section is required by Kenshi.
        if (!current)
        {
            DebugLog("Manual Weapon Switch: no current weapon section");
            return false;
        }

        bool result = character->drawWeapon(weapon, current->inventorySection);

        if (result && character->getCurrentWeapon() == weapon)
        {
            DebugLog("Manual Weapon Switch: weapon switched");
            return true;
        }

        DebugLog("Manual Weapon Switch: switch failed");
        return false;
    }

    static bool switchToPrimary(Character* character)
    {
        if (!character || !character->getInventory())
            return false;

        return switchWeapon(character, character->getInventory()->getPrimaryWeapon());
    }

    static bool switchToSecondary(Character* character)
    {
        if (!character || !character->getInventory())
            return false;

        return switchWeapon(character, character->getInventory()->getSecondaryWeapon());
    }

    static void update()
    {
        Character* character = getSelectedCharacter();
        if (!character || !character->getInventory())
            return;

        // Input handling will call switchToPrimary/switchToSecondary.
        // Keep update empty until commands are connected.
    }
}

static void mainLoopHook(GameWorld* world, float time)
{
    mainLoopOriginal(world, time);
    ManualWeaponSwitch::update();
}

__declspec(dllexport) void startPlugin()
{
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(
        KenshiLib::GetRealAddress(&GameWorld::_NV_mainLoop_GPUSensitiveStuff),
        &mainLoopHook,
        &mainLoopOriginal))
    {
        ErrorLog("Manual Weapon Switch: could not install hook");
    }
}
