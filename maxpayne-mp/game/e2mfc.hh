#pragma once

#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <intrin.h>
#include "lib/utils.h"
#include "lib/safetyhook.hpp"


namespace E2MFC
{
    inline uintptr_t g_base = 0;
    constexpr const auto addrImageBase = 0x10000000;

    inline uintptr_t R(uintptr_t dumpAddr)
    {
        return g_base + (dumpAddr - addrImageBase);
    }

    struct M_Vector2 { float x, y; };
    struct M_Vector3 { float x, y, z; };
    struct G_FColor  { float r, g, b, a; };

    namespace P_Font
    {
        inline void* g_lastFont = nullptr;
        inline int g_ctorCount = 0;
        inline int g_dtorCount = 0;

        using Ctor1Fn    = void(__thiscall*)(void*, const void*);
        using Ctor2Fn    = void(__thiscall*)(void*, const char*);
        using Ctor3Fn    = void(__thiscall*)(void*, const char*, const char*);
        using DtorFn     = void(__thiscall*)(void*);
        using ValidateFn = void(__thiscall*)(void*);

        inline SafetyHookInline g_hkCtor1;
        inline SafetyHookInline g_hkCtor2;
        inline SafetyHookInline g_hkCtor3;
        inline SafetyHookInline g_hkDtor;
        inline SafetyHookInline g_hkValidate;

        inline Ctor1Fn    g_origCtor1    = nullptr;
        inline Ctor2Fn    g_origCtor2    = nullptr;
        inline Ctor3Fn    g_origCtor3    = nullptr;
        inline DtorFn     g_origDtor     = nullptr;
        inline ValidateFn g_origValidate = nullptr;

        inline void __fastcall Ctor1_Hook(void* s, void*, const void* o)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Font] ctor1 this=%p src=%p ret=%p\n", s, o, _ReturnAddress());
#endif
            g_origCtor1(s, o);
        }
        inline void __fastcall Ctor2_Hook(void* s, void*, const char* n)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Font] ctor2 this=%p name=\"%s\" ret=%p\n", s, n ? n : "(null)", _ReturnAddress());
#endif
            g_origCtor2(s, n);
        }
        inline void __fastcall Ctor3_Hook(void* s, void*, const char* n, const char* p)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Font] ctor3 this=%p name=\"%s\" path=\"%s\" ret=%p\n", s, n ? n : "(null)", p ? p : "(null)", _ReturnAddress());
#endif
            g_origCtor3(s, n, p);
        }
        inline void __fastcall Dtor_Hook(void* s, void*)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Font] dtor this=%p ret=%p\n", s, _ReturnAddress());
#endif
            g_origDtor(s);
        }
        inline void __fastcall Validate_Hook(void* s, void*)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Font] validate this=%p ret=%p\n", s, _ReturnAddress());
