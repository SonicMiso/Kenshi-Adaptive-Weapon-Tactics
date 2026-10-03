#include <Debug.h>
#include <core/Functions.h>
#include <kenshi/Character.h>
#include <kenshi/GameWorld.h>
#include <kenshi/Globals.h>
#include <kenshi/Inventory.h>
#include <kenshi/Gear.h>
#include <kenshi/InputHandler.h>
#include <kenshi/PlayerInterface.h>
#include <ois/OISKeyboard.h>

#include "ManualWeaponInput.h"
#include "DefaultManualWeaponInputProvider.h"

// Manual Weapon Switch runtime
//
// v0.1 goal:
// Switch only between weapons that are already equipped.
// Inventory is never modified.

static void (*mainLoopOriginal)(GameWorld*, float);
static void (*loadConfigOriginal)(InputHandler*);

namespace ManualWeaponSwitch
{
    static Character* getSelectedCharacter()
    {
        if (!ou || !ou->player)
            return nullptr;

        return ou->player->selectedCharacter.getCharacter();
    }

    static bool switchWeapon(Character* character, Weapon* weapon)
    {
        if (!character || !weapon)
            return false;

        Weapon* current = character->getCurrentWeapon();
        if (current == weapon)
            return true;

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

        switch (ManualWeaponInput::poll())
        {
        case ManualWeaponInput::Action::Primary:
            switchToPrimary(character);
            break;

        case ManualWeaponInput::Action::Secondary:
            switchToSecondary(character);
            break;

        default:
            break;
        }
    }
}

static void loadConfigHook(InputHandler* handler)
{
    handler->addCommand(
        "AWT_PrimaryWeapon",
        ManualWeaponInput::primaryCommand,
        OIS::KeyCode::KC_F7,
        OIS::KeyCode::KC_UNASSIGNED,
        InputHandler::NONE_MASK,
        InputHandler::GLOBAL);

    handler->addCommand(
        "AWT_SecondaryWeapon",
        ManualWeaponInput::secondaryCommand,
        OIS::KeyCode::KC_F8,
        OIS::KeyCode::KC_UNASSIGNED,
        InputHandler::NONE_MASK,
        InputHandler::GLOBAL);

    loadConfigOriginal(handler);
}

static void mainLoopHook(GameWorld* world, float time)
{
    mainLoopOriginal(world, time);
    ManualWeaponSwitch::update();
}

__declspec(dllexport) void startPlugin()
{
    static ManualWeaponInput::DefaultProvider defaultProvider;
    ManualWeaponInput::setProvider(&defaultProvider);

    if (KenshiLib::SUCCESS != KenshiLib::AddHook(
        KenshiLib::GetRealAddress(&InputHandler::loadConfig),
        &loadConfigHook,
        &loadConfigOriginal))
    {
        ErrorLog("Manual Weapon Switch: could not install input hook");
    }

    if (KenshiLib::SUCCESS != KenshiLib::AddHook(
        KenshiLib::GetRealAddress(&GameWorld::_NV_mainLoop_GPUSensitiveStuff),
        &mainLoopHook,
        &mainLoopOriginal))
    {
        ErrorLog("Manual Weapon Switch: could not install main loop hook");
    }
}
