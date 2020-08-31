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

/* ScriptData
SDName: The_Barrens
SD%Complete: 90
SDComment: Quest support: 863, 1719, 2458, 4921, 6981, 898
SDCategory: Barrens
EndScriptData */

/* ContentData
npc_beaten_corpse
npc_gilthares
npc_sputtervalve
npc_taskmaster_fizzule
npc_twiggy_flathead
npc_wizzlecrank_shredder
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"

/*######
## npc_beaten_corpse
######*/

#define GOSSIP_CORPSE "Examine corpse in detail..."

enum eQuests
{
    QUEST_LOST_IN_BATTLE    = 4921
};





/*######
# npc_gilthares
######*/

enum eGilthares
{
    SAY_GIL_START               = -1000370,
    SAY_GIL_AT_LAST             = -1000371,
    SAY_GIL_PROCEED             = -1000372,
    SAY_GIL_FREEBOOTERS         = -1000373,
    SAY_GIL_AGGRO_1             = -1000374,
    SAY_GIL_AGGRO_2             = -1000375,
    SAY_GIL_AGGRO_3             = -1000376,
    SAY_GIL_AGGRO_4             = -1000377,
    SAY_GIL_ALMOST              = -1000378,
    SAY_GIL_SWEET               = -1000379,
    SAY_GIL_FREED               = -1000380,

    QUEST_FREE_FROM_HOLD        = 898,
    AREA_MERCHANT_COAST         = 391,
    FACTION_ESCORTEE            = 232                       //guessed, possible not needed for this quest
};







/*######
## npc_sputtervalve
######*/

#define GOSSIP_SPUTTERVALVE "Can you tell me about this shard?"





/*######
## npc_taskmaster_fizzule
######*/

//#define FACTION_HOSTILE_F     430
#define FACTION_HOSTILE_F       16
#define FACTION_FRIENDLY_F      35

#define SPELL_FLARE             10113
#define SPELL_FOLLY             10137





/*#####
## npc_twiggy_flathead
#####*/

enum eTwiggyFlathead
{
    NPC_BIG_WILL                = 6238,
    NPC_AFFRAY_CHALLENGER       = 6240,

    SAY_BIG_WILL_READY          = -1000123,
    SAY_TWIGGY_FLATHEAD_BEGIN   = -1000124,
    SAY_TWIGGY_FLATHEAD_FRAY    = -1000125,
    SAY_TWIGGY_FLATHEAD_DOWN    = -1000126,
    SAY_TWIGGY_FLATHEAD_OVER    = -1000127,
};

Position const AffrayChallengerLoc[6] =
{
    {-1683.0f, -4326.0f, 2.79f, 0.0f},
    {-1682.0f, -4329.0f, 2.79f, 0.0f},
    {-1683.0f, -4330.0f, 2.79f, 0.0f},
    {-1680.0f, -4334.0f, 2.79f, 1.49f},
    {-1674.0f, -4326.0f, 2.79f, 3.49f},
    {-1677.0f, -4334.0f, 2.79f, 1.66f}
};





/*#####
## npc_wizzlecrank_shredder
#####*/

enum eEnums_Wizzlecrank
{
    SAY_START           = -1000298,
    SAY_STARTUP1        = -1000299,
    SAY_STARTUP2        = -1000300,
    SAY_MERCENARY       = -1000301,
    SAY_PROGRESS_1      = -1000302,
    SAY_PROGRESS_2      = -1000303,
    SAY_PROGRESS_3      = -1000304,
    SAY_END             = -1000305,

    QUEST_ESCAPE        = 863,
    FACTION_RATCHET     = 637,
    NPC_PILOT_WIZZ      = 3451,
    NPC_MERCENARY       = 3282,
};








class npc_beaten_corpse : public CreatureScript
{
public: 
    npc_beaten_corpse() : CreatureScript("npc_beaten_corpse") { }
    

    

