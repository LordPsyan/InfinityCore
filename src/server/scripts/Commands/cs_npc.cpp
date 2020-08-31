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
#include "TargetedMovementGenerator.h"

class npc_commandscript : public CommandScript
{
public:
    npc_commandscript() : CommandScript("npc_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> npcDeleteCommandTable =
        {
            { "item",           SEC_ADMINISTRATOR,   false, &HandleNpcDeleteItemCommand,         "" },
            { "",               SEC_ADMINISTRATOR,   false, &HandleNpcDeleteCommand,             "" },
        };

        static std::vector<ChatCommand> npcFollowCommandTable =
        {
            { "stop",           SEC_GAMEMASTER,      false, &HandleNpcUnFollowCommand,           "" },
            { "",               SEC_GAMEMASTER,      false, &HandleNpcFollowCommand,             "" },
        };

        static std::vector<ChatCommand> npcFactionCommandTable =
        {
            { "permanent",      SEC_ADMINISTRATOR,   false, &HandleNpcSetFactionPermanentCommand,"" },
            { "temp",           SEC_ADMINISTRATOR,   false, &HandleNpcSetFactionTempCommand,     "" },
            { "original",       SEC_ADMINISTRATOR,   false, &HandleNpcSetFactionOriginalCommand, "" },
        };

        static std::vector<ChatCommand> npcAddCommandTable =
        {
            { "temp",           SEC_ADMINISTRATOR,   false, &HandleNpcAddTempCommand,            "" },
            { "item",           SEC_ADMINISTRATOR,   false, &HandleNpcAddItemCommand,            "" },
            { "formation",      SEC_ADMINISTRATOR,   false, &HandleNpcAddFormationCommand,       "" },
            { "",               SEC_ADMINISTRATOR,   false, &HandleNpcAddCommand,                "" },
        };

        static std::vector<ChatCommand> npcSetCommandTable =
        {
            { "allowmove",      SEC_ADMINISTRATOR,   false, &HandleNpcSetAllowMoveCommand,       "" },
            { "entry",          SEC_ADMINISTRATOR,   false, &HandleNpcSetEntryCommand,           "" },
            { "level",          SEC_ADMINISTRATOR,   false, &HandleNpcSetLevelCommand,           "" },
            { "link",           SEC_ADMINISTRATOR,   false, &HandleNpcSetLinkCommand,            "" },
            { "model",          SEC_ADMINISTRATOR,   false, &HandleNpcSetModelCommand,           "" },
            { "movetype",       SEC_ADMINISTRATOR,   false, &HandleNpcSetMoveTypeCommand,        "" },
            { "spawntime",      SEC_ADMINISTRATOR,   false, &HandleNpcSetSpawnTimeCommand,       "" },
            { "flag",           SEC_ADMINISTRATOR,   false, &HandleNpcSetFlagCommand,            "" },
            { "setphase",       SEC_GAMEMASTER,      false, HandleNpcSetPhaseCommand,            "" },
            { "faction",        SEC_ADMINISTRATOR,   false, nullptr,                             "", npcFactionCommandTable },
        };

        static std::vector<ChatCommand> npcCommandTable =
        {
            { "info",           SEC_MODERATOR,      false, &HandleNpcInfoCommand,                "" },
            { "near",           SEC_GAMEMASTER,     false, &HandleNpcNearCommand,                "" },
            { "move",           SEC_ADMINISTRATOR,  false, &HandleNpcMoveCommand,                "" },
            { "playemote",      SEC_ADMINISTRATOR,  false, &HandleNpcPlayEmoteCommand,           "" },
            { "say",            SEC_MODERATOR,      false, &HandleNpcSayCommand,                 "" },
            { "textemote",      SEC_GAMEMASTER,     false, &HandleNpcTextEmoteCommand,           "" },
            { "whisper",        SEC_GAMEMASTER,     false, &HandleNpcWhisperCommand,             "" },
            { "yell",           SEC_GAMEMASTER,     false, &HandleNpcYellCommand,                "" },
            { "add",            SEC_GAMEMASTER,     false, nullptr,                              "", npcAddCommandTable },
            { "set",            SEC_ADMINISTRATOR,  false, nullptr,                              "", npcSetCommandTable },
            { "follow",         SEC_GAMEMASTER,     false, nullptr,                              "", npcFollowCommandTable },
            { "delete",         SEC_ADMINISTRATOR,  false, nullptr,                              "", npcDeleteCommandTable },
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "npc",            SEC_MODERATOR,      false, nullptr,                              "", npcCommandTable }
        };
        return commandTable;
    }

    static bool HandleNpcDeleteItemCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Creature* vendor = handler->getSelectedCreature();
        if (!vendor || !vendor->IsVendor())
        {
            handler->SendSysMessage(LANG_COMMAND_VENDORSELECTION);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* pitem = handler->extractKeyFromLink((char*)args, "Hitem");
        if (!pitem)
        {
            handler->SendSysMessage(LANG_COMMAND_NEEDITEMSEND);
            handler->SetSentErrorMessage(true);
            return false;
        }
        uint32 itemId = atol(pitem);

        char* addMulti = strtok(NULL, " ");
        if (!sObjectMgr.RemoveVendorItem(addMulti ? handler->GetSession()->GetCurrentVendor() : vendor->GetEntry(), itemId))
        {
            handler->PSendSysMessage(LANG_ITEM_NOT_IN_LIST, itemId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        ItemTemplate const* pProto = sObjectMgr.GetItemTemplate(itemId);

        handler->PSendSysMessage(LANG_ITEM_DELETED_FROM_LIST, itemId, pProto->Name1);
        return true;
    }

    static bool HandleNpcDeleteCommand(ChatHandler* handler, const char* args)
    {
        Creature* unit = NULL;

        if (*args)
        {
            // number or [name] Shift-click form |color|Hcreature:creature_guid|h[name]|h|r
            char* cId = handler->extractKeyFromLink((char*)args, "Hcreature");
            if (!cId)
                return false;

            uint32 lowguid = atoi(cId);
            if (!lowguid)
                return false;

            if (CreatureData const* cr_data = sObjectMgr.GetCreatureData(lowguid))
                unit = handler->GetSession()->GetPlayer()->GetMap()->GetCreature(MAKE_NEW_GUID(lowguid, cr_data->id, HIGHGUID_UNIT));
        }
        else
            unit = handler->getSelectedCreature();

        if (!unit || unit->IsPet() || unit->IsTotem())
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Delete the creature
        unit->CombatStop();
        unit->DeleteFromDB();
        unit->AddObjectToRemoveList();

        handler->SendSysMessage(LANG_COMMAND_DELCREATMESSAGE);

        return true;
    }

    static bool HandleNpcUnFollowCommand(ChatHandler* handler, const char* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        Creature* creature = handler->getSelectedCreature();

        if (!creature)
        {
            handler->PSendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (/*creature->GetMotionMaster()->empty() ||*/
            creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != FOLLOW_MOTION_TYPE)
        {
            handler->PSendSysMessage(LANG_CREATURE_NOT_FOLLOW_YOU);
            handler->SetSentErrorMessage(true);
            return false;
        }

        FollowMovementGenerator<Creature> const* mgen
            = static_cast<FollowMovementGenerator<Creature> const*>((creature->GetMotionMaster()->top()));

        if (mgen->GetTarget() != player)
        {
            handler->PSendSysMessage(LANG_CREATURE_NOT_FOLLOW_YOU);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // reset movement
        creature->GetMotionMaster()->MovementExpired(true);

        handler->PSendSysMessage(LANG_CREATURE_NOT_FOLLOW_YOU_NOW, creature->GetName());
        return true;
    }

    static bool HandleNpcFollowCommand(ChatHandler* handler, const char* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        Creature* creature = handler->getSelectedCreature();

        if (!creature)
        {
            handler->PSendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        creature->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_NONE);
        player->UpdateObjectVisibility();
        // Follow player - Using pet's default dist and angle
        creature->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);

        handler->PSendSysMessage(LANG_CREATURE_FOLLOW_YOU_NOW, creature->GetName());
        return true;
    }

    static bool HandleNpcSetFactionOriginalCommand(ChatHandler* handler, const char* args)
    {
        Player* me = handler->GetSession()->GetPlayer();

        if (!me)
            return false;

        Creature* creature = me->GetSelectedUnit()->ToCreature();

        if (!creature)
            return false;

        creature->RestoreFaction();

        return true;
    }

    static bool HandleNpcSetFactionTempCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 factionId = (uint32)atoi((char*)args);

        if (!sFactionTemplateStore.LookupEntry(factionId))
        {
            handler->PSendSysMessage(LANG_WRONG_FACTION, factionId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Creature* pCreature = handler->getSelectedCreature();

        if (!pCreature)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        pCreature->SetFaction(factionId);
        return true;
    }

    static bool HandleNpcSetFactionPermanentCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 factionId = (uint32)atoi((char*)args);

        if (!sFactionTemplateStore.LookupEntry(factionId))
        {
            handler->PSendSysMessage(LANG_WRONG_FACTION, factionId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Creature* pCreature = handler->getSelectedCreature();

        if (!pCreature)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        pCreature->SetFaction(factionId);

        // faction is set in creature_template - not inside creature

        // update in memory
        if (CreatureInfo const* cinfo = pCreature->GetCreatureTemplate())
        {
            const_cast<CreatureInfo*>(cinfo)->faction = factionId;
        }

        // and DB
        WorldDatabase.PExecuteLog("UPDATE creature_template SET faction = '%u' WHERE entry = '%u'", factionId, pCreature->GetEntry());

        return true;
    }

    static bool HandleNpcSetPhaseCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 phasemask = (uint32)atoi((char*)args);
        if (phasemask == 0)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Creature* pCreature = handler->getSelectedCreature();
        if (!pCreature)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        pCreature->SetPhaseMask(phasemask, true, false);

        if (!pCreature->IsPet())
            pCreature->SaveToDB();

        return true;
    }


    static bool HandleNpcSetFlagCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 npcFlags = (uint32)atoi((char*)args);

        Creature* pCreature = handler->getSelectedCreature();

        if (!pCreature)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        pCreature->SetUInt32Value(UNIT_NPC_FLAGS, npcFlags);

        WorldDatabase.PExecuteLog("UPDATE creature_template SET npcflag = '%u' WHERE entry = '%u'", npcFlags, pCreature->GetEntry());

        handler->SendSysMessage(LANG_VALUE_SAVED_REJOIN);

        return true;
    }

    static bool HandleNpcSetSpawnTimeCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* stime = strtok((char*)args, " ");

        if (!stime)
            return false;

        int i_stime = atoi((char*)stime);

        if (i_stime < 0)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Creature* pCreature = handler->getSelectedCreature();
        uint32 u_guidlow = 0;

        if (pCreature)
            u_guidlow = pCreature->GetDBTableGUIDLow();
        else
            return false;

        WorldDatabase.PExecuteLog("UPDATE creature SET spawntimesecs=%i WHERE guid=%u", i_stime, u_guidlow);
        pCreature->SetRespawnDelay((uint32)i_stime);
        handler->PSendSysMessage(LANG_COMMAND_SPAWNTIME, i_stime);

        return true;
    }

    static bool HandleNpcSetMoveTypeCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // 3 arguments:
        // GUID (optional - you can also select the creature)
        // stay|random|way (determines the kind of movement)
        // NODEL (optional - tells the system NOT to delete any waypoints)
        //        this is very handy if you want to do waypoints, that are
        //        later switched on/off according to special events (like escort
        //        quests, etc)
        char* guid_str = strtok((char*)args, " ");
        char* type_str = strtok((char*)NULL, " ");
        char* dontdel_str = strtok((char*)NULL, " ");

        bool doNotDelete = false;

        if (!guid_str)
            return false;

        uint32 lowguid = 0;
        Creature* pCreature = NULL;

        if (dontdel_str)
        {
            //sLog.outError("DEBUG: All 3 params are set");

            // All 3 params are set
            // GUID
            // type
            // doNotDEL
            if (stricmp(dontdel_str, "NODEL") == 0)
            {
                //sLog.outError("DEBUG: doNotDelete = true;");
                doNotDelete = true;
            }
        }
        else
        {
            // Only 2 params - but maybe NODEL is set
            if (type_str)
            {
                sLog.outError("DEBUG: Only 2 params ");
                if (stricmp(type_str, "NODEL") == 0)
                {
                    //sLog.outError("DEBUG: type_str, NODEL ");
                    doNotDelete = true;
                    type_str = NULL;
                }
            }
        }

        if (!type_str)                                           // case .setmovetype $move_type (with selected creature)
        {
            type_str = guid_str;
            pCreature = handler->getSelectedCreature();
            if (!pCreature || pCreature->IsPet())
                return false;
            lowguid = pCreature->GetDBTableGUIDLow();
        }
        else                                                    // case .setmovetype #creature_guid $move_type (with selected creature)
        {
            lowguid = atoi((char*)guid_str);

            /* impossible without entry
            if (lowguid)
                pCreature = ObjectAccessor::GetCreature(*m_session->GetPlayer(),MAKE_GUID(lowguid,HIGHGUID_UNIT));
            */

            // attempt check creature existence by DB data
            if (!pCreature)
            {
                CreatureData const* data = sObjectMgr.GetCreatureData(lowguid);
                if (!data)
                {
                    handler->PSendSysMessage(LANG_COMMAND_CREATGUIDNOTFOUND, lowguid);
                    handler->SetSentErrorMessage(true);
                    return false;
                }
            }
            else
                lowguid = pCreature->GetDBTableGUIDLow();
        }

        // now lowguid is low guid really existed creature
        // and pCreature point (maybe) to this creature or NULL

        MovementGeneratorType move_type;

        std::string type = type_str;

        if (type == "stay")
            move_type = IDLE_MOTION_TYPE;
        else if (type == "random")
            move_type = RANDOM_MOTION_TYPE;
        else if (type == "way")
            move_type = WAYPOINT_MOTION_TYPE;
        else
            return false;

        if (pCreature)
        {
            // update movement type
            if (doNotDelete == false)
                pCreature->LoadPath(0);

            pCreature->SetDefaultMovementType(move_type);
            pCreature->GetMotionMaster()->Initialize();
            if (pCreature->IsAlive())                            // dead creature will reset movement generator at respawn
            {
                pCreature->setDeathState(JUST_DIED);
                pCreature->Respawn();
            }
            pCreature->SaveToDB();
        }
        if (doNotDelete == false)
            handler->PSendSysMessage(LANG_MOVE_TYPE_SET, type_str);
        else
            handler->PSendSysMessage(LANG_MOVE_TYPE_SET_NODEL, type_str);

        return true;
    }

    static bool HandleNpcSetModelCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 displayId = (uint32)atoi((char*)args);

        Creature* pCreature = handler->getSelectedCreature();

        if (!pCreature || pCreature->IsPet())
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        pCreature->SetDisplayId(displayId);
        pCreature->SetNativeDisplayId(displayId);

        pCreature->SaveToDB();

        return true;
    }

    static bool HandleNpcSetLinkCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 linkguid = (uint32)atoi((char*)args);

        Creature* pCreature = handler->getSelectedCreature();

        if (!pCreature)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!pCreature->GetDBTableGUIDLow())
        {
            handler->PSendSysMessage("Selected creature (GUID: %u) isn't in creature table", pCreature->GetGUIDLow());
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!sObjectMgr.SetCreatureLinkedRespawn(pCreature->GetDBTableGUIDLow(), linkguid))
        {
            handler->PSendSysMessage("Selected creature can't link with guid '%u'", linkguid);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage("LinkGUID '%u' added to creature with DBTableGUID: '%u'", linkguid, pCreature->GetDBTableGUIDLow());
        return true;
    }

    static bool HandleNpcSetLevelCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint8 lvl = (uint8)atoi((char*)args);
        if (lvl < 1 || lvl > sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL) + 3)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Creature* pCreature = handler->getSelectedCreature();
        if (!pCreature)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (pCreature->IsPet())
            ((Pet*)pCreature)->GivePetLevel(lvl);
        else
        {
            pCreature->SetMaxHealth(100 + 30 * lvl);
            pCreature->SetHealth(100 + 30 * lvl);
            pCreature->SetLevel(lvl);
            pCreature->SaveToDB();
        }

        return true;
    }

    static bool HandleNpcSetEntryCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 newEntryNum = atoi(args);
        if (!newEntryNum)
            return false;

        Unit* unit = handler->getSelectedUnit();
        if (!unit || unit->GetTypeId() != TYPEID_UNIT)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }
        Creature* creature = unit->ToCreature();
        if (creature->UpdateEntry(newEntryNum))
            handler->SendSysMessage(LANG_DONE);
        else
            handler->SendSysMessage(LANG_ERROR);
        return true;
    }

    static bool HandleNpcSetAllowMoveCommand(ChatHandler* handler, const char* /*args*/)
    {
        if (sWorld.getAllowMovement())
        {
            sWorld.SetAllowMovement(false);
            handler->SendSysMessage(LANG_CREATURE_MOVE_DISABLED);
        }
        else
        {
            sWorld.SetAllowMovement(true);
            handler->SendSysMessage(LANG_CREATURE_MOVE_ENABLED);
        }
        return true;
    }
    
    static bool HandleNpcAddFormationCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 leaderGUID = (uint32)atoi((char*)args);
        Creature* creature = handler->getSelectedCreature();

        if (!creature || !creature->GetDBTableGUIDLow())
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 lowguid = creature->GetDBTableGUIDLow();
        if (creature->GetFormation())
        {
            handler->PSendSysMessage("Selected creature is already member of group %u", creature->GetFormation()->GetId());
            return false;
        }

        if (!lowguid)
            return false;

        Player* chr = handler->GetSession()->GetPlayer();
        FormationInfo* group_member;

        group_member = new FormationInfo;
        group_member->follow_angle = (creature->GetAngle(chr) - chr->GetOrientation()) * 180 / M_PI;
        group_member->follow_dist = sqrtf(pow(chr->GetPositionX() - creature->GetPositionX(), int(2)) + pow(chr->GetPositionY() - creature->GetPositionY(), int(2)));
        group_member->leaderGUID = leaderGUID;
        group_member->groupAI = 0;

        sFormationMgr.CreatureGroupMap[lowguid] = group_member;
        creature->SearchFormation();

        WorldDatabase.PExecuteLog("INSERT INTO creature_formations(leaderGUID, memberGUID, dist, angle, groupAI) VALUES('%u' , '%u' , '%f' , '%f' , '%u' )",
            leaderGUID, lowguid, group_member->follow_dist, group_member->follow_angle, uint32(group_member->groupAI));

        handler->PSendSysMessage("Creature %u added to formation with leader %u", lowguid, leaderGUID);

        return true;
    }

    static bool HandleNpcInfoCommand(ChatHandler* handler, const char* /*args*/)
    {
        Creature* target = handler->getSelectedCreature();

        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        CreatureInfo const* cInfo = target->GetCreatureTemplate();
        uint32 faction = target->GetFaction();
        uint32 npcflags = target->GetUInt32Value(UNIT_NPC_FLAGS);
        uint32 displayid = target->GetDisplayId();
        uint32 nativeid = target->GetNativeDisplayId();
        uint32 Entry = target->GetEntry();
        uint32 mechanicImmuneMask = cInfo->MechanicImmuneMask;

        int32 curRespawnDelay = target->GetRespawnTimeEx() - time(nullptr);
        if (curRespawnDelay < 0)
            curRespawnDelay = 0;
        std::string curRespawnDelayStr = secsToTimeString(curRespawnDelay, true);
        std::string defRespawnDelayStr = secsToTimeString(target->GetRespawnDelay(), true);

        handler->PSendSysMessage(LANG_NPCINFO_CHAR, target->GetDBTableGUIDLow(), faction, npcflags, Entry, displayid, nativeid);
        handler->PSendSysMessage(LANG_NPCINFO_LEVEL, target->getLevel());
        handler->PSendSysMessage(LANG_NPCINFO_EQUIPMENT, target->GetCurrentEquipmentId(), target->GetEquipmentId());
        handler->PSendSysMessage(LANG_NPCINFO_HEALTH, target->GetCreateHealth(), target->GetMaxHealth(), target->GetHealth());
        handler->PSendSysMessage(LANG_NPCINFO_INHABIT_TYPE, cInfo->InhabitType);
        handler->PSendSysMessage(LANG_NPCINFO_FLAGS, target->GetUInt32Value(UNIT_FIELD_FLAGS), target->GetUInt32Value(UNIT_DYNAMIC_FLAGS), target->GetFaction());
        handler->PSendSysMessage(LANG_COMMAND_RAWPAWNTIMES, defRespawnDelayStr.c_str(), curRespawnDelayStr.c_str());
        handler->PSendSysMessage(LANG_NPCINFO_LOOT, cInfo->lootid, cInfo->pickpocketLootId, cInfo->SkinLootId);
        handler->PSendSysMessage(LANG_NPCINFO_DUNGEON_ID, target->GetInstanceId());
        handler->PSendSysMessage(LANG_NPCINFO_POSITION, float(target->GetPositionX()), float(target->GetPositionY()), float(target->GetPositionZ()));
        handler->PSendSysMessage(LANG_NPCINFO_AIINFO, target->GetAIName().c_str(), target->GetScriptName().c_str());
        if (const CreatureData* const linked = target->GetLinkedRespawnCreatureData())
            if (CreatureInfo const* master = GetCreatureTemplate(linked->id))
                handler->PSendSysMessage(LANG_NPCINFO_LINKGUID, sObjectMgr.GetLinkedRespawnGuid(target->GetDBTableGUIDLow()), linked->id, master->Name);

        if ((npcflags & UNIT_NPC_FLAG_VENDOR))
            handler->SendSysMessage(LANG_NPCINFO_VENDOR);

        if ((npcflags & UNIT_NPC_FLAG_TRAINER))
            handler->SendSysMessage(LANG_NPCINFO_TRAINER);

        return true;
    }

    static bool HandleNpcNearCommand(ChatHandler* handler, char const* args)
    {
        float distance = (!*args) ? 10.0f : float((atof(args)));
        uint32 count = 0;

        // ANTI LAG
        if (distance > 200.0f)
            distance = 200.0f;

        Player* player = handler->GetSession()->GetPlayer();

        QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT guid, id, position_x, position_y, position_z, map, (POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2))"
            "AS order_ FROM creature WHERE map = '%u' AND (POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2)) <= '%f' ORDER BY order_",
            player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), distance * distance);

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
                uint16 mapId = fields[5].GetUInt16();

                CreatureInfo const* creatureTemplate = GetCreatureTemplate(entry);
                if (!creatureTemplate)
                    continue;

                handler->PSendSysMessage(LANG_CREATURE_LIST_CHAT, guid, guid, creatureTemplate->Name, x, y, z, mapId);

                ++count;
            } while (result->NextRow());
        }

        handler->PSendSysMessage(LANG_COMMAND_NEAR_NPC_MESSAGE, distance, count);

        return true;
    }

    static bool HandleNpcMoveCommand(ChatHandler* handler, const char* args)
    {
        uint32 lowguid = 0;

        Creature* pCreature = handler->getSelectedCreature();

        if (!pCreature)
        {
            // number or [name] Shift-click form |color|Hcreature:creature_guid|h[name]|h|r
            char* cId = handler->extractKeyFromLink((char*)args, "Hcreature");
            if (!cId)
                return false;

            lowguid = atoi(cId);

            /* FIXME: impossibel without entry
            if (lowguid)
                pCreature = ObjectAccessor::GetCreature(*m_session->GetPlayer(),MAKE_GUID(lowguid,HIGHGUID_UNIT));
            */

            // Attempting creature load from DB data
            if (!pCreature)
            {
                CreatureData const* data = sObjectMgr.GetCreatureData(lowguid);
                if (!data)
                {
                    handler->PSendSysMessage(LANG_COMMAND_CREATGUIDNOTFOUND, lowguid);
                    handler->SetSentErrorMessage(true);
                    return false;
                }

                uint32 map_id = data->mapid;

                if (handler->GetSession()->GetPlayer()->GetMapId() != map_id)
                {
                    handler->PSendSysMessage(LANG_COMMAND_CREATUREATSAMEMAP, lowguid);
                    handler->SetSentErrorMessage(true);
                    return false;
                }
            }
            else
                lowguid = pCreature->GetDBTableGUIDLow();
        }
        else
            lowguid = pCreature->GetDBTableGUIDLow();

        float x = handler->GetSession()->GetPlayer()->GetPositionX();
        float y = handler->GetSession()->GetPlayer()->GetPositionY();
        float z = handler->GetSession()->GetPlayer()->GetPositionZ();
        float o = handler->GetSession()->GetPlayer()->GetOrientation();

        if (pCreature)
        {
            if (CreatureData const* data = sObjectMgr.GetCreatureData(pCreature->GetDBTableGUIDLow()))
            {
                const_cast<CreatureData*>(data)->posX = x;
                const_cast<CreatureData*>(data)->posY = y;
                const_cast<CreatureData*>(data)->posZ = z;
                const_cast<CreatureData*>(data)->orientation = o;
            }
            pCreature->GetMap()->CreatureRelocation(pCreature, x, y, z, o);
            pCreature->GetMotionMaster()->Initialize();
            if (pCreature->IsAlive())                            // dead creature will reset movement generator at respawn
            {
                pCreature->setDeathState(DEAD);
                pCreature->Respawn();
            }
        }

        WorldDatabase.PExecuteLog("UPDATE creature SET position_x = '%f', position_y = '%f', position_z = '%f', orientation = '%f' WHERE guid = '%u'", x, y, z, o, lowguid);
        handler->PSendSysMessage(LANG_COMMAND_CREATUREMOVED);
        return true;
    }

    static bool HandleNpcPlayEmoteCommand(ChatHandler* handler, const char* args)
    {
        uint32 emote = atoi((char*)args);

        Creature* target = handler->getSelectedCreature();
        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        target->SetUInt32Value(UNIT_NPC_EMOTESTATE, emote);

        return true;
    }

    static bool HandleNpcSayCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Creature* pCreature = handler->getSelectedCreature();
        if (!pCreature)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        pCreature->Say(args, LANG_UNIVERSAL, 0);

        // make some emotes
        char lastchar = args[strlen(args) - 1];
        switch (lastchar)
        {
        case '?':
            pCreature->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);
            break;
        case '!':
            pCreature->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
            break;
        default:
            pCreature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
            break;
        }

        return true;
    }

    static bool HandleNpcTextEmoteCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Creature* pCreature = handler->getSelectedCreature();

        if (!pCreature)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        pCreature->TextEmote(args, 0);

        return true;
    }

    static bool HandleNpcYellCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Creature* pCreature = handler->getSelectedCreature();
        if (!pCreature)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        pCreature->Yell(args, LANG_UNIVERSAL, 0);

        // make an emote
        pCreature->HandleEmoteCommand(EMOTE_ONESHOT_SHOUT);

        return true;
    }

    static bool HandleNpcWhisperCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* receiver_str = strtok((char*)args, " ");
        char* text = strtok(NULL, "");

        uint64 guid = handler->GetSession()->GetPlayer()->GetSelection();
        Creature* pCreature = handler->GetSession()->GetPlayer()->GetMap()->GetCreature(guid);

        if (!pCreature || !receiver_str || !text)
            return false;

        uint64 receiver_guid = atol(receiver_str);

        pCreature->Whisper(text, receiver_guid);

        return true;
    }

    static bool HandleNpcAddCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;
        char* charID = strtok((char*)args, " ");
        if (!charID)
            return false;

        char* team = strtok(NULL, " ");
        int32 teamval = 0;
        if (team)
            teamval = atoi(team);
        if (teamval < 0)
            teamval = 0;

        uint32 id = atoi(charID);

        Player* chr = handler->GetSession()->GetPlayer();
        float x = chr->GetPositionX();
        float y = chr->GetPositionY();
        float z = chr->GetPositionZ();
        float o = chr->GetOrientation();
        Map* map = chr->GetMap();

        Creature* creature = new Creature;
        if (!creature->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_UNIT), map, PHASEMASK_NORMAL, id, (uint32)teamval, x, y, z, o))
        {
            delete creature;
            return false;
        }

        creature->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), chr->GetPhaseMaskForSpawn());

        uint32 db_guid = creature->GetDBTableGUIDLow();

        // To call _LoadGoods(); _LoadQuests(); CreateTrainerSpells();
        if (!creature->LoadCreatureFromDB(db_guid, map))
        {
            delete creature;
            return false;
        }

        sObjectMgr.AddCreatureToGrid(db_guid, sObjectMgr.GetCreatureData(db_guid));

        return true;
    }

    static bool HandleNpcAddTempCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;
        char* charID = strtok((char*)args, " ");
        if (!charID)
            return false;

        Player* chr = handler->GetSession()->GetPlayer();

        float x = chr->GetPositionX();
        float y = chr->GetPositionY();
        float z = chr->GetPositionZ();
        float ang = chr->GetOrientation();

        uint32 id = atoi(charID);

        chr->SummonCreature(id, x, y, z, ang, TEMPSUMMON_CORPSE_DESPAWN, 120);

        return true;
    }

    static bool HandleNpcAddItemCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* pitem = handler->extractKeyFromLink((char*)args, "Hitem");
        if (!pitem)
        {
            handler->SendSysMessage(LANG_COMMAND_NEEDITEMSEND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 itemId = atol(pitem);

        char* fmaxcount = strtok(NULL, " ");                    //add maxcount, default: 0
        uint32 maxcount = 0;
        if (fmaxcount)
            maxcount = atol(fmaxcount);

        char* fincrtime = strtok(NULL, " ");                    //add incrtime, default: 0
        uint32 incrtime = 0;
        if (fincrtime)
            incrtime = atol(fincrtime);

        char* fextendedcost = strtok(NULL, " ");                //add ExtendedCost, default: 0
        uint32 extendedcost = fextendedcost ? atol(fextendedcost) : 0;

        Creature* vendor = handler->getSelectedCreature();

        char* addMulti = strtok(NULL, " ");
        uint32 vendor_entry = addMulti ? handler->GetSession()->GetCurrentVendor() : vendor ? vendor->GetEntry() : 0;

        if (!sObjectMgr.IsVendorItemValid(vendor_entry, itemId, maxcount, incrtime, extendedcost, handler->GetSession()->GetPlayer()))
        {
            handler->SetSentErrorMessage(true);
            return false;
        }

        sObjectMgr.AddVendorItem(vendor_entry, itemId, maxcount, incrtime, extendedcost);

        ItemTemplate const* pProto = sObjectMgr.GetItemTemplate(itemId);

        handler->PSendSysMessage(LANG_ITEM_ADDED_TO_LIST, itemId, pProto->Name1, maxcount, incrtime, extendedcost);
        return true;
    }

};

void AddSC_npc_commandscript()
{
    new npc_commandscript();
}