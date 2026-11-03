#pragma once

#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <intrin.h>
#include <Windows.h>
#include "lib/utils.h"


namespace CONSOLE
{
    namespace ADDRESS
    {
        constexpr const auto addrDrawText      = 0x584490;
        constexpr const auto addrMother        = 0x51E9E0;
        constexpr const auto addrSub51E8C0     = 0x51E8C0;
        constexpr const auto addrConsoleParser = 0x611E10;
    }

    inline void* addrCTX_Mother = nullptr;

    //      Functions 
    // --------------------------------------------------------------

    inline char* Console_Mother(const char *a1, int ctx = -1)
    {
        __try
        {
            if(ctx == -1)
            {
                ctx = (int)addrCTX_Mother;
                if(!ctx)
                {
                    #ifdef CONSOLE_DEBUG
                        OutputDebugString("CONSOLE::Console_Mother() ctx is null\n");
                    #endif
                    return nullptr;
                }
            }
            void* returnaddress_ = _ReturnAddress();

            #ifdef CONSOLE_DEBUG
            OutputDebugString("CONSOLE::Console_Mother() a1=\"%s\", ctx=0x%p // return address 0x%p\n", a1, (void*)ctx, (void*)returnaddress_);
            #endif
            using Fn = char* (__fastcall*)(const char*, int);
            return reinterpret_cast<Fn>(ADDRESS::addrMother)(a1, ctx);
        }
        __except (EXCEPTION_SDK)
        {
            OutputDebugString("CONSOLE::Console_Mother() The command \"%s\" is likely incorrect, or does not exist.\n", a1);
        }
        return nullptr;
    }

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

    //

    using Console_MotherFn = char* (__fastcall*)(const char*, int);
    inline Console_MotherFn original_Console_Mother = nullptr;
    inline SafetyHookInline g_Console_Mother;

    char* __fastcall Console_Mother_Hook(const char* self, int ctx)
    {
        void* returnaddress_ = _ReturnAddress();

        if (ctx != -1 && ctx != 0)
        {
            addrCTX_Mother = (void*)ctx;
        }

        char* ret = nullptr;
        if (original_Console_Mother)
            ret = original_Console_Mother(self, ctx);

        #ifdef CONSOLE_DEBUG
        OutputDebugString("CONSOLE::Console_Mother_Hook(\"%s\", 0x%p); // return address 0x%p\n", self, (void*)ctx, returnaddress_);
        #endif

        return ret;
    }

    //

    using Sub51E8C0Fn = char* (__fastcall*)(const char*, int, int);
    inline Sub51E8C0Fn original_Sub51E8C0 = nullptr;
    inline SafetyHookInline g_Sub51E8C0;

    char* __fastcall Sub51E8C0_Hook(const char* a1, int a2, int a3)
    {
        void* returnaddress_ = _ReturnAddress();

        char* ret = nullptr;
        if (original_Sub51E8C0)
            ret = original_Sub51E8C0(a1, a2, a3);

        #ifdef CONSOLE_DEBUG
        OutputDebugString("CONSOLE::Sub51E8C0_Hook() a1=\"%s\", a2=0x%p, a3=0x%p // return address=0x%p\n", a1, (void*)a2, (void*)a3, returnaddress_);
        #endif

        return ret;
    }

    //


    using Console_ParserFn = char(__fastcall*)(char* self, int /*edx dummy*/, int tokens);
    inline Console_ParserFn original_Console_Parser = nullptr;
    inline SafetyHookInline g_Console_Parser;

    char __fastcall Console_Parser_Hook(char* self, int /*edx dummy*/, int tokens)
    {
        if (self)
            addrCTX_Mother = (void*)((uintptr_t)self + 0x8A);

        char ret = 0;
        if (original_Console_Parser)
            ret = original_Console_Parser(self, 0, tokens);   // ← passe o dummy!

        return ret;
    }

    //

    void Install()
    {
        g_Console_Mother = safetyhook::create_inline((void*)ADDRESS::addrMother, Console_Mother_Hook);
        original_Console_Mother = g_Console_Mother.original<Console_MotherFn>();

        g_Sub51E8C0 = safetyhook::create_inline((void*)ADDRESS::addrSub51E8C0, Sub51E8C0_Hook);
        original_Sub51E8C0 = g_Sub51E8C0.original<Sub51E8C0Fn>();

        g_Console_Parser = safetyhook::create_inline((void*)ADDRESS::addrConsoleParser, Console_Parser_Hook);
        original_Console_Parser = g_Console_Parser.original<Console_ParserFn>();

        OutputDebugString("CONSOLE::Install()\n");
    }
}