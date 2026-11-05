#pragma once

#include <cstdarg>
#include <cstdio>
#include "lib/utils.h"
#include "console.hh"

namespace SCRIPT
{
    namespace ADDRESS
    {
    }

    //      Functions 
    // --------------------------------------------------------------

    inline bool Execute(const char* cmd)
    {
        using namespace CONSOLE;
        using namespace CONSOLE::ADDRESS;

        if (!g_XConsoleMode || !cmd) return false;

        char* self = (char*)g_XConsoleMode;
        int ctx = (int)((uintptr_t)self + 0x8A);

        using MotherFn = char* (__fastcall*)(const char*, int);
        auto mother = reinterpret_cast<MotherFn>(addrMother);

        char* ret = nullptr;
        __try { ret = mother(cmd, ctx); }
        __except (EXCEPTION_EXECUTE_HANDLER) { return false; }

        if (ret)
        {
            using SubFn = int(__fastcall*)(int, void*, int, int);
            auto sub = reinterpret_cast<SubFn>(addrSub51B6A0);
            __try { sub((int)ret, (void*)((uintptr_t)self + 156), 0, 0); }
            __except (EXCEPTION_EXECUTE_HANDLER) { return false; }

            using Fdfn = int(__fastcall*)(void*, void*);
            auto fdf0 = reinterpret_cast<Fdfn>(addrSub51FDF0);
            __try { fdf0(ret, nullptr); }
            __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
        }
        return true;
    }

    inline void Clear()
    {
        SCRIPT::Execute("X_ConsoleMode->CM_Clear( );");
    }


    // TODO: 
    // constexpr const auto addrScriptRun = 0x51E7C0;
    // inline char* Run(char* formatted, const char* iface)
    // {
    //     char* result = nullptr;
    //     __asm
    //     {
    //         mov  ecx, formatted
    //         mov  edx, iface
    //         mov  eax, ADDRESS::addrScriptRun
    //         call eax
    //         mov  result, eax
    //     }
    //     return result;
    // }
}