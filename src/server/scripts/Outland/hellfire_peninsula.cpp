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
SDName: Hellfire_Peninsula
SD%Complete: 99
 SDComment: Quest support: 9375, 9418, 10129, 10146, 10162, 10163, 10340, 10346, 10347, 10382 (Special flight paths), 9410, 10909, 10935, 9545, 10838, 11516, 10351.
SDCategory: Hellfire Peninsula
EndScriptData */

/* ContentData
npc_aeranas
npc_ancestral_wolf
go_haaleshi_altar
npc_naladu
npc_tracy_proudwell
npc_trollbane
npc_fel_guard_hound
npc_wing_commander_dabiree
npc_gryphoneer_leafbeard
npc_wing_commander_brack
npc_wounded_blood_elf
npc_hand_berserker
npc_anchorite_relic_bunny
npc_anchorite_barada
npc_darkness_released
npc_foul_purge
npc_sedai_quest_credit_marker
npc_vindicator_sedai
npc_demoniac_scryer
npc_magic_sucker_device_spawner
npc_living_flare
npc_pathaleon_image
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"

/*######
## npc_aeranas
######*/
enum eAeranas
{
    SAY_SUMMON                      = -1000138,
    SAY_FREE                        = -1000139,

    FACTION_HOSTILE                 = 16,
    FACTION_FRIENDLY                = 35,

    SPELL_ENVELOPING_WINDS          = 15535,
    SPELL_SHOCK                     = 12553,

    C_AERANAS                       = 17085
};





/*######
## npc_ancestral_wolf
######*/

enum
{
    EMOTE_WOLF_LIFT_HEAD         = -1000496,
    EMOTE_WOLF_HOWL              = -1000497,
    SAY_WOLF_WELCOME             = -1000498,

    SPELL_ANCESTRAL_WOLF_BUFF    = 29981,

    NPC_RYGA                     = 17123
};





/*######
## go_haaleshi_altar
######*/




/*######
## npc_naladu
######*/

#define GOSSIP_NALADU_ITEM1 "Why don't you escape?"

enum eNaladu
{
    GOSSIP_TEXTID_NALADU1   = 9788
};





/*######
## npc_tracy_proudwell
######*/

#define GOSSIP_TEXT_REDEEM_MARKS        "I have marks to redeem!"
#define GOSSIP_TRACY_PROUDWELL_ITEM1    "I heard that your dog Fei Fei took Klatu's prayer beads..."
#define GOSSIP_TRACY_PROUDWELL_ITEM2    "<back>"

enum eTracy
{
    GOSSIP_TEXTID_TRACY_PROUDWELL1       = 10689,
    QUEST_DIGGING_FOR_PRAYER_BEADS       = 10916
};





/*######
## npc_trollbane
######*/

#define GOSSIP_TROLLBANE_ITEM1      "Tell me of the Sons of Lothar."
#define GOSSIP_TROLLBANE_ITEM2      "<more>"
#define GOSSIP_TROLLBANE_ITEM3      "Tell me of your homeland."

enum eTrollbane
{
    GOSSIP_TEXTID_TROLLBANE1        = 9932,
    GOSSIP_TEXTID_TROLLBANE2        = 9933,
    GOSSIP_TEXTID_TROLLBANE3        = 8772
};





/*######
## npc_fel_guard_hound
######*/

enum eFelGuard
{
    SPELL_SUMMON_POO                              = 37688,

    NPC_DERANGED_HELBOAR                          = 16863
};





/*######
## npc_wing_commander_dabiree
######*/

#define GOSSIP_ITEM1_DAB "Fly me to Murketh and Shaadraz Gateways"
#define GOSSIP_ITEM2_DAB "Fly me to Shatter Point"





/*######
## npc_gryphoneer_leafbeard
######*/

enum
{
    SPELL_TAXI_TO_SHATTERP      = 35066
};

#define GOSSIP_ITEM1_LEAF       "Fly me to Shatter Point"





/*######
## npc_wing_commander_brack
######*/

#define GOSSIP_ITEM1_BRA "Fly me to Murketh and Shaadraz Gateways"
#define GOSSIP_ITEM2_BRA "Fly me to The Abyssal Shelf"
#define GOSSIP_ITEM3_BRA "Fly me to Spinebreaker Post"





/*######
## npc_wounded_blood_elf
######*/

enum eWoundedBloodElf
{
    SAY_ELF_START               = -1000117,
    SAY_ELF_SUMMON1             = -1000118,
    SAY_ELF_RESTING             = -1000119,
    SAY_ELF_SUMMON2             = -1000120,
    SAY_ELF_COMPLETE            = -1000121,
    SAY_ELF_AGGRO               = -1000122,

    QUEST_ROAD_TO_FALCON_WATCH  = 9375
};







/*######
## npc_anchorite_relic_bunny
######*/

enum
{
    NPC_HAND_BERSERKER      = 16878,
    NPC_FEL_SPIRIT          = 22454,
    SPELL_CHANNELS          = 39184,
    SPELL_SOUL_BURDEN       = 38879,

    GO_RELIC                = 185298,
    SAY_SP                  = -1900130
};





/*######
## npc_hand_berserker
######*/

enum
{
    SPELL_ENRAGE            = 8599,
    SPELL_CHARGE            = 35570,

    NPC_BUNNY               = 22444
};





/*######
## npc_anchorite_barada
######*/

#define GOSSIP_ITEM_START      "I am ready Anchorite. Let us begin the exorcism."
#define SAY_BARADA1            -1900100
#define SAY_BARADA2            -1900101
#define SAY_BARADA3            -1900104
#define SAY_BARADA4            -1900105
#define SAY_BARADA5            -1900106
#define SAY_BARADA6            -1900107
#define SAY_BARADA7            -1900108
#define SAY_BARADA8            -1900109
#define SAY_COLONEL1           -1900110
#define SAY_COLONEL2           -1900111
#define SAY_COLONEL3           -1900112
#define SAY_COLONEL4           -1900113
#define SAY_COLONEL5           -1900114
#define SAY_COLONEL6           -1900115
#define SAY_COLONEL7           -1900116
#define SAY_COLONEL8           -1900117

enum
{
    QUEST_THE_EXORCIM          = 10935,
    NPC_COLONEL_JULES          = 22432,
    NPC_DARKNESS_RELEASED      = 22507,

    SPELL_EXORCIM              = 39277,
    SPELL_EXORCIM2             = 39278,
    SPELL_COLONEL1             = 39283,
    SPELL_COLONEL2             = 39294,
    SPELL_COLONEL3             = 39284,
    SPELL_COLONEL4             = 39294,
    SPELL_COLONEL5             = 39295,
    SPELL_COLONEL7             = 39381,
    SPELL_COLONEL8             = 39380
};









/*######
## npc_darkness_released
######*/

enum
{
    SPELL_AURA_ME     = 39303,
    SPELL_DARKNESS    = 39307,
    NPC_BARADA        = 22431
};





/*######
## npc_foul_purge
######*/





/*######
## npc_sedai_quest_credit_marker
######*/

enum
{
    NPC_ESCORT1    = 17417,
    NPC_SEDAI      = 17404
};





/*######
## npc_vindicator_sedai
######*/

#define SAY_MAG_ESSCORT    -1900125
#define SAY_SEDAI1         -1900126
#define SAY_SEDAI2         -1900127
#define SAY_KRUN           -1900128

enum
{
    NPC_ESCORT        = 17417,
    NPC_AMBUSHER      = 17418,
    NPC_KRUN          = 17405,

    SPELL_STUN        = 13005,
    SPELL_HOLYFIRE    = 17141
};

/*######
## npc_demoniac_scryer
######*/

#define GOSSIP_ITEM_ATTUNE          "Yes, Scryer. You may possess me."

enum
{
    GOSSIP_TEXTID_PROTECT           = 10659,
    GOSSIP_TEXTID_ATTUNED           = 10643,

    QUEST_DEMONIAC                  = 10838,
    NPC_HELLFIRE_WARDLING           = 22259,
    NPC_ORC_HA                      = 22273,
    NPC_BUTTRESS                    = 22267,
    NPC_BUTTRESS_SPAWNER            = 22260,

    MAX_BUTTRESS                    = 4,
    TIME_TOTAL                      = MINUTE * 10 * IN_MILLISECONDS,

    SPELL_SUMMONED                  = 7741,
    SPELL_DEMONIAC_VISITATION       = 38708,
    SPELL_BUTTRESS_APPERANCE        = 38719,
    SPELL_SUCKER_CHANNEL            = 38721,
    SPELL_SUCKER_DESPAWN_MOB        = 38691,
};









/*######
## npc_magic_sucker_device_spawner
######*/

enum
{
    SPELL_EFFECT    = 38724,
    NPC_SCRYER      = 22258,
    NPC_BUTTRES     = 22267
};




/*######
## npc_living_flare
######*/

enum
{
    SPELL_COSMETIC             = 44880,
    SPELL_UNSTABLE_COSMETIC    = 46196,
    SPELL_ABSORBED             = 44944,

    NPC_FEL_SPARK              = 22323,
    NPC_TRIGGER                = 24959,

    GO_FIRE                    = 185319
};





/*######
## npc_pathaleon_image
######*/

enum
{
    SAY_PATHALEON1         = -1900165,
    SAY_PATHALEON2         = -1900166,
    SAY_PATHALEON3         = -1900167,
    SAY_PATHALEON4         = -1900168,

    SPELL_ROOTS            = 35468,
    SPELL_INSECT           = 35471,
    SPELL_LIGHTING         = 35487,
    SPELL_TELE             = 7741,

    NPC_TARGET_TRIGGER     = 20781,
    NPC_CRYSTAL_TRIGGER    = 20617,
    NPC_GOLIATHON          = 19305,
};





////////////////////////
//*Arrazius the Cruel*//
////////////////////////

enum AraziusSpells
{
	SPELL_ARAZIUS_POWER = 34094,
	SPELL_INFERNO = 34249,
	SPELL_SHADOWBOLT_VOLLEY = 19191,
	SPELL_FEEBLE_WEAPONS = 34088,
	SPELL_DOUBTING_MIND = 34089,
	SPELL_CHILLING_WORDS = 34087,
	SPELL_PYROBLAST = 33975,

	SAY_DIE1 = -1910094,
	SAY_SLAY1 = -1910093,
	SAY_AGGRO1 = -1910091,
	SAY_AGGRO2 = -1910092
};






enum FelCannonSpells
{
	SPELL_FEL_CANNON_BLAST = 36238,
	SPELL_COSMETIC_IMPACT_FIRE = 40109
};





enum Maghar
{
	NPC_MAGHAR = 49850,
	NPC_INJURED_MAGHAR = 16847,

	SAY_THANKS = -1910100
};




enum Dreghood
{
	SPELL_HAMSTRING = 31553,

	SAY_SORRY = -1910101,
	SAY_RUN = -1910102,
	SAY_SORRY2 = -1910103,

	NPC_TASKMASTER = 17058,
};

Position const FleePath = { -414.151f, 4789.621f, 19.757f, 0.0f };





//*Dreghood Elders Quest - Gossip*//

#define GOSSIP_ITEM_1 "Walk free, elder. Bring the spirits back to your tribe."

enum Elders
{
	ELDER_SAY_1 = -1910104,
	ELDER_SAY_2 = -1910105,
	ELDER_SAY_3 = -1910106
};

























/*######
## npc_viera
######*/

enum VieraSays
{
	SAY_GOT_BEER = -1910095,
	VIERA_SAY1 = -1910096,
	VIERA_SAY2 = -1910097,
	SAY_CAT_1 = -1910098,
};

enum VieraSpells
{
	NPC_CAT = 17230,
	NPC_VIERA = 17226
};

enum VieraQuests
{
	QUEST_ARELION_MISTRESS_SIDEQ = 9483
};

Position const ChasePath = { -742.759f, 4073.034f, 47.413f, 0.0f };

// cat change it into MovePath if possible.
Position const ChasePath1 = { -655.107f, 4147.2f, 64.1146f, 0.0f };
Position const ChasePath2 = { -664.461f, 4147.91f, 64.156f, 0.0f };
Position const ChasePath3 = { -681.972f, 4146.48f, 64.4093f, 0.0f };
Position const ChasePath4 = { -684.262f, 4154.27f, 62.1889f, 0.0f };
Position const ChasePath5 = { -693.5f, 4185.7f, 57.0529f, 0.0f };
Position const ChasePath6 = { -708.753f, 4187.82f, 55.1475f, 0.0f };
Position const ChasePath7 = { -721.51f, 4189.59f, 51.8167f, 0.0f };
Position const ChasePath8 = { -721.682f, 4170.45f, 50.7466f, 0.0f };