    bool OnGossipHello(Player* pPlayer, Creature* pCreature) override
    {
        if (pPlayer->GetQuestStatus(QUEST_LOST_IN_BATTLE) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(QUEST_LOST_IN_BATTLE) == QUEST_STATUS_COMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_CORPSE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    
        pPlayer->SEND_GOSSIP_MENU(3557, pCreature->GetGUID());
        return true;
    }
    
    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction) override
    {
        if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        {
            pPlayer->SEND_GOSSIP_MENU(3558, pCreature->GetGUID());
            pPlayer->TalkedToCreature(pCreature->GetEntry(), pCreature->GetGUID());
        }
        return true;
    }
    

    

    
};

class npc_gilthares : public CreatureScript
{
public: 
    npc_gilthares() : CreatureScript("npc_gilthares") { }
    struct npc_giltharesAI : public npc_escortAI
    {
        npc_giltharesAI(Creature* pCreature) : npc_escortAI(pCreature) { }
    
        void Reset() { }
    
        void WaypointReached(uint32 uiPointId)
        {
            Player* pPlayer = GetPlayerForEscort();
    
            if (!pPlayer)
                return;
    
            switch (uiPointId)
            {
            case 16:
                DoScriptText(SAY_GIL_AT_LAST, me, pPlayer);
                break;
            case 17:
                DoScriptText(SAY_GIL_PROCEED, me, pPlayer);
                break;
            case 18:
                DoScriptText(SAY_GIL_FREEBOOTERS, me, pPlayer);
                break;
            case 37:
                DoScriptText(SAY_GIL_ALMOST, me, pPlayer);
                break;
            case 47:
                DoScriptText(SAY_GIL_SWEET, me, pPlayer);
                break;
            case 53:
                DoScriptText(SAY_GIL_FREED, me, pPlayer);
                pPlayer->GroupEventHappens(QUEST_FREE_FROM_HOLD, me);
                break;
            }
        }
    
        void EnterCombat(Unit* pWho)
        {
            //not always use
            if (rand() % 4)
                return;
    
            //only aggro text if not player and only in this area
            if (pWho->GetTypeId() != TYPEID_PLAYER && me->GetAreaId() == AREA_MERCHANT_COAST)
            {
                //appears to be pretty much random (possible only if escorter not in combat with pWho yet?)
                DoScriptText(RAND(SAY_GIL_AGGRO_1, SAY_GIL_AGGRO_2, SAY_GIL_AGGRO_3, SAY_GIL_AGGRO_4), me, pWho);
            }
        }
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_giltharesAI(pCreature);
    }

    bool OnQuestAccept(Player* pPlayer, Creature* pCreature, const Quest* pQuest) override
    {
        if (pQuest->GetQuestId() == QUEST_FREE_FROM_HOLD)
        {
            pCreature->SetFaction(FACTION_ESCORTEE);
            pCreature->SetStandState(UNIT_STAND_STATE_STAND);
    
            DoScriptText(SAY_GIL_START, pCreature, pPlayer);
    
            if (npc_giltharesAI* pEscortAI = CAST_AI(npc_giltharesAI, pCreature->AI()))
                pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest);
        }
        return true;
    }
    

    

    
};

class npc_sputtervalve : public CreatureScript
{
public: 
    npc_sputtervalve() : CreatureScript("npc_sputtervalve") { }
    

    

    bool OnGossipHello(Player* pPlayer, Creature* pCreature) override
    {
        if (pCreature->IsQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());
    
        if (pPlayer->GetQuestStatus(6981) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SPUTTERVALVE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
    
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
        return true;
    }
    


    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction) override
    {
        if (uiAction == GOSSIP_ACTION_INFO_DEF)
        {
            pPlayer->SEND_GOSSIP_MENU(2013, pCreature->GetGUID());
            pPlayer->AreaExploredOrEventHappens(6981);
        }
        return true;
    }
    

    

    
};

class npc_taskmaster_fizzule : public CreatureScript
{
public: 
    npc_taskmaster_fizzule() : CreatureScript("npc_taskmaster_fizzule") { }
    struct npc_taskmaster_fizzuleAI : public ScriptedAI
    {
        npc_taskmaster_fizzuleAI(Creature* c) : ScriptedAI(c) {}
    
