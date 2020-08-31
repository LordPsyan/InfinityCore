#include "Chat.h"
#include "Language.h"
#include "Guild.h"
#include "ObjectAccessor.h"
#include "ScriptMgr.h"

class guild_commandscript : public CommandScript
{
public:
    guild_commandscript() : CommandScript("guild_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> guildCommandTable =
        {
            { "create",         SEC_GAMEMASTER,     true,  &HandleGuildCreateCommand,           "" },
            { "delete",         SEC_GAMEMASTER,     true,  &HandleGuildDeleteCommand,           "" },
            { "invite",         SEC_GAMEMASTER,     true,  &HandleGuildInviteCommand,           "" },
            { "uninvite",       SEC_GAMEMASTER,     true,  &HandleGuildUninviteCommand,         "" },
            { "rank",           SEC_GAMEMASTER,     true,  &HandleGuildRankCommand,             "" }
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "guild",          SEC_GAMEMASTER,  true, nullptr,                                 "", guildCommandTable }
        };
        return commandTable;
    }

    static bool HandleGuildCreateCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* lname = strtok((char*)args, " ");
        char* gname = strtok(NULL, "");

        if (!lname)
            return false;

        if (!gname)
        {
            handler->SendSysMessage(LANG_INSERT_GUILD_NAME);
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string guildname = gname;

        Player* player = ObjectAccessor::Instance().FindPlayerByName(lname);
        if (!player)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetGuildId())
        {
            handler->SendSysMessage(LANG_PLAYER_IN_GUILD);
            return true;
        }

        Guild* guild = new Guild;
        if (!guild->Create(player, guildname))
        {
            delete guild;
            handler->SendSysMessage(LANG_GUILD_NOT_CREATED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        sObjectMgr.AddGuild(guild);
        return true;
    }

    static bool HandleGuildDeleteCommand(ChatHandler* /*handler*/, const char* args)
    {
        if (!*args)
            return false;

        char* par1 = strtok((char*)args, " ");
        if (!par1)
            return false;

        std::string gld = par1;

        Guild* targetGuild = sObjectMgr.GetGuildByName(gld);
        if (!targetGuild)
            return false;

        targetGuild->Disband();
        delete targetGuild;

        return true;
    }

    static bool HandleGuildInviteCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* par1 = strtok((char*)args, " ");
        char* par2 = strtok(NULL, "");
        if (!par1 || !par2)
            return false;

        std::string glName = par2;
        Guild* targetGuild = sObjectMgr.GetGuildByName(glName);
        if (!targetGuild)
            return false;

        std::string plName = par1;
        if (!normalizePlayerName(plName))
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint64 plGuid = 0;
        if (Player* targetPlayer = ObjectAccessor::Instance().FindPlayerByName(plName.c_str()))
            plGuid = targetPlayer->GetGUID();
        else
            plGuid = sObjectMgr.GetPlayerGUIDByName(plName.c_str());

        if (!plGuid)
            return false;

        // player's guild membership checked in AddMember before add
        if (!targetGuild->AddMember(plGuid, targetGuild->GetLowestRank()))
            return false;

        return true;
    }

    static bool HandleGuildUninviteCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* par1 = strtok((char*)args, " ");
        if (!par1)
            return false;

        std::string plName = par1;
        if (!normalizePlayerName(plName))
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint64 plGuid = 0;
        uint32 glId = 0;
        if (Player* targetPlayer = ObjectAccessor::Instance().FindPlayerByName(plName.c_str()))
        {
            plGuid = targetPlayer->GetGUID();
            glId = targetPlayer->GetGuildId();
        }
        else
        {
            plGuid = sObjectMgr.GetPlayerGUIDByName(plName.c_str());
            glId = Player::GetGuildIdFromDB(plGuid);
        }

        if (!plGuid || !glId)
            return false;

        Guild* targetGuild = sObjectMgr.GetGuildById(glId);
        if (!targetGuild)
            return false;

        targetGuild->DelMember(plGuid);
        return true;
    }

    static bool HandleGuildRankCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* par1 = strtok((char*)args, " ");
        char* par2 = strtok(NULL, " ");
        if (!par1 || !par2)
            return false;
        std::string plName = par1;
        if (!normalizePlayerName(plName))
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint64 plGuid = 0;
        uint32 glId = 0;
        if (Player* targetPlayer = ObjectAccessor::Instance().FindPlayerByName(plName.c_str()))
        {
            plGuid = targetPlayer->GetGUID();
            glId = targetPlayer->GetGuildId();
        }
        else
        {
            plGuid = sObjectMgr.GetPlayerGUIDByName(plName.c_str());
            glId = Player::GetGuildIdFromDB(plGuid);
        }

        if (!plGuid || !glId)
            return false;

        Guild* targetGuild = sObjectMgr.GetGuildById(glId);
        if (!targetGuild)
            return false;

        uint32 newrank = uint32(atoi(par2));
        if (newrank > targetGuild->GetLowestRank())
            return false;

        targetGuild->ChangeRank(plGuid, newrank);
        return true;
    }
};

void AddSC_guild_commandscript()
{
    new guild_commandscript();
}