enum FelIniSpells
{
	SPELL_FELCHANNEL = 33535,
	SPELL_SINISTER_STRIKE = 14873,
	SPELL_BITTER_WITHDRAWAL = 29098,
	SPELL_SPELLBREAKER = 35871,

	SAY_FEL1 = -1910107,
	SAY_FEL2 = -1910108,
	SAY_FEL3 = -1910109,
	SAY_FEL4 = -1910110,
	SAY_FEL5 = -1910111
};













/*###
# Quest 10792 "Zeth'Gor Must Burn!" (Horde) - Visual Effect
####*/

enum Quest10792
{
	SPELL_FIRE = 35724,
	GO_Z_FIRE = 183816
};












/*###
# Quest 10895 "Zeth'Gor Must Burn!" (Alliance) - Visual Effect
####*/













/*######
## go_central_beacon
######*/



/*######
## go_western_beacon
######*/



/*######
## go_southern_beacon
######*/



/*######
## trigger_beacon_fire
######*/




/*######
## SOURCE_OF_THE_CORRUPTION_EVENT
######*/

#define SAY_AZETHEN_1 -1910251
#define SAY_AZETHEN_2 -1910252
#define SAY_AZETHEN_3 -1910253
#define SAY_AZETHEN_4 -1910255
#define SAY_PRISONER  -1910254









class npc_aeranas : public CreatureScript
{
public: 
    npc_aeranas() : CreatureScript("npc_aeranas") { }
    struct npc_aeranasAI : public ScriptedAI
    {
        npc_aeranasAI(Creature* c) : ScriptedAI(c) {}
    
        uint32 Faction_Timer;
        uint32 EnvelopingWinds_Timer;
        uint32 Shock_Timer;
    
        void Reset()
        {
            Faction_Timer = 8000;
            EnvelopingWinds_Timer = 9000;
            Shock_Timer = 5000;
    
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            me->SetFaction(FACTION_FRIENDLY);
    
            DoScriptText(SAY_SUMMON, me);
        }
    
        void EnterCombat(Unit*) {}
    
        void UpdateAI(const uint32 diff)
        {
            if (Faction_Timer)
            {
                if (Faction_Timer <= diff)
                {
                    me->SetFaction(FACTION_HOSTILE);
                    Faction_Timer = 0;
                }
                else Faction_Timer -= diff;
            }
    
            if (!UpdateVictim())
                return;
    
            if ((me->GetHealth() * 100) / me->GetMaxHealth() < 30)
            {
                me->SetFaction(FACTION_FRIENDLY);
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                me->RemoveAllAuras();
                me->DeleteThreatList();
                me->CombatStop();
                DoScriptText(SAY_FREE, me);
                return;
            }
    
            if (Shock_Timer <= diff)
            {
                DoCastVictim(SPELL_SHOCK);
                Shock_Timer = 10000;
            }
            else Shock_Timer -= diff;
    
            if (EnvelopingWinds_Timer <= diff)
            {
                DoCastVictim(SPELL_ENVELOPING_WINDS);
                EnvelopingWinds_Timer = 25000;
            }
            else EnvelopingWinds_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_aeranasAI (pCreature);
    }

    

    
};

class npc_ancestral_wolf : public CreatureScript
{
public: 
    npc_ancestral_wolf() : CreatureScript("npc_ancestral_wolf") { }
    struct npc_ancestral_wolfAI : public npc_escortAI
    {
        npc_ancestral_wolfAI(Creature* pCreature) : npc_escortAI(pCreature)  {}
    
        void Reset()
        {
            me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
        }
    
        void AttackedBy(Unit*) {}
        void AttackStart(Unit*) {}
    
        void WaypointReached(uint32 uiPointId)
        {
            switch (uiPointId)
            {
            case 0:
                DoScriptText(EMOTE_WOLF_LIFT_HEAD, me);
                DoCast(me, SPELL_ANCESTRAL_WOLF_BUFF, true);
                break;
            case 2:
                DoScriptText(EMOTE_WOLF_HOWL, me);
                break;
            case 5:
                SetRun();
                break;
            case 15:
                SetRun(false);
                break;
            case 23:
                SetRun();
                break;
            case 42:
                SetRun(false);
                DoCast(me, SPELL_ANCESTRAL_WOLF_BUFF, true);
                break;
            case 50:
                if (Creature* pRyga = me->FindNearestCreature(NPC_RYGA, 15.0f))
                    if (pRyga && pRyga->IsAlive() && !pRyga->IsInCombat())
                        DoScriptText(SAY_WOLF_WELCOME, pRyga);
                break;
            }
        }
    
        void MoveInLineOfSight(Unit* pWho)
        {
            if (HasEscortState(STATE_ESCORT_ESCORTING))
                return;
    
            if (pWho->GetTypeId() == TYPEID_PLAYER)
                Start(false, false, pWho->GetGUID());
        }
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_ancestral_wolfAI(pCreature);
    }

    

    
};

class go_haaleshi_altar : public GameObjectScript
{
public: 
    go_haaleshi_altar() : GameObjectScript("go_haaleshi_altar") { }
    

    

    bool OnGossipHello(Player*, GameObject* _GO) override
    {
        _GO->SummonCreature(C_AERANAS, -1321.79f, 4043.80f, 116.24f, 1.25f, TEMPSUMMON_TIMED_DESPAWN, 180000);
        return false;
    }
    

    

    
};

class npc_naladu : public CreatureScript
{
public: 
    npc_naladu() : CreatureScript("npc_naladu") { }
    

    

    bool OnGossipHello(Player* pPlayer, Creature* pCreature) override
    {
        if (pCreature->IsQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());
    
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_NALADU_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
        return true;
    }
    

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction) override
    {
        if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_NALADU1, pCreature->GetGUID());
    
        return true;
    }
    

    

    
};

class npc_tracy_proudwell : public CreatureScript
{
public: 
    npc_tracy_proudwell() : CreatureScript("npc_tracy_proudwell") { }
    

    

    bool OnGossipHello(Player* pPlayer, Creature* pCreature) override
    {
        if (pCreature->IsQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());
    
        if (pCreature->IsVendor())
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_REDEEM_MARKS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
    
        if (pPlayer->GetQuestStatus(QUEST_DIGGING_FOR_PRAYER_BEADS) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TRACY_PROUDWELL_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
        return true;
    }
    

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction) override
    {
        switch (uiAction)
        {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TRACY_PROUDWELL_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_TRACY_PROUDWELL1, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_TRADE:
            pPlayer->SEND_VENDORLIST(pCreature->GetGUID());
            break;
        }
    
        return true;
    }
    

    

    
};

class npc_trollbane : public CreatureScript
{
public: 
    npc_trollbane() : CreatureScript("npc_trollbane") { }
    

    

    bool OnGossipHello(Player* pPlayer, Creature* pCreature) override
    {
        if (pCreature->IsQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());
    
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TROLLBANE_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TROLLBANE_ITEM3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
        return true;
    }
    

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction) override
    {
        switch (uiAction)
        {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TROLLBANE_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_TROLLBANE1, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_TROLLBANE2, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_TROLLBANE3, pCreature->GetGUID());
            break;
        }
    
        return true;
    }
    

    

    
};

class npc_wing_commander_dabiree : public CreatureScript
{
public: 
    npc_wing_commander_dabiree() : CreatureScript("npc_wing_commander_dabiree") { }
    

    

    bool OnGossipHello(Player* player, Creature* pCreature) override
    {
        if (pCreature->IsQuestGiver())
            player->PrepareQuestMenu(pCreature->GetGUID());
    
        //Mission: The Murketh and Shaadraz Gateways
        if (player->GetQuestStatus(10146) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM1_DAB, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    
        //Shatter Point
        if (!player->GetQuestRewardStatus(10340))
            player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM2_DAB, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());
    
        return true;
    }
    

    bool OnGossipSelect(Player* player, Creature* /*pCreature*/, uint32 /*sender*/, uint32 action) override
    {
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, 33768, true);             //TaxiPath 585 (Gateways Murket and Shaadraz)
        }
        if (action == GOSSIP_ACTION_INFO_DEF + 2)
        {
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, 35069, true);             //TaxiPath 612 (Taxi - Hellfire Peninsula - Expedition Point to Shatter Point)
        }
        return true;
    }
    

    

    
};

class npc_gryphoneer_leafbeard : public CreatureScript
{
public: 
    npc_gryphoneer_leafbeard() : CreatureScript("npc_gryphoneer_leafbeard") { }
    

    

    bool OnGossipHello(Player* player, Creature* pCreature) override
    {
        //Go back to Shatter Point if player has completed the quest 10340 - Shatter Point
        if (player->GetQuestStatus(10340) == QUEST_STATUS_COMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM1_LEAF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());
        return true;
    }
    

    bool OnGossipSelect(Player* player, Creature* /*pCreature*/, uint32 /*sender*/, uint32 action) override
    {
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->CLOSE_GOSSIP_MENU();
            //TaxiPath 609 (3.x.x)
            player->CastSpell(player, SPELL_TAXI_TO_SHATTERP, true);
        }
        return true;
    }
    

    

    
};

class npc_wing_commander_brack : public CreatureScript
{
public: 
    npc_wing_commander_brack() : CreatureScript("npc_wing_commander_brack") { }
    

    

    bool OnGossipHello(Player* player, Creature* pCreature) override
    {
        if (pCreature->IsQuestGiver())
            player->PrepareQuestMenu(pCreature->GetGUID());
    
        //Mission: The Murketh and Shaadraz Gateways
        if (player->GetQuestStatus(10129) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM1_BRA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    
        //Mission: The Abyssal Shelf || Return to the Abyssal Shelf
        if (player->GetQuestStatus(10162) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(10347) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM2_BRA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    
        //Spinebreaker Post
        if (player->GetQuestStatus(10242) == QUEST_STATUS_COMPLETE && !player->GetQuestRewardStatus(10242))
            player->ADD_GOSSIP_ITEM(2, GOSSIP_ITEM3_BRA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());
    
        return true;
    }
    


    bool OnGossipSelect(Player* player, Creature* /*pCreature*/, uint32 /*sender*/, uint32 action) override
    {
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1:
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, 33659, true);             //TaxiPath 584 (Gateways Murket and Shaadraz)
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, 33825, true);             //TaxiPath 587 (Aerial Assault Flight (Horde))
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, 34578, true);             //TaxiPath 604 (Taxi - Reaver's Fall to Spinebreaker Ridge)
            break;
        }
        return true;
    }
    

    

    
};

class npc_wounded_blood_elf : public CreatureScript
{
public: 
    npc_wounded_blood_elf() : CreatureScript("npc_wounded_blood_elf") { }
    struct npc_wounded_blood_elfAI : public npc_escortAI
    {
        npc_wounded_blood_elfAI(Creature* c) : npc_escortAI(c) {}
    