        bool IsFriend;
        uint32 Reset_Timer;
        uint32 FlareCount;
    
        void Reset()
        {
            IsFriend = false;
            Reset_Timer = 120000;
            FlareCount = 0;
            me->SetFaction(FACTION_HOSTILE_F);
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        }
    
        //This is a hack. Spellcast will make creature aggro but that is not
        //supposed to happen (Oregon not implemented/not found way to detect this spell kind)
        void DoUglyHack()
        {
            me->RemoveAllAuras();
            me->DeleteThreatList();
            me->CombatStop();
        }
    
        void SpellHit(Unit* /*caster*/, const SpellEntry* spell)
        {
            if (spell->Id == SPELL_FLARE || spell->Id == SPELL_FOLLY)
            {
                DoUglyHack();
                ++FlareCount;
                if (FlareCount >= 2)
                {
                    me->SetFaction(FACTION_FRIENDLY_F);
                    IsFriend = true;
                }
            }
        }
    
        void EnterCombat(Unit* /*who*/) {}
    
        void UpdateAI(const uint32 diff)
        {
            if (IsFriend)
            {
                if (Reset_Timer <= diff)
                {
                    EnterEvadeMode();
                    return;
                }
                else Reset_Timer -= diff;
            }
    
            if (!UpdateVictim())
                return;
    
            DoMeleeAttackIfReady();
        }
    
        void ReceiveEmote(Player* /*pPlayer*/, uint32 emote)
        {
            if (emote == TEXT_EMOTE_SALUTE)
            {
                if (FlareCount >= 2)
                {
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                }
            }
        }
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_taskmaster_fizzuleAI(pCreature);
    }

    

    
};

class npc_twiggy_flathead : public CreatureScript
{
public: 
    npc_twiggy_flathead() : CreatureScript("npc_twiggy_flathead") { }
    struct npc_twiggy_flatheadAI : public ScriptedAI
    {
        npc_twiggy_flatheadAI(Creature* creature) : ScriptedAI(creature) {}
    
        bool EventInProgress;
        bool EventGrate;
        bool EventBigWill;
        bool ChallengerDown[6];
        uint8 Wave;
        uint32 WaveTimer;
        uint32 ChallengerChecker;
        uint64 PlayerGUID;
        uint64 AffrayChallenger[6];
        uint64 BigWill;
    
        void Reset()
        {
            EventInProgress = false;
            EventGrate = false;
            EventBigWill = false;
            WaveTimer = 600000;
            ChallengerChecker = 0;
            Wave = 0;
            PlayerGUID = 0;
    
            for (uint8 i = 0; i < 6; ++i)
            {
                AffrayChallenger[i] = 0;
                ChallengerDown[i] = false;
            }
            BigWill = 0;
        }
    
        void EnterCombat(Unit* /*who*/) { }
    
        void EnterEvadeMode()
        {
            CleanUp();
            ScriptedAI::EnterEvadeMode();
        }
    
        void CleanUp()
        {
            for (uint8 i = 0; i < 6; ++i) // unsummon challengers
                if (AffrayChallenger[i])
                    if (Creature* creature = ObjectAccessor::GetCreature((*me), AffrayChallenger[i]))
                        creature->DespawnOrUnsummon(1);
    
            if (BigWill) // unsummon bigWill
                if (Creature* creature = ObjectAccessor::GetCreature((*me), BigWill))
                    creature->DespawnOrUnsummon(1);
        }
    
        void MoveInLineOfSight(Unit* who)
        {
            if (!who->IsAlive() || EventInProgress || who->GetTypeId() != TYPEID_PLAYER)
                return;
    
            if (me->IsWithinDistInMap(who, 10.0f) && who->ToPlayer()->GetQuestStatus(1719) == QUEST_STATUS_INCOMPLETE)
            {
                PlayerGUID = who->GetGUID();
                EventInProgress = true;
            }
        }
    
