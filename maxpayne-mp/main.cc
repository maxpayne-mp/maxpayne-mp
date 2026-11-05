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

    -----------------------------------------------------------------------------------------

*/

#define VALIDATE_VERSION_TESTS
// #define CONSOLE_DEBUG
#define E2MFC_DEBUG
// #define PLAYER_DEBUG
#define SCRIPT_EMBEDDED_DEBUG

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

#include "main.hh"

#include "lib/CrashInformer.h"

#include "game/core.hh" 
#include "game/console.hh"
#include "game/script.hh"
#include "game/player.hh"
#include "game/enemy.hh"
#include "game/text.hh"

#include "game/E2MFC.hh"

#include "game/patches/fastboot.hh"
#include "game/patches/keys.hh"
#include "game/patches/menu.hh"
static void* g_exampleText = nullptr;
static void* g_exampleFont = nullptr;


static DWORD WINAPI HotkeyThread(LPVOID)
{
    using namespace E2MFC;
    while (true)
    {
        if ((GetAsyncKeyState(VK_F3) & 0x8001) == 0x0001)
        {
        }

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
    E2MFC::Install();
    CONSOLE::Install();
    PLAYER::Install();
    ENEMY::Install();
    TEXT::Install();

    //      PATCHES 
    // --------------------------------------------------------------
    //PATCHES::MENU::Install();
    PATCHES::FASTBOOT::Install();
    PATCHES::KEYS::Install();

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