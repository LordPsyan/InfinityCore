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
#include "CreatureEventAIMgr.h"
#include "SkillDiscovery.h"
#include "SkillExtraItems.h"
#include "TicketMgr.h"

class reload_commandscript : public CommandScript
{
public:
    reload_commandscript() : CommandScript("reload_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> reloadCommandTable =
        {
            { "all",                            SEC_ADMINISTRATOR, true,  &HandleReloadAllCommand,                     "" },
            { "all_loot",                       SEC_ADMINISTRATOR, true,  &HandleReloadAllLootCommand,                 "" },
            { "all_npc",                        SEC_ADMINISTRATOR, true,  &HandleReloadAllNpcCommand,                  "" },
            { "all_quest",                      SEC_ADMINISTRATOR, true,  &HandleReloadAllQuestCommand,                "" },
            { "all_scripts",                    SEC_ADMINISTRATOR, true,  &HandleReloadAllScriptsCommand,              "" },
            { "all_spell",                      SEC_ADMINISTRATOR, true,  &HandleReloadAllSpellCommand,                "" },
            { "all_item",                       SEC_ADMINISTRATOR, true,  &HandleReloadAllItemCommand,                 "" },
            { "all_locales",                    SEC_ADMINISTRATOR, true,  &HandleReloadAllLocalesCommand,              "" },
            { "config",                         SEC_ADMINISTRATOR, true,  &HandleReloadConfigCommand,                  "" },
            { "areatrigger_tavern",             SEC_ADMINISTRATOR, true,  &HandleReloadAreaTriggerTavernCommand,       "" },
            { "areatrigger_teleport",           SEC_ADMINISTRATOR, true,  &HandleReloadAreaTriggerTeleportCommand,     "" },
            { "access_requirement",             SEC_ADMINISTRATOR, true,  &HandleReloadAccessRequirementCommand,       "" },
            { "areatrigger_involvedrelation",   SEC_ADMINISTRATOR, true,  &HandleReloadQuestAreaTriggersCommand,       "" },
            { "autobroadcast",                  SEC_ADMINISTRATOR, true,  &HandleReloadAutobroadcastCommand,           "" },
            { "event_scripts",                  SEC_ADMINISTRATOR, true,  &HandleReloadEventScriptsCommand,            "" },
            { "command",                        SEC_ADMINISTRATOR, true,  &HandleReloadCommandCommand,                 "" },
            { "conditions",                     SEC_ADMINISTRATOR, true,  &HandleReloadConditions,                     "" },
            { "creature_ai_scripts",            SEC_ADMINISTRATOR, true,  &HandleReloadEventAIScriptsCommand,          "" },
            { "creature_ai_summons",            SEC_ADMINISTRATOR, true,  &HandleReloadEventAISummonsCommand,          "" },
            { "creature_ai_texts",              SEC_ADMINISTRATOR, true,  &HandleReloadEventAITextsCommand,            "" },
            { "creature_questender",            SEC_ADMINISTRATOR, true,  &HandleReloadCreatureQuestEnderCommand,      "" },
            { "creature_linked_respawn",        SEC_ADMINISTRATOR, true,  &HandleReloadCreatureLinkedRespawnCommand,   "" },
            { "creature_loot_template",         SEC_ADMINISTRATOR, true,  &HandleReloadLootTemplatesCreatureCommand,   "" },
            { "creature_queststarter",          SEC_ADMINISTRATOR, true,  &HandleReloadCreatureQuestStarterCommand,    "" },
            { "disables",                       SEC_ADMINISTRATOR, true,  &HandleReloadDisablesCommand,                "" },
            { "disenchant_loot_template",       SEC_ADMINISTRATOR, true,  &HandleReloadLootTemplatesDisenchantCommand, "" },
            { "fishing_loot_template",          SEC_ADMINISTRATOR, true,  &HandleReloadLootTemplatesFishingCommand,    "" },
            { "graveyard_zone",                 SEC_ADMINISTRATOR, true,  &HandleReloadGameGraveyardZoneCommand,       "" },
            { "game_tele",                      SEC_ADMINISTRATOR, true,  &HandleReloadGameTeleCommand,                "" },
            { "gameobject_questender",          SEC_ADMINISTRATOR, true,  &HandleReloadGOQuestEnderCommand,            "" },
            { "gameobject_loot_template",       SEC_ADMINISTRATOR, true,  &HandleReloadLootTemplatesGameobjectCommand, "" },
            { "gameobject_queststarter",        SEC_ADMINISTRATOR, true,  &HandleReloadGOQuestStarterCommand,          "" },
            { "gameobject_scripts",             SEC_ADMINISTRATOR, true,  &HandleReloadGameObjectScriptsCommand,       "" },
            { "gossip_menu",                    SEC_ADMINISTRATOR, true,  &HandleReloadGossipMenuCommand,              "" },
            { "gossip_menu_option",             SEC_ADMINISTRATOR, true,  &HandleReloadGossipMenuOptionCommand,        "" },
            { "item_enchantment_template",      SEC_ADMINISTRATOR, true,  &HandleReloadItemEnchantementsCommand,       "" },
            { "item_loot_template",             SEC_ADMINISTRATOR, true,  &HandleReloadLootTemplatesItemCommand,       "" },
            { "mail_loot_template",             SEC_ADMINISTRATOR, true,  &HandleReloadLootTemplatesMailCommand,       "" },
            { "oregon_string",                  SEC_ADMINISTRATOR, true,  &HandleReloadOregonStringCommand,            "" },
            { "npc_gossip",                     SEC_ADMINISTRATOR, true,  &HandleReloadNpcGossipCommand,               "" },
            { "npc_trainer",                    SEC_ADMINISTRATOR, true,  &HandleReloadNpcTrainerCommand,              "" },
            { "npc_vendor",                     SEC_ADMINISTRATOR, true,  &HandleReloadNpcVendorCommand,               "" },
            { "page_text",                      SEC_ADMINISTRATOR, true,  &HandleReloadPageTextsCommand,               "" },
            { "pickpocketing_loot_template",    SEC_ADMINISTRATOR, true,  &HandleReloadLootTemplatesPickpocketingCommand, ""},
            { "prospecting_loot_template",      SEC_ADMINISTRATOR, true,  &HandleReloadLootTemplatesProspectingCommand, "" },
            { "quest_end_scripts",              SEC_ADMINISTRATOR, true,  &HandleReloadQuestEndScriptsCommand,         "" },
            { "quest_start_scripts",            SEC_ADMINISTRATOR, true,  &HandleReloadQuestStartScriptsCommand,       "" },
            { "quest_template",                 SEC_ADMINISTRATOR, true,  &HandleReloadQuestTemplateCommand,           "" },
            { "reference_loot_template",        SEC_ADMINISTRATOR, true,  &HandleReloadLootTemplatesReferenceCommand,  "" },
            { "reserved_name",                  SEC_ADMINISTRATOR, true,  &HandleReloadReservedNameCommand,            "" },
            { "reputation_spillover_template",  SEC_ADMINISTRATOR, true,  &HandleReloadReputationSpilloverTemplateCommand, "" },
            { "skill_discovery_template",       SEC_ADMINISTRATOR, true,  &HandleReloadSkillDiscoveryTemplateCommand,  "" },
            { "skill_extra_item_template",      SEC_ADMINISTRATOR, true,  &HandleReloadSkillExtraItemTemplateCommand,  "" },
            { "skill_fishing_base_level",       SEC_ADMINISTRATOR, true,  &HandleReloadSkillFishingBaseLevelCommand,   "" },
            { "skinning_loot_template",         SEC_ADMINISTRATOR, true,  &HandleReloadLootTemplatesSkinningCommand,   "" },
            { "spell_affect",                   SEC_ADMINISTRATOR, true,  &HandleReloadSpellAffectCommand,             "" },
            { "spell_required",                 SEC_ADMINISTRATOR, true,  &HandleReloadSpellRequiredCommand,           "" },
            { "spell_groups",                   SEC_ADMINISTRATOR, true,  &HandleReloadSpellGroupsCommand,             "" },
            { "spell_group_stack_rules",        SEC_ADMINISTRATOR, true,  &HandleReloadSpellGroupStackRulesCommand,    "" },
            { "spell_learn_spell",              SEC_ADMINISTRATOR, true,  &HandleReloadSpellLearnSpellCommand,         "" },
            { "spell_linked_spell",             SEC_ADMINISTRATOR, true,  &HandleReloadSpellLinkedSpellCommand,        "" },
            { "spell_pet_auras",                SEC_ADMINISTRATOR, true,  &HandleReloadSpellPetAurasCommand,           "" },
            { "spell_proc_event",               SEC_ADMINISTRATOR, true,  &HandleReloadSpellProcEventCommand,          "" },
            { "spell_scripts",                  SEC_ADMINISTRATOR, true,  &HandleReloadSpellScriptsCommand,            "" },
            { "spell_target_position",          SEC_ADMINISTRATOR, true,  &HandleReloadSpellTargetPositionCommand,     "" },
            { "spell_threats",                  SEC_ADMINISTRATOR, true,  &HandleReloadSpellThreatsCommand,            "" },
            { "locales_creature",               SEC_ADMINISTRATOR, true,  &HandleReloadLocalesCreatureCommand,         "" },
            { "locales_gameobject",             SEC_ADMINISTRATOR, true,  &HandleReloadLocalesGameobjectCommand,       "" },
            { "locales_item",                   SEC_ADMINISTRATOR, true,  &HandleReloadLocalesItemCommand,             "" },
            { "locales_npc_text",               SEC_ADMINISTRATOR, true,  &HandleReloadLocalesNpcTextCommand,          "" },
            { "locales_page_text",              SEC_ADMINISTRATOR, true,  &HandleReloadLocalesPageTextCommand,         "" },
            { "locales_quest",                  SEC_ADMINISTRATOR, true,  &HandleReloadLocalesQuestCommand,            "" },
            { "auctions",                       SEC_ADMINISTRATOR, true,  &HandleReloadAuctionsCommand,                "" },
            { "waypoint_scripts",               SEC_ADMINISTRATOR, true,  &HandleReloadWpScriptsCommand,               "" },
            { "gm_tickets",                     SEC_MODERATOR    , true,  &HandleGMTicketReloadCommand,                "" },
            { "account_referred",               SEC_ADMINISTRATOR, true,  &HandleRAFReloadCommand,                     "" },
            { "",                               SEC_MODERATOR,     true,  &HandleReloadCommand,                        "" }
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "reload",             SEC_MODERATOR,      false, nullptr,                                                "", reloadCommandTable }
        };

