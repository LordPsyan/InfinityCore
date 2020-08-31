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
SDName: Zulfarrak
SD%Complete: 85
SDCategory: Zul'Farrak
SDTodo: Make goblin run away. Spell for Barrel explosion needs proper implementation. Make all eventually hearth/fade out. Bly needs proper gossip.
        All cages should open on opening one. This is handled by event scripts currently but it doesn't seem to execute with a C++ script.
EndScriptData */

/* ContentData
npc_sergeant_bly
npc_weegli_blastfuse
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "zulfarrak.h"
#include "ScriptedGossip.h"

/*######
## npc_sergeant_bly
######*/

enum blySays
{
    SAY_1 = -1209002,
    SAY_2 = -1209003
};

enum blySpells
{
    SPELL_SHIELD_BASH = 11972,
    SPELL_REVENGE = 12170
};

#define GOSSIP_BLY "[PH] In that case, I will take my reward!"








/*######
## npc_weegli_blastfuse
######*/

enum weegliSpells
{
    SPELL_BOMB              = 8858,
    SPELL_GOBLIN_LAND_MINE  = 21688,
    SPELL_SHOOT             = 6660,
    SPELL_WEEGLIS_BARREL    = 10772,
    SPELL_BARREL_EXPLOSION  = 11195
};

enum weegliSays
{
    SAY_WEEGLI_OHNO = -1209000,
    SAY_WEEGLI_OK_I_GO = -1209001
};

#define GOSSIP_WEEGLI "[PH] Please blow up the door."







/*######
## go_shallow_grave
######*/

enum
{
    ZOMBIE = 7286,
    DEAD_HERO = 7276,
    ZOMBIE_CHANCE = 65,
    DEAD_HERO_CHANCE = 10
};





/*######
## at_zumrah
######*/

enum
{
    ZUMRAH_ID = 7271,
    ZUMRAH_HOSTILE_FACTION = 37
};



/*######
## at_antusul
######*/

enum
{
    ANTUSUL_ID = 8127
};





class npc_sergeant_bly : public CreatureScript
{
public: 
    npc_sergeant_bly() : CreatureScript("npc_sergeant_bly") { }
    struct npc_sergeant_blyAI : public ScriptedAI
    {
        npc_sergeant_blyAI(Creature* c) : ScriptedAI(c)
        {
            instance = (ScriptedInstance*)c->GetInstanceData();
            postGossipStep = 0;
        }
    
        uint32 ShieldBash_Timer;
        uint32 Revenge_Timer;                                   //this is wrong, spell should never be used unless me->GetVictim() dodge, parry or block attack. Trinity support required.
        ScriptedInstance* instance;
        uint32 postGossipStep;
        uint32 Text_Timer;
        uint64 PlayerGUID;
    
        void Reset()
        {
            ShieldBash_Timer = 5000;
            Revenge_Timer = 8000;
        }
    
        void UpdateAI(const uint32 diff)
        {
            if (postGossipStep > 0 && postGossipStep < 4)
            {
                if (Text_Timer < diff)
                {
                    switch (postGossipStep)
                    {
                    case 1:
                        //weegli doesn't fight - he goes & blows up the door
                        if (Creature* pWeegli = instance->instance->GetCreature(instance->GetData64(ENTRY_WEEGLI)))
                            pWeegli->AI()->DoAction(0);
                        me->Say(SAY_1, LANG_UNIVERSAL, 0);
                        Text_Timer = 5000;
                        break;
                    case 2:
                        me->Say(SAY_2, LANG_UNIVERSAL, 0);
                        Text_Timer = 5000;
                        break;
                    case 3:
                        me->SetFaction(FACTION_HOSTILE);
                        if (Player* target = ObjectAccessor::GetPlayer(*me, PlayerGUID))
                            AttackStart(target);
    
                        switchFactionIfAlive(instance, ENTRY_RAVEN);
                        switchFactionIfAlive(instance, ENTRY_ORO);
                        switchFactionIfAlive(instance, ENTRY_MURTA);
                        me->CallForHelp(10.0f);
                    }
                    postGossipStep++;
                }
                else Text_Timer -= diff;
            }
    
            if (!UpdateVictim())
                return;
    
            if (ShieldBash_Timer <= diff)
            {
                DoCastVictim( SPELL_SHIELD_BASH);
                ShieldBash_Timer = 15000;
            }
            else ShieldBash_Timer -= diff;
    
            if (Revenge_Timer <= diff)
            {
                DoCastVictim( SPELL_REVENGE);
                Revenge_Timer = 10000;
            }
            else Revenge_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    
        void DoAction(const int32 /*param*/)
        {
            postGossipStep = 1;
            Text_Timer = 0;
        }
    
        void switchFactionIfAlive(ScriptedInstance* instance, uint32 entry)
        {
            if (Creature* crew = instance->GetCreature(instance->GetData64(entry)))
                if (crew->IsAlive())
                    crew->SetFaction(FACTION_HOSTILE);
        }
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_sergeant_blyAI (pCreature);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        ScriptedInstance* instance = (ScriptedInstance*)creature->GetInstanceData();
        if (!instance)
            return false;
    
        if (instance->GetData(EVENT_PYRAMID) == PYRAMID_KILLED_ALL_TROLLS)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BLY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(1517, creature->GetGUID());
        }
        else if (instance->GetData(EVENT_PYRAMID) == PYRAMID_NOT_STARTED)
            player->SEND_GOSSIP_MENU(1515, creature->GetGUID());
        else
            player->SEND_GOSSIP_MENU(1516, creature->GetGUID());
        return true;
    }
    

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction) override
    {
        if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->CLOSE_GOSSIP_MENU();
            CAST_AI(npc_sergeant_blyAI, creature->AI())->PlayerGUID = player->GetGUID();
            creature->AI()->DoAction(0);
        }
        return true;
    }
    

    

    
};