#endif
            g_origValidate(s);
        }
    }

    namespace P_Text
    {
        inline void* g_lastText = nullptr;
        inline int g_printCount = 0;
        inline int g_ctorCount = 0;
        inline int g_dtorCount = 0;

        using CtorFontFn     = void(__thiscall*)(void*, void*, int);
        using CtorCopyFn     = void(__thiscall*)(void*, const void*);
        using DtorFn         = void(__thiscall*)(void*);
        using SetPosition2Fn = void(__thiscall*)(void*, float, float);
        using SetColorFn     = void(__thiscall*)(void*, const G_FColor*);
        using SetScale2Fn    = void(__thiscall*)(void*, float, float);
        using SetScale1Fn    = void(__thiscall*)(void*, float);
        using SetFontFn      = void(__thiscall*)(void*, void*);
        using SetZAngleFn    = void(__thiscall*)(void*, float);
        using SetRefPointFn  = void(__thiscall*)(void*, int);
        using PrintFn        = void(__thiscall*)(void*, const char*);
        using ClearFn        = void(__thiscall*)(void*);
        using AppendFn       = void(__thiscall*)(void*, const G_FColor*, const char*);
        using ExecuteAlwaysFn= bool(__thiscall*)(void*);
        using DumpFn         = void(__thiscall*)(void*, bool);
        using CalcBoundsFn   = void(__thiscall*)(void*);
        using DuplicateFn    = void(__thiscall*)(void*, const void*);
        using CopyContentsFn = void(__thiscall*)(void*, const void*);
        using IsInsideViewFn = bool(__thiscall*)(void*, void*);

        inline SafetyHookInline g_hkCtorFont;
        inline SafetyHookInline g_hkCtorCopy;
        inline SafetyHookInline g_hkDtor;
        inline SafetyHookInline g_hkSetPos2a;
        inline SafetyHookInline g_hkSetPos2b;
        inline SafetyHookInline g_hkSetColor;
        inline SafetyHookInline g_hkSetScale2;
        inline SafetyHookInline g_hkSetScale1;
        inline SafetyHookInline g_hkSetFont;
        inline SafetyHookInline g_hkSetZAngle;
        inline SafetyHookInline g_hkSetRefPoint;
        inline SafetyHookInline g_hkPrint;
        inline SafetyHookInline g_hkClear;
        inline SafetyHookInline g_hkAppend;
        inline SafetyHookInline g_hkExecuteAlways;
        inline SafetyHookInline g_hkDump;
        inline SafetyHookInline g_hkCalcBounds;
        inline SafetyHookInline g_hkDuplicate;
        inline SafetyHookInline g_hkCopyContents;
        inline SafetyHookInline g_hkIsInsideView;

        inline CtorFontFn      g_origCtorFont      = nullptr;
        inline CtorCopyFn      g_origCtorCopy      = nullptr;
        inline DtorFn          g_origDtor          = nullptr;
        inline SetPosition2Fn  g_origSetPos2a      = nullptr;
        inline SetPosition2Fn  g_origSetPos2b      = nullptr;
        inline SetColorFn      g_origSetColor      = nullptr;
        inline SetScale2Fn     g_origSetScale2     = nullptr;
        inline SetScale1Fn     g_origSetScale1     = nullptr;
        inline SetFontFn       g_origSetFont       = nullptr;
        inline SetZAngleFn     g_origSetZAngle     = nullptr;
        inline SetRefPointFn   g_origSetRefPoint   = nullptr;
        inline PrintFn         g_origPrint         = nullptr;
        inline ClearFn         g_origClear         = nullptr;
        inline AppendFn        g_origAppend        = nullptr;
        inline ExecuteAlwaysFn g_origExecuteAlways = nullptr;
        inline DumpFn          g_origDump          = nullptr;
        inline CalcBoundsFn    g_origCalcBounds    = nullptr;
        inline DuplicateFn     g_origDuplicate     = nullptr;
        inline CopyContentsFn  g_origCopyContents  = nullptr;
        inline IsInsideViewFn  g_origIsInsideView  = nullptr;

        inline void __fastcall CtorFont_Hook(void* s, void*, void* f, int rp)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] ctorFont this=%p font=%p refPoint=%d ret=%p\n", s, f, rp, _ReturnAddress());
#endif
            g_origCtorFont(s, f, rp);
            if (f) P_Font::g_lastFont = f;
        }
        inline void __fastcall CtorCopy_Hook(void* s, void*, const void* o)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] ctorCopy this=%p src=%p ret=%p\n", s, o, _ReturnAddress());
#endif
            g_origCtorCopy(s, o);
        }
        inline void __fastcall Dtor_Hook(void* s, void*)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] dtor this=%p ret=%p\n", s, _ReturnAddress());
#endif
            g_origDtor(s);
        }
        inline void __fastcall SetPos2a_Hook(void* s, void*, float x, float y)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] setPos2a this=%p x=%.3f y=%.3f ret=%p\n", s, x, y, _ReturnAddress());
#endif
            g_origSetPos2a(s, x, y);
        }
        inline void __fastcall SetPos2b_Hook(void* s, void*, float x, float y)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] setPos2b this=%p x=%.3f y=%.3f ret=%p\n", s, x, y, _ReturnAddress());
