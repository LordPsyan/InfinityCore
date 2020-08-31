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
#include "ScriptPCH.h"
#include "Configuration/Config.h"
#include "Database/DatabaseEnv.h"
#include "DBCStores.h"
#include "ObjectMgr.h"
#include "ScriptLoader.h"
#include "ScriptSystem.h"
#include "SpellMgr.h"
#include "GossipDef.h"
#include "CreatureAIImpl.h"
#include "ScriptLoader.h"
#include "ScriptSystem.h"
#include "../../game/Teleport/sc_npc_teleport.h"
#ifdef ELUNA
#include "Player.h"
#include "LuaEngine.h"
#endif


INSTANTIATE_SINGLETON_1(ScriptMgr);

// Utility macros to refer to the script registry.
#define SCR_REG_MAP(T) ScriptRegistry<T>::ScriptMap
#define SCR_REG_LST(T) ScriptRegistry<T>::ScriptPointerList

// Utility macros for looping over scripts.
#define FOR_SCRIPTS(T,C,E) \
    if (SCR_REG_LST(T).empty()) \
        return; \
    for (SCR_REG_MAP(T)::iterator C = SCR_REG_LST(T).begin(); \
        C != SCR_REG_LST(T).end(); ++C)
#define FOR_SCRIPTS_RET(T,C,E,R) \
    if (SCR_REG_LST(T).empty()) \
        return R; \
    for (SCR_REG_MAP(T)::iterator C = SCR_REG_LST(T).begin(); \
        C != SCR_REG_LST(T).end(); ++C)
#define FOREACH_SCRIPT(T) \
    FOR_SCRIPTS(T, itr, end) \
    itr->second

// Utility macros for finding specific scripts.
#define GET_SCRIPT(T,I,V) \
    T* V = ScriptRegistry<T>::GetScriptById(I); \
    if (!V) \
        return;
#define GET_SCRIPT_RET(T,I,V,R) \
    T* V = ScriptRegistry<T>::GetScriptById(I); \
    if (!V) \
        return R;


//*********************************
//*** Functions used globally ***

void DoScriptText(int32 iTextEntry, WorldObject* pSource, Unit* pTarget)
{
    if (!pSource)
    {
        error_log("OSCR: DoScriptText entry %i, invalid Source pointer.", iTextEntry);
        return;
    }

    if (iTextEntry >= 0)
    {
        error_log("OSCR: DoScriptText with source entry %u (TypeId=%u, guid=%u) attempts to process text entry %i, but text entry must be negative.", pSource->GetEntry(), pSource->GetTypeId(), pSource->GetGUIDLow(), iTextEntry);
        return;
    }

    const StringTextData* pData = sScriptSystemMgr.GetTextData(iTextEntry);

    if (!pData)
    {
        error_log("OSCR: DoScriptText with source entry %u (TypeId=%u, guid=%u) could not find text entry %i.", pSource->GetEntry(), pSource->GetTypeId(), pSource->GetGUIDLow(), iTextEntry);
        return;
    }

    debug_log("OSCR: DoScriptText: text entry=%i, Sound=%u, Type=%u, Language=%u, Emote=%u", iTextEntry, pData->uiSoundId, pData->uiType, pData->uiLanguage, pData->uiEmote);

    if (pData->uiSoundId)
    {
        if (GetSoundEntriesStore()->LookupEntry(pData->uiSoundId))
            pSource->SendPlaySound(pData->uiSoundId, false);
        else
            error_log("OSCR: DoScriptText entry %i tried to process invalid sound id %u.", iTextEntry, pData->uiSoundId);
    }

    if (pData->uiEmote)
    {
        if (pSource->GetTypeId() == TYPEID_UNIT || pSource->GetTypeId() == TYPEID_PLAYER)
            ((Unit*)pSource)->HandleEmoteCommand(pData->uiEmote);
        else
            error_log("OSCR: DoScriptText entry %i tried to process emote for invalid TypeId (%u).", iTextEntry, pSource->GetTypeId());
    }

    switch (pData->uiType)
    {
    case CHAT_TYPE_SAY:
        pSource->MonsterSay(iTextEntry, pData->uiLanguage, pTarget ? pTarget->GetGUID() : 0);
        break;
    case CHAT_TYPE_YELL:
        pSource->MonsterYell(iTextEntry, pData->uiLanguage, pTarget ? pTarget->GetGUID() : 0);
        break;
    case CHAT_TYPE_TEXT_EMOTE:
        pSource->MonsterTextEmote(iTextEntry, pTarget ? pTarget->GetGUID() : 0);
        break;
    case CHAT_TYPE_BOSS_EMOTE:
        pSource->MonsterTextEmote(iTextEntry, pTarget ? pTarget->GetGUID() : 0, true);
        break;
    case CHAT_TYPE_WHISPER:
        {
            if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
                pSource->MonsterWhisper(iTextEntry, pTarget->GetGUID());
            else
                error_log("OSCR: DoScriptText entry %i cannot whisper without target unit (TYPEID_PLAYER).", iTextEntry);
        }
        break;
    case CHAT_TYPE_BOSS_WHISPER:
        {
            if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
                pSource->MonsterWhisper(iTextEntry, pTarget->GetGUID(), true);
            else
                error_log("OSCR: DoScriptText entry %i cannot whisper without target unit (TYPEID_PLAYER).", iTextEntry);
        }
        break;
    case CHAT_TYPE_ZONE_YELL:
        pSource->MonsterYellToZone(iTextEntry, pData->uiLanguage, pTarget ? pTarget->GetGUID() : 0);
        break;
    }
}

void FillSpellSummary();
void LoadOverridenSQLData();

ScriptMgr::ScriptMgr()
{
}

