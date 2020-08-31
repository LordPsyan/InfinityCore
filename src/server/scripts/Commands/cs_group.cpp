#include "Group.h"
#include "ScriptMgr.h"
#include "Chat.h"
#include "Language.h"
#include "Player.h"

class group_commandscript : public CommandScript
{
public:
    group_commandscript() : CommandScript("group_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> groupCommandTable =
        {
            { "leader",         SEC_MODERATOR,      true,  &HandleGroupLeaderCommand,           "" },
            { "disband",        SEC_MODERATOR,      true,  &HandleGroupDisbandCommand,          "" },
            { "remove",         SEC_MODERATOR,      true,  &HandleGroupRemoveCommand,           "" },
            { "join",           SEC_MODERATOR,      true,  &HandleGroupJoinCommand,             "" },
            { "summon",         SEC_ADMINISTRATOR,  false, &HandleGROUPSUMMONCommand,           "" },
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "gorup",          SEC_MODERATOR,   true, nullptr,          "", groupCommandTable }
        };

        return commandTable;
    }

    static bool HandleGroupLeaderCommand(ChatHandler* handler, const char* args)
    {
        Player* plr = NULL;
        Group* group = NULL;
        uint64 guid = 0;
        char* cname = strtok((char*)args, " ");

        if (handler->GetPlayerGroupAndGUIDByName(cname, plr, group, guid))
            if (group && group->GetLeaderGUID() != guid)
                group->ChangeLeader(guid);

        return true;
    }

    static bool HandleGroupDisbandCommand(ChatHandler* handler, const char* args)
    {
        Player* plr = NULL;
        Group* group = NULL;
        uint64 guid = 0;
        char* cname = strtok((char*)args, " ");

        if (handler->GetPlayerGroupAndGUIDByName(cname, plr, group, guid))
            if (group)
                group->Disband();

        return true;
    }

    static bool HandleGroupRemoveCommand(ChatHandler* handler, const char* args)
    {
        Player* plr = NULL;
        Group* group = NULL;
        uint64 guid = 0;
        char* cname = strtok((char*)args, " ");

        if (handler->GetPlayerGroupAndGUIDByName(cname, plr, group, guid, true))
            if (group)
                group->RemoveMember(guid);

        return true;
    }

    static bool HandleGroupJoinCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* playerSource = NULL;
        Player* playerTarget = NULL;
        Group* groupSource = NULL;
        Group* groupTarget = NULL;
        uint64 guidSource = 0;
        uint64 guidTarget = 0;
        char* nameplgrStr = strtok((char*)args, " ");
        char* nameplStr = strtok(NULL, " ");

        if (handler->GetPlayerGroupAndGUIDByName(nameplgrStr, playerSource, groupSource, guidSource, true))
        {
            if (groupSource)
            {
                if (handler->GetPlayerGroupAndGUIDByName(nameplStr, playerTarget, groupTarget, guidTarget, true))
                {
                    if (!groupTarget && playerTarget->GetGroup() != groupSource)
                    {
                        if (!groupSource->IsFull())
                        {
                            groupSource->AddMember(guidTarget, playerTarget->GetName());
                            groupSource->BroadcastGroupUpdate();
                            handler->PSendSysMessage(LANG_GROUP_PLAYER_JOINED, playerTarget->GetName(), playerSource->GetName());
                            return true;
                        }
                        else
                        {
                            // group is full
                            handler->PSendSysMessage(LANG_GROUP_FULL);
                            return true;
                        }
                    }
                    else
                    {
                        // group is full or target player already in a group
                        handler->PSendSysMessage(LANG_GROUP_ALREADY_IN_GROUP, playerTarget->GetName());
                        return true;
                    }
                }
            }
            else
            {
                // specified source player is not in a group
                handler->PSendSysMessage(LANG_GROUP_NOT_IN_GROUP, playerSource->GetName());
                return true;
            }
        }

        return true;
    }

    static bool HandleGROUPSUMMONCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        std::string name = args;

        if (!normalizePlayerName(name))
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = sObjectMgr.GetPlayer(name.c_str());
        if (!player)
        {
            handler->PSendSysMessage(LANG_NO_PLAYER, args);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Group* grp = player->GetGroup();

        if (!grp)
        {
            handler->PSendSysMessage(LANG_NOT_IN_GROUP, player->GetName());
            handler->SetSentErrorMessage(true);
            return false;
        }

        Map* gmMap = handler->GetSession()->GetPlayer()->GetMap();
        bool to_instance = gmMap->Instanceable();

        // we are in instance, and can summon only player in our group with us as lead
        if (to_instance && (
            !handler->GetSession()->GetPlayer()->GetGroup() || (grp->GetLeaderGUID() != handler->GetSession()->GetPlayer()->GetGUID()) ||
            (handler->GetSession()->GetPlayer()->GetGroup()->GetLeaderGUID() != handler->GetSession()->GetPlayer()->GetGUID())))
            // the last check is a bit excessive, but let it be, just in case
        {
            handler->SendSysMessage(LANG_CANNOT_SUMMON_TO_INST);
            handler->SetSentErrorMessage(true);
            return false;
        }

        for (GroupReference* itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* pl = itr->GetSource();

            if (!pl || pl == handler->GetSession()->GetPlayer() || !pl->GetSession())
                continue;

            if (pl->IsBeingTeleported())
            {
                handler->PSendSysMessage(LANG_IS_TELEPORTED, pl->GetName());
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (to_instance)
            {
                Map* plMap = pl->GetMap();

                if (plMap->Instanceable() && plMap->GetInstanceId() != gmMap->GetInstanceId())
                {
                    // cannot summon from instance to instance
                    handler->PSendSysMessage(LANG_CANNOT_SUMMON_TO_INST, pl->GetName());
                    handler->SetSentErrorMessage(true);
                    return false;
                }
            }

            handler->PSendSysMessage(LANG_SUMMONING, pl->GetName(), "");
            if (handler->needReportToTarget(pl))
                ChatHandler(pl).PSendSysMessage(LANG_SUMMONED_BY, handler->GetName());

            // stop flight if need
            if (pl->IsInFlight())
            {
                pl->GetMotionMaster()->MovementExpired();
                pl->m_taxi.ClearTaxiDestinations();
            }
            // save only in non-flight case
            else
                pl->SaveRecallPosition();

            // before GM
            float x, y, z;
            handler->GetSession()->GetPlayer()->GetClosePoint(x, y, z, pl->GetObjectSize());
            pl->TeleportTo(handler->GetSession()->GetPlayer()->GetMapId(), x, y, z, pl->GetOrientation());
        }

        return true;
    }
};

void AddSC_group_commandscript()
{
    new group_commandscript();
}