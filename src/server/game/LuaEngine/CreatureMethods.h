/*
* Copyright (C) 2010 - 2016 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.md for more information
*/

#ifndef CREATUREMETHODS_H
#define CREATUREMETHODS_H

/***
 * Non-[Player] controlled [Unit]s (i.e. NPCs).
 *
 * Inherits all methods from: [Object], [WorldObject], [Unit]
 */
namespace LuaCreature
{
    /**
     * Returns `true` if the [Creature] is set to not give reputation when killed,
     *   and returns `false` otherwise.
     *
     * @return bool reputationDisabled
     */
    int IsReputationGainDisabled(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->IsReputationGainDisabled());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] can regenerate its health out-of-combat,
     *   and returns `false` otherwise.
     *
     * @return bool regeneratesHealth
     */
    int CanRegenerateHealth(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->isRegeneratingHealth());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] completes the [Quest] with the ID `questID`,
     *   and returns `false` otherwise.
     *
     * @param uint32 questID : the ID of a [Quest]
     * @return bool completesQuest
     */
    int CanCompleteQuest(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        uint32 quest_id = Eluna::CHECKVAL<uint32>(L, 2);

        Eluna::Push(L, creature->hasInvolvedQuest(quest_id));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] can be targeted for attack,
     *   and returns `false` otherwise.
     *
     * @param bool mustBeDead = false : if `true`, only returns `true` if the [Creature] is also dead. Otherwise, it must be alive.
     * @return bool targetable
     */
    int IsTargetableForAttack(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        bool mustBeDead = Eluna::CHECKVAL<bool>(L, 2, false);

        Eluna::Push(L, creature->isTargetableForAttack(mustBeDead));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] can assist `friend` in combat against `enemy`,
     *   and returns `false` otherwise.
     *
     * @param [Unit] friend : the Unit we will be assisting
     * @param [Unit] enemy : the Unit that we would attack if we assist `friend`
     * @param bool checkFaction = true : if `true`, the [Creature] must be the same faction as `friend` to assist
     * @return bool canAssist
     */
    int CanAssistTo(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Unit* u = Eluna::CHECKOBJ<Unit>(L, 2);
        Unit* enemy = Eluna::CHECKOBJ<Unit>(L, 3);
        bool checkfaction = Eluna::CHECKVAL<bool>(L, 4, true);

        Eluna::Push(L, creature->CanAssistTo(u, enemy, checkfaction));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] has searched for combat assistance already,
     *   and returns `false` otherwise.
     *
     * @return bool searchedForAssistance
     */
    int HasSearchedAssistance(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->HasSearchedAssistance());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] will give its loot to `player`,
     *   and returns `false` otherwise.
     *
     * @return bool tapped
     */
    int IsTappedBy(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Player* player = Eluna::CHECKOBJ<Player>(L, 2);

        Eluna::Push(L, creature->isTappedBy(player));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] will give its loot to a [Player] or [Group],
     *   and returns `false` otherwise.
     *
     * @return bool hasLootRecipient
     */
    int HasLootRecipient(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->hasLootRecipient());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] can start attacking nearby hostile [Unit]s,
     *   and returns `false` otherwise.
     *
     * @return bool canAggro
     */
    int CanAggro(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, !creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] can move through deep water,
     *   and returns `false` otherwise.
     *
     * @return bool canSwim
     */
    int CanSwim(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->CanSwim());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] can move on land,
     *   and returns `false` otherwise.
     *
     * @return bool canWalk
     */
    int CanWalk(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->CanWalk());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] is returning to its spawn position from combat,
     *   and returns `false` otherwise.
     *
     * @return bool inEvadeMode
     */
    int IsInEvadeMode(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->IsInEvadeMode());
        return 1;
    }

    /**
     * Returns `true` if the [Creature]'s rank is Elite or Rare Elite,
     *   and returns `false` otherwise.
     *
     * @return bool isElite
     */
    int IsElite(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->isElite());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] is a city guard,
     *   and returns `false` otherwise.
     *
     * @return bool isGuard
     */
    int IsGuard(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->IsGuard());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] is a civilian,
     *   and returns `false` otherwise.
     *
     * @return bool isCivilian
     */
    int IsCivilian(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->IsCivilian());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] is the leader of a player faction,
     *   and returns `false` otherwise.
     *
     * @return bool isLeader
     */
    int IsRacialLeader(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->IsRacialLeader());
        return 1;
    }

    /**
     * Returns `true` if the [Creature]'s rank is Boss,
     *   and returns `false` otherwise.
     *
     * @return bool isWorldBoss
     */
    int IsWorldBoss(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->isWorldBoss());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] cannot cast `spellId` due to a category cooldown,
     *   and returns `false` otherwise.
     *
     * @param uint32 spellId : the ID of a [Spell]
     * @return bool hasCooldown
     */
    int HasCategoryCooldown(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        uint32 spell = Eluna::CHECKVAL<uint32>(L, 2);

        Eluna::Push(L, creature->HasCategoryCooldown(spell));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] can cast `spellId` when mind-controlled,
     *   and returns `false` otherwise.
     *
     * @param uint32 spellId : the ID of a [Spell]
     * @return bool hasSpell
     */
    int HasSpell(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        uint32 id = Eluna::CHECKVAL<uint32>(L, 2);

        Eluna::Push(L, creature->HasSpell(id));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] starts the [Quest] `questId`,
     *   and returns `false` otherwise.
     *
     * @param uint32 questId : the ID of a [Quest]
     * @return bool hasQuest
     */
    int HasQuest(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        uint32 questId = Eluna::CHECKVAL<uint32>(L, 2);

        Eluna::Push(L, creature->hasQuest(questId));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] has `spellId` on cooldown,
     *   and returns `false` otherwise.
     *
     * @param uint32 spellId : the ID of a [Spell]
     * @return bool hasCooldown
     */
    int HasSpellCooldown(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        uint32 spellId = Eluna::CHECKVAL<uint32>(L, 2);

        Eluna::Push(L, creature->HasSpellCooldown(spellId));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] can fly,
     *   and returns `false` otherwise.
     *
     * @return bool canFly
     */
    int CanFly(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->CanFly());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] is an invisible trigger,
     *   and returns `false` otherwise.
     *
     * @return bool canFly
     */
    int IsTrigger(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->IsTrigger());
        return 1;
    }

    /**
     * Returns true if the [Creature] is damaged enough for looting
     *
     * @return bool isDamagedEnough
     */
    int IsDamageEnoughForLootingAndReward(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->IsDamageEnoughForLootingAndReward());
        return 1;
    }

    /**
     * Returns true if the [Creature] can start attacking specified target
     *
     * Does not work on most targets
     *
     * @param [Unit] target
     * @param bool force = true : force [Creature] to attack
     */
    int CanStartAttack(Eluna* /*E*/, lua_State* L, Creature* creature) // TODO: Implement core side
    {
        Unit* target = Eluna::CHECKOBJ<Unit>(L, 2);
        bool force = Eluna::CHECKVAL<bool>(L, 3, true);

        Eluna::Push(L, creature->canStartAttack(target, force));
        return 1;
    }

    /**
     * Returns the time it takes for this [Creature] to respawn once killed.
     *
     * This value does not usually change over a [Creature]'s lifespan,
     *   but can be modified by [Creature:SetRespawnDelay].
     *
     * @return uint32 respawnDelay : the respawn delay, in seconds
     */
    int GetRespawnDelay(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->GetRespawnDelay());
        return 1;
    }

    /**
     * Returns the radius the [Creature] is permitted to wander from its
     *   respawn point.
     *
     * @return float wanderRadius
     */
    int GetWanderRadius(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->GetRespawnRadius());
        return 1;
    }

    /**
     * Returns the current waypoint path ID of the [Creature].
     *
     * @return uint32 pathId
     */
    int GetWaypointPath(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->GetWaypointPath());
        return 1;
    }

    /**
     * Returns the current waypoint ID of the [Creature].
     *
     * @return uint32 wpId
     */
    int GetCurrentWaypointId(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->GetCurrentWaypointID());
        return 1;
    }

    /**
     * Returns the default movement type for this [Creature].
     *
     * @return [MovementGeneratorType] defaultMovementType
     */
    int GetDefaultMovementType(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->GetDefaultMovementType());
        return 1;
    }

    /**
     * Returns the effective aggro range of the [Creature] for `target`.
     *
     * If this is smaller than the minimum aggro range set in the config file,
     *   that is used as the aggro range instead.
     *
     * @param [Unit] target
     * @return float attackDistance
     */
    int GetAttackDistance(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Unit* target = Eluna::CHECKOBJ<Unit>(L, 2);

        Eluna::Push(L, creature->GetAttackDistance(target));
        return 1;
    }

    /**
     * Returns the [Group] that can loot this [Creature].
     *
     * @return [Group] lootRecipientGroup : the group or `nil`
     */
    int GetLootRecipientGroup(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->GetLootRecipientGroup());
        return 1;
    }

    /**
     * Returns the [Player] that can loot this [Creature].
     *
     * @return [Player] lootRecipient : the player or `nil`
     */
    int GetLootRecipient(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->GetLootRecipient());
        return 1;
    }

    /**
     * Returns the [Creature]'s script name.
     *
     * This is used by the core to apply C++ scripts to the Creature.
     *
     * It is not used by Eluna. Eluna will override AI scripts.
     *
     * @return string scriptName
     */
    int GetScriptName(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->GetScriptName());
        return 1;
    }

    /**
     * Returns the [Creature]'s AI name.
     *
     * This is used by the core to assign the Creature's default AI.
     *
     * If the Creature is scripted by Eluna, the AI is overriden.
     *
     * @return string AIName
     */
    int GetAIName(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->GetAIName());
        return 1;
    }

    /**
     * Returns the [Creature]'s script ID.
     *
     * Every C++ script name is assigned a unique ID by the core.
     *   This returns the ID for this [Creature]'s script name.
     *
     * @return uint32 scriptID
     */
    int GetScriptId(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->GetScriptId());
        return 1;
    }

    /**
     * Returns the [Creature]'s cooldown for `spellID`.
     *
     * @param uint32 spellID
     * @return uint32 cooldown : the cooldown, in milliseconds
     */
    int GetCreatureSpellCooldownDelay(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        uint32 spell = Eluna::CHECKVAL<uint32>(L, 2);

        Eluna::Push(L, creature->GetCreatureSpellCooldownDelay(spell));
        return 1;
    }

    /**
     * Returns the delay between when the [Creature] dies and when its body despawns.
     *
     * @return uint32 corpseDelay : the delay, in seconds
     */
    int GetCorpseDelay(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->GetCorpseDelay());
        return 1;
    }

    /**
     * Returns position the [Creature] returns to when evading from combat
     *   or respawning.
     *
     * @return float x
     * @return float y
     * @return float z
     * @return float o
     */
    int GetHomePosition(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        float x, y, z, o;
        creature->GetHomePosition(x, y, z, o);

        Eluna::Push(L, x);
        Eluna::Push(L, y);
        Eluna::Push(L, z);
        Eluna::Push(L, o);
        return 4;
    }

    /**
     * Sets the position the [Creature] returns to when evading from combat
     *   or respawning.
     *
     * @param float x
     * @param float y
     * @param float z
     * @param float o
     */
    int SetHomePosition(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        float x = Eluna::CHECKVAL<float>(L, 2);
        float y = Eluna::CHECKVAL<float>(L, 3);
        float z = Eluna::CHECKVAL<float>(L, 4);
        float o = Eluna::CHECKVAL<float>(L, 5);

        creature->SetHomePosition(x, y, z, o);

        return 0;
    }

    /**
     * Returns a target from the [Creature]'s threat list based on the
     *   supplied arguments.
     *
     *     enum SelectAggroTarget
     *     {
     *         SELECT_TARGET_RANDOM = 0,  //Just selects a random target
     *         SELECT_TARGET_TOPAGGRO,    //Selects targets from top aggro to bottom
     *         SELECT_TARGET_BOTTOMAGGRO, //Selects targets from bottom aggro to top
     *         SELECT_TARGET_NEAREST,
     *         SELECT_TARGET_FARTHEST
     *     };
     *
     * For example, if you wanted to select the third-farthest [Player]
     *   within 50 yards that has the [Aura] "Corrupted Blood" (ID 24328),
     *   you could use this function like so:
     *
     *     target = creature:GetAITarget(4, true, 3, 50, 24328)
     *
     * @param [SelectAggroTarget] targetType : how the threat list should be sorted
     * @param bool playerOnly = false : if `true`, skips targets that aren't [Player]s
     * @param uint32 position = 0 : used as an offset into the threat list. If `targetType` is random, used as the number of players from top of aggro to choose from
     * @param float distance = 0.0 : if positive, the maximum distance for the target. If negative, the minimum distance
     * @param int32 aura = 0 : if positive, the target must have this [Aura]. If negative, the the target must not have this Aura
     * @return [Unit] target : the target, or `nil`
     */
    int GetAITarget(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        uint32 targetType = Eluna::CHECKVAL<uint32>(L, 2);
        bool playerOnly = Eluna::CHECKVAL<bool>(L, 3, false);
        uint32 position = Eluna::CHECKVAL<uint32>(L, 4, 0);
        float dist = Eluna::CHECKVAL<float>(L, 5, 0.0f);
        int32 aura = Eluna::CHECKVAL<int32>(L, 6, 0);

        ThreatContainer::StorageType const& threatlist = creature->getThreatManager().getThreatList();
        if (threatlist.empty())
            return 1;
        if (position >= threatlist.size())
            return 1;

        std::list<Unit*> targetList;
        for (ThreatContainer::StorageType::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
        {
            Unit* target = (*itr)->getTarget();
            if (!target)
                continue;
            if (playerOnly && target->GetTypeId() != TYPEID_PLAYER)
                continue;
            if (aura > 0 && !target->HasAura(aura))
                continue;
            else if (aura < 0 && target->HasAura(-aura))
                continue;
            if (dist > 0.0f && !creature->IsWithinDist(target, dist))
                continue;
            else if (dist < 0.0f && creature->IsWithinDist(target, -dist))
                continue;
            targetList.push_back(target);
        }

        if (targetList.empty())
            return 1;
        if (position >= targetList.size())
            return 1;

        if (targetType == SELECT_TARGET_NEAREST || targetType == SELECT_TARGET_FARTHEST)
            targetList.sort(ElunaUtil::ObjectDistanceOrderPred(creature));

        switch (targetType)
        {
            case SELECT_TARGET_NEAREST:
            case SELECT_TARGET_TOPAGGRO:
            {
                std::list<Unit*>::const_iterator itr = targetList.begin();
                if (position)
                    std::advance(itr, position);
                Eluna::Push(L, *itr);
            }
            break;
            case SELECT_TARGET_FARTHEST:
            case SELECT_TARGET_BOTTOMAGGRO:
            {
                std::list<Unit*>::reverse_iterator ritr = targetList.rbegin();
                if (position)
                    std::advance(ritr, position);
                Eluna::Push(L, *ritr);
            }
            break;
            case SELECT_TARGET_RANDOM:
            {
                std::list<Unit*>::const_iterator itr = targetList.begin();
                if (position)
                    std::advance(itr, urand(0, position));
                else
                    std::advance(itr, urand(0, targetList.size() - 1));
                Eluna::Push(L, *itr);
            }
            break;
            default:
                luaL_argerror(L, 2, "SelectAggroTarget expected");
                break;
        }

        return 1;
    }

    /**
     * Returns all [Unit]s in the [Creature]'s threat list.
     *
     * @return table targets
     */
    int GetAITargets(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        lua_newtable(L);
        int tbl = lua_gettop(L);
        uint32 i = 0;

        ThreatContainer::StorageType const& threatlist = creature->getThreatManager().getThreatList();
        if (threatlist.empty())
            return 1;
        for (ThreatContainer::StorageType::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
        {
            Unit* target = (*itr)->getTarget();
            if (!target)
                continue;
            ++i;
            Eluna::Push(L, i);
            Eluna::Push(L, target);
            lua_settable(L, tbl);
        }

        lua_settop(L, tbl);
        return 1;
    }

    /**
     * Returns the number of [Unit]s in this [Creature]'s threat list.
     *
     * @return int targetsCount
     */
    int GetAITargetsCount(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->getThreatManager().getThreatList().size());
        return 1;
    }

    /**
     * Returns the [Creature]'s NPC flags.
     *
     * These are used to control whether the NPC is a vendor, can repair items,
     *   can give quests, etc.
     *
     * @return [NPCFlags] npcFlags
     */
    int GetNPCFlags(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->GetUInt32Value(UNIT_NPC_FLAGS));
        return 1;
    }

    /**
     * Returns the [Creature]'s shield block value.
     *
     * @return uint32 shieldBlockValue
     */
    int GetShieldBlockValue(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->GetShieldBlockValue());
        return 1;
    }

    /**
     * Returns the guid of the [Creature] that is used as the ID in the database
     *
     * @return uint32 dbguid
     */
    int GetDBTableGUIDLow(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->GetDBTableGUIDLow());
        return 1;
    }

    /**
     * Sets the [Creature]'s NPC flags to `flags`.
     *
     * @param [NPCFlags] flags
     */
    int SetNPCFlags(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        uint32 flags = Eluna::CHECKVAL<uint32>(L, 2);

        creature->SetUInt32Value(UNIT_NPC_FLAGS, flags);
        return 0;
    }


    /**
     * Makes the [Creature] able to fly if enabled.
     *
     * @param bool enable = true
     */
    int SetDisableGravity(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        bool enable = Eluna::CHECKVAL<bool>(L, 2, true);

        creature->SetLevitate(enable);
        return 0;
    }

    /**
     * Sets the [Creature]'s death state to `deathState`.
     *
     * @param [DeathState] deathState
     */
    int SetDeathState(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        int32 state = Eluna::CHECKVAL<int32>(L, 2);

        creature->setDeathState((DeathState)state);
        return 0;
    }

    /**
     * Sets whether the [Creature] is currently walking or running.
     *
     * @param bool enable = true : `true` to enable walking, `false` for running
     */
    int SetWalk(Eluna* /*E*/, lua_State* L, Creature* creature)           // TODO: Move same to Player ?
    {
        bool enable = Eluna::CHECKVAL<bool>(L, 2, true);

        creature->SetWalk(enable);
        return 0;
    }

    /**
     * Equips given [Item]s to the [Unit]. Using 0 removes the equipped [Item]
     *
     * @param uint32 main_hand : main hand [Item]'s entry
     * @param uint32 off_hand : off hand [Item]'s entry
     * @param uint32 ranged : ranged [Item]'s entry
     */
    int SetEquipmentSlots(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        uint32 main_hand = Eluna::CHECKVAL<uint32>(L, 2);
        uint32 off_hand = Eluna::CHECKVAL<uint32>(L, 3);
        uint32 ranged = Eluna::CHECKVAL<uint32>(L, 4);

        creature->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, main_hand);
        creature->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, off_hand);
        creature->SetVirtualItem(VIRTUAL_ITEM_SLOT_2, ranged);
        return 0;
    }

    /**
     * Sets whether the [Creature] can be aggroed.
     *
     * @param bool allow = true : `true` to allow aggro, `false` to disable aggro
     */
    int SetAggroEnabled(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        bool allow = Eluna::CHECKVAL<bool>(L, 2, true);

        if (allow)
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        else
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);

        return 0;
    }

    /**
     * Sets whether the [Creature] gives reputation or not.
     *
     * @param bool disable = true : `true` to disable reputation, `false` to enable
     */
    int SetDisableReputationGain(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        bool disable = Eluna::CHECKVAL<bool>(L, 2, true);

        creature->SetDisableReputationGain(disable);
        return 0;
    }

    /**
     * Sets the [Creature] as in combat with all [Player]s in the dungeon instance.
     *
     * This is used by raid bosses to prevent Players from using out-of-combat
     *   actions once the encounter has begun.
     */
    int SetInCombatWithZone(Eluna* /*E*/, lua_State* /*L*/, Creature* creature)
    {
        creature->SetInCombatWithZone();
        return 0;
    }

    /**
     * Sets the distance the [Creature] can wander from it's spawn point.
     *
     * @param float distance
     */
    int SetWanderRadius(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        float dist = Eluna::CHECKVAL<float>(L, 2);

        creature->SetRespawnRadius(dist);
        return 0;
    }

    /**
     * Sets the time it takes for the [Creature] to respawn when killed.
     *
     * @param uint32 delay : the delay, in seconds
     */
    int SetRespawnDelay(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        uint32 delay = Eluna::CHECKVAL<uint32>(L, 2);

        creature->SetRespawnDelay(delay);
        return 0;
    }

    /**
     * Sets the default movement type of the [Creature].
     *
     * @param [MovementGeneratorType] type
     */
    int SetDefaultMovementType(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        int32 type = Eluna::CHECKVAL<int32>(L, 2);

        creature->SetDefaultMovementType((MovementGeneratorType)type);
        return 0;
    }

    /**
     * Sets whether the [Creature] can search for assistance at low health or not.
     *
     * @param bool enable = true : `true` to disable searching, `false` to allow
     */
    int SetNoSearchAssistance(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        bool val = Eluna::CHECKVAL<bool>(L, 2, true);

        creature->SetNoSearchAssistance(val);
        return 0;
    }

    /**
     * Sets whether the [Creature] can call nearby enemies for help in combat or not.
     *
     * @param bool enable = true : `true` to disable calling for help, `false` to enable
     */
    int SetNoCallAssistance(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        bool val = Eluna::CHECKVAL<bool>(L, 2, true);

        creature->SetNoCallAssistance(val);
        return 0;
    }

    /**
     * Sets whether the creature is hovering / levitating or not.
     *
     * @param bool enable = true : `true` to enable hovering, `false` to disable
     */
    int SetHover(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        bool enable = Eluna::CHECKVAL<bool>(L, 2, true);

        creature->SetHover(enable);
        return 0;
    }

    /**
     * Despawn this [Creature].
     *
     * @param uint32 delay = 0 : dely to despawn in milliseconds
     */
    int DespawnOrUnsummon(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        uint32 msTimeToDespawn = Eluna::CHECKVAL<uint32>(L, 2, 0);

        creature->DespawnOrUnsummon(msTimeToDespawn);
        return 0;
    }

    /**
     * Respawn this [Creature].
     */
    int Respawn(Eluna* /*E*/, lua_State* /*L*/, Creature* creature)
    {
        creature->Respawn();
        return 0;
    }

    /**
     * Remove this [Creature]'s corpse.
     */
    int RemoveCorpse(Eluna* /*E*/, lua_State* /*L*/, Creature* creature)
    {
        creature->RemoveCorpse();
        return 0;
    }

    /**
     * Make the [Creature] start following its waypoint path.
     */
    int MoveWaypoint(Eluna* /*E*/, lua_State* /*L*/, Creature* creature)
    {
        creature->GetMotionMaster()->MovePath(creature->GetWaypointPath(), true);
        return 0;
    }

    /**
     * Make the [Creature] call for assistance in combat from other nearby [Creature]s.
     */
    int CallAssistance(Eluna* /*E*/, lua_State* /*L*/, Creature* creature)
    {
        creature->CallAssistance();
        return 0;
    }

    /**
     * Make the [Creature] call for help in combat from friendly [Creature]s within `radius`.
     *
     * @param float radius
     */
    int CallForHelp(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        float radius = Eluna::CHECKVAL<float>(L, 2);

        creature->CallForHelp(radius);
        return 0;
    }

    /**
     * Make the [Creature] flee combat to get assistance from a nearby friendly [Creature].
     */
    int FleeToGetAssistance(Eluna* /*E*/, lua_State* /*L*/, Creature* creature)
    {
        creature->DoFleeToGetAssistance();
        return 0;
    }

    /**
     * Make the [Creature] attack `target`.
     *
     * @param [Unit] target
     */
    int AttackStart(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Unit* target = Eluna::CHECKOBJ<Unit>(L, 2);

        creature->AI()->AttackStart(target);
        return 0;
    }

    /**
     * Save the [Creature] in the database.
     */
    int SaveToDB(Eluna* /*E*/, lua_State* /*L*/, Creature* creature)
    {
        creature->SaveToDB();
        return 0;
    }

    /**
     * Make the [Creature] try to find a new target.
     *
     * This should be called every update cycle for the Creature's AI.
     */
    int SelectVictim(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        Eluna::Push(L, creature->SelectVictim());
        return 1;
    }

    /**
     * Transform the [Creature] into another Creature.
     *
     * @param uint32 entry : the Creature ID to transform into
     * @param uint32 dataGUIDLow = 0 : use this Creature's model and equipment instead of the defaults
     */
    int UpdateEntry(Eluna* /*E*/, lua_State* L, Creature* creature)
    {
        uint32 entry = Eluna::CHECKVAL<uint32>(L, 2);
        uint32 dataGuidLow = Eluna::CHECKVAL<uint32>(L, 3, 0);

        creature->UpdateEntry(entry, ALLIANCE, dataGuidLow ? sObjectMgr.GetCreatureData(dataGuidLow) : NULL);
        return 0;
    }
};

#endif