ScriptMgr::~ScriptMgr()
{
#define SCR_CLEAR(T) \
        FOR_SCRIPTS(T, itr, end) \
            delete itr->second; \
        SCR_REG_LST(T).clear();

    // Clear scripts for every script type.
    SCR_CLEAR(SpellHandlerScript);
    SCR_CLEAR(AuraHandlerScript);
    SCR_CLEAR(ServerScript);
    SCR_CLEAR(WorldScript);
	SCR_CLEAR(PlayerScript);
    SCR_CLEAR(GroupScript);
    SCR_CLEAR(FormulaScript);
    SCR_CLEAR(WorldMapScript);
    SCR_CLEAR(InstanceMapScript);
    SCR_CLEAR(BattlegroundMapScript);
    SCR_CLEAR(ItemScript);
    SCR_CLEAR(CreatureScript);
    SCR_CLEAR(GameObjectScript);
    SCR_CLEAR(AreaTriggerScript);
    SCR_CLEAR(BattlegroundScript);
    SCR_CLEAR(OutdoorPvPScript);
    SCR_CLEAR(CommandScript);
    SCR_CLEAR(WeatherScript);
    SCR_CLEAR(AuctionHouseScript);
    SCR_CLEAR(ConditionScript);
    SCR_CLEAR(DynamicObjectScript);
    SCR_CLEAR(TransportScript);

#undef SCR_CLEAR
}
void ScriptMgr::ScriptsInit()
{
    outstring_log("   ____                              _____           _       _   ");
    outstring_log("  / __ \\                            / ____|         (_)     | |  ");
    outstring_log(" | |  | |_ __ ___  __ _  ___  _ __ | (___   ___ _ __ _ _ __ | |_ ");
    outstring_log(" | |  | | '__/ _ \\/ _` |/ _ \\| '_ \\ \\___ \\ / __| '__| | '_ \\| __|");
    outstring_log(" | |__| | | |  __/ (_| | (_) | | | |____) | (__| |  | | |_) | |_ ");
    outstring_log("  \\____/|_|  \\___|\\__, |\\___/|_| |_|_____/ \\___|_|  |_| .__/ \\__|");
    outstring_log("                   __/ |                              | |        ");
    outstring_log("                  |___/                               |_|  \n");

    //Load database (must be called after SD2Config.SetSource).
    LoadDatabase();

    FillSpellSummary();

    AddScripts();

    outstring_log(">> Loaded %i C++ Scripts.", sScriptMgr.GetScriptCount());

    sLog.outString(">> Load Overriden SQL Data.");
    LoadOverridenSQLData();
}


struct TSpellSummary
{
    uint8 Targets;                                          // set of enum SelectTarget
    uint8 Effects;                                          // set of enum SelectEffect
} *SpellSummary;

void ScriptMgr::LoadDatabase()
{
    sScriptSystemMgr.LoadVersion();
    sScriptSystemMgr.LoadScriptTexts();
    sScriptSystemMgr.LoadScriptTextsCustom();
    sScriptSystemMgr.LoadScriptWaypoints();
}

void ScriptMgr::FillSpellSummary()
{
    SpellSummary = new TSpellSummary[GetSpellStore()->GetNumRows()];

    SpellEntry const* pTempSpell;

    for (uint32 i = 0; i < GetSpellStore()->GetNumRows(); ++i)
    {
        SpellSummary[i].Effects = 0;
        SpellSummary[i].Targets = 0;

        pTempSpell = GetSpellStore()->LookupEntry(i);
        //This spell doesn't exist
        if (!pTempSpell)
            continue;

        for (uint32 j = 0; j < 3; ++j)
        {
            //Spell targets self
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SELF - 1);

            //Spell targets a single enemy
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_ENEMY ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_DST_TARGET_ENEMY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_ENEMY - 1);

            //Spell targets AoE at enemy
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_AREA_ENEMY_SRC ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_AREA_ENEMY_DST ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_SRC_CASTER ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_DEST_DYNOBJ_ENEMY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_AOE_ENEMY - 1);

            //Spell targets an enemy
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_ENEMY ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_DST_TARGET_ENEMY ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_AREA_ENEMY_SRC ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_AREA_ENEMY_DST ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_SRC_CASTER ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_DEST_DYNOBJ_ENEMY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_ENEMY - 1);

            //Spell targets a single friend(or self)
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_CASTER ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_ALLY ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_PARTY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_FRIEND - 1);

            //Spell targets aoe friends
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_PARTY_CASTER ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_PARTY_TARGET ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_SRC_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_AOE_FRIEND - 1);

            //Spell targets any friend(or self)
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_CASTER ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_ALLY ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_TARGET_PARTY ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_PARTY_CASTER ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_UNIT_PARTY_TARGET ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_SRC_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_FRIEND - 1);

            //Make sure that this spell includes a damage effect
            if (pTempSpell->Effect[j] == SPELL_EFFECT_SCHOOL_DAMAGE ||
                pTempSpell->Effect[j] == SPELL_EFFECT_INSTAKILL ||
                pTempSpell->Effect[j] == SPELL_EFFECT_ENVIRONMENTAL_DAMAGE ||
                pTempSpell->Effect[j] == SPELL_EFFECT_HEALTH_LEECH)
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_DAMAGE - 1);

            //Make sure that this spell includes a healing effect (or an apply aura with a periodic heal)
            if (pTempSpell->Effect[j] == SPELL_EFFECT_HEAL ||
                pTempSpell->Effect[j] == SPELL_EFFECT_HEAL_MAX_HEALTH ||
                pTempSpell->Effect[j] == SPELL_EFFECT_HEAL_MECHANICAL ||
                (pTempSpell->Effect[j] == SPELL_EFFECT_APPLY_AURA && pTempSpell->EffectApplyAuraName[j] == 8))
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_HEALING - 1);

            //Make sure that this spell applies an aura
            if (pTempSpell->Effect[j] == SPELL_EFFECT_APPLY_AURA)
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_AURA - 1);
        }
    }
}

void ScriptMgr::OnNetworkStart()
{
    FOREACH_SCRIPT(ServerScript)->OnNetworkStart();
}

void ScriptMgr::OnNetworkStop()
{
    FOREACH_SCRIPT(ServerScript)->OnNetworkStop();
}

void ScriptMgr::OnSocketOpen(WorldSocket* socket)
{
    ASSERT(socket);

    FOREACH_SCRIPT(ServerScript)->OnSocketOpen(socket);
}
void ScriptMgr::OnSocketClose(WorldSocket* socket, bool wasNew)
{
    ASSERT(socket);

    FOREACH_SCRIPT(ServerScript)->OnSocketClose(socket, wasNew);
}

void ScriptMgr::OnPacketReceive(WorldSocket* socket, WorldPacket& packet)
{
    ASSERT(socket);

    FOREACH_SCRIPT(ServerScript)->OnPacketReceive(socket, packet);
}

void ScriptMgr::OnPacketSend(WorldSocket* socket, WorldPacket& packet)
{
    ASSERT(socket);

    FOREACH_SCRIPT(ServerScript)->OnPacketSend(socket, packet);
}

void ScriptMgr::OnUnknownPacketReceive(WorldSocket* socket, WorldPacket& packet)
{
    ASSERT(socket);

    FOREACH_SCRIPT(ServerScript)->OnUnknownPacketReceive(socket, packet);
}

