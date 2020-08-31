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
#include "MapManager.h"

class tele_commandscript : public CommandScript
{
public:
    tele_commandscript() : CommandScript("tele_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> teleCommandTable =
        {
            { "add",            SEC_ADMINISTRATOR,  false, &HandleAddTeleCommand,             "" },
            { "del",            SEC_ADMINISTRATOR,  true,  &HandleDelTeleCommand,             "" },
            { "name",           SEC_MODERATOR,      true,  &HandleNameTeleCommand,            "" },
            { "group",          SEC_MODERATOR,      false, &HandleGroupTeleCommand,           "" },
            { "",               SEC_MODERATOR,      false, &HandleTeleCommand,                "" },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "tele",             SEC_MODERATOR,      false, nullptr,                           "", teleCommandTable }
        };
        return commandTable;
    }

    static bool HandleAddTeleCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        std::string name = args;

        if (sObjectMgr.GetGameTele(name))
        {
            handler->SendSysMessage(LANG_COMMAND_TP_ALREADYEXIST);
            handler->SetSentErrorMessage(true);
            return false;
        }

        GameTele tele;
        tele.position_x = player->GetPositionX();
        tele.position_y = player->GetPositionY();
        tele.position_z = player->GetPositionZ();
        tele.orientation = player->GetOrientation();
        tele.mapId = player->GetMapId();
        tele.name = name;

        if (sObjectMgr.AddGameTele(tele))
            handler->SendSysMessage(LANG_COMMAND_TP_ADDED);
        else
        {
            handler->SendSysMessage(LANG_COMMAND_TP_ADDEDERR);
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    static bool HandleDelTeleCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string name = args;

        if (!sObjectMgr.DeleteGameTele(name))
        {
            handler->SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->SendSysMessage(LANG_COMMAND_TP_DELETED);
        return true;
    }

    // teleport player to given game_tele.entry
    static bool HandleNameTeleCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* pName = strtok((char*)args, " ");

        if (!pName)
            return false;

        std::string name = pName;

        if (!normalizePlayerName(name))
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* tail = strtok(NULL, "");
        if (!tail)
            return false;

        // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
        GameTele const* tele = handler->extractGameTeleFromLink(tail);
        if (!tele)
        {
            handler->SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        MapEntry const* me = sMapStore.LookupEntry(tele->mapId);
        if (!me || me->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_CANNOT_TELE_TO_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = sObjectMgr.GetPlayer(name.c_str());
        if (target)
        {
            if (target->IsBeingTeleported())
            {
                handler->PSendSysMessage(LANG_IS_TELEPORTED, target->GetName());
                handler->SetSentErrorMessage(true);
                return false;
            }

            handler->PSendSysMessage(LANG_TELEPORTING_TO, target->GetName(), "", tele->name.c_str());
            if (handler->needReportToTarget(target))
                ChatHandler(target).PSendSysMessage(LANG_TELEPORTED_TO_BY, handler->GetName());

            // stop flight if need
            if (target->IsInFlight())
            {
                target->GetMotionMaster()->MovementExpired();
                target->m_taxi.ClearTaxiDestinations();
            }
            // save only in non-flight case
            else
                target->SaveRecallPosition();

            target->TeleportTo(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation);
        }
        else if (uint64 guid = sObjectMgr.GetPlayerGUIDByName(name.c_str()))
        {
            handler->PSendSysMessage(LANG_TELEPORTING_TO, name.c_str(), handler->GetOregonString(LANG_OFFLINE), tele->name.c_str());
            Player::SavePositionInDB(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation,
                MapManager::Instance().GetZoneId(tele->mapId, tele->position_x, tele->position_y, tele->position_z), guid);
        }
        else
            handler->PSendSysMessage(LANG_NO_PLAYER, name.c_str());

        return true;
    }

    //Teleport group to given game_tele.entry
    static bool HandleGroupTeleCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* player = handler->getSelectedPlayer();
        if (!player)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
        GameTele const* tele = handler->extractGameTeleFromLink((char*)args);
        if (!tele)
        {
            handler->SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        MapEntry const* me = sMapStore.LookupEntry(tele->mapId);
        if (!me || me->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_CANNOT_TELE_TO_BG);
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

        for (GroupReference* itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* pl = itr->GetSource();

            if (!pl || !pl->GetSession())
                continue;

            if (pl->IsBeingTeleported())
            {
                handler->PSendSysMessage(LANG_IS_TELEPORTED, pl->GetName());
                continue;
            }

            handler->PSendSysMessage(LANG_TELEPORTING_TO, pl->GetName(), "", tele->name.c_str());
            if (handler->needReportToTarget(pl))
                ChatHandler(pl).PSendSysMessage(LANG_TELEPORTED_TO_BY, handler->GetName());

            // stop flight if need
            if (pl->IsInFlight())
            {
                pl->GetMotionMaster()->MovementExpired();
                pl->m_taxi.ClearTaxiDestinations();
            }
            // save only in non-flight case
            else
                pl->SaveRecallPosition();

            pl->TeleportTo(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation);
        }

        return true;
    }

    static bool HandleTeleCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* _player = handler->GetSession()->GetPlayer();

        // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
        GameTele const* tele = handler->extractGameTeleFromLink((char*)args);

        if (!tele)
        {
            handler->SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        MapEntry const* me = sMapStore.LookupEntry(tele->mapId);
        if (!me || me->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_CANNOT_TELE_TO_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (_player->IsInFlight())
        {
            _player->GetMotionMaster()->MovementExpired();
            _player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            _player->SaveRecallPosition();

        // let gm fly remain
        if (_player->HasUnitMovementFlag(MOVEMENTFLAG_FLYING))
        {
            WorldPacket data;
            data.SetOpcode(SMSG_MOVE_SET_CAN_FLY);
            data << _player->GetPackGUID();
            data << uint32(0);                                      // unknown
            _player->SendMessageToSet(&data, true);
        }

        _player->TeleportTo(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation);
        return true;
    }


};

void AddSC_tele_commandscript()
{
    new tele_commandscript();
}