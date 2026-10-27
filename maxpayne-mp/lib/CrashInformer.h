#pragma once

#ifndef _M_IX86
#error CrashInformer requires a 32-bit x86 build
#endif

#include <windows.h>
#include <tlhelp32.h>
#include <dbghelp.h>
#include <psapi.h>
#include <shellapi.h>
#include <intrin.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <atomic>
#include <exception>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "../main.hh"

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shell32.lib")

class CrashInformer {
private:
    enum : DWORD {
        CRASH_CPP_TERMINATE = 0xE0000001,
        CRASH_ABORT = 0xE0000002,
        CRASH_PURE_CALL = 0xE0000003,
        CRASH_INVALID_PARAMETER = 0xE0000004,
        CRASH_CPP_EXCEPTION = 0xE06D7363
    };

    enum : int {
        ID_CLOSE = 1,
        ID_COPY = 2,
        ID_REPORT = 3,
        ID_EDIT = 4
    };

    struct CrashJob {
        EXCEPTION_POINTERS* pointers;
        DWORD faultThreadId;
        HANDLE faultThread;
        HANDLE jobObject;
    };

    static inline std::atomic<bool> bInitialized{ false };
    static inline std::atomic<bool> bHandling{ false };
    static inline std::atomic<DWORD> dwHandlerThreadId{ 0 };
    static inline std::string strLastCrashInfo;
    static inline HCURSOR hDefaultCursor = nullptr;
    static inline const char* kLogDir = "maxpayne-mp";
    static inline const char* kAppName = "MaxPayne-MP";
    static inline const char* kReportUrl = "https://example.com/report";
    static inline bool bCatchFirstChance = true;
    static inline HBRUSH hBgBrush = nullptr;
    static inline HBRUSH hPanelBrush = nullptr;
    static inline HBRUSH hBorderBrush = nullptr;

    static constexpr COLORREF kColorBg = RGB(16, 16, 16);
    static constexpr COLORREF kColorPanel = RGB(28, 8, 8);
    static constexpr COLORREF kColorLight = RGB(240, 210, 200);
    static constexpr COLORREF kColorText = RGB(255, 80, 60);
    static constexpr COLORREF kColorBorder = RGB(90, 10, 10);
    static constexpr COLORREF kColorButton = RGB(70, 14, 14);
    static constexpr COLORREF kColorHover = RGB(120, 22, 22);
    static constexpr COLORREF kColorPressed = RGB(40, 6, 6);

    static std::string Hex(uintptr_t value, int width = 8) {
        std::stringstream ss;
        ss << "0x" << std::uppercase << std::hex << std::setw(width) << std::setfill('0') << value;
        return ss.str();
    }