void ScriptMgr::OnLoadCustomDatabaseTable()
{
	FOREACH_SCRIPT(WorldScript)->OnLoadCustomDatabaseTable();
}

void ScriptMgr::OnOpenStateChange(bool open)
{
    FOREACH_SCRIPT(WorldScript)->OnOpenStateChange(open);
}

void ScriptMgr::OnConfigLoad(bool reload)
{
    FOREACH_SCRIPT(WorldScript)->OnConfigLoad(reload);
}

void ScriptMgr::OnMotdChange(std::string& newMotd)
{
    FOREACH_SCRIPT(WorldScript)->OnMotdChange(newMotd);
}

void ScriptMgr::OnStartup()
{
	FOREACH_SCRIPT(WorldScript)->OnStartup();
}

void ScriptMgr::OnShutdown(ShutdownExitCode code, ShutdownMask mask)
{
    FOREACH_SCRIPT(WorldScript)->OnShutdown(code, mask);
}

void ScriptMgr::OnShutdownCancel()
{
    FOREACH_SCRIPT(WorldScript)->OnShutdownCancel();
}

void ScriptMgr::OnWorldUpdate(uint32 diff)
{
    FOREACH_SCRIPT(WorldScript)->OnUpdate(NULL, diff);
}

void ScriptMgr::OnHonorCalculation(float& honor, uint8 level, uint32 count)
{
    FOREACH_SCRIPT(FormulaScript)->OnHonorCalculation(honor, level, count);
}

void ScriptMgr::OnHonorCalculation(uint32& honor, uint8 level, uint32 count)
{
    FOREACH_SCRIPT(FormulaScript)->OnHonorCalculation(honor, level, count);
}

void ScriptMgr::OnGetGrayLevel(uint8& grayLevel, uint8 playerLevel)
{
    FOREACH_SCRIPT(FormulaScript)->OnGetGrayLevel(grayLevel, playerLevel);
}

void ScriptMgr::OnGetColorCode(XPColorChar& color, uint8 playerLevel, uint8 mobLevel)
{
    FOREACH_SCRIPT(FormulaScript)->OnGetColorCode(color, playerLevel, mobLevel);
}

void ScriptMgr::OnGetZeroDifference(uint8& diff, uint8 playerLevel)
{
    FOREACH_SCRIPT(FormulaScript)->OnGetZeroDifference(diff, playerLevel);
}

void ScriptMgr::OnGetBaseGain(uint32& gain, uint8 playerLevel, uint8 mobLevel, ContentLevels content)
{
    FOREACH_SCRIPT(FormulaScript)->OnGetBaseGain(gain, playerLevel, mobLevel, content);
}

void ScriptMgr::OnGetGain(uint32& gain, Player* player, Unit* unit)
{
    ASSERT(player);
    ASSERT(unit);

    FOREACH_SCRIPT(FormulaScript)->OnGetGain(gain, player, unit);
}

void ScriptMgr::OnGetGroupRate(float& rate, uint32 count, bool isRaid)
{
    FOREACH_SCRIPT(FormulaScript)->OnGetGroupRate(rate, count, isRaid);
}

void ScriptMgr::OnPlayerMove(Player* player, MovementInfo movementInfo, uint32 opcode)
{
    FOREACH_SCRIPT(MovementHandlerScript)->OnPlayerMove(player, movementInfo, opcode);
}

// Battleground 
void ScriptMgr::OnPlayerJoinBG(Player* player, Battleground* bg)
{
    FOREACH_SCRIPT(BGScript)->OnPlayerJoinBG(player, bg);
}

void ScriptMgr::OnPlayerLeaveBG(Player* player, Battleground* bg)
{
    FOREACH_SCRIPT(BGScript)->OnPlayerLeaveBG(player, bg);
}

void ScriptMgr::OnBGAssignTeam(Player* player, Battleground* bg, uint32& team)
{
    FOREACH_SCRIPT(BGScript)->OnBGAssignTeam(player, bg, team);
}

#define SCR_MAP_BGN(M,V,I,E,C,T) \
    if (V->GetEntry()->T()) \
    { \
        FOR_SCRIPTS(M, I, E) \
        { \
            MapEntry const* C = I->second->GetEntry(); \
            if (!C) \
                continue; \
            if (entry->MapID == V->GetId()) \
            {

#define SCR_MAP_END \
                break; \
            } \
        } \
}

void ScriptMgr::OnCreateMap(Map* map)
{
    ASSERT(map);

    SCR_MAP_BGN(WorldMapScript, map, itr, end, entry, IsContinent);
    itr->second->OnCreate(map);
    SCR_MAP_END;

    SCR_MAP_BGN(InstanceMapScript, map, itr, end, entry, IsDungeon);
    itr->second->OnCreate((InstanceMap*)map);
    SCR_MAP_END;
}

void ScriptMgr::OnDestroyMap(Map* map)
{
    ASSERT(map);
    SCR_MAP_BGN(WorldMapScript, map, itr, end, entry, IsContinent);
    itr->second->OnDestroy(map);
    SCR_MAP_END;

    SCR_MAP_BGN(InstanceMapScript, map, itr, end, entry, IsDungeon);
    itr->second->OnDestroy((InstanceMap*)map);
    SCR_MAP_END;
}

void ScriptMgr::OnLoadGridMap(Map* map, uint32 gx, uint32 gy)
{
    ASSERT(map);

    SCR_MAP_BGN(WorldMapScript, map, itr, end, entry, IsContinent);
    itr->second->OnLoadGridMap(map, gx, gy);
    SCR_MAP_END;

    SCR_MAP_BGN(InstanceMapScript, map, itr, end, entry, IsDungeon);
    itr->second->OnLoadGridMap((InstanceMap*)map, gx, gy);
    SCR_MAP_END;
}

void ScriptMgr::OnUnloadGridMap(Map* map, uint32 gx, uint32 gy)
{
    ASSERT(map);

    SCR_MAP_BGN(WorldMapScript, map, itr, end, entry, IsContinent);
    itr->second->OnUnloadGridMap(map, gx, gy);
    SCR_MAP_END;

    SCR_MAP_BGN(InstanceMapScript, map, itr, end, entry, IsDungeon);
    itr->second->OnUnloadGridMap((InstanceMap*)map, gx, gy);
    SCR_MAP_END;
}

