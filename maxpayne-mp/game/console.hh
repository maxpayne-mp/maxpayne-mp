#pragma once

#include <cstdarg>
#include <cstdio>

namespace CONSOLE
{
    namespace ADDRESS
    {
        constexpr const auto addrDrawText = 0x584490;
    }

    //      Functions 
    // --------------------------------------------------------------

    // TODO: Draw text in the (game) console 
    inline int DrawText(const char* format, ...)
    {
        char buf[4096];

        va_list va;
        va_start(va, format);
        vsnprintf(buf, sizeof(buf), format, va);
        va_end(va);

        std::printf("%s", buf);

        using Fn = int(__cdecl*)(const char*, ...);
        return reinterpret_cast<Fn>(ADDRESS::addrDrawText)("%s", buf);
    }
}