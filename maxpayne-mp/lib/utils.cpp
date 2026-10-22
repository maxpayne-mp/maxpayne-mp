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

void PATCH_DATA(DWORD addr, BYTE* data, size_t size) 
{
    DWORD oldProtect;
    VirtualProtect((LPVOID)addr, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy((LPVOID)addr, data, size);
    VirtualProtect((LPVOID)addr, size, oldProtect, &oldProtect);
}