void ScriptMgr::OnPlayerEnter(Map* map, Player* player)
{
    ASSERT(map);
    ASSERT(player);

    FOREACH_SCRIPT(PlayerScript)->OnMapChanged(player);

    SCR_MAP_BGN(WorldMapScript, map, itr, end, entry, IsContinent);
    itr->second->OnPlayerEnter(map, player);
    SCR_MAP_END;

    SCR_MAP_BGN(InstanceMapScript, map, itr, end, entry, IsDungeon);
    itr->second->OnPlayerEnter((InstanceMap*)map, player);
    SCR_MAP_END;
}

void ScriptMgr::OnPlayerLeave(Map* map, Player* player)
{
    ASSERT(map);
    ASSERT(player);

    SCR_MAP_BGN(WorldMapScript, map, itr, end, entry, IsContinent);
    itr->second->OnPlayerLeave(map, player);
    SCR_MAP_END;

    SCR_MAP_BGN(InstanceMapScript, map, itr, end, entry, IsDungeon);
    itr->second->OnPlayerLeave((InstanceMap*)map, player);
    SCR_MAP_END;

}

void ScriptMgr::OnMapUpdate(Map* map, uint32 diff)
{
    ASSERT(map);

    SCR_MAP_BGN(WorldMapScript, map, itr, end, entry, IsContinent);
    itr->second->OnUpdate(map, diff);
    SCR_MAP_END;

    SCR_MAP_BGN(InstanceMapScript, map, itr, end, entry, IsDungeon);
    itr->second->OnUpdate((InstanceMap*)map, diff);
    SCR_MAP_END;
}

#undef SCR_MAP_BGN
#undef SCR_MAP_END

InstanceData* ScriptMgr::CreateInstanceData(InstanceMap* map)
{
    ASSERT(map);
    GET_SCRIPT_RET(InstanceMapScript, map->GetScriptId(), tmpscript, NULL);
    return tmpscript->GetInstanceScript(map);
}

bool ScriptMgr::OnDummyEffect(Unit* caster, uint32 spellId, uint32 effIndex, Item* target)
{
    ASSERT(caster);
    ASSERT(target);


#ifdef ELUNA
    if (sEluna->OnDummyEffect(caster, spellId, (SpellEffIndex)effIndex, target))
        return true;
#endif

    GET_SCRIPT_RET(ItemScript, target->GetProto()->ScriptId, tmpscript, false);
    return tmpscript->OnDummyEffect(caster, spellId, effIndex, target);
}

void ScriptMgr::OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action)
{
    ASSERT(player);
    ASSERT(item);
#ifdef ELUNA
	sEluna->HandleGossipSelectOption(player, item, sender, action, "");
#endif

    GET_SCRIPT(ItemScript, item->GetProto()->ScriptId, tmpscript);
    tmpscript->OnGossipSelect(player, item, sender, action);
}

void ScriptMgr::OnGossipSelectCode(Player* player, Item* item, uint32 sender, uint32 action, const char* code)
{
    ASSERT(player);
    ASSERT(item);
#ifdef ELUNA
	sEluna->HandleGossipSelectOption(player, item, sender, action, code);
#endif

    GET_SCRIPT(ItemScript, item->GetProto()->ScriptId, tmpscript);
    tmpscript->OnGossipSelectCode(player, item, sender, action, code);
}

bool ScriptMgr::OnQuestAccept(Player* player, Item* item, Quest const* quest)
{
    ASSERT(player);
    ASSERT(quest);
    ASSERT(item);


#ifdef ELUNA
    if (sEluna->OnQuestAccept(player, item, quest))
        return true;
#endif

    GET_SCRIPT_RET(ItemScript, item->GetProto()->ScriptId, tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestAccept(player, item, quest);
}

bool ScriptMgr::OnItemUse(Player* player, Item* item, SpellCastTargets const& targets)
{
    ASSERT(player);
    ASSERT(item);

#ifdef ELUNA
    if (!sEluna->OnUse(player, item, targets))
        return true;
#endif

    GET_SCRIPT_RET(ItemScript, item->GetProto()->ScriptId, tmpscript, false);
    return tmpscript->OnUse(player, item, targets);
}

bool ScriptMgr::OnDummyEffect(Unit* caster, uint32 spellId, uint32 effIndex, Creature* target)
{
    ASSERT(caster);
    ASSERT(target);


#ifdef ELUNA
    if (sEluna->OnDummyEffect(caster, spellId, (SpellEffIndex)effIndex, target))
        return true;
#endif
    GET_SCRIPT_RET(CreatureScript, target->GetScriptId(), tmpscript, false);
    return tmpscript->OnDummyEffect(caster, spellId, effIndex, target);
}


bool ScriptMgr::OnGossipHello(Player* player, Creature* creature)
{
    ASSERT(player);
    ASSERT(creature);
#ifdef ELUNA
    if (sEluna->OnGossipHello(player, creature))
        return true;
#endif


    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnGossipHello(player, creature);
}

bool ScriptMgr::OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    ASSERT(player);
    ASSERT(creature);


#ifdef ELUNA
    if (sEluna->OnGossipSelect(player, creature, sender, action))
        return true;
#endif

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnGossipSelect(player, creature, sender, action);
}

bool ScriptMgr::OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code)
{
    ASSERT(player);
    ASSERT(creature);
    ASSERT(code);


#ifdef ELUNA
    if (sEluna->OnGossipSelectCode(player, creature, sender, action, code))
        return true;
#endif

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnGossipSelectCode(player, creature, sender, action, code);
}

bool ScriptMgr::OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
{
    ASSERT(player);
    ASSERT(creature);
    ASSERT(quest);

#ifdef ELUNA

    if (sEluna->OnQuestAccept(player, creature, quest))
        return true;
#endif


    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestAccept(player, creature, quest);
}

bool ScriptMgr::OnQuestSelect(Player* player, Creature* creature, Quest const* quest)
{
    ASSERT(player);
    ASSERT(creature);
    ASSERT(quest);

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestSelect(player, creature, quest);
}

bool ScriptMgr::OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
{
    ASSERT(player);
    ASSERT(creature);
    ASSERT(quest);

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestComplete(player, creature, quest);
}

bool ScriptMgr::OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 opt)
{
    ASSERT(player);
    ASSERT(creature);
    ASSERT(quest);

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestReward(player, creature, quest, opt);
}

