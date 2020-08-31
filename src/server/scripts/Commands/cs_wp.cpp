#include "Chat.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "WaypointManager.h"

class wp_commandscript : public CommandScript
{
public:
    wp_commandscript() : CommandScript("wp_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> wpCommandTable =
        {
            { "add",            SEC_ADMINISTRATOR,     false, &HandleWpAddCommand,                "" },
            { "event",          SEC_ADMINISTRATOR,     false, &HandleWpEventCommand,              "" },
            { "modify",         SEC_ADMINISTRATOR,     false, &HandleWpModifyCommand,             "" },
            { "show",           SEC_ADMINISTRATOR,     false, &HandleWpShowCommand,               "" }
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "wp",             SEC_ADMINISTRATOR,     false, nullptr,                            "", wpCommandTable }
        };
        return commandTable;
    }

    static bool HandleWpAddCommand(ChatHandler* handler, const char* args)
    {
        sLog.outDebug("DEBUG: HandleWpAddCommand");

        // optional
        char* path_number = NULL;
        uint32 pathid = 0;

        if (*args)
            path_number = strtok((char*)args, " ");

        uint32 point = 0;
        Creature* target = handler->getSelectedCreature();

        if (!path_number)
        {
            if (target)
                pathid = target->GetWaypointPath();
            else
            {
                QueryResult_AutoPtr result = WorldDatabase.Query("SELECT MAX(id) FROM waypoint_data");
                uint32 maxpathid = result->Fetch()->GetInt32();
                pathid = maxpathid + 1;
                sLog.outDebug("DEBUG: HandleWpAddCommand - New path started.");
                handler->PSendSysMessage("%s%s|r", "|cff00ff00", "New path started.");
            }
        }
        else
            pathid = atoi(path_number);

        // path_id -> ID of the Path
        // point   -> number of the waypoint (if not 0)

        if (!pathid)
        {
            sLog.outDebug("DEBUG: HandleWpAddCommand - Current creature has no loaded path.");
            handler->PSendSysMessage("%s%s|r", "|cffff33ff", "Current creature has no loaded path.");
            return true;
        }

        sLog.outDebug("DEBUG: HandleWpAddCommand - point == 0");

        QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT MAX(point) FROM waypoint_data WHERE id = '%u'", pathid);

        if (result)
            point = (*result)[0].GetUInt32();

        Player* player = handler->GetSession()->GetPlayer();
        //Map *map = player->GetMap();

        WorldDatabase.PExecuteLog("INSERT INTO waypoint_data (id, point, position_x, position_y, position_z) VALUES ('%u','%u','%f', '%f', '%f')",
            pathid, point + 1, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());

        handler->PSendSysMessage("%s%s%u%s%u%s|r", "|cff00ff00", "PathID: |r|cff00ffff", pathid, "|r|cff00ff00: Waypoint |r|cff00ffff", point + 1, "|r|cff00ff00 created. ");
        return true;
    }                                                           // HandleWpAddCommand

    static bool HandleWpEventCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* show_str = strtok((char*)args, " ");
        std::string show = show_str;

        // Check
        if ((show != "add") && (show != "mod") && (show != "del") && (show != "listid")) return false;

        char* arg_id = strtok(NULL, " ");
        uint32 id = 0;

        if (show == "add")
        {
            if (arg_id)
                id = atoi(arg_id);

            if (id)
            {
                QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT id FROM waypoint_scripts WHERE guid = %u", id);

                if (!result)
                {
                    WorldDatabase.PExecute("INSERT INTO waypoint_scripts(guid)VALUES(%u)", id);
                    handler->PSendSysMessage("%s%s%u|r", "|cff00ff00", "Wp Event: New waypoint event added: ", id);
                }
                else
                    handler->PSendSysMessage("|cff00ff00Wp Event: You have chosen an existing waypoint script guid: %u|r", id);
            }
            else
            {
                QueryResult_AutoPtr result = WorldDatabase.Query("SELECT MAX(guid) FROM waypoint_scripts");
                id = result->Fetch()->GetUInt32();
                WorldDatabase.PExecute("INSERT INTO waypoint_scripts(guid)VALUES(%u)", id + 1);
                handler->PSendSysMessage("%s%s%u|r", "|cff00ff00", "Wp Event: New waypoint event added: |r|cff00ffff", id + 1);
            }

            return true;
        }

        if (show == "listid")
        {
            if (!arg_id)
            {
                handler->PSendSysMessage("%s%s|r", "|cff33ffff", "Wp Event: You must provide waypoint script id.");
                return true;
            }

            id = atoi(arg_id);

            uint32 a2, a3, a4, a5, a6;
            float a8, a9, a10, a11;
            char const* a7;

            QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT guid, delay, command, datalong, datalong2, dataint, x, y, z, o FROM waypoint_scripts WHERE id = %u", id);

            if (!result)
            {
                handler->PSendSysMessage("%s%s%u|r", "|cff33ffff", "Wp Event: No waypoint scripts found on id: ", id);
                return true;
            }

            Field* fields;

            do
            {
                fields = result->Fetch();
                a2 = fields[0].GetUInt32();
                a3 = fields[1].GetUInt32();
                a4 = fields[2].GetUInt32();
                a5 = fields[3].GetUInt32();
                a6 = fields[4].GetUInt32();
                a7 = fields[5].GetString();
                a8 = fields[6].GetFloat();
                a9 = fields[7].GetFloat();
                a10 = fields[8].GetFloat();
                a11 = fields[9].GetFloat();

                handler->PSendSysMessage("|cffff33ffid:|r|cff00ffff %u|r|cff00ff00, guid: |r|cff00ffff%u|r|cff00ff00, delay: |r|cff00ffff%u|r|cff00ff00, command: |r|cff00ffff%u|r|cff00ff00, datalong: |r|cff00ffff%u|r|cff00ff00, datalong2: |r|cff00ffff%u|r|cff00ff00, datatext: |r|cff00ffff%s|r|cff00ff00, posx: |r|cff00ffff%f|r|cff00ff00, posy: |r|cff00ffff%f|r|cff00ff00, posz: |r|cff00ffff%f|r|cff00ff00, orientation: |r|cff00ffff%f|r", id, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
            } while (result->NextRow());
        }

        if (show == "del")
        {
            id = atoi(arg_id);

            QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT guid FROM waypoint_scripts WHERE guid = %u", id);

            if (result)
            {
                WorldDatabase.PExecuteLog("DELETE FROM waypoint_scripts WHERE guid = %u", id);
                handler->PSendSysMessage("%s%s%u|r", "|cff00ff00", "Wp Event: Waypoint script removed: ", id);
            }
            else
                handler->PSendSysMessage("|cffff33ffWp Event: ERROR: you have selected an invalid script: %u|r", id);

            return true;
        }

        if (show == "mod")
        {
            if (!arg_id)
            {
                handler->SendSysMessage("|cffff33ffERROR: Waypoint script guid not present.|r");
                return true;
            }

            id = atoi(arg_id);

            if (!id)
            {
                handler->SendSysMessage("|cffff33ffERROR: No vallid waypoint script id not present.|r");
                return true;
            }

            char* arg_2 = strtok(NULL, " ");

            if (!arg_2)
            {
                handler->SendSysMessage("|cffff33ffERROR: No argument present.|r");
                return true;
            }

            std::string arg_string = arg_2;

            if ((arg_string != "setid") && (arg_string != "delay") && (arg_string != "command")
                && (arg_string != "datalong") && (arg_string != "datalong2") && (arg_string != "dataint") && (arg_string != "posx")
                && (arg_string != "posy") && (arg_string != "posz") && (arg_string != "orientation"))
            {
                handler->SendSysMessage("|cffff33ffERROR: No valid argument present.|r");
                return true;
            }

            char* arg_3;
            std::string arg_str_2 = arg_2;
            arg_3 = strtok(NULL, " ");

            if (!arg_3)
            {
                handler->SendSysMessage("|cffff33ffERROR: No additional argument present.|r");
                return true;
            }

            float coord;

            if (arg_str_2 == "setid")
            {
                uint32 newid = atoi(arg_3);
                handler->PSendSysMessage("%s%s|r|cff00ffff%u|r|cff00ff00%s|r|cff00ffff%u|r", "|cff00ff00", "Wp Event: Wypoint scipt guid: ", newid, " id changed: ", id);
                WorldDatabase.PExecuteLog("UPDATE waypoint_scripts SET id='%u' WHERE guid='%u'",
                    newid, id);
                return true;
            }
            else
            {
                QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT id FROM waypoint_scripts WHERE guid='%u'", id);

                if (!result)
                {
                    handler->SendSysMessage("|cffff33ffERROR: You have selected an invalid waypoint script guid.|r");
                    return true;
                }

                if (arg_str_2 == "posx")
                {
                    coord = atof(arg_3);
                    WorldDatabase.PExecuteLog("UPDATE waypoint_scripts SET x='%f' WHERE guid='%u'",
                        coord, id);
                    handler->PSendSysMessage("|cff00ff00Waypoint script:|r|cff00ffff %u|r|cff00ff00 position_x updated.|r", id);
                    return true;
                }
                else if (arg_str_2 == "posy")
                {
                    coord = atof(arg_3);
                    WorldDatabase.PExecuteLog("UPDATE waypoint_scripts SET y='%f' WHERE guid='%u'",
                        coord, id);
                    handler->PSendSysMessage("|cff00ff00Waypoint script: %u position_y updated.|r", id);
                    return true;
                }
                else if (arg_str_2 == "posz")
                {
                    coord = atof(arg_3);
                    WorldDatabase.PExecuteLog("UPDATE waypoint_scripts SET z='%f' WHERE guid='%u'",
                        coord, id);
                    handler->PSendSysMessage("|cff00ff00Waypoint script: |r|cff00ffff%u|r|cff00ff00 position_z updated.|r", id);
                    return true;
                }
                else if (arg_str_2 == "orientation")
                {
                    coord = atof(arg_3);
                    WorldDatabase.PExecuteLog("UPDATE waypoint_scripts SET o='%f' WHERE guid='%u'",
                        coord, id);
                    handler->PSendSysMessage("|cff00ff00Waypoint script: |r|cff00ffff%u|r|cff00ff00 orientation updated.|r", id);
                    return true;
                }
                else if (arg_str_2 == "dataint")
                {
                    WorldDatabase.PExecuteLog("UPDATE waypoint_scripts SET %s='%u' WHERE guid='%u'",
                        arg_2, atoi(arg_3), id);
                    handler->PSendSysMessage("|cff00ff00Waypoint script: |r|cff00ffff%u|r|cff00ff00 dataint updated.|r", id);
                    return true;
                }
                else
                {
                    std::string arg_str_3 = arg_3;
                    WorldDatabase.escape_string(arg_str_3);
                    WorldDatabase.PExecuteLog("UPDATE waypoint_scripts SET %s='%s' WHERE guid='%u'",
                        arg_2, arg_str_3.c_str(), id);
                }
            }
            handler->PSendSysMessage("%s%s|r|cff00ffff%u:|r|cff00ff00 %s %s|r", "|cff00ff00", "Waypoint script:", id, arg_2, "updated.");
        }
        return true;
    }

    static bool HandleWpModifyCommand(ChatHandler* handler, const char* args)
    {
        sLog.outDebug("DEBUG: HandleWpModifyCommand");

        if (!*args)
            return false;

        // first arg: add del text emote spell waittime move
        char* show_str = strtok((char*)args, " ");
        if (!show_str)
            return false;

        std::string show = show_str;
        // Check
        // Remember: "show" must also be the name of a column!
        if ((show != "delay") && (show != "action") && (show != "action_chance")
            && (show != "move_type") && (show != "del") && (show != "move") && (show != "wpadd")
            )
            return false;

        // Next arg is: <PATHID> <WPNUM> <ARGUMENT>
        char* arg_str = NULL;

        // Did user provide a GUID
        // or did the user select a creature?
        // -> variable lowguid is filled with the GUID of the NPC
        uint32 pathid = 0;
        uint32 point = 0;
        uint32 wpGuid = 0;
        Creature* target = handler->getSelectedCreature();

        if (!target || target->GetEntry() != VISUAL_WAYPOINT)
        {
            handler->SendSysMessage("|cffff33ffERROR: You must select a waypoint.|r");
            return false;
        }

        sLog.outDebug("DEBUG: HandleWpModifyCommand - User did select an NPC");
        // The visual waypoint
        Creature* wpCreature = NULL;
        wpGuid = target->GetGUIDLow();

        // Did the user select a visual spawnpoint?
        if (wpGuid)
            wpCreature = handler->GetSession()->GetPlayer()->GetMap()->GetCreature(MAKE_NEW_GUID(wpGuid, VISUAL_WAYPOINT, HIGHGUID_UNIT));
        // attempt check creature existence by DB data
        else
        {
            handler->PSendSysMessage(LANG_WAYPOINT_CREATNOTFOUND, wpGuid);
            return false;
        }
        // User did select a visual waypoint?
        // Check the creature
        if (wpCreature->GetEntry() == VISUAL_WAYPOINT)
        {
            QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT id, point FROM waypoint_data WHERE wpguid = %u", wpGuid);

            if (!result)
            {
                sLog.outDebug("DEBUG: HandleWpModifyCommand - No waypoint found - used 'wpguid'");

                handler->PSendSysMessage(LANG_WAYPOINT_NOTFOUNDSEARCH, target->GetGUIDLow());
                // Select waypoint number from database
                // Since we compare float values, we have to deal with
                // some difficulties.
                // Here we search for all waypoints that only differ in one from 1 thousand
                // (0.001) - There is no other way to compare C++ floats with mySQL floats
                // See also: http://dev.mysql.com/doc/refman/5.0/en/problems-with-float.html
                const char* maxDIFF = "0.01";
                result = WorldDatabase.PQuery("SELECT id, point FROM waypoint_data WHERE (abs(position_x - %f) <= %s) and (abs(position_y - %f) <= %s) and (abs(position_z - %f) <= %s)",
                    wpCreature->GetPositionX(), maxDIFF, wpCreature->GetPositionY(), maxDIFF, wpCreature->GetPositionZ(), maxDIFF);
                if (!result)
                {
                    handler->PSendSysMessage(LANG_WAYPOINT_NOTFOUNDDBPROBLEM, wpGuid);
                    return true;
                }
            }
            sLog.outDebug("DEBUG: HandleWpModifyCommand - After getting wpGuid");

            do
            {
                Field* fields = result->Fetch();
                pathid = fields[0].GetUInt32();
                point = fields[1].GetUInt32();
            } while (result->NextRow());

            // We have the waypoint number and the GUID of the "master npc"
            // Text is enclosed in "<>", all other arguments not
            arg_str = strtok((char*)NULL, " ");
        }

        sLog.outDebug("DEBUG: HandleWpModifyCommand - Parameters parsed - now execute the command");

        // Check for argument
        if (show != "del" && show != "move" && arg_str == NULL)
        {
            handler->PSendSysMessage(LANG_WAYPOINT_ARGUMENTREQ, show_str);
            return false;
        }

        if (show == "del" && target)
        {
            handler->PSendSysMessage("|cff00ff00DEBUG: wp modify del, PathID: |r|cff00ffff%u|r", pathid);

            // wpCreature
            Creature* wpCreature = NULL;

            if (wpGuid != 0)
            {
                wpCreature = handler->GetSession()->GetPlayer()->GetMap()->GetCreature(MAKE_NEW_GUID(wpGuid, VISUAL_WAYPOINT, HIGHGUID_UNIT));
                wpCreature->CombatStop();
                wpCreature->DeleteFromDB();
                wpCreature->AddObjectToRemoveList();
            }

            WorldDatabase.PExecuteLog("DELETE FROM waypoint_data WHERE id='%u' AND point='%u'",
                pathid, point);
            WorldDatabase.PExecuteLog("UPDATE waypoint_data SET point=point-1 WHERE id='%u' AND point>'%u'",
                pathid, point);

            handler->PSendSysMessage(LANG_WAYPOINT_REMOVED);
            return true;
        }                                                       // del

        if (show == "move" && target)
        {
            handler->PSendSysMessage("|cff00ff00DEBUG: wp move, PathID: |r|cff00ffff%u|r", pathid);

            Player* chr = handler->GetSession()->GetPlayer();
            Map* map = chr->GetMap();
            {
                // wpCreature
                Creature* wpCreature = NULL;
                // What to do:
                // Move the visual spawnpoint
                // Respawn the owner of the waypoints
                if (wpGuid != 0)
                {
                    wpCreature = handler->GetSession()->GetPlayer()->GetMap()->GetCreature(MAKE_NEW_GUID(wpGuid, VISUAL_WAYPOINT, HIGHGUID_UNIT));
                    wpCreature->CombatStop();
                    wpCreature->DeleteFromDB();
                    wpCreature->AddObjectToRemoveList();
                    // re-create
                    Creature* wpCreature2 = new Creature;
                    if (!wpCreature2->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_UNIT), map, PHASEMASK_NORMAL, VISUAL_WAYPOINT, 0, chr->GetPositionX(), chr->GetPositionY(), chr->GetPositionZ(), chr->GetOrientation()))
                    {
                        handler->PSendSysMessage(LANG_WAYPOINT_VP_NOTCREATED, VISUAL_WAYPOINT);
                        delete wpCreature2;
                        return false;
                    }

                    wpCreature2->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), chr->GetPhaseMaskForSpawn());
                    // To call _LoadGoods(); _LoadQuests(); CreateTrainerSpells();
                    //TODO: Should we first use "Create" then use "LoadFromDB"?
                    if (!wpCreature2->LoadCreatureFromDB(wpCreature2->GetDBTableGUIDLow(), map))
                    {
                        handler->PSendSysMessage(LANG_WAYPOINT_VP_NOTCREATED, VISUAL_WAYPOINT);
                        delete wpCreature2;
                        return false;
                    }
                }

                WorldDatabase.PExecuteLog("UPDATE waypoint_data SET position_x = '%f',position_y = '%f',position_z = '%f' where id = '%u' AND point='%u'",
                    chr->GetPositionX(), chr->GetPositionY(), chr->GetPositionZ(), pathid, point);

                handler->PSendSysMessage(LANG_WAYPOINT_CHANGED);
            }
            return true;
        }                                                       // move

        const char* text = arg_str;

        if (text == 0)
        {
            // show_str check for present in list of correct values, no sql injection possible
            WorldDatabase.PExecuteLog("UPDATE waypoint_data SET %s=NULL WHERE id='%u' AND point='%u'",
                show_str, pathid, point);
        }
        else
        {
            // show_str check for present in list of correct values, no sql injection possible
            std::string text2 = text;
            WorldDatabase.escape_string(text2);
            WorldDatabase.PExecuteLog("UPDATE waypoint_data SET %s='%s' WHERE id='%u' AND point='%u'",
                show_str, text2.c_str(), pathid, point);
        }

        handler->PSendSysMessage(LANG_WAYPOINT_CHANGED_NO, show_str);
        return true;
    }

    static bool HandleWpShowCommand(ChatHandler* handler, const char* args)
    {
        sLog.outDebug("DEBUG: HandleWpShowCommand");

        if (!*args)
            return false;

        // first arg: on, off, first, last
        char* show_str = strtok((char*)args, " ");
        if (!show_str)
            return false;

        // second arg: GUID (optional, if a creature is selected)
        char* guid_str = strtok((char*)NULL, " ");
        sLog.outDebug("DEBUG: HandleWpShowCommand: show_str: %s guid_str: %s", show_str, guid_str);

        uint32 pathid = 0;
        Creature* target = handler->getSelectedCreature();

        // Did player provide a PathID?

        if (!guid_str)
        {
            sLog.outDebug("DEBUG: HandleWpShowCommand: !guid_str");
            // No PathID provided
            // -> Player must have selected a creature

            if (!target)
            {
                handler->SendSysMessage(LANG_SELECT_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            pathid = target->GetWaypointPath();
        }
        else
        {
            sLog.outDebug("|cff00ff00DEBUG: HandleWpShowCommand: PathID provided|r");
            // PathID provided
            // Warn if player also selected a creature
            // -> Creature selection is ignored <-
            if (target)
                handler->SendSysMessage(LANG_WAYPOINT_CREATSELECTED);

            pathid = atoi((char*)guid_str);
        }

        sLog.outDebug("DEBUG: HandleWpShowCommand: danach");

        std::string show = show_str;
        uint32 Maxpoint;

        sLog.outDebug("DEBUG: HandleWpShowCommand: PathID: %u", pathid);

        //PSendSysMessage("wpshow - show: %s", show);

        // Show info for the selected waypoint
        if (show == "info")
        {
            // Check if the user did specify a visual waypoint
            if (target->GetEntry() != VISUAL_WAYPOINT)
            {
                handler->PSendSysMessage(LANG_WAYPOINT_VP_SELECT);
                handler->SetSentErrorMessage(true);
                return false;
            }

            QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT id, point, delay, move_type, action, action_chance FROM waypoint_data WHERE wpguid = %u", target->GetGUIDLow());

            if (!result)
            {
                handler->SendSysMessage(LANG_WAYPOINT_NOTFOUNDDBPROBLEM);
                return true;
            }

            handler->SendSysMessage("|cff00ffffDEBUG: wp show info:|r");
            do
            {
                Field* fields = result->Fetch();
                pathid = fields[0].GetUInt32();
                uint32 point = fields[1].GetUInt32();
                uint32 delay = fields[2].GetUInt32();
                uint32 flag = fields[3].GetUInt32();
                uint32 ev_id = fields[4].GetUInt32();
                uint32 ev_chance = fields[5].GetUInt32();

                handler->PSendSysMessage("|cff00ff00Show info: for current point: |r|cff00ffff%u|r|cff00ff00, Path ID: |r|cff00ffff%u|r", point, pathid);
                handler->PSendSysMessage("|cff00ff00Show info: delay: |r|cff00ffff%u|r", delay);
                handler->PSendSysMessage("|cff00ff00Show info: Move flag: |r|cff00ffff%u|r", flag);
                handler->PSendSysMessage("|cff00ff00Show info: Waypoint event: |r|cff00ffff%u|r", ev_id);
                handler->PSendSysMessage("|cff00ff00Show info: Event chance: |r|cff00ffff%u|r", ev_chance);
            } while (result->NextRow());

            return true;
        }

        if (show == "on")
        {
            QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT point, position_x,position_y,position_z FROM waypoint_data WHERE id = '%u'", pathid);

            if (!result)
            {
                handler->SendSysMessage("|cffff33ffPath no found.|r");
                handler->SetSentErrorMessage(true);
                return false;
            }

            handler->PSendSysMessage("|cff00ff00DEBUG: wp on, PathID: |cff00ffff%u|r", pathid);

            // Delete all visuals for this NPC
            QueryResult_AutoPtr result2 = WorldDatabase.PQuery("SELECT wpguid FROM waypoint_data WHERE id = '%u' and wpguid <> 0", pathid);

            if (result2)
            {
                bool hasError = false;
                do
                {
                    Field* fields = result2->Fetch();
                    uint32 wpguid = fields[0].GetUInt32();
                    Creature* pCreature = handler->GetSession()->GetPlayer()->GetMap()->GetCreature(MAKE_NEW_GUID(wpguid, VISUAL_WAYPOINT, HIGHGUID_UNIT));

                    if (!pCreature)
                    {
                        handler->PSendSysMessage(LANG_WAYPOINT_NOTREMOVED, wpguid);
                        hasError = true;
                        WorldDatabase.PExecuteLog("DELETE FROM creature WHERE guid = '%u'", wpguid);
                    }
                    else
                    {
                        pCreature->CombatStop();
                        pCreature->DeleteFromDB();
                        pCreature->AddObjectToRemoveList();
                    }

                } while (result2->NextRow());

                if (hasError)
                {
                    handler->PSendSysMessage(LANG_WAYPOINT_TOOFAR1);
                    handler->PSendSysMessage(LANG_WAYPOINT_TOOFAR2);
                    handler->PSendSysMessage(LANG_WAYPOINT_TOOFAR3);
                }
            }

            do
            {
                Field* fields = result->Fetch();
                uint32 point = fields[0].GetUInt32();
                float x = fields[1].GetFloat();
                float y = fields[2].GetFloat();
                float z = fields[3].GetFloat();

                uint32 id = VISUAL_WAYPOINT;

                Player* chr = handler->GetSession()->GetPlayer();
                Map* map = chr->GetMap();
                float o = chr->GetOrientation();

                Creature* wpCreature = new Creature;
                if (!wpCreature->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_UNIT), map, chr->GetPhaseMaskForSpawn(), id, 0, x, y, z, o))
                {
                    handler->PSendSysMessage(LANG_WAYPOINT_VP_NOTCREATED, id);
                    delete wpCreature;
                    return false;
                }

                sLog.outDebug("DEBUG: UPDATE waypoint_data SET wpguid = '%u'", wpCreature->GetGUIDLow());
                // set "wpguid" column to the visual waypoint
                WorldDatabase.PExecuteLog("UPDATE waypoint_data SET wpguid = '%u' WHERE id = '%u' and point = '%u'", wpCreature->GetGUIDLow(), pathid, point);

                wpCreature->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), chr->GetPhaseMaskForSpawn());
                if (!wpCreature->LoadCreatureFromDB(wpCreature->GetDBTableGUIDLow(), map))
                {
                    handler->PSendSysMessage(LANG_WAYPOINT_VP_NOTCREATED, id);
                    delete wpCreature;
                    return false;
                }

                if (target)
                {
                    wpCreature->SetDisplayId(target->GetDisplayId());
                    wpCreature->SetObjectScale(0.5);
                }
            } while (result->NextRow());

            handler->SendSysMessage("|cff00ff00Showing the current creature's path.|r");
            return true;
        }

        if (show == "first")
        {
            handler->PSendSysMessage("|cff00ff00DEBUG: wp first, GUID: %u|r", pathid);

            QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT position_x,position_y,position_z,orientation FROM waypoint_data WHERE point='1' AND id = '%u'", pathid);
            if (!result)
            {
                handler->PSendSysMessage(LANG_WAYPOINT_NOTFOUND, pathid);
                handler->SetSentErrorMessage(true);
                return false;
            }

            Field* fields = result->Fetch();
            float x = fields[0].GetFloat();
            float y = fields[1].GetFloat();
            float z = fields[2].GetFloat();
            float o = fields[3].GetFloat();
            uint32 id = VISUAL_WAYPOINT;

            Player* chr = handler->GetSession()->GetPlayer();
            Map* map = chr->GetMap();

            Creature* creature = new Creature;
            if (!creature->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_UNIT), map, chr->GetPhaseMaskForSpawn(), id, 0, x, y, z, o))
            {
                handler->PSendSysMessage(LANG_WAYPOINT_VP_NOTCREATED, id);
                delete creature;
                return false;
            }

            creature->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), chr->GetPhaseMaskForSpawn());
            if (!creature->LoadCreatureFromDB(creature->GetDBTableGUIDLow(), map))
            {
                handler->PSendSysMessage(LANG_WAYPOINT_VP_NOTCREATED, id);
                delete creature;
                return false;
            }

            if (target)
            {
                creature->SetDisplayId(target->GetDisplayId());
                creature->SetObjectScale(0.5);
            }

            return true;
        }

        if (show == "last")
        {
            handler->PSendSysMessage("|cff00ff00DEBUG: wp last, PathID: |r|cff00ffff%u|r", pathid);

            QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT MAX(point) FROM waypoint_data WHERE id = '%u'", pathid);
            if (result)
                Maxpoint = (*result)[0].GetUInt32();
            else
                Maxpoint = 0;

            result = WorldDatabase.PQuery("SELECT position_x,position_y,position_z FROM waypoint_data WHERE point ='%u' AND id = '%u'", Maxpoint, pathid);
            if (!result)
            {
                handler->PSendSysMessage(LANG_WAYPOINT_NOTFOUNDLAST, pathid);
                handler->SetSentErrorMessage(true);
                return false;
            }
            Field* fields = result->Fetch();
            float x = fields[0].GetFloat();
            float y = fields[1].GetFloat();
            float z = fields[2].GetFloat();
            float o = fields[3].GetFloat();
            uint32 id = VISUAL_WAYPOINT;

            Player* chr = handler->GetSession()->GetPlayer();
            Map* map = chr->GetMap();

            Creature* creature = new Creature;
            if (!creature->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_UNIT), map, chr->GetPhaseMaskForSpawn(), id, 0, x, y, z, o))
            {
                handler->PSendSysMessage(LANG_WAYPOINT_NOTCREATED, id);
                delete creature;
                return false;
            }

            creature->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), chr->GetPhaseMaskForSpawn());
            if (!creature->LoadCreatureFromDB(creature->GetDBTableGUIDLow(), map))
            {
                handler->PSendSysMessage(LANG_WAYPOINT_NOTCREATED, id);
                delete creature;
                return false;
            }

            if (target)
            {
                creature->SetDisplayId(target->GetDisplayId());
                creature->SetObjectScale(0.5);
            }

            return true;
        }

        if (show == "off")
        {
            QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT guid FROM creature WHERE id = '%u'", 1);
            if (!result)
            {
                handler->SendSysMessage(LANG_WAYPOINT_VP_NOTFOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }
            bool hasError = false;
            do
            {
                Field* fields = result->Fetch();
                uint32 guid = fields[0].GetUInt32();
                Creature* creature = handler->GetSession()->GetPlayer()->GetMap()->GetCreature(MAKE_NEW_GUID(guid, VISUAL_WAYPOINT, HIGHGUID_UNIT));
                if (!creature)
                {
                    handler->PSendSysMessage(LANG_WAYPOINT_NOTREMOVED, guid);
                    hasError = true;
                    WorldDatabase.PExecuteLog("DELETE FROM creature WHERE guid = '%u'", guid);
                }
                else
                {
                    creature->CombatStop();
                    creature->DeleteFromDB();
                    creature->AddObjectToRemoveList();
                }
            } while (result->NextRow());
            // set "wpguid" column to "empty" - no visual waypoint spawned
            WorldDatabase.PExecuteLog("UPDATE waypoint_data SET wpguid = '0'");
            //WorldDatabase.PExecuteLog("UPDATE creature_movement SET wpguid = '0' WHERE wpguid <> '0'");

            if (hasError)
            {
                handler->PSendSysMessage(LANG_WAYPOINT_TOOFAR1);
                handler->PSendSysMessage(LANG_WAYPOINT_TOOFAR2);
                handler->PSendSysMessage(LANG_WAYPOINT_TOOFAR3);
            }

            handler->SendSysMessage(LANG_WAYPOINT_VP_ALLREMOVED);
            return true;
        }

        handler->PSendSysMessage("|cffff33ffDEBUG: wpshow - no valid command found|r");
        return true;
    }

};

void AddSC_wp_commandscript()
{
    new wp_commandscript();
}