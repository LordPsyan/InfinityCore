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
 SDName: Npcs_Special
 SD%Complete: 100
 SDComment: To be used for special NPCs that are located globally.
 SDCategory: NPCs
 EndScriptData
 */

 /* ContentData
 npc_lunaclaw_spirit          80%    support for quests 6001/6002 (Body and Heart)
 npc_chicken_cluck           100%    support for quest 3861 (Cluck!)
 npc_dancing_flames          100%    midsummer event NPC
 npc_guardian                100%    guardianAI used to prevent players from accessing off-limits areas. Not in use by SD2
 npc_garments_of_quests       80%    NPC's related to all Garments of-quests 5621, 5624, 5625, 5648, 5650
 npc_injured_patient         100%    patients for triage-quests (6622 and 6624)
 npc_doctor                  100%    Gustaf Vanhowzen and Gregory Victor, quest 6622 and 6624 (Triage)
 npc_mount_vendor            100%    Regular mount vendors all over the world. Display gossip if player doesn't meet the requirements to buy
 npc_rogue_trainer            80%    Scripted trainers, so they are able to offer item 17126 for class quest 6681
 npc_sayge                   100%    Darkmoon event fortune teller, buff player based on answers given
 npc_snake_trap_serpents     100%    AI for snakes that summoned by Snake Trap
 npc_force_of_nature_treants 100%    AI for force of nature (druid spell)
 mob_inferno_infernal        100%    AI for Inferno (warlock spell)
 mob_explosive_sheep         100%    AI for Explosive Sheep
 npc_barmaid                 100%    Reponsive emotes for barmaids
 EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "ObjectMgr.h"
#include "World.h"
#include "WorldPacket.h"

enum
{
    NPC_EXPLOSIVE_SHEEP = 2675,
    SPELL_EXPLOSIVE_SHEEP = 4050
};

class npc_explosive_sheep : public CreatureScript
{
public:
    npc_explosive_sheep() : CreatureScript("npc_explosive_sheep") {}

    struct npc_explosive_sheepAI : ScriptedAI
    {
        npc_explosive_sheepAI(Creature* creature) : ScriptedAI(creature)
        {
            checkTimer = 0;
            DespawnTimer = 0;
        }

        uint32 checkTimer;
        uint32 DespawnTimer;
        Unit* target = nullptr;

        void UpdateAI(const uint32 uiDiff) override
        {
            checkTimer += uiDiff;
            DespawnTimer += uiDiff;

            if (DespawnTimer >= 3 * MINUTE * IN_MILLISECONDS || (target && !target->IsAlive()))
            {
                me->Kill(me, false);
                me->ForcedDespawn(12000);
            }

            if (checkTimer >= 1000)
            {
                checkTimer = 0;
                if (Unit* target = me->SelectNearestTarget(30.0f))
                {
                    me->GetMotionMaster()->MoveChase(target);
                    if (me->GetDistance(target) < 3.0f)
                    {
                        me->CastSpell(me, SPELL_EXPLOSIVE_SHEEP, false);
                        me->Kill(me, false);
                        me->DespawnOrUnsummon(12000);
                    }
                }
                else if (!me->HasUnitState(UNIT_STATE_FOLLOW))
                {
                    if (Unit* owner = me->GetCharmerOrOwner())
                    {
                        me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_explosive_sheepAI(pCreature);
    }
};

/*######
## npc_lunaclaw_spirit
######*/

enum eLunaclaw
{
    QUEST_BODY_HEART_A = 6001,
    QUEST_BODY_HEART_H = 6002,

    TEXT_ID_DEFAULT = 4714,
    TEXT_ID_PROGRESS = 4715
};

#define GOSSIP_ITEM_GRANT   "You have thought well, spirit. I ask you to grant me the strength of your body and the strength of your heart."

class npc_lunaclaw_spirit : public CreatureScript
{
public:
    npc_lunaclaw_spirit() : CreatureScript("npc_lunaclaw_spirit") {}

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if (pPlayer->GetQuestStatus(QUEST_BODY_HEART_A) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(QUEST_BODY_HEART_H) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GRANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_DEFAULT, pCreature->GetGUID());
        return true;

    }

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
    {
        if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        {
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_PROGRESS, pCreature->GetGUID());
            pPlayer->AreaExploredOrEventHappens(pPlayer->GetTeam() == ALLIANCE ? QUEST_BODY_HEART_A : QUEST_BODY_HEART_H);
        }
        return true;
    }
};

/*########
# npc_chicken_cluck
#########*/

enum eChicken
{
    EMOTE_HELLO = -1000204,
    EMOTE_CLUCK_TEXT = -1000206,

    QUEST_CLUCK = 3861,
    FACTION_FRIENDLY = 35,
    FACTION_CHICKEN = 31
};

class npc_chicken_cluck : public CreatureScript
{
public:
    npc_chicken_cluck() : CreatureScript("npc_chicken_cluck") {}

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {

        if (quest->GetQuestId() == QUEST_CLUCK)
            CAST_AI(npc_chicken_cluckAI, creature->AI())->Reset();

        return true;
    }

    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_CLUCK)
            CAST_AI(npc_chicken_cluckAI, creature->AI())->Reset();

        return true;
    }

    struct npc_chicken_cluckAI : public ScriptedAI
    {
        npc_chicken_cluckAI(Creature* c) : ScriptedAI(c) {}

        uint32 ResetFlagTimer;

        void Reset()
        {
            ResetFlagTimer = 120000;
            me->SetFaction(FACTION_CHICKEN);
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
        }

        void EnterCombat(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            // Reset flags after a certain time has passed so that the next player has to start the 'event' again
            if (me->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER))
            {
                if (ResetFlagTimer <= diff)
                {
                    EnterEvadeMode();
                    return;
                }
                else ResetFlagTimer -= diff;
            }

            if (UpdateVictim())
                DoMeleeAttackIfReady();
        }

        void ReceiveEmote(Player* player, uint32 emote)
        {
            switch (emote)
            {
            case TEXT_EMOTE_CHICKEN:
                if (player->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_NONE && rand32() % 30 == 1)
                {
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                    me->SetFaction(FACTION_FRIENDLY);
                    DoScriptText(EMOTE_HELLO, me);
                }
                break;
            case TEXT_EMOTE_CHEER:
                if (player->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_COMPLETE)
                {
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                    me->SetFaction(FACTION_FRIENDLY);
                    DoScriptText(EMOTE_CLUCK_TEXT, me);
                }
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_chicken_cluckAI(pCreature);
    }
};

/*######
## npc_dancing_flames
######*/

#define SPELL_BRAZIER       45423
#define SPELL_SEDUCTION     47057
#define SPELL_FIERY_AURA    45427

class npc_dancing_flames : public CreatureScript
{
public:
    npc_dancing_flames() : CreatureScript("npc_dancing_flames") {}

    struct npc_dancing_flamesAI : public ScriptedAI
    {
        npc_dancing_flamesAI(Creature* c) : ScriptedAI(c) {}

        bool active;
        uint32 can_iteract;

