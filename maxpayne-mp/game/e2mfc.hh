#pragma once

#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <intrin.h>
#include "lib/utils.h"
#include "lib/safetyhook.hpp"

#define E2MFC_LOG_ENABLED

#define E2MFC_LOG_FONT
#define E2MFC_LOG_TEXT
//#define E2MFC_LOG_SCENE
//#define E2MFC_LOG_BASEOBJECT
//#define E2MFC_LOG_MAP
//#define E2MFC_LOG_MATERIAL

#define E2MFC_LOG_VERBOSE

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

    namespace Log
    {
#ifdef E2MFC_LOG_ENABLED
        inline void F(const char* fmt, ...)
        {
            char buf[1024];
            va_list ap;
            va_start(ap, fmt);
            _vsnprintf_s(buf, _TRUNCATE, fmt, ap);
            va_end(ap);
            OutputDebugStringA(buf);
            OutputDebugStringA("\n");
        }
#else
        inline void F(const char*, ...) {}
#endif
    }

#ifdef E2MFC_LOG_FONT
#define E2MFC_LOG_FONT_ONLY(...) E2MFC::Log::F(__VA_ARGS__)
#else
#define E2MFC_LOG_FONT_ONLY(...) do {} while (0)
#endif

#ifdef E2MFC_LOG_TEXT
#define E2MFC_LOG_TEXT_ONLY(...) E2MFC::Log::F(__VA_ARGS__)
#else
#define E2MFC_LOG_TEXT_ONLY(...) do {} while (0)
#endif

#ifdef E2MFC_LOG_SCENE
#define E2MFC_LOG_SCENE_ONLY(...) E2MFC::Log::F(__VA_ARGS__)
#else
#define E2MFC_LOG_SCENE_ONLY(...) do {} while (0)
#endif

#ifdef E2MFC_LOG_BASEOBJECT
#define E2MFC_LOG_BASEOBJECT_ONLY(...) E2MFC::Log::F(__VA_ARGS__)
#else
#define E2MFC_LOG_BASEOBJECT_ONLY(...) do {} while (0)
#endif

#ifdef E2MFC_LOG_MAP
#define E2MFC_LOG_MAP_ONLY(...) E2MFC::Log::F(__VA_ARGS__)
#else
#define E2MFC_LOG_MAP_ONLY(...) do {} while (0)
#endif

