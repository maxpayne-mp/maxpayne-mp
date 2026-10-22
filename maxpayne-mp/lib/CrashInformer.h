#pragma once

#include <sstream>
#include <ctime>
#include <dbghelp.h>
#include <psapi.h>
#include <shellapi.h>

#include "../main.hh"

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shell32.lib")

class CrashInformer {
private:
    static inline bool bInitialized = false;
    static inline std::string strLastCrashInfo = "";
    static inline HCURSOR hDefaultCursor = nullptr;

    static std::string GetExceptionName(DWORD code) {
        switch (code) {
        case EXCEPTION_ACCESS_VIOLATION: return "ACCESS_VIOLATION";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "ARRAY_BOUNDS_EXCEEDED";
        case EXCEPTION_BREAKPOINT: return "BREAKPOINT";
        case EXCEPTION_DATATYPE_MISALIGNMENT: return "DATATYPE_MISALIGNMENT";
        case EXCEPTION_FLT_DENORMAL_OPERAND: return "FLT_DENORMAL_OPERAND";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "FLT_DIVIDE_BY_ZERO";
        case EXCEPTION_FLT_INEXACT_RESULT: return "FLT_INEXACT_RESULT";
        case EXCEPTION_FLT_INVALID_OPERATION: return "FLT_INVALID_OPERATION";
        case EXCEPTION_FLT_OVERFLOW: return "FLT_OVERFLOW";
        case EXCEPTION_FLT_STACK_CHECK: return "FLT_STACK_CHECK";
        case EXCEPTION_FLT_UNDERFLOW: return "FLT_UNDERFLOW";
        case EXCEPTION_ILLEGAL_INSTRUCTION: return "ILLEGAL_INSTRUCTION";
        case EXCEPTION_IN_PAGE_ERROR: return "IN_PAGE_ERROR";
        case EXCEPTION_INT_DIVIDE_BY_ZERO: return "INT_DIVIDE_BY_ZERO";
        case EXCEPTION_INT_OVERFLOW: return "INT_OVERFLOW";
        case EXCEPTION_INVALID_DISPOSITION: return "INVALID_DISPOSITION";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "NONCONTINUABLE_EXCEPTION";
        case EXCEPTION_PRIV_INSTRUCTION: return "PRIV_INSTRUCTION";
        case EXCEPTION_SINGLE_STEP: return "SINGLE_STEP";
        case EXCEPTION_STACK_OVERFLOW: return "STACK_OVERFLOW";
        default: return "UNKNOWN";
        }
    }

