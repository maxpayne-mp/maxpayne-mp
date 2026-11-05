    /*
    SCRIPT::Execute("player->c_pickupweapon ( leadpipe );");
    SCRIPT::Execute("player->c_setinfiniteammo( leadpipe, true);");
    SCRIPT::Execute("player->c_pickupweapon ( baseballbat );");
    SCRIPT::Execute("player->c_setinfiniteammo( baseballbat, true);");
    SCRIPT::Execute("player->c_pickupweapon ( beretta );");
    SCRIPT::Execute("player->c_pickupweapon ( berettadual );");
    SCRIPT::Execute("player->c_pickupammo( berettadual, 1000 );");
    SCRIPT::Execute("player->c_pickupweapon ( deserteagle );");
    SCRIPT::Execute("player->c_pickupammo( deserteagle, 1000 );");
    SCRIPT::Execute("player->c_pickupweapon ( sawedshotgun );");
    SCRIPT::Execute("player->c_pickupammo( sawedshotgun, 1000 );");
    SCRIPT::Execute("player->c_pickupweapon ( pumpshotgun );");
    SCRIPT::Execute("player->c_pickupammo( pumpshotgun, 1000 );");
    SCRIPT::Execute("player->c_pickupweapon ( ingram );");
    SCRIPT::Execute("player->c_pickupweapon ( ingramdual );");
    SCRIPT::Execute("player->c_pickupammo( ingramdual, 1000 );");
    SCRIPT::Execute("player->c_pickupweapon ( sniper );");
    SCRIPT::Execute("player->c_pickupammo(sniper, 1000 );");
    SCRIPT::Execute("player->c_pickupweapon ( mp5 );");
    SCRIPT::Execute("player->c_pickupammo( mp5, 1000 );");
    SCRIPT::Execute("player->c_pickupweapon ( jackhammer );");
    SCRIPT::Execute("player->c_pickupammo( jackhammer, 1000 );");
    SCRIPT::Execute("player->c_pickupweapon ( m79 );");
    SCRIPT::Execute("player->c_pickupammo( m79, 20 );");
    SCRIPT::Execute("player->c_pickupweapon ( grenade );");
    SCRIPT::Execute("player->c_pickupammo( grenade, 20 );");
    SCRIPT::Execute("player->c_pickupweapon ( molotov );");
    SCRIPT::Execute("player->c_pickupammo( molotov, 20 );");
    SCRIPT::Execute("player->c_pickupweapon ( painkiller );");
    SCRIPT::Execute("player->c_pickupammo( painkiller, 20 );");
    */

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