/*

            .        :    :::.        .,::      .:   ::::::::::. :::.    .-:.     ::-.:::.    :::..,::::::
            ;;,.    ;;;   ;;`;;       `;;;,  .,;;     `;;;```.;;;;;`;;    ';;.   ;;;;'`;;;;,  `;;;;;;;''''
            [[[[, ,[[[[, ,[[ '[[,       '[[,,[['       `]]nnn]]',[[ '[[,    '[[,[[['    [[[[[. '[[ [[cccc
            $$$$$$$$"$$$c$$$cc$$$c       Y$$$P          $$$""  c$$$cc$$$c     c$$"      $$$ "Y$c$$ $$""""
            888 Y88" 888o888   888     oP"``"Yo,        888o    888   888   ,8P"`       888    Y88 888oo,__
            MMM  M'  "MMMYMM   \"\"`,m"       "Mm,      YMMMb   YMM   \"\"`mM"          MMM     YM """"YUMMM

    ______________________________________________________________________________________________________________

    - Created by: mateusware
    - Module: maxpayne-injector
    - Description: 
        It's the injector; I created this to avoid using external injectors (outside the project).

*/

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

DWORD GetProcId(const char* name) {
    DWORD pid = 0;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(entry);

    if (Process32FirstW(snap, &entry)) {
        do {
            char exeFile[260];
            WideCharToMultiByte(CP_ACP, 0, entry.szExeFile, -1, exeFile, 260, NULL, NULL);
            if (!_stricmp(exeFile, name)) {
                pid = entry.th32ProcessID;
                break;
            }
        } while (Process32NextW(snap, &entry));
    }
    CloseHandle(snap);
    return pid;
}

bool Inject(DWORD pid, const char* dll) {
    HANDLE p = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!p) return false;

    LPVOID mem = VirtualAllocEx(p, NULL, strlen(dll) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (!mem) {
        CloseHandle(p);
        return false;
    }

    WriteProcessMemory(p, mem, dll, strlen(dll) + 1, NULL);

    HANDLE t = CreateRemoteThread(p, NULL, 0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32"), "LoadLibraryA"),
        mem, 0, NULL);

    if (!t) {
        VirtualFreeEx(p, mem, 0, MEM_RELEASE);
        CloseHandle(p);
        return false;
    }

    WaitForSingleObject(t, INFINITE);
    CloseHandle(t);
    VirtualFreeEx(p, mem, 0, MEM_RELEASE);
    CloseHandle(p);
    return true;
}

int main() {
    char dllPath[MAX_PATH];
    char exePath[MAX_PATH];

    char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);

    strcpy_s(exePath, currentDir);
    strcat_s(exePath, "\\..\\MaxPayne.exe");

    strcpy_s(dllPath, currentDir);
    strcat_s(dllPath, "\\maxpayne-mp.dll");

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (!CreateProcessA(exePath, NULL, NULL, NULL, FALSE,
        CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        return 1;
    }

    LPVOID mem = VirtualAllocEx(pi.hProcess, NULL, strlen(dllPath) + 1,
        MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(pi.hProcess, mem, dllPath, strlen(dllPath) + 1, NULL);

    HANDLE t = CreateRemoteThread(pi.hProcess, NULL, 0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32"), "LoadLibraryA"),
        mem, 0, NULL);

    WaitForSingleObject(t, INFINITE);
    CloseHandle(t);
    VirtualFreeEx(pi.hProcess, mem, 0, MEM_RELEASE);

    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return 0;
}