        void UpdateAI(uint32 diff)
        {
            if (EventInProgress)
            {
                Player* pWarrior = ObjectAccessor::GetPlayer(*me, PlayerGUID);
                if (!pWarrior || me->GetDistance2d(pWarrior) >= 200.0f)
                {
                    EnterEvadeMode();
                    return;
                }
    
                if (!pWarrior->IsAlive() && pWarrior->GetQuestStatus(1719) == QUEST_STATUS_INCOMPLETE)
                {
                    DoScriptText(SAY_TWIGGY_FLATHEAD_DOWN, me, nullptr);
                    pWarrior->FailQuest(1719);
                    EnterEvadeMode();
                    return;
                }
    
                if (!EventGrate)
                {
                    float x, y, z;
                    pWarrior->GetPosition(x, y, z);
    
                    if (x >= -1684 && x <= -1674 && y >= -4334 && y <= -4324)
                    {
                        pWarrior->AreaExploredOrEventHappens(1719);
                        DoScriptText(SAY_TWIGGY_FLATHEAD_BEGIN, me,  pWarrior);
    
                        for (uint8 i = 0; i < 6; ++i)
                        {
                            Creature* creature = me->SummonCreature(NPC_AFFRAY_CHALLENGER, AffrayChallengerLoc[i], TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
                            if (!creature)
                                continue;
    
                            creature->SetFaction(35);
                            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            creature->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                            AffrayChallenger[i] = creature->GetGUID();
                        }
                        WaveTimer = 5000;
                        ChallengerChecker = 1000;
                        EventGrate = true;
                    }
                }
                else
                {
                    if (ChallengerChecker <= diff)
                    {
                        for (uint8 i = 0; i < 6; ++i)
                        {
                            if (AffrayChallenger[i])
                            {
                                Creature* creature = ObjectAccessor::GetCreature(*me, AffrayChallenger[i]);
                                if ((!creature || !creature->IsAlive()) && !ChallengerDown[i])
                                {
                                    DoScriptText(SAY_TWIGGY_FLATHEAD_DOWN, me, nullptr);
                                    ChallengerDown[i] = true;
                                }
                            }
                        }
                        ChallengerChecker = 1000;
                    }
                    else ChallengerChecker -= diff;
    
                    if (WaveTimer <= diff)
                    {
                        if (Wave < 6 && !EventBigWill)
                        {
                            DoScriptText(SAY_TWIGGY_FLATHEAD_FRAY, me, nullptr);
                            Creature* creature = ObjectAccessor::GetCreature(*me, AffrayChallenger[Wave]);
                            if (creature && creature->IsAlive())
                            {
                                creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                creature->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                                creature->SetFaction(14);
                                creature->AI()->AttackStart(pWarrior);
                            }
                            ++Wave;
                            WaveTimer = 20000;
                        }
                        else if (Wave >= 6 && !EventBigWill)
                        {
                            if (Creature* creature = me->SummonCreature(NPC_BIG_WILL, -1722, -4341, 6.12f, 6.26f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 480000))
                            {
                                BigWill = creature->GetGUID();
                                creature->GetMotionMaster()->MovePoint(2, -1682, -4329, 2.79f);
                                creature->HandleEmoteCommand(EMOTE_ONESHOT_READYUNARMED);
                                EventBigWill = true;
                                WaveTimer = 10000;
                            }
                        }
                        else if (Wave >= 6 && EventBigWill)
                        {
                            Creature* creature = ObjectAccessor::GetCreature(*me, BigWill);
                            if (!creature || !creature->IsAlive())
                            {
                                DoScriptText(SAY_TWIGGY_FLATHEAD_OVER, me);
                                EnterEvadeMode();
                                return;
                            }
                            else if (creature) // Makes BIG WILL attackable.
                            {
                                creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                creature->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                                creature->SetFaction(14);
                                creature->AI()->AttackStart(pWarrior);
                            }
                            WaveTimer = 2000;
                        }
                    }
                    else
                        WaveTimer -= diff;
                }
            }
        }
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_twiggy_flatheadAI (pCreature);
    }

    

    
};

class npc_wizzlecrank_shredder : public CreatureScript
{
public: 
    npc_wizzlecrank_shredder() : CreatureScript("npc_wizzlecrank_shredder") { }
    struct npc_wizzlecrank_shredderAI : public npc_escortAI
    {
        npc_wizzlecrank_shredderAI(Creature* pCreature) : npc_escortAI(pCreature)
        {
            m_bIsPostEvent = false;
            m_uiPostEventTimer = 1000;
            m_uiPostEventCount = 0;
        }
    
