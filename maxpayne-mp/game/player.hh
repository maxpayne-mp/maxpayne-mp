#pragma once

#include <cstdarg>
#include <cstdio>
#include <string_view>
#include "lib/utils.h"
#include "e2mfc.hh"

#include "script.hh"

namespace PLAYER
{
    namespace ADDRESS
    {
        constexpr const auto addrGetPlayer    = 0x698EC0;
        constexpr const auto addrCharInit     = 0x692E40;
        constexpr const auto addrCharUpdate   = 0x694FE0;
        constexpr const auto addrCharDtor     = 0x692F90;
        constexpr const auto vtableCharacter  = 0x007D59D0;
        constexpr const auto vtableLocalInput = 0x007CFF9C;
    }

    namespace OFFSET
    {
        constexpr const auto transform = 0x1E;
        constexpr const auto forward   = 0x7F4;
        constexpr const auto matrixPos = 0x30;
        constexpr const auto camNormal = 0x24;
        constexpr const auto camTarget = 0x34;
    }

    struct Vec3
    {
        float x;
        float y;
        float z;
    };

    inline bool  b_playerInitialized = false;
    inline void* g_pPlayer      = nullptr;
    inline void* g_pPlayerInput = nullptr;
    inline void* g_pCamera      = nullptr;

    inline bool Jump(int height)
    {
        char buf[128];
        std::snprintf(buf, sizeof(buf), "player->C_Jump(%d);", height);
        return SCRIPT::Execute(buf);
    }

    inline bool SetHealth(float health)
    {
        if(health < 0.0f) health = 0.0f;
        if(health > 100.0f) health = 100.0f;
        char buf[128];
        std::snprintf(buf, sizeof(buf), "player->c_sethealth(%f);", (health / 100.0f));
        return SCRIPT::Execute(buf);
    }

    inline bool CauseDamage(float damage)
    {
        if(damage < 0.0f) damage = 0.0f;
        if(damage > 100.0f) damage = 100.0f;
        char buf[128];
        std::snprintf(buf, sizeof(buf), "player->c_causedamage(%f);", (damage / 100.0f));
        return SCRIPT::Execute(buf);
    }

    #include "weapon.inc"