        void WaypointReached(uint32 i)
        {
            Player* pPlayer = GetPlayerForEscort();
    
            if (!pPlayer)
                return;
    
            switch (i)
            {
            case 0:
                DoScriptText(SAY_ELF_START, me, pPlayer);
                break;
            case 9:
                DoScriptText(SAY_ELF_SUMMON1, me, pPlayer);
                // Spawn two Haal'eshi Talonguard
                DoSpawnCreature(16967, -15, -15, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                DoSpawnCreature(16967, -17, -17, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                break;
            case 13:
                DoScriptText(SAY_ELF_RESTING, me, pPlayer);
                break;
            case 14:
                DoScriptText(SAY_ELF_SUMMON2, me, pPlayer);
                // Spawn two Haal'eshi Windwalker
                DoSpawnCreature(16966, -15, -15, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                DoSpawnCreature(16966, -17, -17, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                break;
            case 27:
                DoScriptText(SAY_ELF_COMPLETE, me, pPlayer);
                // Award quest credit
                pPlayer->GroupEventHappens(QUEST_ROAD_TO_FALCON_WATCH, me);
                break;
            }
        }
    
        void Reset() { }
    
        void EnterCombat(Unit*)
        {
            if (HasEscortState(STATE_ESCORT_ESCORTING))
                DoScriptText(SAY_ELF_AGGRO, me);
        }
    
        void JustSummoned(Creature* summoned)
        {
            summoned->AI()->AttackStart(me);
        }
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_wounded_blood_elfAI(pCreature);
    }

    bool OnQuestAccept(Player* pPlayer, Creature* pCreature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_ROAD_TO_FALCON_WATCH)
        {
            if (npc_escortAI* pEscortAI = CAST_AI(npc_wounded_blood_elfAI, pCreature->AI()))
                pEscortAI->Start(true, false, pPlayer->GetGUID());
    
            // Change faction so mobs attack
            pCreature->SetFaction(775);
        }
    
        return true;
    }
    

    

    
};

class npc_anchorite_relic_bunny : public CreatureScript
{
public:
	npc_anchorite_relic_bunny() : CreatureScript("npc_anchorite_relic_bunny") { }
	struct npc_anchorite_relic_bunnyAI : public ScriptedAI
	{
		npc_anchorite_relic_bunnyAI(Creature* creature) : ScriptedAI(creature) {}

		uint32 ChTimer;
		uint32 EndTimer;

		void Reset()
		{
			ChTimer = 2000;
			EndTimer = 60000;
		}

		void JustSummoned(Creature* summoned)
		{
			if (summoned->GetEntry() == NPC_FEL_SPIRIT)
			{
				DoScriptText(SAY_SP, summoned);
				summoned->AI()->AttackStart(summoned->GetVictim());
			}
		}

		void DoSpellHit(Unit* caster)
		{
			float x, y, z;
			x = caster->GetPositionX();
			y = caster->GetPositionY();
			z = caster->GetPositionZ();

			me->InterruptNonMeleeSpells(false);
			me->SummonCreature(NPC_FEL_SPIRIT, x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			ChTimer = 2000;
		}

		// @todo Fix SpellHit not being called when caster is dead
		// This may have other implications elsewhere in the core
		void SpellHit(Unit* /*caster*/, const SpellEntry* spell)
		{
			if (spell->Id == SPELL_SOUL_BURDEN)
			{
				sLog.outDebug("SpellHit on dead caster has been resolved. Please remove me from hellfire_peninsula.cpp");
				me->InterruptNonMeleeSpells(false);
				me->SummonCreature(NPC_FEL_SPIRIT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				ChTimer = 2000;
			}
		}

		void UpdateAI(const uint32 diff)
		{
			if (ChTimer <= diff)
			{
				if (Creature* Ber = GetClosestCreatureWithEntry(me, NPC_HAND_BERSERKER, 17.5f, true))
				{
					{
						DoCast(Ber, SPELL_CHANNELS, false);
						ChTimer = 95000;
					}
				}
				else me->InterruptNonMeleeSpells(false);
			}
			else ChTimer -= diff;

			if (EndTimer <= diff)
			{
				if (GameObject* relic = GetClosestGameObjectWithEntry(me, GO_RELIC, 5.0f))
				{
					relic->RemoveFromWorld();
					me->setDeathState(JUST_DIED);
					me->RemoveCorpse();
				}

				EndTimer = 60000;
			}
			else EndTimer -= diff;
		}
	};



	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_anchorite_relic_bunnyAI(creature);
	}




};

class npc_hand_berserker : public CreatureScript
{
public: 
    npc_hand_berserker() : CreatureScript("npc_hand_berserker") { }
    struct npc_hand_berserkerAI : public ScriptedAI
    {
        npc_hand_berserkerAI(Creature* creature) : ScriptedAI(creature) {}
    
        void Reset() {}
    
        void EnterCombat(Unit* who)
        {
            if (rand() % 60)
                DoCast(who, SPELL_CHARGE);
        }
    
        void DamageTaken(Unit* doneby, uint32& damage)
        {
            if (me->HasAura(SPELL_ENRAGE))
                return;
    
            if (doneby->GetTypeId() == TYPEID_PLAYER && (me->GetHealth() * 100 - damage) / me->GetMaxHealth() < 30)
                DoCast(me, SPELL_ENRAGE);
        }
    
        void JustDied(Unit* /*who*/)
        {
            if (Creature* Bunny = GetClosestCreatureWithEntry(me, NPC_BUNNY, 17.5f))
            {
                Bunny->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                DoCast(Bunny, SPELL_SOUL_BURDEN);
                CAST_AI(npc_anchorite_relic_bunny::npc_anchorite_relic_bunnyAI, Bunny->AI())->DoSpellHit(me);
            }
        }
    };

    

     CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hand_berserkerAI(creature);
    }

    

    
};



class npc_anchorite_barada : public CreatureScript
{
public: 
    npc_anchorite_barada() : CreatureScript("npc_anchorite_barada") { }
    struct npc_anchorite_baradaAI : public ScriptedAI
    {
        npc_anchorite_baradaAI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
        bool Exorcim;
    
        uint64 uiPlayerGUID;
        uint32 uiStepsTimer;
        uint32 uiSteps;
    
        float colonel;
    
        void Reset()
        {
            Exorcim = false;
            uiStepsTimer = 0;
            uiSteps = 0;
            uiPlayerGUID = 0;
        }
    
        void AttackedBy(Unit*) {}
        void AttackStart(Unit*) {}
    
        void DoSpawnDarkness()
        {
            me->SummonCreature(NPC_DARKNESS_RELEASED, -710.924f, 2754.683f, 105.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
        }
    
        void MoveInLineOfSight(Unit* pWho)
        {
            if (pWho->GetTypeId() == TYPEID_PLAYER)
            {
                if (CAST_PLR(pWho)->GetQuestStatus(QUEST_THE_EXORCIM) == QUEST_STATUS_INCOMPLETE)
                {
                    if (me->IsWithinDistInMap(((Player*)pWho), 5))
                        uiPlayerGUID = pWho->GetGUID();
                }
            }
        }
    
        uint32 NextStep(uint32 uiSteps)
        {
            Creature* pColonel = me->FindNearestCreature(NPC_COLONEL_JULES, 15);
            colonel = me->GetAngle(pColonel->GetPositionX(), pColonel->GetPositionY());
    
            switch (uiSteps)
            {
            case 1:
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                pColonel->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                return 2000;
            case 2:
                DoScriptText(SAY_BARADA1, me, 0);
                return 5000;
            case 3:
                DoScriptText(SAY_BARADA2, me, 0);
                return 3000;
            case 4:
                DoScriptText(SAY_COLONEL1, pColonel, 0);
                return 3000;
            case 5:
                me->SetWalk(true);
                return 3000;
            case 6:
                me->GetMotionMaster()->MovePoint(0, -707.702f, 2749.038f, 101.590f);
                return 2000;
            case 7:
                me->GetMotionMaster()->MovePoint(0, -710.810f, 2748.376f, 101.590f);
                return 2100;
            case 8:
                me->SetOrientation(colonel);
                return 2000;
            case 9:
                me->CastSpell(me, SPELL_EXORCIM , false);
                return 10000;
            case 10:
                DoScriptText(SAY_BARADA3, me, 0);
                return 10000;
            case 11:
                DoScriptText(SAY_COLONEL2, pColonel, 0);
                return 8000;
            case 12:
                me->RemoveAllAuras();
            case 13:
                me->CastSpell(me, SPELL_EXORCIM2 , false);
            case 14:
                pColonel->CastSpell(pColonel, SPELL_COLONEL1, false);
            case 15:
                pColonel->SetCanFly(true);
                pColonel->SetSpeed(MOVE_RUN, 0.17f);
    
                pColonel->GetMotionMaster()->MovePoint(0, -710.611f, 2753.435f, 103.774f);
                pColonel->CastSpell(pColonel, SPELL_COLONEL3, false);
                return 14000;
            case 16:
                DoScriptText(SAY_COLONEL3, pColonel, 0);
                DoSpawnDarkness();
                DoSpawnDarkness();
                return 14000;
            case 17:
                DoScriptText(SAY_BARADA4, me, 0);
                DoSpawnDarkness();
                DoSpawnDarkness();
                return 14000;
            case 18:
                DoScriptText(SAY_COLONEL4, pColonel, 0);
                DoSpawnDarkness();
                return 14000;
            case 19:
                DoScriptText(SAY_BARADA5, me, 0);
                return 14000;
            case 20:
                pColonel->CastSpell(pColonel, SPELL_COLONEL4, false);
                pColonel->CastSpell(pColonel, SPELL_COLONEL2, false);
                DoSpawnDarkness();
                return 1500;
            case 21:
                pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
                return 5000;
            case 22:
                DoScriptText(SAY_COLONEL5, pColonel, 0);
                return 1000;
            case 23:
                pColonel->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 103.774f);
                DoSpawnDarkness();
                return 4000;
            case 24:
                pColonel->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 103.774f);
                pColonel->CastSpell(me, SPELL_COLONEL5, false);
                return 2500;
            case 25:
                DoScriptText(SAY_BARADA6, me, 0);
            case 26:
                pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
                DoSpawnDarkness();
                return 3500;
            case 27:
                pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
                return 4000;
            case 28:
                pColonel->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 103.774f);
                DoScriptText(SAY_COLONEL6, pColonel, 0);
                DoSpawnDarkness();
                return 4000;
            case 29:
                pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
                return 4000;
            case 30:
                pColonel->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 103.774f);
                return 4000;
            case 31:
                DoScriptText(SAY_BARADA7, me, 0);
                return 0;
            case 32:
                pColonel->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 103.774f);
                DoSpawnDarkness();
                return 4000;
            case 33:
                pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
                return 4000;
            case 34:
                pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
                DoScriptText(SAY_COLONEL7, pColonel, 0);
                DoSpawnDarkness();
                return 4000;
            case 35:
                pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
                return 4000;
            case 36:
                pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
                DoSpawnDarkness();
                return 4000;
            case 37:
                DoScriptText(SAY_BARADA6, me, 0);
            case 38:
                pColonel->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 103.774f);
                return 4000;
            case 39:
                pColonel->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 103.774f);
                return 4000;
            case 40:
                pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
                DoScriptText(SAY_COLONEL8, pColonel, 0);
                return 4000;
            case 41:
                pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
                return 4000;
            case 42:
                pColonel->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 103.774f);
                return 4000;
            case 43:
                DoScriptText(SAY_BARADA6, me, 0);
                return 1000;
            case 44:
                pColonel->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 103.774f);
                return 4000;
            case 45:
                pColonel->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 103.774f);
                pColonel->CastSpell(pColonel, SPELL_COLONEL8, false);
                return 4000;
            case 46:
                pColonel->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 103.774f);
                pColonel->CastSpell(pColonel, SPELL_COLONEL7, false);
                return 4000;
            case 47:
                pColonel->GetMotionMaster()->MovePoint(0, -710.792f, 2750.693f, 103.774f);
                return 5000;
            case 48:
                DoScriptText(SAY_BARADA8, me, 0);
                return 1000;
            case 49:
                pColonel->GetMotionMaster()->MovePoint(0, -710.111f, 2754.346f, 102.367f);
                return 3000;
            case 50:
                pColonel->RemoveAllAuras();
            case 51:
                me->RemoveAllAuras();
                return 2000;
            case 52:
                me->SetWalk(true);
                return 2000;
            case 53:
                me->GetMotionMaster()->MovePoint(0, -706.726f, 2751.632f, 101.591f);
                return 2200;
            case 54:
                me->GetMotionMaster()->MovePoint(0, -707.382f, 2753.994f, 101.591f);
                return 7000;
            case 55:
                me->SetStandState(UNIT_STAND_STATE_KNEEL);
                me->CombatStop();
                return 3000;
            case 56:
                pColonel->SetFlag(UNIT_NPC_FLAGS, 1);
                return 6000;
            case 57:
                me->SetFlag(UNIT_NPC_FLAGS, 1);
                if (Player* pPlayer = Unit::GetPlayer(*me, uiPlayerGUID))
                    pPlayer->AreaExploredOrEventHappens(QUEST_THE_EXORCIM);
                return 1000;
            case 58:
                Reset();
            default:
                return 0;
            }
        }
    
        void JustDied(Unit*)
        {
            if (Creature* pColonel = me->FindNearestCreature(NPC_COLONEL_JULES, 15.0f, true))
            {
                pColonel->GetMotionMaster()->MovePoint(0, -710.111f, 2754.346f, 102.367f);
                pColonel->RemoveAllAuras();
            }
        }
    
        void UpdateAI(const uint32 uiDiff)
        {
            if (uiStepsTimer <= uiDiff)
            {
                if (Exorcim)
                    uiStepsTimer = NextStep(++uiSteps);
            }
            else uiStepsTimer -= uiDiff;
        }
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_anchorite_baradaAI(pCreature);
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature) override
    {
        if (pPlayer->GetQuestStatus(QUEST_THE_EXORCIM) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction) override
    {
        if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        {
            ((npc_anchorite_baradaAI*)pCreature->AI())->Exorcim = true;
            pPlayer->CLOSE_GOSSIP_MENU();
        }
        return true;
    }
    

    

    
};