        bool m_bIsPostEvent;
        uint32 m_uiPostEventTimer;
        uint32 m_uiPostEventCount;
    
        void Reset()
        {
            if (!HasEscortState(STATE_ESCORT_ESCORTING))
            {
                me->setDeathState(ALIVE);
                m_bIsPostEvent = false;
                m_uiPostEventTimer = 1000;
                m_uiPostEventCount = 0;
            }
        }
    
        void WaypointReached(uint32 uiPointId)
        {
            Player* pPlayer = GetPlayerForEscort();
    
            if (!pPlayer)
                return;
    
            switch (uiPointId)
            {
            case 0:
                DoScriptText(SAY_STARTUP1, me);
                break;
            case 9:
                SetRun(false);
                break;
            case 17:
                if (Creature* pTemp = me->SummonCreature(NPC_MERCENARY, 1128.489f, -3037.611f, 92.701f, 1.472f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000))
                {
                    DoScriptText(SAY_MERCENARY, pTemp);
                    me->SummonCreature(NPC_MERCENARY, 1160.172f, -2980.168f, 97.313f, 3.690f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                }
                break;
            case 24:
                m_bIsPostEvent = true;
                break;
            }
        }
    
        void WaypointStart(uint32 uiPointId)
        {
            Player* pPlayer = GetPlayerForEscort();
    
            if (!pPlayer)
                return;
    
            switch (uiPointId)
            {
            case 9:
                DoScriptText(SAY_STARTUP2, me, pPlayer);
                break;
            case 18:
                DoScriptText(SAY_PROGRESS_1, me, pPlayer);
                SetRun();
                break;
            }
        }
    
        void JustSummoned(Creature* pSummoned)
        {
            if (pSummoned->GetEntry() == NPC_PILOT_WIZZ)
                me->setDeathState(JUST_DIED);
    
            if (pSummoned->GetEntry() == NPC_MERCENARY)
                pSummoned->AI()->AttackStart(me);
        }
    
        void UpdateEscortAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
            {
                if (m_bIsPostEvent)
                {
                    if (m_uiPostEventTimer <= uiDiff)
                    {
                        switch (m_uiPostEventCount)
                        {
                        case 0:
                            DoScriptText(SAY_PROGRESS_2, me);
                            break;
                        case 1:
                            DoScriptText(SAY_PROGRESS_3, me);
                            break;
                        case 2:
                            DoScriptText(SAY_END, me);
                            break;
                        case 3:
                            if (Player* pPlayer = GetPlayerForEscort())
                            {
                                pPlayer->GroupEventHappens(QUEST_ESCAPE, me);
                                me->SummonCreature(NPC_PILOT_WIZZ, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 180000);
                            }
                            break;
                        }
    
                        ++m_uiPostEventCount;
                        m_uiPostEventTimer = 5000;
                    }
                    else
                        m_uiPostEventTimer -= uiDiff;
                }
    
                return;
            }
    
            DoMeleeAttackIfReady();
        }
    };

    

    

    bool OnQuestAccept(Player* pPlayer, Creature* pCreature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_ESCAPE)
        {
            pCreature->SetFaction(FACTION_RATCHET);
            if (npc_escortAI* pEscortAI = CAST_AI(npc_wizzlecrank_shredderAI, pCreature->AI()))
                pEscortAI->Start(true, false, pPlayer->GetGUID());
        }
        return true;
    }
    
     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_wizzlecrank_shredderAI(pCreature);
    }


    

    
};


void AddSC_the_barrens()
{
    new npc_beaten_corpse();
    new npc_gilthares();
    new npc_sputtervalve();
    new npc_taskmaster_fizzule();
    new npc_twiggy_flathead();
    new npc_wizzlecrank_shredder();

}