uint32 ScriptMgr::GetDialogStatus(Player* player, Creature* creature)
{
    ASSERT(player);
    ASSERT(creature);


#ifdef ELUNA
    if (uint32 dialogId = sEluna->GetDialogStatus(player, creature))
        return dialogId;
#endif

    // TODO: 100 is a funny magic number to have hanging around here...
    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, 100);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnDialogStatus(player, creature);
}

CreatureAI* ScriptMgr::GetCreatureAI(Creature* creature)
{
    ASSERT(creature);

#ifdef ELUNA
    if (CreatureAI* luaAI = sEluna->GetAI(creature))
        return luaAI;
#endif


    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, NULL);
    return tmpscript->GetAI(creature);
}

void ScriptMgr::OnCreatureUpdate(Creature* creature, uint32 diff)
{
    ASSERT(creature);

    GET_SCRIPT(CreatureScript, creature->GetScriptId(), tmpscript);
    tmpscript->OnUpdate(creature, diff);
}

bool ScriptMgr::OnGossipHello(Player* player, GameObject* go)
{
    ASSERT(player);
    ASSERT(go);


#ifdef ELUNA
    if (sEluna->OnGossipHello(player, go))
        return true;

    if (sEluna->OnGameObjectUse(player, go))
        return true;
#endif


    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnGossipHello(player, go);
}

bool ScriptMgr::OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action)
{
    ASSERT(player);
    ASSERT(go);

#ifdef ELUNA
    if (sEluna->OnGossipSelect(player, go, sender, action))
        return true;
#endif


    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnGossipSelect(player, go, sender, action);
}

bool ScriptMgr::OnGossipSelectCode(Player* player, GameObject* go, uint32 sender, uint32 action, const char* code)
{
    ASSERT(player);
    ASSERT(go);
    ASSERT(code);


#ifdef ELUNA
    if (sEluna->OnGossipSelectCode(player, go, sender, action, code))
        return true;
#endif

    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnGossipSelectCode(player, go, sender, action, code);
}

bool ScriptMgr::OnQuestAccept(Player* player, GameObject* go, Quest const* quest)
{
    ASSERT(player);
    ASSERT(go);
    ASSERT(quest);

#ifdef ELUNA
    if (sEluna->OnQuestAccept(player, go, quest))
        return true;
#endif


    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestAccept(player, go, quest);
}

bool ScriptMgr::OnQuestReward(Player* player, GameObject* go, Quest const* quest, uint32 opt)
{
    ASSERT(player);
    ASSERT(go);
    ASSERT(quest);

    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestReward(player, go, quest, opt);
}


bool ScriptMgr::OnQuestComplete(Player* player, GameObject* gameobject, Quest const* quest)
{
    ASSERT(player);
    ASSERT(gameobject);
    ASSERT(quest);

    GET_SCRIPT_RET(GameObjectScript, gameobject->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestComplete(player, gameobject, quest);
}

uint32 ScriptMgr::GetDialogStatus(Player* player, GameObject* go)
{
    ASSERT(player);
    ASSERT(go);

#ifdef ELUNA
    if (uint32 dialogId = sEluna->GetDialogStatus(player, go))
        return dialogId;
#endif

    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, 100);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnDialogStatus(player, go);
}

void ScriptMgr::OnGameObjectDestroyed(Player * player, GameObject * go, uint32 eventId)
{
    ASSERT(go);

    GET_SCRIPT(GameObjectScript, go->GetScriptId(), tmpscript);
    tmpscript->OnDestroyed(player, go, eventId);
}

void ScriptMgr::OnGameObjectUpdate(GameObject* go, uint32 diff)
{
    ASSERT(go);

    GET_SCRIPT(GameObjectScript, go->GetScriptId(), tmpscript);
    tmpscript->OnUpdate(go, diff);
}


bool ScriptMgr::OnTrigger(Player* player, AreaTriggerEntry const* trigger)
{
    ASSERT(player);
    ASSERT(trigger);

#ifdef ELUNA
    if (sEluna->OnAreaTrigger(player, trigger))
        return true;
#endif


    GET_SCRIPT_RET(AreaTriggerScript, sObjectMgr.GetAreaTriggerScriptId(trigger->id), tmpscript, false);
    return tmpscript->OnTrigger(player, trigger);
}


std::vector<ChatCommand> ScriptMgr::GetChatCommands()
{
    std::vector<ChatCommand> table;

    FOR_SCRIPTS_RET(CommandScript, itr, end, table)
    {
        std::vector<ChatCommand> cmds = itr->second->GetCommands();
        table.insert(table.end(), cmds.begin(), cmds.end());
    }

    std::sort(table.begin(), table.end(), [](const ChatCommand& a, const ChatCommand&b)
    {
        return strcmp(a.Name, b.Name) < 0;
    });

    return table;
}

void ScriptMgr::OnAddPassenger(Transport* transport, Player* player)
{
    ASSERT(transport);
    ASSERT(player);

    GET_SCRIPT(TransportScript, transport->GetScriptId(), tmpscript);
    tmpscript->OnAddPassenger(transport, player);
}

void ScriptMgr::OnAddCreaturePassenger(Transport* transport, Creature* creature)
{
    ASSERT(transport);
    ASSERT(creature);

    GET_SCRIPT(TransportScript, transport->GetScriptId(), tmpscript);
    tmpscript->OnAddCreaturePassenger(transport, creature);
}

void ScriptMgr::OnRemovePassenger(Transport* transport, Player* player)
{
    ASSERT(transport);
    ASSERT(player);

    GET_SCRIPT(TransportScript, transport->GetScriptId(), tmpscript);
    tmpscript->OnRemovePassenger(transport, player);
}

void ScriptMgr::OnTransportUpdate(Transport* transport, uint32 diff)
{
    ASSERT(transport);

    GET_SCRIPT(TransportScript, transport->GetScriptId(), tmpscript);
    tmpscript->OnUpdate(transport, diff);
}

void ScriptMgr::OnRelocate(Transport* transport, uint32 waypointId, uint32 mapId, float x, float y, float z)
{
    GET_SCRIPT(TransportScript, transport->GetScriptId(), tmpscript);
    tmpscript->OnRelocate(transport, waypointId, mapId, x, y, z);
}

void ScriptMgr::OnAuctionAdd(AuctionHouseObject* ah, AuctionEntry* entry)
{
    ASSERT(ah);
    ASSERT(entry);

    FOREACH_SCRIPT(AuctionHouseScript)->OnAuctionAdd(ah, entry);
}

