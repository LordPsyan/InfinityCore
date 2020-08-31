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
 SDName: Custom_Gossip_Codebox
 SD%Complete: 100
 SDComment: Show a codebox in gossip option
 SDCategory: Script Examples
 EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include <cstring>

class custom_gossip_codebox : CreatureScript
{
public:
    custom_gossip_codebox() : CreatureScript("custom_gossip_codebox") {}

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        player->ADD_GOSSIP_ITEM_EXTENDED(0, "A quiz: what's your name?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "", 0, true);
        player->ADD_GOSSIP_ITEM(0, "I'm not interested", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

        player->PlayerTalkClass->SendGossipMenu(907, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        if (action == GOSSIP_ACTION_INFO_DEF + 2)
        {
            creature->Say("Normal select, guess you're not interested.", LANG_UNIVERSAL, 0);
            player->CLOSE_GOSSIP_MENU();
        }
        return true;
    }

    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code) override
    {
        if (sender == GOSSIP_SENDER_MAIN)
        {
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                if (std::strcmp(code, player->GetName()) != 0)
                {
                    creature->Say("Wrong!", LANG_UNIVERSAL, 0);
                    creature->CastSpell(player, 12826, true);
                }
                else
                {
                    creature->Say("You're right, you are allowed to see my inner secrets.", LANG_UNIVERSAL, 0);
                    creature->CastSpell(player, 26990, true);
                }
                player->CLOSE_GOSSIP_MENU();
                return true;
            }
        }
        return false;
    }

};

void AddSC_custom_gossip_codebox()
{
    new custom_gossip_codebox;
}