    static std::string GetTimeString(const char* format = "%Y-%m-%d %H:%M:%S") {
        time_t now = time(nullptr);
        struct tm timeinfo = {};
        localtime_s(&timeinfo, &now);
        char buffer[64] = {};
        strftime(buffer, sizeof(buffer), format, &timeinfo);
        return buffer;
    }

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
        case CRASH_CPP_TERMINATE: return "CPP_TERMINATE";
        case CRASH_ABORT: return "ABORT";
        case CRASH_PURE_CALL: return "PURE_VIRTUAL_CALL";
        case CRASH_INVALID_PARAMETER: return "INVALID_PARAMETER";
        case CRASH_CPP_EXCEPTION: return "CPP_EXCEPTION";
        default: return "UNKNOWN";
        }
    }

    static std::string GetModuleName(uintptr_t address) {
        HMODULE modules[1024];
        DWORD needed = 0;
        HANDLE process = GetCurrentProcess();

        if (!EnumProcessModules(process, modules, sizeof(modules), &needed))
            return "Unknown";

        size_t count = needed / sizeof(HMODULE);
        if (count > 1024) count = 1024;

        for (size_t i = 0; i < count; i++) {
            MODULEINFO info = {};
            if (!GetModuleInformation(process, modules[i], &info, sizeof(info)))
                continue;

            uintptr_t base = (uintptr_t)info.lpBaseOfDll;
            uintptr_t size = (uintptr_t)info.SizeOfImage;
            if (address < base || address >= base + size)
                continue;

            char path[MAX_PATH] = {};
            GetModuleFileNameExA(process, modules[i], path, MAX_PATH);
            const char* name = strrchr(path, '\\');
            name = name ? name + 1 : path;
            return std::string(name) + "+" + Hex(address - base, 1);
        }
        return "Unknown";
    }

    static bool SafeRead(const void* address, void* out, SIZE_T size) {
        SIZE_T read = 0;
        return ReadProcessMemory(GetCurrentProcess(), address, out, size, &read) && read == size;
    }

    static std::string GetCallStack(HANDLE thread, CONTEXT context) {
        std::stringstream ss;
        ss << "CALL STACK:\r\n----------------------------------------\r\n";

        HANDLE process = GetCurrentProcess();

        STACKFRAME64 frame = {};
        frame.AddrPC.Offset = context.Eip;
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrStack.Offset = context.Esp;
        frame.AddrStack.Mode = AddrModeFlat;
        frame.AddrFrame.Offset = context.Ebp;
        frame.AddrFrame.Mode = AddrModeFlat;

        alignas(SYMBOL_INFO) char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)symbolBuffer;

        for (int index = 0; index < 48; index++) {
            if (!StackWalk64(IMAGE_FILE_MACHINE_I386, process, thread, &frame, &context, nullptr,
                SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
                break;

            if (frame.AddrPC.Offset == 0)
                break;

            ss << "[" << std::dec << index << "] " << Hex((uintptr_t)frame.AddrPC.Offset) << " "
                << GetModuleName((uintptr_t)frame.AddrPC.Offset);

            memset(symbolBuffer, 0, sizeof(symbolBuffer));
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            symbol->MaxNameLen = MAX_SYM_NAME;
            DWORD64 displacement = 0;

            if (SymFromAddr(process, frame.AddrPC.Offset, &displacement, symbol)) {
                ss << " - " << symbol->Name;
                if (displacement)
                    ss << "+" << Hex((uintptr_t)displacement, 1);
            }

            DWORD lineDisplacement = 0;
            IMAGEHLP_LINE64 line = {};
            line.SizeOfStruct = sizeof(line);
            if (SymGetLineFromAddr64(process, frame.AddrPC.Offset, &lineDisplacement, &line))
                ss << " (" << line.FileName << ":" << std::dec << line.LineNumber << ")";

            ss << "\r\n";
        }
        return ss.str();
    }

    static std::string GetRegistersString(const CONTEXT* c) {
        std::stringstream ss;
        ss << "REGISTERS:\r\n----------------------------------------\r\n";
        ss << "EAX: " << Hex(c->Eax) << "  EBX: " << Hex(c->Ebx) << "\r\n";
        ss << "ECX: " << Hex(c->Ecx) << "  EDX: " << Hex(c->Edx) << "\r\n";
        ss << "ESI: " << Hex(c->Esi) << "  EDI: " << Hex(c->Edi) << "\r\n";
        ss << "EBP: " << Hex(c->Ebp) << "  ESP: " << Hex(c->Esp) << "\r\n";
        ss << "EIP: " << Hex(c->Eip) << "  EFLAGS: " << Hex(c->EFlags) << "\r\n";
        return ss.str();
    }

    static std::string GetStackDumpString(uintptr_t esp, int count = 24) {
        std::stringstream ss;
        ss << "STACK DUMP:\r\n----------------------------------------\r\n";

        for (int i = 0; i < count; i++) {
            uintptr_t address = esp + i * sizeof(DWORD);
            DWORD value = 0;

            if (!SafeRead((const void*)address, &value, sizeof(value))) {
                ss << Hex(address) << " | INVALID\r\n";
                continue;
            }

            ss << Hex(address) << " | " << Hex(value);
            std::string module = GetModuleName(value);
            if (module != "Unknown")
                ss << " (" << module << ")";
            ss << "\r\n";
        }
        return ss.str();
    }

    static std::string GetModulesString() {
        std::stringstream ss;
        ss << "LOADED MODULES:\r\n----------------------------------------\r\n";

        HMODULE modules[1024];
        DWORD needed = 0;
        HANDLE process = GetCurrentProcess();

        if (EnumProcessModules(process, modules, sizeof(modules), &needed)) {
            size_t count = needed / sizeof(HMODULE);
            if (count > 1024) count = 1024;

            for (size_t i = 0; i < count; i++) {
                MODULEINFO info = {};
                char path[MAX_PATH] = {};
                if (GetModuleInformation(process, modules[i], &info, sizeof(info)) &&
                    GetModuleFileNameExA(process, modules[i], path, MAX_PATH)) {
                    const char* name = strrchr(path, '\\');
                    ss << Hex((uintptr_t)info.lpBaseOfDll) << " " << (name ? name + 1 : path) << "\r\n";
                }
            }
        }
        return ss.str();
    }

    static void EnsureLogDirectory() {
        CreateDirectoryA(kLogDir, nullptr);
    }

    static void WriteTextFile(const std::string& path, const std::string& text, const char* mode) {
        FILE* file = nullptr;
        if (fopen_s(&file, path.c_str(), mode) == 0 && file) {
            fwrite(text.data(), 1, text.size(), file);
            fclose(file);
        }
    }

    static std::string WriteMiniDump(EXCEPTION_POINTERS* pointers, DWORD faultThreadId, const std::string& stamp) {
        std::string path = std::string(kLogDir) + "/crash_" + stamp + ".dmp";

        HANDLE file = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE)
            return "";

        MINIDUMP_EXCEPTION_INFORMATION exceptionInfo = {};
        exceptionInfo.ThreadId = faultThreadId;
        exceptionInfo.ExceptionPointers = pointers;
        exceptionInfo.ClientPointers = FALSE;

        BOOL ok = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file,
            (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithDataSegs | MiniDumpWithThreadInfo),
            pointers ? &exceptionInfo : nullptr, nullptr, nullptr);

        CloseHandle(file);
        if (!ok) {
            DeleteFileA(path.c_str());
            return "";
        }
        return path;
    }

    typedef LONG(NTAPI* NtSuspendProcessFn)(HANDLE);

    static NtSuspendProcessFn GetNtSuspendProcess() {
        static NtSuspendProcessFn fn = nullptr;
        if (!fn) {
            HMODULE ntdll = GetModuleHandleA("ntdll.dll");
            if (ntdll)
                fn = (NtSuspendProcessFn)GetProcAddress(ntdll, "NtSuspendProcess");
        }
        return fn;
    }

    static void SuspendAllThreadsHard(DWORD keepThreadId) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snapshot != INVALID_HANDLE_VALUE) {
            THREADENTRY32 entry = {};
            entry.dwSize = sizeof(entry);
            DWORD pid = GetCurrentProcessId();

            if (Thread32First(snapshot, &entry)) {
                do {
                    if (entry.th32OwnerProcessID != pid) continue;
                    if (entry.th32ThreadID == keepThreadId) continue;

                    HANDLE thread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, entry.th32ThreadID);
                    if (thread) {
                        for (int i = 0; i < 5; i++)
                            SuspendThread(thread);
                        CloseHandle(thread);
                    }
                } while (Thread32Next(snapshot, &entry));
            }
            CloseHandle(snapshot);
        }

        NtSuspendProcessFn ntSuspend = GetNtSuspendProcess();
        if (ntSuspend)
            ntSuspend(GetCurrentProcess());
    }

    static HANDLE CreateKillJob() {
        HANDLE job = CreateJobObjectA(nullptr, nullptr);
        if (!job)
            return nullptr;

        JOBOBJECT_EXTENDED_LIMIT_INFORMATION info = {};
        info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

        if (!SetInformationJobObject(job, JobObjectExtendedLimitInformation, &info, sizeof(info))) {
            CloseHandle(job);
            return nullptr;
        }

        AssignProcessToJobObject(job, GetCurrentProcess());
        return job;
    }

    static std::string BuildReport(const CrashJob& job, const std::string& dumpPath) {
        EXCEPTION_RECORD* record = job.pointers->ExceptionRecord;
        CONTEXT* context = job.pointers->ContextRecord;

        std::stringstream info;
        info << "========================================\r\n";
        info << "          CRASH DETECTED!\r\n";
        info << "========================================\r\n\r\n";
        info << "[TIME] " << GetTimeString() << "\r\n";
        info << "[PID] " << std::dec << GetCurrentProcessId() << "  [THREAD] " << job.faultThreadId << "\r\n\r\n";

        info << "EXCEPTION:\r\n----------------------------------------\r\n";
        info << "Code: " << Hex(record->ExceptionCode) << "\r\n";
        info << "Name: " << GetExceptionName(record->ExceptionCode) << "\r\n";
        info << "Address: " << Hex((uintptr_t)record->ExceptionAddress) << "\r\n";
        info << "Module: " << GetModuleName((uintptr_t)record->ExceptionAddress) << "\r\n";

        if (record->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && record->NumberParameters >= 2) {
            const char* kind = record->ExceptionInformation[0] == 0 ? "Read" :
                record->ExceptionInformation[0] == 1 ? "Write" : "Execute";
            info << "Type: " << kind << "\r\n";
            info << "Target: " << Hex(record->ExceptionInformation[1]) << "\r\n";
        }
        info << "\r\n";

        info << GetRegistersString(context) << "\r\n";
        info << GetCallStack(job.faultThread, *context) << "\r\n";
        info << GetStackDumpString(context->Esp) << "\r\n";
        info << GetModulesString() << "\r\n";

        info << "========================================\r\n";
        info << "Report: " << kLogDir << "/crash_report.txt\r\n";
        if (!dumpPath.empty())
            info << "Dump: " << dumpPath << "\r\n";
        info << "========================================\r\n";
        return info.str();
    }

    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        switch (message) {
        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_SETCURSOR:
            SetCursor(hDefaultCursor);
            return TRUE;

        case WM_ERASEBKGND: {
            RECT area;
            GetClientRect(hWnd, &area);
            FillRect((HDC)wParam, &area, hBgBrush);
            return 1;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT frame = { 9, 9, 721, 441 };
            FrameRect(hdc, &frame, hBorderBrush);
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORSTATIC:
            SetTextColor((HDC)wParam, kColorLight);
            SetBkColor((HDC)wParam, kColorPanel);
            return (LRESULT)hPanelBrush;

        case WM_TIMER:
            if (wParam == 1) {
                for (int id = ID_CLOSE; id <= ID_REPORT; id++)
                    InvalidateRect(GetDlgItem(hWnd, id), nullptr, FALSE);
            }
            else if (wParam == 2) {
                KillTimer(hWnd, 2);
                SetWindowTextA(GetDlgItem(hWnd, ID_COPY), "Copy");
            }
            return 0;

        case WM_DRAWITEM: {
            DRAWITEMSTRUCT* item = (DRAWITEMSTRUCT*)lParam;
            if (item->CtlType != ODT_BUTTON)
                break;

            RECT screenRect;
            POINT cursor;
            GetWindowRect(item->hwndItem, &screenRect);
            GetCursorPos(&cursor);

            bool pressed = (item->itemState & ODS_SELECTED) != 0;
            bool hover = PtInRect(&screenRect, cursor) != 0;

            HBRUSH fill = CreateSolidBrush(pressed ? kColorPressed : hover ? kColorHover : kColorButton);
            HBRUSH edge = CreateSolidBrush(hover || pressed ? kColorText : kColorBorder);
            FillRect(item->hDC, &item->rcItem, fill);
            FrameRect(item->hDC, &item->rcItem, edge);
            DeleteObject(fill);
            DeleteObject(edge);

            char text[64] = {};
            GetWindowTextA(item->hwndItem, text, sizeof(text));

            HFONT font = (HFONT)SendMessageA(item->hwndItem, WM_GETFONT, 0, 0);
            HGDIOBJ oldFont = font ? SelectObject(item->hDC, font) : nullptr;
            SetBkMode(item->hDC, TRANSPARENT);
            SetTextColor(item->hDC, hover || pressed ? kColorText : kColorLight);
            RECT textRect = item->rcItem;
            if (pressed) OffsetRect(&textRect, 1, 1);
            DrawTextA(item->hDC, text, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            if (oldFont) SelectObject(item->hDC, oldFont);
            return TRUE;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
            case ID_CLOSE:
                DestroyWindow(hWnd);
                break;

            case ID_COPY:
                if (OpenClipboard(hWnd)) {
                    EmptyClipboard();
                    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, strLastCrashInfo.size() + 1);
                    if (memory) {
                        char* data = (char*)GlobalLock(memory);
                        if (data) {
                            memcpy(data, strLastCrashInfo.c_str(), strLastCrashInfo.size() + 1);
                            GlobalUnlock(memory);
                            if (!SetClipboardData(CF_TEXT, memory))
                                GlobalFree(memory);
                        }
                        else {
                            GlobalFree(memory);
                        }
                    }
                    CloseClipboard();
                    SetWindowTextA(GetDlgItem(hWnd, ID_COPY), "Copied!");
                    SetTimer(hWnd, 2, 1500, nullptr);
                }
                break;

            case ID_REPORT:
                ShellExecuteA(nullptr, "open", kReportUrl, nullptr, nullptr, SW_SHOW);
                break;
            }
            return 0;
        }
        return DefWindowProcA(hWnd, message, wParam, lParam);
    }

    static HWND CreateButton(HWND parent, HFONT font, const char* text, int x, int id) {
        HWND button = CreateWindowA("BUTTON", text, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            x, 455, 120, 35, parent, (HMENU)(INT_PTR)id, GetModuleHandleA(nullptr), nullptr);
        SendMessageA(button, WM_SETFONT, (WPARAM)font, TRUE);
        return button;
    }

    static void ShowCrashWindow(const std::string& title, const std::string& message) {
        hDefaultCursor = LoadCursorA(nullptr, MAKEINTRESOURCEA(32512));
        HINSTANCE instance = GetModuleHandleA(nullptr);

        hBgBrush = CreateSolidBrush(kColorBg);
        hPanelBrush = CreateSolidBrush(kColorPanel);
        hBorderBrush = CreateSolidBrush(kColorBorder);

        WNDCLASSEXA wc = {};
        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = instance;
        wc.hCursor = hDefaultCursor;
        wc.hbrBackground = hBgBrush;
        wc.lpszClassName = "CrashInformerClass";
        RegisterClassExA(&wc);

        HWND hWnd = CreateWindowExA(WS_EX_TOPMOST, wc.lpszClassName, title.c_str(),
            WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 750, 550,
            nullptr, nullptr, instance, nullptr);

        if (!hWnd) {
            MessageBoxA(nullptr, message.c_str(), title.c_str(), MB_OK | MB_ICONERROR | MB_TOPMOST);
            return;
        }

        HFONT font = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas");

        HWND edit = CreateWindowExA(0, "EDIT", message.c_str(),
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | WS_HSCROLL,
            12, 12, 706, 426, hWnd, (HMENU)(INT_PTR)ID_EDIT, instance, nullptr);
        SendMessageA(edit, WM_SETFONT, (WPARAM)font, TRUE);

        BOOL darkMode = TRUE;
        if (HMODULE dwm = LoadLibraryA("dwmapi.dll")) {
            typedef HRESULT(WINAPI* DwmSetAttributeFn)(HWND, DWORD, LPCVOID, DWORD);
            DwmSetAttributeFn setAttribute = (DwmSetAttributeFn)GetProcAddress(dwm, "DwmSetWindowAttribute");
            if (setAttribute) {
                setAttribute(hWnd, 20, &darkMode, sizeof(darkMode));
                setAttribute(hWnd, 19, &darkMode, sizeof(darkMode));
            }
        }

        if (HMODULE uxtheme = LoadLibraryA("uxtheme.dll")) {
            typedef HRESULT(WINAPI* SetWindowThemeFn)(HWND, LPCWSTR, LPCWSTR);
            SetWindowThemeFn setTheme = (SetWindowThemeFn)GetProcAddress(uxtheme, "SetWindowTheme");
            if (setTheme)
                setTheme(edit, L"DarkMode_Explorer", nullptr);
        }

        SetTimer(hWnd, 1, 60, nullptr);

        HFONT uiFont = CreateFontA(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana");

        CreateButton(hWnd, uiFont, "Copy", 10, ID_COPY);
        CreateButton(hWnd, uiFont, "Report Crash", 140, ID_REPORT);
        CreateButton(hWnd, uiFont, "Close", 600, ID_CLOSE);

        ShowWindow(hWnd, SW_SHOW);
        UpdateWindow(hWnd);
        SetForegroundWindow(hWnd);
        BringWindowToTop(hWnd);
        SetFocus(hWnd);

        MSG msg;
        while (GetMessageA(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }

        DeleteObject(font);
        DeleteObject(uiFont);
        UnregisterClassA(wc.lpszClassName, instance);
        DeleteObject(hBgBrush);
        DeleteObject(hPanelBrush);
        DeleteObject(hBorderBrush);
        hBgBrush = hPanelBrush = hBorderBrush = nullptr;
    }

    static DWORD WINAPI CrashThreadProc(LPVOID parameter) {
        CrashJob* job = (CrashJob*)parameter;
        dwHandlerThreadId = GetCurrentThreadId();

        SuspendAllThreadsHard(GetCurrentThreadId());

        std::string stamp = GetTimeString("%Y%m%d_%H%M%S");
        EnsureLogDirectory();

        std::string dumpPath = WriteMiniDump(job->pointers, job->faultThreadId, stamp);
        std::string report = BuildReport(*job, dumpPath);

        strLastCrashInfo = report;
        WriteTextFile(std::string(kLogDir) + "/crash_report.txt", report, "w");
        WriteTextFile(std::string(kLogDir) + "/crash_" + stamp + ".txt", report, "w");

        ShowCrashWindow(kAppName, report);

        if (job->jobObject)
            CloseHandle(job->jobObject);

        TerminateProcess(GetCurrentProcess(), 0xDEAD);
        ExitProcess(0xDEAD);
        return 0;
    }

    static void Terminate(UINT code) {
        TerminateProcess(GetCurrentProcess(), code);
        ExitProcess(code);
    }

    static LONG HandleFatal(EXCEPTION_POINTERS* pointers) {
        if (bHandling.exchange(true)) {
            if (GetCurrentThreadId() == dwHandlerThreadId.load())
                Terminate(0xDEAD);
            Sleep(INFINITE);
            return EXCEPTION_CONTINUE_SEARCH;
        }

        CrashJob job = {};
        job.pointers = pointers;
        job.faultThreadId = GetCurrentThreadId();
        job.jobObject = CreateKillJob();

        DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(),
            &job.faultThread, 0, FALSE, DUPLICATE_SAME_ACCESS);

        HANDLE worker = CreateThread(nullptr, 0, CrashThreadProc, &job, 0, nullptr);
        if (worker) {
            WaitForSingleObject(worker, INFINITE);
            CloseHandle(worker);
        }

        if (job.jobObject)
            CloseHandle(job.jobObject);

        TerminateProcess(GetCurrentProcess(), 0xDEAD);
        ExitProcess(0xDEAD);
        return EXCEPTION_EXECUTE_HANDLER;
    }

    static void RaiseFatal(DWORD code) {
        EXCEPTION_RECORD record = {};
        record.ExceptionCode = code;
        record.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
        record.ExceptionAddress = _ReturnAddress();

        CONTEXT context = {};
        context.ContextFlags = CONTEXT_FULL;
        RtlCaptureContext(&context);

        EXCEPTION_POINTERS pointers = { &record, &context };
        HandleFatal(&pointers);
    }

    static void OnTerminate() { RaiseFatal(CRASH_CPP_TERMINATE); }
    static void OnAbort(int) { RaiseFatal(CRASH_ABORT); }
    static void OnPureCall() { RaiseFatal(CRASH_PURE_CALL); }

    static void OnInvalidParameter(const wchar_t*, const wchar_t*, const wchar_t*, unsigned int, uintptr_t) {
        RaiseFatal(CRASH_INVALID_PARAMETER);
    }

    static bool IsFatalCode(DWORD code) {
        switch (code) {
        case EXCEPTION_ACCESS_VIOLATION:
        case EXCEPTION_ILLEGAL_INSTRUCTION:
        case EXCEPTION_PRIV_INSTRUCTION:
        case EXCEPTION_STACK_OVERFLOW:
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
        case EXCEPTION_IN_PAGE_ERROR:
            return true;
        default:
            return false;
        }
    }

    static LONG CALLBACK VectoredHandler(EXCEPTION_POINTERS* pointers) {
        if (IsFatalCode(pointers->ExceptionRecord->ExceptionCode))
            return HandleFatal(pointers);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    static void LockUnhandledExceptionFilter() {
        HMODULE kernel = GetModuleHandleA("kernel32.dll");
        if (!kernel) return;

        BYTE* target = (BYTE*)GetProcAddress(kernel, "SetUnhandledExceptionFilter");
        if (!target) return;

        static const BYTE patch[] = { 0x33, 0xC0, 0xC2, 0x04, 0x00 };

        DWORD oldProtect = 0;
        if (VirtualProtect(target, sizeof(patch), PAGE_EXECUTE_READWRITE, &oldProtect)) {
            memcpy(target, patch, sizeof(patch));
            VirtualProtect(target, sizeof(patch), oldProtect, &oldProtect);
            FlushInstructionCache(GetCurrentProcess(), target, sizeof(patch));
        }
    }

public:
    static void Initialize() {
        if (bInitialized.exchange(true)) return;

        SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME | SYMOPT_LOAD_LINES);
        SymInitialize(GetCurrentProcess(), nullptr, TRUE);

        SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
        if (bCatchFirstChance)
            AddVectoredExceptionHandler(1, VectoredHandler);

        SetUnhandledExceptionFilter(UnhandledExceptionFilter);
        LockUnhandledExceptionFilter();

        std::set_terminate(OnTerminate);
        signal(SIGABRT, OnAbort);
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
        _set_purecall_handler(OnPureCall);
        _set_invalid_parameter_handler(OnInvalidParameter);
    }

    static LONG WINAPI UnhandledExceptionFilter(EXCEPTION_POINTERS* pointers) {
        return HandleFatal(pointers);
    }

    static void ShowError(const char* format, ...) {
        char buffer[2048];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        std::string message = "========================================\r\n";
        message += "          ERROR!\r\n";
        message += "========================================\r\n\r\n";
        message += "[TIME] " + GetTimeString() + "\r\n\r\n";
        message += std::string(buffer) + "\r\n\r\n";
        message += "========================================\r\n";

        strLastCrashInfo = message;

        EnsureLogDirectory();
        WriteTextFile(std::string(kLogDir) + "/error_log.txt", message, "a");

        ShowCrashWindow(std::string(kAppName) + " Error", message);

        TerminateProcess(GetCurrentProcess(), 0xDEAD);
    }

    static void ShowFatalError(const char* format, ...) {
        char buffer[2048];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        ShowError("%s", buffer);
        Terminate(0xDEAD);
    }
};