class npc_darkness_released : public CreatureScript
{
public: 
    npc_darkness_released() : CreatureScript("npc_darkness_released") { }
    struct npc_darkness_releasedAI : public ScriptedAI
    {
        npc_darkness_releasedAI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
        uint32 uiAtTimer;
        uint32 uiChTimer;
    
        void Reset()
        {
            uiChTimer = 5000;
            uiAtTimer = 10000;
            DoCast(SPELL_AURA_ME);
            me->SetLevitate(true);
            me->SetSpeed(MOVE_RUN, 0.10f);
            switch (urand(0, 3))
            {
            case 0:
                me->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 105.0f);
                break;
            case 1:
                me->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 105.0f);
                break;
            case 2:
                me->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 105.0f);
                break;
            case 3:
                me->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 105.0f);
                break;
            }
        }
    
        void AttackedBy(Unit* /*pWho*/) {}
        void AttackStart(Unit* /*pWho*/) {}
    
        void JustDied(Unit* /*pWho*/)
        {
            me->RemoveCorpse();
        }
    
        void UpdateAI(const uint32 uiDiff)
        {
            if (uiAtTimer <= uiDiff)
            {
                DoCast(SPELL_DARKNESS);
                switch (urand(0, 3))
                {
                case 0:
                    me->GetMotionMaster()->MovePoint(0, -714.212f, 2750.606f, 105.0f);
                    break;
                case 1:
                    me->GetMotionMaster()->MovePoint(0, -713.406f, 2744.240f, 105.0f);
                    break;
                case 2:
                    me->GetMotionMaster()->MovePoint(0, -707.784f, 2749.562f, 105.0f);
                    break;
                case 3:
                    me->GetMotionMaster()->MovePoint(0, -708.558f, 2744.923f, 105.0f);
                    break;
                }
    
                uiAtTimer = 10000;
            }
            else uiAtTimer -= uiDiff;
    
            if (uiChTimer <= uiDiff)
            {
                if (/*Creature* pBar = */me->FindNearestCreature(NPC_BARADA, 15.0f, false))
                    me->setDeathState(CORPSE);
    
                if (Creature* pBara = me->FindNearestCreature(NPC_BARADA, 15.0f, true))
                {
                    if (!pBara->HasAura(SPELL_EXORCIM2, 0))
                        me->setDeathState(CORPSE);
                }
    
                uiChTimer = 5000;
            }
            else uiChTimer -= uiDiff;
        }
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_darkness_releasedAI(pCreature);
    }

    

    
};

class npc_foul_purge : public CreatureScript
{
public: 
    npc_foul_purge() : CreatureScript("npc_foul_purge") { }
    struct npc_foul_purgeAI : public ScriptedAI
    {
        npc_foul_purgeAI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
        uint32 uiChTimer;
    
        void Reset()
        {
            if (Creature* pBara = me->FindNearestCreature(NPC_BARADA, 15.0f, true))
                AttackStart(pBara);
            uiChTimer = 4000;
        }
    
        void JustDied(Unit* /*pWho*/)
        {
            me->RemoveCorpse();
        }
    
        void UpdateAI(const uint32 uiDiff)
        {
            if (uiChTimer <= uiDiff)
            {
                if (/*Creature* pBar = */me->FindNearestCreature(NPC_BARADA, 15.0f, false))
                    me->setDeathState(CORPSE);
    
                if (Creature* pBara = me->FindNearestCreature(NPC_BARADA, 15.0f, true))
                {
                    if (!pBara->HasAura(SPELL_EXORCIM2, 0))
                        me->setDeathState(CORPSE);
                }
    
                uiChTimer = 4000;
            }
            else uiChTimer -= uiDiff;
    
            DoMeleeAttackIfReady();
        }
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_foul_purgeAI(pCreature);
    }

    

    
};

class npc_sedai_quest_credit_marker : public CreatureScript
{
public: 
    npc_sedai_quest_credit_marker() : CreatureScript("npc_sedai_quest_credit_marker") { }
    struct npc_sedai_quest_credit_markerAI : public ScriptedAI
    {
        npc_sedai_quest_credit_markerAI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
        void Reset()
        {
            DoSpawn();
        }
    
        void DoSpawn()
        {
            me->SummonCreature(NPC_SEDAI, 225.908f, 4124.034f, 82.505f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 100000);
            me->SummonCreature(NPC_ESCORT1, 229.257f, 4125.271f, 83.388f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 40000);
        }
    
        void JustSummoned(Creature* pSummoned)
        {
            if (pSummoned->GetEntry() == NPC_ESCORT1)
            {
                pSummoned->SetWalk(true);
                pSummoned->GetMotionMaster()->MovePoint(0, 208.029f, 4134.618f, 77.763f);
            }
        }
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_sedai_quest_credit_markerAI(pCreature);
    }

    

    
};

class npc_demoniac_scryer : public CreatureScript
{
public: 
    npc_demoniac_scryer() : CreatureScript("npc_demoniac_scryer") { }
    struct npc_demoniac_scryerAI : public ScriptedAI
    {
        npc_demoniac_scryerAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            IfIsComplete = false;
            uiSpawnDemonTimer = 15000;
            uiSpawnOrcTimer = 30000;
            uiSpawnButtressTimer = 45000;
            uiEndTimer = 262000;
            uiButtressCount = 0;
            Reset();
        }
    
        bool IfIsComplete;
    
        uint32 uiSpawnDemonTimer;
        uint32 uiSpawnOrcTimer;
        uint32 uiSpawnButtressTimer;
        uint32 uiEndTimer;
        uint32 uiButtressCount;
    
        void Reset() {}
    
        void AttackedBy(Unit* /*pEnemy*/) {}
        void AttackStart(Unit* /*pEnemy*/) {}
    
        void DoSpawnButtress()
        {
            ++uiButtressCount;
    
            float fAngle;
            switch (uiButtressCount)
            {
            case 1:
                fAngle = 0.0f;
                break;
            case 2:
                fAngle = 4.6f;
                break;
            case 3:
                fAngle = 1.5f;
                break;
            case 4:
                fAngle = 3.1f;
                break;
            }
    
            float fX, fY, fZ;
            me->GetNearPoint(me, fX, fY, fZ, 0.0f, 7.0f, fAngle);
            uint32 uiTime = TIME_TOTAL - (uiSpawnButtressTimer * uiButtressCount);
            me->SummonCreature(NPC_BUTTRESS, fX, fY, fZ, me->GetAngle(fX, fY), TEMPSUMMON_TIMED_DESPAWN, uiTime);
            me->SummonCreature(NPC_BUTTRESS_SPAWNER, fX, fY, fZ, me->GetAngle(fX, fY), TEMPSUMMON_TIMED_DESPAWN, uiTime);
        }
    
        void DoSpawnDemon()
        {
            float fX, fY, fZ;
            me->GetNearPoint(me, fX, fY, fZ, 5.0f, 5.0f, -13.0f);
            me->SummonCreature(NPC_HELLFIRE_WARDLING, fX, fY, fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
        }
    
        void DospawnOrc()
        {
            float fX, fY, fZ;
            me->GetNearPoint(me, fX, fY, fZ, 5.0f, 5.0f, -13.0f);
            me->SummonCreature(NPC_ORC_HA, fX, fY, fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
        }
    
        void JustSummoned(Creature* pSummoned)
        {
            if (pSummoned->GetEntry() == NPC_HELLFIRE_WARDLING)
            {
                pSummoned->CastSpell(pSummoned, SPELL_SUMMONED, false);
                pSummoned->AI()->AttackStart(me);
            }
            if (pSummoned->GetEntry() == NPC_ORC_HA)
            {
                pSummoned->CastSpell(pSummoned, SPELL_SUMMONED, false);
                pSummoned->AI()->AttackStart(me);
            }
            if (pSummoned->GetEntry() == NPC_BUTTRESS)
                pSummoned->CastSpell(pSummoned, SPELL_BUTTRESS_APPERANCE, false);
            else
            {
                if (pSummoned->GetEntry() == NPC_BUTTRESS_SPAWNER)
                    pSummoned->CastSpell(me, SPELL_SUCKER_CHANNEL, true);
            }
        }
    
        void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
        {
            if (pTarget->GetEntry() == NPC_BUTTRESS && pSpell->Id == SPELL_SUCKER_DESPAWN_MOB)
                ((Creature*)pTarget)->setDeathState(CORPSE);
            if (pTarget->GetEntry() == NPC_BUTTRESS_SPAWNER && pSpell->Id == SPELL_SUCKER_DESPAWN_MOB)
                ((Creature*)pTarget)->setDeathState(CORPSE);
        }
    
        void UpdateAI(const uint32 uiDiff)
        {
            if (uiEndTimer <= uiDiff)
            {
                me->ForcedDespawn();
                uiEndTimer = 262000;
            }
            else uiEndTimer -= uiDiff;
    
            if (IfIsComplete || !me->IsAlive())
                return;
    
            if (uiSpawnButtressTimer <= uiDiff)
            {
                if (uiButtressCount >= MAX_BUTTRESS)
                {
                    DoCast(SPELL_SUCKER_DESPAWN_MOB);
                    IfIsComplete = true;
                    return;
                }
                uiSpawnButtressTimer = 45000;
                DoSpawnButtress();
            }
            else uiSpawnButtressTimer -= uiDiff;
    
            if (uiSpawnDemonTimer <= uiDiff)
            {
                DoSpawnDemon();
                uiSpawnDemonTimer = 15000;
            }
            else uiSpawnDemonTimer -= uiDiff;
    
            if (uiSpawnOrcTimer <= uiDiff)
            {
                DospawnOrc();
                uiSpawnOrcTimer = 30000;
            }
            else uiSpawnOrcTimer -= uiDiff;
        }
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_demoniac_scryerAI(pCreature);
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature) override
    {
        if (npc_demoniac_scryerAI* pScryerAI = dynamic_cast<npc_demoniac_scryerAI*>(pCreature->AI()))
        {
            if (pScryerAI->IfIsComplete)
            {
                if (pPlayer->GetQuestStatus(QUEST_DEMONIAC) == QUEST_STATUS_INCOMPLETE)
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ATTUNE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ATTUNED, pCreature->GetGUID());
                return true;
            }
        }
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_PROTECT, pCreature->GetGUID());
        return true;
    }
    
    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction) override
    {
        if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        {
            pPlayer->CLOSE_GOSSIP_MENU();
            pCreature->CastSpell(pPlayer, SPELL_DEMONIAC_VISITATION, false);
        }
        return true;
    }
    

    

    
};

class npc_magic_sucker_device_spawner : public CreatureScript
{
public: 
    npc_magic_sucker_device_spawner() : CreatureScript("npc_magic_sucker_device_spawner") { }
    struct npc_magic_sucker_device_spawnerAI : public ScriptedAI
    {
        npc_magic_sucker_device_spawnerAI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
        uint32 uiCastTimer;
        uint32 uiCheckTimer;
    
        void Reset()
        {
            uiCastTimer = 1800;
            uiCheckTimer = 5000;
        }
    
        void UpdateAI(const uint32 uiDiff)
        {
            if (uiCastTimer <= uiDiff)
            {
                DoCast(SPELL_EFFECT);
                uiCastTimer = 1800;
            }
            else uiCastTimer -= uiDiff;
    
            if (uiCheckTimer <= uiDiff)
            {
                if (/*Creature* pScr = */me->FindNearestCreature(NPC_SCRYER, 15.0f, false))
                {
                    if (Creature* pBut = me->FindNearestCreature(NPC_BUTTRES, 5.0f))
                    {
                        pBut->setDeathState(CORPSE);
                        me->setDeathState(CORPSE);
                    }
                }
                else return;
    
                uiCheckTimer = 5000;
            }
            else uiCheckTimer -= uiDiff;
        }
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_magic_sucker_device_spawnerAI(pCreature);
    }

    

    
};

class npc_living_flare : public CreatureScript
{
public: 
    npc_living_flare() : CreatureScript("npc_living_flare") { }
    struct npc_living_flareAI : public ScriptedAI
    {
        npc_living_flareAI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
        uint32 uiCheckTimer;
        uint64 uiSparkGUID;
        uint32 Count;
    
