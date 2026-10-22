#pragma once

#define PROJECT_NAME "maxpayne-mp"
#define PROJECT_VERSION "0.0.1"
#define PROJECT_REPORT_CRASH "google.com"

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