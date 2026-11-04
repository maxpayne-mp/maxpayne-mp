#pragma once

#include <cstdarg>
#include <cstdio>
#include <string_view>
#include "lib/utils.h"

#include "script.hh"

namespace PLAYER
{
    namespace ADDRESS
    {
        constexpr const auto addrGetPlayer = 0x698EC0;
    }

    //      Functions 
    // --------------------------------------------------------------

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

    namespace WEAPON
    {
        enum class WeaponId {
            Leadpipe,
            BaseballBat,
            Beretta,
            BerettaDual,
            DesertEagle,
            SawedShotgun,
            PumpShotgun,
            Ingram,
            IngramDual,
            Sniper,
            MP5,
            Jackhammer,
            M79,
            Grenade,
            Molotov,
            Painkiller,
        };

        constexpr std::string_view WeaponName(WeaponId id) {
            switch (id) {
                case WeaponId::Leadpipe:     return "leadpipe";
                case WeaponId::BaseballBat:  return "baseballbat";
                case WeaponId::Beretta:      return "beretta";
                case WeaponId::BerettaDual:  return "berettadual";
                case WeaponId::DesertEagle:  return "deserteagle";
                case WeaponId::SawedShotgun: return "sawedshotgun";
                case WeaponId::PumpShotgun:  return "pumpshotgun";
                case WeaponId::Ingram:       return "ingram";
                case WeaponId::IngramDual:   return "ingramdual";
                case WeaponId::Sniper:       return "sniper";
                case WeaponId::MP5:          return "mp5";
                case WeaponId::Jackhammer:   return "jackhammer";
                case WeaponId::M79:          return "m79";
                case WeaponId::Grenade:      return "grenade";
                case WeaponId::Molotov:      return "molotov";
                case WeaponId::Painkiller:   return "painkiller";
            }
            return "";
        }

        inline bool GiveWeapon(const char* weapon)
        {
            char buf[128];
            std::snprintf(buf, sizeof(buf), "player->c_pickupweapon ( %s );", weapon);
            return SCRIPT::Execute(buf);
        }

        inline bool SetInfiniteAmmo(const char* weapon, bool enable)
        {
            char buf[128];
            std::snprintf(buf, sizeof(buf), "player->c_setinfiniteammo( %s, %s );",
                          weapon, enable ? "true" : "false");
            return SCRIPT::Execute(buf);
        }

        inline bool GiveAmmo(const char* weapon, int amount)
        {
            char buf[128];
            std::snprintf(buf, sizeof(buf), "player->c_pickupammo( %s, %d );", weapon, amount);
            return SCRIPT::Execute(buf);
        }

        inline bool RemoveWeapon(const char* weapon)
        {
            char buf[128];
            std::snprintf(buf, sizeof(buf), "player->c_removeallweapons( %s );", weapon);
            return SCRIPT::Execute(buf);
        }

        inline bool GiveWeapon(WeaponId weapon)
        {
            return GiveWeapon(WeaponName(weapon).data());
        }

        inline bool SetInfiniteAmmo(WeaponId weapon, bool enable)
        {
            return SetInfiniteAmmo(WeaponName(weapon).data(), enable);
        }

        inline bool GiveAmmo(WeaponId weapon, int amount)
        {
            return GiveAmmo(WeaponName(weapon).data(), amount);
        }

        inline bool RemoveWeapon(WeaponId weapon)
        {
            return RemoveWeapon(WeaponName(weapon).data());
        }
    }


    using GetPlayerFn = int(__fastcall*)(void* self, int edx, int a2);
    inline GetPlayerFn original_GetPlayer = nullptr;
    inline SafetyHookInline g_GetPlayer;

    int __fastcall GetPlayer_Hook(void* self, int edx, int a2)
    {
        // if (self) g_XConsoleMode = self;
        int r = original_GetPlayer(self, edx, a2);
        OutputDebugString("GetPlayer: self=0x%p a2=0x%p\n", self, (void*)a2);
        return r;
    }
    

    inline void Install()
    {
        g_GetPlayer = safetyhook::create_inline((void*)ADDRESS::addrGetPlayer, GetPlayer_Hook);
        original_GetPlayer = g_GetPlayer.original<GetPlayerFn>();
        OutputDebugString("PLAYER::Install()\n");
    }
}