        void Reset()
        {
            active = true;
            can_iteract = 3500;
            DoCast(me, SPELL_BRAZIER, true);
            DoCast(me, SPELL_FIERY_AURA, false);
            float x, y, z;
            me->GetPosition(x, y, z);
            me->GetMap()->CreatureRelocation(me, x, y, z + 0.94f, 0.0f);
            me->SetLevitate(true);
            me->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
            WorldPacket data;                       //send update position to client
            me->BuildHeartBeatMsg(&data);
            me->SendMessageToSet(&data, true);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!active)
            {
                if (can_iteract <= diff)
                {
                    active = true;
                    can_iteract = 3500;
                    me->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
                }
                else can_iteract -= diff;
            }
        }

        void EnterCombat(Unit* /*who*/) {}

        void ReceiveEmote(Player* pPlayer, uint32 emote)
        {
            if (me->IsWithinLOS(pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ()) && me->IsWithinDistInMap(pPlayer, 30.0f))
            {
                me->SetInFront(pPlayer);
                active = false;

                WorldPacket data;
                me->BuildHeartBeatMsg(&data);
                me->SendMessageToSet(&data, true);
                switch (emote)
                {
                case TEXT_EMOTE_KISS:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SHY);
                    break;
                case TEXT_EMOTE_WAVE:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                    break;
                case TEXT_EMOTE_BOW:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                    break;
                case TEXT_EMOTE_JOKE:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
                    break;
                case TEXT_EMOTE_DANCE:
                {
                    if (!pPlayer->HasAura(SPELL_SEDUCTION, 0))
                        DoCast(pPlayer, SPELL_SEDUCTION, true);
                }
                break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_dancing_flamesAI(pCreature);
    }

};

/*######
## Triage quest
######*/

enum eDoctor
{
    SAY_DOC1 = -1000201,
    SAY_DOC2 = -1000202,
    SAY_DOC3 = -1000203,

    DOCTOR_ALLIANCE = 12939,
    DOCTOR_HORDE = 12920,
    ALLIANCE_COORDS = 7,
    HORDE_COORDS = 6
};

struct Location
{
    float x, y, z, o;
};

static Location AllianceCoords[] =
{
    { -3757.38f, -4533.05f, 14.16f, 3.62f},                     // Top-far-right bunk as seen from entrance
    { -3754.36f, -4539.13f, 14.16f, 5.13f},                     // Top-far-left bunk
    { -3749.54f, -4540.25f, 14.28f, 3.34f},                     // Far-right bunk
    { -3742.10f, -4536.85f, 14.28f, 3.64f},                     // Right bunk near entrance
    { -3755.89f, -4529.07f, 14.05f, 0.57f},                     // Far-left bunk
    { -3749.51f, -4527.08f, 14.07f, 5.26f},                     // Mid-left bunk
    { -3746.37f, -4525.35f, 14.16f, 5.22f},                     // Left bunk near entrance
};

//alliance run to where
#define A_RUNTOX -3742.96f
#define A_RUNTOY -4531.52f
#define A_RUNTOZ 11.91f

static Location HordeCoords[] =
{
    { -1013.75f, -3492.59f, 62.62f, 4.34f},                     // Left, Behind
    { -1017.72f, -3490.92f, 62.62f, 4.34f},                     // Right, Behind
    { -1015.77f, -3497.15f, 62.82f, 4.34f},                     // Left, Mid
    { -1019.51f, -3495.49f, 62.82f, 4.34f},                     // Right, Mid
    { -1017.25f, -3500.85f, 62.98f, 4.34f},                     // Left, front
    { -1020.95f, -3499.21f, 62.98f, 4.34f}                      // Right, Front
};

//horde run to where
#define H_RUNTOX -1016.44f
#define H_RUNTOY -3508.48f
#define H_RUNTOZ 62.96f

const uint32 AllianceSoldierId[3] =
{
    12938,                                                  // 12938 Injured Alliance Soldier
    12936,                                                  // 12936 Badly injured Alliance Soldier
    12937                                                   // 12937 Critically injured Alliance Soldier
};

const uint32 HordeSoldierId[3] =
{
    12923,                                                  //12923 Injured Soldier
    12924,                                                  //12924 Badly injured Soldier
    12925                                                   //12925 Critically injured Soldier
};

/*######
## npc_doctor (handles both Gustaf Vanhowzen and Gregory Victor)
######*/
class npc_doctor : public CreatureScript
{
public:
    npc_doctor() : CreatureScript("npc_doctor") { }

    struct npc_doctorAI : public ScriptedAI
    {
        npc_doctorAI(Creature* creature) : ScriptedAI(creature) { }

        uint64 PlayerGUID;

        uint32 SummonPatientTimer;
        uint32 SummonPatientCount;
        uint32 PatientDiedCount;
        uint32 PatientSavedCount;

        bool Event;

        std::list<uint64> Patients;
        std::vector<Location*> Coordinates;

        void Reset()
        {
            PlayerGUID = 0;

            SummonPatientTimer = 10000;
            SummonPatientCount = 0;
            PatientDiedCount = 0;
            PatientSavedCount = 0;

            Patients.clear();
            Coordinates.clear();

            Event = false;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void BeginEvent(Player* player)
        {
            PlayerGUID = player->GetGUID();

            SummonPatientTimer = 10000;
            SummonPatientCount = 0;
            PatientDiedCount = 0;
            PatientSavedCount = 0;

            switch (me->GetEntry())
            {
            case DOCTOR_ALLIANCE:
                for (uint8 i = 0; i < ALLIANCE_COORDS; ++i)
                    Coordinates.push_back(&AllianceCoords[i]);
                break;
            case DOCTOR_HORDE:
                for (uint8 i = 0; i < HORDE_COORDS; ++i)
                    Coordinates.push_back(&HordeCoords[i]);
                break;
            }

            Event = true;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void PatientDied(Location* point)
        {
            Player* player = ObjectAccessor::GetPlayer(*me, PlayerGUID);
            if (player && ((player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)))
            {
                ++PatientDiedCount;

                if (PatientDiedCount > 5 && Event)
                {
                    if (player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE)
                        player->FailQuest(6624);
                    else if (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)
                        player->FailQuest(6622);

                    Reset();
                    return;
                }

                Coordinates.push_back(point);
            }
            else
                // If no player or player abandon quest in progress
                Reset();
        }

        void PatientSaved(Creature* /*soldier*/, Player* player, Location* point)
        {
            if (player && PlayerGUID == player->GetGUID())
            {
                if ((player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE))
                {
                    ++PatientSavedCount;

                    if (PatientSavedCount == 15)
                    {
                        if (!Patients.empty())
                        {
                            std::list<uint64>::const_iterator itr;
                            for (itr = Patients.begin(); itr != Patients.end(); ++itr)
                            {
                                if (Creature* patient = ObjectAccessor::GetCreature((*me), *itr))
                                    patient->setDeathState(JUST_DIED);
                            }
                        }

                        if (player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE)
                            player->AreaExploredOrEventHappens(6624);
                        else if (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)
                            player->AreaExploredOrEventHappens(6622);

                        Reset();
                        return;
                    }

                    Coordinates.push_back(point);
                }
            }
        }

        void UpdateAI(uint32 diff);

        void EnterCombat(Unit* /*who*/) { }
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if ((quest->GetQuestId() == 6624) || (quest->GetQuestId() == 6622))
            CAST_AI(npc_doctor::npc_doctorAI, creature->AI())->BeginEvent(player);

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_doctorAI(creature);
    }
};

/*#####
## npc_injured_patient (handles all the patients, no matter Horde or Alliance)
#####*/

class npc_injured_patient : public CreatureScript
{
public:
    npc_injured_patient() : CreatureScript("npc_injured_patient") { }

