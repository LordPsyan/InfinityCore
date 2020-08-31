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
SDName: Felwood
SD%Complete: 95
SDComment: Quest support: 4101, 4102
SDCategory: Felwood
EndScriptData */

/* ContentData
npcs_riverbreeze_and_silversky
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"

/*######
## npcs_riverbreeze_and_silversky
######*/

#define GOSSIP_ITEM_BEACON  "Please make me a Cenarion Beacon"

class npcs_riverbreeze_and_silversky : public CreatureScript
{
public:
    npcs_riverbreeze_and_silversky() : CreatureScript("npcs_riverbreeze_and_silversky") { }
    struct npcs_riverbreeze_and_silverskyAI : public ScriptedAI
    {
        npcs_riverbreeze_and_silverskyAI(Creature* c) : ScriptedAI(c) {}

        bool OnGossipHello(Player* pPlayer, Creature* pCreature)
        {
            uint32 eCreature = pCreature->GetEntry();

            if (pCreature->IsQuestGiver())
                pPlayer->PrepareQuestMenu(pCreature->GetGUID());

            if (eCreature == 9528)
            {
                if (pPlayer->GetQuestRewardStatus(4101))
                {
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BEACON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                    pPlayer->SEND_GOSSIP_MENU(2848, pCreature->GetGUID());
                }
                else if (pPlayer->GetTeam() == HORDE)
                    pPlayer->SEND_GOSSIP_MENU(2845, pCreature->GetGUID());
                else
                    pPlayer->SEND_GOSSIP_MENU(2844, pCreature->GetGUID());
            }

            if (eCreature == 9529)
            {
                if (pPlayer->GetQuestRewardStatus(4102))
                {
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BEACON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                    pPlayer->SEND_GOSSIP_MENU(2849, pCreature->GetGUID());
                }
                else if (pPlayer->GetTeam() == ALLIANCE)
                    pPlayer->SEND_GOSSIP_MENU(2843, pCreature->GetGUID());
                else
                    pPlayer->SEND_GOSSIP_MENU(2842, pCreature->GetGUID());
            }

            return true;
        }

        
    };

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction) override
    {
        if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        {
            pPlayer->CLOSE_GOSSIP_MENU();
            pCreature->CastSpell(pPlayer, 15120, false);
        }
        return true;
    }

     CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npcs_riverbreeze_and_silverskyAI(pCreature);
    }
};

void AddSC_felwood()
{
    new npcs_riverbreeze_and_silversky();
}

