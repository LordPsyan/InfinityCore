#include "ScriptMgr.h"
#include "ScriptedCreature.h"



class go_helculars_grave : public GameObjectScript
{
public:
    go_helculars_grave() : GameObjectScript("go_helculars_grave") { }

    bool OnQuestComplete(Player *player, GameObject *pGo, const Quest *_Quest) override
    {
        if (_Quest->GetQuestId() == 553)
        {
            Position pos;
            pos = pGo->GetPosition();
            Creature* Hellcular = pGo->SummonCreature(2433, pos, TEMPSUMMON_CORPSE_DESPAWN, 0);

            Hellcular->SetWalk(true);
            Hellcular->GetMotionMaster()->MovePoint(0, -811.053, -527.344, 483561, true);

            return true;
        }


        return true;
    }
};

void AddSC_hillsbrad_foothills()
{
    new go_helculars_grave();
}

