#pragma once

#include <windows.h>
#include <cstdint>
#include <cstring>
#include "lib/utils.h"
#include "game/script.hh"

namespace TEXT
{
    namespace ADDRESS
    {
    }

    inline void DrawTextRight(const char* text)
    {
        if (!text) return;
        char buf[256];
        snprintf(buf, sizeof(buf), "maxpayne_hudmode->mphm_printdirect( %s );", text);
        SCRIPT::Execute(buf);
    }

    inline void Install()
    {
        OutputDebugString("TEXT::Install()\n");
    }
}