#ifdef E2MFC_LOG_MATERIAL
#define E2MFC_LOG_MATERIAL_ONLY(...) E2MFC::Log::F(__VA_ARGS__)
#else
#define E2MFC_LOG_MATERIAL_ONLY(...) do {} while (0)
#endif

    namespace P_Font
    {
        inline void* g_lastFont = nullptr;
        inline int g_ctorCount = 0;
        inline int g_dtorCount = 0;

        using P_Font_CtorCopyFn         = void(__thiscall*)(void*, const void*);
        using P_Font_CtorNameFn         = void(__thiscall*)(void*, const char*);
        using P_Font_CtorNamePathFn     = void(__thiscall*)(void*, const char*, const char*);
        using P_Font_DtorFn             = int(__thiscall*)(void*);
        using P_Font_GetViewportSizeFn  = float*(__thiscall*)(void*, float*, const char*);
        using P_Font_GetViewportSizefFn = int(__cdecl*)(void*, float*, const char*, ...);
        using P_Font_ValidateFn         = void(__thiscall*)(void*);

        inline SafetyHookInline g_hkP_Font_CtorCopy;
        inline SafetyHookInline g_hkP_Font_CtorName;
        inline SafetyHookInline g_hkP_Font_CtorNamePath;
        inline SafetyHookInline g_hkP_Font_Dtor;
        inline SafetyHookInline g_hkP_Font_GetViewportSize;
        inline SafetyHookInline g_hkP_Font_GetViewportSizef;
        inline SafetyHookInline g_hkP_Font_Validate;

        inline P_Font_CtorCopyFn         g_origP_Font_CtorCopy         = nullptr;
        inline P_Font_CtorNameFn         g_origP_Font_CtorName         = nullptr;
        inline P_Font_CtorNamePathFn     g_origP_Font_CtorNamePath     = nullptr;
        inline P_Font_DtorFn             g_origP_Font_Dtor             = nullptr;
        inline P_Font_GetViewportSizeFn  g_origP_Font_GetViewportSize  = nullptr;
        inline P_Font_GetViewportSizefFn g_origP_Font_GetViewportSizef = nullptr;
        inline P_Font_ValidateFn         g_origP_Font_Validate         = nullptr;

        inline void CtorCopy(void* f, const void* src)                        { g_origP_Font_CtorCopy(f, src); }
        inline void CtorName(void* f, const char* name)                       { g_origP_Font_CtorName(f, name); }
        inline void CtorNamePath(void* f, const char* name, const char* path) { g_origP_Font_CtorNamePath(f, name, path); }
        inline int  Dtor(void* f)                                             { return g_origP_Font_Dtor(f); }

        inline float* GetViewportSize(void* f, float* outVec2, const char* str)
        {
            return g_origP_Font_GetViewportSize(f, outVec2, str);
        }

        inline M_Vector2 GetViewportSizeVec(void* f, const char* str)
        {
            M_Vector2 v{ 0.0f, 0.0f };
            g_origP_Font_GetViewportSize(f, &v.x, str);
            return v;
        }

        inline int GetViewportSizef(void* f, float* outVec2, const char* fmt, ...)
        {
            if (!f || !g_origP_Font_GetViewportSizef || !fmt) return 0;
            va_list ap;
            va_start(ap, fmt);
            char b[1024];
            _vsnprintf_s(b, _TRUNCATE, fmt, ap);
            va_end(ap);
            return g_origP_Font_GetViewportSizef(f, outVec2, "%s", b);
        }

        inline void Validate(void* f) { g_origP_Font_Validate(f); }
        inline void* GetLast()        { return g_lastFont; }

        inline void __fastcall P_Font_CtorCopy_Hook(void* s, void*, const void* o)
        {
            E2MFC_LOG_FONT_ONLY("[E2MFC][P_Font] ctorCopy this=%p src=%p", s, o);
            g_origP_Font_CtorCopy(s, o);
        }
        inline void __fastcall P_Font_CtorName_Hook(void* s, void*, const char* n)
        {
            E2MFC_LOG_FONT_ONLY("[E2MFC][P_Font] ctorName this=%p name=\"%s\"", s, n ? n : "(null)");
            g_origP_Font_CtorName(s, n);
        }
        inline void __fastcall P_Font_CtorNamePath_Hook(void* s, void*, const char* n, const char* p)
        {
            E2MFC_LOG_FONT_ONLY("[E2MFC][P_Font] ctorNamePath this=%p name=\"%s\" path=\"%s\"",
                                s, n ? n : "(null)", p ? p : "(null)");
            g_origP_Font_CtorNamePath(s, n, p);
            g_lastFont = s;
        }
        inline int __fastcall P_Font_Dtor_Hook(void* s, void*)
        {
            E2MFC_LOG_FONT_ONLY("[E2MFC][P_Font] dtor this=%p", s);
            return g_origP_Font_Dtor(s);
        }
        inline float* __fastcall P_Font_GetViewportSize_Hook(void* s, void*, float* outVec2, const char* str)
        {
            E2MFC_LOG_FONT_ONLY("[E2MFC][P_Font] getViewportSize this=%p str=\"%s\"",
                                s, str ? str : "(null)");
            float* r = g_origP_Font_GetViewportSize(s, outVec2, str);
            if (outVec2)
            {
                E2MFC_LOG_FONT_ONLY("[E2MFC][P_Font]   -> w=%.3f h=%.3f", outVec2[0], outVec2[1]);
            }
            return r;
        }
        inline int __cdecl P_Font_GetViewportSizef_Hook(void* s, float* outVec2, const char* fmt, ...)
        {
            char buf[1024];
            va_list ap;
            va_start(ap, fmt);
            _vsnprintf_s(buf, _TRUNCATE, fmt, ap);
            va_end(ap);
            E2MFC_LOG_FONT_ONLY("[E2MFC][P_Font] getViewportSizef this=%p fmt=\"%s\" out=\"%s\"",
                                s, fmt ? fmt : "(null)", buf);
            return g_origP_Font_GetViewportSizef(s, outVec2, "%s", buf);
        }
        inline void __fastcall P_Font_Validate_Hook(void* s, void*)
        {
            E2MFC_LOG_FONT_ONLY("[E2MFC][P_Font] validate this=%p", s);
            g_origP_Font_Validate(s);
        }
    }

    namespace P_Text
    {
        inline void* g_lastText = nullptr;
        inline int g_printCount = 0;
        inline int g_printfCount = 0;
        inline int g_ctorCount = 0;
        inline int g_dtorCount = 0;

        using CtorFontFn      = void(__thiscall*)(void*, void*, int);
        using CtorCopyFn      = void(__thiscall*)(void*, const void*);
        using DtorFn          = void(__thiscall*)(void*);
        using SetPosition2Fn  = void(__thiscall*)(void*, float, float);
        using SetPositionV3Fn = void(__thiscall*)(void*, const M_Vector3*);
        using SetColorFn      = void(__thiscall*)(void*, const G_FColor*);
        using SetScale2Fn     = void(__thiscall*)(void*, float, float);
        using SetScale1Fn     = void(__thiscall*)(void*, float);
        using SetFontFn       = void(__thiscall*)(void*, void*);
        using SetZAngleFn     = void(__thiscall*)(void*, float);
        using SetRefPointFn   = void(__thiscall*)(void*, int);
        using PrintFn         = void(__thiscall*)(void*, const char*);
        using PrintfFn        = void(__cdecl*)(void*, const char*, ...);
        using ClearFn         = void(__thiscall*)(void*);
        using AppendFn        = void(__thiscall*)(void*, const G_FColor*, const char*);
        using ExecuteAlwaysFn = bool(__thiscall*)(void*);
        using DumpFn          = void(__thiscall*)(void*, bool);
        using CalcBoundsFn    = void(__thiscall*)(void*);
        using DuplicateFn     = void(__thiscall*)(void*, const void*);
        using CopyContentsFn  = void(__thiscall*)(void*, const void*);
        using IsInsideViewFn  = bool(__thiscall*)(void*, void*);

        inline SafetyHookInline g_hkCtorFont;
        inline SafetyHookInline g_hkCtorCopy;
        inline SafetyHookInline g_hkDtor;
        inline SafetyHookInline g_hkSetPos2a;
        inline SafetyHookInline g_hkSetPos2b;
        inline SafetyHookInline g_hkSetPosV3;
        inline SafetyHookInline g_hkSetColor;
        inline SafetyHookInline g_hkSetScale2;
        inline SafetyHookInline g_hkSetScale1;
        inline SafetyHookInline g_hkSetFont;
        inline SafetyHookInline g_hkSetZAngle;
        inline SafetyHookInline g_hkSetRefPoint;
        inline SafetyHookInline g_hkPrint;
        inline SafetyHookInline g_hkPrintf;
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
        inline SetPositionV3Fn g_origSetPosV3      = nullptr;
        inline SetColorFn      g_origSetColor      = nullptr;
        inline SetScale2Fn     g_origSetScale2     = nullptr;
        inline SetScale1Fn     g_origSetScale1     = nullptr;
        inline SetFontFn       g_origSetFont       = nullptr;
        inline SetZAngleFn     g_origSetZAngle     = nullptr;
        inline SetRefPointFn   g_origSetRefPoint   = nullptr;
        inline PrintFn         g_origPrint         = nullptr;
        inline PrintfFn        g_origPrintf        = nullptr;
        inline ClearFn         g_origClear         = nullptr;
        inline AppendFn        g_origAppend        = nullptr;
        inline ExecuteAlwaysFn g_origExecuteAlways = nullptr;
        inline DumpFn          g_origDump          = nullptr;
        inline CalcBoundsFn    g_origCalcBounds    = nullptr;
        inline DuplicateFn     g_origDuplicate     = nullptr;
        inline CopyContentsFn  g_origCopyContents  = nullptr;
        inline IsInsideViewFn  g_origIsInsideView  = nullptr;

        inline void SetPosition(void* t, float x, float y)              { g_origSetPos2a(t, x, y); }
        inline void SetPosition2(void* t, float x, float y)             { g_origSetPos2b(t, x, y); }
        inline void SetPositionV3(void* t, const M_Vector3& v)          { g_origSetPosV3(t, &v); }
        inline void SetColor(void* t, const G_FColor& c)                { g_origSetColor(t, &c); }
        inline void SetScale(void* t, float s)                          { g_origSetScale1(t, s); }
        inline void SetScale(void* t, float sx, float sy)               { g_origSetScale2(t, sx, sy); }
        inline void SetFont(void* t, void* f)                           { g_origSetFont(t, f); }
        inline void SetZAngle(void* t, float a)                         { g_origSetZAngle(t, a); }
        inline void SetRefPoint(void* t, int rp)                        { g_origSetRefPoint(t, rp); }
        inline void Print(void* t, const char* str)                     { g_origPrint(t, str); }

        inline void Printf(void* t, const char* fmt, ...)
        {
            if (!g_origPrintf || !t || !fmt) return;
            va_list ap;
            va_start(ap, fmt);
            char buf[1024];
            _vsnprintf_s(buf, _TRUNCATE, fmt, ap);
            va_end(ap);
            g_origPrintf(t, "%s", buf);
        }

        inline void Clear(void* t)                                      { g_origClear(t); }
        inline void Append(void* t, const G_FColor& c, const char* str) { g_origAppend(t, &c, str); }
        inline bool ExecuteAlways(void* t)                              { return g_origExecuteAlways(t); }
        inline void Dump(void* t, bool f)                               { g_origDump(t, f); }
        inline void Duplicate(void* t, const void* src)                 { g_origDuplicate(t, src); }
        inline void CopyContents(void* t, const void* src)              { g_origCopyContents(t, src); }
        inline bool IsInsideView(void* t, void* cam)                    { return g_origIsInsideView(t, cam); }
        inline void* GetLast()                                          { return g_lastText; }

        inline void __fastcall CtorFont_Hook(void* s, void*, void* f, int rp)
        {
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] ctorFont this=%p font=%p refPoint=%d", s, f, rp);
            g_origCtorFont(s, f, rp);
            if (f) P_Font::g_lastFont = f;
        }
        inline void __fastcall CtorCopy_Hook(void* s, void*, const void* o)
        {
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] ctorCopy this=%p src=%p", s, o);
            g_origCtorCopy(s, o);
        }
        inline void __fastcall Dtor_Hook(void* s, void*)
        {
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] dtor this=%p", s);
            g_origDtor(s);
        }
        inline void __fastcall SetPos2a_Hook(void* s, void*, float x, float y)
        {
#ifdef E2MFC_LOG_VERBOSE
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] setPos2a this=%p x=%.3f y=%.3f", s, x, y);
#endif
            g_origSetPos2a(s, x, y);
        }
        inline void __fastcall SetPos2b_Hook(void* s, void*, float x, float y)
        {
#ifdef E2MFC_LOG_VERBOSE
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] setPos2b this=%p x=%.3f y=%.3f", s, x, y);
#endif
            g_origSetPos2b(s, x, y);
        }
        inline void __fastcall SetPosV3_Hook(void* s, void*, const M_Vector3* v)
        {
#ifdef E2MFC_LOG_VERBOSE
            if (v) E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] setPosV3 this=%p x=%.3f y=%.3f z=%.3f",
                                       s, v->x, v->y, v->z);
            else   E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] setPosV3 this=%p null", s);
