#include "ScriptMgr.h"
#include "Chat.h"
#include "Group.h"
#include "InstanceSaveMgr.h"
#include "InstanceData.h"
#include "MapManager.h"
#include "Player.h"
#include "Language.h"

class instance_commandscript : public CommandScript
{
public:
    instance_commandscript() : CommandScript("instance_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> instanceCommandTable =
        {
            { "listbinds",      SEC_MODERATOR,      false,  &HandleInstanceListBindsCommand,    "" },
            { "unbind",         SEC_MODERATOR,      false,  &HandleInstanceUnbindCommand,       "" },
            { "stats",          SEC_MODERATOR,      true,   &HandleInstanceStatsCommand,        "" },
            { "savedata",       SEC_ADMINISTRATOR,  false,  &HandleInstanceSaveDataCommand,     "" }
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "instance",       SEC_MODERATOR,      true,   nullptr,                            "", instanceCommandTable }
        };

        return commandTable;
    }

    static bool HandleInstanceListBindsCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* player = handler->getSelectedPlayer();
        if (!player) player = handler->GetSession()->GetPlayer();
        uint32 counter = 0;
        for (uint8 i = 0; i < TOTAL_DIFFICULTIES; ++i)
        {
            Player::BoundInstancesMap& binds = player->GetBoundInstances(i);
            for (Player::BoundInstancesMap::iterator itr = binds.begin(); itr != binds.end(); ++itr)
            {
                InstanceSave* save = itr->second.save;
                std::string timeleft = handler->GetTimeString(save->GetResetTime() - time(NULL));
                handler->PSendSysMessage("map: %d inst: %d perm: %s diff: %s canReset: %s TTR: %s", itr->first, save->GetInstanceId(), itr->second.perm ? "yes" : "no", save->GetDifficulty() == DIFFICULTY_NORMAL ? "normal" : "heroic", save->CanReset() ? "yes" : "no", timeleft.c_str());
                counter++;
            }
        }
        handler->PSendSysMessage("player binds: %d", counter);
        counter = 0;
        Group* group = player->GetGroup();
        if (group)
        {
            for (uint8 i = 0; i < TOTAL_DIFFICULTIES; ++i)
            {
                Group::BoundInstancesMap& binds = group->GetBoundInstances(DungeonDifficulty(i));
                for (Group::BoundInstancesMap::iterator itr = binds.begin(); itr != binds.end(); ++itr)
                {
                    InstanceSave* save = itr->second.save;
                    std::string timeleft = handler->GetTimeString(save->GetResetTime() - time(NULL));
                    handler->PSendSysMessage("map: %d inst: %d perm: %s diff: %s canReset: %s TTR: %s", itr->first, save->GetInstanceId(), itr->second.perm ? "yes" : "no", save->GetDifficulty() == DIFFICULTY_NORMAL ? "normal" : "heroic", save->CanReset() ? "yes" : "no", timeleft.c_str());
                    counter++;
                }
            }
        }
        handler->PSendSysMessage("group binds: %d", counter);

        return true;
    }

    static bool HandleInstanceUnbindCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string cmd = args;
        if (cmd == "all")
        {
            Player* player = handler->getSelectedPlayer();
            if (!player) player = handler->GetSession()->GetPlayer();
            uint32 counter = 0;
            for (uint8 i = 0; i < TOTAL_DIFFICULTIES; i++)
            {
                Player::BoundInstancesMap& binds = player->GetBoundInstances(i);
                for (Player::BoundInstancesMap::iterator itr = binds.begin(); itr != binds.end();)
                {
                    if (itr->first != player->GetMapId())
                    {
                        InstanceSave* save = itr->second.save;
                        std::string timeleft = handler->GetTimeString(save->GetResetTime() - time(NULL));
                        handler->PSendSysMessage("unbinding map: %d inst: %d perm: %s diff: %s canReset: %s TTR: %s", itr->first, save->GetInstanceId(), itr->second.perm ? "yes" : "no", save->GetDifficulty() == DIFFICULTY_NORMAL ? "normal" : "heroic", save->CanReset() ? "yes" : "no", timeleft.c_str());
                        player->UnbindInstance(itr, i);
                        counter++;
                    }
                    else
                        ++itr;
                }
            }
            handler->PSendSysMessage("instances unbound: %d", counter);
        }
        return true;
    }

    static bool HandleInstanceStatsCommand(ChatHandler* handler, const char* /*args*/)
    {
        handler->PSendSysMessage("instances loaded: %d", MapManager::Instance().GetNumInstances());
        handler->PSendSysMessage("players in instances: %d", MapManager::Instance().GetNumPlayersInInstances());
        handler->PSendSysMessage("instance saves: %d", sInstanceSaveMgr.GetNumInstanceSaves());
        handler->PSendSysMessage("players bound: %d", sInstanceSaveMgr.GetNumBoundPlayersTotal());
        handler->PSendSysMessage("groups bound: %d", sInstanceSaveMgr.GetNumBoundGroupsTotal());
        return true;
    }

    static bool HandleInstanceSaveDataCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* pl = handler->GetSession()->GetPlayer();

        Map* map = pl->GetMap();
        if (!map->IsDungeon())
        {
            handler->PSendSysMessage("Map is not a dungeon.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!((InstanceMap*)map)->GetInstanceData())
        {
            handler->PSendSysMessage("Map has no instance data.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        ((InstanceMap*)map)->GetInstanceData()->SaveToDB();
        return true;
    }

};

void AddSC_instance_commandscript()
{
    new instance_commandscript();
}