class npc_weegli_blastfuse : public CreatureScript
{
public: 
    npc_weegli_blastfuse() : CreatureScript("npc_weegli_blastfuse") { }
    struct npc_weegli_blastfuseAI : public ScriptedAI
    {
        npc_weegli_blastfuseAI(Creature* c) : ScriptedAI(c)
        {
            instance = (ScriptedInstance*)c->GetInstanceData();
            destroyingDoor = false;
        }

        ScriptedInstance* instance;
        bool destroyingDoor;

        void Reset()
        {
        }

        void EnterCombat(Unit* /*who*/)
        {
        }

        void DoAction(const int32 /*param*/)
        {
            DestroyDoor();
        }

        void MovementInform(uint32 /*type*/, uint32 /*id*/)
        {
            if (instance->GetData(EVENT_PYRAMID) == PYRAMID_CAGES_OPEN)
            {
                instance->SetData(EVENT_PYRAMID, PYRAMID_ARRIVED_AT_STAIR);
                me->Say(SAY_WEEGLI_OHNO, LANG_UNIVERSAL, 0);
                me->SetHomePosition(1882.69f, 1272.28f, 41.87f, 0);
            }
            else if (destroyingDoor)
            {
                /// @todo Spell needs proper implementation.
                DoCastAOE(SPELL_BARREL_EXPLOSION, true);
                /// @todo leave the area...
                me->DisappearAndDie();
            };
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }

        void DestroyDoor()
        {
            if (me->IsAlive())
            {
                me->SetFaction(FACTION_FRIENDLY);
                me->GetMotionMaster()->MovePoint(0, 1858.57f, 1146.35f, 14.745f);
                me->SetHomePosition(1858.57f, 1146.35f, 14.745f, 3.85f); // in case he gets interrupted
                me->Say(SAY_WEEGLI_OK_I_GO, LANG_UNIVERSAL, 0);
                destroyingDoor = true;
            }
        }
    };

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_weegli_blastfuseAI (pCreature);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        ScriptedInstance* instance = (ScriptedInstance*)creature->GetInstanceData();
        if (!instance)
            return false;
    
        switch (instance->GetData(EVENT_PYRAMID))
        {
        case PYRAMID_KILLED_ALL_TROLLS:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_WEEGLI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(1514, creature->GetGUID()); //if event can proceed to end
            break;
        case PYRAMID_NOT_STARTED:
            player->SEND_GOSSIP_MENU(1511, creature->GetGUID()); //if event not started
            break;
        default:
            player->SEND_GOSSIP_MENU(1513, creature->GetGUID()); //if event are in progress
        }
    
        return true;
    }
    


    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction) override
    {
        ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        if (!pInstance)
            return false;
    
        if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        {
            pPlayer->CLOSE_GOSSIP_MENU();
            pCreature->AI()->DoAction(0);
        }
    
        return true;
    }
    

    

    
};

class go_shallow_grave : public GameObjectScript
{
public: 
    go_shallow_grave() : GameObjectScript("go_shallow_grave") { }
    

    

    bool OnGossipHello(Player* /*pPlayer*/, GameObject* pGo) override
    {
        // randomly summon a zombie or dead hero the first time a grave is used
        if (pGo->GetUseCount() == 0)
        {
            uint32 randomchance = urand(0, 99);
            if (randomchance < ZOMBIE_CHANCE)
                pGo->SummonCreature(ZOMBIE, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
            else if ((randomchance - ZOMBIE_CHANCE) < DEAD_HERO_CHANCE)
                pGo->SummonCreature(DEAD_HERO, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
        }
        pGo->AddUse();
        return false;
    }
    

    

    
};

class go_troll_cage : public GameObjectScript
{
public: 
    go_troll_cage() : GameObjectScript("go_troll_cage") { }
    

    

    bool OnGossipHello(Player* /*pPlayer*/, GameObject* pGo) override
    {
        ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();
        if (!pInstance)
            return false;
    
        pInstance->SetData(EVENT_PYRAMID, PYRAMID_CAGES_OPEN);
        return false;
    }
    

    

    
};

class at_zumrah : public AreaTriggerScript
{
public: 
    at_zumrah() : AreaTriggerScript("at_zumrah") { }
    

    bool OnTrigger(Player* pPlayer, const AreaTriggerEntry* /*at*/) override
    {
        Creature* Zumrah = pPlayer->FindNearestCreature(ZUMRAH_ID, 30.0f);
    
        if (!Zumrah)
            return false;
    
        Zumrah->SetFaction(ZUMRAH_HOSTILE_FACTION);
        Zumrah->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
        Zumrah->AI()->AttackStart(pPlayer);
        return true;
    }
};

class at_antusul : public AreaTriggerScript
{
public: 
    at_antusul() : AreaTriggerScript("at_antusul") { }
    
    

    bool OnTrigger(Player* pPlayer, const AreaTriggerEntry* /*at*/) override
    {
        Creature* Antusul = pPlayer->FindNearestCreature(ANTUSUL_ID, 80.0f);
    
        if (!Antusul)
            return false;
    
        Antusul->AI()->AttackStart(pPlayer);
        return true;
    }
    

 
    
};


void AddSC_zulfarrak()
{
    new npc_sergeant_bly();
    new npc_weegli_blastfuse();
    new go_shallow_grave();
    new go_troll_cage();
    new at_zumrah();
    new at_antusul();

}

