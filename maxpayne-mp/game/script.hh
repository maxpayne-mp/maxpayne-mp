#pragma once

#include <cstdarg>
#include <cstdio>
#include "lib/utils.h"

#include "console.hh"

namespace SCRIPT
{
    namespace ADDRESS
    {
        constexpr const auto addrScriptRun = 0x51E7C0;
    }

    //      Functions 
    // --------------------------------------------------------------

    // TODO: 
    inline char* Run(const char* format)
    {
        return CONSOLE::Console_Mother(format, 0x0);
    }
}