void ScriptMgr::OnAuctionRemove(AuctionHouseObject* ah, AuctionEntry* entry)
{
    ASSERT(ah);
    ASSERT(entry);

    FOREACH_SCRIPT(AuctionHouseScript)->OnAuctionRemove(ah, entry);
}

void ScriptMgr::OnAuctionSuccessful(AuctionHouseObject* ah, AuctionEntry* entry)
{
    ASSERT(ah);
    ASSERT(entry);

    FOREACH_SCRIPT(AuctionHouseScript)->OnAuctionSuccessful(ah, entry);
}

void ScriptMgr::OnAuctionExpire(AuctionHouseObject* ah, AuctionEntry* entry)
{
    ASSERT(ah);
    ASSERT(entry);

    FOREACH_SCRIPT(AuctionHouseScript)->OnAuctionExpire(ah, entry);
}


void ScriptMgr::OnDynamicObjectUpdate(DynamicObject* dynobj, uint32 diff)
{
    ASSERT(dynobj);
    FOR_SCRIPTS(DynamicObjectScript, itr, end)
        itr->second->OnUpdate(dynobj, diff);
}

SpellHandlerScript::SpellHandlerScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<SpellHandlerScript>::AddScript(this);
}

AuraHandlerScript::AuraHandlerScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<AuraHandlerScript>::AddScript(this);
}

ServerScript::ServerScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<ServerScript>::AddScript(this);
}

WorldScript::WorldScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<WorldScript>::AddScript(this);
}

FormulaScript::FormulaScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<FormulaScript>::AddScript(this);
}

WorldMapScript::WorldMapScript(const char* name, uint32 mapId)
    : ScriptObject(name), MapScript(mapId)
{
    if (GetEntry() && !GetEntry()->IsContinent())
        sLog.outError("WorldMapScript for map %u is invalid.", mapId);

    ScriptMgr::ScriptRegistry<WorldMapScript>::AddScript(this);
}

InstanceMapScript::InstanceMapScript(const char* name, uint32 mapId)
    : ScriptObject(name), MapScript(mapId)
{
    if (GetEntry() && !GetEntry()->IsDungeon())
        sLog.outError("InstanceMapScript for map %u is invalid.", mapId);

    ScriptMgr::ScriptRegistry<InstanceMapScript>::AddScript(this);
}

BattlegroundMapScript::BattlegroundMapScript(const char* name, uint32 mapId)
    : ScriptObject(name), MapScript(mapId)
{
    if (GetEntry() && !GetEntry()->IsBattleground())
        sLog.outError("BattlegroundMapScript for map %u is invalid.", mapId);

    ScriptMgr::ScriptRegistry<BattlegroundMapScript>::AddScript(this);
}

AreaTriggerScript::AreaTriggerScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<AreaTriggerScript>::AddScript(this);
}

ItemScript::ItemScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<ItemScript>::AddScript(this);
}

CreatureScript::CreatureScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<CreatureScript>::AddScript(this);
}

GameObjectScript::GameObjectScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<GameObjectScript>::AddScript(this);
}

BattlegroundScript::BattlegroundScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<BattlegroundScript>::AddScript(this);
}

OutdoorPvPScript::OutdoorPvPScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<OutdoorPvPScript>::AddScript(this);
}

CommandScript::CommandScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<CommandScript>::AddScript(this);
}

WeatherScript::WeatherScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<WeatherScript>::AddScript(this);
}

AuctionHouseScript::AuctionHouseScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<AuctionHouseScript>::AddScript(this);
}

ConditionScript::ConditionScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<ConditionScript>::AddScript(this);
}

DynamicObjectScript::DynamicObjectScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<DynamicObjectScript>::AddScript(this);
}

TransportScript::TransportScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<TransportScript>::AddScript(this);
}

GroupScript::GroupScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<GroupScript>::AddScript(this);
}

PlayerScript::PlayerScript(const char* name)
	: ScriptObject(name)
{
	ScriptMgr::ScriptRegistry<PlayerScript>::AddScript(this);
}

// Movement
MovementHandlerScript::MovementHandlerScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<MovementHandlerScript>::AddScript(this);
}

BGScript::BGScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<BGScript>::AddScript(this);
}

GuildScript::GuildScript(const char* name)
    : ScriptObject(name)
{
    ScriptMgr::ScriptRegistry<GuildScript>::AddScript(this);
}

// Group
void ScriptMgr::OnGroupAddMember(Group* group, Player* guid)
{
    ASSERT(group);
    FOREACH_SCRIPT(GroupScript)->OnAddMember(group, guid);
}

void ScriptMgr::OnGroupInviteMember(Group* group, Player* guid)
{
    ASSERT(group);
    FOREACH_SCRIPT(GroupScript)->OnInviteMember(group, guid);
}

void ScriptMgr::OnGroupRemoveMember(Group* group, Player* guid, RemoveMethod method, uint64 kicker, char const* reason)
{
    ASSERT(group);
    FOREACH_SCRIPT(GroupScript)->OnRemoveMember(group, guid, method, kicker, reason);
}

void ScriptMgr::OnGroupMemberJoin(Group* group, Player* player)
{
    ASSERT(group);
    FOREACH_SCRIPT(GroupScript)->OnMemberJoin(group, player);
}

void ScriptMgr::OnGroupChangeLeader(Group* group, Player* newLeader, Player* oldLeader)
{
    ASSERT(group);
    FOREACH_SCRIPT(GroupScript)->OnChangeLeader(group, newLeader, oldLeader);
}

void ScriptMgr::OnGroupCreate(Group* group, Player* leader)
{
    ASSERT(group);
    FOREACH_SCRIPT(GroupScript)->OnCreate(group, leader);
}

void ScriptMgr::OnGroupDisband(Group* group, Player* leader)
{
    ASSERT(group);
    FOREACH_SCRIPT(GroupScript)->OnDisband(group, leader);
}


// Player
void ScriptMgr::OnLootMoney(Player* player, uint32 amount)
{
    FOREACH_SCRIPT(PlayerScript)->OnLootMoney(player, amount);
}

void ScriptMgr::OnBeforePlayerUpdate(Player* player, uint32 p_time)
{
    FOREACH_SCRIPT(PlayerScript)->OnBeforeUpdate(player, p_time);
}

void ScriptMgr::OnLootItem(Player* player, Item* item, uint32 count, uint64 lootGUID)
{
    FOREACH_SCRIPT(PlayerScript)->OnLootItem(player, item, count, lootGUID);
}

