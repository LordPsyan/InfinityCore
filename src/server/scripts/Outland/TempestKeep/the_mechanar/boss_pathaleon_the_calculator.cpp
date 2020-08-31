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
 SDName: Boss Pathaleon the Calculator
 SD%Complete: 50
 SDComment: Event missing. Script for himself 99% blizzlike.
 SDCategory: Tempest Keep, The Mechanar
 EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "mechanar.h"

enum pathaleon
{
    SAY_AGGRO = -1554020,
    SAY_DOMINATION_1 = -1554021,
    SAY_DOMINATION_2 = -1554022,
    SAY_SUMMON = -1554023,
    SAY_ENRAGE = -1554024,
    SAY_SLAY_1 = -1554025,
    SAY_SLAY_2 = -1554026,
    SAY_DEATH = -1554027,
    SAY_APPEAR = -1554028,

    SPELL_MANA_TAP = 36021,
    SPELL_ARCANE_TORRENT = 36022,
    SPELL_DOMINATION = 35280,
    SPELL_ARCANE_EXPLOSION = 15453,
    SPELL_FRENZY = 36992,
    SPELL_DISGRUNTLED_ANGER = 35289,

    SPELL_SUMMON_NETHER_WRAITH_1 = 35285,
    SPELL_SUMMON_NETHER_WRAITH_2 = 35286,
    SPELL_SUMMON_NETHER_WRAITH_3 = 35287,
    SPELL_SUMMON_NETHER_WRAITH_4 = 35288,

    SPELL_DETONATION = 35058,
    SPELL_ARCANE_MISSILES = 35034,

    EVENT_SUMMON = 1,
    EVENT_MANA_TAP = 2,
    EVENT_ARCANE_TORRENT = 3,
    EVENT_DOMINATION = 4,
    EVENT_ARCANE_EXPLOSION = 5,
    EVENT_FRENZY = 6,

    EVENT_ARCANE_MISSILE = 7,
    EVENT_DETONATION = 8,
    EVENT_DIE = 9,

};

class boss_pathaleon_the_calculator : public CreatureScript
{
public:
    boss_pathaleon_the_calculator() : CreatureScript("boss_pathaleon_the_calculator") { }

    struct boss_pathaleon_the_calculatorAI : public ScriptedAI
    {
        boss_pathaleon_the_calculatorAI(Creature* c) : ScriptedAI(c), summons(me)
        {
            me->GetMap()->IsHeroic() ? wraith == 4 : wraith == 3;
        }

        SummonList summons;
        EventMap events;
        ScriptedInstance* pInstance;
        uint8 wraith;

        void InitializeAI()
        {
            me->SetVisible(false);
            me->SetReactState(REACT_PASSIVE);
        }

        void DoAction(int32 /*param*/)
        {
            me->SetVisible(true);
            me->CastSpell(me, SPELL_TELEPORT_VISUAL, true);
            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY1H);
            me->SetReactState(REACT_AGGRESSIVE);
            DoScriptText(SAY_APPEAR, me);
        }

        void Reset()
        {
            events.Reset();
            summons.DespawnAll();
            InitializeAI();
        }
        void EnterCombat(Unit* /*who*/)
        {
            DoScriptText(SAY_AGGRO, me);
            events.ScheduleEvent(EVENT_SUMMON, 30000);
            events.ScheduleEvent(EVENT_MANA_TAP, 12000);
            events.ScheduleEvent(EVENT_ARCANE_TORRENT, 16000);
            events.ScheduleEvent(EVENT_DOMINATION, 25000);
            events.ScheduleEvent(EVENT_ARCANE_EXPLOSION, 8000);
            events.ScheduleEvent(EVENT_FRENZY, 1000);
        }

        void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() == TYPEID_PLAYER)
                DoScriptText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustDied(Unit* /*Killer*/)
        {
            DoScriptText(SAY_DEATH, me);
            summons.DespawnAll();
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);
        }

        void UpdateAI(const uint32 diff)
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            events.Update(diff);
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
            case EVENT_ARCANE_EXPLOSION:
                me->CastSpell(me, SPELL_ARCANE_EXPLOSION, false);
                events.ScheduleEvent(EVENT_ARCANE_EXPLOSION, 12000);
                break;
            case EVENT_ARCANE_TORRENT:
                me->RemoveAurasDueToSpell(SPELL_MANA_TAP);
                me->ModifyPower(POWER_MANA, 5000);
                me->CastSpell(me, SPELL_ARCANE_TORRENT, false);
                events.ScheduleEvent(EVENT_ARCANE_TORRENT, 15000);
                break;
            case EVENT_MANA_TAP:
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                {
                    if (target->getPowerType() != POWER_MANA)
                        SelectTarget(SELECT_TARGET_RANDOM, 0);

                    me->CastSpell(target, SPELL_MANA_TAP, false);
                    events.ScheduleEvent(EVENT_MANA_TAP, 18000);
                }
                break;
            case EVENT_DOMINATION:
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 50.0f))
                {
                    DoScriptText(RAND(SAY_DOMINATION_1, SAY_DOMINATION_2), me);
                    me->CastSpell(target, SPELL_DOMINATION, false);
                }
                events.ScheduleEvent(EVENT_DOMINATION, 30000);
                break;
            case EVENT_FRENZY:
                if (me->HealthBelowPct(20))
                {
                    summons.DespawnAll();
                    me->CastSpell(me, SPELL_DISGRUNTLED_ANGER, true);
                    DoScriptText(SAY_ENRAGE, me);
                    break;
                }
                events.ScheduleEvent(EVENT_FRENZY, 1000);
                break;
            case EVENT_SUMMON:
                for (uint32 i = 0; i < wraith; ++i)
                    me->CastSpell(me, SPELL_SUMMON_NETHER_WRAITH_1 + i, true);
                DoScriptText(SAY_SUMMON, me);
                events.ScheduleEvent(EVENT_SUMMON, urand(30000, 45000));
                break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new boss_pathaleon_the_calculatorAI(pCreature);
    }
};

class mob_nether_wraith : public CreatureScript
{
public:
    mob_nether_wraith() : CreatureScript("mob_nether_wraith") { }

    struct mob_nether_wraithAI : public ScriptedAI
    {
        mob_nether_wraithAI(Creature* c) : ScriptedAI(c) {}

        EventMap events;

        void Reset() { }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_ARCANE_MISSILE, 1000 + rand() % 3000);
            events.ScheduleEvent(EVENT_DETONATION, 20000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            switch (events.ExecuteEvent())
            {
            case EVENT_ARCANE_MISSILE:
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 1))
                    DoCast(pTarget, SPELL_ARCANE_MISSILES);
                else
                    DoCastVictim(SPELL_ARCANE_MISSILES);
                events.Repeat(5000 + rand() % 5000);
                break;
            case EVENT_DETONATION:
                DoCast(me, SPELL_DETONATION);
                events.ScheduleEvent(EVENT_DIE, 2200);
                break;
            case EVENT_DIE:
                me->setDeathState(JUST_DIED);
                me->RemoveCorpse();
                break;
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_nether_wraithAI(pCreature);
    }
};


void AddSC_boss_pathaleon_the_calculator()
{
    new boss_pathaleon_the_calculator();
    new mob_nether_wraith();

}

