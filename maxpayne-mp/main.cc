/*

            .        :    :::.        .,::      .:   ::::::::::. :::.    .-:.     ::-.:::.    :::..,::::::
            ;;,.    ;;;   ;;`;;       `;;;,  .,;;     `;;;```.;;;;;`;;    ';;.   ;;;;'`;;;;,  `;;;;;;;''''
            [[[[, ,[[[[, ,[[ '[[,       '[[,,[['       `]]nnn]]',[[ '[[,    '[[,[[['    [[[[[. '[[ [[cccc
            $$$$$$$$"$$$c$$$cc$$$c       Y$$$P          $$$""  c$$$cc$$$c     c$$"      $$$ "Y$c$$ $$""""
            888 Y88" 888o888   888     oP"``"Yo,        888o    888   888   ,8P"`       888    Y88 888oo,__
            MMM  M'  "MMMYMM   \"\"`,m"       "Mm,      YMMMb   YMM   \"\"`mM"          MMM     YM """"YUMMM

    ______________________________________________________________________________________________________________

    - Created by: mateusware
    - Module: server
    - Description:
        This is the server of the maxpayne-mp project.

*/

#define VALIDATE_VERSION_TESTS // se definida, valida algumas coisas do jogo, que podem impedir o mod de funcionar corretamente, como versão do jogo, checksum e build.
#define CONSOLE_DEBUG

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

#include "main.hh"

#include "lib/CrashInformer.h"

#include "game/core.hh" 
#include "game/console.hh"
#include "game/script.hh"

#include "game/patches/fastboot.hh"
#include "game/patches/keys.hh"
#include "game/patches/menu.hh"

static DWORD WINAPI HotkeyThread(LPVOID)
{
    bool wasDown = false;
    while (true)
    {
        const bool down = (GetAsyncKeyState(VK_F1) & 0x8000) != 0;

        if (down && !wasDown)
        {
            CONSOLE::Print("[CORE] g_pGameMode not captured yet. Run 'Dump' first.\n");
        }
        else if ( GetAsyncKeyState(VK_F4) & 0x8000 )
        {
        }
        else if ( GetAsyncKeyState(VK_F5) & 0x8000 )
        {
            SCRIPT::Execute("player->C_Jump(5);");
            // CONSOLE::Print("[gay] you is 'gay'.\n");
        }
        else if ( GetAsyncKeyState(VK_F3) & 0x8000 )
        {
            /*
            SCRIPT::Run("player->c_pickupweapon ( leadpipe );");
            SCRIPT::Run("player->c_setinfiniteammo( leadpipe, true);");
            SCRIPT::Run("player->c_pickupweapon ( baseballbat );");
            SCRIPT::Run("player->c_setinfiniteammo( baseballbat, true);");
            SCRIPT::Run("player->c_pickupweapon ( beretta );");
            SCRIPT::Run("player->c_pickupweapon ( berettadual );");
            SCRIPT::Run("player->c_pickupammo( berettadual, 1000 );");
            SCRIPT::Run("player->c_pickupweapon ( deserteagle );");
            SCRIPT::Run("player->c_pickupammo( deserteagle, 1000 );");
            SCRIPT::Run("player->c_pickupweapon ( sawedshotgun );");
            SCRIPT::Run("player->c_pickupammo( sawedshotgun, 1000 );");
            SCRIPT::Run("player->c_pickupweapon ( pumpshotgun );");
            SCRIPT::Run("player->c_pickupammo( pumpshotgun, 1000 );");
            SCRIPT::Run("player->c_pickupweapon ( ingram );");
            SCRIPT::Run("player->c_pickupweapon ( ingramdual );");
            SCRIPT::Run("player->c_pickupammo( ingramdual, 1000 );");
            SCRIPT::Run("player->c_pickupweapon ( sniper );");
            SCRIPT::Run("player->c_pickupammo(sniper, 1000 );");
            SCRIPT::Run("player->c_pickupweapon ( mp5 );");
            SCRIPT::Run("player->c_pickupammo( mp5, 1000 );");
            SCRIPT::Run("player->c_pickupweapon ( jackhammer );");
            SCRIPT::Run("player->c_pickupammo( jackhammer, 1000 );");
            SCRIPT::Run("player->c_pickupweapon ( m79 );");
            SCRIPT::Run("player->c_pickupammo( m79, 20 );");
            SCRIPT::Run("player->c_pickupweapon ( grenade );");
            SCRIPT::Run("player->c_pickupammo( grenade, 20 );");
            SCRIPT::Run("player->c_pickupweapon ( molotov );");
            SCRIPT::Run("player->c_pickupammo( molotov, 20 );");
            SCRIPT::Run("player->c_pickupweapon ( painkiller );");
            SCRIPT::Run("player->c_pickupammo( painkiller, 20 );");
            */
        }

        wasDown = down;
        Sleep(50);
    }
    return 0;
}

void Install()
{
    HANDLE h = CreateThread(nullptr, 0, HotkeyThread, nullptr, 0, nullptr);
    if (h) CloseHandle(h);
}

DWORD WINAPI MainThread(LPVOID) {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    CrashInformer::Initialize();

    CORE::Install();
    CONSOLE::Install();
    // CONSOLE::Print("maxpayne-mp started!\n");

    //      PATCHES 
    // --------------------------------------------------------------
    //PATCHES::MENU::Install();
    PATCHES::FASTBOOT::Install();
    PATCHES::KEYS::Install();

    //
    Install();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        DisableThreadLibraryCalls(hModule);

        HANDLE hThread = CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
        if (hThread)
            CloseHandle(hThread);
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}