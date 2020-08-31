#include "Chat.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "ScriptMgr.h"
#include "Language.h"

class quest_commandscript : public CommandScript
{
public:
    quest_commandscript() : CommandScript("quest_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> questCommandTable =
        {
            { "add",            SEC_GAMEMASTER,  false, &HandleAddQuest,                    "" },
            { "complete",       SEC_GAMEMASTER,  false, &HandleCompleteQuest,               "" },
            { "remove",         SEC_GAMEMASTER,  false, &HandleRemoveQuest,                 "" },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "quest",          SEC_GAMEMASTER,  false, nullptr, "", questCommandTable },
        };
        return commandTable;
    }

    static bool HandleAddQuest(ChatHandler* handler, const char* args)
    {
        Player* player = handler->getSelectedPlayerOrSelf();
        if (!player)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // .addquest #entry'
        // number or [name] Shift-click form |color|Hquest:quest_id|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hquest");
        if (!cId)
            return false;

        uint32 entry = atol(cId);

        Quest const* pQuest = sObjectMgr.GetQuestTemplate(entry);

        if (!pQuest)
        {
            handler->PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check item starting quest (it can work incorrectly if added without item in inventory)
        for (uint32 id = 0; id < sItemStorage.MaxEntry; id++)
        {
            ItemTemplate const* pProto = sItemStorage.LookupEntry<ItemTemplate>(id);
            if (!pProto)
                continue;

            if (pProto->StartQuest == entry)
            {
                handler->PSendSysMessage(LANG_COMMAND_QUEST_STARTFROMITEM, entry, pProto->ItemId);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        // ok, normal (creature/GO starting) quest
        if (player->CanAddQuest(pQuest, true))
        {
            player->AddQuest(pQuest, NULL);

            if (player->CanCompleteQuest(entry))
                player->CompleteQuest(entry);
        }

        return true;
    }

    static bool HandleRemoveQuest(ChatHandler* handler, const char* args)
    {
        Player* player = handler->getSelectedPlayerOrSelf();
        if (!player)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // .removequest #entry'
        // number or [name] Shift-click form |color|Hquest:quest_id|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hquest");
        if (!cId)
            return false;

        uint32 entry = atol(cId);

        Quest const* pQuest = sObjectMgr.GetQuestTemplate(entry);

        if (!pQuest)
        {
            handler->PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // remove all quest entries for 'entry' from quest log
        for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
        {
            uint32 quest = player->GetQuestSlotQuestId(slot);
            if (quest == entry)
            {
                player->SetQuestSlot(slot, 0);

                // we ignore unequippable quest items in this case, its' still be equipped
                player->TakeQuestSourceItem(quest, false);
            }
        }

        // set quest status to not started (will updated in DB at next save)
        player->SetQuestStatus(entry, QUEST_STATUS_NONE);

        // reset rewarded for restart repeatable quest
        player->getQuestStatusMap()[entry].m_rewarded = false;

        handler->SendSysMessage(LANG_COMMAND_QUEST_REMOVED);
        return true;
    }

    static bool HandleCompleteQuest(ChatHandler* handler, const char* args)
    {
        Player* player = handler->getSelectedPlayerOrSelf();
        if (!player)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // .quest complete #entry
        // number or [name] Shift-click form |color|Hquest:quest_id|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hquest");
        if (!cId)
            return false;

        uint32 entry = atol(cId);

        Quest const* pQuest = sObjectMgr.GetQuestTemplate(entry);

        // If player doesn't have the quest
        if (!pQuest || player->GetQuestStatus(entry) == QUEST_STATUS_NONE)
        {
            handler->PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Add quest items for quests that require items
        for (uint8 x = 0; x < QUEST_OBJECTIVES_COUNT; ++x)
        {
            uint32 id = pQuest->ReqItemId[x];
            uint32 count = pQuest->ReqItemCount[x];
            if (!id || !count)
                continue;

            uint32 curItemCount = player->GetItemCount(id, true);

            ItemPosCountVec dest;
            uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, id, count - curItemCount);
            if (msg == EQUIP_ERR_OK)
            {
                Item* item = player->StoreNewItem(dest, id, true);
                player->SendNewItem(item, count - curItemCount, true, false);
            }
        }

        // All creature/GO slain/casted (not required, but otherwise it will display "Creature slain 0/10")
        for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
        {
            int32 creature = pQuest->ReqCreatureOrGOId[i];
            uint32 creaturecount = pQuest->ReqCreatureOrGOCount[i];

            if (uint32 spell_id = pQuest->ReqSpell[i])
            {
                for (uint16 z = 0; z < creaturecount; ++z)
                    player->CastedCreatureOrGO(creature, 0, spell_id);
            }
            else if (creature > 0)
            {
                if (CreatureInfo const* cInfo = sObjectMgr.GetCreatureTemplate(creature))
                    for (uint16 z = 0; z < creaturecount; ++z)
                        player->KilledMonster(cInfo, 0);
            }
            else if (creature < 0)
            {
                for (uint16 z = 0; z < creaturecount; ++z)
                    player->CastedCreatureOrGO(creature, 0, 0);
            }
        }

        // If the quest requires reputation to complete
        if (uint32 repFaction = pQuest->GetRepObjectiveFaction())
        {
            uint32 repValue = pQuest->GetRepObjectiveValue();
            uint32 curRep = player->GetReputationMgr().GetReputation(repFaction);
            if (curRep < repValue)
            {
                FactionEntry const* factionEntry = sFactionStore.LookupEntry(repFaction);
                player->GetReputationMgr().SetReputation(factionEntry, repValue);
            }
        }

        // If the quest requires money
        int32 ReqOrRewMoney = pQuest->GetRewOrReqMoney();
        if (ReqOrRewMoney < 0)
            player->ModifyMoney(-ReqOrRewMoney);

        player->CompleteQuest(entry);
        return true;
    }
};

void AddSC_quest_commandscript()
{
    new quest_commandscript();
}