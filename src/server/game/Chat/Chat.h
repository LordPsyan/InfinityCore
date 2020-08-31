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

#ifndef OREGONCORE_CHAT_H
#define OREGONCORE_CHAT_H

#include "SharedDefines.h"

class ChatHandler;
class WorldSession;
class Creature;
class Player;
class Unit;
struct GameTele;

class ChatCommand
{
    typedef bool(*pHandler)(ChatHandler*, char const*);

public:
    ChatCommand(char const* name, uint32 securityLevel, bool allowConsole, pHandler handler, std::string help, std::vector<ChatCommand> childCommands = std::vector<ChatCommand>())
        : Name(name), SecurityLevel(securityLevel), AllowConsole(allowConsole), Handler(handler), Help(std::move(help)), ChildCommands(std::move(childCommands)) { }

    const char* Name;
    uint32 SecurityLevel;                   
    bool AllowConsole;
    pHandler Handler;
    std::string Help;
    std::vector<ChatCommand> ChildCommands;
};

class ChatHandler
{
public:
    WorldSession * GetSession() { return m_session; }
    explicit ChatHandler(WorldSession* session) : m_session(session), sentErrorMessage(false) { }
    explicit ChatHandler(Player* player) : m_session(player->GetSession()) { }
    ~ChatHandler() {}

    static void FillMessageData(WorldPacket* data, WorldSession* session, uint8 type, uint32 language, const char* channelName, uint64 target_guid, const char* message, Unit* speaker);

    void FillMessageData(WorldPacket* data, uint8 type, uint32 language, uint64 target_guid, const char* message)
    {
        FillMessageData(data, m_session, type, language, NULL, target_guid, message, NULL);
    }

    void FillSystemMessageData(WorldPacket* data, const char* message)
    {
        FillMessageData(data, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, 0, message);
    }

    static char* LineFromMessage(char*& pos)
    {
        char* start = strtok(pos, "\n");
        pos = NULL;
        return start;
    }

    // function with different implementation for chat/console
    virtual const char* GetOregonString(int32 entry) const;
    virtual void SendSysMessage(const char* str);

    char* extractQuotedArg(char* args);

    void SendSysMessage(int32     entry);
    void PSendSysMessage(const char* format, ...) ATTR_PRINTF(2, 3);
    void PSendSysMessage(int32     entry, ...);
    std::string PGetParseString(int32 entry, ...);
    std::string GetTimeString(uint32 time);

    int ParseCommands(const char* text);

    static std::vector<ChatCommand> const& getCommandTable();

    bool isValidChatMessage(const char* msg);
    bool HasSentErrorMessage()
    {
        return sentErrorMessage;
    }
    virtual char const* GetName() const;

    static bool LoadCommandTable() { return load_command_table; }
    static void SetLoadCommandTable(bool val) { load_command_table = val; }
    bool ShowHelpForCommand(std::vector<ChatCommand> const& table, const char* cmd);
    bool ShowHelpForSubCommands(std::vector<ChatCommand> const& table, char const* cmd, char const* subcmd);

    void SetSentErrorMessage(bool val)
    {
        sentErrorMessage = val;
    };

    Player*   getSelectedPlayer();
    Player*   getSelectedPlayerOrSelf();
    Creature* getSelectedCreature();
    Unit*     getSelectedUnit();

    struct DeletedInfo
    {
        uint32      lowguid;                            ///< the low GUID from the character
        std::string name;                               ///< the character name
        uint32      accountId;                          ///< the account id
        std::string accountName;                        ///< the account name
        time_t      deleteDate;                         ///< the date at which the character has been deleted
    };

    char*     extractKeyFromLink(char* text, char const* linkType, char** something1 = NULL);
    char*     extractKeyFromLink(char* text, char const* const* linkTypes, int* found_idx, char** something1 = NULL);
    uint32    extractSpellIdFromLink(char* text);
    GameTele const* extractGameTeleFromLink(char* text);
    bool GetPlayerGroupAndGUIDByName(const char* cname, Player*& plr, Group*& group, uint64& guid, bool offline = false);

    GameObject* GetObjectGlobalyWithGuidOrNearWithDbGuid(uint32 lowguid, uint32 entry);

    // Utility methods for commands
    bool LookupPlayerSearchCommand(QueryResult_AutoPtr result, int32 limit);
    bool HandleBanListHelper(QueryResult_AutoPtr result);
    bool HandleBanHelper(BanMode mode, char const* args);
    bool HandleBanInfoHelper(uint32 accountid, char const* accountname);
    bool HandleUnBanHelper(BanMode mode, char const* args);
    virtual std::string GetNameLink() const { return GetNameLink(m_session->GetPlayer()); }


    bool hasStringAbbr(const char* name, const char* part);

    // function with different implementation for chat/console
    virtual bool isAvailable(ChatCommand const& cmd) const;
    virtual bool needReportToTarget(Player* chr) const;

    void SendGlobalSysMessage(const char* str);
    void SendGlobalGMSysMessage(const char* str);

protected:
    explicit ChatHandler() : m_session(NULL), sentErrorMessage(false) { }      // for CLI subclass

    static bool SetDataForCommandInTable(std::vector<ChatCommand> & table, const char* text, uint32 securityLevel, std::string const& help, std::string const& fullcommand);
    bool ExecuteCommandInTable(std::vector<ChatCommand> const& table, const char* text, std::string const& fullcmd);
    
    /**
     * Stores informations about a deleted character
     
    

    typedef std::list<DeletedInfo> DeletedInfoList;
    bool GetDeletedCharacterInfoList(DeletedInfoList& foundList, std::string searchString = "");
    std::string GenerateDeletedCharacterGUIDsWhereStr(DeletedInfoList::const_iterator& itr, DeletedInfoList::const_iterator const& itr_end);
    void HandleCharacterDeletedListHelper(DeletedInfoList const& foundList);
    void HandleCharacterDeletedRestoreHelper(DeletedInfo const& delInfo);*/


    std::string playerLink(std::string const& name) const { return m_session ? "|cffffffff|Hplayer:"+name+"|h["+name+"]|h|r" : name; }
    std::string GetNameLink(Player* chr) const;

private:
    WorldSession* m_session;                            // != NULL for chat command call and NULL for CLI command

    // common global flag
    static bool load_command_table;
    bool sentErrorMessage;
};

class CliHandler : public ChatHandler
{
public:
    typedef void Print(void*, char const*);
    explicit CliHandler(void* callbackArg, Print* zprint) : m_callbackArg(callbackArg), m_print(zprint) {}

    // overwrite functions
    const char* GetOregonString(int32 entry) const;
    bool isAvailable(ChatCommand const& cmd) const;
    void SendSysMessage(const char* str);
    char const* GetName() const;
    std::string GetNameLink() const override;
    bool needReportToTarget(Player* chr) const;

private:
    void* m_callbackArg;
    Print* m_print;
};

char const* fmtstring(char const* format, ...);

#endif
