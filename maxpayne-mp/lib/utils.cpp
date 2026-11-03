#include "utils.h"


int MyExceptionFilter(EXCEPTION_POINTERS* ep)
{
    printf("Code: %08X\n",
        ep->ExceptionRecord->ExceptionCode);

    printf("EIP: %08X\n",
        ep->ContextRecord->Eip);

    return EXCEPTION_EXECUTE_HANDLER;
}


// --------------------------------------------------------------------------------

void ClearConsole()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
    DWORD count;

    FillConsoleOutputCharacter(
        hConsole,
        ' ',
        cellCount,
        { 0, 0 },
        &count
    );

    FillConsoleOutputAttribute(
        hConsole,
        csbi.wAttributes,
        cellCount,
        { 0, 0 },
        &count
    );

    SetConsoleCursorPosition(hConsole, { 0, 0 });
}


bool PATCH_DATA(uintptr_t addr, const BYTE* data, size_t size)
{
    if (addr == 0 || data == nullptr || size == 0)
        return false;

    void* destination = reinterpret_cast<void*>(addr);
    DWORD oldProtect = 0;

    if (!VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &oldProtect))
        return false;

    std::memcpy(destination, data, size);

    FlushInstructionCache(GetCurrentProcess(), destination, size);

    DWORD ignored = 0;
    BOOL restored = VirtualProtect(destination, size, oldProtect, &ignored);

    return restored != FALSE;
}