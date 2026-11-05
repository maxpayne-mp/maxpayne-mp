#pragma once

#include <cstdarg>
#include <cstdio>
#include <intrin.h>
#include <Windows.h>
#include "lib/utils.h"

namespace CONSOLE
{
    namespace ADDRESS
    {
        constexpr const auto addrDrawText  = 0x584490;
        constexpr const auto addrKeyboard  = 0x611AC0;
        constexpr const auto addrParser    = 0x611CB0;
        constexpr const auto addrMother    = 0x51E9E0;
        constexpr const auto addrSub51B6A0 = 0x51B6A0;
        constexpr const auto addrSub51FDF0 = 0x51FDF0;
    }

    inline void* addrCTX_Mother = nullptr;
    inline void* g_XConsoleMode = nullptr;

    using ParserFn = char(__fastcall*)(char* self, int edx, int tokens);
    inline ParserFn original_Parser = nullptr;
    inline SafetyHookInline g_Parser;

    char __fastcall Parser_Hook(char* self, int edx, int tokens)
    {
        if (self)
        {
            g_XConsoleMode = self;
            addrCTX_Mother = (void*)((uintptr_t)self + 0x8A);
        }

        char r = original_Parser(self, edx, tokens);

        #ifdef CONSOLE_DEBUG
            OutputDebugString("PARSER: self=0x%p tokens=0x%p ctx=0x%p ret=0x%02X\n",
                self, (void*)tokens, addrCTX_Mother, (unsigned)r);
        #endif
        return r;
    }

    using Console_MotherFn = char* (__fastcall*)(const char*, int);
    inline Console_MotherFn original_Console_Mother = nullptr;
    inline SafetyHookInline g_Console_Mother;

    char* __fastcall Console_Mother_Hook(const char* self, int ctx)
    {
        void* returnaddress_ = _ReturnAddress();

        char* ret = nullptr;
        if (original_Console_Mother)
            ret = original_Console_Mother(self, ctx);

        #ifdef CONSOLE_DEBUG
            OutputDebugString("CONSOLE::Console_Mother_Hook(\"%s\", 0x%p); // return address 0x%p\n",
                self, (void*)ctx, returnaddress_);
        #endif

        return ret;
    }

    using Sub51B6A0Fn = int(__fastcall*)(int a1, void* a2, int a3, int a4);
    inline Sub51B6A0Fn original_Sub51B6A0 = nullptr;
    inline SafetyHookInline g_Sub51B6A0;

    int __fastcall Sub51B6A0_Hook(int a1, void* a2, int a3, int a4)
    {
        int r = original_Sub51B6A0(a1, a2, a3, a4);
        #ifdef CONSOLE_DEBUG
            OutputDebugString("51B6A0: a1=0x%p a2=0x%p a3=0x%p a4=0x%p\n", (void*)a1, a2, (void*)a3, (void*)a4);
        #endif
        return r;
    }

    using Sub51FDF0Fn = int(__fastcall*)(void* self, void* edx);
    inline Sub51FDF0Fn original_Sub51FDF0 = nullptr;
    inline SafetyHookInline g_Sub51FDF0;

    int __fastcall Sub51FDF0_Hook(void* self, void* edx)
    {
        #ifdef CONSOLE_DEBUG
            OutputDebugString("51FDF0: this=0x%p\n", self);
        #endif
        return original_Sub51FDF0(self, edx);
    }

    using KeyboardFn = int(__fastcall*)(void* self, int edx, int a2);
    inline KeyboardFn original_Keyboard = nullptr;
    inline SafetyHookInline g_Keyboard;

    int __fastcall Keyboard_Hook(void* self, int edx, int a2)
    {
        if (self) g_XConsoleMode = self;
        int r = original_Keyboard(self, edx, a2);
        #ifdef CONSOLE_DEBUG
            OutputDebugString("KEYBOARD: self=0x%p a2=0x%p\n", self, (void*)a2);
        #endif
        return r;
    }
    
    char* Console_Mother(const char* cmd, int ctx = -1)
    {
        __try
        {
            if (ctx == -1)
            {
                ctx = (int)addrCTX_Mother;
                if (!ctx) 
                { 
                    #ifdef CONSOLE_DEBUG
                        OutputDebugString("MOTHER: ctx nulo\n"); 
                    #endif
                    return nullptr; 
                }
            }
            // OutputDebugString("MOTHER publico: cmd=\"%s\" ctx=0x%p", cmd, (void*)ctx);
            using Fn = char*(__fastcall*)(const char*, int);
            return reinterpret_cast<Fn>(ADDRESS::addrMother)(cmd, ctx);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            OutputDebugString("CONSOLE MOTHER: EXCEPTION cmd=\"%s\"\n", cmd);
        }
        return nullptr;
    }


    int Print(const char* format, ...)
    {
        char buf[4096];
        va_list va; va_start(va, format);
        vsnprintf(buf, sizeof(buf), format, va);
        va_end(va);
        #ifdef CONSOLE_DEBUG
        OutputDebugString("%s\n", buf);
        #endif 
        fflush(stdout);
        using Fn = int(__cdecl*)(const char*, ...);
        return reinterpret_cast<Fn>(ADDRESS::addrDrawText)("%s", buf);
    }


    inline void Install()
    {
        g_Parser = safetyhook::create_inline((void*)ADDRESS::addrParser, Parser_Hook);
        original_Parser = g_Parser.original<ParserFn>();

        g_Console_Mother = safetyhook::create_inline((void*)ADDRESS::addrMother, Console_Mother_Hook);
        original_Console_Mother = g_Console_Mother.original<Console_MotherFn>();

        g_Sub51B6A0 = safetyhook::create_inline((void*)ADDRESS::addrSub51B6A0, Sub51B6A0_Hook);
        original_Sub51B6A0 = g_Sub51B6A0.original<Sub51B6A0Fn>();

        g_Sub51FDF0 = safetyhook::create_inline((void*)ADDRESS::addrSub51FDF0, Sub51FDF0_Hook);
        original_Sub51FDF0 = g_Sub51FDF0.original<Sub51FDF0Fn>();

        g_Keyboard = safetyhook::create_inline((void*)ADDRESS::addrKeyboard, Keyboard_Hook);
        original_Keyboard = g_Keyboard.original<KeyboardFn>();

        OutputDebugString("CONSOLE::Install()\n");
    }
}