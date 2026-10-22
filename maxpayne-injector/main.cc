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
#include <cstdio>
#include <string>

static void PrintLastError(const char* operation) {
    const DWORD error = GetLastError();
    char message[512] = {};
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error,
        0,
        message,
        sizeof(message),
        nullptr);
    printf("ERRO: %s falhou (codigo %lu): %s\n", operation, error, message);
}

int main() {
    char launcherPath[MAX_PATH] = {};
    const DWORD pathLength = GetModuleFileNameA(nullptr, launcherPath, MAX_PATH);
    if (pathLength == 0 || pathLength >= MAX_PATH) {
        PrintLastError("GetModuleFileNameA");
        return 1;
    }

    std::string launcherDirectory(launcherPath, pathLength);
    const size_t separator = launcherDirectory.find_last_of("\\/");
    if (separator == std::string::npos) {
        printf("ERRO: nao foi possivel determinar a pasta do launcher.\n");
        return 1;
    }
    launcherDirectory.resize(separator);

    const std::string dllPath = launcherDirectory + "\\maxpayne-mp.dll";
    char gameDirectoryBuffer[MAX_PATH] = {};
    if (!GetFullPathNameA(
        (launcherDirectory + "\\..").c_str(),
        MAX_PATH,
        gameDirectoryBuffer,
        nullptr)) {
        PrintLastError("GetFullPathNameA");
        return 1;
    }
    const std::string gameDirectory = gameDirectoryBuffer;
    const std::string exePath = gameDirectory + "\\maxpayne.exe";

    if (GetFileAttributesA(dllPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        PrintLastError("encontrar maxpayne-mp.dll");
        return 1;
    }
    if (GetFileAttributesA(exePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        PrintLastError("encontrar maxpayne.exe");
        return 1;
    }

    STARTUPINFOA startupInfo = { sizeof(startupInfo) };
    PROCESS_INFORMATION processInfo = {};
    if (!CreateProcessA(
        exePath.c_str(),
        nullptr,
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        gameDirectory.c_str(),
        &startupInfo,
        &processInfo)) {
        PrintLastError("CreateProcessA");
        return 1;
    }

    Sleep(2000);

    const SIZE_T dllPathSize = dllPath.size() + 1;
    LPVOID remotePath = VirtualAllocEx(
        processInfo.hProcess,
        nullptr,
        dllPathSize,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE);
    if (!remotePath) {
        PrintLastError("VirtualAllocEx");
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        return 1;
    }

    SIZE_T bytesWritten = 0;
    if (!WriteProcessMemory(
        processInfo.hProcess,
        remotePath,
        dllPath.c_str(),
        dllPathSize,
        &bytesWritten) ||
        bytesWritten != dllPathSize) {
        PrintLastError("WriteProcessMemory");
        VirtualFreeEx(processInfo.hProcess, remotePath, 0, MEM_RELEASE);
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        return 1;
    }

    const auto loadLibrary = reinterpret_cast<LPTHREAD_START_ROUTINE>(
        GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"));
    if (!loadLibrary) {
        PrintLastError("GetProcAddress(LoadLibraryA)");
        VirtualFreeEx(processInfo.hProcess, remotePath, 0, MEM_RELEASE);
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        return 1;
    }

    HANDLE remoteThread = CreateRemoteThread(
        processInfo.hProcess,
        nullptr,
        0,
        loadLibrary,
        remotePath,
        0,
        nullptr);
    if (!remoteThread) {
        PrintLastError("CreateRemoteThread");
        VirtualFreeEx(processInfo.hProcess, remotePath, 0, MEM_RELEASE);
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        return 1;
    }

    WaitForSingleObject(remoteThread, INFINITE);
    DWORD loadResult = 0;
    const bool gotExitCode = GetExitCodeThread(remoteThread, &loadResult) != FALSE;
    CloseHandle(remoteThread);
    VirtualFreeEx(processInfo.hProcess, remotePath, 0, MEM_RELEASE);

    if (!gotExitCode) {
        PrintLastError("GetExitCodeThread");
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        return 1;
    }
    if (loadResult == 0) {
        printf("ERRO: LoadLibraryA nao conseguiu carregar maxpayne-mp.dll.\n");
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        return 1;
    }

    printf("DLL injetada com sucesso. PID: %lu\n", processInfo.dwProcessId);
    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);

    return 0;
}