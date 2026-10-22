#pragma once
#include "../main.hh"

#define NAKED __declspec(naked)
#define EXCEPTION_SDK ( MyExceptionFilter(GetExceptionInformation()) )

void PATCH_DATA(DWORD addr, BYTE* data, size_t size);

#define PATCH(addr, value, size) do { \
    DWORD old; \
    VirtualProtect((PVOID)(addr), size, PAGE_EXECUTE_READWRITE, &old); \
    memset((PVOID)(addr), value, size); \
    VirtualProtect((PVOID)(addr), size, old, &old); \
} while(0)

template<typename T>
inline void JUMP(uintptr_t address, T destination)
{
    DWORD oldProtect;

    VirtualProtect(
        (LPVOID)address,
        5,
        PAGE_EXECUTE_READWRITE,
        &oldProtect);

    *(uint8_t*)address = 0xE9;

    *(int32_t*)(address + 1) =
        (int32_t)(
            (uintptr_t)destination -
            address -
            5);

    FlushInstructionCache(
        GetCurrentProcess(),
        (LPCVOID)address,
        5);

    VirtualProtect(
        (LPVOID)address,
        5,
        oldProtect,
        &oldProtect);
}

template<typename T>
inline void CALL(uintptr_t address, T destination)
{
    DWORD oldProtect;

    VirtualProtect(
        (LPVOID)address,
        5,
        PAGE_EXECUTE_READWRITE,
        &oldProtect);

    *(uint8_t*)address = 0xE8;

    *(int32_t*)(address + 1) =
        (int32_t)(
            (uintptr_t)destination -
            address -
            5);

    FlushInstructionCache(
        GetCurrentProcess(),
        (LPCVOID)address,
        5);

    VirtualProtect(
        (LPVOID)address,
        5,
        oldProtect,
        &oldProtect);
}



//

template <unsigned int address, typename... Args>
static void Call(Args... args) {
	reinterpret_cast<void(__cdecl*)(Args...)>(address)(args...);
}

template <typename Ret, unsigned int address, typename... Args>
static Ret CallAndReturn(Args... args) {
	return reinterpret_cast<Ret(__cdecl*)(Args...)>(address)(args...);
}

template <unsigned int address, typename C, typename... Args>
static void CallMethod(C _this, Args... args) {
	reinterpret_cast<void(__thiscall*)(C, Args...)>(address)(_this, args...);
}

template <typename Ret, unsigned int address, typename C, typename... Args>
static Ret CallMethodAndReturn(C _this, Args... args) {
	return reinterpret_cast<Ret(__thiscall*)(C, Args...)>(address)(_this, args...);
}

// -----------------------------------------------------------

void ClearConsole();

// -----------------------------------------------------------

int MyExceptionFilter(EXCEPTION_POINTERS* ep);


//

