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
namespace P_Font
{
    namespace ADDRESS
    {
        // constexpr const auto addrAddReceiver    = 0x478D00;
    }

    
        P_Font
    _DWORD __thiscall P_Font::P_Font(P_Font *this, const struct P_Font *) -> 0x10001000
    _DWORD __thiscall P_Font::P_Font(P_Font *this, const char *) -> 0x100019E0
     _DWORD __thiscall P_Font::P_Font(P_Font *this, const char *, const char *) -> 0x10001B20
      _DWORD __thiscall P_Font::~P_Font(P_Font *__hidden this) -> 0x10001D30
      public: class M_Vector2Template<float> __thiscall P_Font::getViewportSize(char const *)const -> 0x10001DC0
      int __cdecl P_Font::getViewportSizef(int, int, char *Format, char ArgList) -> 0x10001E10
      void __thiscall P_Font::validate(P_Font *__hidden this) -> 0x10001E60
      
    
}
}