        void Reset()
        {
            DoCast(SPELL_COSMETIC);
            uiCheckTimer = 8000;
            uiSparkGUID = 0;
            Count = 0;
        }
    
        void AttackedBy(Unit* /*pWho*/) {}
        void AttackStart(Unit* /*pWho*/) {}
    
        void UpdateAI(const uint32 uiDiff)
        {
            if (uiCheckTimer <= uiDiff)
            {
                if (Creature* pSpark = me->FindNearestCreature(NPC_FEL_SPARK, 10.0f, false))
                {
                    if (pSpark->GetGUID() != uiSparkGUID && CAST_PLR(me->GetOwner())->GetQuestStatus(11516) == QUEST_STATUS_INCOMPLETE)
                    {
                        if (Count <= 7)
                        {
                            ++Count;
                            DoCast(SPELL_ABSORBED);
                            uiSparkGUID = pSpark->GetGUID();
                        }
                        else DoCast(SPELL_UNSTABLE_COSMETIC);
                    }
                }
    
                if (Creature* pTrigger = me->FindNearestCreature(NPC_TRIGGER, 8.0f, true))
                {
                    pTrigger->SummonGameObject(GO_FIRE, pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 60);
                    CAST_PLR(me->GetOwner())->AreaExploredOrEventHappens(11516);
                    me->setDeathState(CORPSE);
                }
            }
            else uiCheckTimer -= uiDiff;
        }
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_living_flareAI(pCreature);
    }

    

    
};

class npc_pathaleon_image : public CreatureScript
{
public: 
    npc_pathaleon_image() : CreatureScript("npc_pathaleon_image") { }
    struct npc_pathaleon_imageAI : public ScriptedAI
    {
        npc_pathaleon_imageAI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
        bool Event;
        bool SummonTrigger;
    
        uint32 uiSumTimer;
        uint32 uiStepsTimer;
        uint32 uiSteps;
    
        void Reset()
        {
            uiSumTimer = 5000;
            uiStepsTimer = 0;
            uiSteps = 0;
            Event = true;
            SummonTrigger = false;
        }
    
        void DoSpawnGoliathon()
        {
            me->SummonCreature(NPC_GOLIATHON, 113.29f, 4858.19f, 74.37f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
        }
    
        void DoSpawnTrigger()
        {
            me->SummonCreature(NPC_TARGET_TRIGGER, 81.20f, 4806.26f, 51.75f, 2.0f, TEMPSUMMON_TIMED_DESPAWN, 120000);
        }
    
        void DoSpawnCtrigger()
        {
            me->SummonCreature(NPC_CRYSTAL_TRIGGER, 106.21f, 4834.39f, 79.56f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 7000);
            me->SummonCreature(NPC_CRYSTAL_TRIGGER, 124.98f, 4813.17f, 79.66f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 7000);
            me->SummonCreature(NPC_CRYSTAL_TRIGGER, 124.01f, 4778.61f, 77.86f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 7000);
            me->SummonCreature(NPC_CRYSTAL_TRIGGER, 46.37f, 4795.72f, 66.73f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 7000);
            me->SummonCreature(NPC_CRYSTAL_TRIGGER, 60.14f, 4830.46f, 77.83f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 7000);
        }
    
        void JustSummoned(Creature* pSummoned)
        {
            if (pSummoned->GetEntry() == NPC_GOLIATHON)
            {
                pSummoned->CastSpell(pSummoned, SPELL_TELE, false);
                pSummoned->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
            }
            if (pSummoned->GetEntry() == NPC_CRYSTAL_TRIGGER)
            {
                pSummoned->CastSpell(pSummoned, SPELL_INSECT, false);
                pSummoned->CastSpell(pSummoned, SPELL_LIGHTING, false);
            }
            else
            {
                if (pSummoned->GetEntry() == NPC_TARGET_TRIGGER)
                    pSummoned->CastSpell(pSummoned, SPELL_ROOTS, false);
            }
        }
    
        int32 NextStep(uint32 uiSteps)
        {
            switch (uiSteps)
            {
            case 1:
                return 10000;
            case 2:
                DoSpawnTrigger();
                SummonTrigger = true;
                return 2000;
            case 3:
                DoScriptText(SAY_PATHALEON1, me, 0);
                return 15000;
            case 4:
                DoScriptText(SAY_PATHALEON2, me, 0);
                return 15000;
            case 5:
                DoScriptText(SAY_PATHALEON3, me, 0);
                return 15000;
            case 6:
                DoScriptText(SAY_PATHALEON4, me, 0);
                return 5000;
            case 7:
                DoSpawnGoliathon();
                return 1000;
            case 8:
                DoCast(SPELL_TELE);
                return 500;
            case 9:
                me->SetVisible(false);
                return 60000;
            case 10:
                me->setDeathState(CORPSE);
            default:
                return 0;
            }
        }
    
        void UpdateAI(const uint32 uiDiff)
        {
            if (uiStepsTimer <= uiDiff)
            {
                if (Event)
                    uiStepsTimer = NextStep(++uiSteps);
            }
            else uiStepsTimer -= uiDiff;
    
            if (SummonTrigger)
            {
                if (uiSumTimer <= uiDiff)
                {
                    DoSpawnCtrigger();
                    uiSumTimer = 5000;
                }
                else uiSumTimer -= uiDiff;
            }
        }
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_pathaleon_imageAI(pCreature);
    }

    

    
};

class npc_fel_guard_hound : public CreatureScript
{
public: 
    npc_fel_guard_hound() : CreatureScript("npc_fel_guard_hound") { }
    struct npc_fel_guard_houndAI : public ScriptedAI
    {
        npc_fel_guard_houndAI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
        uint32 uiCheckTimer;
        uint64 uiHelboarGUID;
    
        void Reset()
        {
            uiCheckTimer = 5000; //check for creature every 5 sec
            uiHelboarGUID = 0;
        }
    
        void MovementInform(uint32 uiType, uint32 uiId)
        {
            if (uiType != POINT_MOTION_TYPE || uiId != 1)
                return;
    
            if (Creature* pHelboar = me->GetCreature(*me, uiHelboarGUID))
            {
                pHelboar->RemoveCorpse();
                DoCast(SPELL_SUMMON_POO);
    
                if (Player* pOwner = me->GetCharmerOrOwnerPlayerOrPlayerItself())
                    me->GetMotionMaster()->MoveFollow(pOwner, 0.0f, 0.0f);
            }
        }
    
        void UpdateAI(const uint32 uiDiff)
        {
            if (uiCheckTimer <= uiDiff)
            {
                if (Creature* pHelboar = me->FindNearestCreature(NPC_DERANGED_HELBOAR, 10.0f, false))
                {
                    if (pHelboar->GetGUID() != uiHelboarGUID && me->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE && !me->FindCurrentSpellBySpellId(SPELL_SUMMON_POO))
                    {
                        uiHelboarGUID = pHelboar->GetGUID();
                        me->GetMotionMaster()->MovePoint(1, pHelboar->GetPositionX(), pHelboar->GetPositionY(), pHelboar->GetPositionZ());
                    }
                }
                uiCheckTimer = 5000;
            }
            else uiCheckTimer -= uiDiff;
    
            if (!UpdateVictim())
                return;
    
            DoMeleeAttackIfReady();
        }
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_fel_guard_houndAI(pCreature);
    }

    

    
};

class npc_arrazius_the_cruel : public CreatureScript
{
public: 
    npc_arrazius_the_cruel() : CreatureScript("npc_arrazius_the_cruel") { }
    struct npc_arrazius_the_cruelAI : public ScriptedAI
    {
    	npc_arrazius_the_cruelAI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
    	uint32 infernoTimer;
    	uint32 pyroblastTimer;
    	uint32 shadowboltTimer;
    	uint32 chillingTimer;
    	uint32 doubtingTimer;
    	uint32 feebleTimer;
    	uint32 arraziusTimer;
    
    	void Reset()
    	{
    		infernoTimer = 5000;
    		pyroblastTimer = 8000;
    		shadowboltTimer = 3000;
    		chillingTimer = 1500;
    		feebleTimer = 1000;
    		doubtingTimer = 2000;
    		arraziusTimer = 0;
    
    		me->ApplySpellImmune(0, IMMUNITY_ID, 18223, true);
    		me->ApplySpellImmune(0, IMMUNITY_ID, 29539, true);
    		me->ApplySpellImmune(0, IMMUNITY_ID, 46434, true);
    	}
    
    	void EnterCombat(Unit* /*who*/)
    	{
    		DoScriptText(RAND(SAY_AGGRO1, SAY_AGGRO2), me);
    	}
    
    	void KilledUnit(Unit* /*victim*/)
    	{
    		DoScriptText(SAY_SLAY1, me);
    	}
    
    	void JustDied(Unit* /*killer*/)
    	{
    		DoScriptText(SAY_DIE1, me);
    	}
    
    	void UpdateAI(const uint32 uiDiff)
    	{
    		if (infernoTimer <= uiDiff)
    		{
    			DoCast(SPELL_INFERNO);
    			infernoTimer = 20000;
    		}
    		else infernoTimer -= uiDiff;
    
    		if (pyroblastTimer <= uiDiff)
    		{
    			DoCastVictim(SPELL_PYROBLAST);
    			pyroblastTimer = 10000;
    		}
    		else pyroblastTimer -= uiDiff;
    
    		if (shadowboltTimer <= uiDiff)
    		{
    			DoCast(SPELL_SHADOWBOLT_VOLLEY);
    			shadowboltTimer = 4000;
    		}
    		else shadowboltTimer -= uiDiff;
    
    		if (arraziusTimer <= uiDiff)
    		{
    			DoCast(SPELL_ARAZIUS_POWER);
    			arraziusTimer = 30000;
    		}
    		else arraziusTimer -= uiDiff;
    
    		if (chillingTimer <= uiDiff)
    		{
    			if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 4, 100, true))
    				DoCast(pTarget, SPELL_CHILLING_WORDS);
    			chillingTimer = 13000;
    		}
    		else chillingTimer -= uiDiff;
    
    		if (doubtingTimer <= uiDiff)
    		{
    			if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
    				DoCast(pTarget, SPELL_DOUBTING_MIND);
    			doubtingTimer = 13500;
    		}
    		else doubtingTimer -= uiDiff;
    
    		if (feebleTimer <= uiDiff)
    		{
    			if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
    				DoCast(pTarget, SPELL_FEEBLE_WEAPONS);
    			feebleTimer = 14000;
    		}
    		else feebleTimer -= uiDiff;
    
    		DoMeleeAttackIfReady();
    	}
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
    	return new npc_arrazius_the_cruelAI(pCreature);
    }

    

    
};

class npc_fel_cannon : public CreatureScript
{
public: 
    npc_fel_cannon() : CreatureScript("npc_fel_cannon") { }
    struct npc_fel_cannonAI : public ScriptedAI
    {
    	npc_fel_cannonAI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
    	uint32 blastTimer;
    	uint32 shootTimer;
    
    	void Reset()
    	{
    		blastTimer = 8000;
    		shootTimer = 2000;
    
    		SetCombatMovement(false);
    	}
    
    	void EnterCombat(Unit* /*who*/)
    	{		
    	}
    
    	void UpdateAI(const uint32 uiDiff)
    	{
    		if (!UpdateVictim())
    		{
    			if (blastTimer <= uiDiff)
    			{
    				if (Unit* target = me->FindNearestCreature(32996, 300, true))
    				DoCast(target, SPELL_COSMETIC_IMPACT_FIRE);
    				blastTimer = 8000;
    			}
    			else blastTimer -= uiDiff;
    		}
    
    		if (shootTimer <= uiDiff)
    		{
    			DoCastVictim(SPELL_FEL_CANNON_BLAST);
    			shootTimer = 5000;
    		}
    		else shootTimer -= uiDiff;
    
    		DoMeleeAttackIfReady();
    	}
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
    	return new npc_fel_cannonAI(pCreature);
    }

    

    
};

class npc_maghar_grunt : public CreatureScript
{
public: 
    npc_maghar_grunt() : CreatureScript("npc_maghar_grunt") { }
    struct npc_maghar_gruntAI : public ScriptedAI
    {
    	npc_maghar_gruntAI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
    	void Reset() 
    	{
    		spellHit = false;
    		lifeTimer = 120000;
    		tTimer = 3000;
    
    		me->SetStandState(UNIT_STAND_STATE_KNEEL);
    
    		if (me->GetEntry() == NPC_MAGHAR)
    			me->UpdateEntry(NPC_INJURED_MAGHAR);
    	}
    