    inline bool GetPosition(Vec3* out)
    {
        if (!g_pPlayer)
            return false;

        __try {
            out->x = *(float*)((char*)g_pPlayer + OFFSET::transform + OFFSET::matrixPos + 0);
            out->y = *(float*)((char*)g_pPlayer + OFFSET::transform + OFFSET::matrixPos + 4);
            out->z = *(float*)((char*)g_pPlayer + OFFSET::transform + OFFSET::matrixPos + 8);
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    inline bool GetForward(Vec3* out)
    {
        if (!g_pPlayer)
            return false;

        __try {
            out->x = *(float*)((char*)g_pPlayer + OFFSET::forward + 0);
            out->y = *(float*)((char*)g_pPlayer + OFFSET::forward + 4);
            out->z = *(float*)((char*)g_pPlayer + OFFSET::forward + 8);
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    inline bool GetCameraNormal(Vec3* out)
    {
        if (!g_pCamera)
            return false;

        __try {
            out->x = *(float*)((char*)g_pCamera + OFFSET::camNormal + 0);
            out->y = *(float*)((char*)g_pCamera + OFFSET::camNormal + 4);
            out->z = *(float*)((char*)g_pCamera + OFFSET::camNormal + 8);
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    inline bool GetCameraTargetPosition(Vec3* out)
    {
        if (!g_pCamera)
            return false;

        __try {
            void* target = *(void**)((char*)g_pCamera + OFFSET::camTarget);
            if (!target)
                return false;

            out->x = *(float*)((char*)target + OFFSET::matrixPos + 0);
            out->y = *(float*)((char*)target + OFFSET::matrixPos + 4);
            out->z = *(float*)((char*)target + OFFSET::matrixPos + 8);
            return true;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    using CharInitFn   = int(__fastcall*)(void* self, int edx);
    using CharUpdateFn = int(__fastcall*)(void* self, int edx, int a2, int a3, int a4, int a5, int a6, int a7);
    using CharDtorFn   = int(__fastcall*)(void* self, int edx, char a2);

    inline CharInitFn   original_CharInit   = nullptr;
    inline CharUpdateFn original_CharUpdate = nullptr;
    inline CharDtorFn   original_CharDtor   = nullptr;

    inline SafetyHookInline g_CharInit;
    inline SafetyHookInline g_CharUpdate;
    inline SafetyHookInline g_CharDtor;

    int __fastcall CharInit_Hook(void* self, int edx)
    {
        if (!self)
            return original_CharInit(self, edx);

        void* vptr = nullptr;
        __try {
            vptr = *(void**)self;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            OutputDebugString("CharInit: self=0x%p vptr invalido\n", self);
            return original_CharInit(self, edx);
        }

        int r = original_CharInit(self, edx);

        if (vptr == (void*)ADDRESS::vtableCharacter)
        {
            g_pPlayer = self;
            #ifdef PLAYER_DEBUG
                OutputDebugString("CharInit: X_Character -> g_pPlayer=0x%p\n", self);
            #endif
        }

        return r;
    }

    int __fastcall CharUpdate_Hook(void* self, int edx, int a2, int a3, int a4, int a5, int a6, int a7)
    {
        if (a6)
            g_pCamera = (void*)a6;

        if (self == g_pPlayer)
        {
            if(!b_playerInitialized)
            {
                #ifdef SCRIPT_EMBEDDED_DEBUG
                    OutputDebugString("# OnPlayerSpawn\n");
                    CONSOLE::Print("# OnPlayerSpawn\n");
                #endif
                b_playerInitialized = !b_playerInitialized;
            }
            else
            {
                #ifdef SCRIPT_EMBEDDED_DEBUG
                    //OutputDebugString("# OnPlayerUpdate");
                    //CONSOLE::Print("# OnPlayerUpdate\n");
                #endif


            }

            #ifdef PLAYER_DEBUG
                // Vec3 pos, fwd, cam, camTarget;
                // bool hasPos = GetPosition(&pos);
                // bool hasFwd = GetForward(&fwd);
                // bool hasCam = GetCameraNormal(&cam);
                // bool hasCamTarget = GetCameraTargetPosition(&camTarget);
                // OutputDebugString("Player pos=(%.2f %.2f %.2f) %d fwd=(%.2f %.2f %.2f) %d cam=(%.2f %.2f %.2f) %d camTarget=(%.2f %.2f %.2f) %d\n",
                //                   pos.x, pos.y, pos.z, (int)hasPos,
                //                   fwd.x, fwd.y, fwd.z, (int)hasFwd,
                //                   cam.x, cam.y, cam.z, (int)hasCam,
                //                   camTarget.x, camTarget.y, camTarget.z, (int)hasCamTarget);
            #endif
        }
        return original_CharUpdate(self, edx, a2, a3, a4, a5, a6, a7);
    }

    int __fastcall CharDtor_Hook(void* self, int edx, char a2)
    {
        if (self == g_pPlayer)
        {
            g_pPlayer      = nullptr;
            g_pPlayerInput = nullptr;
            g_pCamera      = nullptr;
        }
        return original_CharDtor(self, edx, a2);
    }

    inline void Install()
    {
        g_CharInit = safetyhook::create_inline((void*)ADDRESS::addrCharInit, CharInit_Hook);
        original_CharInit = g_CharInit.original<CharInitFn>();

        g_CharUpdate = safetyhook::create_inline((void*)ADDRESS::addrCharUpdate, CharUpdate_Hook);
        original_CharUpdate = g_CharUpdate.original<CharUpdateFn>();

        g_CharDtor = safetyhook::create_inline((void*)ADDRESS::addrCharDtor, CharDtor_Hook);
        original_CharDtor = g_CharDtor.original<CharDtorFn>();

        OutputDebugString("PLAYER::Install()\n");
    }
}