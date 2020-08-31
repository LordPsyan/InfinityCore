#include "AccountMgr.h"
#include "Chat.h"
#include "Language.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SharedDefines.h"


class ban_commandscript : public CommandScript
{
public:
    ban_commandscript() : CommandScript("ban_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> unbanCommandTable =
        {
            { "account",        SEC_GAMEMASTER,  true,  &HandleUnBanAccountCommand,          "" },
            { "character",      SEC_GAMEMASTER,  true,  &HandleUnBanCharacterCommand,        "" },
            { "ip",             SEC_GAMEMASTER,  true,  &HandleUnBanIPCommand,               "" }
        };

        static std::vector<ChatCommand> banlistCommandTable =
        {
            { "account",        SEC_GAMEMASTER,  true,  &HandleBanListAccountCommand,        "" },
            { "character",      SEC_GAMEMASTER,  true,  &HandleBanListCharacterCommand,      "" },
            { "ip",             SEC_GAMEMASTER,  true,  &HandleBanListIPCommand,             "" }
        };

        static std::vector<ChatCommand> baninfoCommandTable =
        {
            { "account",        SEC_GAMEMASTER,  true,  &HandleBanInfoAccountCommand,        "" },
            { "character",      SEC_GAMEMASTER,  true,  &HandleBanInfoCharacterCommand,      "" },
            { "ip",             SEC_GAMEMASTER,  true,  &HandleBanInfoIPCommand,             "" }
        };

        static std::vector<ChatCommand> banCommandTable =
        {
            { "account",        SEC_GAMEMASTER,  true,  &HandleBanAccountCommand,            "" },
            { "ip",             SEC_GAMEMASTER,  true,  &HandleBanIPCommand,                 "" }
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "ban",            SEC_GAMEMASTER,  true,  nullptr,                             "", banCommandTable },
            { "baninfo",        SEC_GAMEMASTER,  true,  nullptr,                             "", baninfoCommandTable },
            { "banlist",        SEC_GAMEMASTER,  true,  nullptr,                             "", banlistCommandTable },
            { "unban",          SEC_GAMEMASTER,  true,  nullptr,                             "", unbanCommandTable }
        };