#endif
            g_origSetPosV3(s, v);
        }
        inline void __fastcall SetColor_Hook(void* s, void*, const G_FColor* c)
        {
#ifdef E2MFC_LOG_VERBOSE
            if (c) E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] setColor this=%p r=%.3f g=%.3f b=%.3f a=%.3f",
                                       s, c->r, c->g, c->b, c->a);
            else   E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] setColor this=%p null", s);
#endif
            g_origSetColor(s, c);
        }
        inline void __fastcall SetScale2_Hook(void* s, void*, float sx, float sy)
        {
#ifdef E2MFC_LOG_VERBOSE
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] setScale2 this=%p sx=%.3f sy=%.3f", s, sx, sy);
#endif
            g_origSetScale2(s, sx, sy);
        }
        inline void __fastcall SetScale1_Hook(void* s, void*, float sc)
        {
#ifdef E2MFC_LOG_VERBOSE
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] setScale1 this=%p sc=%.3f", s, sc);
#endif
            g_origSetScale1(s, sc);
        }
        inline void __fastcall SetFont_Hook(void* s, void*, void* f)
        {
#ifdef E2MFC_LOG_VERBOSE
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] setFont this=%p font=%p", s, f);
#endif
            if (f) P_Font::g_lastFont = f;
            g_origSetFont(s, f);
        }
        inline void __fastcall SetZAngle_Hook(void* s, void*, float a)
        {
#ifdef E2MFC_LOG_VERBOSE
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] setZAngle this=%p angle=%.3f", s, a);
#endif
            g_origSetZAngle(s, a);
        }
        inline void __fastcall SetRefPoint_Hook(void* s, void*, int rp)
        {
#ifdef E2MFC_LOG_VERBOSE
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] setRefPoint this=%p rp=%d", s, rp);
#endif
            g_origSetRefPoint(s, rp);
        }
        inline void __fastcall Print_Hook(void* s, void*, const char* t)
        {
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] print this=%p text=\"%s\"", s, t ? t : "(null)");
            if (s) g_lastText = s;
            ++g_printCount;
            g_origPrint(s, t);
        }
        inline void __cdecl Printf_Hook(void* s, const char* fmt, ...)
        {
            if (!g_origPrintf || !s || !fmt) return;

            char buf[1024];
            va_list ap;
            va_start(ap, fmt);
            _vsnprintf_s(buf, _TRUNCATE, fmt, ap);
            va_end(ap);
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] printf this=%p fmt=\"%s\" out=\"%s\"", s, fmt, buf);
            if (s) g_lastText = s;
            ++g_printfCount;
            g_origPrintf(s, "%s", buf);
        }
        inline void __fastcall Clear_Hook(void* s, void*)
        {
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] clear this=%p", s);
            g_origClear(s);
        }
        inline void __fastcall Append_Hook(void* s, void*, const G_FColor* c, const char* t)
        {
#ifdef E2MFC_LOG_VERBOSE
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] append this=%p text=\"%s\"", s, t ? t : "(null)");
#else
            (void)c;
