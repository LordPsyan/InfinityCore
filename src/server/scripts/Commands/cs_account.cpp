/*
 * This file is part of the OregonCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "AccountMgr.h"
#include "Language.h"
#include "World.h"
#include "Player.h"
#include "Opcodes.h"

class account_commandscript : public CommandScript
{
public:
    account_commandscript() : CommandScript("account_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> accountSetCommandTable =
        {
            { "addon",          SEC_ADMINISTRATOR,  true,  &HandleAccountSetAddonCommand,     "" },
            { "gmlevel",        SEC_CONSOLE,        true,  &HandleAccountSetGmLevelCommand,   "" },
            { "password",       SEC_CONSOLE,        true,  &HandleAccountSetPasswordCommand,  "" },
            { "2fa",            SEC_PLAYER,         true,  &HandleAccountSet2FACommand,       "" },       
        };

        static std::vector<ChatCommand> accountCommandTable =
        {
            { "create",         SEC_CONSOLE,        true,  &HandleAccountCreateCommand,              "" },
            { "delete",         SEC_CONSOLE,        true,  &HandleAccountDeleteCommand,        "" },
            { "onlinelist",     SEC_CONSOLE,        true,  &HandleAccountOnlineListCommand,          "" },
            { "lock",           SEC_PLAYER,         false, &HandleAccountLockCommand,             "" },
            { "set",            SEC_ADMINISTRATOR,  true,  nullptr,                           "", accountSetCommandTable},
            { "password",       SEC_PLAYER,         false, &HandleAccountPasswordCommand,             "" },
            { "",               SEC_PLAYER,         false, &HandleAccountCommand,             "" }
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "account",        SEC_PLAYER,         false, nullptr,                           "", accountCommandTable }
        };
        return commandTable;
    };

    static bool HandleAccountCommand(ChatHandler* handler, const char* /*args*/)
    {
        uint32 gmlevel = handler->GetSession()->GetSecurity();
        handler->PSendSysMessage(LANG_ACCOUNT_LEVEL, gmlevel);
        return true;
    }

    static bool HandleAccountSet2FACommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string accountName = strtok((char*)args, " ");
        std::string secret = strtok(NULL, " ");

        if (handler->GetSession())
        {
            if (handler->GetSession()->GetSecurity() < SEC_ADMINISTRATOR)
                sAccountMgr->GetName(handler->GetSession()->GetAccountId(), accountName);
        }
        else if (accountName.empty() || secret.empty())
            return false;

        uint32 targetAccountId = NULL;

        if (handler->GetSession())
        {
            if (handler->GetSession()->GetSecurity() == SEC_ADMINISTRATOR)
                targetAccountId = sAccountMgr->GetId(accountName.c_str());
            else
                targetAccountId = handler->GetSession()->GetAccountId();
        }
        else
            targetAccountId = sAccountMgr->GetId(accountName.c_str());

        if (!targetAccountId)
        {
            handler->PSendSysMessage("Account %s does not exist", accountName.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (secret == "off")
        {
            LoginDatabase.PQuery("UPDATE `account` SET `token_key` = '', `security_flag` = '0' WHERE `id` = '%u';", targetAccountId);
            handler->PSendSysMessage("Successfully removed 2FA for account %s", accountName.c_str());
            return true;
        }

        switch (secret.size())
        {
        case 6: // Pin
        {
            //Check string only contains numbers from 0-9
            std::size_t found = secret.find_first_not_of("0123456789 ");

            if (found != std::string::npos)
            {
                handler->PSendSysMessage("Please only use numbers from 0-9");
                return false;
            }

            // Players should be allowed to set this ingame.
            LoginDatabase.PQuery("UPDATE `account` SET `token_key` = '%u', `security_flag` = '1' WHERE `id` = '%u';", atoi(secret.c_str()), targetAccountId);
            handler->PSendSysMessage("Account %s has been successfully updated with [PIN] \nYour pin is %u",accountName.c_str(), atoi(secret.c_str()));
            break;
        }
        case 16: // TOTP
        {
            if (handler->GetSession())
                if (sAccountMgr->GetSecurity(handler->GetSession()->GetAccountId()) < SEC_ADMINISTRATOR)
                    return false;

            LoginDatabase.PQuery("UPDATE `account` SET `token_key` = '%s', `security_flag` = '4' WHERE `id` = '%u';", secret.c_str(), targetAccountId);
            handler->PSendSysMessage("Account %s has been successfully updated with [TOTP]", accountName.c_str());
            break;
        }
        default:
            handler->PSendSysMessage("[PIN] Please use 6 digit numberic value \n[TOTP] Please use 16 Digit value");
            return false;
            break;
        }

        return true;
    }

    static bool HandleAccountPasswordCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* old_pass = strtok((char*)args, " ");
        char* new_pass = strtok(NULL, " ");
        char* new_pass_c = strtok(NULL, " ");

        if (!old_pass || !new_pass || !new_pass_c)
            return false;

        std::string password_old = old_pass;
        std::string password_new = new_pass;
        std::string password_new_c = new_pass_c;

        if (strcmp(new_pass, new_pass_c) != 0)
        {
            handler->SendSysMessage(LANG_NEW_PASSWORDS_NOT_MATCH);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!sAccountMgr->CheckPassword(handler->GetSession()->GetAccountId(), password_old))
        {
            handler->SendSysMessage(LANG_COMMAND_WRONGOLDPASSWORD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        AccountOpResult result = sAccountMgr->ChangePassword(handler->GetSession()->GetAccountId(), password_new);

        switch (result)
        {
        case AOR_OK:
            handler->SendSysMessage(LANG_COMMAND_PASSWORD);
            break;
        case AOR_PASS_TOO_LONG:
            handler->SendSysMessage(LANG_PASSWORD_TOO_LONG);
            handler->SetSentErrorMessage(true);
            return false;
        case AOR_NAME_NOT_EXIST:                            // not possible case, don't want get account name for output
        default:
            handler->SendSysMessage(LANG_COMMAND_NOTCHANGEPASSWORD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    static bool HandleAccountLockCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
        {
            handler->SendSysMessage(LANG_USE_BOL);
            return true;
        }

        std::string argstr = (char*)args;
        if (argstr == "on")
        {
            LoginDatabase.PExecute("UPDATE account SET locked = '1' WHERE id = '%d'", handler->GetSession()->GetAccountId());
            handler->PSendSysMessage(LANG_COMMAND_ACCLOCKLOCKED);
            return true;
        }

        if (argstr == "off")
        {
            LoginDatabase.PExecute("UPDATE account SET locked = '0' WHERE id = '%d'", handler->GetSession()->GetAccountId());
            handler->PSendSysMessage(LANG_COMMAND_ACCLOCKUNLOCKED);
            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        return true;
    }

    // Display info on users currently in the realm
    static bool HandleAccountOnlineListCommand(ChatHandler* handler, const char* /*args*/)
    {
        // Get the list of accounts ID logged to the realm
        QueryResult_AutoPtr resultDB = CharacterDatabase.Query("SELECT name,account FROM characters WHERE online > 0");
        if (!resultDB)
            return true;

        // Display the list of account/characters online
        handler->SendSysMessage("=====================================================================");
        handler->SendSysMessage(LANG_ACCOUNT_LIST_HEADER);
        handler->SendSysMessage("=====================================================================");

        // Circle through accounts
        do
        {
            Field* fieldsDB = resultDB->Fetch();
            std::string name = fieldsDB[0].GetCppString();
            uint32 account = fieldsDB[1].GetUInt32();

            // Get the username, last IP and GM level of each account
            // No SQL injection. account is uint32.
            QueryResult_AutoPtr resultLogin =
                LoginDatabase.PQuery("SELECT a.username, a.last_ip, aa.gmlevel, a.expansion "
                    "FROM account a "
                    "LEFT JOIN account_access aa "
                    "ON (a.id = aa.id) "
                    "WHERE a.id = '%u'", account);
            if (resultLogin)
            {
                Field* fieldsLogin = resultLogin->Fetch();
                handler->PSendSysMessage("|%15s| %20s | %15s |%4d|%5d|",
                    fieldsLogin[0].GetString(), name.c_str(), fieldsLogin[1].GetString(), fieldsLogin[2].GetUInt32(), fieldsLogin[3].GetUInt32());
            }
            else
                handler->PSendSysMessage(LANG_ACCOUNT_LIST_ERROR, name.c_str());

        } while (resultDB->NextRow());

        handler->SendSysMessage("=====================================================================");
        return true;
    }

    static bool HandleAccountDeleteCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        ///- Get the account name from the command line
        char* account_name_str = strtok((char*)args, " ");
        if (!account_name_str)
            return false;

        std::string account_name = account_name_str;
        if (!AccountMgr::normalizeString(account_name))
        {
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 account_id = sAccountMgr->GetId(account_name);
        if (!account_id)
        {
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Commands not recommended call from chat, but support anyway
        if (handler->GetSession())
        {
            uint32 targetSecurity = sAccountMgr->GetSecurity(account_id);

            // can delete only for account with less security
            // This is also reject self apply in fact
            if (targetSecurity >= handler->GetSession()->GetSecurity())
            {
                handler->SendSysMessage(LANG_YOURS_SECURITY_IS_LOW);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        AccountOpResult result = sAccountMgr->DeleteAccount(account_id);
        switch (result)
        {
        case AOR_OK:
            handler->PSendSysMessage(LANG_ACCOUNT_DELETED, account_name.c_str());
            break;
        case AOR_NAME_NOT_EXIST:
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        case AOR_DB_INTERNAL_ERROR:
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_DELETED_SQL_ERROR, account_name.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        default:
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_DELETED, account_name.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    // Create an account
    static bool HandleAccountCreateCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // Parse the command line arguments
        char* szAcc = strtok((char*)args, " ");
        char* szPassword = strtok(NULL, " ");
        if (!szAcc || !szPassword)
            return false;

        // normalized in sAccountMgr->CreateAccount
        std::string account_name = szAcc;
        std::string password = szPassword;

        AccountOpResult result = sAccountMgr->CreateAccount(account_name, password);
        switch (result)
        {
        case AOR_OK:
            handler->PSendSysMessage(LANG_ACCOUNT_CREATED, account_name.c_str());
            break;
        case AOR_NAME_TOO_LONG:
            handler->SendSysMessage(LANG_ACCOUNT_TOO_LONG);
            handler->SetSentErrorMessage(true);
            return false;
        case AOR_NAME_ALREDY_EXIST:
            handler->SendSysMessage(LANG_ACCOUNT_ALREADY_EXIST);
            handler->SetSentErrorMessage(true);
            return false;
        case AOR_DB_INTERNAL_ERROR:
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_CREATED_SQL_ERROR, account_name.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        default:
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_CREATED, account_name.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    // Set password for account
    static bool HandleAccountSetPasswordCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // Get the command line arguments
        char* szAccount = strtok((char*)args, " ");
        char* szPassword1 = strtok(NULL, " ");
        char* szPassword2 = strtok(NULL, " ");

        if (!szAccount || !szPassword1 || !szPassword2)
            return false;

        std::string account_name = szAccount;
        if (!AccountMgr::normalizeString(account_name))
        {
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 targetAccountId = sAccountMgr->GetId(account_name);
        if (!targetAccountId)
        {
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 targetSecurity = sAccountMgr->GetSecurity(targetAccountId);

        // m_session == NULL only for console
        uint32 plSecurity = handler->GetSession() ? handler->GetSession()->GetSecurity() : uint32(SEC_CONSOLE);

        // can set password only for target with less security
        // This is also reject self apply in fact
        if (targetSecurity >= plSecurity)
        {
            handler->SendSysMessage(LANG_YOURS_SECURITY_IS_LOW);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (strcmp(szPassword1, szPassword2))
        {
            handler->SendSysMessage(LANG_NEW_PASSWORDS_NOT_MATCH);
            handler->SetSentErrorMessage(true);
            return false;
        }

        AccountOpResult result = sAccountMgr->ChangePassword(targetAccountId, szPassword1);

        switch (result)
        {
        case AOR_OK:
            handler->SendSysMessage(LANG_COMMAND_PASSWORD);
            break;
        case AOR_NAME_NOT_EXIST:
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        case AOR_PASS_TOO_LONG:
            handler->SendSysMessage(LANG_PASSWORD_TOO_LONG);
            handler->SetSentErrorMessage(true);
            return false;
        default:
            handler->SendSysMessage(LANG_COMMAND_NOTCHANGEPASSWORD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    static bool HandleAccountSetGmLevelCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string targetAccountName;
        uint32 targetAccountId = 0;
        uint32 targetSecurity = 0;
        int32 gm = 0;
        char* arg1 = strtok((char*)args, " ");
        char* arg2 = strtok(NULL, " ");
        char* arg3 = strtok(NULL, " ");

        if (handler->getSelectedPlayer() && arg1 && !arg3)
        {
            targetAccountId = handler->getSelectedPlayer()->GetSession()->GetAccountId();
            sAccountMgr->GetName(targetAccountId, targetAccountName);
            Player* targetPlayer = handler->getSelectedPlayer();
            gm = atoi(arg1);
            uint32 gmRealmID = arg2 ? strtoul(arg2, NULL, 10) : realmID;

            // Check for invalid specified GM level.
            if (gm < SEC_PLAYER || gm > SEC_ADMINISTRATOR)
            {
                handler->SendSysMessage(LANG_BAD_VALUE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            // Check if targets GM level and specified GM level is not higher than current gm level
            targetSecurity = targetPlayer->GetSession()->GetSecurity();
            if (targetSecurity >= handler->GetSession()->GetSecurity() ||
                uint32(gm) >= handler->GetSession()->GetSecurity() ||
                (gmRealmID != realmID && handler->GetSession()->GetSecurity() < SEC_CONSOLE))
            {
                handler->SendSysMessage(LANG_YOURS_SECURITY_IS_LOW);
                handler->SetSentErrorMessage(true);
                return false;
            }

            // Decide which string to show
            if (handler->GetSession()->GetPlayer() != targetPlayer)
                handler->PSendSysMessage(LANG_YOU_CHANGE_SECURITY, targetAccountName.c_str(), gm);
            else
                handler->PSendSysMessage(LANG_YOURS_SECURITY_CHANGED, handler->GetSession()->GetPlayer()->GetName(), gm);

            // If gmRealmID is -1, delete all values for the account id, else, insert values for the specific realmID
            if (gmRealmID == uint32(-1))
            {
                LoginDatabase.PExecute("DELETE FROM account_access WHERE id = '%u'", targetAccountId);
                LoginDatabase.PExecute("INSERT INTO account_access VALUES ('%u', '%d', -1)", targetAccountId, gm);
            }
            else
            {
                LoginDatabase.PExecute("DELETE FROM account_access WHERE id = '%u' AND RealmID = '%d'", targetAccountId, realmID);
                LoginDatabase.PExecute("INSERT INTO account_access VALUES ('%u','%d','%d')", targetAccountId, gm, realmID);
            }
            return true;
        }
        else
        {
            // Check for second parameter
            if (!arg2)
                return false;

            // Check for account
            targetAccountName = arg1;
            if (!AccountMgr::normalizeString(targetAccountName))
            {
                handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, targetAccountName.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }

            // Check for username not exist
            targetAccountId = sAccountMgr->GetId(targetAccountName);
            if (!targetAccountId)
            {
                handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, targetAccountName.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }

            // Check for invalid specified GM level.
            gm = atoi(arg2);
            if (gm < SEC_PLAYER || gm > SEC_ADMINISTRATOR)
            {
                handler->SendSysMessage(LANG_BAD_VALUE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 gmRealmID = arg3 ? atoi(arg3) : realmID;
            // Check if provided realmID is not current realmID, or isn't -1
            if (gmRealmID != realmID && gmRealmID != uint32(-1))
            {
                handler->SendSysMessage(LANG_INVALID_REALMID);
                handler->SetSentErrorMessage(true);
                return false;
            }

            targetAccountId = sAccountMgr->GetId(arg1);
            // m_session == NULL only for console
            uint32 plSecurity = handler->GetSession() ? handler->GetSession()->GetSecurity() : uint32(SEC_CONSOLE);

            // can set security level only for target with less security and to less security that we have
            // This is also reject self apply in fact
            targetSecurity = sAccountMgr->GetSecurity(targetAccountId);
            if (targetSecurity >= plSecurity || uint32(gm) >= plSecurity)
            {
                handler->SendSysMessage(LANG_YOURS_SECURITY_IS_LOW);
                handler->SetSentErrorMessage(true);
                return false;
            }

            handler->PSendSysMessage(LANG_YOU_CHANGE_SECURITY, targetAccountName.c_str(), gm);
            // If gmRealmID is -1, delete all values for the account id, else, insert values for the specific realmID
            if (gmRealmID == uint32(-1))
            {
                LoginDatabase.PExecute("DELETE FROM account_access WHERE id = '%u'", targetAccountId);
                LoginDatabase.PExecute("INSERT INTO account_access VALUES ('%u', '%d', -1)", targetAccountId, gm);
            }
            else
            {
                LoginDatabase.PExecute("DELETE FROM account_access WHERE id = '%u' AND RealmID = '%d'", targetAccountId, realmID);
                LoginDatabase.PExecute("INSERT INTO account_access VALUES ('%u','%d','%d')", targetAccountId, gm, realmID);
            }
            return true;
        }
    }

    // Set/Unset the expansion level for an account
    static bool HandleAccountSetAddonCommand(ChatHandler* handler, const char* args)
    {
        // Get the command line arguments
        char* szAcc = strtok((char*)args, " ");
        char* szExp = strtok(NULL, " ");

        if (!szAcc)
            return false;

        std::string account_name;
        uint32 account_id;

        if (!szExp)
        {
            Player* player = handler->getSelectedPlayer();
            if (!player)
                return false;

            account_id = player->GetSession()->GetAccountId();
            sAccountMgr->GetName(account_id, account_name);
            szExp = szAcc;
        }
        else
        {
            // Convert Account name to Upper Format
            account_name = szAcc;
            if (!AccountMgr::normalizeString(account_name))
            {
                handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }

            account_id = sAccountMgr->GetId(account_name);
            if (!account_id)
            {
                handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        int lev = atoi(szExp);                                  //get int anyway (0 if error)
        if (lev < 0)
            return false;

        // No SQL injection
        LoginDatabase.PExecute("UPDATE account SET expansion = '%d' WHERE id = '%u'", lev, account_id);
        handler->PSendSysMessage(LANG_ACCOUNT_SETADDON, account_name.c_str(), account_id, lev);
        return true;
    }


};

void AddSC_account_commandscript()
{
    new account_commandscript();
}