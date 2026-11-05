#pragma once

#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <intrin.h>
#include <Windows.h>
#include "lib/utils.h"
#include "lib/safetyhook.hpp"

#define ENEMY_LOG(...) printf(__VA_ARGS__); printf("\n")

namespace ENEMY
{
    namespace ADDRESS
    {
    }

    inline void Install()
    {
        ENEMY_LOG("ENEMY::Install() ");
    }
}