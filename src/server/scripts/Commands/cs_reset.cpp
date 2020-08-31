#include "Chat.h"
#include "Language.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Pet.h"
#include "ScriptMgr.h"

class reset_commandscript : public CommandScript
{
public:
    reset_commandscript() : CommandScript("reset_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> resetCommandTable =
        {
            { "honor",          SEC_ADMINISTRATOR,  true,  &HandleResetHonorCommand,            "" },
            { "level",          SEC_ADMINISTRATOR,  true,  &HandleResetLevelCommand,            "" },
            { "spells",         SEC_ADMINISTRATOR,  true,  &HandleResetSpellsCommand,           "" },
            { "stats",          SEC_ADMINISTRATOR,  true,  &HandleResetStatsCommand,            "" },
            { "talents",        SEC_ADMINISTRATOR,  true,  &HandleResetTalentsCommand,          "" },
            { "all",            SEC_CONSOLE,        true,  &HandleResetAllCommand,              "" }
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "reset",          SEC_ADMINISTRATOR,  true, nullptr,                              "", resetCommandTable }
        };
        return commandTable;
    }

    static bool HandleResetHonorCommand(ChatHandler* handler, const char* args)
    {
        char* pName = strtok((char*)args, "");
        Player* player = NULL;
        if (pName)
        {
            std::string name = pName;
            if (!normalizePlayerName(name))
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint64 guid = sObjectMgr.GetPlayerGUIDByName(name.c_str());
            player = sObjectMgr.GetPlayer(guid);
        }
        else
            player = handler->getSelectedPlayer();

        if (!player)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            return true;
        }

        player->SetUInt32Value(PLAYER_FIELD_KILLS, 0);
        player->SetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS, 0);
        player->SetUInt32Value(PLAYER_FIELD_HONOR_CURRENCY, 0);
        player->SetUInt32Value(PLAYER_FIELD_TODAY_CONTRIBUTION, 0);
        player->SetUInt32Value(PLAYER_FIELD_YESTERDAY_CONTRIBUTION, 0);

        return true;
    }

    static bool HandleResetStatsOrLevelHelper(Player* player)
    {
        PlayerInfo const* info = sObjectMgr.GetPlayerInfo(player->getRace(), player->getClass());
        if (!info) return false;

        ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(player->getClass());
        if (!cEntry)
        {
            sLog.outError("Class %u not found in DBC (Wrong DBC files?)", player->getClass());
            return false;
        }

        uint8 powertype = cEntry->powerType;

        uint32 unitfield;
        if (powertype == POWER_RAGE)
            unitfield = 0x1100EE00;
        else if (powertype == POWER_ENERGY)
            unitfield = 0x00000000;
        else if (powertype == POWER_MANA)
            unitfield = 0x0000EE00;
        else
        {
            sLog.outError("Invalid default powertype %u for player (class %u)", powertype, player->getClass());
            return false;
        }

        // reset m_form if no aura
        if (!player->HasAuraType(SPELL_AURA_MOD_SHAPESHIFT))
            player->m_form = FORM_NONE;

        player->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, DEFAULT_WORLD_OBJECT_SIZE);
        player->SetFloatValue(UNIT_FIELD_COMBATREACH, DEFAULT_COMBAT_REACH);

        player->setFactionForRace(player->getRace());

        player->SetUInt32Value(UNIT_FIELD_BYTES_0, ((player->getRace()) | (player->getClass() << 8) | (player->getGender() << 16) | (powertype << 24)));

        // reset only if player not in some form;
        if (player->m_form == FORM_NONE)
        {
            switch (player->getGender())
            {
            case GENDER_FEMALE:
                player->SetDisplayId(info->displayId_f);
                player->SetNativeDisplayId(info->displayId_f);
                break;
            case GENDER_MALE:
                player->SetDisplayId(info->displayId_m);
                player->SetNativeDisplayId(info->displayId_m);
                break;
            default:
                break;
            }
        }

        // set UNIT_FIELD_BYTES_1 to init state but preserve m_form value
        player->SetUInt32Value(UNIT_FIELD_BYTES_1, unitfield);
        player->SetByteValue(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_SANCTUARY | UNIT_BYTE2_FLAG_UNK5);
        player->SetByteValue(UNIT_FIELD_BYTES_2, 3, player->m_form);

        player->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);

        //-1 is default value
        player->SetInt32Value(PLAYER_FIELD_WATCHED_FACTION_INDEX, -1);

        //player->SetUInt32Value(PLAYER_FIELD_BYTES, 0xEEE00000);
        return true;
    }

    static bool HandleResetLevelCommand(ChatHandler* handler, const char* args)
    {
        char* pName = strtok((char*)args, "");
        Player* player = NULL;
        if (pName)
        {
            std::string name = pName;
            if (!normalizePlayerName(name))
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint64 guid = sObjectMgr.GetPlayerGUIDByName(name.c_str());
            player = sObjectMgr.GetPlayer(guid);
        }
        else
            player = handler->getSelectedPlayer();

        if (!player)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!HandleResetStatsOrLevelHelper(player))
            return false;

        // set starting level
        uint32 startLevel = sWorld.getConfig(CONFIG_START_PLAYER_LEVEL);

        player->SetLevel(startLevel);
        player->InitStatsForLevel(true);
        player->InitTaxiNodesForLevel();
        player->InitTalentForLevel();
        player->SetUInt32Value(PLAYER_XP, 0);

        // reset level to summoned pet
        Guardian* pet = player->GetGuardianPet();
        if (pet)
            pet->InitStatsForLevel(startLevel);

        return true;
    }

    static bool HandleResetSpellsCommand(ChatHandler* handler, const char* args)
    {
        char* pName = strtok((char*)args, "");
        Player* player = NULL;
        uint64 playerGUID = 0;
        if (pName)
        {
            std::string name = pName;

            if (!normalizePlayerName(name))
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            player = sObjectMgr.GetPlayer(name.c_str());
            if (!player)
                playerGUID = sObjectMgr.GetPlayerGUIDByName(name.c_str());
        }
        else
            player = handler->getSelectedPlayer();

        if (!player && !playerGUID)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player)
        {
            player->ResetSpells();

            ChatHandler(player).SendSysMessage(LANG_RESET_SPELLS);

            if (handler->GetSession()->GetPlayer() != player)
                handler->PSendSysMessage(LANG_RESET_SPELLS_ONLINE, player->GetName());
        }
        else
        {
            CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '%u' WHERE guid = '%u'", uint32(AT_LOGIN_RESET_SPELLS), GUID_LOPART(playerGUID));
            handler->PSendSysMessage(LANG_RESET_SPELLS_OFFLINE, pName);
        }

        return true;
    }

    static bool HandleResetStatsCommand(ChatHandler* handler, const char* args)
    {
        char* pName = strtok((char*)args, "");
        Player* player = NULL;
        if (pName)
        {
            std::string name = pName;
            if (!normalizePlayerName(name))
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint64 guid = sObjectMgr.GetPlayerGUIDByName(name.c_str());
            player = sObjectMgr.GetPlayer(guid);
        }
        else
            player = handler->getSelectedPlayer();

        if (!player)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!HandleResetStatsOrLevelHelper(player))
            return false;

        player->InitStatsForLevel(true);
        player->InitTaxiNodesForLevel();
        player->InitTalentForLevel();

        return true;
    }

    static bool HandleResetTalentsCommand(ChatHandler* handler, const char* args)
    {
        char* pName = strtok((char*)args, "");
        Player* player = NULL;
        uint64 playerGUID = 0;
        if (pName)
        {
            std::string name = pName;
            if (!normalizePlayerName(name))
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            player = sObjectMgr.GetPlayer(name.c_str());
            if (!player)
                playerGUID = sObjectMgr.GetPlayerGUIDByName(name.c_str());
        }
        else
            player = handler->getSelectedPlayer();

        if (!player && !playerGUID)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player)
        {
            player->ResetTalents(true);

            ChatHandler(player).SendSysMessage(LANG_RESET_TALENTS);

            if (handler->GetSession()->GetPlayer() != player)
                handler->PSendSysMessage(LANG_RESET_TALENTS_ONLINE, player->GetName());
        }
        else
        {
            CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '%u' WHERE guid = '%u'", uint32(AT_LOGIN_RESET_TALENTS), GUID_LOPART(playerGUID));
            handler->PSendSysMessage(LANG_RESET_TALENTS_OFFLINE, pName);
        }

        return true;
    }

    static bool HandleResetAllCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string casename = args;

        AtLoginFlags atLogin;

        // Command specially created as single command to prevent using short case names
        if (casename == "spells")
        {
            atLogin = AT_LOGIN_RESET_SPELLS;
            sWorld.SendWorldText(LANG_RESETALL_SPELLS);
        }
        else if (casename == "talents")
        {
            atLogin = AT_LOGIN_RESET_TALENTS;
            sWorld.SendWorldText(LANG_RESETALL_TALENTS);
        }
        else
        {
            handler->PSendSysMessage(LANG_RESETALL_UNKNOWN_CASE, args);
            handler->SetSentErrorMessage(true);
            return false;
        }

        CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '%u' WHERE (at_login & '%u') = '0'", atLogin, atLogin);

        ObjectAccessor::Guard guard(*HashMapHolder<Player>::GetLock());
        HashMapHolder<Player>::MapType const& plist = ObjectAccessor::Instance().GetPlayers();
        for (HashMapHolder<Player>::MapType::const_iterator itr = plist.begin(); itr != plist.end(); ++itr)
            itr->second->SetAtLoginFlag(atLogin);

        return true;
    }

};

void AddSC_reset_commandscript()
{
    new reset_commandscript();
}