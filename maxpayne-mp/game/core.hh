#pragma once

#include <vector>
#include <string>
#include "lib/utils.h"
#include "console.hh"
#include "lib/safetyhook.hpp"

#include <wincrypt.h>
#pragma comment(lib, "advapi32.lib")

namespace CORE
{
    namespace ADDRESS
    {
        constexpr const auto addrAddReceiver    = 0x478D00;
        constexpr const auto addrVersion        = 0x7CB730;
        constexpr const auto addrBuild          = 0x40A80E + 0x3EE;

    }

    inline void* g_pGameMode = nullptr;

    using AddReceiverFn = int(__thiscall*)(void* self);
    inline AddReceiverFn original_AddReceiver = nullptr;
    inline SafetyHookInline g_AddReceiver;

    inline int __fastcall AddReceiver_Hook(void* self, void* edx)
    {
        if (CORE::g_pGameMode == nullptr && self != nullptr)
            CORE::g_pGameMode = self;          // self = MaxPayne_GameMode*
        return original_AddReceiver(self);
    }

    static bool checksum(const char* expectedSha256) {
        char exePath[MAX_PATH] = {};
        if (!GetModuleFileNameA(nullptr, exePath, MAX_PATH)) {
            CrashInformer::ShowError("Checksum failed: cannot resolve executable path (error %lu)", GetLastError());
            return false;
        }

        HANDLE file = CreateFileA(exePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE) {
            CrashInformer::ShowError("Checksum failed: cannot open %s (error %lu)", exePath, GetLastError());
            return false;
        }

        HCRYPTPROV prov = 0;
        HCRYPTHASH hash = 0;
        BYTE readBuffer[8192];
        DWORD bytesRead = 0;
        bool ok = false;

        if (!CryptAcquireContextA(&prov, nullptr, nullptr, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
            CloseHandle(file);
            CrashInformer::ShowError("Checksum failed: CryptAcquireContext error %lu", GetLastError());
            return false;
        }

        if (!CryptCreateHash(prov, CALG_SHA_256, 0, 0, &hash)) {
            CryptReleaseContext(prov, 0);
            CloseHandle(file);
            CrashInformer::ShowError("Checksum failed: CryptCreateHash error %lu", GetLastError());
            return false;
        }

        while (ReadFile(file, readBuffer, sizeof(readBuffer), &bytesRead, nullptr) && bytesRead > 0) {
            if (!CryptHashData(hash, readBuffer, bytesRead, 0)) {
                CryptDestroyHash(hash);
                CryptReleaseContext(prov, 0);
                CloseHandle(file);
                CrashInformer::ShowError("Checksum failed: CryptHashData error %lu", GetLastError());
                return false;
            }
        }

        BYTE digest[32] = {};
        DWORD digestSize = sizeof(digest);
        if (!CryptGetHashParam(hash, HP_HASHVAL, digest, &digestSize, 0)) {
            CryptDestroyHash(hash);
            CryptReleaseContext(prov, 0);
            CloseHandle(file);
            CrashInformer::ShowError("Checksum failed: CryptGetHashParam error %lu", GetLastError());
            return false;
        }

        char hex[65] = {};
        for (DWORD i = 0; i < digestSize; i++)
            sprintf_s(hex + i * 2, 3, "%02x", digest[i]);

        if (_stricmp(hex, expectedSha256) != 0)
            CrashInformer::ShowError("Unsupported game patch. Expected %s, got %s\r\nYour game executable appears to have a patch applied, and the checksum does not match. For safety reasons, the mod will not initialize unless the executable is in a valid, intact state. \r\nWithout this verification, the game could experience sudden crashes caused by invalid memory addresses. \r\nTherefore, to ensure the mod runs safely, we have blocked its initialization in this instance.", expectedSha256, hex);

        else
            ok = true;

        CryptDestroyHash(hash);
        CryptReleaseContext(prov, 0);
        CloseHandle(file);
        return ok;
    }

    inline void Install()
    {
        using namespace ADDRESS;

        const char* version = reinterpret_cast<const char*>(addrVersion);
        if (strcmp(version, "1.05")) {
            OutputDebugString("Unsupported game version. Expected 1.05, got ");
            CrashInformer::ShowError("Unsupported game version. Expected 1.05, got %s", version);
            return;
        }
 
        #ifdef VALIDATE_VERSION_TESTS
            if(checksum("E0D6F82B002791391749FE72FDF855EC76EEE0719F0500E65016A48E151498D5")) {
                OutputDebugString("Game checksum verified successfully.\n");
            }

            uint8_t opcode = *reinterpret_cast<uint8_t*>(addrBuild);
            uint8_t imm8   = *reinterpret_cast<uint8_t*>(addrBuild + 1);
            uint32_t imm32 = *reinterpret_cast<uint32_t*>(addrBuild + 1);

            // 171 in game
            // 69 in dialog
            if (imm8 == 171 || imm8 == 69) {
                OutputDebugString("Game build verified successfully.\n");
            } else {
                char buf[128];
                sprintf_s(buf, "Unsupported 'BUILD %d' version. (0x%X) operation code=0x%02X", (int)imm8, (unsigned)addrBuild, opcode);
                CrashInformer::ShowError("%s", buf);
                return;
            }
        #endif

        g_AddReceiver = safetyhook::create_inline((void*)addrAddReceiver, AddReceiver_Hook);
        original_AddReceiver = g_AddReceiver.original<AddReceiverFn>();
    }
}