        return commandTable;
    }

    static bool HandleUnBanAccountCommand(ChatHandler* handler, const char* args)
    {
        return HandleUnBanHelper(handler, BAN_ACCOUNT, args);
    }

    static bool HandleUnBanCharacterCommand(ChatHandler* handler, const char* args)
    {
        return HandleUnBanHelper(handler, BAN_CHARACTER, args);
    }

    static bool HandleUnBanIPCommand(ChatHandler* handler, const char* args)
    {
        return HandleUnBanHelper(handler, BAN_IP, args);
    }

    static bool HandleUnBanHelper(ChatHandler* handler, BanMode mode, const char* args)
    {
        if (!*args)
            return false;

        char* cnameOrIP = strtok((char*)args, " ");
        if (!cnameOrIP)
            return false;

        std::string nameOrIP = cnameOrIP;

        switch (mode)
        {
        case BAN_ACCOUNT:
            if (!AccountMgr::normalizeString(nameOrIP))
            {
                handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, nameOrIP.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }
            break;
        case BAN_CHARACTER:
            if (!normalizePlayerName(nameOrIP))
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }
            break;
        case BAN_IP:
            if (!IsIPAddress(nameOrIP.c_str()))
                return false;
            break;
        }

        if (sWorld.RemoveBanAccount(mode, nameOrIP))
            handler->PSendSysMessage(LANG_UNBAN_UNBANNED, nameOrIP.c_str());
        else
            handler->PSendSysMessage(LANG_UNBAN_ERROR, nameOrIP.c_str());

        return true;
    }

    static bool HandleBanListAccountCommand(ChatHandler* handler, const char* args)
    {
        LoginDatabase.Execute("DELETE FROM ip_banned WHERE unbandate <= UNIX_TIMESTAMP() AND unbandate<>bandate");

        char* cFilter = strtok((char*)args, " ");
        std::string filter = cFilter ? cFilter : "";
        LoginDatabase.escape_string(filter);

        QueryResult_AutoPtr result;

        if (filter.empty())
        {
            result = LoginDatabase.Query("SELECT account.id, username FROM account, account_banned"
                " WHERE account.id = account_banned.id AND active = 1 GROUP BY account.id");
        }
        else
        {
            result = LoginDatabase.PQuery("SELECT account.id, username FROM account, account_banned"
                " WHERE account.id = account_banned.id AND active = 1 AND username " _LIKE_ " " _CONCAT3_("'%%'", "'%s'", "'%%'")" GROUP BY account.id",
                filter.c_str());
        }

        if (!result)
        {
            handler->PSendSysMessage(LANG_BANLIST_NOACCOUNT);
            return true;
        }

        return HandleBanListHelper(handler, result);
    }

    static bool HandleBanListCharacterCommand(ChatHandler* handler, const char* args)
    {
        LoginDatabase.Execute("DELETE FROM ip_banned WHERE unbandate <= UNIX_TIMESTAMP() AND unbandate<>bandate");

        char* cFilter = strtok((char*)args, " ");
        if (!cFilter)
            return false;

        std::string filter = cFilter;
        LoginDatabase.escape_string(filter);
        QueryResult_AutoPtr result = CharacterDatabase.PQuery("SELECT account FROM characters WHERE name " _LIKE_ " " _CONCAT3_("'%%'", "'%s'", "'%%'"), filter.c_str());
        if (!result)
        {
            handler->PSendSysMessage(LANG_BANLIST_NOCHARACTER);
            return true;
        }

        return HandleBanListHelper(handler, result);
    }

    static bool HandleBanListIPCommand(ChatHandler* handler, const char* args)
    {
        LoginDatabase.Execute("DELETE FROM ip_banned WHERE unbandate <= UNIX_TIMESTAMP() AND unbandate<>bandate");

        char* cFilter = strtok((char*)args, " ");
        std::string filter = cFilter ? cFilter : "";
        LoginDatabase.escape_string(filter);

        QueryResult_AutoPtr result;

        if (filter.empty())
        {
            result = LoginDatabase.Query("SELECT ip,bandate,unbandate,bannedby,banreason FROM ip_banned"
                " WHERE (bandate=unbandate OR unbandate>UNIX_TIMESTAMP())"
                " ORDER BY unbandate");
        }
        else
        {
            result = LoginDatabase.PQuery("SELECT ip,bandate,unbandate,bannedby,banreason FROM ip_banned"
                " WHERE (bandate=unbandate OR unbandate>UNIX_TIMESTAMP()) AND ip " _LIKE_ " " _CONCAT3_("'%%'", "'%s'", "'%%'")
                " ORDER BY unbandate", filter.c_str());
        }

        if (!result)
        {
            handler->PSendSysMessage(LANG_BANLIST_NOIP);
            return true;
        }

        handler->PSendSysMessage(LANG_BANLIST_MATCHINGIP);
        // Chat short output
        if (handler->GetSession())
        {
            do
            {
                Field* fields = result->Fetch();
                handler->PSendSysMessage("%s", fields[0].GetString());
            } while (result->NextRow());
        }
        // Console wide output
        else
        {
            handler->SendSysMessage(LANG_BANLIST_IPS);
            handler->SendSysMessage(" ===============================================================================");
            handler->SendSysMessage(LANG_BANLIST_IPS_HEADER);
            do
            {
                handler->SendSysMessage("-------------------------------------------------------------------------------");
                Field* fields = result->Fetch();
                time_t t_ban = fields[1].GetUInt64();
                tm* aTm_ban = localtime(&t_ban);
                if (fields[1].GetUInt64() == fields[2].GetUInt64())
                {
                    handler->PSendSysMessage("|%-15.15s|%02d-%02d-%02d %02d:%02d|   permanent  |%-15.15s|%-15.15s|",
                        fields[0].GetString(), aTm_ban->tm_year % 100, aTm_ban->tm_mon + 1, aTm_ban->tm_mday, aTm_ban->tm_hour, aTm_ban->tm_min,
                        fields[3].GetString(), fields[4].GetString());
                }
                else
                {
                    time_t t_unban = fields[2].GetUInt64();
                    tm* aTm_unban = localtime(&t_unban);
                    handler->PSendSysMessage("|%-15.15s|%02d-%02d-%02d %02d:%02d|%02d-%02d-%02d %02d:%02d|%-15.15s|%-15.15s|",
                        fields[0].GetString(), aTm_ban->tm_year % 100, aTm_ban->tm_mon + 1, aTm_ban->tm_mday, aTm_ban->tm_hour, aTm_ban->tm_min,
                        aTm_unban->tm_year % 100, aTm_unban->tm_mon + 1, aTm_unban->tm_mday, aTm_unban->tm_hour, aTm_unban->tm_min,
                        fields[3].GetString(), fields[4].GetString());
                }
            } while (result->NextRow());
            handler->SendSysMessage(" ===============================================================================");
        }

        return true;
    }

    static bool HandleBanListHelper(ChatHandler* handler, QueryResult_AutoPtr result)
    {
        handler->PSendSysMessage(LANG_BANLIST_MATCHINGACCOUNT);

        // Chat short output
        if (handler->GetSession())
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 accountid = fields[0].GetUInt32();

                QueryResult_AutoPtr banresult = LoginDatabase.PQuery("SELECT account.username FROM account,account_banned WHERE account_banned.id='%u' AND account_banned.id=account.id", accountid);
                if (banresult)
                {
                    Field* fields2 = banresult->Fetch();
                    handler->PSendSysMessage("%s", fields2[0].GetString());
                }
            } while (result->NextRow());
        }
        // Console wide output
        else
        {
            handler->SendSysMessage(LANG_BANLIST_ACCOUNTS);
            handler->SendSysMessage(" ===============================================================================");
            handler->SendSysMessage(LANG_BANLIST_ACCOUNTS_HEADER);
            do
            {
                handler->SendSysMessage("-------------------------------------------------------------------------------");
                Field* fields = result->Fetch();
                uint32 account_id = fields[0].GetUInt32();

                std::string account_name;

                // "account" case, name can be get in same query
                if (result->GetFieldCount() > 1)
                    account_name = fields[1].GetCppString();
                // "character" case, name need extract from another DB
                else
                    sAccountMgr->GetName(account_id, account_name);

                // No SQL injection. id is uint32.
                QueryResult_AutoPtr banInfo = LoginDatabase.PQuery("SELECT bandate,unbandate,bannedby,banreason FROM account_banned WHERE id = %u ORDER BY unbandate", account_id);
                if (banInfo)
                {
                    Field* fields2 = banInfo->Fetch();
                    do
                    {
                        time_t t_ban = fields2[0].GetUInt64();
                        tm* aTm_ban = localtime(&t_ban);

                        if (fields2[0].GetUInt64() == fields2[1].GetUInt64())
                        {
                            handler->PSendSysMessage("|%-15.15s|%02d-%02d-%02d %02d:%02d|   permanent  |%-15.15s|%-15.15s|",
                                account_name.c_str(), aTm_ban->tm_year % 100, aTm_ban->tm_mon + 1, aTm_ban->tm_mday, aTm_ban->tm_hour, aTm_ban->tm_min,
                                fields2[2].GetString(), fields2[3].GetString());
                        }
                        else
                        {
                            time_t t_unban = fields2[1].GetUInt64();
                            tm* aTm_unban = localtime(&t_unban);
                            handler->PSendSysMessage("|%-15.15s|%02d-%02d-%02d %02d:%02d|%02d-%02d-%02d %02d:%02d|%-15.15s|%-15.15s|",
                                account_name.c_str(), aTm_ban->tm_year % 100, aTm_ban->tm_mon + 1, aTm_ban->tm_mday, aTm_ban->tm_hour, aTm_ban->tm_min,
                                aTm_unban->tm_year % 100, aTm_unban->tm_mon + 1, aTm_unban->tm_mday, aTm_unban->tm_hour, aTm_unban->tm_min,
                                fields2[2].GetString(), fields2[3].GetString());
                        }
                    } while (banInfo->NextRow());
                }
            } while (result->NextRow());
            handler->SendSysMessage(" ===============================================================================");
        }
        return true;
    }

    static bool HandleBanInfoAccountCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* cname = strtok((char*)args, "");
        if (!cname)
            return false;

        std::string account_name = cname;
        if (!AccountMgr::normalizeString(account_name))
        {
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 accountid = sAccountMgr->GetId(account_name);
        if (!accountid)
        {
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
            return true;
        }

        return HandleBanInfoHelper(handler, accountid, account_name.c_str());
    }

    static bool HandleBanInfoCharacterCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* cname = strtok((char*)args, "");
        if (!cname)
            return false;

        std::string name = cname;
        if (!normalizePlayerName(name))
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 accountid = sObjectMgr.GetPlayerAccountIdByPlayerName(name);
        if (!accountid)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string accountname;
        if (!sAccountMgr->GetName(accountid, accountname))
        {
            handler->PSendSysMessage(LANG_BANINFO_NOCHARACTER);
            return true;
        }

        return HandleBanInfoHelper(handler, accountid, accountname.c_str());
    }

    static bool HandleBanInfoIPCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* cIP = strtok((char*)args, "");
        if (!cIP)
            return false;

        if (!IsIPAddress(cIP))
            return false;

        std::string IP = cIP;

        LoginDatabase.escape_string(IP);
        QueryResult_AutoPtr result = LoginDatabase.PQuery("SELECT ip, FROM_UNIXTIME(bandate), FROM_UNIXTIME(unbandate), unbandate-UNIX_TIMESTAMP(), banreason,bannedby,unbandate-bandate FROM ip_banned WHERE ip = '%s'", IP.c_str());
        if (!result)
        {
            handler->PSendSysMessage(LANG_BANINFO_NOIP);
            return true;
        }

        Field* fields = result->Fetch();
        bool permanent = !fields[6].GetUInt64();
        handler->PSendSysMessage(LANG_BANINFO_IPENTRY,
            fields[0].GetString(), fields[1].GetString(), permanent ? handler->GetOregonString(LANG_BANINFO_NEVER) : fields[2].GetString(),
            permanent ? handler->GetOregonString(LANG_BANINFO_INFINITE) : secsToTimeString(fields[3].GetUInt64(), true).c_str(), fields[4].GetString(), fields[5].GetString());

        return true;
    }

    static bool HandleBanInfoHelper(ChatHandler* handler, uint32 accountid, char const* accountname)
    {
        QueryResult_AutoPtr result = LoginDatabase.PQuery("SELECT FROM_UNIXTIME(bandate), unbandate-bandate, active, unbandate,banreason,bannedby FROM account_banned WHERE id = '%u' ORDER BY bandate ASC", accountid);
        if (!result)
        {
            handler->PSendSysMessage(LANG_BANINFO_NOACCOUNTBAN, accountname);
            return true;
        }

        handler->PSendSysMessage(LANG_BANINFO_BANHISTORY, accountname);
        do
        {
            Field* fields = result->Fetch();

            time_t unbandate = time_t(fields[3].GetUInt64());
            bool active = false;
            if (fields[2].GetBool() && (fields[1].GetUInt64() == (uint64)0 || unbandate >= time(NULL)))
                active = true;
            bool permanent = (fields[1].GetUInt64() == (uint64)0);
            std::string bantime = permanent ? handler->GetOregonString(LANG_BANINFO_INFINITE) : secsToTimeString(fields[1].GetUInt64(), true);
            handler->PSendSysMessage(LANG_BANINFO_HISTORYENTRY,
                fields[0].GetString(), bantime.c_str(), active ? handler->GetOregonString(LANG_BANINFO_YES) : handler->GetOregonString(LANG_BANINFO_NO), fields[4].GetString(), fields[5].GetString());
        } while (result->NextRow());

        return true;
    }

    static bool HandleBanAccountCommand(ChatHandler* handler, const char* args)
    {
        return HandleBanHelper(handler, BAN_ACCOUNT, args);
    }

    static bool HandleBanIPCommand(ChatHandler* handler, const char* args)
    {
        return HandleBanHelper(handler, BAN_IP, args);
    }

    static bool HandleBanHelper(ChatHandler* handler, BanMode mode, const char* args)
    {
        if (!*args)
            return false;

        char* cnameOrIP = strtok((char*)args, " ");
        if (!cnameOrIP)
            return false;

        std::string nameOrIP = cnameOrIP;

        char* duration = strtok(NULL, " ");
        if (!duration || !atoi(duration))
            return false;

        char* reason = strtok(NULL, "");
        if (!reason)
            return false;

        switch (mode)
        {
        case BAN_ACCOUNT:
            if (!AccountMgr::normalizeString(nameOrIP))
            {
                handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, nameOrIP.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }
            break;
        case BAN_CHARACTER:
            if (!normalizePlayerName(nameOrIP))
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }
            break;
        case BAN_IP:
            if (!IsIPAddress(nameOrIP.c_str()))
                return false;
            break;
        }

        switch (sWorld.BanAccount(mode, nameOrIP, duration, reason, handler->GetSession() ? handler->GetSession()->GetPlayerName() : ""))
        {
        case BAN_SUCCESS:
            if (atoi(duration) > 0)
                handler->PSendSysMessage(LANG_BAN_YOUBANNED, nameOrIP.c_str(), secsToTimeString(TimeStringToSecs(duration), true).c_str(), reason);
            else
                handler->PSendSysMessage(LANG_BAN_YOUPERMBANNED, nameOrIP.c_str(), reason);
            break;
        case BAN_SYNTAX_ERROR:
            return false;
        case BAN_NOTFOUND:
            switch (mode)
            {
            default:
                handler->PSendSysMessage(LANG_BAN_NOTFOUND, "account", nameOrIP.c_str());
                break;
            case BAN_CHARACTER:
                handler->PSendSysMessage(LANG_BAN_NOTFOUND, "character", nameOrIP.c_str());
                break;
            case BAN_IP:
                handler->PSendSysMessage(LANG_BAN_NOTFOUND, "ip", nameOrIP.c_str());
                break;
            }
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }
};

void AddSC_ban_commandscript()
{
    new ban_commandscript();
}