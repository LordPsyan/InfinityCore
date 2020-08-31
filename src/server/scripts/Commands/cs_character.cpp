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

class character_commandscript : public CommandScript
{
public:
    character_commandscript() : CommandScript("character_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> characterDeletedCommandTable =
        {
            { "delete",         SEC_GAMEMASTER,     false, &HandleCharacterDeletedDeleteCommand,    "" },
            { "list",           SEC_GAMEMASTER,     false, &HandleCharacterDeletedListCommand,           "" },
            { "restore",        SEC_GAMEMASTER,     false, &HandleCharacterDeletedRestoreCommand,           "" },
            { "old",            SEC_GAMEMASTER,     false, &HandleCharacterDeletedOldCommand,           "" },
        };
        static std::vector<ChatCommand> characterCommandTable =
        {
            { "rename",         SEC_GAMEMASTER,     false, &HandleCharacterRenameCommand,           "" },
            { "titles",         SEC_GAMEMASTER,     false, &HandleCharacterTitlesCommand,           "" },
            { "erase",          SEC_CONSOLE,        false, &HandleCharacterEraseCommand,            "" },
            { "deleted",        SEC_GAMEMASTER,     false, nullptr,                                 "" },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "character",         SEC_MODERATOR,      false, nullptr,                                 "", characterCommandTable }
        };
        return commandTable;
    };

    static bool HandleCharacterDeletedOldCommand(ChatHandler* handler, char const* args)
    {
        int32 keepDays = sWorld.getConfig(CONFIG_CHARDELETE_KEEP_DAYS);

        char* px = strtok((char*)args, " ");
        if (px)
        {
            if (!isNumeric(px))
                return false;

            keepDays = atoi(px);
            if (keepDays < 0)
                return false;
        }
        // config option value 0 -> disabled and can't be used
        else if (keepDays <= 0)
            return false;

        Player::DeleteOldCharacters((uint32)keepDays);
        return true;
    }

    static bool HandleCharacterDeletedRestoreCommand(ChatHandler* handler, const char* args)
    {
        // It is required to submit at least one argument
        if (!*args)
            return false;

        std::string searchString;
        std::string newCharName;
        uint32 newAccount = 0;

        // GCC by some strange reason fail build code without temporary variable
        std::istringstream params(args);
        params >> searchString >> newCharName >> newAccount;

        DeletedInfoList foundList;
        if (!GetDeletedCharacterInfoList(foundList, searchString))
            return false;

        if (foundList.empty())
        {
            handler->SendSysMessage(LANG_CHARACTER_DELETED_LIST_EMPTY);
            return false;
        }

        handler->SendSysMessage(LANG_CHARACTER_DELETED_RESTORE);
        HandleCharacterDeletedListHelper(handler, foundList);

        if (newCharName.empty())
        {
            // Drop not existed account cases
            for (DeletedInfoList::iterator itr = foundList.begin(); itr != foundList.end(); ++itr)
                HandleCharacterDeletedRestoreHelper(handler, *itr);
        }
        else if (foundList.size() == 1 && normalizePlayerName(newCharName))
        {
            DeletedInfo delInfo = foundList.front();

            // update name
            delInfo.name = newCharName;

            // if new account provided update deleted info
            if (newAccount && newAccount != delInfo.accountId)
            {
                delInfo.accountId = newAccount;
                sAccountMgr->GetName(newAccount, delInfo.accountName);
            }

            HandleCharacterDeletedRestoreHelper(handler, delInfo);
        }
        else
            handler->SendSysMessage(LANG_CHARACTER_DELETED_ERR_RENAME);

        return true;
    }

    static bool HandleCharacterDeletedListCommand(ChatHandler* handler, const char* args)
    {
        DeletedInfoList foundList;
        if (!GetDeletedCharacterInfoList(foundList, args))
            return false;

        // if no characters have been found, output a warning
        if (foundList.empty())
        {
            handler->SendSysMessage(LANG_CHARACTER_DELETED_LIST_EMPTY);
            return false;
        }

        HandleCharacterDeletedListHelper(handler, foundList);
        return true;
    }

    static bool HandleCharacterDeletedDeleteCommand(ChatHandler* handler, const char* args)
    {
        // It is required to submit at least one argument
        if (!*args)
            return false;

        DeletedInfoList foundList;
        if (!GetDeletedCharacterInfoList(foundList, args))
            return false;

        if (foundList.empty())
        {
            handler->SendSysMessage(LANG_CHARACTER_DELETED_LIST_EMPTY);
            return false;
        }

        handler->SendSysMessage(LANG_CHARACTER_DELETED_DELETE);
        HandleCharacterDeletedListHelper(handler, foundList);

        // Call the appropriate function to delete them (current account for deleted characters is 0)
        for (DeletedInfoList::const_iterator itr = foundList.begin(); itr != foundList.end(); ++itr)
            Player::DeleteFromDB(itr->lowguid, 0, false, true);

        return true;
    }

    static bool HandleCharacterEraseCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* character_name_str = strtok((char*)args, " ");
        if (!character_name_str)
            return false;

        std::string character_name = character_name_str;
        if (!normalizePlayerName(character_name))
            return false;

        uint64 character_guid;
        uint32 account_id;

        Player* player = sObjectMgr.GetPlayer(character_name.c_str());
        if (player)
        {
            character_guid = player->GetGUID();
            account_id = player->GetSession()->GetAccountId();
            player->GetSession()->KickPlayer();
        }
        else
        {
            character_guid = sObjectMgr.GetPlayerGUIDByName(character_name);
            if (!character_guid)
            {
                handler->PSendSysMessage(LANG_NO_PLAYER, character_name.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }

            account_id = sObjectMgr.GetPlayerAccountIdByGUID(character_guid);
        }

        std::string account_name;
        sAccountMgr->GetName(account_id, account_name);

        Player::DeleteFromDB(character_guid, account_id, true);
        handler->PSendSysMessage(LANG_CHARACTER_DELETED, character_name.c_str(), GUID_LOPART(character_guid), account_name.c_str(), account_id);
        return true;
    }

    static bool HandleCharacterTitlesCommand(ChatHandler* handler, char const* args)
    {
        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        LocaleConstant loc = handler->GetSession()->GetSessionDbcLocale();
        char const* targetName = target->GetName();
        char const* knownStr = handler->GetOregonString(LANG_KNOWN);

        // Search in CharTitles.dbc
        for (uint32 id = 0; id < sCharTitlesStore.GetNumRows(); id++)
        {
            CharTitlesEntry const* titleInfo = sCharTitlesStore.LookupEntry(id);
            if (titleInfo && target->HasTitle(titleInfo))
            {
                std::string name = titleInfo->name[loc];
                if (name.empty())
                    continue;

                char const* activeStr = target && target->GetUInt32Value(PLAYER_CHOSEN_TITLE) == titleInfo->bit_index
                    ? handler->GetOregonString(LANG_ACTIVE)
                    : "";

                char titleNameStr[80];
                snprintf(titleNameStr, 80, name.c_str(), targetName);

                // send title in "id (idx:idx) - [namedlink locale]" format
                if (handler->GetSession())
                    handler->PSendSysMessage(LANG_TITLE_LIST_CHAT, id, titleInfo->bit_index, id, titleNameStr, localeNames[loc], knownStr, activeStr);
                else
                    handler->PSendSysMessage(LANG_TITLE_LIST_CONSOLE, id, titleInfo->bit_index, name.c_str(), localeNames[loc], knownStr, activeStr);
            }
        }
        return true;
    }

    static bool HandleCharacterRenameCommand(ChatHandler* handler, char const* args)
    {
        Player* target = NULL;
        uint64 targetGUID = 0;
        std::string oldname;

        char* px = strtok((char*)args, " ");

        if (px)
        {
            oldname = px;

            if (!normalizePlayerName(oldname))
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            target = sObjectMgr.GetPlayer(oldname.c_str());

            if (!target)
                targetGUID = sObjectMgr.GetPlayerGUIDByName(oldname);
        }

        if (!target && !targetGUID)
            target = handler->getSelectedPlayer();

        if (!target && !targetGUID)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target)
        {
            handler->PSendSysMessage(LANG_RENAME_PLAYER, target->GetName());
            target->SetAtLoginFlag(AT_LOGIN_RENAME);
        }
        else
        {
            handler->PSendSysMessage(LANG_RENAME_PLAYER_GUID, oldname.c_str(), GUID_LOPART(targetGUID));
            CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '1' WHERE guid = '%u'", GUID_LOPART(targetGUID));
        }

        return true;
    }