#endif
            g_origSetPos2b(s, x, y);
        }
        inline void __fastcall SetColor_Hook(void* s, void*, const G_FColor* c)
        {
#ifdef E2MFC_DEBUG
            if (c) OutputDebugString("[E2MFC][P_Text] setColor this=%p r=%.3f g=%.3f b=%.3f a=%.3f ret=%p\n", s, c->r, c->g, c->b, c->a, _ReturnAddress());
            else   OutputDebugString("[E2MFC][P_Text] setColor this=%p null ret=%p\n", s, _ReturnAddress());
#endif
            g_origSetColor(s, c);
        }
        inline void __fastcall SetScale2_Hook(void* s, void*, float sx, float sy)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] setScale2 this=%p sx=%.3f sy=%.3f ret=%p\n", s, sx, sy, _ReturnAddress());
#endif
            g_origSetScale2(s, sx, sy);
        }
        inline void __fastcall SetScale1_Hook(void* s, void*, float sc)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] setScale1 this=%p sc=%.3f ret=%p\n", s, sc, _ReturnAddress());
#endif
            g_origSetScale1(s, sc);
        }
        inline void __fastcall SetFont_Hook(void* s, void*, void* f)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] setFont this=%p font=%p ret=%p\n", s, f, _ReturnAddress());
#endif
            if (f) P_Font::g_lastFont = f;
            g_origSetFont(s, f);
        }
        inline void __fastcall SetZAngle_Hook(void* s, void*, float a)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] setZAngle this=%p angle=%.3f ret=%p\n", s, a, _ReturnAddress());
#endif
            g_origSetZAngle(s, a);
        }
        inline void __fastcall SetRefPoint_Hook(void* s, void*, int rp)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] setRefPoint this=%p rp=%d ret=%p\n", s, rp, _ReturnAddress());
#endif
            g_origSetRefPoint(s, rp);
        }
        inline void __fastcall Print_Hook(void* s, void*, const char* t)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] print this=%p text=\"%s\" ret=%p\n", s, t ? t : "(null)", _ReturnAddress());
#endif
            if (s) g_lastText = s;
            ++g_printCount;
            g_origPrint(s, t);
        }
        inline void __fastcall Clear_Hook(void* s, void*)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] clear this=%p ret=%p\n", s, _ReturnAddress());
#endif
            g_origClear(s);
        }
        inline void __fastcall Append_Hook(void* s, void*, const G_FColor* c, const char* t)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] append this=%p text=\"%s\" ret=%p\n", s, t ? t : "(null)", _ReturnAddress());
#endif
            g_origAppend(s, c, t);
        }
        inline bool __fastcall ExecuteAlways_Hook(void* s, void*)
        {
            bool r = g_origExecuteAlways(s);
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] executeAlways this=%p r=%d ret=%p\n", s, r ? 1 : 0, _ReturnAddress());
#endif
            return r;
        }
        inline void __fastcall Dump_Hook(void* s, void*, bool f)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] dump this=%p flag=%d ret=%p\n", s, f ? 1 : 0, _ReturnAddress());
#endif
            g_origDump(s, f);
        }
        inline void __fastcall CalcBounds_Hook(void* s, void*)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] calcBounds this=%p ret=%p\n", s, _ReturnAddress());
#endif
            g_origCalcBounds(s);
        }
        inline void __fastcall Duplicate_Hook(void* s, void*, const void* o)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] duplicate this=%p src=%p ret=%p\n", s, o, _ReturnAddress());
#endif
            g_origDuplicate(s, o);
        }
        inline void __fastcall CopyContents_Hook(void* s, void*, const void* o)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] copyContents this=%p src=%p ret=%p\n", s, o, _ReturnAddress());
