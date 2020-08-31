#include "Player.h"
#include "ScriptMgr.h"
#include "GameObject.h"
#include "Language.h"
#include "GameEventMgr.h"
#include "MapManager.h"
#include "PoolMgr.h"


class gameobject_commandscript : CommandScript
{
public:
    gameobject_commandscript() : CommandScript("gameobject_commandscript") {}

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> gobjectCommandTable =
        {
            { "add",            SEC_GAMEMASTER,     false, HandleGameObjectCommand,          "" },
            { "delete",         SEC_GAMEMASTER,     false, HandleDelObjectCommand,           "" },
            { "target",         SEC_GAMEMASTER,     false, HandleTargetObjectCommand,        "" },
            { "turn",           SEC_GAMEMASTER,     false, HandleTurnObjectCommand,          "" },
            { "move",           SEC_GAMEMASTER,     false, HandleMoveObjectCommand,          "" },
            { "near",           SEC_ADMINISTRATOR,  false, HandleNearObjectCommand,          "" },
            { "activate",       SEC_GAMEMASTER,     false, HandleActivateObjectCommand,      "" },
            { "setphase",       SEC_GAMEMASTER,     false, HandleGOPhaseCommand,             "" },
            { "addtemp",        SEC_GAMEMASTER,     false, HandleTempGameObjectCommand,      "" }
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "gobject",          SEC_GAMEMASTER,   true, nullptr,    "", gobjectCommandTable}
        };

        return commandTable;

    }

    static bool HandleGameObjectCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* pParam1 = strtok((char*)args, " ");
        if (!pParam1)
            return false;

        uint32 id = atoi((char*)pParam1);
        if (!id)
            return false;

        char* spawntimeSecs = strtok(NULL, " ");

        const GameObjectInfo* gInfo = sObjectMgr.GetGameObjectInfo(id);

        if (!gInfo)
        {
            handler->PSendSysMessage(LANG_GAMEOBJECT_NOT_EXIST, id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* chr = handler->GetSession()->GetPlayer();
        float x = float(chr->GetPositionX());
        float y = float(chr->GetPositionY());
        float z = float(chr->GetPositionZ());
        float o = float(chr->GetOrientation());
        Map* map = chr->GetMap();

        GameObject* pGameObj = new GameObject;
        uint32 db_lowGUID = sObjectMgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT);

        if (!pGameObj->Create(db_lowGUID, gInfo->id, map, PHASEMASK_NORMAL, x, y, z, o, 0.0f, 0.0f, 0.0f, 0.0f, 0, GO_STATE_READY))
        {
            delete pGameObj;
            return false;
        }

        if (spawntimeSecs)
        {
            uint32 value = atoi((char*)spawntimeSecs);
            pGameObj->SetRespawnTime(value);
            //sLog.outDebug("*** spawntimeSecs: %d", value);
        }

        // fill the gameobject data and save to the db
        pGameObj->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), chr->GetPhaseMaskForSpawn());
        delete pGameObj;

        pGameObj = new GameObject();
        // this will generate a new guid if the object is in an instance
        if (!pGameObj->LoadGameObjectFromDB(db_lowGUID, map))
        {
            delete pGameObj;
            return false;
        }

        sLog.outDebug(handler->GetOregonString(LANG_GAMEOBJECT_CURRENT), gInfo->name, db_lowGUID, x, y, z, o);

        // @todo is it really necessary to add both the real and DB table guid here ?
        sObjectMgr.AddGameobjectToGrid(db_lowGUID, sObjectMgr.GetGOData(db_lowGUID));

        handler->PSendSysMessage(LANG_GAMEOBJECT_ADD, id, gInfo->name, db_lowGUID, x, y, z);
        return true;
    }

    static bool HandleDelObjectCommand(ChatHandler* handler, const char* args)
    {
        // number or [name] Shift-click form |color|Hgameobject:go_guid|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hgameobject");
        if (!cId)
            return false;

        uint32 lowguid = atoi(cId);
        if (!lowguid)
            return false;

        GameObject* obj = NULL;

        // by DB guid
        if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowguid))
            obj = handler->GetObjectGlobalyWithGuidOrNearWithDbGuid(lowguid, go_data->id);

        if (!obj)
        {
            handler->PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint64 owner_guid = obj->GetOwnerGUID();
        if (owner_guid)
        {
            Unit* owner = ObjectAccessor::GetUnit(*handler->GetSession()->GetPlayer(), owner_guid);
            if (!owner || !IS_PLAYER_GUID(owner_guid))
            {
                handler->PSendSysMessage(LANG_COMMAND_DELOBJREFERCREATURE, GUID_LOPART(owner_guid), obj->GetGUIDLow());
                handler->SetSentErrorMessage(true);
                return false;
            }

            owner->RemoveGameObject(obj, false);
        }

        obj->SetRespawnTime(0);                                 // not save respawn time
        obj->Delete();
        obj->DeleteFromDB();

        handler->PSendSysMessage(LANG_COMMAND_DELOBJMESSAGE, obj->GetGUIDLow());

        return true;
    }

    static bool HandleTargetObjectCommand(ChatHandler* handler, const char* args)
    {
        Player* pl = handler->GetSession()->GetPlayer();
        QueryResult_AutoPtr result;
        GameEventMgr::ActiveEvents const& activeEventsList = sGameEventMgr.GetActiveEventList();
        if (*args)
        {
            int32 id = atoi((char*)args);
            if (id)
                result = WorldDatabase.PQuery("SELECT guid, id, position_x, position_y, position_z, orientation, map, (POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2)) AS order_ FROM gameobject WHERE map = '%i' AND id = '%u' ORDER BY order_ ASC LIMIT 1",
                    pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), pl->GetMapId(), id);
            else
            {
                std::string name = args;
                WorldDatabase.escape_string(name);
                result = WorldDatabase.PQuery(
                    "SELECT guid, id, position_x, position_y, position_z, orientation, map, (POW(position_x - %f, 2) + POW(position_y - %f, 2) + POW(position_z - %f, 2)) AS order_ "
                    "FROM gameobject,gameobject_template WHERE gameobject_template.entry = gameobject.id AND map = %i AND name " _LIKE_ " " _CONCAT3_("'%%'", "'%s'", "'%%'")" ORDER BY order_ ASC LIMIT 1",
                    pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), pl->GetMapId(), name.c_str());
            }
        }
        else
        {
            std::ostringstream eventFilter;
            eventFilter << " AND (event IS NULL ";
            bool initString = true;

            for (GameEventMgr::ActiveEvents::const_iterator itr = activeEventsList.begin(); itr != activeEventsList.end(); ++itr)
            {
                if (initString)
                {
                    eventFilter << "OR event IN (" << *itr;
                    initString = false;
                }
                else
                    eventFilter << "," << *itr;
            }

            if (!initString)
                eventFilter << "))";
            else
                eventFilter << ")";

            result = WorldDatabase.PQuery("SELECT gameobject.guid, id, position_x, position_y, position_z, orientation, map, "
                "(POW(position_x - %f, 2) + POW(position_y - %f, 2) + POW(position_z - %f, 2)) AS order_ FROM gameobject "
                "LEFT OUTER JOIN game_event_gameobject on gameobject.guid=game_event_gameobject.guid WHERE map = '%i' %s ORDER BY order_ ASC LIMIT 10",
                handler->GetSession()->GetPlayer()->GetPositionX(), handler->GetSession()->GetPlayer()->GetPositionY(), handler->GetSession()->GetPlayer()->GetPositionZ(), handler->GetSession()->GetPlayer()->GetMapId(), eventFilter.str().c_str());
        }

        if (!result)
        {
            handler->SendSysMessage(LANG_COMMAND_TARGETOBJNOTFOUND);
            return true;
        }

        bool found = false;
        float x, y, z, o;
        uint32 lowguid, id;
        uint16 mapid;
        uint32 pool_id;

        do
        {
            Field* fields = result->Fetch();
            lowguid = fields[0].GetUInt32();
            id = fields[1].GetUInt32();
            x = fields[2].GetFloat();
            y = fields[3].GetFloat();
            z = fields[4].GetFloat();
            o = fields[5].GetFloat();
            mapid = fields[6].GetUInt16();
            pool_id = sPoolMgr.IsPartOfAPool<GameObject>(lowguid);
            if (!pool_id || (pool_id && sPoolMgr.IsSpawnedObject<GameObject>(pool_id)))
                found = true;
        } while (result->NextRow() && (!found));

        if (!found)
        {
            handler->PSendSysMessage(LANG_GAMEOBJECT_NOT_EXIST, id);
            return false;
        }

        GameObjectInfo const* goI = sObjectMgr.GetGameObjectInfo(id);

        if (!goI)
        {
            handler->PSendSysMessage(LANG_GAMEOBJECT_NOT_EXIST, id);
            return false;
        }

        GameObject* target = handler->GetSession()->GetPlayer()->GetMap()->GetGameObject(MAKE_NEW_GUID(lowguid, id, HIGHGUID_GAMEOBJECT));

        handler->PSendSysMessage(LANG_GAMEOBJECT_DETAIL, lowguid, goI->name, lowguid, id, x, y, z, mapid, o);

        if (target)
        {
            int32 curRespawnDelay = target->GetRespawnTimeEx() - time(NULL);
            if (curRespawnDelay < 0)
                curRespawnDelay = 0;

            std::string curRespawnDelayStr = secsToTimeString(curRespawnDelay, true);
            std::string defRespawnDelayStr = secsToTimeString(target->GetRespawnDelay(), true);

            handler->PSendSysMessage(LANG_COMMAND_RAWPAWNTIMES, defRespawnDelayStr.c_str(), curRespawnDelayStr.c_str());
        }
        return true;
    }

    static bool HandleTurnObjectCommand(ChatHandler* handler, const char* args)
    {
        // number or [name] Shift-click form |color|Hgameobject:go_id|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hgameobject");
        if (!cId)
            return false;

        uint32 lowguid = atoi(cId);
        if (!lowguid)
            return false;

        GameObject* obj = NULL;

        // by DB guid
        if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowguid))
            obj = handler->GetObjectGlobalyWithGuidOrNearWithDbGuid(lowguid, go_data->id);

        if (!obj)
        {
            handler->PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* po = strtok(NULL, " ");
        float o;

        if (po)
            o = (float)atof(po);
        else
        {
            Player* chr = handler->GetSession()->GetPlayer();
            o = chr->GetOrientation();
        }

        obj->Relocate(obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), o);
        obj->UpdateRotationFields();
        obj->DestroyForNearbyPlayers();
        obj->UpdateObjectVisibility();

        obj->SaveToDB();
        obj->Refresh();

        handler->PSendSysMessage(LANG_COMMAND_TURNOBJMESSAGE, obj->GetGUIDLow(), o);

        return true;
    }

    static bool HandleMoveObjectCommand(ChatHandler* handler, const char* args)
    {
        // number or [name] Shift-click form |color|Hgameobject:go_guid|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hgameobject");
        if (!cId)
            return false;

        uint32 lowguid = atoi(cId);
        if (!lowguid)
            return false;

        GameObject* obj = NULL;

        // by DB guid
        if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowguid))
            obj = handler->GetObjectGlobalyWithGuidOrNearWithDbGuid(lowguid, go_data->id);

        if (!obj)
        {
            handler->PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* px = strtok(NULL, " ");
        char* py = strtok(NULL, " ");
        char* pz = strtok(NULL, " ");

        if (!px)
        {
            Player* chr = handler->GetSession()->GetPlayer();
            obj->Relocate(chr->GetPositionX(), chr->GetPositionY(), chr->GetPositionZ(), obj->GetOrientation());
            obj->SetFloatValue(GAMEOBJECT_POS_X, chr->GetPositionX());
            obj->SetFloatValue(GAMEOBJECT_POS_Y, chr->GetPositionY());
            obj->SetFloatValue(GAMEOBJECT_POS_Z, chr->GetPositionZ());

            obj->DestroyForNearbyPlayers();
            obj->UpdateObjectVisibility();
        }
        else
        {
            if (!py || !pz)
                return false;

            float x = (float)atof(px);
            float y = (float)atof(py);
            float z = (float)atof(pz);

            if (!MapManager::IsValidMapCoord(obj->GetMapId(), x, y, z))
            {
                handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, obj->GetMapId());
                handler->SetSentErrorMessage(true);
                return false;
            }

            obj->Relocate(x, y, z, obj->GetOrientation());
            obj->SetFloatValue(GAMEOBJECT_POS_X, x);
            obj->SetFloatValue(GAMEOBJECT_POS_Y, y);
            obj->SetFloatValue(GAMEOBJECT_POS_Z, z);

            obj->DestroyForNearbyPlayers();
            obj->UpdateObjectVisibility();
        }

        obj->SaveToDB();
        obj->Refresh();

        handler->PSendSysMessage(LANG_COMMAND_MOVEOBJMESSAGE, obj->GetGUIDLow());

        return true;
    }

    static bool HandleNearObjectCommand(ChatHandler* handler, const char* args)
    {
        float distance = (!*args) ? 10 : atol(args);
        uint32 count = 0;

        Player* pl = handler->GetSession()->GetPlayer();
        QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT guid, id, position_x, position_y, position_z, map, "
            "(POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2)) AS order_ "
            "FROM gameobject WHERE map='%u' AND (POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2)) <= '%f' ORDER BY order_",
            pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(),
            pl->GetMapId(), pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), distance * distance);

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 guid = fields[0].GetUInt32();
                uint32 entry = fields[1].GetUInt32();
                float x = fields[2].GetFloat();
                float y = fields[3].GetFloat();
                float z = fields[4].GetFloat();
                int mapid = fields[5].GetUInt16();

                GameObjectInfo const* gInfo = sObjectMgr.GetGameObjectInfo(entry);

                if (!gInfo)
                    continue;

                handler->PSendSysMessage(LANG_GO_LIST_CHAT, guid, entry, guid, gInfo->name, x, y, z, mapid);

                ++count;
            } while (result->NextRow());
        }

        handler->PSendSysMessage(LANG_COMMAND_NEAROBJMESSAGE, distance, count);
        return true;
    }

    static bool HandleActivateObjectCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* cId = handler->extractKeyFromLink((char*)args, "Hgameobject");
        if (!cId)
            return false;

        uint32 lowguid = atoi(cId);
        if (!lowguid)
            return false;

        GameObject* obj = NULL;

        // by DB guid
        if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowguid))
            obj = handler->GetObjectGlobalyWithGuidOrNearWithDbGuid(lowguid, go_data->id);

        if (!obj)
        {
            handler->PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Activate
        obj->SetLootState(GO_READY);
        obj->UseDoorOrButton(10000, false, handler->GetSession()->GetPlayer());

        handler->PSendSysMessage("Object activated!");

        return true;
    }

    static bool HandleTempGameObjectCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;
        char* id = strtok((char*)args, " ");
        if (!id)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        char* spawntime = strtok(NULL, " ");
        uint32 spawntm = 0;

        if (spawntime)
            spawntm = atoi((char*)spawntime);

        float x = player->GetPositionX();
        float y = player->GetPositionY();
        float z = player->GetPositionZ();
        float ang = player->GetOrientation();

        float rot2 = sin(ang / 2);
        float rot3 = cos(ang / 2);

        uint32 objectId = atoi(id);

        if (!sObjectMgr.GetGameObjectInfo(objectId))
        {
            handler->PSendSysMessage(LANG_GAMEOBJECT_NOT_EXIST, objectId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        player->SummonGameObject(objectId, x, y, z, ang, 0, 0, rot2, rot3, spawntm);

        return true;
    }

    static bool HandleGOPhaseCommand(ChatHandler* handler, const char* args)
    {
        // number or [name] Shift-click form |color|Hgameobject:go_id|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hgameobject");
        if (!cId)
            return false;

        uint32 lowguid = atoi(cId);
        if (!lowguid)
            return false;

        GameObject* obj = NULL;
        // by DB guid
        if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowguid))
            obj = handler->GetObjectGlobalyWithGuidOrNearWithDbGuid(lowguid, go_data->id);

        if (!obj)
        {
            handler->PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* phaseStr = strtok(NULL, " ");
        uint32 phasemask = phaseStr ? atoi(phaseStr) : 0;
        if (phasemask == 0)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        obj->SetPhaseMask(phasemask, true, false);
        obj->SaveToDB();
        return true;
    }
};

void AddSC_gameobject_commandscript()
{
    new gameobject_commandscript();
}
