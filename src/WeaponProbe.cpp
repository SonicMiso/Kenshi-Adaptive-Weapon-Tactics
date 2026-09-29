#include <Debug.h>
#include <core/Functions.h>
#include <kenshi/Character.h>
#include <kenshi/GameWorld.h>
#include <kenshi/Gear.h>
#include <kenshi/Globals.h>
#include <kenshi/Inventory.h>
#include <kenshi/PlayerInterface.h>

static void (*mainLoopOriginal)(GameWorld*, float);
static bool reported;

static void reportWeapon(Character* character)
{
    Inventory* inventory = character->getInventory();
    Weapon* current = character->getCurrentWeapon();
    Weapon* primary = inventory->getPrimaryWeapon();
    Weapon* secondary = inventory->getSecondaryWeapon();

    DebugLog("Adaptive Weapon Tactics: " + character->getName()
        + " current=" + (current == primary ? "primary" : current == secondary ? "secondary" : current ? "other" : "none")
        + " primary=" + (primary ? primary->inventorySection : "none")
        + " secondary=" + (secondary ? secondary->inventorySection : "none"));
}

static void mainLoopHook(GameWorld* world, float time)
{
    mainLoopOriginal(world, time);
    if (reported || !ou || !ou->player)
        return;

    const lektor<Character*>& characters = ou->player->getAllPlayerCharacters();
    if (!characters.size())
        return;

    for (int i = 0; i < characters.size(); ++i)
        if (characters[i] && characters[i]->getInventory())
            reportWeapon(characters[i]);
    reported = true;
}

__declspec(dllexport) void startPlugin()
{
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(
            KenshiLib::GetRealAddress(&GameWorld::_NV_mainLoop_GPUSensitiveStuff),
            &mainLoopHook,
            &mainLoopOriginal))
        ErrorLog("Adaptive Weapon Tactics: could not install weapon probe hook");
}
