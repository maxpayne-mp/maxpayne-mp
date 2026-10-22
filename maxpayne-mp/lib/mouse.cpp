#include "mouse.h"
#include <iostream>

HHOOK MouseBlocker::hMouseHook = NULL;
bool MouseBlocker::isBlocked = false;

LRESULT CALLBACK MouseBlocker::MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && isBlocked) {
        if (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP ||
            wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONUP ||
            wParam == WM_MBUTTONDOWN || wParam == WM_MBUTTONUP ||
            wParam == WM_MOUSEWHEEL) {
            return 1;
        }
    }
    return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

bool MouseBlocker::Initialize() {
    hMouseHook = SetWindowsHookEx(
        WH_MOUSE_LL,        
        MouseProc,
        GetModuleHandle(NULL),
        0
    );

    if (hMouseHook == NULL) {
        std::cerr << "Falha ao instalar hook! Erro: " << GetLastError() << std::endl;
        return false;
    }

    isBlocked = false;
    return true;
}

void MouseBlocker::BlockClicks() {
    isBlocked = true;
}

void MouseBlocker::UnblockClicks() {
    isBlocked = false;
}

void MouseBlocker::Cleanup() {
    if (hMouseHook) {
        
        UnhookWindowsHookEx(hMouseHook);
        hMouseHook = NULL;
    }
}