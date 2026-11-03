#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstring>

#include "lib/utils.h"

namespace PATCHES
{
    namespace MENU
    {
        namespace ADDRESS
        {
            constexpr uintptr_t ImageBase = 0x400000;

            constexpr uintptr_t AutoAimRead = 0x42C040 + 0x66;
            constexpr uintptr_t EnableTipsRead = 0x42C040 + 0xB2;

            constexpr uintptr_t AutoAimWrite = 0x42C130 + 0x4F;
            constexpr uintptr_t EnableTipsWrite = 0x42C130 + 0x87;
        }

        inline bool Install()
        {
            OutputDebugString("PATCHES::MENU::Install()\n");
            return false;
        }
    }
}