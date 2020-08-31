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
 SDName: Areatrigger_Scripts
 SD%Complete: 100
 SDComment: Scripts for areatriggers
 SDCategory: Areatrigger
 EndScriptData */

 /* ContentData
 at_coilfang_waterfall           4591
 at_legion_teleporter            4560 Teleporter TO Invasion Point: Cataclysm
 at_ravenholdt
 at_scent_larkorwi               1726,1727,1728,1729,1730,1731,1732,1733,1734,1735,1736,1737,1738,1739,1740
 at_ancient_leaf                 3587
 EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

 /*######
 ## at_coilfang_waterfall
 ######*/

enum eCoilfangGOs
{
    GO_COILFANG_WATERFALL = 184212
};
class AreaTrigger_at_coilfang_waterfall : public AreaTriggerScript
{
public:
    AreaTrigger_at_coilfang_waterfall() : AreaTriggerScript("at_coilfang_waterfall") {}

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
    {
        if (GameObject* pGo = GetClosestGameObjectWithEntry(player, GO_COILFANG_WATERFALL, 35.0f))
            if (pGo->getLootState() == GO_READY)
                pGo->UseDoorOrButton();

        return false;
    }
};

/*#####
## at_legion_teleporter
#####*/

enum eLegionTeleporter
{
    SPELL_TELE_A_TO = 37387,
    QUEST_GAINING_ACCESS_A = 10589,

    SPELL_TELE_H_TO = 37389,
    QUEST_GAINING_ACCESS_H = 10604
};

class AreaTrigger_at_legion_teleporter : AreaTriggerScript
{
public:
    AreaTrigger_at_legion_teleporter() : AreaTriggerScript("at_legion_teleporter") {}

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
    {
        if (player->IsAlive() && !player->IsInCombat())
        {
            if (player->GetTeam() == ALLIANCE && player->GetQuestRewardStatus(QUEST_GAINING_ACCESS_A))
            {
                player->CastSpell(player, SPELL_TELE_A_TO, false);
                return true;
            }

            if (player->GetTeam() == HORDE && player->GetQuestRewardStatus(QUEST_GAINING_ACCESS_H))
            {
                player->CastSpell(player, SPELL_TELE_H_TO, false);
                return true;
            }

            return false;
        }
        return false;
    }
};


enum eRavenholdt
{
    QUEST_MANOR_RAVENHOLDT = 6681,
    NPC_RAVENHOLDT = 13936
};

class AreaTrigger_at_ravenholt : AreaTriggerScript
{
public:
    AreaTrigger_at_ravenholt() : AreaTriggerScript("at_ravenholdt") {}

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
    {
        if (player->GetQuestStatus(QUEST_MANOR_RAVENHOLDT) == QUEST_STATUS_INCOMPLETE)
            player->KilledMonsterCredit(NPC_RAVENHOLDT, 0);

        return false;
    }
};


/*######
## at_scent_larkorwi
######*/

enum eLarkorwi
{
    QUEST_SCENT_OF_LARKORWI = 4291,
    NPC_LARKORWI_MATE = 9683
};

class AreaTrigger_at_scent_larkorwi : AreaTriggerScript
{
public:
    AreaTrigger_at_scent_larkorwi() : AreaTriggerScript("at_scent_larkorwi") {}

    bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
    {
        if (player->IsAlive() && !player->IsGameMaster() && player->GetQuestStatus(QUEST_SCENT_OF_LARKORWI) == QUEST_STATUS_INCOMPLETE)
        {
            if (!GetClosestCreatureWithEntry(player, NPC_LARKORWI_MATE, 25.0f, false))
                player->SummonCreature(NPC_LARKORWI_MATE, trigger->x, trigger->y, trigger->z, 3.3f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 2 * MINUTE * IN_MILLISECONDS);
        }

        return false;
    }

};

/*######
## at_ancient_leaf
######*/

enum eAncientLeaf
{
    QUEST_ANCIENT_LEAF = 7632,

    NPC_VARTRUS = 14524,
    NPC_STOMA = 14525,
    NPC_HASTAT = 14526,

    MAX_ANCIENTS = 3
};

struct AncientSpawn
{
    uint32 uiEntry;
    float fX, fY, fZ, fO;
};

static const AncientSpawn afSpawnLocations[MAX_ANCIENTS] =
{
    { NPC_VARTRUS, 6204.051758f, -1172.575684f, 370.079224f, 0.86052f },    // Vartus the Ancient
    { NPC_STOMA,   6246.953613f, -1155.985718f, 366.182953f, 2.90269f },    // Stoma the Ancient
    { NPC_HASTAT,  6193.449219f, -1137.834106f, 366.260529f, 5.77332f },    // Hastat the Ancient
};

class AreaTrigger_at_ancient_leaf : AreaTriggerScript
{
public:
    AreaTrigger_at_ancient_leaf() : AreaTriggerScript("at_ancient_leaf") {}

    bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
    {
        if (player->IsGameMaster() || !player->IsAlive())
            return false;

        // Handle Call Ancients event start - The area trigger summons 3 ancients
        if (player->GetQuestStatus(QUEST_ANCIENT_LEAF) == QUEST_STATUS_COMPLETE)
        {
            // If ancients are already spawned, skip the rest
            if (GetClosestCreatureWithEntry(player, NPC_VARTRUS, 50.0f) || GetClosestCreatureWithEntry(player, NPC_STOMA, 50.0f) || GetClosestCreatureWithEntry(player, NPC_HASTAT, 50.0f))
                return true;

            for (uint8 i = 0; i < MAX_ANCIENTS; ++i)
                player->SummonCreature(afSpawnLocations[i].uiEntry, afSpawnLocations[i].fX, afSpawnLocations[i].fY, afSpawnLocations[i].fZ, afSpawnLocations[i].fO, TEMPSUMMON_TIMED_DESPAWN, 5 * MINUTE * IN_MILLISECONDS);
        }

        return false;
    }
};

/*######
## at_haramad_teleport
######*/

enum
{
    QUEST_SPECIAL_DELIVERY_TO_SHATTRATH = 10280
};

static const WorldLocation haramadTeleportDest(530, -1810.465f, 5323.083f, -12.428f, 2.040f);

class AreaTrigger_at_haramad_teleport : AreaTriggerScript
{
public:
    AreaTrigger_at_haramad_teleport() : AreaTriggerScript("at_haramad_teleport") {}

    bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
    {
        if (player->IsGameMaster() || !player->IsAlive())
            return false;

        if (player->GetQuestStatus(QUEST_SPECIAL_DELIVERY_TO_SHATTRATH) == QUEST_STATUS_INCOMPLETE)
            player->TeleportTo(haramadTeleportDest);

        return false;
    }
};

void AddSC_areatrigger_scripts()
{
    new AreaTrigger_at_coilfang_waterfall();
    new AreaTrigger_at_legion_teleporter();
    new AreaTrigger_at_ravenholt();
    new AreaTrigger_at_scent_larkorwi();
    new AreaTrigger_at_ancient_leaf();
    new AreaTrigger_at_haramad_teleport();
}
