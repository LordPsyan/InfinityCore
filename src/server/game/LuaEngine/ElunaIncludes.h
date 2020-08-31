/*
* Copyright (C) 2010 - 2016 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.md for more information
*/

#ifndef _ELUNA_INCLUDES_H
#define _ELUNA_INCLUDES_H

// Required
#include "AccountMgr.h"
#include "AuctionHouseMgr.h"
#include "Cell.h"
#include "CellImpl.h"
#include "Chat.h"
#include "Channel.h"
#include "DBCStores.h"
#include "GossipDef.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "Guild.h"
#include "Language.h"
#include "Mail.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Player.h"
#include "Pet.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "TemporarySummon.h"
#include "WorldPacket.h"
#include "WorldSession.h"

//#include "config.h"
#include "ScriptedCreature.h"
#include "Battleground.h"
#include "Configuration/Config.h"
#include "BattlegroundMgr.h"
#include "revision.h"

#include "ArenaTeam.h"

typedef Opcodes                 OpcodesList;

/*
 * Note: if you add or change a CORE_NAME or CORE_VERSION #define,
 *   please update LuaGlobalFunctions::GetCoreName or LuaGlobalFunctions::GetCoreVersion documentation example string.
 */
#define CORE_NAME               "OregonCore"
#define CORE_VERSION            "Unk"

#define TOTAL_LOCALES           MAX_LOCALE
#define DIALOG_STATUS_SCRIPTED_NO_STATUS    DIALOG_STATUS_UNDEFINED

#define PLAYER_FIELD_LIFETIME_HONORABLE_KILLS   PLAYER_FIELD_LIFETIME_HONORABLE_KILLS
#define SPELL_AURA_MOD_KILL_XP_PCT  SPELL_AURA_MOD_XP_PCT

typedef SpellEntry SpellInfo;

#endif // _ELUNA_INCLUDES_H