#endif
            g_origAppend(s, c, t);
        }
        inline bool __fastcall ExecuteAlways_Hook(void* s, void*)
        {
            bool r = g_origExecuteAlways(s);
#ifdef E2MFC_LOG_VERBOSE
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] executeAlways this=%p r=%d", s, r ? 1 : 0);
#endif
            return r;
        }
        inline void __fastcall Dump_Hook(void* s, void*, bool f)
        {
#ifdef E2MFC_LOG_VERBOSE
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] dump this=%p flag=%d", s, f ? 1 : 0);
#endif
            g_origDump(s, f);
        }
        inline void __fastcall CalcBounds_Hook(void* s, void*)
        {
#ifdef E2MFC_LOG_VERBOSE
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] calcBounds this=%p", s);
#endif
            g_origCalcBounds(s);
        }
        inline void __fastcall Duplicate_Hook(void* s, void*, const void* o)
        {
#ifdef E2MFC_LOG_VERBOSE
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] duplicate this=%p src=%p", s, o);
#endif
            g_origDuplicate(s, o);
        }
        inline void __fastcall CopyContents_Hook(void* s, void*, const void* o)
        {
#ifdef E2MFC_LOG_VERBOSE
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] copyContents this=%p src=%p", s, o);
#endif
            g_origCopyContents(s, o);
        }
        inline bool __fastcall IsInsideView_Hook(void* s, void*, void* cam)
        {
            bool r = g_origIsInsideView(s, cam);
#ifdef E2MFC_LOG_VERBOSE
            E2MFC_LOG_TEXT_ONLY("[E2MFC][P_Text] isInsideView this=%p cam=%p r=%d", s, cam, r ? 1 : 0);
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
            E2MFC_LOG_SCENE_ONLY("[E2MFC][P_Scene] render this=%p cam=%p count=%d", s, cam, g_renderCount);
            g_origRender(s, cam);
        }
    }

    namespace P_BaseObject
    {
        inline void* g_lastContainer = nullptr;
        inline int g_addToCount = 0;
        inline int g_removeCount = 0;

        using AddToFn    = void(__thiscall*)(void*, void*);
        using RemoveFn   = void(__thiscall*)(void*);
        using SetPos3Fn  = void(__thiscall*)(void*, float, float, float);
        using SetPosV3Fn = void(__thiscall*)(void*, const M_Vector3*);
        using EnableFn   = void(__thiscall*)(void*, bool);

        inline SafetyHookInline g_hkAddTo;
        inline SafetyHookInline g_hkRemove;
        inline SafetyHookInline g_hkSetPos;
        inline SafetyHookInline g_hkSetPosV3;
        inline SafetyHookInline g_hkEnable;

        inline AddToFn    g_origAddTo    = nullptr;
        inline RemoveFn   g_origRemove   = nullptr;
        inline SetPos3Fn  g_origSetPos   = nullptr;
        inline SetPosV3Fn g_origSetPosV3 = nullptr;
        inline EnableFn   g_origEnable   = nullptr;

        inline void AddTo(void* o, void* c)                        { g_origAddTo(o, c); }
        inline void Remove(void* o)                                { g_origRemove(o); }
        inline void SetPosition(void* o, float x, float y, float z){ g_origSetPos(o, x, y, z); }
        inline void SetPositionV3(void* o, const M_Vector3& v)     { g_origSetPosV3(o, &v); }
        inline void Enable(void* o, bool e)                        { g_origEnable(o, e); }
        inline void* GetLastContainer()                            { return g_lastContainer; }

        inline void __fastcall AddTo_Hook(void* s, void*, void* c)
        {
            g_lastContainer = c;
            ++g_addToCount;
            E2MFC_LOG_BASEOBJECT_ONLY("[E2MFC][P_BaseObject] addTo this=%p container=%p count=%d",
                                      s, c, g_addToCount);
            g_origAddTo(s, c);
        }
        inline void __fastcall Remove_Hook(void* s, void*)
        {
            ++g_removeCount;
            E2MFC_LOG_BASEOBJECT_ONLY("[E2MFC][P_BaseObject] remove this=%p count=%d", s, g_removeCount);
            g_origRemove(s);
        }
        inline void __fastcall SetPos_Hook(void* s, void*, float x, float y, float z)
        {
            E2MFC_LOG_BASEOBJECT_ONLY("[E2MFC][P_BaseObject] setPosition this=%p x=%.3f y=%.3f z=%.3f",
                                      s, x, y, z);
            g_origSetPos(s, x, y, z);
        }
        inline void __fastcall SetPosV3_Hook(void* s, void*, const M_Vector3* v)
        {
#ifdef E2MFC_LOG_BASEOBJECT
            if (v) E2MFC_LOG_BASEOBJECT_ONLY("[E2MFC][P_BaseObject] setPositionV3 this=%p x=%.3f y=%.3f z=%.3f",
                                             s, v->x, v->y, v->z);
            else   E2MFC_LOG_BASEOBJECT_ONLY("[E2MFC][P_BaseObject] setPositionV3 this=%p null", s);
#endif
            g_origSetPosV3(s, v);
        }
        inline void __fastcall Enable_Hook(void* s, void*, bool e)
        {
            E2MFC_LOG_BASEOBJECT_ONLY("[E2MFC][P_BaseObject] enable this=%p v=%d", s, e ? 1 : 0);
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
            E2MFC_LOG_MAP_ONLY("[E2MFC][P_Map] validate this=%p", s);
            g_origValidate(s);
        }
    }

    namespace P_Material
    {
        using InitFn     = void(__thiscall*)(void*, uint32_t, uint32_t);
        using ValidateFn = void(__thiscall*)(void*);

        inline SafetyHookInline g_hkInit;
        inline SafetyHookInline g_hkValidate;
        inline InitFn     g_origInit     = nullptr;
        inline ValidateFn g_origValidate = nullptr;

        inline void __fastcall Init_Hook(void* s, void*, uint32_t a, uint32_t b)
        {
            E2MFC_LOG_MATERIAL_ONLY("[E2MFC][P_Material] init this=%p a=0x%08X b=0x%08X", s, a, b);
            g_origInit(s, a, b);
        }
        inline void __fastcall Validate_Hook(void* s, void*)
        {
            E2MFC_LOG_MATERIAL_ONLY("[E2MFC][P_Material] validate this=%p", s);
            g_origValidate(s);
        }
    }

    inline void Install()
    {
        HMODULE hMod = GetModuleHandleW(L"e2mfc.dll");
        if (!hMod) return;
        g_base = (uintptr_t)hMod;

        E2MFC::Log::F("[E2MFC] Install start (base=0x%p)", (void*)g_base);

        P_Font::g_hkP_Font_CtorCopy         = safetyhook::create_inline((void*)R(0x10001000), P_Font::P_Font_CtorCopy_Hook);
        P_Font::g_hkP_Font_CtorName         = safetyhook::create_inline((void*)R(0x100019E0), P_Font::P_Font_CtorName_Hook);
        P_Font::g_hkP_Font_CtorNamePath     = safetyhook::create_inline((void*)R(0x10001B20), P_Font::P_Font_CtorNamePath_Hook);
        P_Font::g_hkP_Font_Dtor             = safetyhook::create_inline((void*)R(0x10001D30), P_Font::P_Font_Dtor_Hook);
        P_Font::g_hkP_Font_GetViewportSize  = safetyhook::create_inline((void*)R(0x10001DC0), P_Font::P_Font_GetViewportSize_Hook);
        P_Font::g_hkP_Font_GetViewportSizef = safetyhook::create_inline((void*)R(0x10001E10), P_Font::P_Font_GetViewportSizef_Hook);
        P_Font::g_hkP_Font_Validate         = safetyhook::create_inline((void*)R(0x10001E60), P_Font::P_Font_Validate_Hook);

        P_Font::g_origP_Font_CtorCopy         = P_Font::g_hkP_Font_CtorCopy.original<P_Font::P_Font_CtorCopyFn>();
        P_Font::g_origP_Font_CtorName         = P_Font::g_hkP_Font_CtorName.original<P_Font::P_Font_CtorNameFn>();
        P_Font::g_origP_Font_CtorNamePath     = P_Font::g_hkP_Font_CtorNamePath.original<P_Font::P_Font_CtorNamePathFn>();
        P_Font::g_origP_Font_Dtor             = P_Font::g_hkP_Font_Dtor.original<P_Font::P_Font_DtorFn>();
        P_Font::g_origP_Font_GetViewportSize  = P_Font::g_hkP_Font_GetViewportSize.original<P_Font::P_Font_GetViewportSizeFn>();
        P_Font::g_origP_Font_GetViewportSizef = P_Font::g_hkP_Font_GetViewportSizef.original<P_Font::P_Font_GetViewportSizefFn>();
        P_Font::g_origP_Font_Validate         = P_Font::g_hkP_Font_Validate.original<P_Font::P_Font_ValidateFn>();

        P_Text::g_hkCtorFont      = safetyhook::create_inline((void*)R(0x100033F0), P_Text::CtorFont_Hook);
        P_Text::g_hkCtorCopy      = safetyhook::create_inline((void*)R(0x10003650), P_Text::CtorCopy_Hook);
        P_Text::g_hkDtor          = safetyhook::create_inline((void*)R(0x10003A20), P_Text::Dtor_Hook);
        P_Text::g_hkSetPos2a      = safetyhook::create_inline((void*)R(0x10002F70), P_Text::SetPos2a_Hook);
        P_Text::g_hkSetPos2b      = safetyhook::create_inline((void*)R(0x10002F90), P_Text::SetPos2b_Hook);
        P_Text::g_hkSetPosV3      = safetyhook::create_inline((void*)R(0x10001B10), P_Text::SetPosV3_Hook);
        P_Text::g_hkSetColor      = safetyhook::create_inline((void*)R(0x10002EF0), P_Text::SetColor_Hook);
        P_Text::g_hkSetScale2     = safetyhook::create_inline((void*)R(0x10002E80), P_Text::SetScale2_Hook);
        P_Text::g_hkSetScale1     = safetyhook::create_inline((void*)R(0x10002EA0), P_Text::SetScale1_Hook);
        P_Text::g_hkSetFont       = safetyhook::create_inline((void*)R(0x10002E50), P_Text::SetFont_Hook);
        P_Text::g_hkSetZAngle     = safetyhook::create_inline((void*)R(0x10002E30), P_Text::SetZAngle_Hook);
        P_Text::g_hkSetRefPoint   = safetyhook::create_inline((void*)R(0x100058C0), P_Text::SetRefPoint_Hook);
        P_Text::g_hkPrint         = safetyhook::create_inline((void*)R(0x10003AE0), P_Text::Print_Hook);
        P_Text::g_hkPrintf        = safetyhook::create_inline((void*)R(0x10003C80), P_Text::Printf_Hook);
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
        P_Text::g_origSetPosV3      = P_Text::g_hkSetPosV3.original<P_Text::SetPositionV3Fn>();
        P_Text::g_origSetColor      = P_Text::g_hkSetColor.original<P_Text::SetColorFn>();
        P_Text::g_origSetScale2     = P_Text::g_hkSetScale2.original<P_Text::SetScale2Fn>();
        P_Text::g_origSetScale1     = P_Text::g_hkSetScale1.original<P_Text::SetScale1Fn>();
        P_Text::g_origSetFont       = P_Text::g_hkSetFont.original<P_Text::SetFontFn>();
        P_Text::g_origSetZAngle     = P_Text::g_hkSetZAngle.original<P_Text::SetZAngleFn>();
        P_Text::g_origSetRefPoint   = P_Text::g_hkSetRefPoint.original<P_Text::SetRefPointFn>();
        P_Text::g_origPrint         = P_Text::g_hkPrint.original<P_Text::PrintFn>();
        P_Text::g_origPrintf        = P_Text::g_hkPrintf.original<P_Text::PrintfFn>();
        P_Text::g_origClear         = P_Text::g_hkClear.original<P_Text::ClearFn>();
        P_Text::g_origAppend        = P_Text::g_hkAppend.original<P_Text::AppendFn>();
        P_Text::g_origExecuteAlways = P_Text::g_hkExecuteAlways.original<P_Text::ExecuteAlwaysFn>();
        P_Text::g_origDump          = P_Text::g_hkDump.original<P_Text::DumpFn>();
        P_Text::g_origCalcBounds    = P_Text::g_hkCalcBounds.original<P_Text::CalcBoundsFn>();
        P_Text::g_origDuplicate     = P_Text::g_hkDuplicate.original<P_Text::DuplicateFn>();
        P_Text::g_origCopyContents  = P_Text::g_hkCopyContents.original<P_Text::CopyContentsFn>();
        P_Text::g_origIsInsideView  = P_Text::g_hkIsInsideView.original<P_Text::IsInsideViewFn>();

        P_Scene::g_hkRender   = safetyhook::create_inline((void*)R(0x10018EC0), P_Scene::Render_Hook);
        P_Scene::g_origRender = P_Scene::g_hkRender.original<P_Scene::RenderFn>();

        P_BaseObject::g_hkAddTo    = safetyhook::create_inline((void*)R(0x1001C6B0), P_BaseObject::AddTo_Hook);
        P_BaseObject::g_hkRemove   = safetyhook::create_inline((void*)R(0x1001C770), P_BaseObject::Remove_Hook);
        P_BaseObject::g_hkSetPos   = safetyhook::create_inline((void*)R(0x1001C550), P_BaseObject::SetPos_Hook);
        P_BaseObject::g_hkSetPosV3 = safetyhook::create_inline((void*)R(0x1001C520), P_BaseObject::SetPosV3_Hook);
        P_BaseObject::g_hkEnable   = safetyhook::create_inline((void*)R(0x1001C4D0), P_BaseObject::Enable_Hook);

        P_BaseObject::g_origAddTo    = P_BaseObject::g_hkAddTo.original<P_BaseObject::AddToFn>();
        P_BaseObject::g_origRemove   = P_BaseObject::g_hkRemove.original<P_BaseObject::RemoveFn>();
        P_BaseObject::g_origSetPos   = P_BaseObject::g_hkSetPos.original<P_BaseObject::SetPos3Fn>();
        P_BaseObject::g_origSetPosV3 = P_BaseObject::g_hkSetPosV3.original<P_BaseObject::SetPosV3Fn>();
        P_BaseObject::g_origEnable   = P_BaseObject::g_hkEnable.original<P_BaseObject::EnableFn>();

        P_Map::g_hkValidate   = safetyhook::create_inline((void*)R(0x1001FED0), P_Map::Validate_Hook);
        P_Map::g_origValidate = P_Map::g_hkValidate.original<P_Map::ValidateFn>();

        P_Material::g_hkInit     = safetyhook::create_inline((void*)R(0x10023E10), P_Material::Init_Hook);
        P_Material::g_hkValidate = safetyhook::create_inline((void*)R(0x10024780), P_Material::Validate_Hook);
        P_Material::g_origInit     = P_Material::g_hkInit.original<P_Material::InitFn>();
        P_Material::g_origValidate = P_Material::g_hkValidate.original<P_Material::ValidateFn>();

        E2MFC::Log::F("[E2MFC] Install done");
    }
}