private:
    /**
     * Stores informations about a deleted character
     */
    struct DeletedInfo
    {
        uint32      lowguid;                            ///< the low GUID from the character
        std::string name;                               ///< the character name
        uint32      accountId;                          ///< the account id
        std::string accountName;                        ///< the account name
        time_t      deleteDate;                         ///< the date at which the character has been deleted
    };

    typedef std::list<DeletedInfo> DeletedInfoList;
    static bool GetDeletedCharacterInfoList(DeletedInfoList& foundList, std::string searchString = "")
    {
        QueryResult_AutoPtr resultChar;
        if (!searchString.empty())
        {
            // search by GUID
            if (isNumeric(searchString.c_str()))
                resultChar = CharacterDatabase.PQuery("SELECT guid, deleteInfos_Name, deleteInfos_Account, deleteDate FROM characters WHERE deleteDate IS NOT NULL AND guid = %llu", uint64(atoi(searchString.c_str())));
            // search by name
            else
            {
                if (!normalizePlayerName(searchString))
                    return false;

                resultChar = CharacterDatabase.PQuery("SELECT guid, deleteInfos_Name, deleteInfos_Account, deleteDate FROM characters WHERE deleteDate IS NOT NULL AND deleteInfos_Name " _LIKE_ " " _CONCAT3_("'%%'", "'%s'", "'%%'"), searchString.c_str());
            }
        }
        else
            resultChar = CharacterDatabase.Query("SELECT guid, deleteInfos_Name, deleteInfos_Account, deleteDate FROM characters WHERE deleteDate IS NOT NULL");

        if (resultChar)
        {
            do
            {
                Field* fields = resultChar->Fetch();

                DeletedInfo info;

                info.lowguid = fields[0].GetUInt32();
                info.name = fields[1].GetCppString();
                info.accountId = fields[2].GetUInt32();

                // account name will be empty for not existed account
                sAccountMgr->GetName(info.accountId, info.accountName);

                info.deleteDate = time_t(fields[3].GetUInt64());

                foundList.push_back(info);
            } while (resultChar->NextRow());
        }

        return true;
    }
    std::string GenerateDeletedCharacterGUIDsWhereStr(DeletedInfoList::const_iterator& itr, DeletedInfoList::const_iterator const& itr_end)
    {
        std::ostringstream wherestr;
        wherestr << "guid IN ('";
        for (; itr != itr_end; ++itr)
        {
            wherestr << itr->lowguid;

            if (wherestr.str().size() > MAX_QUERY_LEN - 50)     // near to max query
            {
                ++itr;
                break;
            }

            DeletedInfoList::const_iterator itr2 = itr;
            if (++itr2 != itr_end)
                wherestr << "','";
        }
        wherestr << "')";
        return wherestr.str();
    }
    static void HandleCharacterDeletedListHelper(ChatHandler* handler, DeletedInfoList const& foundList)
    {
        if (!handler->GetSession())
        {
            handler->SendSysMessage(LANG_CHARACTER_DELETED_LIST_BAR);
            handler->SendSysMessage(LANG_CHARACTER_DELETED_LIST_HEADER);
            handler->SendSysMessage(LANG_CHARACTER_DELETED_LIST_BAR);
        }

        for (DeletedInfoList::const_iterator itr = foundList.begin(); itr != foundList.end(); ++itr)
        {
            std::string dateStr = TimeToTimestampStr(itr->deleteDate);

            if (!handler->GetSession())
                handler->PSendSysMessage(LANG_CHARACTER_DELETED_LIST_LINE_CONSOLE,
                    itr->lowguid, itr->name.c_str(), itr->accountName.empty() ? "<Not existed>" : itr->accountName.c_str(),
                    itr->accountId, dateStr.c_str());
            else
                handler->PSendSysMessage(LANG_CHARACTER_DELETED_LIST_LINE_CHAT,
                    itr->lowguid, itr->name.c_str(), itr->accountName.empty() ? "<Not existed>" : itr->accountName.c_str(),
                    itr->accountId, dateStr.c_str());
        }

        if (!handler->GetSession())
            handler->SendSysMessage(LANG_CHARACTER_DELETED_LIST_BAR);
    }
    static void HandleCharacterDeletedRestoreHelper(ChatHandler* handler, DeletedInfo const& delInfo)
    {
        if (delInfo.accountName.empty())                    // account not exist
        {
            handler->PSendSysMessage(LANG_CHARACTER_DELETED_SKIP_ACCOUNT, delInfo.name.c_str(), delInfo.lowguid, delInfo.accountId);
            return;
        }

        // check character count
        uint32 charcount = sAccountMgr->GetCharactersCount(delInfo.accountId);
        if (charcount >= 10)
        {
            handler->PSendSysMessage(LANG_CHARACTER_DELETED_SKIP_FULL, delInfo.name.c_str(), delInfo.lowguid, delInfo.accountId);
            return;
        }

        if (sObjectMgr.GetPlayerGUIDByName(delInfo.name))
        {
            handler->PSendSysMessage(LANG_CHARACTER_DELETED_SKIP_NAME, delInfo.name.c_str(), delInfo.lowguid, delInfo.accountId);
            return;
        }

        CharacterDatabase.PExecute("UPDATE characters SET name='%s', account='%u', deleteDate=NULL, deleteInfos_Name=NULL, deleteInfos_Account=NULL WHERE deleteDate IS NOT NULL AND guid = %u",
            delInfo.name.c_str(), delInfo.accountId, delInfo.lowguid);
    }
};

void AddSC_character_commandscript()
{
    new character_commandscript();
}