void ScriptMgr::OnCreateItem(Player* player, Item* item, uint32 count)
{
    FOREACH_SCRIPT(PlayerScript)->OnCreateItem(player, item, count);
}

void ScriptMgr::OnQuestRewardItem(Player* player, Item* item, uint32 count)
{
    FOREACH_SCRIPT(PlayerScript)->OnQuestRewardItem(player, item, count);
}

void ScriptMgr::OnPlayerLoadFromDB(Player* player)
{
    FOREACH_SCRIPT(PlayerScript)->OnPlayerLoadFromDB(player);
}

void ScriptMgr::OnPVPKill(Player* killer, Player* killed)
{
    FOREACH_SCRIPT(PlayerScript)->OnPVPKill(killer, killed);
}

void ScriptMgr::OnCreatureKill(Player* killer, Creature* killed)
{
    FOREACH_SCRIPT(PlayerScript)->OnCreatureKill(killer, killed);
}

void ScriptMgr::OnPlayerKilledByCreature(Creature* killer, Player* killed)
{
    FOREACH_SCRIPT(PlayerScript)->OnPlayerKilledByCreature(killer, killed);
}

void ScriptMgr::OnPlayerLevelChanged(Player* player, uint8 oldLevel, uint8 newLevel)
{
    FOREACH_SCRIPT(PlayerScript)->OnLevelChanged(player, oldLevel, newLevel);
}

void ScriptMgr::OnPlayerFreeTalentPointsChanged(Player* player, uint32 points)
{
    FOREACH_SCRIPT(PlayerScript)->OnFreeTalentPointsChanged(player, points);
}

void ScriptMgr::OnPlayerTalentsReset(Player* player, bool noCost)
{
    FOREACH_SCRIPT(PlayerScript)->OnTalentsReset(player, noCost);
}

void ScriptMgr::OnPlayerMoneyChanged(Player* player, int32& amount)
{
    FOREACH_SCRIPT(PlayerScript)->OnMoneyChanged(player, amount);
}

void ScriptMgr::OnPlayerMoneyLimit(Player* player, int32 amount)
{
    FOREACH_SCRIPT(PlayerScript)->OnMoneyLimit(player, amount);
}

void ScriptMgr::OnGivePlayerXP(Player* player, uint32& amount, Unit* victim)
{
    FOREACH_SCRIPT(PlayerScript)->OnGiveXP(player, amount, victim);
}

void ScriptMgr::OnPlayerReputationChange(Player* player, uint32 factionID, int32& standing, bool incremental)
{
    FOREACH_SCRIPT(PlayerScript)->OnReputationChange(player, factionID, standing, incremental);
}

void ScriptMgr::OnPlayerDuelRequest(Player* target, Player* challenger)
{
    FOREACH_SCRIPT(PlayerScript)->OnDuelRequest(target, challenger);
}

void ScriptMgr::OnPlayerDuelStart(Player* player1, Player* player2)
{
    FOREACH_SCRIPT(PlayerScript)->OnDuelStart(player1, player2);
}

void ScriptMgr::OnPlayerDuelEnd(Player* winner, Player* loser, DuelCompleteType type)
{
    FOREACH_SCRIPT(PlayerScript)->OnDuelEnd(winner, loser, type);
}

void ScriptMgr::OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg)
{
    FOREACH_SCRIPT(PlayerScript)->OnChat(player, type, lang, msg);
}

void ScriptMgr::OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg, Player* receiver)
{
    FOREACH_SCRIPT(PlayerScript)->OnChat(player, type, lang, msg, receiver);
}

void ScriptMgr::OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg, Group* group)
{
    FOREACH_SCRIPT(PlayerScript)->OnChat(player, type, lang, msg, group);
}

void ScriptMgr::OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg, Guild* guild)
{
    FOREACH_SCRIPT(PlayerScript)->OnChat(player, type, lang, msg, guild);
}

void ScriptMgr::OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg, Channel* channel)
{
    FOREACH_SCRIPT(PlayerScript)->OnChat(player, type, lang, msg, channel);
}

void ScriptMgr::OnPlayerEmote(Player* player, uint32 emote)
{
    FOREACH_SCRIPT(PlayerScript)->OnEmote(player, emote);
}

void ScriptMgr::OnPlayerTextEmote(Player* player, uint32 textEmote, uint32 emoteNum, ObjectGuid guid)
{
    FOREACH_SCRIPT(PlayerScript)->OnTextEmote(player, textEmote, emoteNum, guid);
}

void ScriptMgr::OnPlayerSpellCast(Player* player, Spell* spell, bool skipCheck)
{
    FOREACH_SCRIPT(PlayerScript)->OnSpellCast(player, spell, skipCheck);
}

void ScriptMgr::OnPlayerLogin(Player* player, bool firstLogin)
{
    FOREACH_SCRIPT(PlayerScript)->OnLogin(player, firstLogin);
}

void ScriptMgr::OnPlayerCompleteQuest(Player* player, Quest const* quest)
{
    FOREACH_SCRIPT(PlayerScript)->OnPlayerCompleteQuest(player, quest);
}

void ScriptMgr::OnPlayerLogout(Player* player)
{
    FOREACH_SCRIPT(PlayerScript)->OnLogout(player);
}

void ScriptMgr::OnPlayerCreate(Player* player)
{
    FOREACH_SCRIPT(PlayerScript)->OnCreate(player);
}

void ScriptMgr::OnPlayerDelete(ObjectGuid guid, uint32 accountId)
{
    FOREACH_SCRIPT(PlayerScript)->OnDelete(guid, accountId);
}

void ScriptMgr::OnPlayerFailedDelete(ObjectGuid guid, uint32 accountId)
{
    FOREACH_SCRIPT(PlayerScript)->OnFailedDelete(guid, accountId);
}

void ScriptMgr::OnPlayerSave(Player* player)
{
    FOREACH_SCRIPT(PlayerScript)->OnSave(player);
}

void ScriptMgr::OnPlayerBindToInstance(Player* player, DungeonDifficulty difficulty, uint32 mapid, bool permanent, uint8 extendState)
{
    FOREACH_SCRIPT(PlayerScript)->OnBindToInstance(player, difficulty, mapid, permanent, extendState);
}

void ScriptMgr::OnPlayerUpdateZone(Player* player, uint32 newZone, uint32 newArea)
{
    FOREACH_SCRIPT(PlayerScript)->OnUpdateZone(player, newZone, newArea);
}