        return commandTable;
    }

    static bool HandleReloadAllCommand(ChatHandler* handler, char const* /*args*/)
    {
        HandleReloadAreaTriggerTeleportCommand(handler,"");
        HandleReloadSkillFishingBaseLevelCommand(handler, "");

        HandleReloadAllAreaCommand(handler, "");
        HandleReloadAllLootCommand(handler, "");
        HandleReloadAllNpcCommand(handler, "");
        HandleReloadAllQuestCommand(handler, "");
        HandleReloadAllSpellCommand(handler, "");
        HandleReloadAllItemCommand(handler, "");
        HandleReloadAllLocalesCommand(handler, "");

        HandleReloadCommandCommand(handler, "");
        HandleReloadReservedNameCommand(handler, "");
        HandleReloadOregonStringCommand(handler, "");
        HandleReloadGameTeleCommand(handler, "");
        HandleReloadAutobroadcastCommand(handler, "");
        return true;
    }

    static bool HandleReloadAllAreaCommand(ChatHandler* handler, const char* /*args*/)
    {
        //HandleReloadQuestAreaTriggersCommand(""); -- reloaded in HandleReloadAllQuestCommand
        HandleReloadAreaTriggerTeleportCommand(handler, "");
        HandleReloadAreaTriggerTavernCommand(handler, "");
        HandleReloadGameGraveyardZoneCommand(handler, "");
        return true;
    }

    static bool HandleReloadAllLootCommand(ChatHandler* handler, char const* /*args*/)
    {
        sLog.outString("Re-Loading Loot Tables...");
        LoadLootTables();
        handler->SendGlobalGMSysMessage("DB tables *_loot_template reloaded.");
        sConditionMgr.LoadConditions(true);
        return true;
    }

    static bool HandleReloadAllNpcCommand(ChatHandler* handler, char const* /*args*/)
    {
        HandleReloadNpcGossipCommand(handler, "a");
        HandleReloadNpcTrainerCommand(handler, "a");
        HandleReloadNpcVendorCommand(handler, "a");
        return true;
    }

    static bool HandleReloadAllQuestCommand(ChatHandler* handler, char const* /*args*/)
    {
        HandleReloadQuestAreaTriggersCommand(handler, "a");
        HandleReloadQuestTemplateCommand(handler, "a");

        sLog.outString("Re-Loading Quests Relations...");
        sObjectMgr.LoadQuestStartersAndEnders();
        handler->SendGlobalGMSysMessage("DB tables *_queststarter and *_questender reloaded.");
        return true;
    }

    static bool HandleReloadAllScriptsCommand(ChatHandler* handler, char const* args)
    {
        if (sWorld.IsScriptScheduled())
        {
            handler->PSendSysMessage("DB scripts used currently, please attempt reload later.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        sLog.outString("Re-Loading Scripts...");
        HandleReloadGameObjectScriptsCommand(handler, "a");
        HandleReloadEventScriptsCommand(handler, "a");
        HandleReloadQuestEndScriptsCommand(handler, "a");
        HandleReloadQuestStartScriptsCommand(handler, "a");
        HandleReloadSpellScriptsCommand(handler, "a");
        handler->SendGlobalGMSysMessage("DB tables *_scripts reloaded.");
        HandleReloadDbScriptStringCommand(handler, "");
        HandleReloadWpScriptsCommand(handler, "a");
        return true;
    }

    static bool HandleReloadDbScriptStringCommand(ChatHandler* handler, const char* /*arg*/)
    {
        sLog.outString("Re-Loading Script strings from db_script_string...");
        sObjectMgr.LoadDbScriptStrings();
        handler->SendGlobalGMSysMessage("DB table db_script_string reloaded.");
        return true;
    }

    static bool HandleReloadAllSpellCommand(ChatHandler* handler, char const* args)
    {
        HandleReloadSkillDiscoveryTemplateCommand(handler, "a");
        HandleReloadSkillExtraItemTemplateCommand(handler, "a");
        HandleReloadSpellAffectCommand(handler, "a");
        HandleReloadSpellRequiredCommand(handler, "a");
        HandleReloadSpellLearnSpellCommand(handler, "a");
        HandleReloadSpellGroupsCommand(handler, "a");
        HandleReloadSpellGroupStackRulesCommand(handler, "a");
        HandleReloadSpellLinkedSpellCommand(handler, "a");
        HandleReloadSpellProcEventCommand(handler, "");
        HandleReloadSpellTargetPositionCommand(handler, "a");
        HandleReloadSpellThreatsCommand(handler, "a");
        HandleReloadSpellPetAurasCommand(handler, "a");
        return true;
    }

    static bool HandleReloadAllItemCommand(ChatHandler* handler, char const* args)
    {
        HandleReloadPageTextsCommand(handler, "a");
        HandleReloadItemEnchantementsCommand(handler, "a");
        return true;
    }

    static bool HandleReloadAllLocalesCommand(ChatHandler* handler, char const* args)
    {
        HandleReloadLocalesCreatureCommand(handler, "a");
        HandleReloadLocalesGameobjectCommand(handler, "a");
        HandleReloadLocalesItemCommand(handler, "a");
        HandleReloadLocalesNpcTextCommand(handler, "a");
        HandleReloadLocalesPageTextCommand(handler, "a");
        HandleReloadLocalesQuestCommand(handler, "a");
        return true;
    }

    static bool HandleReloadConfigCommand(ChatHandler* handler, char const* /*args*/)
    {
        sLog.outString("Re-Loading config settings...");
        sWorld.LoadConfigSettings(true);
        sWorld.LoadModSQLUpdates();
        sWorld.LoadModuleConfig();
        MapManager::Instance().InitializeVisibilityDistanceInfo();
        handler->SendGlobalGMSysMessage("World config settings reloaded.");
        return true;
    }

    static bool HandleReloadAreaTriggerTavernCommand(ChatHandler* handler, char const* /*args*/)
    {
        sLog.outString("Re-Loading Tavern Area Triggers...");
        sObjectMgr.LoadTavernAreaTriggers();
        handler->SendGlobalGMSysMessage("DB table areatrigger_tavern reloaded.");
        return true;
    }

    static bool HandleReloadAreaTriggerTeleportCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading AreaTrigger teleport definitions...");
        sObjectMgr.LoadAreaTriggerTeleports();
        handler->SendGlobalGMSysMessage("DB table areatrigger_teleport reloaded.");
        return true;
    }

    static bool HandleReloadAccessRequirementCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Access Requirement definitions...");
        sObjectMgr.LoadAccessRequirements();
        handler->SendGlobalGMSysMessage("DB table access_requirement reloaded.");
        return true;
    }

    static bool HandleReloadQuestAreaTriggersCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Quest Area Triggers...");
        sObjectMgr.LoadQuestAreaTriggers();
        handler->SendGlobalGMSysMessage("DB table areatrigger_involvedrelation (quest area triggers) reloaded.");
        return true;
    }

    static bool HandleReloadAutobroadcastCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Autobroadcast...");
        sWorld.LoadAutobroadcasts();
        handler->SendGlobalGMSysMessage("DB table autobroadcast reloaded.");
        return true;
    }

    static bool HandleReloadEventScriptsCommand(ChatHandler* handler, const char* arg)
    {
        if (sWorld.IsScriptScheduled())
        {
            handler->SendSysMessage("DB scripts used currently, please attempt reload later.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (*arg != 'a')
            sLog.outString("Re-Loading Scripts from event_scripts...");

        sObjectMgr.LoadEventScripts();

        if (*arg != 'a')
            handler->SendGlobalGMSysMessage("DB table event_scripts reloaded.");

        return true;
    }

    static bool HandleReloadCommandCommand(ChatHandler* handler, const char* /*args*/)
    {
        handler->SetLoadCommandTable(true);
        handler->SendGlobalGMSysMessage("DB table command will be reloaded at next chat command use.");
        return true;
    }

    static bool HandleReloadConditions(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Conditions...");
        sConditionMgr.LoadConditions(true);
        handler->SendGlobalGMSysMessage("Conditions reloaded.");
        return true;
    }

    static bool HandleReloadEventAIScriptsCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Scripts from `creature_ai_scripts`...");
        CreatureEAI_Mgr.LoadCreatureEventAI_Scripts();
        handler->SendGlobalSysMessage("DB table `creature_ai_scripts` reloaded.");
        return true;
    }

    static bool HandleReloadEventAISummonsCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Summons from `creature_ai_summons`...");
        CreatureEAI_Mgr.LoadCreatureEventAI_Summons(true);
        handler->SendGlobalSysMessage("DB table `creature_ai_summons` reloaded.");
        return true;
    }

    static bool HandleReloadEventAITextsCommand(ChatHandler* handler, const char* /*args*/)
    {

        sLog.outString("Re-Loading Texts from `creature_ai_texts`...");
        CreatureEAI_Mgr.LoadCreatureEventAI_Texts(true);
        handler->SendGlobalSysMessage("DB table `creature_ai_texts` reloaded.");
        return true;
    }

    static bool HandleReloadCreatureQuestEnderCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Loading Quests Relations... (creature_questender)");
        sObjectMgr.LoadCreatureQuestEnders();
        handler->SendGlobalGMSysMessage("DB table creature_questender reloaded.");
        return true;
    }

    static bool HandleReloadCreatureLinkedRespawnCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Loading Linked Respawns... (creature_linked_respawn)");
        sObjectMgr.LoadCreatureLinkedRespawn();
        handler->SendGlobalGMSysMessage("DB table creature_linked_respawn (creature linked respawns) reloaded.");
        return true;
    }

    static bool HandleReloadLootTemplatesCreatureCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Loot Tables... (creature_loot_template)");
        LoadLootTemplates_Creature();
        LootTemplates_Creature.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table creature_loot_template reloaded.");
        sConditionMgr.LoadConditions(true);
        return true;
    }

    static bool HandleReloadCreatureQuestStarterCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Loading Quests Relations... (creature_queststarter)");
        sObjectMgr.LoadCreatureQuestStarters();
        handler->SendGlobalGMSysMessage("DB table creature_queststarter reloaded.");
        return true;
    }

    static bool HandleReloadDisablesCommand(ChatHandler* handler, const char* /*arg*/)
    {
        sLog.outString("Re-Loading disables table...");
        sDisableMgr.LoadDisables();
        sLog.outString("Checking quest disables...");
        sDisableMgr.CheckQuestDisables();
        handler->SendGlobalGMSysMessage("DB table `disables` reloaded.");

        return true;
    }

    static bool HandleReloadLootTemplatesDisenchantCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Loot Tables... (disenchant_loot_template)");
        LoadLootTemplates_Disenchant();
        LootTemplates_Disenchant.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table disenchant_loot_template reloaded.");
        sConditionMgr.LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesFishingCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Loot Tables... (fishing_loot_template)");
        LoadLootTemplates_Fishing();
        LootTemplates_Fishing.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table fishing_loot_template reloaded.");
        sConditionMgr.LoadConditions(true);
        return true;
    }

    static bool HandleReloadGameGraveyardZoneCommand(ChatHandler* handler, const char* /*arg*/)
    {
        sLog.outString("Re-Loading Graveyard-zone links...");

        sObjectMgr.LoadGraveyardZones();

        handler->SendGlobalGMSysMessage("DB table `graveyard_zone` reloaded.");

        return true;
    }

    static bool HandleReloadGameTeleCommand(ChatHandler* handler, const char* /*arg*/)
    {
        sLog.outString("Re-Loading Game Tele coordinates...");

        sObjectMgr.LoadGameTele();

        handler->SendGlobalGMSysMessage("DB table game_tele reloaded.");

        return true;
    }

    static bool HandleReloadGOQuestEnderCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Loading Quests Relations... (gameobject_questender)");
        sObjectMgr.LoadGameobjectQuestEnders();
        handler->SendGlobalGMSysMessage("DB table gameobject_questender reloaded.");
        return true;
    }

    static bool HandleReloadLootTemplatesGameobjectCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Loot Tables... (gameobject_loot_template)");
        LoadLootTemplates_Gameobject();
        LootTemplates_Gameobject.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table gameobject_loot_template reloaded.");
        sConditionMgr.LoadConditions(true);
        return true;
    }

    static bool HandleReloadGOQuestStarterCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Loading Quests Relations... (gameobject_queststarter)");
        sObjectMgr.LoadGameobjectQuestStarters();
        handler->SendGlobalGMSysMessage("DB table gameobject_queststarter reloaded.");
        return true;
    }

    static bool HandleReloadGameObjectScriptsCommand(ChatHandler* handler, const char* arg)
    {
        if (sWorld.IsScriptScheduled())
        {
            handler->SendSysMessage("DB scripts used currently, please attempt reload later.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (*arg != 'a')
            sLog.outString("Re-Loading Scripts from gameobject_scripts...");

        sObjectMgr.LoadGameObjectScripts();

        if (*arg != 'a')
            handler->SendGlobalGMSysMessage("DB table gameobject_scripts reloaded.");

        return true;
    }

    static bool HandleReloadGossipMenuCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading `gossip_menu` Table!");
        sObjectMgr.LoadGossipMenu();
        handler->SendGlobalSysMessage("DB table `gossip_menu` reloaded.");
        sConditionMgr.LoadConditions(true);
        return true;
    }

    static bool HandleReloadGossipMenuOptionCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading `gossip_menu_option` Table!");
        sObjectMgr.LoadGossipMenuItems();
        handler->SendGlobalSysMessage("DB table `gossip_menu_option` reloaded.");
        sConditionMgr.LoadConditions(true);
        return true;
    }

    static bool HandleReloadItemEnchantementsCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Item Random Enchantments Table...");
        LoadRandomEnchantmentsTable();
        handler->SendGlobalGMSysMessage("DB table item_enchantment_template reloaded.");
        return true;
    }

    static bool HandleReloadLootTemplatesItemCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Loot Tables... (item_loot_template)");
        LoadLootTemplates_Item();
        LootTemplates_Item.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table item_loot_template reloaded.");
        sConditionMgr.LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesMailCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Loot Tables... (`mail_loot_template`)");
        LoadLootTemplates_Mail();
        LootTemplates_Mail.CheckLootRefs();
        handler->SendGlobalSysMessage("DB table `mail_loot_template` reloaded.");
        sConditionMgr.LoadConditions(true);
        return true;
    }

    static bool HandleReloadOregonStringCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading oregon_string Table!");
        sObjectMgr.LoadOregonStrings();
        handler->SendGlobalGMSysMessage("DB table oregon_string reloaded.");
        return true;
    }

    static bool HandleReloadNpcGossipCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading npc_gossip Table!");
        sObjectMgr.LoadNpcTextId();
        handler->SendGlobalGMSysMessage("DB table npc_gossip reloaded.");
        return true;
    }

    static bool HandleReloadNpcTrainerCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading npc_trainer Table!");
        sObjectMgr.LoadTrainerSpell();
        handler->SendGlobalGMSysMessage("DB table npc_trainer reloaded.");
        return true;
    }

    static bool HandleReloadNpcVendorCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading npc_vendor Table!");
        sObjectMgr.LoadVendors();
        handler->SendGlobalGMSysMessage("DB table npc_vendor reloaded.");
        return true;
    }

    static bool HandleReloadPageTextsCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Page Texts...");
        sObjectMgr.LoadPageTexts();
        handler->SendGlobalGMSysMessage("DB table page_texts reloaded.");
        return true;
    }

    static bool HandleReloadLootTemplatesPickpocketingCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Loot Tables... (pickpocketing_loot_template)");
        LoadLootTemplates_Pickpocketing();
        LootTemplates_Pickpocketing.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table pickpocketing_loot_template reloaded.");
        sConditionMgr.LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesProspectingCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Loot Tables... (prospecting_loot_template)");
        LoadLootTemplates_Prospecting();
        LootTemplates_Prospecting.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table prospecting_loot_template reloaded.");
        sConditionMgr.LoadConditions(true);
        return true;
    }

    static bool HandleReloadQuestEndScriptsCommand(ChatHandler* handler, const char* arg)
    {
        if (sWorld.IsScriptScheduled())
        {
            handler->SendSysMessage("DB scripts used currently, please attempt reload later.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (*arg != 'a')
            sLog.outString("Re-Loading Scripts from quest_end_scripts...");

        sObjectMgr.LoadQuestEndScripts();

        if (*arg != 'a')
            handler->SendGlobalGMSysMessage("DB table quest_end_scripts reloaded.");

        return true;
    }

    static bool HandleReloadQuestStartScriptsCommand(ChatHandler* handler, const char* arg)
    {
        if (sWorld.IsScriptScheduled())
        {
            handler->SendSysMessage("DB scripts used currently, please attempt reload later.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (*arg != 'a')
            sLog.outString("Re-Loading Scripts from quest_start_scripts...");

        sObjectMgr.LoadQuestStartScripts();

        if (*arg != 'a')
            handler->SendGlobalGMSysMessage("DB table quest_start_scripts reloaded.");

        return true;
    }

    static bool HandleReloadQuestTemplateCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Quest Templates...");
        sObjectMgr.LoadQuests();
        handler->SendGlobalGMSysMessage("DB table quest_template (quest definitions) reloaded.");
        return true;
    }

    static bool HandleReloadLootTemplatesReferenceCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Loot Tables... (reference_loot_template)");
        LoadLootTemplates_Reference();
        handler->SendGlobalGMSysMessage("DB table reference_loot_template reloaded.");
        sConditionMgr.LoadConditions(true);
        return true;
    }

    static bool HandleReloadReservedNameCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Loading ReservedNames... (reserved_name)");
        sObjectMgr.LoadReservedPlayersNames();
        handler->SendGlobalGMSysMessage("DB table reserved_name (player reserved names) reloaded.");
        return true;
    }

    static bool HandleReloadReputationSpilloverTemplateCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading `reputation_spillover_template` Table!");
        sObjectMgr.LoadReputationSpilloverTemplate();
        handler->SendGlobalSysMessage("DB table `reputation_spillover_template` reloaded.");
        return true;
    }

    static bool HandleReloadSkillDiscoveryTemplateCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Skill Discovery Table...");
        LoadSkillDiscoveryTable();
        handler->SendGlobalGMSysMessage("DB table skill_discovery_template (recipes discovered at crafting) reloaded.");
        return true;
    }

    static bool HandleReloadSkillExtraItemTemplateCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Skill Extra Item Table...");
        LoadSkillExtraItemTable();
        handler->SendGlobalGMSysMessage("DB table skill_extra_item_template (extra item creation when crafting) reloaded.");
        return true;
    }

    static bool HandleReloadSkillFishingBaseLevelCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Skill Fishing base level requirements...");
        sObjectMgr.LoadFishingBaseSkillLevel();
        handler->SendGlobalGMSysMessage("DB table skill_fishing_base_level (fishing base level for zone/subzone) reloaded.");
        return true;
    }

    static bool HandleReloadLootTemplatesSkinningCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Loot Tables... (skinning_loot_template)");
        LoadLootTemplates_Skinning();
        LootTemplates_Skinning.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table skinning_loot_template reloaded.");
        sConditionMgr.LoadConditions(true);
        return true;
    }

    static bool HandleReloadSpellAffectCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading SpellAffect definitions...");
        sSpellMgr.LoadSpellAffects();
        handler->SendGlobalGMSysMessage("DB table spell_affect (spell mods apply requirements) reloaded.");
        return true;
    }

    static bool HandleReloadSpellRequiredCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Spell Required Data... ");
        sSpellMgr.LoadSpellRequired();
        handler->SendGlobalGMSysMessage("DB table spell_required reloaded.");
        return true;
    }

    static bool HandleReloadSpellGroupsCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Spell Groups...");
        sSpellMgr.LoadSpellGroups();
        handler->SendGlobalGMSysMessage("DB table `spell_group` (spell elixir types) reloaded.");
        return true;
    }

    static bool HandleReloadSpellGroupStackRulesCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Spell Group Stack Rules...");
        sSpellMgr.LoadSpellGroupStackRules();
        handler->SendGlobalGMSysMessage("DB table `spell_group_stack_rules` (spell stacking definitions) reloaded.");
        return true;
    }

    static bool HandleReloadSpellLearnSpellCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Spell Learn Spells...");
        sSpellMgr.LoadSpellLearnSpells();
        handler->SendGlobalGMSysMessage("DB table spell_learn_spell reloaded.");
        return true;
    }

    static bool HandleReloadSpellLinkedSpellCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Spell Linked Spells...");
        sSpellMgr.LoadSpellLinked();
        handler->SendGlobalGMSysMessage("DB table spell_linked_spell reloaded.");
        return true;
    }

    static bool HandleReloadSpellPetAurasCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Spell pet auras...");
        sSpellMgr.LoadSpellPetAuras();
        handler->SendGlobalGMSysMessage("DB table spell_pet_auras reloaded.");
        return true;
    }

    static bool HandleReloadSpellProcEventCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Spell Proc Event conditions...");
        sSpellMgr.LoadSpellProcEvents();
        handler->SendGlobalGMSysMessage("DB table spell_proc_event (spell proc trigger requirements) reloaded.");
        return true;
    }

    static bool HandleReloadSpellScriptsCommand(ChatHandler* handler, const char* arg)
    {
        if (sWorld.IsScriptScheduled())
        {
            handler->SendSysMessage("DB scripts used currently, please attempt reload later.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (*arg != 'a')
            sLog.outString("Re-Loading Scripts from spell_scripts...");

        sObjectMgr.LoadSpellScripts();

        if (*arg != 'a')
            handler->SendGlobalGMSysMessage("DB table spell_scripts reloaded.");

        return true;
    }

    static bool HandleReloadSpellTargetPositionCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Spell target coordinates...");
        sSpellMgr.LoadSpellTargetPositions();
        handler->SendGlobalGMSysMessage("DB table spell_target_position (destination coordinates for spell targets) reloaded.");
        return true;
    }

    static bool HandleReloadSpellThreatsCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog.outString("Re-Loading Aggro Spells Definitions...");
        sSpellMgr.LoadSpellThreats();
        handler->SendGlobalGMSysMessage("DB table spell_threat (spell aggro definitions) reloaded.");
        return true;
    }

    static bool HandleReloadLocalesCreatureCommand(ChatHandler* handler, const char* /*arg*/)
    {
        sLog.outString("Re-Loading Locales Creature ...");
        sObjectMgr.LoadCreatureLocales();
        handler->SendGlobalGMSysMessage("DB table locales_creature reloaded.");
        return true;
    }

    static bool HandleReloadLocalesGameobjectCommand(ChatHandler* handler, const char* /*arg*/)
    {
        sLog.outString("Re-Loading Locales Gameobject ... ");
        sObjectMgr.LoadGameObjectLocales();
        handler->SendGlobalGMSysMessage("DB table locales_gameobject reloaded.");
        return true;
    }

    static bool HandleReloadLocalesItemCommand(ChatHandler* handler, const char* /*arg*/)
    {
        sLog.outString("Re-Loading Locales Item ... ");
        sObjectMgr.LoadItemLocales();
        handler->SendGlobalGMSysMessage("DB table locales_item reloaded.");
        return true;
    }

    static bool HandleReloadLocalesNpcTextCommand(ChatHandler* handler, const char* /*arg*/)
    {
        sLog.outString("Re-Loading Locales NPC Text ... ");
        sObjectMgr.LoadNpcTextLocales();
        handler->SendGlobalGMSysMessage("DB table locales_npc_text reloaded.");
        return true;
    }

    static bool HandleReloadLocalesPageTextCommand(ChatHandler* handler, const char* /*arg*/)
    {
        sLog.outString("Re-Loading Locales Page Text ... ");
        sObjectMgr.LoadPageTextLocales();
        handler->SendGlobalGMSysMessage("DB table locales_page_text reloaded.");
        return true;
    }

    static bool HandleReloadLocalesQuestCommand(ChatHandler* handler, const char* /*arg*/)
    {
        sLog.outString("Re-Loading Locales Quest ... ");
        sObjectMgr.LoadQuestLocales();
        handler->SendGlobalGMSysMessage("DB table locales_quest reloaded.");
        return true;
    }

    static bool HandleReloadAuctionsCommand(ChatHandler* handler, const char* /*args*/)
    {
        // Reload dynamic data tables from the database
        sLog.outString("Re-Loading Auctions...");
        sAuctionMgr->LoadAuctionItems();
        sAuctionMgr->LoadAuctions();
        handler->SendGlobalGMSysMessage("Auctions reloaded.");
        return true;
    }

    static bool HandleReloadWpScriptsCommand(ChatHandler* handler, const char* arg)
    {
        if (sWorld.IsScriptScheduled())
        {
            handler->SendSysMessage("DB scripts used currently, please attempt reload later.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (*arg != 'a')
            sLog.outString("Re-Loading Scripts from waypoint_scripts...");

        sObjectMgr.LoadWaypointScripts();

        if (*arg != 'a')
            handler->SendGlobalGMSysMessage("DB table waypoint_scripts reloaded.");

        return true;
    }

    static bool HandleGMTicketReloadCommand(ChatHandler* handler, const char* /*args*/)
    {
        ticketmgr.LoadGMTickets();
        return true;
    }

    static bool HandleRAFReloadCommand(ChatHandler* handler, const char* /*args*/)
    {
        sObjectMgr.LoadReferredFriends();
        handler->PSendSysMessage("RAF reloaded.");
        return true;
    }

    static bool HandleReloadCommand(ChatHandler* handler, const char* args)
    {
        if (*args)
        {
            handler->PSendSysMessage("Db table with name starting from '%s' not found and can't be reloaded.", args);
            handler->SetSentErrorMessage(true);
        }

        return false;
    }

};

void AddSC_reload_commandscript()
{
    new reload_commandscript();
}