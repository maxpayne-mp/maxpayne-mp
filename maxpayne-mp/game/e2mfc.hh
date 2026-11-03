#pragma once

#include <vector>
#include <string>
#include "lib/utils.h"
#include "console.hh"
#include "lib/safetyhook.hpp"

#include <wincrypt.h>
#pragma comment(lib, "advapi32.lib")

namespace e2mfc
{
    #ifdef e2mfc_R
        #undef e2mfc_R
    #endif

    #define e2mfc_R(dumpAddr)  \
        (g_base + ((dumpAddr) - addrImageBase)) 

    namespace ADDRESS
    {
        constexpr const auto addrImageBase    = 0x10000000;


        P_Text
    private: virtual void __thiscall P_Text::setPosition(class M_Vector3Template<float> const &) -> 0x10001B10
    private: virtual class M_Vector3Template<float> & __thiscall P_Text::accessPosition(void) -> 0x10002E10
    const char *__thiscall P_Text::getString(P_Text *__hidden this) -> 0x10002E20
    void __thiscall P_Text::setZAngle(P_Text *this, float) -> 0x10002E30
    float __thiscall P_Text::getZAngle(P_Text *__hidden this) -> 0x10002E40
    void __thiscall P_Text::setFont(P_Text *this, struct P_Font *) -> 0x10002E50
    struct P_Font *__thiscall P_Text::getFont(P_Text *__hidden this) -> 0x10002E70
    void __thiscall P_Text::setScale(P_Text *this, float, float) -> 0x10002E80
    void __thiscall P_Text::setScale(P_Text *this, float) -> 0x10002EA0
    public: void __thiscall P_Text::setScale(class M_Vector2Template<float> const &) -> 0x10002EC0
    public: class M_Vector2Template<float> const & __thiscall P_Text::getScale(void)const -> 0x10002EE0
    void __thiscall P_Text::setColor(P_Text *this, const struct G_FColor *) -> 0x10002EF0
    const struct G_FColor *__thiscall P_Text::getColor(P_Text *__hidden this) -> 0x10002F60
    void __thiscall P_Text::setPosition(P_Text *this, float, float) -> 0x10002F70
    void __thiscall P_Text::setPosition(P_Text *this, float, float) -> 0x10002F90
    public: class M_Vector2Template<float> const & __thiscall P_Text::getPosition(void) -> 0x10002FB0
    public: enum  P_Text::ReferencePoint __thiscall P_Text::getReferencePoint(void)const -> 0x10002FC0
    public: __thiscall P_Text::P_Text(class P_Font *, enum  P_Text::ReferencePoint) -> 0x100033F0
    _DWORD __thiscall P_Text::P_Text(P_Text *this, const struct P_Text *) -> 0x10003650
    void __thiscall P_Text::duplicate(P_Text *this, const struct P_Text *) -> 0x10003710
    void __thiscall P_Text::copyContents(P_Text *this, const struct P_Text *) -> 0x10003740
    _DWORD __thiscall P_Text::~P_Text(P_Text *this) -> 0x10003A20
    const P_Text::`vftable' - > .rdata:10049560 ; const P_Text::`vftable'
                                .rdata:10049560 ??_7P_Text@@6B@_0 dd offset sub_10003630
    const char *__thiscall P_Text::getObjectType(P_Text *__hidden this) -> 0x10003AC0
    void __thiscall P_Text::calculateBoundingVolumes(P_Text *__hidden this) -> 0x10003AD0
    void __thiscall P_Text::print(P_Text *this, const char *) -> 0x10003AE0
    void __thiscall P_Text::clear(P_Text *__hidden this) -> 0x10003CB0
    void __thiscall P_Text::append(P_Text *this, const struct G_Color *, const char *) -> 0x10003E40
    public: class M_Vector2Template<float> __thiscall P_Text::getViewportSize(void)const -> 0x100040C0
    public: class M_Vector2Template<float> __thiscall P_Text::getViewportSize(char const *)const -> 0x10004100
    public: class M_Vector2Template<float> __cdecl P_Text::getViewportSizef(char const *, ...)const -> 0x100041C0
    bool __thiscall P_Text::executeAlways(P_Text *this) -> 0x10004280
    void __thiscall P_Text::dump(P_Text *this, bool) -> 0x10004E60
    public: void __thiscall P_Text::setReferencePoint(enum  P_Text::ReferencePoint) -> 0x100058C0
    private: virtual class M_Vector3Template<float> const & __thiscall P_Text::getPosition(void)const -> 0x10007AF0
    bool __thiscall P_Text::isInsideViewVolume(P_Text *this, struct P_Camera *) -> 0x10007E40
    }

    inline uintptr_t g_base = 0;

    inline void Install()
    {
        using namespace ADDRESS;

        HMODULE hMod = GetModuleHandleW("e2mfc.dll");
        if(hMod == NULL) {
            OutputDebugString("Failed to get module handle for e2mfc.dll\n");
            return;
        }

        g_base = (uintptr_t)hMod;

        OutputDebugString("e2mfc::Install()\n");
    }

    #undef e2mfc_R

}