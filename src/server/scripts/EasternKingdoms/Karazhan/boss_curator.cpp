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
 SDName: Boss_Curator
 SD%Complete: 100
 SDComment:
 SDCategory: Karazhan
 EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum CuratorInfo
{
    SAY_AGGRO = -1532057,
    SAY_SUMMON1 = -1532058,
    SAY_SUMMON2 = -1532059,
    SAY_EVOCATE = -1532060,
    SAY_ENRAGE = -1532061,
    SAY_KILL1 = -1532062,
    SAY_KILL2 = -1532063,
    SAY_DEATH = -1532064,

    SPELL_HATEFUL_BOLT = 30383,
    SPELL_EVOCATION = 30254,
    SPELL_ARCANE_INFUSION = 30403,
    SPELL_ASTRAL_DECONSTRUCTION = 30407,

    EVENT_SPELL_HATEFUL_BOLT = 1,
    EVENT_SPELL_EVOCATION = 2,
    EVENT_SPELL_ASTRAL_FLARE = 3,
    EVENT_SPELL_BERSERK = 4,
    EVENT_CHECK_HEALTH = 5

};


class boss_curator : public CreatureScript
{
public:
    boss_curator() : CreatureScript("boss_curator") { }

    struct boss_curatorAI : public ScriptedAI
    {
        boss_curatorAI(Creature* c) : ScriptedAI(c), summons(me) {}

        SummonList summons;
        EventMap events;

        void Reset()
        {
            me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ARCANE, true);
            summons.DespawnAll();
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoScriptText(RAND(SAY_KILL1, SAY_KILL2), me);
        }

        void JustDied(Unit* /*victim*/)
        {
            DoScriptText(SAY_DEATH, me);
        }

        void EnterCombat(Unit* /*who*/)
        {
            DoScriptText(SAY_AGGRO, me);
            events.ScheduleEvent(EVENT_SPELL_HATEFUL_BOLT, 10000);
            events.ScheduleEvent(EVENT_SPELL_ASTRAL_FLARE, 6000);
            events.ScheduleEvent(EVENT_SPELL_BERSERK, 600000);
            events.ScheduleEvent(EVENT_CHECK_HEALTH, 1000);
            DoZoneInCombat();
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);
            if (Unit* target = summon->SelectNearbyTarget(nullptr, 40.0f))
            {
                summon->AI()->AttackStart(target);
                summon->AddThreat(target, 1000.0f);
            }
            summon->SetInCombatWithZone();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (me->HasAura(SPELL_EVOCATION, 0))
                return;

            events.Update(diff);

            switch (events.ExecuteEvent())
            {
            case EVENT_CHECK_HEALTH:
                if (me->HealthBelowPct(16))
                {
                    events.CancelEvent(EVENT_SPELL_ASTRAL_FLARE);
                    me->CastSpell(me, SPELL_ARCANE_INFUSION, true);
                    DoScriptText(SAY_ENRAGE, me, nullptr);
                    break;
                }
                events.ScheduleEvent(EVENT_CHECK_HEALTH, 1000);
                break;
            case EVENT_SPELL_BERSERK:
                DoScriptText(SAY_ENRAGE, me, nullptr);
                me->InterruptNonMeleeSpells(true);
                me->CastSpell(me, SPELL_ASTRAL_DECONSTRUCTION, true);
                break;
            case EVENT_SPELL_HATEFUL_BOLT:
                if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, urand(1, 2), 40.0f))
                    me->CastSpell(target, SPELL_HATEFUL_BOLT, false);
                events.ScheduleEvent(EVENT_SPELL_HATEFUL_BOLT, urand(5000, 7500) * (events.GetNextEventTime(EVENT_SPELL_BERSERK) == 0 ? 1 : 2));
                break;
            case EVENT_SPELL_ASTRAL_FLARE:
            {
                int32 mana = CalculatePct(me->GetMaxPower(POWER_MANA), 10);
                me->ModifyPower(POWER_MANA, -mana);
                if (me->GetPower(POWER_MANA) < 10.0f)
                {
                    DoScriptText(SAY_EVOCATE, me, nullptr);
                    me->CastSpell(me, SPELL_EVOCATION, false);
                    events.DelayEvents(20000);
                    events.ScheduleEvent(EVENT_SPELL_ASTRAL_FLARE, 20000);
                }
                else
                {
                    if (roll_chance_i(50))
                        DoScriptText(RAND(SAY_SUMMON1, SAY_SUMMON2), me, nullptr);
                    Creature* AstralFlare = DoSpawnCreature(17096, rand() % 37, rand() % 37, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                    events.ScheduleEvent(EVENT_SPELL_ASTRAL_FLARE, 10000);
                }

                break;
            }
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new boss_curatorAI(pCreature);
    }
};

void AddSC_boss_curator()
{
    new boss_curator();
}