    struct npc_injured_patientAI : public ScriptedAI
    {
        npc_injured_patientAI(Creature* creature) : ScriptedAI(creature) { }

        uint64 DoctorGUID;
        Location* Coord;

        void Reset()
        {
            DoctorGUID = 0;
            Coord = NULL;

            //no select
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            //no regen health
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

            //to make them lay with face down
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_DEAD);

            uint32 mobId = me->GetEntry();

            switch (mobId)
            {                                                   //lower max health
            case 12923:
            case 12938:                                     //Injured Soldier
                me->SetHealth(me->CountPctFromMaxHealth(75));
                break;
            case 12924:
            case 12936:                                     //Badly injured Soldier
                me->SetHealth(me->CountPctFromMaxHealth(50));
                break;
            case 12925:
            case 12937:                                     //Critically injured Soldier
                me->SetHealth(me->CountPctFromMaxHealth(25));
                break;
            }
        }

        void EnterCombat(Unit* /*who*/) { }

        void SpellHit(Unit* caster, SpellEntry const* spell)
        {
            Player* player = caster->ToPlayer();
            if (!player || !me->IsAlive() || spell->Id != 20804)
                return;

            if (player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)
                if (DoctorGUID)
                    if (Creature* doctor = ObjectAccessor::GetCreature(*me, DoctorGUID))
                        CAST_AI(npc_doctor::npc_doctorAI, doctor->AI())->PatientSaved(me, player, Coord);

            //make not selectable
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            //regen health
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

            //stand up
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_STAND);

            DoScriptText(RAND(SAY_DOC1, SAY_DOC2, SAY_DOC3), me);

            uint32 mobId = me->GetEntry();
            me->SetWalk(false);

            switch (mobId)
            {
            case 12923:
            case 12924:
            case 12925:
                me->GetMotionMaster()->MovePoint(0, H_RUNTOX, H_RUNTOY, H_RUNTOZ);
                break;
            case 12936:
            case 12937:
            case 12938:
                me->GetMotionMaster()->MovePoint(0, A_RUNTOX, A_RUNTOY, A_RUNTOZ);
                break;
            }
        }

