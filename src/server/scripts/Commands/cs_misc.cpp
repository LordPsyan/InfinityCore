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
#include "InstanceSaveMgr.h"
#include "PointMovementGenerator.h"
#include "TargetedMovementGenerator.h"

// FOR PINFO
static uint32 ReputationRankStrIndex[MAX_REPUTATION_RANK] =
{
    LANG_REP_HATED,    LANG_REP_HOSTILE, LANG_REP_UNFRIENDLY, LANG_REP_NEUTRAL,
    LANG_REP_FRIENDLY, LANG_REP_HONORED, LANG_REP_REVERED,    LANG_REP_EXALTED
};

class misc_commandscript : public CommandScript
{
public:
    misc_commandscript() : CommandScript("misc_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> referFriendCommandTable =
        {
            { "info",               SEC_GAMEMASTER,            true,  &HandleRAFInfoCommand,                           "" },
            { "link",               SEC_GAMEMASTER,            true,  &HandleRAFLinkCommand,                           "" },
            { "unlink",             SEC_GAMEMASTER,            true,  &HandleRAFUnlinkCommand,                         "" },
            { "summon",             SEC_PLAYER,                false, &HandleRAFSummonCommand,                         "" },
            { "grantlevel",         SEC_PLAYER,                false, &HandleRAFGrantLevelCommand,                     "" },
        };

        static std::vector<ChatCommand> sendCommandTable =
        {
            { "items",              SEC_ADMINISTRATOR,         true, &HandleSendItemCommand,                          "" },
            { "mail",               SEC_MODERATOR,             true, &HandleSendMailCommand,                          "" },
            { "message",            SEC_ADMINISTRATOR,         true, &HandleSendMessageCommand,                       "" },
            { "money",              SEC_ADMINISTRATOR,         true, &HandleSendMoneyCommand,                         "" }
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "help",               SEC_MODERATOR,             false, &HandleHELPCommand,                              "" },
            { "aura",               SEC_ADMINISTRATOR,         false, &HandleAURACommand,                              "" },
            { "unaura",             SEC_ADMINISTRATOR,         false, &HandleUNAURACommand,                            "" },
            { "appear",             SEC_ADMINISTRATOR,         false, &HandleAPPEARCommand,                            "" },
            { "summon",             SEC_ADMINISTRATOR,         false, &HandleSUMMONCommand,                            "" },
            { "die",                SEC_ADMINISTRATOR,         false, &HandleDIECommand,                               "" },
            { "revive",             SEC_ADMINISTRATOR,         true, &HandleREVIVECommand,                            "" },
            { "dismount",           SEC_PLAYER,                false, &HandleDISMOUNTCommand,                          "" },
            { "guid",               SEC_GAMEMASTER,            false, &HandleGUIDCommand,                              "" },
            { "itemmove",           SEC_GAMEMASTER,            false, &HandleITEMMOVECommand,                          "" },
            { "cooldown",           SEC_ADMINISTRATOR,         false, &HandleCOOLDOWNCommand,                          "" },
            { "distance",           SEC_ADMINISTRATOR,         false, &HandleDISTANCECommand,                          "" },
            { "recall",             SEC_MODERATOR,             false, &HandleRECALLCommand,                            "" },
            { "save",               SEC_PLAYER,                false, &HandleSAVECommand,                              "" },
            { "saveall",            SEC_MODERATOR,             true, &HandleSAVEALLCommand,                           "" },
            { "kick",               SEC_GAMEMASTER,            true, &HandleKICKCommand,                              "" },
            { "linkgrave",          SEC_ADMINISTRATOR,         false, &HandleLINKGRAVECommand,                         "" },
            { "neargrave",          SEC_ADMINISTRATOR,         false, &HandleNEARGRAVECommand,                         "" },
            { "showarea",           SEC_ADMINISTRATOR,         false, &HandleSHOWAREACommand,                          "" },
            { "hidearea",           SEC_ADMINISTRATOR,         false, &HandleHIDEAREACommand,                          "" },
            { "additem",            SEC_ADMINISTRATOR,         false, &HandleAddItemCommand,                           "" },
            { "additemset",         SEC_ADMINISTRATOR,         false, &HandleAddItemSetCommand,                        "" },
            { "bank",               SEC_ADMINISTRATOR,         false, &HandleBankCommand,                              "" },
            { "wchange",            SEC_ADMINISTRATOR,         false, &HandleChangeWeatherCommand,                     "" },
            { "maxskill",           SEC_ADMINISTRATOR,         false, &HandleMaxSkillCommand,                          "" },
            { "setskill",           SEC_ADMINISTRATOR,         false, &HandleSetSkillCommand,                          "" },
            { "pinfo",              SEC_GAMEMASTER,            false, &HandlePInfoCommand,                             "" },
            { "respawn",            SEC_ADMINISTRATOR,         false, &HandleRespawnCommand,                           "" },
            { "gps",                SEC_MODERATOR,             false, &HandleGPSCommand,                               "" },
            { "levelup",            SEC_MODERATOR,             false, &HandleLevelUpCommand,                           "" },
            { "mute",               SEC_GAMEMASTER,            false, &HandleMuteCommand,                              "" },
            { "unmute",             SEC_GAMEMASTER,            false, &HandleUnmuteCommand,                            "" },
            { "cometome",           SEC_ADMINISTRATOR,         false, &HandleComeToMeCommand,                          "" },
            { "movegens",           SEC_ADMINISTRATOR,         false, &HandleMovegensCommand,                          "" },
            { "damage",             SEC_ADMINISTRATOR,         false, &HandleDamageCommand,                            "" },
            { "combatstop",         SEC_GAMEMASTER,            false, &HandleCombatStopCommand,                        "" },
            { "ahbotoptions",       SEC_ADMINISTRATOR,         false, &HandleAHBotOptionsCommand,                      "" },
            { "flusharenapoints",   SEC_ADMINISTRATOR,         false, &HandleFlushArenaPointsCommand,                  "" },
            { "playall",            SEC_ADMINISTRATOR,         false, &HandlePlayAllCommand,                           "" },
            { "repairitems",        SEC_GAMEMASTER,            false, &HandleRepairitemsCommand,                       "" },
            { "freeze",             SEC_ADMINISTRATOR,         false, &HandleFreezeCommand,                            "" },
            { "unfreeze",           SEC_ADMINISTRATOR,         false, &HandleUnFreezeCommand,                          "" },
            { "listfreeze",         SEC_ADMINISTRATOR,         false, &HandleListFreezeCommand,                        "" },
            { "possess",            SEC_ADMINISTRATOR,         false, &HandlePossessCommand,                           "" },
            { "unpossess",          SEC_ADMINISTRATOR,         false, &HandleUnPossessCommand,                         "" },
            { "bindsight",          SEC_ADMINISTRATOR,         false, &HandleBindSightCommand,                         "" },
            { "unbindsight",        SEC_ADMINISTRATOR,         false, &HandleUnbindSightCommand,                       "" },
            { "recall",             SEC_MODERATOR,             false, &HandleRecallCommand,                            "" },
            { "commands",           SEC_PLAYER,                true,  &HandleCommandsCommand,                          "" },
            { "start",              SEC_PLAYER,                false, &HandleStartCommand,                             "" },
            { "allowmove",          SEC_ADMINISTRATOR,         false, &HandleAllowMovementCommand,                     "" },
            { "raf",                SEC_ADMINISTRATOR,         true,  NULL,                                            "", referFriendCommandTable },
            { "send",               SEC_MODERATOR,             true, nullptr,                                         "", sendCommandTable }
            
        };
        return commandTable;
    }

    static bool HandleLevelUpCommand(ChatHandler* handler, char const* args)
    {
        char* px = strtok((char*)args, " ");
        char* py = strtok((char*)NULL, " ");

        // command format parsing
        char* pname = (char*)NULL;
        int addlevel = 1;

        if (px && py)                                            // .levelup name level
        {
            addlevel = atoi(py);
            pname = px;
        }
        else if (px && !py)                                      // .levelup name OR .levelup level
        {
            if (isalpha(px[0]))                                  // .levelup name
                pname = px;
            else                                                // .levelup level
                addlevel = atoi(px);
        }
        // else .levelup - nothing do for preparing

        // player
        Player* chr = NULL;
        uint64 chr_guid = 0;

        std::string name;

        if (pname)                                               // player by name
        {
            name = pname;
            if (!normalizePlayerName(name))
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            chr = sObjectMgr.GetPlayer(name.c_str());
            if (!chr)                                            // not in game
            {
                chr_guid = sObjectMgr.GetPlayerGUIDByName(name);
                if (chr_guid == 0)
                {
                    handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                    handler->SetSentErrorMessage(true);
                    return false;
                }
            }
        }
        else                                                    // player by selection
        {
            chr = handler->getSelectedPlayer();

            if (chr == NULL)
            {
                handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
                handler->SetSentErrorMessage(true);
                return false;
            }

            name = chr->GetName();
        }

        ASSERT(chr || chr_guid);

        int32 oldlevel = chr ? chr->getLevel() : Player::GetUInt32ValueFromDB(UNIT_FIELD_LEVEL, chr_guid);
        int32 newlevel = oldlevel + addlevel;

        if (newlevel < 1)
            newlevel = 1;

        if (newlevel > STRONG_MAX_LEVEL)                         // hardcoded maximum level
            newlevel = STRONG_MAX_LEVEL;

        if (chr)
        {
            chr->GiveLevel(newlevel);
            chr->InitTalentForLevel();
            chr->SetUInt32Value(PLAYER_XP, 0);

            if (oldlevel == newlevel)
                ChatHandler(chr).SendSysMessage(LANG_YOURS_LEVEL_PROGRESS_RESET);
            else if (oldlevel < newlevel)
                ChatHandler(chr).PSendSysMessage(LANG_YOURS_LEVEL_UP, newlevel - oldlevel);
            else if (oldlevel > newlevel)
                ChatHandler(chr).PSendSysMessage(LANG_YOURS_LEVEL_DOWN, newlevel - oldlevel);
        }
        else
        {
            // update level and XP at level, all other will be updated at loading
            CharacterDatabase.PExecute("UPDATE characters SET level = '%u', xp = 0 WHERE guid = '" UI64FMTD "'", newlevel, chr_guid);
        }

        if (handler->GetSession()->GetPlayer() != chr)                       // including chr == NULL
            handler->PSendSysMessage(LANG_YOU_CHANGE_LVL, name.c_str(), newlevel);
        return true;
    }

    static bool HandleGPSCommand(ChatHandler* handler, char const* args)
    {
        WorldObject* obj = NULL;
        if (*args)
        {
            std::string name = args;
            if (normalizePlayerName(name))
                obj = sObjectMgr.GetPlayer(name.c_str());

            if (!obj)
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        else
        {
            obj = handler->getSelectedUnit();

            if (!obj)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        CellCoord cell_val = Oregon::ComputeCellCoord(obj->GetPositionX(), obj->GetPositionY());
        Cell cell(cell_val);

        uint32 zone_id = obj->GetZoneId();
        uint32 area_id = obj->GetAreaId();

        MapEntry const* mapEntry = sMapStore.LookupEntry(obj->GetMapId());
        AreaTableEntry const* zoneEntry = GetAreaEntryByAreaID(zone_id);
        AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(area_id);

        float zone_x = obj->GetPositionX();
        float zone_y = obj->GetPositionY();

        Map2ZoneCoordinates(zone_x, zone_y, zone_id);

        Map const* map = obj->GetMap();
        float ground_z = map->GetHeight(obj->GetPositionX(), obj->GetPositionY(), MAX_HEIGHT);
        float floor_z = map->GetHeight(obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ());

        GridCoord p = Oregon::ComputeGridCoord(obj->GetPositionX(), obj->GetPositionY());

        int gx = 63 - p.x_coord;
        int gy = 63 - p.y_coord;

        uint32 have_map = Map::ExistMap(obj->GetMapId(), gx, gy) ? 1 : 0;
        uint32 have_vmap = Map::ExistVMap(obj->GetMapId(), gx, gy) ? 1 : 0;

        if (have_vmap)
        {
            if (map->IsOutdoors(obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ()))
                handler->PSendSysMessage("You are outdoors");
            else
                handler->PSendSysMessage("You are indoor");
        }
        else handler->PSendSysMessage("no VMAP available for area info");

        handler->PSendSysMessage(LANG_MAP_POSITION,
            obj->GetMapId(), (mapEntry ? mapEntry->name[handler->GetSession()->GetSessionDbcLocale()] : "<unknown>"),
            zone_id, (zoneEntry ? zoneEntry->area_name[handler->GetSession()->GetSessionDbcLocale()] : "<unknown>"),
            area_id, (areaEntry ? areaEntry->area_name[handler->GetSession()->GetSessionDbcLocale()] : "<unknown>"),
            obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), obj->GetOrientation(),
            cell.GridX(), cell.GridY(), cell.CellX(), cell.CellY(), obj->GetInstanceId(),
            zone_x, zone_y, ground_z, floor_z, have_map, have_vmap);

        sLog.outDebug("Player %s GPS call for %s '%s' (%s: %u):",
            handler->GetName(),
            (obj->GetTypeId() == TYPEID_PLAYER ? "player" : "creature"), obj->GetName(),
            (obj->GetTypeId() == TYPEID_PLAYER ? "GUID" : "Entry"), (obj->GetTypeId() == TYPEID_PLAYER ? obj->GetGUIDLow() : obj->GetEntry()));
        sLog.outDebug(handler->GetOregonString(LANG_MAP_POSITION),
            obj->GetMapId(), (mapEntry ? mapEntry->name[sWorld.GetDefaultDbcLocale()] : "<unknown>"),
            zone_id, (zoneEntry ? zoneEntry->area_name[sWorld.GetDefaultDbcLocale()] : "<unknown>"),
            area_id, (areaEntry ? areaEntry->area_name[sWorld.GetDefaultDbcLocale()] : "<unknown>"),
            obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), obj->GetOrientation(),
            cell.GridX(), cell.GridY(), cell.CellX(), cell.CellY(), obj->GetInstanceId(),
            zone_x, zone_y, ground_z, floor_z, have_map, have_vmap);

        LiquidData liquid_status;
        ZLiquidStatus res = map->getLiquidStatus(obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), MAP_ALL_LIQUIDS, &liquid_status);
        if (res)
            handler->PSendSysMessage(LANG_LIQUID_STATUS, liquid_status.level, liquid_status.depth_level, liquid_status.entry, liquid_status.type_flags, res);
        return true;
    }

    static bool HandleHELPCommand(ChatHandler* handler, char const* args)
    {
        char* cmd = strtok((char*)args, " ");
        if (!cmd)
        {
            handler->ShowHelpForCommand(handler->getCommandTable(), "help");
            handler->ShowHelpForCommand(handler->getCommandTable(), "");
        }
        else
        {
            if (!handler->ShowHelpForCommand(handler->getCommandTable(), cmd))
                handler->SendSysMessage(LANG_NO_HELP_CMD);
        }

        return true;
    }

    static bool HandleAURACommand(ChatHandler* handler, char const* args)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 spellID = handler->extractSpellIdFromLink((char*)args);
        if (!spellID)
            return false;

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellID);
        if (spellInfo)
        {
            for (uint32 i = 0; i < 3; i++)
            {
                uint8 eff = spellInfo->Effect[i];
                if (eff >= TOTAL_SPELL_EFFECTS)
                    continue;
                if (IsAreaAuraEffect(eff) ||
                    eff == SPELL_EFFECT_APPLY_AURA ||
                    eff == SPELL_EFFECT_PERSISTENT_AREA_AURA)
                {
                    Aura* Aur = CreateAura(spellInfo, i, NULL, target);
                    target->AddAura(Aur);
                }
            }
        }

        return true;
    }

    static bool HandleUNAURACommand(ChatHandler* handler, char const* args)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string argstr = args;
        if (argstr == "all")
        {
            target->RemoveAllAuras();
            return true;
        }

        uint32 spellID = handler->extractSpellIdFromLink((char*)args);
        if (!spellID)
            return false;
        target->RemoveAurasDueToSpell(spellID);

        return true;
    }

    static bool HandleAPPEARCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* _player = handler->GetSession()->GetPlayer();

        std::string name = args;

        if (!normalizePlayerName(name))
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = sObjectMgr.GetPlayer(name.c_str());
        if (target)
        {
            Map* cMap = target->GetMap();
            if (cMap->IsBattlegroundOrArena())
            {
                // only allow if gm mode is on
                if (!_player->IsGameMaster())
                {
                    handler->PSendSysMessage(LANG_CANNOT_GO_TO_BG_GM, target->GetName());
                    handler->SetSentErrorMessage(true);
                    return false;
                }
                // if both players are in different bgs
                else if (_player->GetBattlegroundId() && _player->GetBattlegroundId() != target->GetBattlegroundId())
                    _player->LeaveBattleground(false); // Note: should be changed so _player gets no Deserter debuff

                // all's well, set bg id
                // when porting out from the bg, it will be reset to 0
                _player->SetBattlegroundId(target->GetBattlegroundId());
                // remember current position as entry point for return at bg end teleportation
                if (!_player->GetMap()->IsBattlegroundOrArena())
                    _player->SetBattlegroundEntryPoint();
            }
            else if (cMap->IsDungeon())
            {
                // we have to go to instance, and can go to player only if:
                //   1) we are in his group (either as leader or as member)
                //   2) we are not bound to any group and have GM mode on
                if (_player->GetGroup())
                {
                    // we are in group, we can go only if we are in the player group
                    if (_player->GetGroup() != target->GetGroup())
                    {
                        handler->PSendSysMessage(LANG_CANNOT_GO_TO_INST_PARTY, target->GetName());
                        handler->SetSentErrorMessage(true);
                        return false;
                    }
                }
                else
                {
                    // we are not in group, let's verify our GM mode
                    if (!_player->IsGameMaster())
                    {
                        handler->PSendSysMessage(LANG_CANNOT_GO_TO_INST_GM, target->GetName());
                        handler->SetSentErrorMessage(true);
                        return false;
                    }
                }

                // if the player or the player's group is bound to another instance
                // the player will not be bound to another one
                InstancePlayerBind *pBind = _player->GetBoundInstance(target->GetMapId(), target->GetDifficulty());
                if (!pBind)
                {
                    Group *group = _player->GetGroup();
                    // if no bind exists, create a solo bind
                    InstanceGroupBind *gBind = group ? group->GetBoundInstance(target) : NULL;                // if no bind exists, create a solo bind
                    if (!gBind)
                        if (InstanceSave *save = sInstanceSaveMgr.GetInstanceSave(target->GetInstanceId()))
                            _player->BindToInstance(save, !save->CanReset());
                }

                _player->SetDifficulty(target->GetDifficulty());
            }

            handler->PSendSysMessage(LANG_APPEARING_AT, target->GetName());

            //if (_player->IsVisibleGloballyFor(target))
            //    ChatHandler(target).PSendSysMessage(LANG_APPEARING_TO, _player->GetName());

            // stop flight if need
            if (_player->IsInFlight())
            {
                _player->GetMotionMaster()->MovementExpired();
                _player->CleanupAfterTaxiFlight();
            }
            // save only in non-flight case
            else
                _player->SaveRecallPosition();

            // to point to see at target with same orientation
            float x, y, z;
            target->GetPosition(x, y, z);

            if (_player->HasUnitMovementFlag(MOVEMENTFLAG_FLYING) ||
                target->HasUnitMovementFlag(MOVEMENTFLAG_FLYING))
            {
                WorldPacket data;
                data.SetOpcode(SMSG_MOVE_SET_CAN_FLY);
                data << _player->GetPackGUID();
                data << uint32(0);                                      // unknown
                _player->SendMessageToSet(&data, true);
            }

            _player->TeleportTo(target->GetMapId(), x, y, z, _player->GetAngle(target), TELE_TO_GM_MODE);

            return true;
        }

        if (uint64 guid = sObjectMgr.GetPlayerGUIDByName(name))
        {
            handler->PSendSysMessage(LANG_APPEARING_AT, name.c_str());

            // to point where player stay (if loaded)
            float x, y, z, o;
            uint32 map;
            bool in_flight;
            if (!Player::LoadPositionFromDB(map, x, y, z, o, in_flight, guid))
                return false;

            // stop flight if need
            if (_player->IsInFlight())
            {
                _player->GetMotionMaster()->MovementExpired();
                _player->CleanupAfterTaxiFlight();
            }
            // save only in non-flight case
            else
                _player->SaveRecallPosition();

            _player->TeleportTo(map, x, y, z, _player->GetOrientation());
            return true;
        }

        handler->PSendSysMessage(LANG_NO_PLAYER, args);

        handler->SetSentErrorMessage(true);
        return false;
    }

    static bool HandleSUMMONCommand(ChatHandler* handler, char const* args)
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

        Player* target = sObjectMgr.GetPlayer(name.c_str());
        if (target)
        {
            if (target->IsBeingTeleported())
            {
                handler->PSendSysMessage(LANG_IS_TELEPORTED, target->GetName());
                handler->SetSentErrorMessage(true);
                return false;
            }

            Map* pMap = handler->GetSession()->GetPlayer()->GetMap();

            if (pMap->IsBattlegroundOrArena())
            {
                // only allow if gm mode is on
                if (!target->IsGameMaster())
                {
                    handler->PSendSysMessage(LANG_CANNOT_GO_TO_BG_GM, target->GetName());
                    handler->SetSentErrorMessage(true);
                    return false;
                }
                // if both players are in different bgs
                else if (target->GetBattlegroundId() && handler->GetSession()->GetPlayer()->GetBattlegroundId() != target->GetBattlegroundId())
                {
                    handler->PSendSysMessage(LANG_CANNOT_GO_TO_BG_FROM_BG, target->GetName());
                    handler->SetSentErrorMessage(true);
                    return false;
                }
                // all's well, set bg id
                // when porting out from the bg, it will be reset to 0
                target->SetBattlegroundId(handler->GetSession()->GetPlayer()->GetBattlegroundId());
                // remember current position as entry point for return at bg end teleportation
                if (!target->GetMap()->IsBattlegroundOrArena())
                    target->SetBattlegroundEntryPoint();
            }
            else if (pMap->IsDungeon())
            {
                Map* cMap = target->GetMap();
                if (cMap->Instanceable() && cMap->GetInstanceId() != pMap->GetInstanceId())
                {
                    // cannot summon from instance to instance
                    handler->PSendSysMessage(LANG_CANNOT_SUMMON_TO_INST, target->GetName());
                    handler->SetSentErrorMessage(true);
                    return false;
                }

                // we are in instance, and can summon only player in our group with us as lead
                if (!handler->GetSession()->GetPlayer()->GetGroup() || !target->GetGroup() ||
                    (target->GetGroup()->GetLeaderGUID() != handler->GetSession()->GetPlayer()->GetGUID()) ||
                    (handler->GetSession()->GetPlayer()->GetGroup()->GetLeaderGUID() != handler->GetSession()->GetPlayer()->GetGUID()))
                    // the last check is a bit excessive, but let it be, just in case
                {
                    handler->PSendSysMessage(LANG_CANNOT_SUMMON_TO_INST, target->GetName());
                    handler->SetSentErrorMessage(true);
                    return false;
                }
            }

            handler->PSendSysMessage(LANG_SUMMONING, target->GetName(), "");
            if (handler->needReportToTarget(target))
                ChatHandler(target).PSendSysMessage(LANG_SUMMONED_BY, handler->GetName());

            // stop flight if need
            if (target->IsInFlight())
            {
                target->GetMotionMaster()->MovementExpired();
                target->CleanupAfterTaxiFlight();
            }
            // save only in non-flight case
            else
                target->SaveRecallPosition();

            // before GM
            float x, y, z;
            handler->GetSession()->GetPlayer()->GetClosePoint(x, y, z, target->GetObjectSize());
            target->TeleportTo(handler->GetSession()->GetPlayer()->GetMapId(), x, y, z, target->GetOrientation());
        }
        else if (uint64 guid = sObjectMgr.GetPlayerGUIDByName(name))
        {
            handler->PSendSysMessage(LANG_SUMMONING, name.c_str(), handler->GetOregonString(LANG_OFFLINE));

            Player* _player = handler->GetSession()->GetPlayer();
            Player::SavePositionInDB(_player->GetMapId(),
                _player->GetPositionX(),
                _player->GetPositionY(),
                _player->GetPositionZ(),
                _player->GetOrientation(),
                _player->GetZoneId(),
                guid);
        }
        else
        {
            handler->PSendSysMessage(LANG_NO_PLAYER, args);
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    static bool HandleDIECommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* target = handler->getSelectedUnit();

        if (!target || !handler->GetSession()->GetPlayer()->GetSelection())
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target->IsAlive())
        {
            if (sWorld.getConfig(CONFIG_DIE_COMMAND_MODE))
                target->CastSpell(target, SPELL_SUICIDE, true);
            else
                handler->GetSession()->GetPlayer()->CastSpell(target, SPELL_DEATH_TOUCH, true);
        }

        return true;
    }

    static bool HandleREVIVECommand(ChatHandler* handler, char const* args)
    {
        Player* player = nullptr;
        uint32 player_guid = 0;

        if (*args)
        {
            std::string name = args;
            if (!normalizePlayerName(name))
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            player = sObjectMgr.GetPlayer(name.c_str());
            if (!player)
                player_guid = sObjectMgr.GetPlayerGUIDByName(name);
        }
        else
            player = handler->getSelectedPlayer();

        if (player)
        {
            player->ResurrectPlayer(player->GetSession()->GetSecurity() > SEC_PLAYER ? 1.0f : 0.5f);
            player->SpawnCorpseBones();
            player->SaveToDB();
        }
        else if (player_guid)
        {
            // will resurrected at login without corpse
            ObjectAccessor::Instance().ConvertCorpseForPlayer(player_guid);
        }
        else
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    static bool HandleDISMOUNTCommand(ChatHandler* handler, char const* /*args*/)
    {
        //If player is not mounted, so go out :)
        if (!handler->GetSession()->GetPlayer()->IsMounted())
        {
            handler->SendSysMessage(LANG_CHAR_NON_MOUNTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (handler->GetSession()->GetPlayer()->IsInFlight())
        {
            handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->GetSession()->GetPlayer()->Dismount();
        handler->GetSession()->GetPlayer()->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
        return true;
    }

    static bool HandleGUIDCommand(ChatHandler* handler, char const* /*args*/)
    {
        uint64 guid = handler->GetSession()->GetPlayer()->GetSelection();

        if (guid == 0)
        {
            handler->SendSysMessage(LANG_NO_SELECTION);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_OBJECT_GUID, GUID_LOPART(guid), GUID_HIPART(guid));
        return true;
    }

    static bool HandleITEMMOVECommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;
        uint8 srcslot, dstslot;

        char* pParam1 = strtok((char*)args, " ");
        if (!pParam1)
            return false;

        char* pParam2 = strtok(NULL, " ");
        if (!pParam2)
            return false;

        srcslot = (uint8)atoi(pParam1);
        dstslot = (uint8)atoi(pParam2);

        if (srcslot == dstslot)
            return true;

        if (!handler->GetSession()->GetPlayer()->IsValidPos(INVENTORY_SLOT_BAG_0, srcslot))
            return false;

        if (!handler->GetSession()->GetPlayer()->IsValidPos(INVENTORY_SLOT_BAG_0, dstslot))
            return false;

        uint16 src = ((INVENTORY_SLOT_BAG_0 << 8) | srcslot);
        uint16 dst = ((INVENTORY_SLOT_BAG_0 << 8) | dstslot);

        handler->GetSession()->GetPlayer()->SwapItem(src, dst);

        return true;
    }

    static bool HandleCOOLDOWNCommand(ChatHandler* handler, char const* args)
    {
        Player* target = handler->getSelectedPlayerOrSelf();
        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!*args)
        {
            target->RemoveAllSpellCooldown();
            handler->PSendSysMessage(LANG_REMOVEALL_COOLDOWN, target->GetName());
        }
        else
        {
            // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
            uint32 spell_id = handler->extractSpellIdFromLink((char*)args);
            if (!spell_id)
                return false;

            if (!sSpellStore.LookupEntry(spell_id))
            {
                handler->PSendSysMessage(LANG_UNKNOWN_SPELL, target == handler->GetSession()->GetPlayer() ? handler->GetOregonString(LANG_YOU) : target->GetName());
                handler->SetSentErrorMessage(true);
                return false;
            }

            target->RemoveSpellCooldown(spell_id, true);
            handler->PSendSysMessage(LANG_REMOVE_COOLDOWN, spell_id, target == handler->GetSession()->GetPlayer() ? handler->GetOregonString(LANG_YOU) : target->GetName());
        }
        return true;
    }

    static bool HandleDISTANCECommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* pUnit = handler->getSelectedUnit();

        if (!pUnit)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_DISTANCE, handler->GetSession()->GetPlayer()->GetDistance(pUnit), handler->GetSession()->GetPlayer()->GetDistance2d(pUnit));

        return true;
    }

    static bool HandleRECALLCommand(ChatHandler* handler, char const* args)
    {
        Player* target = nullptr;

        if (!*args)
        {
            target = handler->getSelectedPlayerOrSelf();
        }
        else
        {
            std::string name = args;

            if (!normalizePlayerName(name))
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            target = sObjectMgr.GetPlayer(name.c_str());

            if (!target)
            {
                handler->PSendSysMessage(LANG_NO_PLAYER, args);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        if (target->IsBeingTeleported())
        {
            handler->PSendSysMessage(LANG_IS_TELEPORTED, target->GetName());
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (target->IsInFlight())
        {
            target->GetMotionMaster()->MovementExpired();
            target->m_taxi.ClearTaxiDestinations();
        }

        target->TeleportTo(target->m_recallMap, target->m_recallX, target->m_recallY, target->m_recallZ, target->m_recallO);
        return true;
    }

    static bool HandleSAVECommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        // save GM account without delay and output message (testing, etc)
        if (handler->GetSession()->GetSecurity())
        {
            if (Player* target = handler->getSelectedPlayer())
                target->SaveToDB();
            else
                player->SaveToDB();
            handler->SendSysMessage(LANG_PLAYER_SAVED);
            return true;
        }

        // save or plan save after 20 sec (logout delay) if current next save time more this value and _not_ output any messages to prevent cheat planning
        uint32 save_interval = sWorld.getConfig(CONFIG_INTERVAL_SAVE);
        if (save_interval == 0 || (save_interval > 20 * IN_MILLISECONDS && player->GetSaveTimer() <= save_interval - 20 * IN_MILLISECONDS))
            player->SaveToDB();

        return true;
    }

    static bool HandleSAVEALLCommand(ChatHandler* handler, char const* /*args*/)
    {
        ObjectAccessor::Instance().SaveAllPlayers();
        handler->SendSysMessage(LANG_PLAYERS_SAVED);
        return true;
    }

    static bool HandleKICKCommand(ChatHandler* handler, char const* args)
    {
        const char* kickName = strtok((char*)args, " ");
        char* kickReason = strtok(NULL, "\n");
        std::string reason = "No Reason";
        std::string kicker = "Console";
        if (kickReason)
            reason = kickReason;
        if (handler->GetSession())
            kicker = handler->GetSession()->GetPlayer()->GetName();

        if (!kickName)
        {
            Player* player = handler->getSelectedPlayer();
            if (!player)
            {
                handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (player == handler->GetSession()->GetPlayer())
            {
                handler->SendSysMessage(LANG_COMMAND_KICKSELF);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (sWorld.getConfig(CONFIG_SHOW_KICK_IN_WORLD) == 1)
                sWorld.SendWorldText(LANG_COMMAND_KICKMESSAGE, player->GetName(), kicker.c_str(), reason.c_str());
            else
                handler->PSendSysMessage(LANG_COMMAND_KICKMESSAGE, player->GetName(), kicker.c_str(), reason.c_str());

            player->GetSession()->KickPlayer();
        }
        else
        {
            std::string name = kickName;
            if (!normalizePlayerName(name))
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (handler->GetSession() && name == handler->GetSession()->GetPlayer()->GetName())
            {
                handler->SendSysMessage(LANG_COMMAND_KICKSELF);
                handler->SetSentErrorMessage(true);
                return false;
            }

            Player* player = sObjectMgr.GetPlayer(kickName);
            if (!player)
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (handler->GetSession() && player->GetSession()->GetSecurity() > handler->GetSession()->GetSecurity())
            {
                handler->SendSysMessage(LANG_YOURS_SECURITY_IS_LOW); //maybe replacement string for this later on
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (sWorld.KickPlayer(name.c_str()))
            {
                if (sWorld.getConfig(CONFIG_SHOW_KICK_IN_WORLD) == 1)

                    sWorld.SendWorldText(LANG_COMMAND_KICKMESSAGE, name.c_str(), kicker.c_str(), reason.c_str());
                else
                    handler->PSendSysMessage(LANG_COMMAND_KICKMESSAGE, name.c_str(), kicker.c_str(), reason.c_str());
            }
            else
            {
                handler->PSendSysMessage(LANG_COMMAND_KICKNOTFOUNDPLAYER, name.c_str());
                return false;
            }
        }
        return true;
    }

    static bool HandleLINKGRAVECommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* px = strtok((char*)args, " ");
        if (!px)
            return false;

        uint32 g_id = (uint32)atoi(px);

        uint32 g_team;

        char* px2 = strtok(NULL, " ");

        if (!px2)
            g_team = 0;
        else if (strncmp(px2, "horde", 6) == 0)
            g_team = HORDE;
        else if (strncmp(px2, "alliance", 9) == 0)
            g_team = ALLIANCE;
        else
            return false;

        WorldSafeLocsEntry const* graveyard = sWorldSafeLocsStore.LookupEntry(g_id);

        if (!graveyard)
        {
            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDNOEXIST, g_id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();

        uint32 zoneId = player->GetZoneId();

        AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(zoneId);
        if (!areaEntry || areaEntry->zone != 0)
        {
            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDWRONGZONE, g_id, zoneId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (sObjectMgr.AddGraveYardLink(g_id, player->GetZoneId(), g_team))
            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDLINKED, g_id, zoneId);
        else
            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDALRLINKED, g_id, zoneId);

        return true;
    }

    static bool HandleNEARGRAVECommand(ChatHandler* handler, char const* args)
    {
        uint32 g_team;

        size_t argslen = strlen(args);

        if (!*args)
            g_team = 0;
        else if (strncmp((char*)args, "horde", argslen) == 0)
            g_team = HORDE;
        else if (strncmp((char*)args, "alliance", argslen) == 0)
            g_team = ALLIANCE;
        else
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        WorldSafeLocsEntry const* graveyard = sObjectMgr.GetClosestGraveYard(
            player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetMapId(), g_team);

        if (graveyard)
        {
            uint32 g_id = graveyard->ID;

            GraveYardData const* data = sObjectMgr.FindGraveYardData(g_id, player->GetZoneId());
            if (!data)
            {
                handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDERROR, g_id);
                handler->SetSentErrorMessage(true);
                return false;
            }

            g_team = data->team;

            std::string team_name = handler->GetOregonString(LANG_COMMAND_GRAVEYARD_NOTEAM);

            if (g_team == 0)
                team_name = handler->GetOregonString(LANG_COMMAND_GRAVEYARD_ANY);
            else if (g_team == HORDE)
                team_name = handler->GetOregonString(LANG_COMMAND_GRAVEYARD_HORDE);
            else if (g_team == ALLIANCE)
                team_name = handler->GetOregonString(LANG_COMMAND_GRAVEYARD_ALLIANCE);

            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDNEAREST, g_id, team_name.c_str(), player->GetZoneId());
        }
        else
        {
            std::string team_name;

            if (g_team == 0)
                team_name = handler->GetOregonString(LANG_COMMAND_GRAVEYARD_ANY);
            else if (g_team == HORDE)
                team_name = handler->GetOregonString(LANG_COMMAND_GRAVEYARD_HORDE);
            else if (g_team == ALLIANCE)
                team_name = handler->GetOregonString(LANG_COMMAND_GRAVEYARD_ALLIANCE);

            if (g_team == ~uint32(0))
                handler->PSendSysMessage(LANG_COMMAND_ZONENOGRAVEYARDS, player->GetZoneId());
            else
                handler->PSendSysMessage(LANG_COMMAND_ZONENOGRAFACTION, player->GetZoneId(), team_name.c_str());
        }

        return true;
    }

    static bool HandleSHOWAREACommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        int area = atoi((char*)args);

        Player* chr = handler->getSelectedPlayer();
        if (chr == NULL)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        int offset = area / 32;
        uint32 val = (uint32)(1 << (area % 32));

        if (offset >= 128)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 currFields = chr->GetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset);
        chr->SetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset, (uint32)(currFields | val));

        handler->SendSysMessage(LANG_EXPLORE_AREA);
        return true;
    }

    static bool HandleHIDEAREACommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        int area = atoi((char*)args);

        Player* chr = handler->getSelectedPlayer();
        if (chr == NULL)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        int offset = area / 32;
        uint32 val = (uint32)(1 << (area % 32));

        if (offset >= 128)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 currFields = chr->GetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset);
        chr->SetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset, (uint32)(currFields ^ val));

        handler->SendSysMessage(LANG_UNEXPLORE_AREA);
        return true;
    }

    static bool HandleAddItemCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 itemId = 0;

        if (args[0] == '[')                                        // [name] manual form
        {
            char* citemName = strtok((char*)args, "]");

            if (citemName && citemName[0])
            {
                std::string itemName = citemName + 1;
                WorldDatabase.escape_string(itemName);
                QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT entry FROM item_template WHERE name = '%s'", itemName.c_str());
                if (!result)
                {
                    handler->PSendSysMessage(LANG_COMMAND_COULDNOTFIND, citemName + 1);
                    handler->SetSentErrorMessage(true);
                    return false;
                }
                itemId = result->Fetch()->GetUInt16();
            }
            else
                return false;
        }
        else                                                    // item_id or [name] Shift-click form |color|Hitem:item_id:0:0:0|h[name]|h|r
        {
            char* cId = handler->extractKeyFromLink((char*)args, "Hitem");
            if (!cId)
                return false;
            itemId = atol(cId);
        }

        char* ccount = strtok(NULL, " ");

        int32 count = 1;

        if (ccount)
            count = strtol(ccount, NULL, 10);

        if (count == 0)
            count = 1;

        Player* pl = handler->GetSession()->GetPlayer();
        Player* plTarget = handler->getSelectedPlayer();
        if (!plTarget)
            plTarget = pl;

        sLog.outDetail(handler->GetOregonString(LANG_ADDITEM), itemId, count);

        ItemTemplate const* pProto = sObjectMgr.GetItemTemplate(itemId);
        if (!pProto)
        {
            handler->PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, itemId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        //Subtract
        if (count < 0)
        {
            plTarget->DestroyItemCount(itemId, -count, true, false);
            handler->PSendSysMessage(LANG_REMOVEITEM, itemId, -count, plTarget->GetName());
            return true;
        }

        //Adding items
        uint32 noSpaceForCount = 0;

        // check space and find places
        ItemPosCountVec dest;
        uint8 msg = plTarget->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, count, &noSpaceForCount);
        if (msg != EQUIP_ERR_OK)                               // convert to possible store amount
            count -= noSpaceForCount;

        if (count == 0 || dest.empty())                         // can't add any
        {
            handler->PSendSysMessage(LANG_ITEM_CANNOT_CREATE, itemId, noSpaceForCount);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Item* item = plTarget->StoreNewItem(dest, itemId, true, Item::GenerateItemRandomPropertyId(itemId));

        // remove binding (let GM give it to another player later)
        if (pl == plTarget)
            for (ItemPosCountVec::const_iterator itr = dest.begin(); itr != dest.end(); ++itr)
                if (Item* item1 = pl->GetItemByPos(itr->pos))
                    item1->SetBinding(false);

        if (count > 0 && item)
        {
            pl->SendNewItem(item, count, false, true);
            if (pl != plTarget)
                plTarget->SendNewItem(item, count, true, false);
        }

        if (noSpaceForCount > 0)
            handler->PSendSysMessage(LANG_ITEM_CANNOT_CREATE, itemId, noSpaceForCount);

        return true;
    }

    static bool HandleAddItemSetCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* cId = handler->extractKeyFromLink((char*)args, "Hitemset"); // number or [name] Shift-click form |color|Hitemset:itemset_id|h[name]|h|r
        if (!cId)
            return false;

        uint32 itemsetId = atol(cId);

        // prevent generation all items with itemset field value '0'
        if (itemsetId == 0)
        {
            handler->PSendSysMessage(LANG_NO_ITEMS_FROM_ITEMSET_FOUND, itemsetId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* pl = handler->GetSession()->GetPlayer();
        Player* plTarget = handler->getSelectedPlayer();
        if (!plTarget)
            plTarget = pl;

        sLog.outDetail(handler->GetOregonString(LANG_ADDITEMSET), itemsetId);

        bool found = false;
        for (uint32 id = 0; id < sItemStorage.MaxEntry; id++)
        {
            ItemTemplate const* pProto = sItemStorage.LookupEntry<ItemTemplate>(id);
            if (!pProto)
                continue;

            if (pProto->ItemSet == itemsetId)
            {
                found = true;
                ItemPosCountVec dest;
                uint8 msg = plTarget->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, pProto->ItemId, 1);
                if (msg == EQUIP_ERR_OK)
                {
                    Item* item = plTarget->StoreNewItem(dest, pProto->ItemId, true);

                    // remove binding (let GM give it to another player later)
                    if (pl == plTarget)
                        item->SetBinding(false);

                    pl->SendNewItem(item, 1, false, true);
                    if (pl != plTarget)
                        plTarget->SendNewItem(item, 1, true, false);
                }
                else
                {
                    pl->SendEquipError(msg, NULL, NULL);
                    handler->PSendSysMessage(LANG_ITEM_CANNOT_CREATE, pProto->ItemId, 1);
                }
            }
        }

        if (!found)
        {
            handler->PSendSysMessage(LANG_NO_ITEMS_FROM_ITEMSET_FOUND, itemsetId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    static bool HandleBankCommand(ChatHandler* handler, const char* /*args*/)
    {
        handler->GetSession()->SendShowBank(handler->GetSession()->GetPlayer()->GetGUID());
        return true;
    }

    static bool HandleChangeWeatherCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        //Weather is OFF
        if (!sWorld.getConfig(CONFIG_WEATHER))
        {
            handler->SendSysMessage(LANG_WEATHER_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        //*Change the weather of a cell
        char* px = strtok((char*)args, " ");
        char* py = strtok(NULL, " ");

        if (!px || !py)
            return false;

        uint32 type = (uint32)atoi(px);                         //0 to 3, 0: fine, 1: rain, 2: snow, 3: sand
        float grade = (float)atof(py);                          //0 to 1, sending -1 is instand good weather

        Player* player = handler->GetSession()->GetPlayer();
        uint32 zoneid = player->GetZoneId();

        Weather* wth = sWorld.FindWeather(zoneid);

        if (!wth)
            wth = sWorld.AddWeather(zoneid);

        if (!wth)
        {
            handler->SendSysMessage(LANG_NO_WEATHER);
            handler->SetSentErrorMessage(true);
            return false;
        }

        wth->SetWeather(WeatherType(type), grade);

        return true;
    }

    static bool HandleMaxSkillCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* _player = handler->getSelectedPlayer();
        if (!_player)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // each skills that have max skill value dependent from level seted to current level max skill value
        _player->UpdateSkillsToMaxSkillsForLevel();
        return true;
    }

    static bool HandleSetSkillCommand(ChatHandler* handler, const char* args)
    {
        // number or [name] Shift-click form |color|Hskill:skill_id|h[name]|h|r
        char* skill_p = handler->extractKeyFromLink((char*)args, "Hskill");
        if (!skill_p)
            return false;

        char* level_p = strtok(NULL, " ");

        if (!level_p)
            return false;

        char* max_p = strtok(NULL, " ");

        int32 skill = atoi(skill_p);
        if (skill <= 0)
        {
            handler->PSendSysMessage(LANG_INVALID_SKILL_ID, skill);
            handler->SetSentErrorMessage(true);
            return false;
        }

        int32 level = atol(level_p);

        Player* target = handler->getSelectedPlayerOrSelf();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        SkillLineEntry const* sl = sSkillLineStore.LookupEntry(skill);
        if (!sl)
        {
            handler->PSendSysMessage(LANG_INVALID_SKILL_ID, skill);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->GetSkillValue(skill))
        {
            handler->PSendSysMessage(LANG_SET_SKILL_ERROR, target->GetName(), skill, sl->name[handler->GetSession()->GetSessionDbcLocale()]);
            handler->SetSentErrorMessage(true);
            return false;
        }

        int32 max = max_p ? atol(max_p) : target->GetPureMaxSkillValue(skill);

        if (level <= 0 || level > max || max <= 0)
            return false;

        target->SetSkill(skill, level, max);
        handler->PSendSysMessage(LANG_SET_SKILL, skill, sl->name[handler->GetSession()->GetSessionDbcLocale()], target->GetName(), level, max);

        return true;
    }

    static bool HandlePInfoCommand(ChatHandler* handler, const char* args)
    {
        Player* target = NULL;
        uint64 targetGUID = 0;

        char* px = strtok((char*)args, " ");
        char* py = NULL;

        std::string name;

        if (px)
        {
            name = px;

            if (name.empty())
                return false;

            if (!normalizePlayerName(name))
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            target = sObjectMgr.GetPlayer(name.c_str());
            if (target)
                py = strtok(NULL, " ");
            else
            {
                targetGUID = sObjectMgr.GetPlayerGUIDByName(name);
                if (targetGUID)
                    py = strtok(NULL, " ");
                else
                    py = px;
            }
        }

        if (!target && !targetGUID)
            target = handler->getSelectedPlayer();

        if (!target && !targetGUID)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 accId = 0;
        uint32 money = 0;
        uint32 total_player_time = 0;
        uint32 level = 0;
        uint32 latency = 0;
        uint8 race;
        uint8 Class;

        // get additional information from Player object
        if (target)
        {
            targetGUID = target->GetGUID();
            name = target->GetName();                           // re-read for case getSelectedPlayer() target
            accId = target->GetSession()->GetAccountId();
            money = target->GetMoney();
            total_player_time = target->GetTotalPlayedTime();
            level = target->getLevel();
            latency = target->GetSession()->GetLatency();
            race = target->getRace();
            Class = target->getClass();
        }
        // get additional information from DB
        else
        {
            QueryResult_AutoPtr result = CharacterDatabase.PQuery("SELECT totaltime, level, money, account, race, class FROM characters WHERE guid = '%u'", GUID_LOPART(targetGUID));
            if (!result)
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }
            Field* fields = result->Fetch();
            total_player_time = fields[0].GetUInt32();
            level = fields[1].GetUInt32();
            money = fields[2].GetUInt32();
            accId = fields[3].GetUInt32();
            race = fields[4].GetUInt8();
            Class = fields[5].GetUInt8();
        }

        std::string username = handler->GetOregonString(LANG_ERROR);
        std::string email = handler->GetOregonString(LANG_ERROR);
        std::string last_ip = handler->GetOregonString(LANG_ERROR);
        uint32 security = 0;
        std::string last_login = handler->GetOregonString(LANG_ERROR);

        QueryResult_AutoPtr result = LoginDatabase.PQuery("SELECT a.username,aa.gmlevel,a.email,a.last_ip,a.last_login "
            "FROM account a "
            "LEFT JOIN account_access aa "
            "ON (a.id = aa.id) "
            "WHERE a.id = '%u'", accId);
        if (result)
        {
            Field* fields = result->Fetch();
            username = fields[0].GetCppString();
            security = fields[1].GetUInt32();
            email = fields[2].GetCppString();

            if (email.empty())
                email = "-";

            if (!handler->GetSession() || handler->GetSession()->GetSecurity() >= security)
            {
                last_ip = fields[3].GetCppString();
                last_login = fields[4].GetCppString();

                uint32 ip = inet_addr(last_ip.c_str());
#if OREGON_ENDIAN == BIGENDIAN
                EndianConvertReverse(ip);
#endif

                if (QueryResult_AutoPtr result2 = WorldDatabase.PQuery("SELECT c.country FROM ip2nationCountries c, ip2nation i WHERE "
                    "i.ip < %u AND c.code = i.country ORDER BY i.ip DESC LIMIT 0,1", ip))
                {
                    Field* fields2 = result2->Fetch();
                    last_ip.append(" (");
                    last_ip.append(fields2[0].GetString());
                    last_ip.append(")");
                }
            }
            else
            {
                last_ip = "-";
                last_login = "-";
            }
        }

        handler->PSendSysMessage(LANG_PINFO_ACCOUNT, (target ? "" : handler->GetOregonString(LANG_OFFLINE)), name.c_str(), GUID_LOPART(targetGUID), username.c_str(), accId, email.c_str(), security, last_ip.c_str(), last_login.c_str(), latency);

        std::string race_s, Class_s;
        switch (race)
        {
        case RACE_HUMAN:
            race_s = "Human";
            break;
        case RACE_ORC:
            race_s = "Orc";
            break;
        case RACE_DWARF:
            race_s = "Dwarf";
            break;
        case RACE_NIGHTELF:
            race_s = "Night Elf";
            break;
        case RACE_UNDEAD_PLAYER:
            race_s = "Undead";
            break;
        case RACE_TAUREN:
            race_s = "Tauren";
            break;
        case RACE_GNOME:
            race_s = "Gnome";
            break;
        case RACE_TROLL:
            race_s = "Troll";
            break;
        case RACE_BLOODELF:
            race_s = "Blood Elf";
            break;
        case RACE_DRAENEI:
            race_s = "Draenei";
            break;
        }
        switch (Class)
        {
        case CLASS_WARRIOR:
            Class_s = "Warrior";
            break;
        case CLASS_PALADIN:
            Class_s = "Paladin";
            break;
        case CLASS_HUNTER:
            Class_s = "Hunter";
            break;
        case CLASS_ROGUE:
            Class_s = "Rogue";
            break;
        case CLASS_PRIEST:
            Class_s = "Priest";
            break;
        case CLASS_SHAMAN:
            Class_s = "Shaman";
            break;
        case CLASS_MAGE:
            Class_s = "Mage";
            break;
        case CLASS_WARLOCK:
            Class_s = "Warlock";
            break;
        case CLASS_DRUID:
            Class_s = "Druid";
            break;
        }

        std::string timeStr = secsToTimeString(total_player_time, true, true);
        uint32 gold = money / GOLD;
        uint32 silv = (money % GOLD) / SILVER;
        uint32 copp = (money % GOLD) % SILVER;
        handler->PSendSysMessage(LANG_PINFO_LEVEL, race_s.c_str(), Class_s.c_str(), timeStr.c_str(), level, gold, silv, copp);

        if (py && strncmp(py, "rep", 3) == 0)
        {
            if (!target)
            {
                // rep option not implemented for offline case
                handler->SendSysMessage(LANG_PINFO_NO_REP);
                handler->SetSentErrorMessage(true);
                return false;
            }

            const char* FactionName;
            FactionStateList const& targetFSL = target->GetReputationMgr().GetStateList();
            for (FactionStateList::const_iterator itr = targetFSL.begin(); itr != targetFSL.end(); ++itr)
            {
                FactionEntry const* factionEntry = sFactionStore.LookupEntry(itr->second.ID);
                if (factionEntry)
                    FactionName = factionEntry->name[handler->GetSession()->GetSessionDbcLocale()];
                else
                    FactionName = "#Not found#";
                ReputationRank rank = target->GetReputationMgr().GetRank(factionEntry);
                std::string rankName = handler->GetOregonString(ReputationRankStrIndex[rank]);
                std::ostringstream ss;
                ss << itr->second.ID << ": |cffffffff|Hfaction:" << itr->second.ID << "|h[" << FactionName << "]|h|r " << rankName << "|h|r (" << target->GetReputationMgr().GetReputation(factionEntry) << ")";

                if (itr->second.Flags & FACTION_FLAG_VISIBLE)
                    ss << handler->GetOregonString(LANG_FACTION_VISIBLE);
                if (itr->second.Flags & FACTION_FLAG_AT_WAR)
                    ss << handler->GetOregonString(LANG_FACTION_ATWAR);
                if (itr->second.Flags & FACTION_FLAG_PEACE_FORCED)
                    ss << handler->GetOregonString(LANG_FACTION_PEACE_FORCED);
                if (itr->second.Flags & FACTION_FLAG_HIDDEN)
                    ss << handler->GetOregonString(LANG_FACTION_HIDDEN);
                if (itr->second.Flags & FACTION_FLAG_INVISIBLE_FORCED)
                    ss << handler->GetOregonString(LANG_FACTION_INVISIBLE_FORCED);
                if (itr->second.Flags & FACTION_FLAG_INACTIVE)
                    ss << handler->GetOregonString(LANG_FACTION_INACTIVE);

                handler->SendSysMessage(ss.str().c_str());
            }
        }
        return true;
    }

    static bool HandleRespawnCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* pl = handler->GetSession()->GetPlayer();

        // accept only explicitly selected target (not implicitly self targeting case)
        Unit* target = handler->getSelectedUnit();
        if (pl->GetSelection() && target)
        {
            if (target->GetTypeId() != TYPEID_UNIT)
            {
                handler->SendSysMessage(LANG_SELECT_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }
            else if (!target->isDead())
            {
                handler->SendSysMessage(LANG_CREATURE_NOT_DEAD);
                handler->SetSentErrorMessage(true);
                return false;
            }
            target->ToCreature()->Respawn();
            return true;
        }

        CellCoord p(Oregon::ComputeCellCoord(pl->GetPositionX(), pl->GetPositionY()));
        Cell cell(p);
        cell.SetNoCreate();

        Oregon::RespawnDo u_do;
        Oregon::WorldObjectWorker<Oregon::RespawnDo> worker(pl, u_do);

        TypeContainerVisitor<Oregon::WorldObjectWorker<Oregon::RespawnDo>, GridTypeMapContainer > obj_worker(worker);
        cell.Visit(p, obj_worker, *pl->GetMap(), *pl, pl->GetGridActivationRange());

        return true;
    }

    static bool HandleSendItemCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        // format: name "subject text" "mail text" item1[:count1] item2[:count2] ... item12[:count12]

        char* pName = strtok((char*)args, " ");
        if (!pName)
            return false;

        char* tail1 = strtok(NULL, "");
        if (!tail1)
            return false;

        char* msgSubject;
        if (*tail1 == '"')
            msgSubject = strtok(tail1 + 1, "\"");
        else
        {
            char* space = strtok(tail1, "\"");
            if (!space)
                return false;
            msgSubject = strtok(NULL, "\"");
        }

        if (!msgSubject)
            return false;

        char* tail2 = strtok(NULL, "");
        if (!tail2)
            return false;

        char* msgText;
        if (*tail2 == '"')
            msgText = strtok(tail2 + 1, "\"");
        else
        {
            char* space = strtok(tail2, "\"");
            if (!space)
                return false;
            msgText = strtok(NULL, "\"");
        }

        if (!msgText)
            return false;

        // pName, msgSubject, msgText isn't NUL after prev. check
        std::string name = pName;
        std::string subject = msgSubject;
        std::string text = msgText;

        // extract items
        typedef std::pair<uint32, uint32> ItemPair;
        typedef std::list< ItemPair > ItemPairs;
        ItemPairs items;

        // get all tail string
        char* tail = strtok(NULL, "");

        // get from tail next item str
        while (char* itemStr = strtok(tail, " "))
        {
            // and get new tail
            tail = strtok(NULL, "");

            // parse item str
            char* itemIdStr = strtok(itemStr, ":");
            char* itemCountStr = strtok(NULL, " ");

            uint32 item_id = atoi(itemIdStr);
            if (!item_id)
                return false;

            ItemTemplate const* item_proto = sObjectMgr.GetItemTemplate(item_id);
            if (!item_proto)
            {
                handler->PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, item_id);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 item_count = itemCountStr ? atoi(itemCountStr) : 1;
            if (item_count < 1 || (item_proto->MaxCount && item_count > item_proto->MaxCount))
            {
                handler->PSendSysMessage(LANG_COMMAND_INVALID_ITEM_COUNT, item_count, item_id);
                handler->SetSentErrorMessage(true);
                return false;
            }

            while (item_count > item_proto->Stackable)
            {
                items.push_back(ItemPair(item_id, item_proto->Stackable));
                item_count -= item_proto->Stackable;
            }

            items.push_back(ItemPair(item_id, item_count));

            if (items.size() > MAX_MAIL_ITEMS)
            {
                handler->PSendSysMessage(LANG_COMMAND_MAIL_ITEMS_LIMIT, MAX_MAIL_ITEMS);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        if (!normalizePlayerName(name))
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint64 receiver_guid = sObjectMgr.GetPlayerGUIDByName(name);
        if (!receiver_guid)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // from console show not existed sender
        MailSender sender(MAIL_NORMAL, handler->GetSession() ? handler->GetSession()->GetPlayer()->GetGUIDLow() : 0, MAIL_STATIONERY_GM);

        uint32 itemTextId = !text.empty() ? sObjectMgr.CreateItemText(text) : 0;

        Player* receiver = sObjectMgr.GetPlayer(receiver_guid);

        // fill mail
        MailDraft draft(subject, itemTextId);

        for (ItemPairs::const_iterator itr = items.begin(); itr != items.end(); ++itr)
        {
            if (Item* item = Item::CreateItem(itr->first, itr->second, handler->GetSession() ? handler->GetSession()->GetPlayer() : 0))
            {
                item->SaveToDB();                               // save for prevent lost at next mail load, if send fail then item will deleted
                draft.AddItem(item);
            }
        }

        draft.SendMailTo(MailReceiver(receiver, GUID_LOPART(receiver_guid)), sender);

        handler->PSendSysMessage(LANG_MAIL_SENT, name.c_str());
        return true;
    }

    static bool HandleSendMailCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        // format: name "subject text" "mail text"

        char* pName = strtok((char*)args, " ");
        if (!pName)
            return false;

        char* tail1 = strtok(NULL, "");
        if (!tail1)
            return false;

        char* msgSubject;
        if (*tail1 == '"')
            msgSubject = strtok(tail1 + 1, "\"");
        else
        {
            char* space = strtok(tail1, "\"");
            if (!space)
                return false;
            msgSubject = strtok(NULL, "\"");
        }

        if (!msgSubject)
            return false;

        char* tail2 = strtok(NULL, "");
        if (!tail2)
            return false;

        char* msgText;
        if (*tail2 == '"')
            msgText = strtok(tail2 + 1, "\"");
        else
        {
            char* space = strtok(tail2, "\"");
            if (!space)
                return false;
            msgText = strtok(NULL, "\"");
        }

        if (!msgText)
            return false;

        // pName, msgSubject, msgText isn't NUL after prev. check
        std::string name = pName;
        std::string subject = msgSubject;
        std::string text = msgText;

        if (!normalizePlayerName(name))
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint64 receiver_guid = sObjectMgr.GetPlayerGUIDByName(name);
        if (!receiver_guid)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        sObjectMgr.GenerateMailID();

        // from console show not existed sender
        MailSender sender(MAIL_NORMAL, handler->GetSession() ? handler->GetSession()->GetPlayer()->GetGUIDLow() : 0, MAIL_STATIONERY_GM);

        uint32 itemTextId = !text.empty() ? sObjectMgr.CreateItemText(text) : 0;

        Player* receiver = sObjectMgr.GetPlayer(receiver_guid);

        MailDraft(subject, itemTextId)
            .SendMailTo(MailReceiver(receiver, GUID_LOPART(receiver_guid)), sender);

        handler->PSendSysMessage(LANG_MAIL_SENT, name.c_str());
        return true;
    }

    static bool HandleSendMessageCommand(ChatHandler* handler, char const* args)
    {
        // Get the command line arguments
        char* name_str = strtok((char*)args, " ");
        char* msg_str = strtok(NULL, "");

        if (!name_str || !msg_str)
            return false;

        std::string name = name_str;

        if (!normalizePlayerName(name))
            return false;

        // Find the player and check that he is not logging out.
        Player* rPlayer = sObjectMgr.GetPlayer(name.c_str());
        if (!rPlayer)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (rPlayer->GetSession()->isLogingOut())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Send the message
        // Use SendAreaTriggerMessage for fastest delivery.
        rPlayer->GetSession()->SendAreaTriggerMessage("%s", msg_str);
        rPlayer->GetSession()->SendAreaTriggerMessage("|cffff0000[Message from administrator]:|r");

        //Confirmation message
        handler->PSendSysMessage(LANG_SENDMESSAGE, name.c_str(), msg_str);
        return true;
    }
    static bool HandleSendMoneyCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        // format: name "subject text" "mail text" money

        char* pName = strtok((char*)args, " ");
        if (!pName)
            return false;

        char* tail1 = strtok(NULL, "");
        if (!tail1)
            return false;

        char* msgSubject;
        if (*tail1 == '"')
            msgSubject = strtok(tail1 + 1, "\"");
        else
        {
            char* space = strtok(tail1, "\"");
            if (!space)
                return false;
            msgSubject = strtok(NULL, "\"");
        }

        if (!msgSubject)
            return false;

        char* tail2 = strtok(NULL, "");
        if (!tail2)
            return false;

        char* msgText;
        if (*tail2 == '"')
            msgText = strtok(tail2 + 1, "\"");
        else
        {
            char* space = strtok(tail2, "\"");
            if (!space)
                return false;
            msgText = strtok(NULL, "\"");
        }

        if (!msgText)
            return false;

        char* money_str = strtok(NULL, "");
        int32 money = money_str ? atoi(money_str) : 0;
        if (money <= 0)
            return false;

        // pName, msgSubject, msgText isn't NUL after prev. check
        std::string name = pName;
        std::string subject = msgSubject;
        std::string text = msgText;

        if (!normalizePlayerName(name))
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint64 receiver_guid = sObjectMgr.GetPlayerGUIDByName(name);
        if (!receiver_guid)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        sObjectMgr.GenerateMailID();

        // from console show not existed sender
        MailSender sender(MAIL_NORMAL, handler->GetSession() ? handler->GetSession()->GetPlayer()->GetGUIDLow() : 0, MAIL_STATIONERY_GM);

        uint32 itemTextId = !text.empty() ? sObjectMgr.CreateItemText(text) : 0;

        Player* receiver = sObjectMgr.GetPlayer(receiver_guid);

        MailDraft(subject, itemTextId)
            .AddMoney(money)
            .SendMailTo(MailReceiver(receiver, GUID_LOPART(receiver_guid)), sender);

        handler->PSendSysMessage(LANG_MAIL_SENT, name.c_str());
        return true;
    }

    //mute player for some times
    static bool HandleMuteCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* charname = strtok((char*)args, " ");
        if (!charname)
            return false;

        std::string cname = charname;

        char* timetonotspeak = strtok(NULL, " ");
        if (!timetonotspeak)
            return false;

        char* mutereason = strtok(NULL, "");
        std::string mutereasonstr;
        if (!mutereason)
            mutereasonstr = "No reason.";
        else
            mutereasonstr = mutereason;

        uint32 notspeaktime = (uint32)atoi(timetonotspeak);

        if (!normalizePlayerName(cname))
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint64 guid = sObjectMgr.GetPlayerGUIDByName(cname.c_str());
        if (!guid)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* chr = sObjectMgr.GetPlayer(guid);

        // check security
        uint32 account_id = 0;
        uint32 security = 0;

        if (chr)
        {
            account_id = chr->GetSession()->GetAccountId();
            security = chr->GetSession()->GetSecurity();
        }
        else
        {
            account_id = sObjectMgr.GetPlayerAccountIdByGUID(guid);
            security = sAccountMgr->GetSecurity(account_id);
        }

        if (handler->GetSession() && security >= handler->GetSession()->GetSecurity())
        {
            handler->SendSysMessage(LANG_YOURS_SECURITY_IS_LOW);
            handler->SetSentErrorMessage(true);
            return false;
        }

        time_t mutetime = time(NULL) + notspeaktime * 60;

        if (chr)
            chr->GetSession()->m_muteTime = mutetime;

        LoginDatabase.PExecute("UPDATE account SET mutetime = " UI64FMTD " WHERE id = '%u'", uint64(mutetime), account_id);

        if (chr)
            ChatHandler(chr).PSendSysMessage(LANG_YOUR_CHAT_DISABLED, notspeaktime, mutereasonstr.c_str());

        handler->PSendSysMessage(LANG_YOU_DISABLE_CHAT, cname.c_str(), notspeaktime, mutereasonstr.c_str());

        return true;
    }

    //unmute player
    static bool HandleUnmuteCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* charname = strtok((char*)args, " ");
        if (!charname)
            return false;

        std::string cname = charname;

        if (!normalizePlayerName(cname))
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint64 guid = sObjectMgr.GetPlayerGUIDByName(cname.c_str());
        if (!guid)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* chr = sObjectMgr.GetPlayer(guid);

        // check security
        uint32 account_id = 0;
        uint32 security = 0;

        if (chr)
        {
            account_id = chr->GetSession()->GetAccountId();
            security = chr->GetSession()->GetSecurity();
        }
        else
        {
            account_id = sObjectMgr.GetPlayerAccountIdByGUID(guid);
            security = sAccountMgr->GetSecurity(account_id);
        }

        if (handler->GetSession() && security >= handler->GetSession()->GetSecurity())
        {
            handler->SendSysMessage(LANG_YOURS_SECURITY_IS_LOW);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (chr)
        {
            if (chr->CanSpeak())
            {
                handler->SendSysMessage(LANG_CHAT_ALREADY_ENABLED);
                handler->SetSentErrorMessage(true);
                return false;
            }

            chr->GetSession()->m_muteTime = 0;
        }

        LoginDatabase.PExecute("UPDATE account SET mutetime = '0' WHERE id = '%u'", account_id);

        if (chr)
            ChatHandler(chr).PSendSysMessage(LANG_YOUR_CHAT_ENABLED);

        handler->PSendSysMessage(LANG_YOU_ENABLE_CHAT, cname.c_str());
        return true;
    }

    /*
    ComeToMe command REQUIRED for 3rd party scripting library to have access to PointMovementGenerator
    Without this function 3rd party scripting library will get linking errors (unresolved external)
    when attempting to use the PointMovementGenerator
    */
    static bool HandleComeToMeCommand(ChatHandler* handler, const char* args)
    {
        char* newFlagStr = strtok((char*)args, " ");

        if (!newFlagStr)
            return false;

        uint32 newFlags = (uint32)strtoul(newFlagStr, NULL, 0);

        Creature* caster = handler->getSelectedCreature();
        if (!caster)
        {
            handler->GetSession()->GetPlayer()->SetUnitMovementFlags(newFlags);
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        caster->SetUnitMovementFlags(newFlags);

        Player* pl = handler->GetSession()->GetPlayer();

        caster->GetMotionMaster()->MovePoint(0, pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ());
        return true;
    }

    static bool HandleMovegensCommand(ChatHandler* handler, const char* /*args*/)
    {
        Unit* unit = handler->getSelectedUnit();
        if (!unit)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_MOVEGENS_LIST, (unit->GetTypeId() == TYPEID_PLAYER ? "Player" : "Creature"), unit->GetGUIDLow());

        MotionMaster* mm = unit->GetMotionMaster();
        for (uint8 i = 0; i < MAX_MOTION_SLOT; ++i)
        {
            MovementGenerator* mg = mm->GetMotionSlot(i);
            if (!mg)
            {
                handler->SendSysMessage("Empty");
                continue;
            }
            switch (mg->GetMovementGeneratorType())
            {
            case IDLE_MOTION_TYPE:
                handler->SendSysMessage(LANG_MOVEGENS_IDLE);
                break;
            case RANDOM_MOTION_TYPE:
                handler->SendSysMessage(LANG_MOVEGENS_RANDOM);
                break;
            case WAYPOINT_MOTION_TYPE:
                handler->SendSysMessage(LANG_MOVEGENS_WAYPOINT);
                break;
            case ANIMAL_RANDOM_MOTION_TYPE:
                handler->SendSysMessage(LANG_MOVEGENS_ANIMAL_RANDOM);
                break;
            case CONFUSED_MOTION_TYPE:
                handler->SendSysMessage(LANG_MOVEGENS_CONFUSED);
                break;
            case CHASE_MOTION_TYPE:
            {
                if (unit->GetTypeId() == TYPEID_PLAYER)
                {
                    ChaseMovementGenerator<Player> const* mgen = static_cast<ChaseMovementGenerator<Player> const*>(mg);
                    Unit* target = mgen->GetTarget();
                    if (target)
                        handler->PSendSysMessage(LANG_MOVEGENS_TARGETED_PLAYER, target->GetName(), target->GetGUIDLow());
                    else
                        handler->SendSysMessage(LANG_MOVEGENS_TARGETED_NULL);
                }
                else
                {
                    ChaseMovementGenerator<Creature> const* mgen = static_cast<ChaseMovementGenerator<Creature> const*>(mg);
                    Unit* target = mgen->GetTarget();
                    if (target)
                        handler->PSendSysMessage(LANG_MOVEGENS_TARGETED_CREATURE, target->GetName(), target->GetGUIDLow());
                    else
                        handler->SendSysMessage(LANG_MOVEGENS_TARGETED_NULL);
                }
                break;
            }
            case FOLLOW_MOTION_TYPE:
            {
                if (unit->GetTypeId() == TYPEID_PLAYER)
                {
                    FollowMovementGenerator<Player> const* mgen = static_cast<FollowMovementGenerator<Player> const*>(mg);
                    Unit* target = mgen->GetTarget();
                    if (target)
                        handler->PSendSysMessage(LANG_MOVEGENS_TARGETED_PLAYER, target->GetName(), target->GetGUIDLow());
                    else
                        handler->SendSysMessage(LANG_MOVEGENS_TARGETED_NULL);
                }
                else
                {
                    FollowMovementGenerator<Creature> const* mgen = static_cast<FollowMovementGenerator<Creature> const*>(mg);
                    Unit* target = mgen->GetTarget();
                    if (target)
                        handler->PSendSysMessage(LANG_MOVEGENS_TARGETED_CREATURE, target->GetName(), target->GetGUIDLow());
                    else
                        handler->SendSysMessage(LANG_MOVEGENS_TARGETED_NULL);
                }
                break;
            }
            case HOME_MOTION_TYPE:
                if (unit->GetTypeId() == TYPEID_UNIT)
                {
                    float x, y, z;
                    mm->GetDestination(x, y, z);
                    handler->PSendSysMessage(LANG_MOVEGENS_HOME_CREATURE, x, y, z);
                }
                else
                    handler->SendSysMessage(LANG_MOVEGENS_HOME_PLAYER);
                break;
            case FLIGHT_MOTION_TYPE:
                handler->SendSysMessage(LANG_MOVEGENS_FLIGHT);
                break;
            case POINT_MOTION_TYPE:
            {
                float x, y, z;
                mm->GetDestination(x, y, z);
                handler->PSendSysMessage(LANG_MOVEGENS_POINT, x, y, z);
                break;
            }
            case FLEEING_MOTION_TYPE:
                handler->SendSysMessage(LANG_MOVEGENS_FEAR);
                break;
            case DISTRACT_MOTION_TYPE:
                handler->SendSysMessage(LANG_MOVEGENS_DISTRACT);
                break;
            default:
                handler->PSendSysMessage(LANG_MOVEGENS_UNKNOWN, mg->GetMovementGeneratorType());
                break;
            }
        }
        return true;
    }

    static bool HandleDamageCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Unit* target = handler->getSelectedUnit();

        if (!target || !handler->GetSession()->GetPlayer()->GetSelection())
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->IsAlive())
            return true;

        char* damageStr = strtok((char*)args, " ");
        if (!damageStr)
            return false;

        int32 damage = atoi((char*)damageStr);
        if (damage <= 0)
            return true;

        char* schoolStr = strtok((char*)NULL, " ");

        // flat melee damage without resistence/etc reduction
        if (!schoolStr)
        {
            handler->GetSession()->GetPlayer()->DealDamage(target, damage, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            handler->GetSession()->GetPlayer()->SendAttackStateUpdate(HITINFO_NORMALSWING2, target, 1, SPELL_SCHOOL_MASK_NORMAL, damage, 0, 0, VICTIMSTATE_NORMAL, 0);
            return true;
        }

        uint32 school = schoolStr ? atoi((char*)schoolStr) : SPELL_SCHOOL_NORMAL;
        if (school >= MAX_SPELL_SCHOOL)
            return false;

        SpellSchoolMask schoolmask = SpellSchoolMask(1 << school);

        if (schoolmask & SPELL_SCHOOL_MASK_NORMAL)
            damage = handler->GetSession()->GetPlayer()->CalcArmorReducedDamage(target, damage);

        char* spellStr = strtok((char*)NULL, " ");

        // melee damage by specific school
        if (!spellStr)
        {
            uint32 absorb = 0;
            uint32 resist = 0;

            handler->GetSession()->GetPlayer()->CalcAbsorbResist(target, schoolmask, SPELL_DIRECT_DAMAGE, damage, &absorb, &resist);

            if (damage <= int32(absorb + resist))
                return true;

            damage -= absorb + resist;

            handler->GetSession()->GetPlayer()->DealDamage(target, damage, NULL, DIRECT_DAMAGE, schoolmask, NULL, false);
            handler->GetSession()->GetPlayer()->SendAttackStateUpdate(HITINFO_NORMALSWING2, target, 1, schoolmask, damage, absorb, resist, VICTIMSTATE_NORMAL, 0);
            return true;
        }

        // non-melee damage

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spellid = handler->extractSpellIdFromLink((char*)args);
        if (!spellid || !sSpellStore.LookupEntry(spellid))
            return false;

        handler->GetSession()->GetPlayer()->SpellNonMeleeDamageLog(target, spellid, damage, false);
        return true;
    }

    static bool HandleCombatStopCommand(ChatHandler* handler, const char* args)
    {
        Player* player;

        if (*args)
        {
            std::string playername = args;

            if (!normalizePlayerName(playername))
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            player = sObjectMgr.GetPlayer(playername.c_str());

            if (!player)
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        else
        {
            player = handler->getSelectedPlayer();

            if (!player)
                player = handler->GetSession()->GetPlayer();
        }

        player->CombatStop();
        player->getHostileRefManager().deleteReferences();
        return true;
    }

    static bool HandleAHBotOptionsCommand(ChatHandler* handler, const char* args)
    {
        uint32 ahMapID = 0;
        char* opt = strtok((char*)args, " ");
        char* ahMapIdStr = strtok(NULL, " ");
        if (ahMapIdStr)
            ahMapID = (uint32)strtoul(ahMapIdStr, NULL, 0);
        if (!opt)
        {
            handler->PSendSysMessage("Syntax is: ahbotoptions $option $ahMapID (2, 6 or 7) $parameter");
            handler->PSendSysMessage("Try ahbotoptions help to see a list of options.");
            return false;
        }
        int l = strlen(opt);

        if (strncmp(opt, "help", l) == 0)
        {
            handler->PSendSysMessage("AHBot commands:");
            handler->PSendSysMessage("ahexpire");
            handler->PSendSysMessage("minitems");
            handler->PSendSysMessage("maxitems");
            //PSendSysMessage("");
            //PSendSysMessage("");
            handler->PSendSysMessage("percentages");
            handler->PSendSysMessage("minprice");
            handler->PSendSysMessage("maxprice");
            handler->PSendSysMessage("minbidprice");
            handler->PSendSysMessage("maxbidprice");
            handler->PSendSysMessage("maxstack");
            handler->PSendSysMessage("buyerprice");
            handler->PSendSysMessage("bidinterval");
            handler->PSendSysMessage("bidsperinterval");
            return true;
        }
        else if (strncmp(opt, "ahexpire", l) == 0)
        {
            if (!ahMapIdStr)
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions ahexpire $ahMapID (2, 6 or 7)");
                return false;
            }
            auctionbot.Commands(0, ahMapID, 0, NULL);
        }
        else if (strncmp(opt, "minitems", l) == 0)
        {
            char* param1 = strtok(NULL, " ");
            if ((!ahMapIdStr) || (!param1))
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions minitems $ahMapID (2, 6 or 7) $minItems");
                return false;
            }
            auctionbot.Commands(1, ahMapID, 0, param1);
        }
        else if (strncmp(opt, "maxitems", l) == 0)
        {
            char* param1 = strtok(NULL, " ");
            if ((!ahMapIdStr) || (!param1))
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions maxitems $ahMapID (2, 6 or 7) $maxItems");
                return false;
            }
            auctionbot.Commands(2, ahMapID, 0, param1);
        }
        else if (strncmp(opt, "mintime", l) == 0)
        {
            handler->PSendSysMessage("ahbotoptions mintime has been deprecated");
            return false;
            /*
            char * param1 = strtok(NULL, " ");
            if ((!ahMapIdStr) || (!param1))
            {
                PSendSysMessage("Syntax is: ahbotoptions mintime $ahMapID (2, 6 or 7) $mintime");
                return false;
            }
            auctionbot.Commands(3, ahMapID, 0, param1);
            */
        }
        else if (strncmp(opt, "maxtime", l) == 0)
        {
            handler->PSendSysMessage("ahbotoptions maxtime has been deprecated");
            return false;
            /*
            char * param1 = strtok(NULL, " ");
            if ((!ahMapIdStr) || (!param1))
            {
                PSendSysMessage("Syntax is: ahbotoptions maxtime $ahMapID (2, 6 or 7) $maxtime");
                return false;
            }
            auctionbot.Commands(4, ahMapID, 0, param1);
            */
        }
        else if (strncmp(opt, "percentages", l) == 0)
        {
            char* param1 = strtok(NULL, " ");
            char* param2 = strtok(NULL, " ");
            char* param3 = strtok(NULL, " ");
            char* param4 = strtok(NULL, " ");
            char* param5 = strtok(NULL, " ");
            char* param6 = strtok(NULL, " ");
            char* param7 = strtok(NULL, " ");
            char* param8 = strtok(NULL, " ");
            char* param9 = strtok(NULL, " ");
            char* param10 = strtok(NULL, " ");
            char* param11 = strtok(NULL, " ");
            char* param12 = strtok(NULL, " ");
            char* param13 = strtok(NULL, " ");
            char* param14 = strtok(NULL, " ");
            if ((!ahMapIdStr) || (!param14))
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions percentages $ahMapID (2, 6 or 7) $1 $2 $3 $4 $5 $6 $7 $8 $9 $10 $11 $12 $13 $14");
                handler->PSendSysMessage("1 GreyTradeGoods 2 WhiteTradeGoods 3 GreenTradeGoods 4 BlueTradeGoods 5 PurpleTradeGoods");
                handler->PSendSysMessage("6 OrangeTradeGoods 7 YellowTradeGoods 8 GreyItems 9 WhiteItems 10 GreenItems 11 BlueItems");
                handler->PSendSysMessage("12 PurpleItems 13 OrangeItems 14 YellowItems");
                handler->PSendSysMessage("The total must add up to 100%%");
                return false;
            }
            uint32 greytg = (uint32)strtoul(param1, NULL, 0);
            uint32 whitetg = (uint32)strtoul(param2, NULL, 0);
            uint32 greentg = (uint32)strtoul(param3, NULL, 0);
            uint32 bluetg = (uint32)strtoul(param3, NULL, 0);
            uint32 purpletg = (uint32)strtoul(param5, NULL, 0);
            uint32 orangetg = (uint32)strtoul(param6, NULL, 0);
            uint32 yellowtg = (uint32)strtoul(param7, NULL, 0);
            uint32 greyi = (uint32)strtoul(param8, NULL, 0);
            uint32 whitei = (uint32)strtoul(param9, NULL, 0);
            uint32 greeni = (uint32)strtoul(param10, NULL, 0);
            uint32 bluei = (uint32)strtoul(param11, NULL, 0);
            uint32 purplei = (uint32)strtoul(param12, NULL, 0);
            uint32 orangei = (uint32)strtoul(param13, NULL, 0);
            uint32 yellowi = (uint32)strtoul(param14, NULL, 0);
            uint32 totalPercent = greytg + whitetg + greentg + bluetg + purpletg + orangetg + yellowtg + greyi + whitei + greeni + bluei + purplei + orangei + yellowi;
            if ((totalPercent == 0) || (totalPercent != 100))
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions percentages $ahMapID (2, 6 or 7) $1 $2 $3 $4 $5 $6 $7 $8 $9 $10 $11 $12 $13 $14");
                handler->PSendSysMessage("1 GreyTradeGoods 2 WhiteTradeGoods 3 GreenTradeGoods 4 BlueTradeGoods 5 PurpleTradeGoods");
                handler->PSendSysMessage("6 OrangeTradeGoods 7 YellowTradeGoods 8 GreyItems 9 WhiteItems 10 GreenItems 11 BlueItems");
                handler->PSendSysMessage("12 PurpleItems 13 OrangeItems 14 YellowItems");
                handler->PSendSysMessage("The total must add up to 100%%");
                return false;
            }
            char param[100];
            param[0] = '\0';
            strcat(param, param1);
            strcat(param, " ");
            strcat(param, param2);
            strcat(param, " ");
            strcat(param, param3);
            strcat(param, " ");
            strcat(param, param4);
            strcat(param, " ");
            strcat(param, param5);
            strcat(param, " ");
            strcat(param, param6);
            strcat(param, " ");
            strcat(param, param7);
            strcat(param, " ");
            strcat(param, param8);
            strcat(param, " ");
            strcat(param, param9);
            strcat(param, " ");
            strcat(param, param10);
            strcat(param, " ");
            strcat(param, param11);
            strcat(param, " ");
            strcat(param, param12);
            strcat(param, " ");
            strcat(param, param13);
            strcat(param, " ");
            strcat(param, param14);
            auctionbot.Commands(5, ahMapID, 0, param);
        }
        else if (strncmp(opt, "minprice", l) == 0)
        {
            char* param1 = strtok(NULL, " ");
            char* param2 = strtok(NULL, " ");
            if ((!ahMapIdStr) || (!param1) || (!param2))
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions minprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $price");
                return false;
            }
            if (strncmp(param1, "grey", l) == 0)
                auctionbot.Commands(6, ahMapID, AHB_GREY, param2);
            else if (strncmp(param1, "white", l) == 0)
                auctionbot.Commands(6, ahMapID, AHB_WHITE, param2);
            else if (strncmp(param1, "green", l) == 0)
                auctionbot.Commands(6, ahMapID, AHB_GREEN, param2);
            else if (strncmp(param1, "blue", l) == 0)
                auctionbot.Commands(6, ahMapID, AHB_BLUE, param2);
            else if (strncmp(param1, "purple", l) == 0)
                auctionbot.Commands(6, ahMapID, AHB_PURPLE, param2);
            else if (strncmp(param1, "orange", l) == 0)
                auctionbot.Commands(6, ahMapID, AHB_ORANGE, param2);
            else if (strncmp(param1, "yellow", l) == 0)
                auctionbot.Commands(6, ahMapID, AHB_YELLOW, param2);
            else
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions minprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $price");
                return false;
            }
        }
        else if (strncmp(opt, "maxprice", l) == 0)
        {
            char* param1 = strtok(NULL, " ");
            char* param2 = strtok(NULL, " ");
            if ((!ahMapIdStr) || (!param1) || (!param2))
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions maxprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $price");
                return false;
            }
            if (strncmp(param1, "grey", l) == 0)
                auctionbot.Commands(7, ahMapID, AHB_GREY, param2);
            else if (strncmp(param1, "white", l) == 0)
                auctionbot.Commands(7, ahMapID, AHB_WHITE, param2);
            else if (strncmp(param1, "green", l) == 0)
                auctionbot.Commands(7, ahMapID, AHB_GREEN, param2);
            else if (strncmp(param1, "blue", l) == 0)
                auctionbot.Commands(7, ahMapID, AHB_BLUE, param2);
            else if (strncmp(param1, "purple", l) == 0)
                auctionbot.Commands(7, ahMapID, AHB_PURPLE, param2);
            else if (strncmp(param1, "orange", l) == 0)
                auctionbot.Commands(7, ahMapID, AHB_ORANGE, param2);
            else if (strncmp(param1, "yellow", l) == 0)
                auctionbot.Commands(7, ahMapID, AHB_YELLOW, param2);
            else
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions maxprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $price");
                return false;
            }
        }
        else if (strncmp(opt, "minbidprice", l) == 0)
        {
            char* param1 = strtok(NULL, " ");
            char* param2 = strtok(NULL, " ");
            if ((!ahMapIdStr) || (!param1) || (!param2))
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions minbidprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $price");
                return false;
            }
            uint32 minBidPrice = (uint32)strtoul(param2, NULL, 0);
            if ((minBidPrice < 1) || (minBidPrice > 100))
            {
                handler->PSendSysMessage("The min bid price multiplier must be between 1 and 100");
                return false;
            }
            if (strncmp(param1, "grey", l) == 0)
                auctionbot.Commands(8, ahMapID, AHB_GREY, param2);
            else if (strncmp(param1, "white", l) == 0)
                auctionbot.Commands(8, ahMapID, AHB_WHITE, param2);
            else if (strncmp(param1, "green", l) == 0)
                auctionbot.Commands(8, ahMapID, AHB_GREEN, param2);
            else if (strncmp(param1, "blue", l) == 0)
                auctionbot.Commands(8, ahMapID, AHB_BLUE, param2);
            else if (strncmp(param1, "purple", l) == 0)
                auctionbot.Commands(8, ahMapID, AHB_PURPLE, param2);
            else if (strncmp(param1, "orange", l) == 0)
                auctionbot.Commands(8, ahMapID, AHB_ORANGE, param2);
            else if (strncmp(param1, "yellow", l) == 0)
                auctionbot.Commands(8, ahMapID, AHB_YELLOW, param2);
            else
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions minbidprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $price");
                return false;
            }
        }
        else if (strncmp(opt, "maxbidprice", l) == 0)
        {
            char* param1 = strtok(NULL, " ");
            char* param2 = strtok(NULL, " ");
            if ((!ahMapIdStr) || (!param1) || (!param2))
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions maxbidprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $price");
                return false;
            }
            uint32 maxBidPrice = (uint32)strtoul(param2, NULL, 0);
            if ((maxBidPrice < 1) || (maxBidPrice > 100))
            {
                handler->PSendSysMessage("The max bid price multiplier must be between 1 and 100");
                return false;
            }
            if (strncmp(param1, "grey", l) == 0)
                auctionbot.Commands(9, ahMapID, AHB_GREY, param2);
            else if (strncmp(param1, "white", l) == 0)
                auctionbot.Commands(9, ahMapID, AHB_WHITE, param2);
            else if (strncmp(param1, "green", l) == 0)
                auctionbot.Commands(9, ahMapID, AHB_GREEN, param2);
            else if (strncmp(param1, "blue", l) == 0)
                auctionbot.Commands(9, ahMapID, AHB_BLUE, param2);
            else if (strncmp(param1, "purple", l) == 0)
                auctionbot.Commands(9, ahMapID, AHB_PURPLE, param2);
            else if (strncmp(param1, "orange", l) == 0)
                auctionbot.Commands(9, ahMapID, AHB_ORANGE, param2);
            else if (strncmp(param1, "yellow", l) == 0)
                auctionbot.Commands(9, ahMapID, AHB_YELLOW, param2);
            else
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions max bidprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $price");
                return false;
            }
        }
        else if (strncmp(opt, "maxstack", l) == 0)
        {
            char* param1 = strtok(NULL, " ");
            char* param2 = strtok(NULL, " ");
            if ((!ahMapIdStr) || (!param1) || (!param2))
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions maxstack $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $value");
                return false;
            }
            int32 maxStack = (uint32)strtol(param2, NULL, 0);
            if (maxStack < 0)
            {
                handler->PSendSysMessage("maxstack can't be a negative number.");
                return false;
            }
            if (strncmp(param1, "grey", l) == 0)
                auctionbot.Commands(10, ahMapID, AHB_GREY, param2);
            else if (strncmp(param1, "white", l) == 0)
                auctionbot.Commands(10, ahMapID, AHB_WHITE, param2);
            else if (strncmp(param1, "green", l) == 0)
                auctionbot.Commands(10, ahMapID, AHB_GREEN, param2);
            else if (strncmp(param1, "blue", l) == 0)
                auctionbot.Commands(10, ahMapID, AHB_BLUE, param2);
            else if (strncmp(param1, "purple", l) == 0)
                auctionbot.Commands(10, ahMapID, AHB_PURPLE, param2);
            else if (strncmp(param1, "orange", l) == 0)
                auctionbot.Commands(10, ahMapID, AHB_ORANGE, param2);
            else if (strncmp(param1, "yellow", l) == 0)
                auctionbot.Commands(10, ahMapID, AHB_YELLOW, param2);
            else
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions maxstack $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $value");
                return false;
            }
        }
        else if (strncmp(opt, "buyerprice", l) == 0)
        {
            char* param1 = strtok(NULL, " ");
            char* param2 = strtok(NULL, " ");
            if ((!ahMapIdStr) || (!param1) || (!param2))
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions buyerprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue or purple) $price");
                return false;
            }
            if (strncmp(param1, "grey", l) == 0)
                auctionbot.Commands(11, ahMapID, AHB_GREY, param2);
            else if (strncmp(param1, "white", l) == 0)
                auctionbot.Commands(11, ahMapID, AHB_WHITE, param2);
            else if (strncmp(param1, "green", l) == 0)
                auctionbot.Commands(11, ahMapID, AHB_GREEN, param2);
            else if (strncmp(param1, "blue", l) == 0)
                auctionbot.Commands(11, ahMapID, AHB_BLUE, param2);
            else if (strncmp(param1, "purple", l) == 0)
                auctionbot.Commands(11, ahMapID, AHB_PURPLE, param2);
            else if (strncmp(param1, "orange", l) == 0)
                auctionbot.Commands(11, ahMapID, AHB_ORANGE, param2);
            else if (strncmp(param1, "yellow", l) == 0)
                auctionbot.Commands(11, ahMapID, AHB_YELLOW, param2);
            else
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions buyerprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue or purple) $price");
                return false;
            }
        }
        else if (strncmp(opt, "bidinterval", l) == 0)
        {
            char* param1 = strtok(NULL, " ");
            if ((!ahMapIdStr) || (!param1))
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions bidinterval $ahMapID (2, 6 or 7) $interval(in minutes)");
                return false;
            }
            auctionbot.Commands(12, ahMapID, 0, param1);
        }
        else if (strncmp(opt, "bidsperinterval", l) == 0)
        {
            char* param1 = strtok(NULL, " ");
            if ((!ahMapIdStr) || (!param1))
            {
                handler->PSendSysMessage("Syntax is: ahbotoptions bidsperinterval $ahMapID (2, 6 or 7) $bids");
                return false;
            }
            auctionbot.Commands(13, ahMapID, 0, param1);
        }
        else
        {
            handler->PSendSysMessage("Syntax is: ahbotoptions $option $ahMapID (2, 6 or 7) $parameter");
            handler->PSendSysMessage("Try ahbotoptions help to see a list of options.");
            return false;
        }
        return true;
    }

    static bool HandleFlushArenaPointsCommand(ChatHandler* /*handler*/,const char* /*args*/)
    {
        sBattlegroundMgr.DistributeArenaPoints();
        return true;
    }

    static bool HandlePlayAllCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 soundId = atoi((char*)args);

        if (!sSoundEntriesStore.LookupEntry(soundId))
        {
            handler->PSendSysMessage(LANG_SOUND_NOT_EXIST, soundId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        WorldPacket data(SMSG_PLAY_SOUND, 4);
        data << uint32(soundId) << handler->GetSession()->GetPlayer()->GetGUID();
        sWorld.SendGlobalMessage(&data);

        handler->PSendSysMessage(LANG_COMMAND_PLAYED_TO_ALL, soundId);
        return true;
    }

    static bool HandleRepairitemsCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* target = handler->getSelectedPlayer();

        if (!target)
        {
            handler->PSendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Repair items
        target->DurabilityRepairAll(false, 0, false);

        handler->PSendSysMessage(LANG_YOU_REPAIR_ITEMS, target->GetName());
        if (handler->needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_YOUR_ITEMS_REPAIRED, handler->GetName());
        return true;
    }

    static bool HandleFreezeCommand(ChatHandler* handler, const char* args)
    {
        std::string name;
        Player* player;
        char* TargetName = strtok((char*)args, " "); //get entered name
        if (!TargetName) //if no name entered use target
        {
            player = handler->getSelectedPlayer();
            if (player) //prevent crash with creature as target
            {
                name = player->GetName();
                normalizePlayerName(name);
            }
        }
        else // if name entered
        {
            name = TargetName;
            normalizePlayerName(name);
            player = sObjectMgr.GetPlayer(name.c_str()); //get player by name
        }

        if (!player)
        {
            handler->SendSysMessage(LANG_COMMAND_FREEZE_WRONG);
            return true;
        }

        if (player == handler->GetSession()->GetPlayer())
        {
            handler->SendSysMessage(LANG_COMMAND_FREEZE_ERROR);
            return true;
        }

        //effect
        if (player && player != handler->GetSession()->GetPlayer())
        {
            handler->PSendSysMessage(LANG_COMMAND_FREEZE, name.c_str());

            //stop combat + make player unattackable + duel stop + stop some spells
            player->SetFaction(35);
            player->CombatStop();
            if (player->IsNonMeleeSpellCast(true))
                player->InterruptNonMeleeSpells(true);
            player->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            //if player class = hunter || warlock remove pet if alive
            if ((player->getClass() == CLASS_HUNTER) || (player->getClass() == CLASS_WARLOCK))
            {
                if (Pet* pet = player->GetPet())
                {
                    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
                    // not let dismiss dead pet
                    if (pet->IsAlive())
                        player->RemovePet(pet, PET_SAVE_NOT_IN_SLOT);
                }
            }

            //m_session->GetPlayer()->CastSpell(player,spellID,false);
            if (SpellEntry const* spellInfo = sSpellStore.LookupEntry(9454))
            {
                for (uint32 i = 0; i < 3; i++)
                {
                    uint8 eff = spellInfo->Effect[i];
                    if (eff >= TOTAL_SPELL_EFFECTS)
                        continue;
                    if (eff == SPELL_EFFECT_APPLY_AREA_AURA_PARTY || eff == SPELL_EFFECT_APPLY_AURA ||
                        eff == SPELL_EFFECT_PERSISTENT_AREA_AURA || eff == SPELL_EFFECT_APPLY_AREA_AURA_FRIEND ||
                        eff == SPELL_EFFECT_APPLY_AREA_AURA_ENEMY)
                    {
                        Aura* Aur = CreateAura(spellInfo, i, NULL, player);
                        player->AddAura(Aur);
                    }
                }
            }

            //save player
            player->SaveToDB();
        }
        return true;
    }

    static bool HandleUnFreezeCommand(ChatHandler* handler, const char* args)
    {
        std::string name;
        Player* player;
        char* TargetName = strtok((char*)args, " "); //get entered name
        if (!TargetName) //if no name entered use target
        {
            player = handler->getSelectedPlayer();
            if (player) //prevent crash with creature as target
                name = player->GetName();
        }

        else // if name entered
        {
            name = TargetName;
            normalizePlayerName(name);
            player = sObjectMgr.GetPlayer(name.c_str()); //get player by name
        }

        //effect
        if (player)
        {
            handler->PSendSysMessage(LANG_COMMAND_UNFREEZE, name.c_str());

            //Reset player faction + allow combat + allow duels
            player->setFactionForRace(player->getRace());
            player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            //allow movement and spells
            player->RemoveAurasDueToSpell(9454);

            //save player
            player->SaveToDB();
        }

        if (!player)
        {
            if (TargetName)
            {
                //check for offline players
                QueryResult_AutoPtr result = CharacterDatabase.PQuery("SELECT characters.guid FROM characters WHERE characters.name = '%s'", name.c_str());
                if (!result)
                {
                    handler->SendSysMessage(LANG_COMMAND_FREEZE_WRONG);
                    return true;
                }
                //if player found: delete his freeze aura
                Field* fields = result->Fetch();
                uint64 pguid = fields[0].GetUInt64();

                CharacterDatabase.PQuery("DELETE FROM character_aura WHERE character_aura.spell = 9454 AND character_aura.guid = '" UI64FMTD "'", pguid);
                handler->PSendSysMessage(LANG_COMMAND_UNFREEZE, name.c_str());
                return true;
            }
            else
            {
                handler->SendSysMessage(LANG_COMMAND_FREEZE_WRONG);
                return true;
            }
        }

        return true;
    }

    static bool HandleListFreezeCommand(ChatHandler* handler, const char* /*args*/)
    {
        //Get names from DB
        QueryResult_AutoPtr result = CharacterDatabase.Query("SELECT characters.name FROM characters LEFT JOIN character_aura ON (characters.guid = character_aura.guid) WHERE character_aura.spell = 9454");
        if (!result)
        {
            handler->SendSysMessage(LANG_COMMAND_NO_FROZEN_PLAYERS);
            return true;
        }
        //Header of the names
        handler->PSendSysMessage(LANG_COMMAND_LIST_FREEZE);

        //Output of the results
        do
        {
            Field* fields = result->Fetch();
            std::string fplayers = fields[0].GetCppString();
            handler->PSendSysMessage(LANG_COMMAND_FROZEN_PLAYERS, fplayers.c_str());
        } while (result->NextRow());

        return true;
    }

    static bool HandlePossessCommand(ChatHandler* handler, const char* /*args*/)
    {
        Unit* pUnit = handler->getSelectedUnit();
        if (!pUnit)
            return false;

        handler->GetSession()->GetPlayer()->CastSpell(pUnit, 530, true);
        return true;
    }

    static bool HandleUnPossessCommand(ChatHandler* handler, const char* /*args*/)
    {
        Unit* pUnit = handler->getSelectedUnit();
        if (!pUnit)
            pUnit = handler->GetSession()->GetPlayer();

        pUnit->RemoveSpellsCausingAura(SPELL_AURA_MOD_CHARM);
        pUnit->RemoveSpellsCausingAura(SPELL_AURA_MOD_POSSESS_PET);
        pUnit->RemoveSpellsCausingAura(SPELL_AURA_MOD_POSSESS);

        return true;
    }

    static bool HandleBindSightCommand(ChatHandler* handler, const char* /*args*/)
    {
        Unit* pUnit = handler->getSelectedUnit();
        if (!pUnit)
            return false;

        handler->GetSession()->GetPlayer()->CastSpell(pUnit, 6277, true);
        return true;
    }

    static bool HandleUnbindSightCommand(ChatHandler* handler, const char* /*args*/)
    {
        if (handler->GetSession()->GetPlayer()->isPossessing())
            return false;

        handler->GetSession()->GetPlayer()->StopCastingBindSight();
        return true;
    }

    static bool HandleRAFInfoCommand(ChatHandler* handler, const char* args)
    {
        uint64 account = strtoull(args, NULL, 10);
        if (!account)
            return false;

        uint64 linked;
        switch (sObjectMgr.GetRAFLinkStatus(account, &linked))
        {
        case RAF_LINK_NONE:
            handler->PSendSysMessage("Account " UI64FMTD " is not linked to any account.", account);
            break;
        case RAF_LINK_REFERRER:
            handler->PSendSysMessage("Account " UI64FMTD " is referrer account. (friend is: " UI64FMTD ")", account, linked);
            break;
        case RAF_LINK_REFERRED:
            handler->PSendSysMessage("Account " UI64FMTD " is referred account. (friend is: " UI64FMTD ")", account, linked);
            if (Player* player = ObjectAccessor::Instance().FindPlayerByAccountId(account))
                handler->PSendSysMessage("Character %s has %.02f grantable levels", player->GetName(), player->GetGrantableLevels());
            break;
        }

        return true;
    }

    static bool HandleRAFLinkCommand(ChatHandler* handler, const char* args)
    {
        uint64 referrer = strtoull(args, NULL, 10);
        if (!referrer)
            return false;

        args = strchr(args, ' ');
        if (!args)
            return false;

        uint64 referred = strtoull(args, NULL, 10);
        if (!referred)
            return false;

        if (sObjectMgr.GetRAFLinkStatus(referrer) != RAF_LINK_NONE ||
            sObjectMgr.GetRAFLinkStatus(referred) != RAF_LINK_NONE)
        {
            handler->PSendSysMessage("First or second account is already linked with an account.");
            return true;
        }

        sObjectMgr.LinkIntoRAF(referrer, referred);
        handler->PSendSysMessage("Accounts successfully linked.");
        return true;
    }

    static bool HandleRAFUnlinkCommand(ChatHandler* handler, const char* args)
    {
        uint64 acc = strtoull(args, NULL, 10);
        if (!acc)
            return false;

        sObjectMgr.UnlinkFromRAF(acc);
        handler->PSendSysMessage("Account was sucessfully unlinked");
        return true;
    }

    static bool HandleRAFSummonCommand(ChatHandler* handler, const char* /*args*/)
    {
        if (!handler->GetSession() || !handler->GetSession()->GetPlayer() || !handler->GetSession()->GetPlayer()->IsInWorld())
            return true;

        handler->GetSession()->GetPlayer()->CastSpell(handler->GetSession()->GetPlayer(), SPELL_SUMMON_FRIEND, false);
        return true;
    }

    static bool HandleRAFGrantLevelCommand(ChatHandler* handler, const char* /*args*/)
    {
        if (!handler->GetSession() || !handler->GetSession()->GetPlayer() || !handler->GetSession()->GetPlayer()->IsInWorld())
            return true;

        Player* buddy = sObjectMgr.GetRAFLinkedBuddyForPlayer(handler->GetSession()->GetPlayer());
        if (!buddy || !buddy->IsInWorld())
        {
            handler->PSendSysMessage("Couldn't find you friend");
            return true;
        }

        PackedGuid guid = buddy->GetPackGUID();

        WorldPacket data(CMSG_GRANT_LEVEL, guid.size());
        data << guid;
        handler->GetSession()->HandleGrantLevel(data);

        return true;
    }

    // Teleport player to last position
    static bool HandleRecallCommand(ChatHandler* handler, const char* args)
    {
        Player* target = NULL;

        if (!*args)
        {
            target = handler->getSelectedPlayerOrSelf();
        }
        else
        {
            std::string name = args;

            if (!normalizePlayerName(name))
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            target = sObjectMgr.GetPlayer(name.c_str());

            if (!target)
            {
                handler->PSendSysMessage(LANG_NO_PLAYER, args);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        if (target->IsBeingTeleported())
        {
            handler->PSendSysMessage(LANG_IS_TELEPORTED, target->GetName());
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (target->IsInFlight())
        {
            target->GetMotionMaster()->MovementExpired();
            target->m_taxi.ClearTaxiDestinations();
        }

        target->TeleportTo(target->m_recallMap, target->m_recallX, target->m_recallY, target->m_recallZ, target->m_recallO);
        return true;
    }

    static bool HandleCommandsCommand(ChatHandler* handler, const char* /*args*/)
    {
        handler->ShowHelpForCommand(handler->getCommandTable(), "");
        return true;
    }

    static bool HandleStartCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* chr = handler->GetSession()->GetPlayer();

        if (chr->IsInFlight())
        {
            handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (chr->IsInCombat())
        {
            handler->SendSysMessage(LANG_YOU_IN_COMBAT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Teleport to starting location
        chr->TeleportTo(chr->GetStartPosition());
        return true;
    }

    static bool HandleAllowMovementCommand(ChatHandler* handler, const char* /*args*/)
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

};

void AddSC_misc_commandscript()
{
    new misc_commandscript();
}