#endif
            g_origCopyContents(s, o);
        }
        inline bool __fastcall IsInsideView_Hook(void* s, void*, void* cam)
        {
            bool r = g_origIsInsideView(s, cam);
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Text] isInsideView this=%p cam=%p r=%d ret=%p\n", s, cam, r ? 1 : 0, _ReturnAddress());
#endif
            return r;
        }
    }

    namespace P_Scene
    {
        inline void* g_lastScene = nullptr;
        inline int g_renderCount = 0;

        using RenderFn = void(__thiscall*)(void*, void*);

        inline SafetyHookInline g_hkRender;
        inline RenderFn g_origRender = nullptr;

        inline void __fastcall Render_Hook(void* s, void*, void* cam)
        {
            if (s) g_lastScene = s;
            ++g_renderCount;
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Scene] render this=%p cam=%p count=%d ret=%p\n", s, cam, g_renderCount, _ReturnAddress());
#endif
            g_origRender(s, cam);
        }
    }

    namespace P_BaseObject
    {
        inline void* g_lastContainer = nullptr;
        inline int g_addToCount = 0;
        inline int g_removeCount = 0;

        using AddToFn   = void(__thiscall*)(void*, void*);
        using RemoveFn  = void(__thiscall*)(void*);
        using SetPos3Fn = void(__thiscall*)(void*, float, float, float);
        using EnableFn  = void(__thiscall*)(void*, bool);

        inline SafetyHookInline g_hkAddTo;
        inline SafetyHookInline g_hkRemove;
        inline SafetyHookInline g_hkSetPos;
        inline SafetyHookInline g_hkEnable;

        inline AddToFn   g_origAddTo  = nullptr;
        inline RemoveFn  g_origRemove = nullptr;
        inline SetPos3Fn g_origSetPos = nullptr;
        inline EnableFn  g_origEnable = nullptr;

        inline void __fastcall AddTo_Hook(void* s, void*, void* c)
        {
            g_lastContainer = c;
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_BaseObject] addTo this=%p container=%p count=%d ret=%p\n", s, c, ++g_addToCount, _ReturnAddress());
#else
            ++g_addToCount;
#endif
            g_origAddTo(s, c);
        }
        inline void __fastcall Remove_Hook(void* s, void*)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_BaseObject] remove this=%p count=%d ret=%p\n", s, ++g_removeCount, _ReturnAddress());
#else
            ++g_removeCount;
#endif
            g_origRemove(s);
        }
        inline void __fastcall SetPos_Hook(void* s, void*, float x, float y, float z)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_BaseObject] setPosition this=%p x=%.3f y=%.3f z=%.3f ret=%p\n", s, x, y, z, _ReturnAddress());
#endif
            g_origSetPos(s, x, y, z);
        }
        inline void __fastcall Enable_Hook(void* s, void*, bool e)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_BaseObject] enable this=%p v=%d ret=%p\n", s, e ? 1 : 0, _ReturnAddress());
#endif
            g_origEnable(s, e);
        }
    }

    namespace P_Map
    {
        using ValidateFn = void(__thiscall*)(void*);

        inline SafetyHookInline g_hkValidate;
        inline ValidateFn g_origValidate = nullptr;

        inline void __fastcall Validate_Hook(void* s, void*)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Map] validate this=%p ret=%p\n", s, _ReturnAddress());
#endif
            g_origValidate(s);
        }
    }

    namespace P_Material
    {
        using InitFn     = void(__thiscall*)(void*, uint32_t, uint32_t);
        using ValidateFn = void(__thiscall*)(void*);

        inline SafetyHookInline g_hkInit;
        inline SafetyHookInline g_hkValidate;
        inline InitFn g_origInit = nullptr;
        inline ValidateFn g_origValidate = nullptr;

        inline void __fastcall Init_Hook(void* s, void*, uint32_t a, uint32_t b)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Material] init this=%p a=0x%08X b=0x%08X ret=%p\n", s, a, b, _ReturnAddress());
