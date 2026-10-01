#include <Debug.h>
#include <core/Functions.h>
#include <kenshi/Character.h>
#include <kenshi/GameWorld.h>
#include <kenshi/Globals.h>
#include <kenshi/PlayerInterface.h>

// Manual Weapon Switch runtime
//
// This branch intentionally does not use the previous adaptive decision engine.
// The first goal is a small, reliable RE_Kenshi plugin that can:
// - read player selected character
// - receive manual weapon switch commands
// - switch between equipped primary and secondary weapons
//
// Input handling and weapon mutation will be added after confirming the
// corresponding KenshiLib bindings.

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

    static void update()
    {
        Character* character = getSelectedCharacter();
        if (!character || !character->getInventory())
            return;

        // TODO:
        // Implement actual weapon switching after validating the RE_Kenshi API.
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
