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
#include "TicketMgr.h"

class go_commandscript : public CommandScript
{
public:
    go_commandscript() : CommandScript("go_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> goCommandTable =
        {
            { "creature",       SEC_MODERATOR,      false, &HandleGoCreatureCommand,          "" },
            { "graveyard",      SEC_MODERATOR,      true,  &HandleGoGraveyardCommand,         "" },
            { "grid",           SEC_MODERATOR,      true,  &HandleGoGridCommand,              "" },
            { "object",         SEC_MODERATOR,      false, &HandleGoGObjectCommand,           "" },
            { "gobject",        SEC_MODERATOR,      false, &HandleGoGObjectCommand,           "" },
            { "taxinode",       SEC_MODERATOR,      false, &HandleGoTaxiNodeCommand,          "" },
            { "trigger",        SEC_MODERATOR,      false, &HandleGoTriggerCommand,           "" },
            { "zonexy",         SEC_MODERATOR,      false, &HandleGoZoneXYCommand,            "" },
            { "xyz",            SEC_MODERATOR,      false, &HandleGoXYZCommand,               "" },
            { "xy",             SEC_MODERATOR,      false, &HandleGoXYCommand,                "" },
            { "ticket",         SEC_GAMEMASTER,     false, &HandleGoTicketCommand,            "" },
            { "",               SEC_MODERATOR,      false, &HandleGoXYZCommand,               "" },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "go",             SEC_MODERATOR,      false, nullptr,                           "", goCommandTable }
        };
        return commandTable;
    }

    static bool HandleGoCreatureCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;
        Player* _player = handler->GetSession()->GetPlayer();

        // "id" or number or [name] Shift-click form |color|Hcreature_entry:creature_id|h[name]|h|r
        char* pParam1 = handler->extractKeyFromLink((char*)args, "Hcreature");
        if (!pParam1)
            return false;

        std::ostringstream whereClause;

        // User wants to teleport to the NPC's template entry
        if (strcmp(pParam1, "id") == 0)
        {
            //sLog.outError("DEBUG: ID found");

            // Get the "creature_template.entry"
            // number or [name] Shift-click form |color|Hcreature_entry:creature_id|h[name]|h|r
            char* tail = strtok(NULL, "");
            if (!tail)
                return false;
            char* cId = handler->extractKeyFromLink(tail, "Hcreature_entry");
            if (!cId)
                return false;

            int32 tEntry = atoi(cId);
            //sLog.outError("DEBUG: ID value: %d", tEntry);
            if (!tEntry)
                return false;

            whereClause << "WHERE id = '" << tEntry << "'";
        }
        else
        {
            //sLog.outError("DEBUG: ID *not found*");

            int32 guid = atoi(pParam1);

            // Number is invalid - maybe the user specified the mob's name
            if (!guid)
            {
                std::string name = pParam1;
                WorldDatabase.escape_string(name);
                whereClause << ", creature_template WHERE creature.id = creature_template.entry AND creature_template.name " _LIKE_ " '" << name << "'";
            }
            else
                whereClause << "WHERE guid = '" << guid << "'";
        }
        //sLog.outError("DEBUG: %s", whereClause.c_str());

        QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT position_x,position_y,position_z,orientation,map FROM creature %s", whereClause.str().c_str());
        if (!result)
        {
            handler->SendSysMessage(LANG_COMMAND_GOCREATNOTFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }
        if (result->GetRowCount() > 1)
            handler->SendSysMessage(LANG_COMMAND_GOCREATMULTIPLE);

        Field* fields = result->Fetch();
        float x = fields[0].GetFloat();
        float y = fields[1].GetFloat();
        float z = fields[2].GetFloat();
        float ort = fields[3].GetFloat();
        int mapid = fields[4].GetUInt16();

        if (!MapManager::IsValidMapCoord(mapid, x, y, z, ort))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapid);
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

        _player->TeleportTo(mapid, x, y, z, ort);
        return true;
    }

    static bool HandleGoGraveyardCommand(ChatHandler* handler, char const* args)
    {
        Player* _player = handler->GetSession()->GetPlayer();

        if (!*args)
            return false;

        char* gyId = strtok((char*)args, " ");
        if (!gyId)
            return false;

        int32 i_gyId = atoi(gyId);

        if (!i_gyId)
            return false;

        WorldSafeLocsEntry const* gy = sWorldSafeLocsStore.LookupEntry(i_gyId);
        if (!gy)
        {
            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDNOEXIST, i_gyId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!MapManager::IsValidMapCoord(gy->map_id, gy->x, gy->y, gy->z))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, gy->x, gy->y, gy->map_id);
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

        _player->TeleportTo(gy->map_id, gy->x, gy->y, gy->z, _player->GetOrientation());
        return true;
    }

    static bool HandleGoGridCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)    return false;
        Player* _player = handler->GetSession()->GetPlayer();

        char* px = strtok((char*)args, " ");
        char* py = strtok(NULL, " ");
        char* pmapid = strtok(NULL, " ");

        if (!px || !py)
            return false;

        float grid_x = (float)atof(px);
        float grid_y = (float)atof(py);
        uint32 mapid;
        if (pmapid)
            mapid = (uint32)atoi(pmapid);
        else mapid = _player->GetMapId();

        // center of grid
        float x = (grid_x - CENTER_GRID_ID + 0.5f) * SIZE_OF_GRIDS;
        float y = (grid_y - CENTER_GRID_ID + 0.5f) * SIZE_OF_GRIDS;

        if (!MapManager::IsValidMapCoord(mapid, x, y))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapid);
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

        Map const* map = MapManager::Instance().CreateBaseMap(mapid);
        float z = std::max(map->GetHeight(x, y, MAX_HEIGHT), map->GetWaterLevel(x, y));
        _player->TeleportTo(mapid, x, y, z, _player->GetOrientation());

        return true;
    }

    static bool HandleGoGObjectCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* _player = handler->GetSession()->GetPlayer();

        // number or [name] Shift-click form |color|Hgameobject:go_guid|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hgameobject");
        if (!cId)
            return false;

        int32 guid = atoi(cId);
        if (!guid)
            return false;

        float x, y, z, ort;
        int mapid;

        // by DB guid
        if (GameObjectData const* go_data = sObjectMgr.GetGOData(guid))
        {
            x = go_data->posX;
            y = go_data->posY;
            z = go_data->posZ;
            ort = go_data->orientation;
            mapid = go_data->mapid;
        }
        else
        {
            handler->SendSysMessage(LANG_COMMAND_GOOBJNOTFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!MapManager::IsValidMapCoord(mapid, x, y, z, ort))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapid);
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

        _player->TeleportTo(mapid, x, y, z, ort);
        return true;
    }

    static bool HandleGoTaxiNodeCommand(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!*args)
            return false;

        char* id = handler->extractKeyFromLink((char*)args, "Htaxinode");
        if (!id)
            return false;

        int32 nodeId = atoi(id);
        if (!nodeId)
            return false;

        TaxiNodesEntry const* node = sTaxiNodesStore.LookupEntry(nodeId);
        if (!node)
        {
            handler->PSendSysMessage(LANG_COMMAND_GOTAXINODENOTFOUND, nodeId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if ((node->x == 0.0f && node->y == 0.0f && node->z == 0.0f) ||
            !MapManager::IsValidMapCoord(node->map_id, node->x, node->y, node->z))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, node->x, node->y, node->map_id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (player->IsInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        player->TeleportTo(node->map_id, node->x, node->y, node->z, player->GetOrientation());
        return true;
    }

    static bool HandleGoTriggerCommand(ChatHandler* handler, char const* args)
    {
        Player* _player = handler->GetSession()->GetPlayer();

        if (!*args)
            return false;

        char* atId = strtok((char*)args, " ");
        if (!atId)
            return false;

        int32 i_atId = atoi(atId);

        if (!i_atId)
            return false;

        AreaTriggerEntry const* at = sAreaTriggerStore.LookupEntry(i_atId);
        if (!at)
        {
            handler->PSendSysMessage(LANG_COMMAND_GOAREATRNOTFOUND, i_atId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!MapManager::IsValidMapCoord(at->mapid, at->x, at->y, at->z))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, at->x, at->y, at->mapid);
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

        _player->TeleportTo(at->mapid, at->x, at->y, at->z, _player->GetOrientation());
        return true;
    }

    static bool HandleGoZoneXYCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* _player = handler->GetSession()->GetPlayer();

        char* px = strtok((char*)args, " ");
        char* py = strtok(NULL, " ");
        char* tail = strtok(NULL, "");

        char* cAreaId = handler->extractKeyFromLink(tail, "Harea");      // string or [name] Shift-click form |color|Harea:area_id|h[name]|h|r

        if (!px || !py)
            return false;

        float x = (float)atof(px);
        float y = (float)atof(py);
        uint32 areaid = cAreaId ? (uint32)atoi(cAreaId) : _player->GetZoneId();

        AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(areaid);

        if (x < 0 || x > 100 || y < 0 || y > 100 || !areaEntry)
        {
            handler->PSendSysMessage(LANG_INVALID_ZONE_COORD, x, y, areaid);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // update to parent zone if exist (client map show only zones without parents)
        AreaTableEntry const* zoneEntry = areaEntry->zone ? GetAreaEntryByAreaID(areaEntry->zone) : areaEntry;

        Map const* map = MapManager::Instance().CreateBaseMap(zoneEntry->mapid);

        if (map->Instanceable())
        {
            handler->PSendSysMessage(LANG_INVALID_ZONE_MAP, areaEntry->ID, areaEntry->area_name[handler->GetSession()->GetSessionDbcLocale()], map->GetId(), map->GetMapName());
            handler->SetSentErrorMessage(true);
            return false;
        }

        Zone2MapCoordinates(x, y, zoneEntry->ID);

        if (!MapManager::IsValidMapCoord(zoneEntry->mapid, x, y))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, zoneEntry->mapid);
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

        float z = std::max(map->GetHeight(x, y, MAX_HEIGHT), map->GetWaterLevel(x, y));
        _player->TeleportTo(zoneEntry->mapid, x, y, z, _player->GetOrientation());

        return true;
    }

    static bool HandleGoXYCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* _player = handler->GetSession()->GetPlayer();

        char* px = strtok((char*)args, " ");
        char* py = strtok(NULL, " ");
        char* pmapid = strtok(NULL, " ");

        if (!px || !py)
            return false;

        float x = (float)atof(px);
        float y = (float)atof(py);
        uint32 mapid;
        if (pmapid)
            mapid = (uint32)atoi(pmapid);
        else mapid = _player->GetMapId();

        if (!MapManager::IsValidMapCoord(mapid, x, y))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapid);
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

        Map const* map = MapManager::Instance().CreateBaseMap(mapid);
        float z = std::max(map->GetHeight(x, y, MAX_HEIGHT), map->GetWaterLevel(x, y));

        _player->TeleportTo(mapid, x, y, z, _player->GetOrientation());

        return true;
    }

    static bool HandleGoTicketCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* cstrticket_id = strtok((char*)args, " ");

        if (!cstrticket_id)
            return false;

        uint64 ticket_id = atoi(cstrticket_id);
        if (!ticket_id)
            return false;

        GM_Ticket* ticket = ticketmgr.GetGMTicket(ticket_id);
        if (!ticket)
        {
            handler->SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
            return true;
        }

        float x, y, z;
        int mapid;

        x = ticket->pos_x;
        y = ticket->pos_y;
        z = ticket->pos_z;
        mapid = ticket->map;

        Player* _player = handler->GetSession()->GetPlayer();
        if (_player->IsInFlight())
        {
            _player->GetMotionMaster()->MovementExpired();
            _player->CleanupAfterTaxiFlight();
        }
        else
            _player->SaveRecallPosition();

        _player->TeleportTo(mapid, x, y, z, 1, 0);
        return true;
    }

    static bool HandleGoXYZCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* _player = handler->GetSession()->GetPlayer();

        char* px = strtok((char*)args, " ");
        char* py = strtok(NULL, " ");
        char* pz = strtok(NULL, " ");
        char* pmapid = strtok(NULL, " ");

        if (!px || !py || !pz)
            return false;

        float x = (float)atof(px);
        float y = (float)atof(py);
        float z = (float)atof(pz);
        uint32 mapid;
        if (pmapid)
            mapid = (uint32)atoi(pmapid);
        else
            mapid = _player->GetMapId();

        if (!MapManager::IsValidMapCoord(mapid, x, y, z))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapid);
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

        _player->TeleportTo(mapid, x, y, z, _player->GetOrientation());

        return true;
    }
};

void AddSC_go_commandscript()
{
    new go_commandscript();
}