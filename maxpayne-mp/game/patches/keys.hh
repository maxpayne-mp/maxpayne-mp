#pragma once

#include <cstdarg>
#include <cstdio>
#include <windows.h>

#include "lib/utils.h"

namespace PATCHES
{
    namespace KEYS
    {
        namespace ADDRESS
        {
            //      change keys
            // -------------------------------------------------------------------------------------
            constexpr uintptr_t addrKeyConsole    = 0x421827;
            constexpr uintptr_t addrKeyScreenshot = 0x42188D + 0xCA;

            //      disable keys
            // -------------------------------------------------------------------------------------
            constexpr uintptr_t addrPageUp        = 0x44C2BB;
            constexpr uintptr_t addrPageDown      = 0x44C35A;
            constexpr uintptr_t addrEnd           = 0x44CA84;
            constexpr uintptr_t addrHome          = 0x44CA36;
            constexpr uintptr_t addrInsert        = 0x44CAEB;
            constexpr uintptr_t addrDelete        = 0x44CB5D;
            constexpr uintptr_t addrF1            = 0x44C610;
            constexpr uintptr_t addrF5            = 0x44C8EA;
            constexpr uintptr_t addrF9            = 0x44C7C7;
        }

        //      Functions
        // --------------------------------------------------------------

        void Install()
        {
            using namespace ADDRESS;

            //      change keys
            // -------------------------------------------------------------------------------------

            // Patch to the "Press any key" screen
            BYTE patch1[] = { 0x83, 0xF8, 0x71 }; // cmp eax, 0x71
            PATCH_DATA(addrKeyConsole, patch1, sizeof(patch1));

            // Screenshot F8
            BYTE patch2[] = { 0x83, 0xF8, 0x77 }; // cmp eax, 0x77
            PATCH_DATA(addrKeyScreenshot, patch2, sizeof(patch2));

            //      disable keys
            // -------------------------------------------------------------------------------------

            BYTE pPageUp[]   = { 0xE9, 0xAB, 0xFF, 0xFF, 0xFF };
            BYTE pPageDown[] = { 0xE9, 0x0C, 0xFF, 0xFF, 0xFF };
            BYTE pEnd[]      = { 0xE9, 0xE2, 0xF7, 0xFF, 0xFF };
            BYTE pHome[]     = { 0xE9, 0x30, 0xF8, 0xFF, 0xFF };
            BYTE pInsert[]   = { 0xE9, 0x7B, 0xF7, 0xFF, 0xFF };
            BYTE pDelete[]   = { 0xE9, 0x09, 0xF7, 0xFF, 0xFF };
            BYTE pF1[]       = { 0xE9, 0x56, 0xFC, 0xFF, 0xFF };
            BYTE pF5[]       = { 0xE9, 0x7C, 0xF9, 0xFF, 0xFF };
            BYTE pF9[]       = { 0xE9, 0x9F, 0xFA, 0xFF, 0xFF };

            // Skins change
            PATCH_DATA(addrPageUp,   pPageUp,   sizeof(pPageUp));   // Page Up
            PATCH_DATA(addrPageDown, pPageDown, sizeof(pPageDown)); // Page Down

            // Time Change
            PATCH_DATA(addrEnd,      pEnd,      sizeof(pEnd));      // End
            PATCH_DATA(addrHome,     pHome,     sizeof(pHome));     // Home

            // Interior change
            PATCH_DATA(addrInsert,   pInsert,   sizeof(pInsert));   // Insert
            PATCH_DATA(addrDelete,   pDelete,   sizeof(pDelete));   // Delete

            // Misc
            PATCH_DATA(addrF1,       pF1,       sizeof(pF1));       // F1
            PATCH_DATA(addrF5,       pF5,       sizeof(pF5));       // F5
            PATCH_DATA(addrF9,       pF9,       sizeof(pF9));       // F9

            OutputDebugString("PATCHES::KEYS::Install()\n");
        }
    }
}