        void UpdateAI(uint32 /*diff*/)
        {
            //lower HP on every world tick makes it a useful counter, not officlone though
            if (me->IsAlive() && me->GetHealth() > 6)
                me->ModifyHealth(-5);

            if (me->IsAlive() && me->GetHealth() <= 6)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->setDeathState(JUST_DIED);
                me->SetFlag(UNIT_DYNAMIC_FLAGS, 32);

                if (DoctorGUID)
                    if (Creature* doctor = ObjectAccessor::GetCreature((*me), DoctorGUID))
                        CAST_AI(npc_doctor::npc_doctorAI, doctor->AI())->PatientDied(Coord);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_injured_patientAI(creature);
    }
};

void npc_doctor::npc_doctorAI::UpdateAI(uint32 diff)
{
    if (Event && SummonPatientCount >= 20)
    {
        Reset();
        return;
    }

    if (Event)
    {
        if (SummonPatientTimer <= diff)
        {
            if (Coordinates.empty())
                return;

            std::vector<Location*>::iterator itr = Coordinates.begin() + rand() % Coordinates.size();
            uint32 patientEntry = 0;

            switch (me->GetEntry())
            {
            case DOCTOR_ALLIANCE:
                patientEntry = AllianceSoldierId[rand() % 3];
                break;
            case DOCTOR_HORDE:
                patientEntry = HordeSoldierId[rand() % 3];
                break;
            default:
                sLog.outError("TSCR: Invalid entry for Triage doctor. Please check your database");
                return;
            }

            if (Location* point = *itr)
            {
                if (Creature* Patient = me->SummonCreature(patientEntry, point->x, point->y, point->z, point->o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
                {
                    //303, this flag appear to be required for client side item->spell to work (TARGET_SINGLE_FRIEND)
                    Patient->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);

                    Patients.push_back(Patient->GetGUID());
                    CAST_AI(npc_injured_patient::npc_injured_patientAI, Patient->AI())->DoctorGUID = me->GetGUID();
                    CAST_AI(npc_injured_patient::npc_injured_patientAI, Patient->AI())->Coord = point;

                    Coordinates.erase(itr);
                }
            }
            SummonPatientTimer = 10000;
            ++SummonPatientCount;
        }
        else
            SummonPatientTimer -= diff;
    }
}


/*######
## npc_garments_of_quests
######*/

//@todo get text for each NPC

enum eGarments
{
    SPELL_LESSER_HEAL_R2 = 2052,
    SPELL_FORTITUDE_R1 = 1243,

    QUEST_MOON = 5621,
    QUEST_LIGHT_1 = 5624,
    QUEST_LIGHT_2 = 5625,
    QUEST_SPIRIT = 5648,
    QUEST_DARKNESS = 5650,

    ENTRY_SHAYA = 12429,
    ENTRY_ROBERTS = 12423,
    ENTRY_DOLF = 12427,
    ENTRY_KORJA = 12430,
    ENTRY_DG_KEL = 12428,

    SAY_COMMON_HEALED = -1000231,
    SAY_DG_KEL_THANKS = -1000232,
    SAY_DG_KEL_GOODBYE = -1000233,
    SAY_ROBERTS_THANKS = -1000256,
    SAY_ROBERTS_GOODBYE = -1000257,
    SAY_KORJA_THANKS = -1000258,
    SAY_KORJA_GOODBYE = -1000259,
    SAY_DOLF_THANKS = -1000260,
    SAY_DOLF_GOODBYE = -1000261,
    SAY_SHAYA_THANKS = -1000262,
    SAY_SHAYA_GOODBYE = -1000263,
};

class npc_garments_of_quests : public CreatureScript
{
public:
    npc_garments_of_quests() : CreatureScript("npc_garments_of_quests") {}

    struct npc_garments_of_questsAI : public npc_escortAI
    {
        npc_garments_of_questsAI(Creature* c) : npc_escortAI(c)
        {
            Reset();
        }

        uint64 caster;

        bool bIsHealed;
        bool bCanRun;

        uint32 RunAwayTimer;

        void Reset()
        {
            caster = 0;

            bIsHealed = false;
            bCanRun = false;

            RunAwayTimer = 5000;

            me->SetStandState(UNIT_STAND_STATE_KNEEL);
            //expect database to have RegenHealth=0
            me->SetHealth(int(me->GetMaxHealth() * 0.7));
        }

        void EnterCombat(Unit* /*who*/) {}

        void SpellHit(Unit* pCaster, const SpellEntry* Spell)
        {
            if (Spell->Id == SPELL_LESSER_HEAL_R2 || Spell->Id == SPELL_FORTITUDE_R1)
            {
                //not while in combat
                if (me->IsInCombat())
                    return;

                //nothing to be done now
                if (bIsHealed && bCanRun)
                    return;

                if (pCaster->GetTypeId() == TYPEID_PLAYER)
                {
                    switch (me->GetEntry())
                    {
                    case ENTRY_SHAYA:
                        if (CAST_PLR(pCaster)->GetQuestStatus(QUEST_MOON) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_SHAYA_THANKS, me, pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetGUID();
                                me->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, me, pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_ROBERTS:
                        if (CAST_PLR(pCaster)->GetQuestStatus(QUEST_LIGHT_1) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_ROBERTS_THANKS, me, pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetGUID();
                                me->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, me, pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_DOLF:
                        if (CAST_PLR(pCaster)->GetQuestStatus(QUEST_LIGHT_2) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_DOLF_THANKS, me, pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetGUID();
                                me->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, me, pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_KORJA:
                        if (CAST_PLR(pCaster)->GetQuestStatus(QUEST_SPIRIT) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_KORJA_THANKS, me, pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetGUID();
                                me->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, me, pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_DG_KEL:
                        if (CAST_PLR(pCaster)->GetQuestStatus(QUEST_DARKNESS) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_DG_KEL_THANKS, me, pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetGUID();
                                me->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, me, pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    }

                    //give quest credit, not expect any special quest objectives
                    if (bCanRun)
                        CAST_PLR(pCaster)->TalkedToCreature(me->GetEntry(), me->GetGUID());
                }
            }
        }

        void WaypointReached(uint32 /*uiPoint*/)
        {
        }

        void UpdateAI(const uint32 diff)
        {
            if (bCanRun && !me->IsInCombat())
            {
                if (RunAwayTimer <= diff)
                {
                    if (Unit* pUnit = Unit::GetUnit(*me, caster))
                    {
                        switch (me->GetEntry())
                        {
                        case ENTRY_SHAYA:
                            DoScriptText(SAY_SHAYA_GOODBYE, me, pUnit);
                            break;
                        case ENTRY_ROBERTS:
                            DoScriptText(SAY_ROBERTS_GOODBYE, me, pUnit);
                            break;
                        case ENTRY_DOLF:
                            DoScriptText(SAY_DOLF_GOODBYE, me, pUnit);
                            break;
                        case ENTRY_KORJA:
                            DoScriptText(SAY_KORJA_GOODBYE, me, pUnit);
                            break;
                        case ENTRY_DG_KEL:
                            DoScriptText(SAY_DG_KEL_GOODBYE, me, pUnit);
                            break;
                        }

                        Start(false, true, true);
                    }
                    else
                        EnterEvadeMode();                       //something went wrong

                    RunAwayTimer = 30000;
                }
                else RunAwayTimer -= diff;
            }

            npc_escortAI::UpdateAI(diff);
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_garments_of_questsAI(pCreature);
    }
};

/*######
## npc_guardian
######*/

#define SPELL_DEATHTOUCH                5
#define SAY_AGGRO                        "This area is closed!"

class npc_guardian : public CreatureScript
{
public:
    npc_guardian() : CreatureScript("npc_guardian") {}

    struct npc_guardianAI : public ScriptedAI
    {
        npc_guardianAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->MonsterYell(SAY_AGGRO, LANG_UNIVERSAL, 0);
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

            if (me->isAttackReady())
            {
                DoCastVictim(SPELL_DEATHTOUCH, true);
                me->resetAttackTimer();
            }
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_guardianAI(pCreature);
    }
};

/*######
## npc_mount_vendor
######*/

class npc_mount_vendor : public CreatureScript
{
public:
    npc_mount_vendor() : CreatureScript("npc_mount_vendor") {}

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if (pCreature->IsQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());

        bool canBuy;
        canBuy = false;
        uint32 vendor = pCreature->GetEntry();
        uint8 race = pPlayer->getRace();

        switch (vendor)
        {
        case 384:                                           //Katie Hunter
        case 1460:                                          //Unger Statforth
        case 2357:                                          //Merideth Carlson
        case 4885:                                          //Gregor MacVince
            if (pPlayer->GetReputationRank(72) != REP_EXALTED && race != RACE_HUMAN)
                pPlayer->SEND_GOSSIP_MENU(5855, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 1261:                                          //Veron Amberstill
            if (pPlayer->GetReputationRank(47) != REP_EXALTED && race != RACE_DWARF)
                pPlayer->SEND_GOSSIP_MENU(5856, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 3362:                                          //Ogunaro Wolfrunner
            if (pPlayer->GetReputationRank(76) != REP_EXALTED && race != RACE_ORC)
                pPlayer->SEND_GOSSIP_MENU(5841, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 3685:                                          //Harb Clawhoof
            if (pPlayer->GetReputationRank(81) != REP_EXALTED && race != RACE_TAUREN)
                pPlayer->SEND_GOSSIP_MENU(5843, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 4730:                                          //Lelanai
            if (pPlayer->GetReputationRank(69) != REP_EXALTED && race != RACE_NIGHTELF)
                pPlayer->SEND_GOSSIP_MENU(5844, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 4731:                                          //Zachariah Post
            if (pPlayer->GetReputationRank(68) != REP_EXALTED && race != RACE_UNDEAD_PLAYER)
                pPlayer->SEND_GOSSIP_MENU(5840, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 7952:                                          //Zjolnir
            if (pPlayer->GetReputationRank(530) != REP_EXALTED && race != RACE_TROLL)
                pPlayer->SEND_GOSSIP_MENU(5842, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 7955:                                          //Milli Featherwhistle
            if (pPlayer->GetReputationRank(54) != REP_EXALTED && race != RACE_GNOME)
                pPlayer->SEND_GOSSIP_MENU(5857, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 16264:                                         //Winaestra
            if (pPlayer->GetReputationRank(911) != REP_EXALTED && race != RACE_BLOODELF)
                pPlayer->SEND_GOSSIP_MENU(10305, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 17584:                                         //Torallius the Pack Handler
            if (pPlayer->GetReputationRank(930) != REP_EXALTED && race != RACE_DRAENEI)
                pPlayer->SEND_GOSSIP_MENU(10239, pCreature->GetGUID());
            else canBuy = true;
            break;
        }

        if (canBuy)
        {
            if (pCreature->IsVendor())
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
    {
        if (uiAction == GOSSIP_ACTION_TRADE)
            pPlayer->SEND_VENDORLIST(pCreature->GetGUID());

        return true;
    }
};


/*######
## npc_rogue_trainer
######*/

#define GOSSIP_HELLO_ROGUE1 "I wish to unlearn my talents"
#define GOSSIP_HELLO_ROGUE2 "<Take the letter>"
#define GOSSIP_HELLO_ROGUE3 "Purchase a Dual Talent Specialization."

class npc_rogue_trainer : public CreatureScript
{
public:
    npc_rogue_trainer() : CreatureScript("npc_rogue_trainer") {}

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if (pCreature->IsQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());

        if (pCreature->IsTrainer())
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);

        if (pCreature->CanTrainAndResetTalentsOf(pPlayer))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_HELLO_ROGUE1, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_UNLEARNTALENTS);

        if (pPlayer->getClass() == CLASS_ROGUE && pPlayer->getLevel() >= 24 && !pPlayer->HasItemCount(17126, 1) && !pPlayer->GetQuestRewardStatus(6681))
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_ROGUE2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(5996, pCreature->GetGUID());
        }
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
    {
        switch (uiAction)
        {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, 21100, false);
            break;
        case GOSSIP_ACTION_TRAIN:
            pPlayer->SEND_TRAINERLIST(pCreature->GetGUID());
            break;
        case GOSSIP_OPTION_UNLEARNTALENTS:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->SendTalentWipeConfirm(pCreature->GetGUID());
            break;
        }
        return true;
    }
};


/*######
## npc_sayge
######*/

#define SPELL_DMG       23768                               //dmg
#define SPELL_RES       23769                               //res
#define SPELL_ARM       23767                               //arm
#define SPELL_SPI       23738                               //spi
#define SPELL_INT       23766                               //int
#define SPELL_STM       23737                               //stm
#define SPELL_STR       23735                               //str
#define SPELL_AGI       23736                               //agi
#define SPELL_FORTUNE   23765                               //faire fortune

#define GOSSIP_HELLO_SAYGE  "Yes"
#define GOSSIP_SENDACTION_SAYGE1    "Slay the Man"
#define GOSSIP_SENDACTION_SAYGE2    "Turn him over to liege"
#define GOSSIP_SENDACTION_SAYGE3    "Confiscate the corn"
#define GOSSIP_SENDACTION_SAYGE4    "Let him go and have the corn"
#define GOSSIP_SENDACTION_SAYGE5    "Execute your friend painfully"
#define GOSSIP_SENDACTION_SAYGE6    "Execute your friend painlessly"
#define GOSSIP_SENDACTION_SAYGE7    "Let your friend go"
#define GOSSIP_SENDACTION_SAYGE8    "Confront the diplomat"
#define GOSSIP_SENDACTION_SAYGE9    "Show not so quiet defiance"
#define GOSSIP_SENDACTION_SAYGE10   "Remain quiet"
#define GOSSIP_SENDACTION_SAYGE11   "Speak against your brother openly"
#define GOSSIP_SENDACTION_SAYGE12   "Help your brother in"
#define GOSSIP_SENDACTION_SAYGE13   "Keep your brother out without letting him know"
#define GOSSIP_SENDACTION_SAYGE14   "Take credit, keep gold"
#define GOSSIP_SENDACTION_SAYGE15   "Take credit, share the gold"
#define GOSSIP_SENDACTION_SAYGE16   "Let the knight take credit"
#define GOSSIP_SENDACTION_SAYGE17   "Thanks"

class npc_sayge : public CreatureScript
{
public:
    npc_sayge() : CreatureScript("npc_sayge") { }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature) override

    {
        if (pCreature->IsQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());

        if (pPlayer->HasSpellCooldown(SPELL_INT) ||
            pPlayer->HasSpellCooldown(SPELL_ARM) ||
            pPlayer->HasSpellCooldown(SPELL_DMG) ||
            pPlayer->HasSpellCooldown(SPELL_RES) ||
            pPlayer->HasSpellCooldown(SPELL_STR) ||
            pPlayer->HasSpellCooldown(SPELL_AGI) ||
            pPlayer->HasSpellCooldown(SPELL_STM) ||
            pPlayer->HasSpellCooldown(SPELL_SPI))
            pPlayer->SEND_GOSSIP_MENU(7393, pCreature->GetGUID());
        else
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SAYGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(7339, pCreature->GetGUID());
        }

        return true;
    }

    void SendAction(Player* pPlayer, Creature* pCreature, uint32 action)
    {
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(7340, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE5, GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE6, GOSSIP_SENDER_MAIN + 2, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE7, GOSSIP_SENDER_MAIN + 3, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7341, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE8, GOSSIP_SENDER_MAIN + 4, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE9, GOSSIP_SENDER_MAIN + 5, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE10, GOSSIP_SENDER_MAIN + 2, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7361, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE11, GOSSIP_SENDER_MAIN + 6, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE12, GOSSIP_SENDER_MAIN + 7, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE13, GOSSIP_SENDER_MAIN + 8, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7362, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE14, GOSSIP_SENDER_MAIN + 5, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE15, GOSSIP_SENDER_MAIN + 4, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE16, GOSSIP_SENDER_MAIN + 3, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7363, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE17, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            pPlayer->SEND_GOSSIP_MENU(7364, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 6:
            pCreature->CastSpell(pPlayer, SPELL_FORTUNE, false);
            pPlayer->SEND_GOSSIP_MENU(7365, pCreature->GetGUID());
            break;
        }
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        
        switch (sender)
        {
        case GOSSIP_SENDER_MAIN:
            SendAction(player, creature, action);
            break;
        case GOSSIP_SENDER_MAIN + 1:
            creature->CastSpell(player, SPELL_DMG, false);
            player->AddSpellCooldown(SPELL_DMG, 0, 2 * HOUR*IN_MILLISECONDS);
            SendAction(player, creature, action);
            break;
        case GOSSIP_SENDER_MAIN + 2:
            creature->CastSpell(player, SPELL_RES, false);
            player->AddSpellCooldown(SPELL_RES, 0, 2 * HOUR*IN_MILLISECONDS);
            SendAction(player, creature, action);
            break;
        case GOSSIP_SENDER_MAIN + 3:
            creature->CastSpell(player, SPELL_ARM, false);
            player->AddSpellCooldown(SPELL_ARM, 0, 2 * HOUR*IN_MILLISECONDS);
            SendAction(player, creature, action);
            break;
        case GOSSIP_SENDER_MAIN + 4:
            creature->CastSpell(player, SPELL_SPI, false);
            player->AddSpellCooldown(SPELL_SPI, 0, 2 * HOUR*IN_MILLISECONDS);
            SendAction(player, creature, action);
            break;
        case GOSSIP_SENDER_MAIN + 5:
            creature->CastSpell(player, SPELL_INT, false);
            player->AddSpellCooldown(SPELL_INT, 0, 2 * HOUR*IN_MILLISECONDS);
            SendAction(player, creature, action);
            break;
        case GOSSIP_SENDER_MAIN + 6:
            creature->CastSpell(player, SPELL_STM, false);
            player->AddSpellCooldown(SPELL_STM, 0, 2 * HOUR*IN_MILLISECONDS);
            SendAction(player, creature, action);
            break;
        case GOSSIP_SENDER_MAIN + 7:
            creature->CastSpell(player, SPELL_STR, false);
            player->AddSpellCooldown(SPELL_STR, 0, 2 * HOUR*IN_MILLISECONDS);
            SendAction(player, creature, action);
            break;
        case GOSSIP_SENDER_MAIN + 8:
            creature->CastSpell(player, SPELL_AGI, false);
            player->AddSpellCooldown(SPELL_AGI, 0, 2 * HOUR*IN_MILLISECONDS);
            SendAction(player, creature, action);
            break;
        }
        return true;
    }
};

class npc_steam_tonk : public CreatureScript
{
public:
    npc_steam_tonk() : CreatureScript("npc_steam_tonk") {}

    struct npc_steam_tonkAI : public ScriptedAI
    {
        npc_steam_tonkAI(Creature* c) : ScriptedAI(c) {}

        void Reset() {}
        void EnterCombat(Unit* /*who*/) {}

        void OnPossess(bool apply)
        {
            if (apply)
            {
                // Initialize the action bar without the melee attack command
                me->InitCharmInfo();
                me->GetCharmInfo()->InitEmptyActionBar(false);

                me->SetReactState(REACT_PASSIVE);
            }
            else
                me->SetReactState(REACT_AGGRESSIVE);
        }

    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_steam_tonkAI(pCreature);
    }
};

#define SPELL_TONK_MINE_DETONATE 25099

class npc_tonk_mine : public CreatureScript
{
public:
    npc_tonk_mine() : CreatureScript("npc_tonk_mine") {}

    struct npc_tonk_mineAI : public ScriptedAI
    {
        npc_tonk_mineAI(Creature* c) : ScriptedAI(c)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        uint32 ExplosionTimer;

        void Reset()
        {
            ExplosionTimer = 3000;
        }

        void EnterCombat(Unit* /*who*/) {}
        void AttackStart(Unit* /*who*/) {}
        void MoveInLineOfSight(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (ExplosionTimer <= diff)
            {
                DoCast(me, SPELL_TONK_MINE_DETONATE, true);
                me->setDeathState(DEAD); // unsummon it
            }
            else
                ExplosionTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_tonk_mineAI(pCreature);
    }
};

/*####
## npc_brewfest_reveler
####*/

class npc_brewfest_reveler : public CreatureScript
{
public:
    npc_brewfest_reveler() : CreatureScript("npc_brewfest_reveler") {}

    struct npc_brewfest_revelerAI : public ScriptedAI
    {
        npc_brewfest_revelerAI(Creature* c) : ScriptedAI(c) {}
        void ReceiveEmote(Player* pPlayer, uint32 emote)
        {
            if (emote == TEXT_EMOTE_DANCE)
                me->CastSpell(pPlayer, 41586, false);
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_brewfest_revelerAI(pCreature);
    }
};

/*####
## npc_winter_reveler
####*/

class npc_winter_reveler : public CreatureScript
{
public:
    npc_winter_reveler() : CreatureScript("npc_winter_reveler") {}

    struct npc_winter_revelerAI : public ScriptedAI
    {
        npc_winter_revelerAI(Creature* c) : ScriptedAI(c) {}
        void ReceiveEmote(Player* pPlayer, uint32 emote)
        {
            //@todo check auralist.
            if (pPlayer->HasAura(26218, 1))
                return;

            if (emote == TEXT_EMOTE_KISS)
            {
                me->CastSpell(me, 26218, false);
                pPlayer->CastSpell(pPlayer, 26218, false);
                switch (urand(0, 2))
                {
                case 0:
                    me->CastSpell(pPlayer, 26207, false);
                    break;
                case 1:
                    me->CastSpell(pPlayer, 26206, false);
                    break;
                case 2:
                    me->CastSpell(pPlayer, 45036, false);
                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_winter_revelerAI(pCreature);
    }
};


enum TrainingDummy
{
    NPC_ADVANCED_TARGET_DUMMY = 2674,
    NPC_TARGET_DUMMY = 2673,
};

class npc_training_dummy : public CreatureScript
{
public:
    npc_training_dummy() : CreatureScript("npc_training_dummy") {}

    struct npc_training_dummyAI : public ScriptedAI
    {
        npc_training_dummyAI(Creature* c) : ScriptedAI(c)
        {
            SetCombatMovement(false);
        }

        uint32 combatCheckTimer;
        uint32 despawnTimer;
        UNORDERED_MAP<uint64, time_t> _damageTimes;

        void Reset()
        {
            me->SetControlled(true, UNIT_STATE_STUNNED);                                // disable rotate
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);    // imune to knock aways

            _damageTimes.clear();

            if (me->GetEntry() != NPC_ADVANCED_TARGET_DUMMY && me->GetEntry() != NPC_TARGET_DUMMY)
                combatCheckTimer = 1000;
            else
                despawnTimer = 15000;
        }

        void EnterEvadeMode()
        {
            if (!_EnterEvadeMode())
                return;

            Reset();
        }

        void DamageTaken(Unit* doneBy, uint32& damage)
        {
            me->AddThreat(doneBy, float(damage));    // just to create threat reference
            _damageTimes[doneBy->GetGUID()] = time(NULL);
            damage = 0;
        }

        void UpdateAI(const uint32 diff) override
        {
            if (!me->IsInCombat())
                return;

            if (!me->HasUnitState(UNIT_STATE_STUNNED))
                me->SetControlled(true, UNIT_STATE_STUNNED);    // disable rotate

            if (combatCheckTimer <= diff)
            {
                time_t now = time(NULL);
                for (UNORDERED_MAP<uint64, time_t>::iterator itr = _damageTimes.begin(); itr != _damageTimes.end();)
                {
                    // If unit has not dealt damage to training dummy for 5 seconds, remove him from combat
                    if (itr->second < now - 5)
                    {
                        if (Unit* unit = ObjectAccessor::GetUnit(*me, itr->first))
                            unit->getHostileRefManager().deleteReference(me);

                        itr = _damageTimes.erase(itr);
                    }
                    else
                        ++itr;
                }
                combatCheckTimer = 1000;
            }
            else
                combatCheckTimer -= diff;

            if (despawnTimer <= diff)
            {
                me->DisappearAndDie(false);
            }
            else
                despawnTimer -= diff;
        }

        void MoveInLineOfSight(Unit* /*who*/) { }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_training_dummyAI(creature);
    }
};

/************************************************************/

class npc_force_of_nature_treants : public CreatureScript
{
public:
    npc_force_of_nature_treants() : CreatureScript("npc_force_of_nature_treants") {}

    struct npc_force_of_nature_treantsAI : public ScriptedAI
    {

        npc_force_of_nature_treantsAI(Creature* c) : ScriptedAI(c) {}

        Unit* Owner;

        void Reset()
        {
            Owner = me->GetOwner();
            if (!Owner)
                return;

            if (Unit* target = Owner->getAttackerForHelper())
            {
                me->SetInCombatWith(target);
                AttackStart(target);
            }
        }

        void UpdateAI(const uint32 /*diff*/)
        {

            if (!Owner)
                return;

            if (!me->GetVictim())
            {
                if (Unit* target = Owner->getAttackerForHelper())
                    AttackStart(target);
                else if (!me->HasUnitState(UNIT_STATE_FOLLOW))
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveFollow(Owner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                }
            }

            DoMeleeAttackIfReady();
        }

    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_force_of_nature_treantsAI(pCreature);
    }
};

/*####
## npc_snake_trap_serpents
####*/

#define SPELL_MIND_NUMBING_POISON    8692    //Viper
#define SPELL_DEADLY_POISON          34655   //Venomous Snake
#define SPELL_CRIPPLING_POISON       3409    //Viper

#define VENOMOUS_SNAKE_TIMER 1200
#define VIPER_TIMER 3000

#define C_VIPER 19921

class npc_snake_trap_serpents : public CreatureScript
{
public:
    npc_snake_trap_serpents() : CreatureScript("npc_snake_trap_serpents") {}

    struct npc_snake_trap_serpentsAI : public ScriptedAI
    {
        npc_snake_trap_serpentsAI(Creature* c) : ScriptedAI(c), SpellTimer(0) {}

        uint32 SpellTimer;
        Unit* Owner;
        bool IsViper;

        void EnterCombat(Unit* /*who*/) {}

        void Reset()
        {
            Owner = me->GetOwner();

            if (!Owner)
                return;

            CreatureInfo const* Info = me->GetCreatureTemplate();

            if (Info->Entry == C_VIPER)
                IsViper = true;
            else
                IsViper = false;

            //Add delta to make them not all hit the same time
            uint32 delta = (rand() % 7) * 100;
            CreatureBaseStats const* cCLS = sObjectMgr.GetCreatureClassLvlStats(me->getLevel(), Info->unit_class, Info->exp);
            me->SetStatFloatValue(UNIT_FIELD_BASEATTACKTIME, Info->BaseAttackTime + delta);
            me->SetStatFloatValue(UNIT_FIELD_RANGED_ATTACK_POWER, cCLS->BaseMeleeAttackPower);

            if (Unit* attacker = Owner->getAttackerForHelper())
            {
                me->SetInCombatWith(attacker);
                AttackStart(attacker);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!Owner)
                return;

            if (!me->GetVictim())
            {
                if (Owner->getAttackerForHelper())
                    AttackStart(Owner->getAttackerForHelper());
                else if (!me->HasUnitState(UNIT_STATE_FOLLOW))
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveFollow(Owner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                }
            }

            if (SpellTimer <= diff)
            {
                if (IsViper) //Viper
                {
                    if (urand(0, 2) == 0) //33% chance to cast
                    {
                        uint32 spell;
                        if (urand(0, 1) == 0)
                            spell = SPELL_MIND_NUMBING_POISON;
                        else
                            spell = SPELL_CRIPPLING_POISON;

                        DoCastVictim(spell);
                    }

                    SpellTimer = VIPER_TIMER;
                }
                else //Venomous Snake
                {
                    if (rand() % 10 < 8) //80% chance to cast
                        DoCastVictim(SPELL_DEADLY_POISON);
                    SpellTimer = VENOMOUS_SNAKE_TIMER + (rand() % 5) * 100;
                }
            }
            else SpellTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_snake_trap_serpentsAI(pCreature);
    }
};


#define SAY_RANDOM_MOJO0    "Now that's what I call froggy-style!"
#define SAY_RANDOM_MOJO1    "Your lily pad or mine?"
#define SAY_RANDOM_MOJO2    "This won't take long, did it?"
#define SAY_RANDOM_MOJO3    "I thought you'd never ask!"
#define SAY_RANDOM_MOJO4    "I promise not to give you warts..."
#define SAY_RANDOM_MOJO5    "Feelin' a little froggy, are ya?"
#define SAY_RANDOM_MOJO6a   "Listen, "
#define SAY_RANDOM_MOJO6b   ", I know of a little swamp not too far from here...."
#define SAY_RANDOM_MOJO7    "There's just never enough Mojo to go around..."

class mob_mojo : public CreatureScript
{
public:
    mob_mojo() : CreatureScript("mob_mojo") {}

    struct mob_mojoAI : public ScriptedAI
    {
        mob_mojoAI(Creature* c) : ScriptedAI(c)
        {
            Reset();
        }
        uint32 hearts;
        uint64 victimGUID;
        void Reset()
        {
            victimGUID = 0;
            hearts = 15000;
            if (Unit* own = me->GetOwner())
                me->GetMotionMaster()->MoveFollow(own, 0, 0);
        }
        void Aggro(Unit* /*who*/) {}
        void UpdateAI(const uint32 diff)
        {
            if (me->HasAura(20372, 0))
            {
                if (hearts <= diff)
                {
                    me->RemoveAurasDueToSpell(20372);
                    hearts = 15000;
                }
                hearts -= diff;
            }
        }
        void ReceiveEmote(Player* pPlayer, uint32 emote)
        {
            me->HandleEmoteCommand(emote);
            Unit* own = me->GetOwner();
            if (!own || own->GetTypeId() != TYPEID_PLAYER || CAST_PLR(own)->GetTeam() != pPlayer->GetTeam())
                return;
            if (emote == TEXT_EMOTE_KISS)
            {
                std::string whisp = "";
                switch (rand() % 8)
                {
                case 0:
                    whisp.append(SAY_RANDOM_MOJO0);
                    break;
                case 1:
                    whisp.append(SAY_RANDOM_MOJO1);
                    break;
                case 2:
                    whisp.append(SAY_RANDOM_MOJO2);
                    break;
                case 3:
                    whisp.append(SAY_RANDOM_MOJO3);
                    break;
                case 4:
                    whisp.append(SAY_RANDOM_MOJO4);
                    break;
                case 5:
                    whisp.append(SAY_RANDOM_MOJO5);
                    break;
                case 6:
                    whisp.append(SAY_RANDOM_MOJO6a);
                    whisp.append(pPlayer->GetName());
                    whisp.append(SAY_RANDOM_MOJO6b);
                    break;
                case 7:
                    whisp.append(SAY_RANDOM_MOJO7);
                    break;
                }
                me->MonsterWhisper(whisp.c_str(), pPlayer->GetGUID());
                if (victimGUID)
                {
                    Player* victim = Unit::GetPlayer(*me, victimGUID);
                    if (victim && victim->HasAura(43906, 0))
                        victim->RemoveAurasDueToSpell(43906); // remove polymorph frog thing
                }
                me->AddAura(43906, pPlayer); //add polymorph frog thing
                victimGUID = pPlayer->GetGUID();
                DoCast(me, 20372, true);//tag.hearts
                me->GetMotionMaster()->MoveFollow(pPlayer, 0, 0);
                hearts = 15000;
            }
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_mojoAI(pCreature);
    }
};

/*######
## mob_rift_spawn
######*/

#define RIFT_EMOTE_AGGRO    "%s is angered and attacks!"
#define RIFT_EMOTE_EVADE    "%s escapes into the void!"
#define RIFT_EMOTE_SUCKED   "%s is sucked into the coffer!"

enum RiftSpawn
{
    // Creatures
    MOB_RIFT_SPAWN = 6492,

    // Spells
    SPELL_SELF_STUN_30SEC = 9032,
    SPELL_RIFT_SPAWN_INVISIBILITY = 9093,
    SPELL_CANTATION_OF_MANIFESTATION = 9095,
    SPELL_RIFT_SPAWN_MANIFESTATION = 9096,
    SPELL_CREATE_FILLED_CONTAINMENT_COFFER = 9010,

    // Objects
    GO_CONTAINMENT_COFFER = 122088,

    // Factions
    FACTION_HOSTILE = 91,
    FACTION_NEUTRAL = 35,

    // Data 
    ITEM_USED = 1
};

class mob_rift_spawn : public CreatureScript
{
public:
    mob_rift_spawn() : CreatureScript("mob_rift_spawn") {}

    struct mob_rift_spawnAI : public ScriptedAI
    {
        mob_rift_spawnAI(Creature* c) : ScriptedAI(c) { }

        bool creatureActive;
        bool questActive;
        uint32 escapeTimer;

        void Reset()
        {
            DoCast(me, SPELL_RIFT_SPAWN_INVISIBILITY, true);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            me->SetFaction(FACTION_HOSTILE);

            creatureActive = false;
            questActive = false;
            escapeTimer = 0;
        }

        void JustReachedHome()
        {
            Reset();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            if (damage && damage > me->GetHealth())
            {
                escapeTimer = 31000;
                damage = 0;
                me->SetFaction(FACTION_NEUTRAL);
                me->CombatStop();
                me->RemoveAllAuras();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->SetHealth(me->GetMaxHealth());
                DoCast(me, SPELL_SELF_STUN_30SEC, true);
            }
        }

        void EscapeIntoVoid(bool handleQuest)
        {
            if (handleQuest)
            {
                if (GameObject* container = GetClosestGameObjectWithEntry(me, GO_CONTAINMENT_COFFER, 5.0f))
                {
                    if (Creature* trigger = me->SummonTrigger(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, 10000))
                    {
                        trigger->SetVisible(false);
                        trigger->CastSpell(trigger, SPELL_CREATE_FILLED_CONTAINMENT_COFFER, true);
                    }
                    container->Delete();
                }

                me->MonsterTextEmote(RIFT_EMOTE_SUCKED, 0);
                me->DisappearAndDie(false);
                return;
            }

            me->MonsterTextEmote(RIFT_EMOTE_EVADE, 0);
            me->GetMotionMaster()->MoveTargetedHome();
            DoCast(me, SPELL_RIFT_SPAWN_INVISIBILITY, true);
        }


        void SetData(uint32 type, uint32 /*data*/)
        {
            if (type == ITEM_USED)
                questActive = true;
        }

        void UpdateAI(const uint32 diff)
        {
            if (escapeTimer)
            {
                if (escapeTimer <= diff)
                    EscapeIntoVoid(questActive);
                else
                    escapeTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }

        void SpellHit(Unit* caster, const SpellEntry* spell)
        {
            if (!creatureActive && spell->Id == SPELL_CANTATION_OF_MANIFESTATION && me->IsWithinDist(caster, 2.5f, false))
            {
                // Prevent reactivating this sequence
                creatureActive = true;

                // Remove auras
                me->RemoveAurasDueToSpell(SPELL_RIFT_SPAWN_INVISIBILITY);
                me->RemoveAurasDueToSpell(SPELL_CANTATION_OF_MANIFESTATION);

                // Spawn animation and engage in combat
                DoCast(SPELL_RIFT_SPAWN_MANIFESTATION);
                me->CombatStart(caster);

                me->MonsterTextEmote(RIFT_EMOTE_AGGRO, caster->GetGUID());
            }
        }
    };

    CreatureAI* GetAI(Creature* _Creature) const
    {
        return new mob_rift_spawnAI(_Creature);
    }

};

class go_containment_coffer : public GameObjectScript
{
public:
    go_containment_coffer() : GameObjectScript("go_containment_coffer") {}

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (Creature* spawn = GetClosestCreatureWithEntry(go, MOB_RIFT_SPAWN, 5.0f, true))
        {
            if (spawn->HasAura(SPELL_SELF_STUN_30SEC))
            {
                // Send Data State
                spawn->AI()->SetData(ITEM_USED, 0);

                // Update gameobject anim state
                go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
                go->UseDoorOrButton();
            }
        }

        return true;
    }
};


class mob_inferno_infernal : public CreatureScript
{
public:
    mob_inferno_infernal() : CreatureScript("mob_inferno_infernal") {}

    struct mob_inferno_infernalAI : public ScriptedAI
    {
        mob_inferno_infernalAI(Creature* c) : ScriptedAI(c), initialized(false) { }

        void Reset()
        {
            if (!initialized && me->HasUnitTypeMask(~0))
            {
                if (Unit* owner = ((TempSummon*)me)->GetSummoner())
                {
                    owner->CastSpell(me, owner->GetHighestLearnedRankOf(11726), true); // Enslave Demon
                    me->CastSpell(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 22703, false, NULL, NULL, owner->GetGUID()); // Inferno Effect (dmg + stun)
                    ((TempSummon*)me)->SetTempSummonType(TEMPSUMMON_TIMED_DESPAWN_OUT_OF_CHARM);
                    initialized = true;
                }
            }
        }

        bool initialized;
    };

    CreatureAI* GetAI(Creature* _Creature) const
    {
        return new mob_inferno_infernalAI(_Creature);
    }
};

/*######
## npc_barmaid
######*/

enum eBarmaid
{
    SAY_BARMAID1 = -1910260,
    SAY_BARMAID2_MALE = -1910261,
    SAY_BARMAID2_FEMALE = -1910262,
    SAY_BARMAID3 = -1910263,
    SAY_BARMAID4 = -1910264,
    SAY_BARMAID5 = -1910265,
    SAY_BARMAID6 = -1910266,

    EMOTE_BARMAID_RUDE = -1910259,
};

class npc_barmaid : public CreatureScript
{
public:
    npc_barmaid() : CreatureScript("npc_barmaid") {}

    struct npc_barmaidAI : public ScriptedAI
    {
        npc_barmaidAI(Creature* c) : ScriptedAI(c) {}

        void ReceiveEmote(Player* pPlayer, uint32 emote)
        {
            switch (emote)
            {
            case TEXT_EMOTE_APPLAUD:
            case TEXT_EMOTE_BOW:
                me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                break;
            case TEXT_EMOTE_DANCE:
                me->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
                break;
            case TEXT_EMOTE_FLEX:
                me->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
                break;
            case TEXT_EMOTE_KISS:
                me->HandleEmoteCommand(EMOTE_ONESHOT_SHY);
                break;
            case TEXT_EMOTE_RUDE:
                me->HandleEmoteCommand(EMOTE_ONESHOT_RUDE);
                DoScriptText(EMOTE_BARMAID_RUDE, me, pPlayer);
                break;
            case TEXT_EMOTE_SHY:
                me->HandleEmoteCommand(EMOTE_ONESHOT_KISS);
                break;
            case TEXT_EMOTE_WAVE:
                switch (urand(0, 5))
                {
                case 0:
                    DoScriptText(SAY_BARMAID1, me);
                    break;
                case 1:
                    if (pPlayer->getGender() == 0)
                        DoScriptText(SAY_BARMAID2_MALE, me);
                    else
                        DoScriptText(SAY_BARMAID2_FEMALE, me);
                    break;
                case 2:
                    DoScriptText(SAY_BARMAID3, me);
                    break;
                case 3:
                    DoScriptText(SAY_BARMAID4, me, pPlayer);
                    break;
                case 4:
                    DoScriptText(SAY_BARMAID5, me);
                    break;
                case 5:
                    DoScriptText(SAY_BARMAID6, me, pPlayer);
                    break;
                }
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_barmaidAI(pCreature);
    }
};



void AddSC_npcs_special()
{
    new npc_explosive_sheep();
    new npc_lunaclaw_spirit();
    new npc_chicken_cluck();
    new npc_dancing_flames();
    new npc_garments_of_quests();
    new npc_guardian();
    new npc_mount_vendor();
    new npc_rogue_trainer();
    new npc_sayge();
    new npc_steam_tonk();
    new npc_tonk_mine();
    new npc_brewfest_reveler();
    new npc_doctor();
    new npc_injured_patient();
    new npc_winter_reveler();
    new npc_training_dummy();
    new npc_force_of_nature_treants();
    new npc_snake_trap_serpents();
    new mob_mojo();
    new mob_rift_spawn();
    new go_containment_coffer();
    new mob_inferno_infernal();
    new npc_barmaid();
}