void ScriptMgr::OnQuestStatusChange(Player* player, uint32 questId)
{
    FOREACH_SCRIPT(PlayerScript)->OnQuestStatusChange(player, questId);
}

void ScriptMgr::OnGossipSelect(Player* player, uint32 menu_id, uint32 sender, uint32 action)
{
#ifdef ELUNA
	sEluna->HandleGossipSelectOption(player, menu_id, sender, action, "");
#endif
    FOREACH_SCRIPT(PlayerScript)->OnGossipSelect(player, menu_id, sender, action);
}

void ScriptMgr::OnGossipSelectCode(Player* player, uint32 menu_id, uint32 sender, uint32 action, const char* code)
{
#ifdef ELUNA
	sEluna->HandleGossipSelectOption(player, menu_id, sender, action, code);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnGossipSelectCode(player, menu_id, sender, action, code);
}

void ScriptMgr::OnPlayerRepop(Player* player)
{
    FOREACH_SCRIPT(PlayerScript)->OnPlayerRepop(player);
}

void ScriptMgr::OnQuestObjectiveProgress(Player* player, Quest const* quest, uint32 objectiveIndex, uint16 progress)
{
    FOREACH_SCRIPT(PlayerScript)->OnQuestObjectiveProgress(player, quest, objectiveIndex, progress);
}

// Guild
void ScriptMgr::OnGuildRemoveMember(Guild* guild, Player* player, bool isDisbanding)
{
    FOREACH_SCRIPT(GuildScript)->OnRemoveMember(guild, player, isDisbanding);
}

void ScriptMgr::OnGuildAddMember(Guild* guild, Player* player, uint32& plRank)
{
    FOREACH_SCRIPT(GuildScript)->OnAddMember(guild, player, plRank);
}

void ScriptMgr::OnGuildCreate(Guild* guild, Player* leader, const std::string& name)
{
    FOREACH_SCRIPT(GuildScript)->OnCreate(guild, leader, name);
}

void ScriptMgr::OnGuildDisband(Guild* guild)
{
    FOREACH_SCRIPT(GuildScript)->OnDisband(guild);
}

template<class TScript>
void ScriptMgr::ScriptRegistry<TScript>::AddScript(TScript* const script)
{
    ASSERT(script);

    // See if the script is using the same memory as another script. If this happens, it means that
    // someone forgot to allocate new memory for a script.
    typedef typename ScriptMap::iterator ScriptMapIterator;
    for (ScriptMapIterator it = ScriptPointerList.begin(); it != ScriptPointerList.end(); ++it)
    {
        if (it->second == script)
        {
            sLog.outError("Script '%s' forgot to allocate memory, so this script and/or the script before that can't work.",
                script->ToString());

            return;
        }
    }
    // Get an ID for the script. An ID only exists if it's a script that is assigned in the database
    // through a script name (or similar).
    uint32 id = GetScriptId(script->ToString());
    if (id)
    {
        // Try to find an existing script.
        bool existing = false;
        typedef typename ScriptMap::iterator ScriptMapIterator;
        for (ScriptMapIterator it = ScriptPointerList.begin(); it != ScriptPointerList.end(); ++it)
        {
            // If the script names match...
            if (it->second->GetName() == script->GetName())
            {
                // ... It exists.
                existing = true;
                break;
            }
        }

        // If the script isn't assigned -> assign it!
        if (!existing)
        {
            ScriptPointerList[id] = script;
            sScriptMgr.IncrementScriptCount();
        }
        else
        {
            // If the script is already assigned -> delete it!
            sLog.outError("Script '%s' already assigned with the same script name, so the script can't work.",
                script->ToString());

            delete script;
        }
    }
    else if (script->IsDatabaseBound())
    {
        // The script uses a script name from database, but isn't assigned to anything.
        if (script->GetName().find("example") == std::string::npos)
            sLog.outErrorDb("Script named '%s' does not have a script name assigned in database.",
                script->ToString());

        delete script;
    }
    else
    {
        // We're dealing with a code-only script; just add it.
        ScriptPointerList[_scriptIdCounter++] = script;
        sScriptMgr.IncrementScriptCount();
    }
}

// Instantiate static members of ScriptMgr::ScriptRegistry.
template<class TScript> std::map<uint32, TScript*> ScriptMgr::ScriptRegistry<TScript>::ScriptPointerList;
template<class TScript> uint32 ScriptMgr::ScriptRegistry<TScript>::_scriptIdCounter;

// Specialize for each script type class like so:
template class ScriptMgr::ScriptRegistry<SpellHandlerScript>;
template class ScriptMgr::ScriptRegistry<AuraHandlerScript>;
template class ScriptMgr::ScriptRegistry<ServerScript>;
template class ScriptMgr::ScriptRegistry<WorldScript>;
template class ScriptMgr::ScriptRegistry<PlayerScript>;
template class ScriptMgr::ScriptRegistry<GroupScript>;
template class ScriptMgr::ScriptRegistry<FormulaScript>;
template class ScriptMgr::ScriptRegistry<WorldMapScript>;
template class ScriptMgr::ScriptRegistry<InstanceMapScript>;
template class ScriptMgr::ScriptRegistry<BattlegroundMapScript>;
template class ScriptMgr::ScriptRegistry<ItemScript>;
template class ScriptMgr::ScriptRegistry<CreatureScript>;
template class ScriptMgr::ScriptRegistry<GameObjectScript>;
template class ScriptMgr::ScriptRegistry<AreaTriggerScript>;
template class ScriptMgr::ScriptRegistry<BattlegroundScript>;
template class ScriptMgr::ScriptRegistry<OutdoorPvPScript>;
template class ScriptMgr::ScriptRegistry<CommandScript>;
template class ScriptMgr::ScriptRegistry<WeatherScript>;
template class ScriptMgr::ScriptRegistry<AuctionHouseScript>;
template class ScriptMgr::ScriptRegistry<ConditionScript>;
template class ScriptMgr::ScriptRegistry<DynamicObjectScript>;
template class ScriptMgr::ScriptRegistry<TransportScript>;
template class ScriptMgr::ScriptRegistry<MovementHandlerScript>;
template class ScriptMgr::ScriptRegistry<BGScript>;
template class ScriptMgr::ScriptRegistry<GuildScript>;

// Undefine utility macros.
#undef GET_SCRIPT_RET
#undef GET_SCRIPT
#undef FOREACH_SCRIPT
#undef FOR_SCRIPTS_RET
#undef FOR_SCRIPTS
#undef SCR_REG_LST
#undef SCR_REG_MAP