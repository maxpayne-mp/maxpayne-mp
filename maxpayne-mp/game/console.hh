#pragma once

#include <cstdarg>
#include <cstdio>
#include "lib/utils.h"

namespace CONSOLE
{
    namespace ADDRESS
    {
        constexpr const auto addrDrawText = 0x584490;
        constexpr const auto addrMother = 0x51E9E0;
    }

    //      Functions 
    // --------------------------------------------------------------

    inline char* Console_Mother(const char *a1, int a2)
    {
        __try
        {
            using Fn = char* (__cdecl*)(const char*, int);
            return reinterpret_cast<Fn>(ADDRESS::addrMother)(a1, a2);
        }
        __except (EXCEPTION_SDK)
        {
            OutputDebugString("CONSOLE::Console_Mother() exception\n");
        }
        return nullptr;
    }

    // TODO: Draw text in the (game) console 
    inline int Print(const char* format, ...)
    {
        char buf[4096];

        va_list va;
        va_start(va, format);
        vsnprintf(buf, sizeof(buf), format, va);
        va_end(va);

        OutputDebugString("%s", buf);

        using Fn = int(__cdecl*)(const char*, ...);
        return reinterpret_cast<Fn>(ADDRESS::addrDrawText)("%s", buf);
    }
}