    static std::string GetModuleName(DWORD address) {
        HMODULE hMods[1024];
        DWORD cbNeeded;
        char szModName[MAX_PATH];
        HANDLE hProc = GetCurrentProcess();

        if (EnumProcessModules(hProc, hMods, sizeof(hMods), &cbNeeded)) {
            for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                MODULEINFO modInfo;
                if (GetModuleInformation(hProc, hMods[i], &modInfo, sizeof(modInfo))) {
                    DWORD base = (DWORD)modInfo.lpBaseOfDll;
                    DWORD size = (DWORD)modInfo.SizeOfImage;

                    if (address >= base && address < (base + size)) {
                        GetModuleFileNameExA(hProc, hMods[i], szModName, sizeof(szModName));
                        char* fileName = strrchr(szModName, '\\');
                        if (fileName) fileName++;
                        else fileName = szModName;

                        static char result[256];
                        sprintf_s(result, "%s+0x%X", fileName, address - base);
                        return std::string(result);
                    }
                }
            }
        }
        return "Unknown";
    }

    static std::string GetCallStack(CONTEXT* ctx) {
        std::stringstream ss;
        ss << "CALL STACK:\r\n";
        ss << "----------------------------------------\r\n";

        HANDLE hProc = GetCurrentProcess();
        SymInitialize(hProc, NULL, TRUE);

        STACKFRAME64 stackFrame = {};
        stackFrame.AddrPC.Offset = ctx->Eip;
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrStack.Offset = ctx->Esp;
        stackFrame.AddrStack.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Offset = ctx->Ebp;
        stackFrame.AddrFrame.Mode = AddrModeFlat;

        char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;

        DWORD64 displacement = 0;
        DWORD frameNum = 0;

        while (StackWalk64(IMAGE_FILE_MACHINE_I386, hProc, GetCurrentThread(),
            &stackFrame, ctx, NULL, SymFunctionTableAccess64,
            SymGetModuleBase64, NULL)) {

            if (frameNum > 0 && stackFrame.AddrPC.Offset == 0) break;
            if (frameNum > 30) break;

            DWORD pc = (DWORD)stackFrame.AddrPC.Offset;

            ss << "[" << std::dec << frameNum << "] 0x" << std::hex << pc;

            if (SymFromAddr(hProc, stackFrame.AddrPC.Offset, &displacement, pSymbol)) {
                ss << " " << GetModuleName(pc) << " - " << pSymbol->Name;
                if (displacement > 0) {
                    ss << "+0x" << std::hex << displacement;
                }
            }
            else {
                ss << " " << GetModuleName(pc);
            }

            ss << "\r\n";
            frameNum++;
        }

        SymCleanup(hProc);
        return ss.str();
    }

    static std::string GetRegistersString(CONTEXT* ctx) {
        std::stringstream ss;
        ss << "REGISTERS:\r\n";
        ss << "----------------------------------------\r\n";
        ss << "EAX: 0x" << std::hex << ctx->Eax << "  EBX: 0x" << std::hex << ctx->Ebx << "\r\n";
        ss << "ECX: 0x" << std::hex << ctx->Ecx << "  EDX: 0x" << std::hex << ctx->Edx << "\r\n";
        ss << "ESI: 0x" << std::hex << ctx->Esi << "  EDI: 0x" << std::hex << ctx->Edi << "\r\n";
        ss << "EBP: 0x" << std::hex << ctx->Ebp << "  ESP: 0x" << std::hex << ctx->Esp << "\r\n";
        ss << "EIP: 0x" << std::hex << ctx->Eip << "\r\n";
        return ss.str();
    }

    static std::string GetStackDumpString(DWORD esp, int count = 16) {
        std::stringstream ss;
        ss << "STACK DUMP:\r\n";
        ss << "----------------------------------------\r\n";

        DWORD* pStack = (DWORD*)esp;
        for (int i = 0; i < count; i++) {
            DWORD addr = (DWORD)&pStack[i];

            if (IsBadReadPtr(&pStack[i], 4) == FALSE) {
                ss << "0x" << std::hex << addr << " | 0x" << std::hex << pStack[i];

                if (pStack[i] >= 0x00400000 && pStack[i] <= 0x01000000) {
                    ss << " (CODE)";
                }
                ss << "\r\n";
            }
            else {
                ss << "0x" << std::hex << addr << " | INVALID\r\n";
            }
        }
        return ss.str();
    }

    static std::string GetTimeString() {
        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);
        char buffer[64];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return std::string(buffer);
    }

    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_SETCURSOR:
            SetCursor(hDefaultCursor);
            return TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) {
                DestroyWindow(hWnd);
            }
            if (LOWORD(wParam) == 2) {
                if (OpenClipboard(hWnd)) {
                    EmptyClipboard();
                    std::string text = strLastCrashInfo;
                    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, text.length() + 1);
                    if (hGlobal) {
                        char* pData = (char*)GlobalLock(hGlobal);
                        if (pData) {
                            memcpy(pData, text.c_str(), text.length() + 1);
                            GlobalUnlock(hGlobal);
                            SetClipboardData(CF_TEXT, hGlobal);
                        }
                    }
                    CloseClipboard();
                    MessageBoxA(hWnd, "Copied to clipboard!", "Scarface-MP", MB_OK);
                }
            }
            if (LOWORD(wParam) == 3) {
                ShellExecuteA(NULL, "open", PROJECT_REPORT_CRASH, NULL, NULL, SW_SHOW);
            }
            break;

        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
        return 0;
    }

    static void ShowCrashWindow(const std::string& title, const std::string& message) {
        hDefaultCursor = LoadCursor(NULL, IDC_ARROW);

        WNDCLASSEXA wc = {};
        wc.cbSize = sizeof(WNDCLASSEXA);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = "CrashInformerClass";

        RegisterClassExA(&wc);

        HWND hWnd = CreateWindowExA(
            WS_EX_TOPMOST,
            "CrashInformerClass",
            title.c_str(),
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT,
            750, 550,
            nullptr, nullptr,
            GetModuleHandle(nullptr),
            nullptr
        );

        if (!hWnd) {
            MessageBoxA(nullptr, message.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
            return;
        }

        HFONT hFont = CreateFontA(
            16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas"
        );

        HWND hEdit = CreateWindowExA(
            WS_EX_CLIENTEDGE,
            "EDIT",
            message.c_str(),
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY |
            ES_AUTOVSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | WS_HSCROLL,
            10, 10, 710, 430,
            hWnd, nullptr, GetModuleHandle(nullptr), nullptr
        );
        SendMessageA(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

        HWND hBtnCopy = CreateWindowA(
            "BUTTON", "Copy",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            10, 455, 120, 35,
            hWnd, (HMENU)2, GetModuleHandle(nullptr), nullptr
        );
        SendMessageA(hBtnCopy, WM_SETFONT, (WPARAM)hFont, TRUE);

        HWND hBtnReport = CreateWindowA(
            "BUTTON", "Report Crash",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            140, 455, 120, 35,
            hWnd, (HMENU)3, GetModuleHandle(nullptr), nullptr
        );
        SendMessageA(hBtnReport, WM_SETFONT, (WPARAM)hFont, TRUE);

        HWND hBtnClose = CreateWindowA(
            "BUTTON", "Close",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            600, 455, 120, 35,
            hWnd, (HMENU)1, GetModuleHandle(nullptr), nullptr
        );
        SendMessageA(hBtnClose, WM_SETFONT, (WPARAM)hFont, TRUE);

        ShowWindow(hWnd, SW_SHOW);
        UpdateWindow(hWnd);
        SetForegroundWindow(hWnd);
        BringWindowToTop(hWnd);
        SetFocus(hWnd);

        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        DeleteObject(hFont);
    }

public:
    static void Initialize() {
        if (bInitialized) return;

        SymInitialize(GetCurrentProcess(), NULL, TRUE);
        SetUnhandledExceptionFilter(UnhandledExceptionFilter);
        bInitialized = true;
    }

    static LONG WINAPI UnhandledExceptionFilter(EXCEPTION_POINTERS* pExceptionInfo) {
        CONTEXT* ctx = pExceptionInfo->ContextRecord;
        EXCEPTION_RECORD* exc = pExceptionInfo->ExceptionRecord;

        std::stringstream info;

        info << "========================================\r\n";
        info << "          CRASH DETECTED!\r\n";
        info << "========================================\r\n\r\n";

        info << "[TIME] " << GetTimeString() << "\r\n\r\n";

        info << "EXCEPTION:\r\n";
        info << "----------------------------------------\r\n";
        info << "Code: 0x" << std::hex << exc->ExceptionCode << "\r\n";
        info << "Name: " << GetExceptionName(exc->ExceptionCode) << "\r\n";
        info << "Address: 0x" << std::hex << (DWORD)exc->ExceptionAddress << "\r\n";
        info << "Module: " << GetModuleName((DWORD)exc->ExceptionAddress) << "\r\n";

        if (exc->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
            info << "Type: " << (exc->ExceptionInformation[0] == 0 ? "Read" : "Write") << "\r\n";
            info << "Address: 0x" << std::hex << exc->ExceptionInformation[1] << "\r\n";
        }
        info << "\r\n";

        info << GetRegistersString(ctx) << "\r\n";
        info << GetCallStack(ctx) << "\r\n";
        info << GetStackDumpString(ctx->Esp) << "\r\n";

        info << "\r\n========================================\r\n";
        info << "Crash report saved to: crash_report.txt\r\n";
        info << "========================================\r\n";

        strLastCrashInfo = info.str();

        FILE* f = fopen("scarface-mp/crash_report.txt", "w");
        if (f) {
            fwrite(strLastCrashInfo.c_str(), 1, strLastCrashInfo.length(), f);
            fclose(f);
        }

        ShowCrashWindow(" ", strLastCrashInfo);

        return EXCEPTION_EXECUTE_HANDLER;
    }

    static void ShowError(const char* format, ...) {
        char buffer[2048];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        std::string errorMsg = buffer;
        std::string fullMsg = "========================================\r\n";
        fullMsg += "          ERROR!\r\n";
        fullMsg += "========================================\r\n\r\n";
        fullMsg += "[TIME] " + CrashInformer::GetTimeString() + "\r\n\r\n";
        fullMsg += errorMsg + "\r\n\r\n";
        fullMsg += "========================================\r\n";

        CrashInformer::strLastCrashInfo = fullMsg;

        FILE* f = fopen("scarface-mp/error_log.txt", "a");
        if (f) {
            fwrite(fullMsg.c_str(), 1, fullMsg.length(), f);
            fclose(f);
        }

        CrashInformer::ShowCrashWindow(" Error", fullMsg);
    }
};