    	bool spellHit;
    	uint32 lifeTimer;
    	uint32 tTimer;
    
    	void EnterCombat(Unit* /*who*/)
    	{
    	}
    
    	void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)
    	{
    		if (Spellkind->Id == 29314 && !spellHit)
    		{		
    			me->hasInvolvedQuest(9447);
    			me->SetStandState(UNIT_STAND_STATE_STAND);				
    			DoScriptText(SAY_THANKS, me);
    			spellHit = true;			
    		}
    	}
    
    	void UpdateAI(const uint32 uiDiff)
    	{
    		if (!UpdateVictim())
    		{
    			if (me->GetEntry() == NPC_MAGHAR)
    			{
    				if (lifeTimer <= uiDiff)
    				{
    					me->UpdateEntry(NPC_INJURED_MAGHAR);
    					EnterEvadeMode();
    					return;
    				}
    				else
    					lifeTimer -= uiDiff;							
    			}
    
    			if (spellHit == true)
    			{
    				if (tTimer <= uiDiff)
    				{
    					me->UpdateEntry(NPC_MAGHAR);
    					tTimer = 3000;
    				}
    				else tTimer -= uiDiff;
    			}
    		}
    
    		DoMeleeAttackIfReady();
    	}
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
    	return new npc_maghar_gruntAI(pCreature);
    }

    

    
};

class npc_dreghood_brute : public CreatureScript
{
public: 
    npc_dreghood_brute() : CreatureScript("npc_dreghood_brute") { }
    struct npc_dreghood_bruteAI : public ScriptedAI
    {
    	npc_dreghood_bruteAI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
    	void Reset()
    	{
    		hamstring_timer = 1000;
    		flee_timer = 4000;
    
    		me->SetFaction(90);
    		flee = false;
    		say = false;
    	}
    	
    	uint32 hamstring_timer;
    	uint32 flee_timer;
    
    	bool flee;
    	bool say;
    
    	void EnterCombat(Unit* /*who*/) 
    	{
    		switch (rand() % 2)
    		{
    		case 0:
    			DoScriptText(SAY_SORRY, me);
    			break;
    		case 1:
    			DoScriptText(SAY_SORRY2, me);
    			break;
    		}
    	}
    
    	void UpdateAI(const uint32 uiDiff)
    	{
    		if (!UpdateVictim())
    		{			
    			if (flee == true)
    			{
    				me->GetMotionMaster()->MovePoint(1, FleePath, true);
    				
    				if (!say)
    				{
    					DoScriptText(SAY_RUN, me);
    					say = true;
    				}
    
    				if (flee_timer <= uiDiff)
    				{
    					me->DisappearAndDie(false);
    					flee_timer = 4000;
    				}
    				else flee_timer -= uiDiff;
    			}
    		}
    
    		if (hamstring_timer <= uiDiff)
    		{
    			DoCastVictim(SPELL_HAMSTRING);
    			hamstring_timer = 12000;
    		}
    		else hamstring_timer -= uiDiff;
    
    		if (Creature* taskmaster = me->FindNearestCreature(NPC_TASKMASTER, 25, false))
    		{
    			me->SetFaction(35);
    			me->CombatStop();
    			flee = true;
    		}
    	
    		DoMeleeAttackIfReady();
    	}
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
    	return new npc_dreghood_bruteAI(pCreature);
    }

    

    
};

class npc_dreghood_elder1 : public CreatureScript
{
public: 
    npc_dreghood_elder1() : CreatureScript("npc_dreghood_elder1") { }
    struct npc_dreghood_elder1AI : public ScriptedAI
    {
    	npc_dreghood_elder1AI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
    	void Reset() 
    	{
    		lifeTimer = 30000;
    
    		me->SetStandState(UNIT_STAND_STATE_KNEEL);
    	}
    
    	uint32 lifeTimer;
    
    	void EnterCombat(Unit* /*who*/) { }
    
    	void UpdateAI(const uint32 uiDiff)
    	{
    		if (!UpdateVictim())
    		{
    			if (me->IsStandState())
    			{
    				if (lifeTimer <= uiDiff)
    				{
    					EnterEvadeMode();
    					return;
    				}
    				else
    					lifeTimer -= uiDiff;
    			}
    		}
    
    		DoMeleeAttackIfReady();
    	}
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
    	return new npc_dreghood_elder1AI(pCreature);
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature) override
    {
    
    	if (pCreature->IsQuestGiver())
    		pPlayer->PrepareQuestMenu(pCreature->GetGUID());
    
    	if (pPlayer->GetQuestStatus(10368) == QUEST_STATUS_INCOMPLETE)
    	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    
    	pPlayer->SEND_GOSSIP_MENU(10103, pCreature->GetGUID());
    
    	return true;
    }  

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction) override
    {
    	switch (uiAction)
    	{
    	case GOSSIP_ACTION_INFO_DEF + 1:
    		pPlayer->CLOSE_GOSSIP_MENU();
    
    		DoScriptText(ELDER_SAY_1, pCreature, pPlayer);
    		pPlayer->TalkedToCreature(20679, 86006);
    		pCreature->SetStandState(UNIT_STAND_STATE_STAND);
    		break;
    	}
    	return true;
    }
    

    

    
};

class npc_dreghood_elder2 : public CreatureScript
{
public: 
    npc_dreghood_elder2() : CreatureScript("npc_dreghood_elder2") { }
    struct npc_dreghood_elder2AI : public ScriptedAI
    {
    	npc_dreghood_elder2AI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
    	void Reset()
    	{
    		lifeTimer = 30000;
    
    		me->SetStandState(UNIT_STAND_STATE_KNEEL);
    	}
    
    	uint32 lifeTimer;
    
    	void EnterCombat(Unit* /*who*/) { }
    
    	void UpdateAI(const uint32 uiDiff)
    	{
    		if (!UpdateVictim())
    		{
    			if (me->IsStandState())
    			{
    				if (lifeTimer <= uiDiff)
    				{
    					EnterEvadeMode();
    					return;
    				}
    				else
    					lifeTimer -= uiDiff;
    			}
    		}
    
    		DoMeleeAttackIfReady();
    	}
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
    	return new npc_dreghood_elder2AI(pCreature);
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature) override
    {
    	if (pCreature->IsQuestGiver())
    		pPlayer->PrepareQuestMenu(pCreature->GetGUID());
    
    	if (pPlayer->GetQuestStatus(10368) == QUEST_STATUS_INCOMPLETE)
    	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    
    	pPlayer->SEND_GOSSIP_MENU(10104, pCreature->GetGUID());
    
    	return true;
    }

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction) override
    {
    	switch (uiAction)
    	{
    	case GOSSIP_ACTION_INFO_DEF + 1:
    		pPlayer->CLOSE_GOSSIP_MENU();
    		pPlayer->TalkedToCreature(20678, 86008);
    		DoScriptText(ELDER_SAY_2, pCreature, pPlayer);
    		pCreature->SetStandState(UNIT_STAND_STATE_STAND);
    		break;
    	}
    	return true;
    }
    

    

    
};

class npc_dreghood_elder3 : public CreatureScript
{
public: 
    npc_dreghood_elder3() : CreatureScript("npc_dreghood_elder3") { }
    struct npc_dreghood_elder3AI : public ScriptedAI
    {
    	npc_dreghood_elder3AI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
    	void Reset()
    	{
    		lifeTimer = 30000;
    
    		me->SetStandState(UNIT_STAND_STATE_KNEEL);
    	}
    
    	uint32 lifeTimer;
    
    	void EnterCombat(Unit* /*who*/) { }
    
    	void UpdateAI(const uint32 uiDiff)
    	{
    		if (!UpdateVictim())
    		{
    			if (me->IsStandState())
    			{
    				if (lifeTimer <= uiDiff)
    				{
    					EnterEvadeMode();
    					return;
    				}
    				else
    					lifeTimer -= uiDiff;
    			}
    		}
    
    		DoMeleeAttackIfReady();
    	}
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
    	return new npc_dreghood_elder3AI(pCreature);
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature) override
    {
    	if (pCreature->IsQuestGiver())
    		pPlayer->PrepareQuestMenu(pCreature->GetGUID());
    
    	if (pPlayer->GetQuestStatus(10368) == QUEST_STATUS_INCOMPLETE)
    	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    
    	pPlayer->SEND_GOSSIP_MENU(10105, pCreature->GetGUID());
    
    	return true;
    }
   
    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction) override
    {
    	switch (uiAction)
    	{
    	case GOSSIP_ACTION_INFO_DEF + 1:
    		pPlayer->CLOSE_GOSSIP_MENU();
    
    		DoScriptText(ELDER_SAY_3, pCreature, pPlayer);
    		pPlayer->TalkedToCreature(20677, 86007);
    		pCreature->SetStandState(UNIT_STAND_STATE_STAND);
    		break;
    	}
    	return true;
    }
    

    

    
};

class npc_viera : public CreatureScript
{
public: 
    npc_viera() : CreatureScript("npc_viera") { }
    struct npc_vieraAI : public npc_escortAI
    {
    	npc_vieraAI(Creature* c) : npc_escortAI(c)
    	{
    		me->setActive(true);
    	}
    
    	uint64 catGUID;
    
    	bool spellHit;
    
    	void Reset()
    	{
    		if (HasEscortState(STATE_ESCORT_ESCORTING))
    			return;
    
    		catGUID = 0;
    		
    		spellHit = false;
    	}
    
    	void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)
    	{
    		if (Spellkind->Id == 30077 && !spellHit)
    		{
    			me->hasInvolvedQuest(9472);
    			DoScriptText(VIERA_SAY2, me);
    			SetEscortPaused(false);
    			spellHit = true;
    		}
    	}
    
    	void WaypointReached(uint32 i)
    	{
    		if (Creature* kitty = me->FindNearestCreature(NPC_CAT, 40.0f, true))
    			catGUID = kitty->GetGUID();
    
    		switch (i)
    		{
    		case 0:
    			me->SetStandState(UNIT_STAND_STATE_STAND);
    			break;
    		case 1:
    			me->SummonCreature(NPC_CAT, -654.033f, 4135.679f, 64.625f, 2.11f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 180000);			
    			break;
    		case 2:		
    			if (Creature* kitty = me->FindNearestCreature(NPC_CAT, 40.0f, true))
    			{
    				kitty->GetMotionMaster()->MovePoint(0, ChasePath1, true);
    			}
    			break;
    		case 3:
    			if (Creature* kitty = me->FindNearestCreature(NPC_CAT, 40.0f, true))
    			{
    				kitty->GetMotionMaster()->MovePoint(1, ChasePath2, true);
    			}
    			break;
    		case 4:
    			if (Creature* kitty = me->FindNearestCreature(NPC_CAT, 40.0f, true))
    			{
    				kitty->GetMotionMaster()->MovePoint(2, ChasePath3, true);
    			}
    			break;
    		case 5:
    			if (Creature* kitty = me->FindNearestCreature(NPC_CAT, 40.0f, true))
    			{
    				kitty->GetMotionMaster()->MovePoint(3, ChasePath4, true);
    			}
    			break;
    		case 6:
    			if (Creature* kitty = me->FindNearestCreature(NPC_CAT, 40.0f, true))
    			{
    				kitty->GetMotionMaster()->MovePoint(4, ChasePath5, true);
    			}
    			break;
    		case 7:
    			if (Creature* kitty = me->FindNearestCreature(NPC_CAT, 40.0f, true))
    			{
    				kitty->GetMotionMaster()->MovePoint(5, ChasePath6, true);
    			}
    			break;
    		case 8:
    			if (Creature* kitty = me->FindNearestCreature(NPC_CAT, 40.0f, true))
    			{
    				kitty->GetMotionMaster()->MovePoint(6, ChasePath7, true);
    			}
    			break;
    		case 9:
    			if (Creature* kitty = me->FindNearestCreature(NPC_CAT, 40.0f, true))
    			{
    				kitty->GetMotionMaster()->MovePoint(7, ChasePath8, true);
    			}
    
    			DoScriptText(VIERA_SAY1, me);
    			SetEscortPaused(true);
    			break;
    		case 10:
    			if (Creature* kitty = me->FindNearestCreature(NPC_CAT, 40.0f, true))
    			{
    				DoScriptText(SAY_CAT_1, kitty);
    				kitty->GetMotionMaster()->MoveFollow(me, 2.0f, 0);
    			}
    			break;
    		case 12:
    			if (Creature* kitty = me->FindNearestCreature(NPC_CAT, 40.0f, true))
    			{
    				me->DisappearAndDie(false);
    				kitty->DisappearAndDie(false);
    			}
    			break;
    		}
    	}
    
