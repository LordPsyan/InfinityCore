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
 SDName: Boss_Illucia_Barov
 SD%Complete: 100
 SDComment:
 SDCategory: Scholomance
 EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "scholomance.h"

#define SPELL_CURSEOFAGONY      18671
#define SPELL_SHADOWSHOCK       20603
#define SPELL_SILENCE           15487
#define SPELL_FEAR              6215


class boss_illucia_barov : public CreatureScript
{
public:
    boss_illucia_barov() : CreatureScript("boss_illucia_barov") { }

    struct boss_illucia_barovAI : public ScriptedAI
    {
        boss_illucia_barovAI(Creature* c) : ScriptedAI(c) {}

        uint32 CurseOfAgony_Timer;
        uint32 ShadowShock_Timer;
        uint32 Silence_Timer;
        uint32 Fear_Timer;

        void Reset()
        {
            CurseOfAgony_Timer = 18000;
            ShadowShock_Timer = 9000;
            Silence_Timer = 5000;
            Fear_Timer = 30000;
        }

        void JustDied(Unit* /*killer*/)
        {
            ScriptedInstance* pInstance = (ScriptedInstance*)me->GetInstanceData();
            if (pInstance)
            {
                pInstance->SetData(DATA_LADYILLUCIABAROV_DEATH, 0);

                if (pInstance->GetData(TYPE_GANDLING) == IN_PROGRESS)
                    me->SummonCreature(1853, 180.73f, -9.43856f, 75.507f, 1.61399f, TEMPSUMMON_DEAD_DESPAWN, 0);
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            //CurseOfAgony_Timer
            if (CurseOfAgony_Timer <= diff)
            {
                DoCastVictim(SPELL_CURSEOFAGONY);
                CurseOfAgony_Timer = 30000;
            }
            else CurseOfAgony_Timer -= diff;

            //ShadowShock_Timer
            if (ShadowShock_Timer <= diff)
            {
                Unit* pTarget = NULL;
                pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (pTarget) DoCast(pTarget, SPELL_SHADOWSHOCK);

                ShadowShock_Timer = 12000;
            }
            else ShadowShock_Timer -= diff;

            //Silence_Timer
            if (Silence_Timer <= diff)
            {
                DoCastVictim(SPELL_SILENCE);
                Silence_Timer = 14000;
            }
            else Silence_Timer -= diff;

            //Fear_Timer
            if (Fear_Timer <= diff)
            {
                DoCastVictim(SPELL_FEAR);
                Fear_Timer = 30000;
            }
            else Fear_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new boss_illucia_barovAI(pCreature);
    }
};

void AddSC_boss_illuciabarov()
{
    new boss_illucia_barov();
}

