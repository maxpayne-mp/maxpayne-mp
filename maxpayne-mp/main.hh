#pragma once

#define PROJECT_NAME "maxpayne-mp"
#define PROJECT_VERSION "0.0.1"

#define VALIDATE_VERSION_TESTS // se definida, valida algumas coisas do jogo, que podem impedir o mod de funcionar corretamente, como versão do jogo, checksum e build.

#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h>

#include <cstdio>
#include <cstring>
#include <stdio.h>
#include <vector>
#include <string>
#include <cstdarg>

#include <tlhelp32.h>

#include "lib/safetyhook.hpp"

// script
#include <map>
#include <functional>

#include <thread>
#include <chrono>

#include <enet/enet.h>