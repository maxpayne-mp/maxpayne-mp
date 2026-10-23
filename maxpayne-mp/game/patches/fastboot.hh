#pragma once

#include <cstdarg>
#include <cstdio>
#include <windows.h>

#include "lib/utils.h"

namespace PATCHES
{
    namespace FASTBOOT
    {
        namespace ADDRESS
        {
            constexpr uintptr_t addrDeveloper = 0x8A6483;
            constexpr uintptr_t addrSkipStartup = 0x8A6488;
            constexpr uintptr_t addrDeveloperKeys = 0x8A6489;
            constexpr uintptr_t addrNoProfile = 0x8A6482;
            constexpr uintptr_t addrNoDialog = 0x867AFC;

            constexpr uintptr_t addrCALL_WINDOW_ARGS_RVA = 0xA67A; // 0x40A67A - 0x400000;
        }

        //      Functions 
        // --------------------------------------------------------------


        void Install()
        {
            // Skip parser of window args
            uintptr_t base = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
            uintptr_t addr = base + ADDRESS::addrCALL_WINDOW_ARGS_RVA;
            PATCH(addr, 0x90, 5);

            *reinterpret_cast<BYTE*>(ADDRESS::addrDeveloper)     = 1;   // active console
            *reinterpret_cast<BYTE*>(ADDRESS::addrSkipStartup)   = 1;   // skip movie
            *reinterpret_cast<BYTE*>(ADDRESS::addrDeveloperKeys) = 1;   // page up muda de skin, insert de cenario, c = cam mode, F11 - INFORMACOES
            *reinterpret_cast<BYTE*>(ADDRESS::addrNoProfile)     = 1;   // no profile
            *reinterpret_cast<BYTE*>(ADDRESS::addrNoDialog)      = 0;   // no dialog
        }
    }
}