#endif
            g_origInit(s, a, b);
        }
        inline void __fastcall Validate_Hook(void* s, void*)
        {
#ifdef E2MFC_DEBUG
            OutputDebugString("[E2MFC][P_Material] validate this=%p ret=%p\n", s, _ReturnAddress());
#endif
            g_origValidate(s);
        }
    }

    inline void Install()
    {
        HMODULE hMod = GetModuleHandleW(L"e2mfc.dll");
        if (!hMod) return;
        g_base = (uintptr_t)hMod;
#ifdef E2MFC_DEBUG
        OutputDebugString("[E2MFC] Install start\n");
#endif

        P_Font::g_hkCtor1    = safetyhook::create_inline((void*)R(0x10001000), P_Font::Ctor1_Hook);
        P_Font::g_hkCtor2    = safetyhook::create_inline((void*)R(0x100019E0), P_Font::Ctor2_Hook);
        P_Font::g_hkCtor3    = safetyhook::create_inline((void*)R(0x10001B20), P_Font::Ctor3_Hook);
        P_Font::g_hkDtor     = safetyhook::create_inline((void*)R(0x10001D30), P_Font::Dtor_Hook);
        P_Font::g_hkValidate = safetyhook::create_inline((void*)R(0x10001E60), P_Font::Validate_Hook);
        P_Font::g_origCtor1    = P_Font::g_hkCtor1.original<P_Font::Ctor1Fn>();
        P_Font::g_origCtor2    = P_Font::g_hkCtor2.original<P_Font::Ctor2Fn>();
        P_Font::g_origCtor3    = P_Font::g_hkCtor3.original<P_Font::Ctor3Fn>();
        P_Font::g_origDtor     = P_Font::g_hkDtor.original<P_Font::DtorFn>();
        P_Font::g_origValidate = P_Font::g_hkValidate.original<P_Font::ValidateFn>();

        P_Text::g_hkCtorFont      = safetyhook::create_inline((void*)R(0x100033F0), P_Text::CtorFont_Hook);
        P_Text::g_hkCtorCopy      = safetyhook::create_inline((void*)R(0x10003650), P_Text::CtorCopy_Hook);
        P_Text::g_hkDtor          = safetyhook::create_inline((void*)R(0x10003A20), P_Text::Dtor_Hook);
        P_Text::g_hkSetPos2a      = safetyhook::create_inline((void*)R(0x10002F70), P_Text::SetPos2a_Hook);
        P_Text::g_hkSetPos2b      = safetyhook::create_inline((void*)R(0x10002F90), P_Text::SetPos2b_Hook);
        P_Text::g_hkSetColor      = safetyhook::create_inline((void*)R(0x10002EF0), P_Text::SetColor_Hook);
        P_Text::g_hkSetScale2     = safetyhook::create_inline((void*)R(0x10002E80), P_Text::SetScale2_Hook);
        P_Text::g_hkSetScale1     = safetyhook::create_inline((void*)R(0x10002EA0), P_Text::SetScale1_Hook);
        P_Text::g_hkSetFont       = safetyhook::create_inline((void*)R(0x10002E50), P_Text::SetFont_Hook);
        P_Text::g_hkSetZAngle     = safetyhook::create_inline((void*)R(0x10002E30), P_Text::SetZAngle_Hook);
        P_Text::g_hkSetRefPoint   = safetyhook::create_inline((void*)R(0x100058C0), P_Text::SetRefPoint_Hook);
        P_Text::g_hkPrint         = safetyhook::create_inline((void*)R(0x10003AE0), P_Text::Print_Hook);
        P_Text::g_hkClear         = safetyhook::create_inline((void*)R(0x10003CB0), P_Text::Clear_Hook);
        P_Text::g_hkAppend        = safetyhook::create_inline((void*)R(0x10003E40), P_Text::Append_Hook);
        P_Text::g_hkExecuteAlways = safetyhook::create_inline((void*)R(0x10004280), P_Text::ExecuteAlways_Hook);
        P_Text::g_hkDump          = safetyhook::create_inline((void*)R(0x10004E60), P_Text::Dump_Hook);
        P_Text::g_hkCalcBounds    = safetyhook::create_inline((void*)R(0x10003AD0), P_Text::CalcBounds_Hook);
        P_Text::g_hkDuplicate     = safetyhook::create_inline((void*)R(0x10003710), P_Text::Duplicate_Hook);
        P_Text::g_hkCopyContents  = safetyhook::create_inline((void*)R(0x10003740), P_Text::CopyContents_Hook);
        P_Text::g_hkIsInsideView  = safetyhook::create_inline((void*)R(0x10007E40), P_Text::IsInsideView_Hook);
        P_Text::g_origCtorFont      = P_Text::g_hkCtorFont.original<P_Text::CtorFontFn>();
        P_Text::g_origCtorCopy      = P_Text::g_hkCtorCopy.original<P_Text::CtorCopyFn>();
        P_Text::g_origDtor          = P_Text::g_hkDtor.original<P_Text::DtorFn>();
        P_Text::g_origSetPos2a      = P_Text::g_hkSetPos2a.original<P_Text::SetPosition2Fn>();
        P_Text::g_origSetPos2b      = P_Text::g_hkSetPos2b.original<P_Text::SetPosition2Fn>();
        P_Text::g_origSetColor      = P_Text::g_hkSetColor.original<P_Text::SetColorFn>();
        P_Text::g_origSetScale2     = P_Text::g_hkSetScale2.original<P_Text::SetScale2Fn>();
        P_Text::g_origSetScale1     = P_Text::g_hkSetScale1.original<P_Text::SetScale1Fn>();
        P_Text::g_origSetFont       = P_Text::g_hkSetFont.original<P_Text::SetFontFn>();
        P_Text::g_origSetZAngle     = P_Text::g_hkSetZAngle.original<P_Text::SetZAngleFn>();
        P_Text::g_origSetRefPoint   = P_Text::g_hkSetRefPoint.original<P_Text::SetRefPointFn>();
        P_Text::g_origPrint         = P_Text::g_hkPrint.original<P_Text::PrintFn>();
        P_Text::g_origClear         = P_Text::g_hkClear.original<P_Text::ClearFn>();
        P_Text::g_origAppend        = P_Text::g_hkAppend.original<P_Text::AppendFn>();
        P_Text::g_origExecuteAlways = P_Text::g_hkExecuteAlways.original<P_Text::ExecuteAlwaysFn>();
        P_Text::g_origDump          = P_Text::g_hkDump.original<P_Text::DumpFn>();
        P_Text::g_origCalcBounds    = P_Text::g_hkCalcBounds.original<P_Text::CalcBoundsFn>();
        P_Text::g_origDuplicate     = P_Text::g_hkDuplicate.original<P_Text::DuplicateFn>();
        P_Text::g_origCopyContents  = P_Text::g_hkCopyContents.original<P_Text::CopyContentsFn>();
        P_Text::g_origIsInsideView  = P_Text::g_hkIsInsideView.original<P_Text::IsInsideViewFn>();

        P_Scene::g_hkRender = safetyhook::create_inline((void*)R(0x10018EC0), P_Scene::Render_Hook);
        P_Scene::g_origRender = P_Scene::g_hkRender.original<P_Scene::RenderFn>();

        P_BaseObject::g_hkAddTo   = safetyhook::create_inline((void*)R(0x1001C6B0), P_BaseObject::AddTo_Hook);
        P_BaseObject::g_hkRemove  = safetyhook::create_inline((void*)R(0x1001C770), P_BaseObject::Remove_Hook);
        P_BaseObject::g_hkSetPos  = safetyhook::create_inline((void*)R(0x1001C550), P_BaseObject::SetPos_Hook);
        P_BaseObject::g_hkEnable  = safetyhook::create_inline((void*)R(0x1001C4D0), P_BaseObject::Enable_Hook);
        P_BaseObject::g_origAddTo  = P_BaseObject::g_hkAddTo.original<P_BaseObject::AddToFn>();
        P_BaseObject::g_origRemove = P_BaseObject::g_hkRemove.original<P_BaseObject::RemoveFn>();
        P_BaseObject::g_origSetPos = P_BaseObject::g_hkSetPos.original<P_BaseObject::SetPos3Fn>();
        P_BaseObject::g_origEnable = P_BaseObject::g_hkEnable.original<P_BaseObject::EnableFn>();

        P_Map::g_hkValidate = safetyhook::create_inline((void*)R(0x1001FED0), P_Map::Validate_Hook);
        P_Map::g_origValidate = P_Map::g_hkValidate.original<P_Map::ValidateFn>();

        P_Material::g_hkInit     = safetyhook::create_inline((void*)R(0x10023E10), P_Material::Init_Hook);
        P_Material::g_hkValidate = safetyhook::create_inline((void*)R(0x10024780), P_Material::Validate_Hook);
        P_Material::g_origInit     = P_Material::g_hkInit.original<P_Material::InitFn>();
        P_Material::g_origValidate = P_Material::g_hkValidate.original<P_Material::ValidateFn>();

        OutputDebugString("E2MFC::Install()\n");
    }
}