    	void UpdateAI(const uint32 diff)
    	{
    		npc_escortAI::UpdateAI(diff);
    	}
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
    	npc_vieraAI* vieraAI = new npc_vieraAI(pCreature);
    
    	vieraAI->AddWaypoint(0, -655.107f, 4147.2f, 64.1146f, 2000);
    	vieraAI->AddWaypoint(1, -655.107f, 4147.2f, 64.1146f, 0);
    	vieraAI->AddWaypoint(2, -664.461f, 4147.91f, 64.156f, 0);
    	vieraAI->AddWaypoint(3, -681.972f, 4146.48f, 64.4093f, 0);
    	vieraAI->AddWaypoint(4, -684.262f, 4154.27f, 62.1889f, 0);
    	vieraAI->AddWaypoint(5, -689.216f, 4171.12f, 58.0475f, 0);
    	vieraAI->AddWaypoint(6, -693.5f, 4185.7f, 57.0529f, 0);
    	vieraAI->AddWaypoint(7, -708.753f, 4187.82f, 55.1475f, 0);
    	vieraAI->AddWaypoint(8, -721.51f, 4189.59f, 51.8167f, 0); 
    	vieraAI->AddWaypoint(9, -721.682f, 4170.45f, 50.7466f, 0); // end of the road
    	vieraAI->AddWaypoint(10, -721.682f, 4170.45f, 50.7466f, 4000);
    	vieraAI->AddWaypoint(11, -742.759f, 4073.034f, 47.413f, 0);
    	vieraAI->AddWaypoint(12, -742.759f, 4073.034f, 47.413f, 0);
    	vieraAI->AddWaypoint(13, -742.759f, 4073.034f, 47.413f, 0);
    
    	return vieraAI;
    }

    bool OnQuestReward(Player* pPlayer, Creature* pCreature, const Quest* _Quest, uint32 /*item*/) override
    {
    	if (_Quest->GetQuestId() == QUEST_ARELION_MISTRESS_SIDEQ)
    	{
    		{
    			DoScriptText(SAY_GOT_BEER, pCreature);	
    			if (npc_escortAI* pEscortAI = CAST_AI(npc_vieraAI, pCreature->AI()))
    				pEscortAI->Start(false, false, pPlayer->GetGUID());
    		}
    	}
    
    	return true;
    }
    

    

    
};

class npc_felblood_initiate : public CreatureScript
{
public: 
    npc_felblood_initiate() : CreatureScript("npc_felblood_initiate") { }
    struct npc_felblood_initiateAI : public ScriptedAI
    {
    	npc_felblood_initiateAI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
    	void Reset()
    	{
    		spellbreakerTimer = urand(8000, 12000);
    		bitterwithdrawalTimer = urand(17000, 22000);
    		sinisterstrikeTimer = urand(4000, 6000);
    		say_timer = urand(1000, 180000);
    		resetTimer = 190000;
    
    		spellHit = false;
    		say = false;
    
    		me->SetStandState(UNIT_STAND_STATE_STAND);
    
    		if (me->GetEntry() == 24955)
    			me->UpdateEntry(22979);
    	}
    
    	uint32 spellbreakerTimer;
    	uint32 bitterwithdrawalTimer;
    	uint32 sinisterstrikeTimer;
    	uint32 say_timer;
    	uint32 resetTimer;
    
    	bool spellHit;
    	bool say;
    
    	void EnterCombat(Unit* /*who*/) { }
    
    	void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)
    	{
    		if (Spellkind->Id == 44937 && !spellHit)
    		{
    			me->UpdateEntry(24955);
    			me->hasInvolvedQuest(11515);
    			spellHit = true;
    		}
    	}
    
    	void UpdateAI(const uint32 uiDiff)
    	{
    		if (!UpdateVictim())
    		{
    			if (Creature* demon = me->FindNearestCreature(24933, 28.0f, true))
    			{
    				DoCast(demon, SPELL_FELCHANNEL);
    			}
    
    			if (say_timer <= uiDiff)
    			{
    				switch (rand() % 5)
    				{
    				case 0:
    					DoScriptText(SAY_FEL1, me);
    					break;
    				case 1:
    					DoScriptText(SAY_FEL2, me);
    					break;
    				case 2:
    					DoScriptText(SAY_FEL3, me);
    					break;
    				case 3:
    					DoScriptText(SAY_FEL4, me);
    					break;
    				case 4:
    					DoScriptText(SAY_FEL5, me);
    					break;
    				}
    				say_timer = urand(1000, 180000);
    				say = true;
    			}
    			else say_timer -= uiDiff;
    
    			if (resetTimer <= uiDiff)
    			{
    				Reset();
    			}
    			else resetTimer -= uiDiff;
    		}
    
    		if (UpdateVictim())
    		{
    
    			if (spellbreakerTimer <= uiDiff)
    			{
    				DoCastVictim(SPELL_SPELLBREAKER);
    				spellbreakerTimer = urand(18000, 22000);
    			}
    			spellbreakerTimer -= uiDiff;
    
    			if (sinisterstrikeTimer <= uiDiff)
    			{
    				DoCastVictim(SPELL_SINISTER_STRIKE);
    				sinisterstrikeTimer = urand(7000, 9000);
    			}
    			sinisterstrikeTimer -= uiDiff;
    
    			if (bitterwithdrawalTimer <= uiDiff)
    			{
    				DoCastVictim(SPELL_BITTER_WITHDRAWAL);
    				bitterwithdrawalTimer = urand(20000, 26000);
    			}
    			bitterwithdrawalTimer -= uiDiff;
    		}
    
    		DoMeleeAttackIfReady();
    	}
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
    	return new npc_felblood_initiateAI(pCreature);
    }

    

    
};

class npc_felspark : public CreatureScript
{
public: 
    npc_felspark() : CreatureScript("npc_felspark") { }
    struct npc_felsparkAI : public ScriptedAI
    {
    	npc_felsparkAI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
    	void Reset()
    	{
    		felfireballTimer = urand(3000, 6000);
    
    		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
    	}
    
    	uint32 felfireballTimer;
    
    	void EnterCombat(Unit* /*who*/) { }
    
    	void UpdateAI(const uint32 uiDiff)
    	{
    		if (!UpdateVictim())
    			return;
    			
    		if (felfireballTimer <= uiDiff)
    		{
    			DoCastVictim(39058);
    			felfireballTimer = urand(9000, 12000);
    		}
    		felfireballTimer -= uiDiff;
    
    		DoMeleeAttackIfReady();
    	}
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
    	return new npc_felsparkAI(pCreature);
    }

    

    
};

class npc_camerashaker : public CreatureScript
{
public: 
    npc_camerashaker() : CreatureScript("npc_camerashaker") { }
    struct npc_camerashakerAI : public ScriptedAI
    {
    	npc_camerashakerAI(Creature* pCreature) : ScriptedAI(pCreature) {}
    
    	void Reset()
    	{
    		explosiveGUID = 0;
    		explo_timer = 5000;
    	}
    
    	uint64 explosiveGUID;
    	uint32 explo_timer;
    
    	void UpdateAI(const uint32 uiDiff)
    	{
    		if (!UpdateVictim())
    		{
    			if (GameObject* explosive = me->FindNearestGameObject(183410, 30.0f))
    			{
    				if (explo_timer <= uiDiff)
    				{
    					explosiveGUID = explosive->GetGUID();
    					explosive->SetGoState(GO_STATE_ACTIVE);
    					explo_timer = 5000;
    				}
    				else explo_timer -= uiDiff;
    			}
    		}
    	}
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
    	return new npc_camerashakerAI(pCreature);
    }

    

    
};

class npc_east_hovel : public CreatureScript
{
public: 
    npc_east_hovel() : CreatureScript("npc_east_hovel") { }
    struct npc_east_hovelAI : public ScriptedAI
    {
    	npc_east_hovelAI(Creature* creature) : ScriptedAI(creature) {}
    
    	bool Summon;
    	uint32 ResetTimer;
    	void Reset()
    	{
    		Summon = true;
    	}
    
    	void SpellHit(Unit* caster, const SpellEntry* spell)
    	{
    		if (spell->Id == SPELL_FIRE)
    		{
    			if (Summon)
    			{
    				me->SummonGameObject(GO_Z_FIRE, -934.393005f, 1934.01001f, 82.031601f, 3.35103f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -927.877991f, 1927.44995f, 81.048897f, 5.25344f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -935.54303f, 1921.160034f, 82.4132f, 2.67035f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -944.015015f, 1928.160034f, 82.105499f, 5.98648f, 0, 0, 0, 0, 15);
    				ResetTimer = 15000;
    				Summon = false;
    			}
    		}
    	}
    
    	void UpdateAI(const uint32 diff)
    	{
    		if (!Summon)
    		{
    			if (ResetTimer <= diff)
    			{
    				Summon = true;
    			}
    			else ResetTimer -= diff;
    		}
    	}
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
    	return new npc_east_hovelAI(pCreature);
    }

    

    
};

class npc_west_hovel : public CreatureScript
{
public: 
    npc_west_hovel() : CreatureScript("npc_west_hovel") { }
    struct npc_west_hovelAI : public ScriptedAI
    {
    	npc_west_hovelAI(Creature* creature) : ScriptedAI(creature) {}
    
    	bool Summon;
    	uint32 ResetTimer;
    	void Reset()
    	{
    		Summon = true;
    	}
    
    	void SpellHit(Unit* caster, const SpellEntry* spell)
    	{
    		if (spell->Id == SPELL_FIRE)
    		{
    			if (Summon)
    			{
    				me->SummonGameObject(GO_Z_FIRE, -1145.410034f, 2064.830078f, 80.782600f, 5.044f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -1156.839966f, 2060.870117f, 79.176399f, 3.83972f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -1152.719971f, 2073.5f, 80.622902f, 2.00713f, 0, 0, 0, 0, 15);
    				ResetTimer = 15000;
    				Summon = false;
    			}
    		}
    	}
    
    	void UpdateAI(const uint32 diff)
    	{
    		if (!Summon)
    		{
    			if (ResetTimer <= diff)
    			{
    				Summon = true;
    			}
    			else ResetTimer -= diff;
    		}
    	}
    };

    

     CreatureAI* GetAI(Creature* creature) const
    {
    	return new npc_west_hovelAI(creature);
    }

    

    
};

class npc_stable : public CreatureScript
{
public: 
    npc_stable() : CreatureScript("npc_stable") { }
    struct npc_stableAI : public ScriptedAI
    {
    	npc_stableAI(Creature* creature) : ScriptedAI(creature) {}
    
    	bool Summon;
    	uint32 ResetTimer;
    	void Reset()
    	{
    		Summon = true;
    	}
    
    	void SpellHit(Unit* caster, const SpellEntry* spell)
    	{
    		if (spell->Id == SPELL_FIRE)
    		{
    			if (Summon)
    			{
    				me->SummonGameObject(GO_Z_FIRE, -1067.280029f, 1998.949951f, 76.286301f, 5.86431f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -1052.189941f, 2012.099976f, 80.946198f, 5.95157f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -1043.439941f, 2002.140015f, 76.030502f, 2.00713f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -1052.26001f, 1996.339966f, 79.377502f, 0.628319f, 0, 0, 0, 0, 15);
    				ResetTimer = 15000;
    				Summon = false;
    			}
    		}
    	}
    
    	void UpdateAI(const uint32 diff)
    	{
    		if (!Summon)
    		{
    			if (ResetTimer <= diff)
    			{
    				Summon = true;
    			}
    			else ResetTimer -= diff;
    		}
    	}
    };

    

     CreatureAI* GetAI(Creature* creature) const
    {
    	return new npc_stableAI(creature);
    }

    

    
};

class npc_barracks : public CreatureScript
{
public: 
    npc_barracks() : CreatureScript("npc_barracks") { }
    struct npc_barracksAI : public ScriptedAI
    {
    	npc_barracksAI(Creature* creature) : ScriptedAI(creature) {}
    
    	bool Summon;
    	uint32 ResetTimer;
    	void Reset()
    	{
    		Summon = true;
    	}
    
