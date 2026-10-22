#pragma once
#include <windows.h>

class MouseBlocker {
private:
    static HHOOK hMouseHook;
    static bool isBlocked;

public:
    static bool Initialize();
    static void BlockClicks();
    static void UnblockClicks();
    static void Cleanup();

private:
    static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
};