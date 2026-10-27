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

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

#include "main.hh"

#include "lib/CrashInformer.h"

#include "game/core.hh" 
#include "game/console.hh"
#include "game/script.hh"

#include "game/patches/fastboot.hh"
#include "game/patches/keys.hh"

static DWORD WINAPI HotkeyThread(LPVOID)
{
    bool wasDown = false;
    while (true)
    {
        const bool down = (GetAsyncKeyState(VK_F1) & 0x8000) != 0;

        if (down && !wasDown)
        {
            if (CORE::g_pGameMode == nullptr)
                CONSOLE::Print("[CORE] g_pGameMode not captured yet. Run 'Dump' first.\n");
        }
        else if ( GetAsyncKeyState(VK_F3) & 0x8000 )
        {
            //SCRIPT::Run("MaxPayne_HUDMode->MPHM_PrintDirect( \"MaxPayne_SaveGameInterface\" );");
            SCRIPT::Run("Dump");
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
    // CONSOLE::Print("maxpayne-mp started!\n");

    //      PATCHES 
    // --------------------------------------------------------------
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