    	void SpellHit(Unit* caster, const SpellEntry* spell)
    	{
    		if (spell->Id == SPELL_FIRE)
    		{
    			if (Summon)
    			{
    				me->SummonGameObject(GO_Z_FIRE, -1176.709961f, 1972.189941f, 107.182999f, 5.18363f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -1120.219971f, 1929.890015f, 92.360901f, 0.89011f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -1137.099976f, 1951.25f, 94.115898f, 2.32129f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -1152.890015f, 1961.48999f, 92.9795f, 0.994838f, 0, 0, 0, 0, 15);
    				ResetTimer = 15000;
    				Summon = false;
    			}
    		}
    	}
    
    	void UpdateAI(const uint32 diff)
    	{
    		if (!Summon)
    		{
    			if (ResetTimer <= diff)
    			{
    				Summon = true;
    			}
    			else ResetTimer -= diff;
    		}
    	}
    };

    

     CreatureAI* GetAI(Creature* creature) const
    {
    	return new npc_barracksAI(creature);
    }

    

    
};

class npc_tower_forge : public CreatureScript
{
public: 
    npc_tower_forge() : CreatureScript("npc_tower_forge") { }
    struct npc_tower_forgeAI : public ScriptedAI
    {
    	npc_tower_forgeAI(Creature* creature) : ScriptedAI(creature) {}
    
    	bool Summon;
    	uint32 ResetTimer;
    	void Reset()
    	{
    		Summon = true;
    	}
    
    	void UpdateAI(const uint32 diff)
    	{
    		if (me->FindNearestGameObject(184661, 30.0f))
    		{
    			if (Summon)
    			{
    				me->SummonGameObject(GO_Z_FIRE, -900.855103f, 1922.618774f, 92.219215f, 5.980657f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -896.976868f, 1921.480347f, 82.033356f, 5.371974f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -903.249817f, 1919.317261f, 76.100410f, 2.930174f, 0, 0, 0, 0, 15);
    				ResetTimer = 15000;
    				Summon = false;
    			}
    		}
    
    		if (!Summon)
    		{
    			if (ResetTimer <= diff)
    			{
    				Summon = true;
    			}
    			else ResetTimer -= diff;
    		}
    	}
    };

    

     CreatureAI* GetAI(Creature* creature) const
    {
    	return new npc_tower_forgeAI(creature);
    }

    

    
};

class npc_tower_north : public CreatureScript
{
public: 
    npc_tower_north() : CreatureScript("npc_tower_north") { }
    struct npc_tower_northAI : public ScriptedAI
    {
    	npc_tower_northAI(Creature* creature) : ScriptedAI(creature) {}
    
    	bool Summon;
    	uint32 ResetTimer;
    	void Reset()
    	{
    		Summon = true;
    	}
    
    	void UpdateAI(const uint32 diff)
    	{
    		if (me->FindNearestGameObject(184661, 30.0f))
    		{
    			if (Summon)
    			{
    				me->SummonGameObject(GO_Z_FIRE, -825.942f, 2034.932f, 65.399f, 0.841f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -823.178f, 2029.728f, 54.571f, 2.145f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -828.604f, 2033.299f, 46.497f, 1.717f, 0, 0, 0, 0, 15);
    				ResetTimer = 15000;
    				Summon = false;
    			}
    		}
    
    		if (!Summon)
    		{
    			if (ResetTimer <= diff)
    			{
    				Summon = true;
    			}
    			else ResetTimer -= diff;
    		}
    	}
    };

    

     CreatureAI* GetAI(Creature* creature) const
    {
    	return new npc_tower_northAI(creature);
    }

    

    
};

class npc_tower_south : public CreatureScript
{
public: 
    npc_tower_south() : CreatureScript("npc_tower_south") { }
    struct npc_tower_southAI : public ScriptedAI
    {
    	npc_tower_southAI(Creature* creature) : ScriptedAI(creature) {}
    
    	bool Summon;
    	uint32 ResetTimer;
    	void Reset()
    	{
    		Summon = true;
    	}
    
    	void UpdateAI(const uint32 diff)
    	{
    		if (me->FindNearestGameObject(184661, 30.0f))
    		{
    			if (Summon)
    			{
    				me->SummonGameObject(GO_Z_FIRE, -1155.282f, 2103.080f, 93.285f, 6.015f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -1152.252f, 2109.441f, 84.187f, 6.093f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -1149.971f, 2112.339f, 77.230f, 0.205f, 0, 0, 0, 0, 15);
    				ResetTimer = 15000;
    				Summon = false;
    			}
    		}
    
    		if (!Summon)
    		{
    			if (ResetTimer <= diff)
    			{
    				Summon = true;
    			}
    			else ResetTimer -= diff;
    		}
    	}
    };

    

     CreatureAI* GetAI(Creature* creature) const
    {
    	return new npc_tower_southAI(creature);
    }

    

    
};

class npc_tower_foothill : public CreatureScript
{
public: 
    npc_tower_foothill() : CreatureScript("npc_tower_foothill") { }
    struct npc_tower_foothillAI : public ScriptedAI
    {
    	npc_tower_foothillAI(Creature* creature) : ScriptedAI(creature) {}
    
    	bool Summon;
    	uint32 ResetTimer;
    	void Reset()
    	{
    		Summon = true;
    	}
    
    	void UpdateAI(const uint32 diff)
    	{
    		if (me->FindNearestGameObject(184661, 30.0f))
    		{
    			if (Summon)
    			{
    				me->SummonGameObject(GO_Z_FIRE, -976.103f, 1878.009f, 110.138f, 1.162f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -977.411f, 1878.557f, 123.025f, 6.093f, 0, 0, 0, 0, 15);
    				me->SummonGameObject(GO_Z_FIRE, -971.595f, 1880.863f, 101.692f, 0.844f, 0, 0, 0, 0, 15);
    				ResetTimer = 15000;
    				Summon = false;
    			}
    		}
    
    		if (!Summon)
    		{
    			if (ResetTimer <= diff)
    			{
    				Summon = true;
    			}
    			else ResetTimer -= diff;
    		}
    	}
    };

    

     CreatureAI* GetAI(Creature* creature) const
    {
    	return new npc_tower_foothillAI(creature);
    }

    

    
};

class go_central_beacon : public GameObjectScript
{
public: 
    go_central_beacon() : GameObjectScript("go_central_beacon") { }
    

    

    bool OnGossipHello(Player*, GameObject* _GO) override
    {
    	_GO->SummonGameObject(180352, -585.287f, 3779.997f, 31.0f, 0, 0, 0, 0, 0, 60);
    	return false;
    }
    

    

    
};

class go_western_beacon : public GameObjectScript
{
public: 
    go_western_beacon() : GameObjectScript("go_western_beacon") { }
    

    

    bool OnGossipHello(Player*, GameObject* _GO) override
    {
    	_GO->SummonGameObject(180352, -605.096f, 3988.93f, 31.3f, 0, 0, 0, 0, 0, 60);
    	return false;
    }
    

    

    
};

class go_southern_beacon : public GameObjectScript
{
public: 
    go_southern_beacon() : GameObjectScript("go_southern_beacon") { }
    

    

    bool OnGossipHello(Player*, GameObject* _GO) override
    {
    	_GO->SummonGameObject(180352, -769.909f, 3674.104f, 29.6f, 0, 0, 0, 0, 0, 60);
    	return false;
    }
    

    

    
};

class trigger_beacon_fire : public CreatureScript
{
public: 
    trigger_beacon_fire() : CreatureScript("trigger_beacon_fire") { }
    struct trigger_beacon_fireAI : public ScriptedAI
    {
    	trigger_beacon_fireAI(Creature* creature) : ScriptedAI(creature) {}
    
    	bool Delete;
    	uint32 ResetTimer;
    	void Reset()
    	{
    		Delete = false;
    	}
    
    	void UpdateAI(const uint32 diff)
    	{
    		if (me->FindNearestGameObject(180352, 10.0f))
    		{
    			if (!Delete)
    			{				
    				ResetTimer = 60000;
    				Delete = true;
    			}
    		}
    
    		if (Delete)
    		{
    			if (ResetTimer <= diff)
    			{
    				if (GameObject* torch = me->FindNearestGameObject(180352, 10.0f))
    				{
    					torch->Delete();
    					Delete = false;
    				}
    			}
    			else ResetTimer -= diff;
    		}
    	}
    };

    

     CreatureAI* GetAI(Creature* creature) const
    {
    	return new trigger_beacon_fireAI(creature);
    }

    

    
};

class npc_azethen : public CreatureScript
{
public: 
    npc_azethen() : CreatureScript("npc_azethen") { }
    struct npc_azethenAI : public ScriptedAI
    {
    	npc_azethenAI(Creature* pCreature) : ScriptedAI(pCreature) { }
    
    	bool SourceComplete;
    
    	uint64 uiPlayerGUID;
    	uint32 uiStepsTimer;
    	uint32 uiSteps;
    
    	void Reset()
    	{
    		SourceComplete = false;
    		uiPlayerGUID = 0;
    		uiStepsTimer = 0;
    		uiSteps = 0;
    	}
    
    	uint32 NextStep(uint32 uiSteps)
    	{
    		Creature* pPrisoner = me->FindNearestCreature(16795, 10);
    		
    		switch (uiSteps)
    		{
    		case 1:			
    			me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
    			DoScriptText(SAY_AZETHEN_1, me, 0);
    			return 5000;
    		case 2:
    			DoScriptText(SAY_AZETHEN_2, me, 0);
    			return 5000;
    		case 3:
    			me->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
    			return 3000;
    		case 4:
    			me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
    			DoScriptText(SAY_AZETHEN_3, me, 0);
    			return 1500;
    		case 5:
    			DoScriptText(SAY_PRISONER, pPrisoner, 0);
    			pPrisoner->HandleEmoteCommand(EMOTE_ONESHOT_EAT);
    			return 1500;
    		case 6:
    			pPrisoner->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_STUN);
    			return 5000;
    		case 7:
    			pPrisoner->DealDamage(pPrisoner, pPrisoner->GetHealth());
    			return 3000;
    		case 8:
    			me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
    			DoScriptText(SAY_AZETHEN_4, me, 0);			
    			return 1000;
    		case 9:			
    			Reset();
    		default:
    			return 0;
    		}
    	}
    
    	void UpdateAI(const uint32 uiDiff)
    	{
    		if (uiStepsTimer <= uiDiff)
    		{
    			if (SourceComplete)
    				uiStepsTimer = NextStep(++uiSteps);
    		}
    		else uiStepsTimer -= uiDiff;
    	}
    };

    

     CreatureAI* GetAI(Creature* pCreature) const
    {
    	return new npc_azethenAI(pCreature);
    }

    bool OnQuestReward(Player* pPlayer, Creature* pCreature, const Quest* _Quest, uint32 /*item*/) override
    {
    	if (_Quest->GetQuestId() == 9387)
    	{
    		CAST_AI(npc_azethenAI, pCreature->AI())->SourceComplete = true;
    	}
    
    	return true;
    }
    

    

    
};


void AddSC_hellfire_peninsula()
{
    new npc_aeranas();
    new npc_ancestral_wolf();
    new go_haaleshi_altar();
    new npc_naladu();
    new npc_tracy_proudwell();
    new npc_trollbane();
    new npc_wing_commander_dabiree();
    new npc_gryphoneer_leafbeard();
    new npc_wing_commander_brack();
    new npc_wounded_blood_elf();
    new npc_hand_berserker();
    new npc_anchorite_relic_bunny();
    new npc_anchorite_barada();
    new npc_darkness_released();
    new npc_foul_purge();
    new npc_sedai_quest_credit_marker();
    new npc_demoniac_scryer();
    new npc_magic_sucker_device_spawner();
    new npc_living_flare();
    new npc_pathaleon_image();
    new npc_fel_guard_hound();
    new npc_arrazius_the_cruel();
    new npc_fel_cannon();
    new npc_maghar_grunt();
    new npc_dreghood_brute();
    new npc_dreghood_elder1();
    new npc_dreghood_elder2();
    new npc_dreghood_elder3();
    new npc_viera();
    new npc_felblood_initiate();
    new npc_felspark();
    new npc_camerashaker();
    new npc_east_hovel();
    new npc_west_hovel();
    new npc_stable();
    new npc_barracks();
    new npc_tower_forge();
    new npc_tower_north();
    new npc_tower_south();
    new npc_tower_foothill();
    new go_central_beacon();
    new go_western_beacon();
    new go_southern_beacon();
    new trigger_beacon_fire();
    new npc_azethen();

}

