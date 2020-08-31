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

#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "AccountMgr.h"
#include "Language.h"
#include "World.h"
#include "Player.h"
#include "Opcodes.h"
#include <fstream>

class debug_commandscript : public CommandScript
{
public:
    debug_commandscript() : CommandScript("debug_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> debugPlayCommandTable =
        {
            { "cinematic",      SEC_ADMINISTRATOR,  false, &HandleDebugPlayCinematicCommand,  "" },
            { "sound",          SEC_ADMINISTRATOR,  false, &HandleDebugPlaySoundCommand,      "" },
        };
        static std::vector<ChatCommand> debugCommandTable =
        {
            { "inarc",          SEC_ADMINISTRATOR,  false, &HandleDebugInArcCommand,          "" },
            { "spellfail",      SEC_ADMINISTRATOR,  false, &HandleDebugSpellFailCommand,      "" },
            { "raferror",       SEC_ADMINISTRATOR,  false, &HandleDebugRAFError,              "" },
            { "setpoi",         SEC_ADMINISTRATOR,  false, &HandleSetPoiCommand,              "" },
            { "qpartymsg",      SEC_ADMINISTRATOR,  false, &HandleSendQuestPartyMsgCommand,   "" },
            { "qinvalidmsg",    SEC_ADMINISTRATOR,  false, &HandleSendQuestInvalidMsgCommand, "" },
            { "equiperr",       SEC_ADMINISTRATOR,  false, &HandleEquipErrorCommand,          "" },
            { "sellerr",        SEC_ADMINISTRATOR,  false, &HandleSellErrorCommand,           "" },
            { "buyerr",         SEC_ADMINISTRATOR,  false, &HandleBuyErrorCommand,            "" },
            { "sendopcode",     SEC_ADMINISTRATOR,  false, &HandleSendOpcodeCommand,          "" },
            { "uws",            SEC_ADMINISTRATOR,  false, &HandleUpdateWorldStateCommand,    "" },
            { "scn",            SEC_ADMINISTRATOR,  false, &HandleSendChannelNotifyCommand,   "" },
            { "scm",            SEC_ADMINISTRATOR,  false, &HandleSendChatMsgCommand,         "" },
            { "getitemstate",   SEC_ADMINISTRATOR,  false, &HandleGetItemState,               "" },
            { "play",           SEC_MODERATOR,      false, nullptr,                           "", debugPlayCommandTable },
            { "update",         SEC_ADMINISTRATOR,  false, &HandleUpdate,                     "" },
            { "setvalue",       SEC_ADMINISTRATOR,  false, &HandleSetValue,                   "" },
            { "getvalue",       SEC_ADMINISTRATOR,  false, &HandleGetValue,                   "" },
            { "Mod32Value",     SEC_ADMINISTRATOR,  false, &HandleMod32Value,                 "" },
            { "anim",           SEC_GAMEMASTER,     false, &HandleAnimCommand,                "" },
            { "lootrecipient",  SEC_GAMEMASTER,     false, &HandleGetLootRecipient,           "" },
            { "arena",          SEC_ADMINISTRATOR,  false, &HandleDebugArenaCommand,          "" },
            { "bg",             SEC_ADMINISTRATOR,  false, &HandleDebugBattlegroundCommand,   "" },
            { "threatlist",     SEC_ADMINISTRATOR,  false, &HandleDebugThreatList,            "" },
            { "setinstdata",    SEC_ADMINISTRATOR,  false, &HandleSetInstanceDataCommand,     "" },
            { "getinstdata",    SEC_ADMINISTRATOR,  false, &HandleGetInstanceDataCommand,     "" },
            { "spellcrashtest", SEC_ADMINISTRATOR,  false, &HandleSpellCrashTestCommand,      "" },
            { "partyresult",    SEC_ADMINISTRATOR,  false, &HandlePartyResultCommand,         "" },
            { "animate",        SEC_GAMEMASTER,     false, &HandleDebugAnimationCommand,      "" },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "debug",          SEC_GAMEMASTER,      false, nullptr,                          "", debugCommandTable }
        };
        return commandTable;
    }

    static bool HandleDebugInArcCommand(ChatHandler* handler, const char* /*args*/)
    {
        Object* obj = handler->getSelectedUnit();

        if (!obj)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            return true;
        }

        handler->SendSysMessage(LANG_NOT_IMPLEMENTED);

        return true;
    }

    static bool HandleDebugSpellFailCommand(ChatHandler* handler, const char* args)
    {
        if (!args)
            return false;

        char* px = strtok((char*)args, " ");
        if (!px)
            return false;

        uint8 failnum = (uint8)atoi(px);

        WorldPacket data(SMSG_CAST_FAILED, 5);
        data << uint32(133);
        data << uint8(failnum);
        handler->GetSession()->SendPacket(&data);

        return true;
    }

    static bool HandleDebugRAFError(ChatHandler* handler, const char* args)
    {
        uint32 id = strtoul(args, NULL, 10);
        if (!id || !handler->GetSession())
            return false;

        WorldPacket packet(SMSG_REFER_A_FRIEND_FAILURE, 4);
        packet << id;
        handler->GetSession()->SendPacket(&packet);
        return true;
    }

    static bool HandleSetPoiCommand(ChatHandler* handler, const char* args)
    {
        Player* pPlayer = handler->GetSession()->GetPlayer();
        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            return true;
        }

        if (!args)
            return false;

        char* icon_text = strtok((char*)args, " ");
        char* flags_text = strtok(NULL, " ");
        if (!icon_text || !flags_text)
            return false;

        uint32 icon = atol(icon_text);
        uint32 flags = atol(flags_text);

        sLog.outDetail("Command : POI, NPC = %u, icon = %u flags = %u", target->GetGUIDLow(), icon, flags);
        pPlayer->PlayerTalkClass->SendPointOfInterest(target->GetPositionX(), target->GetPositionY(), Poi_Icon(icon), flags, 30, "Test POI");
        return true;
    }

    static bool HandleSendQuestPartyMsgCommand(ChatHandler* handler, const char* args)
    {
        uint32 msg = atol((char*)args);
        handler->GetSession()->GetPlayer()->SendPushToPartyResponse(handler->GetSession()->GetPlayer(), msg);
        return true;
    }

    static bool HandleSendQuestInvalidMsgCommand(ChatHandler* handler, const char* args)
    {
        uint32 msg = atol((char*)args);
        handler->GetSession()->GetPlayer()->SendCanTakeQuestResponse(msg);
        return true;
    }

    static bool HandleEquipErrorCommand(ChatHandler* handler, const char* args)
    {
        if (!args)
            return false;

        uint8 msg = atoi(args);
        handler->GetSession()->GetPlayer()->SendEquipError(msg, 0, 0);
        return true;
    }

    static bool HandleSellErrorCommand(ChatHandler* handler, const char* args)
    {
        if (!args)
            return false;

        uint8 msg = atoi(args);
        handler->GetSession()->GetPlayer()->SendSellError(msg, 0, 0, 0);
        return true;
    }

    static bool HandleBuyErrorCommand(ChatHandler* handler, const char* args)
    {
        if (!args)
            return false;

        uint8 msg = atoi(args);
        handler->GetSession()->GetPlayer()->SendBuyError(msg, 0, 0, 0);
        return true;
    }

    static bool HandleSendOpcodeCommand(ChatHandler* handler, const char* /*args*/)
    {
        Unit* unit = handler->getSelectedUnit();
        Player* player = NULL;
        if (!unit || (unit->GetTypeId() != TYPEID_PLAYER))
            player = handler->GetSession()->GetPlayer();
        else
            player = unit->ToPlayer();
        if (!unit) unit = player;

        std::ifstream ifs("opcode.txt");
        if (ifs.bad())
            return false;

        uint32 opcode;
        ifs >> opcode;

        WorldPacket data(opcode, 0);

        while (!ifs.eof())
        {
            std::string type;
            ifs >> type;

            if (type == "")
                break;

            if (type == "uint8")
            {
                uint16 val1;
                ifs >> val1;
                data << uint8(val1);
            }
            else if (type == "uint16")
            {
                uint16 val2;
                ifs >> val2;
                data << val2;
            }
            else if (type == "uint32")
            {
                uint32 val3;
                ifs >> val3;
                data << val3;
            }
            else if (type == "uint64")
            {
                uint64 val4;
                ifs >> val4;
                data << val4;
            }
            else if (type == "float")
            {
                float val5;
                ifs >> val5;
                data << val5;
            }
            else if (type == "string")
            {
                std::string val6;
                ifs >> val6;
                data << val6;
            }
            else if (type == "pguid")
                data << unit->GetPackGUID();
            else if (type == "myguid")
                data << player->GetPackGUID();
            else if (type == "pos")
            {
                data << unit->GetPositionX();
                data << unit->GetPositionY();
                data << unit->GetPositionZ();
            }
            else if (type == "mypos")
            {
                data << player->GetPositionX();
                data << player->GetPositionY();
                data << player->GetPositionZ();
            }
            else
            {
                sLog.outDebug("Sending opcode: unknown type '%s'", type.c_str());
                break;
            }
        }
        ifs.close();
        sLog.outDebug("Sending opcode %u", data.GetOpcode());
        if (sLog.IsOutDebug())
            data.hexlike();
        unit->ToPlayer()->GetSession()->SendPacket(&data);
        handler->PSendSysMessage(LANG_COMMAND_OPCODESENT, data.GetOpcode(), unit->GetName());
        return true;
    }

    static bool HandleUpdateWorldStateCommand(ChatHandler* handler, const char* args)
    {
        char* w = strtok((char*)args, " ");
        char* s = strtok(NULL, " ");

        if (!w || !s)
            return false;

        uint32 world = (uint32)atoi(w);
        uint32 state = (uint32)atoi(s);
        handler->GetSession()->GetPlayer()->SendUpdateWorldState(world, state);
        return true;
    }

    static bool HandleSendChannelNotifyCommand(ChatHandler* handler, const char* args)
    {
        if (!args)
            return false;

        const char* name = "test";
        uint8 code = atoi(args);

        WorldPacket data(SMSG_CHANNEL_NOTIFY, (1 + 10));
        data << code;                                           // notify type
        data << name;                                           // channel name
        data << uint32(0);
        data << uint32(0);
        handler->GetSession()->SendPacket(&data);
        return true;
    }

    static bool HandleSendChatMsgCommand(ChatHandler* handler, const char* args)
    {
        if (!args)
            return false;

        const char* msg = "testtest";
        uint8 type = atoi(args);
        WorldPacket data;
        ChatHandler::FillMessageData(&data, handler->GetSession(), type, 0, "chan", handler->GetSession()->GetPlayer()->GetGUID(), msg, handler->GetSession()->GetPlayer());
        handler->GetSession()->SendPacket(&data);
        return true;
    }

    static bool HandleGetItemState(ChatHandler* handler, const char* args)
    {
        if (!args)
            return false;

        std::string state_str = args;

        ItemUpdateState state = ITEM_UNCHANGED;
        bool list_queue = false, check_all = false;
        if (state_str == "unchanged") state = ITEM_UNCHANGED;
        else if (state_str == "changed") state = ITEM_CHANGED;
        else if (state_str == "new") state = ITEM_NEW;
        else if (state_str == "removed") state = ITEM_REMOVED;
        else if (state_str == "queue") list_queue = true;
        else if (state_str == "check_all") check_all = true;
        else return false;

        Player* player = handler->getSelectedPlayer();
        if (!player) player = handler->GetSession()->GetPlayer();

        if (!list_queue && !check_all)
        {
            state_str = "The player has the following " + state_str + " items: ";
            handler->SendSysMessage(state_str.c_str());
            for (uint8 i = PLAYER_SLOT_START; i < PLAYER_SLOT_END; i++)
            {
                if (i >= BUYBACK_SLOT_START && i < BUYBACK_SLOT_END)
                    continue;

                Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
                if (!item) continue;
                if (!item->IsBag())
                {
                    if (item->GetState() == state)
                        handler->PSendSysMessage("bag: 255 slot: %d guid: %d owner: %d", item->GetSlot(), item->GetGUIDLow(), GUID_LOPART(item->GetOwnerGUID()));
                }
                else
                {
                    Bag* bag = (Bag*)item;
                    for (uint8 j = 0; j < bag->GetBagSize(); ++j)
                    {
                        Item* item = bag->GetItemByPos(j);
                        if (item && item->GetState() == state)
                            handler->PSendSysMessage("bag: 255 slot: %d guid: %d owner: %d", item->GetSlot(), item->GetGUIDLow(), GUID_LOPART(item->GetOwnerGUID()));
                    }
                }
            }
        }

        if (list_queue)
        {
            std::vector<Item* >& updateQueue = player->GetItemUpdateQueue();
            for (size_t i = 0; i < updateQueue.size(); i++)
            {
                Item* item = updateQueue[i];
                if (!item) continue;

                Bag* container = item->GetContainer();
                uint8 bag_slot = container ? container->GetSlot() : uint8(INVENTORY_SLOT_BAG_0);

                std::string st;
                switch (item->GetState())
                {
                case ITEM_UNCHANGED:
                    st = "unchanged";
                    break;
                case ITEM_CHANGED:
                    st = "changed";
                    break;
                case ITEM_NEW:
                    st = "new";
                    break;
                case ITEM_REMOVED:
                    st = "removed";
                    break;
                }

                handler->PSendSysMessage("bag: %d slot: %d guid: %d - state: %s", bag_slot, item->GetSlot(), item->GetGUIDLow(), st.c_str());
            }
            if (updateQueue.empty())
                handler->PSendSysMessage("updatequeue empty");
        }

        if (check_all)
        {
            bool error = false;
            std::vector<Item* >& updateQueue = player->GetItemUpdateQueue();
            for (uint8 i = PLAYER_SLOT_START; i < PLAYER_SLOT_END; i++)
            {
                if (i >= BUYBACK_SLOT_START && i < BUYBACK_SLOT_END)
                    continue;

                Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
                if (!item) continue;

                if (item->GetSlot() != i)
                {
                    handler->PSendSysMessage("item at slot %d, guid %d has an incorrect slot value: %d", i, item->GetGUIDLow(), item->GetSlot());
                    error = true;
                    continue;
                }

                if (item->GetOwnerGUID() != player->GetGUID())
                {
                    handler->PSendSysMessage("for the item at slot %d and itemguid %d, owner's guid (%d) and player's guid (%d) don't match!", item->GetSlot(), item->GetGUIDLow(), GUID_LOPART(item->GetOwnerGUID()), player->GetGUIDLow());
                    error = true;
                    continue;
                }

                if (Bag* container = item->GetContainer())
                {
                    handler->PSendSysMessage("item at slot: %d guid: %d has a container (slot: %d, guid: %d) but shouldnt!", item->GetSlot(), item->GetGUIDLow(), container->GetSlot(), container->GetGUIDLow());
                    error = true;
                    continue;
                }

                if (item->IsInUpdateQueue())
                {
                    uint16 qp = item->GetQueuePos();
                    if (qp > updateQueue.size())
                    {
                        handler->PSendSysMessage("item at slot: %d guid: %d has a queuepos (%d) larger than the update queue size! ", item->GetSlot(), item->GetGUIDLow(), qp);
                        error = true;
                        continue;
                    }

                    if (updateQueue[qp] == NULL)
                    {
                        handler->PSendSysMessage("item at slot: %d guid: %d has a queuepos (%d) that points to NULL in the queue!", item->GetSlot(), item->GetGUIDLow(), qp);
                        error = true;
                        continue;
                    }

                    if (updateQueue[qp] != item)
                    {
                        handler->PSendSysMessage("item at slot: %d guid: %d has has a queuepos (%d) that points to another item in the queue (bag: %d, slot: %d, guid: %d)", item->GetSlot(), item->GetGUIDLow(), qp, updateQueue[qp]->GetBagSlot(), updateQueue[qp]->GetSlot(), updateQueue[qp]->GetGUIDLow());
                        error = true;
                        continue;
                    }
                }
                else if (item->GetState() != ITEM_UNCHANGED)
                {
                    handler->PSendSysMessage("item at slot: %d guid: %d is not in queue but should be (state: %d)!", item->GetSlot(), item->GetGUIDLow(), item->GetState());
                    error = true;
                    continue;
                }

                if (item->IsBag())
                {
                    Bag* bag = (Bag*)item;
                    for (uint8 j = 0; j < bag->GetBagSize(); ++j)
                    {
                        Item* item = bag->GetItemByPos(j);
                        if (!item) continue;

                        if (item->GetSlot() != j)
                        {
                            handler->PSendSysMessage("the item in bag %d slot %d, guid %d has an incorrect slot value: %d", bag->GetSlot(), j, item->GetGUIDLow(), item->GetSlot());
                            error = true;
                            continue;
                        }

                        if (item->GetOwnerGUID() != player->GetGUID())
                        {
                            handler->PSendSysMessage("for the item in bag %d at slot %d and itemguid %d, owner's guid (%d) and player's guid (%d) don't match!", bag->GetSlot(), item->GetSlot(), item->GetGUIDLow(), GUID_LOPART(item->GetOwnerGUID()), player->GetGUIDLow());
                            error = true;
                            continue;
                        }

                        Bag* container = item->GetContainer();
                        if (!container)
                        {
                            handler->PSendSysMessage("the item in bag %d at slot %d with guid %d has no container!", bag->GetSlot(), item->GetSlot(), item->GetGUIDLow());
                            error = true;
                            continue;
                        }

                        if (container != bag)
                        {
                            handler->PSendSysMessage("the item in bag %d at slot %d with guid %d has a different container(slot %d guid %d)!", bag->GetSlot(), item->GetSlot(), item->GetGUIDLow(), container->GetSlot(), container->GetGUIDLow());
                            error = true;
                            continue;
                        }

                        if (item->IsInUpdateQueue())
                        {
                            uint16 qp = item->GetQueuePos();
                            if (qp > updateQueue.size())
                            {
                                handler->PSendSysMessage("item in bag: %d at slot: %d guid: %d has a queuepos (%d) larger than the update queue size! ", bag->GetSlot(), item->GetSlot(), item->GetGUIDLow(), qp);
                                error = true;
                                continue;
                            }

                            if (updateQueue[qp] == NULL)
                            {
                                handler->PSendSysMessage("item in bag: %d at slot: %d guid: %d has a queuepos (%d) that points to NULL in the queue!", bag->GetSlot(), item->GetSlot(), item->GetGUIDLow(), qp);
                                error = true;
                                continue;
                            }

                            if (updateQueue[qp] != item)
                            {
                                handler->PSendSysMessage("item in bag: %d at slot: %d guid: %d has has a queuepos (%d) that points to another item in the queue (bag: %d, slot: %d, guid: %d)", bag->GetSlot(), item->GetSlot(), item->GetGUIDLow(), qp, updateQueue[qp]->GetBagSlot(), updateQueue[qp]->GetSlot(), updateQueue[qp]->GetGUIDLow());
                                error = true;
                                continue;
                            }
                        }
                        else if (item->GetState() != ITEM_UNCHANGED)
                        {
                            handler->PSendSysMessage("item in bag: %d at slot: %d guid: %d is not in queue but should be (state: %d)!", bag->GetSlot(), item->GetSlot(), item->GetGUIDLow(), item->GetState());
                            error = true;
                            continue;
                        }
                    }
                }
            }

            for (size_t i = 0; i < updateQueue.size(); i++)
            {
                Item* item = updateQueue[i];
                if (!item) continue;

                if (item->GetOwnerGUID() != player->GetGUID())
                {
                    handler->PSendSysMessage("queue(%lu): for the an item (guid %d), the owner's guid (%d) and player's guid (%d) don't match!", i, item->GetGUIDLow(), GUID_LOPART(item->GetOwnerGUID()), player->GetGUIDLow());
                    error = true;
                    continue;
                }

                if (item->GetQueuePos() != i)
                {
                    handler->PSendSysMessage("queue(%lu): for the an item (guid %d), the queuepos doesn't match it's position in the queue!", i, item->GetGUIDLow());
                    error = true;
                    continue;
                }

                if (item->GetState() == ITEM_REMOVED) continue;
                Item* test = player->GetItemByPos(item->GetBagSlot(), item->GetSlot());

                if (test == NULL)
                {
                    handler->PSendSysMessage("queue(%lu): the bag(%d) and slot(%d) values for the item with guid %d are incorrect, the player doesn't have an item at that position!", i, item->GetBagSlot(), item->GetSlot(), item->GetGUIDLow());
                    error = true;
                    continue;
                }

                if (test != item)
                {
                    handler->PSendSysMessage("queue(%lu): the bag(%d) and slot(%d) values for the item with guid %d are incorrect, the item with guid %d is there instead!", i, item->GetBagSlot(), item->GetSlot(), item->GetGUIDLow(), test->GetGUIDLow());
                    error = true;
                    continue;
                }
            }
            if (!error)
                handler->SendSysMessage("All OK!");
        }

        return true;
    }

    static bool HandleUpdate(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 updateIndex;
        uint32 value;

        char* pUpdateIndex = strtok((char*)args, " ");

        Unit* chr = handler->getSelectedUnit();
        if (chr == NULL)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!pUpdateIndex)
            return true;
        updateIndex = atoi(pUpdateIndex);
        //check updateIndex
        if (chr->GetTypeId() == TYPEID_PLAYER)
        {
            if (updateIndex >= PLAYER_END) return true;
        }
        else
        {
            if (updateIndex >= UNIT_END) return true;
        }

        char* pvalue = strtok(NULL, " ");
        if (!pvalue)
        {
            value = chr->GetUInt32Value(updateIndex);

            handler->PSendSysMessage(LANG_UPDATE, chr->GetGUIDLow(), updateIndex, value);
            return true;
        }

        value = atoi(pvalue);

        handler->PSendSysMessage(LANG_UPDATE_CHANGE, chr->GetGUIDLow(), updateIndex, value);

        chr->SetUInt32Value(updateIndex, value);

        return true;
    }

    static bool HandleSetValue(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* px = strtok((char*)args, " ");
        char* py = strtok(NULL, " ");
        char* pz = strtok(NULL, " ");

        if (!px || !py)
            return false;

        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint64 guid = target->GetGUID();

        uint32 Opcode = (uint32)atoi(px);
        if (Opcode >= target->GetValuesCount())
        {
            handler->PSendSysMessage(LANG_TOO_BIG_INDEX, Opcode, GUID_LOPART(guid), target->GetValuesCount());
            return false;
        }
        uint32 iValue;
        float fValue;
        bool isint32 = true;
        if (pz)
            isint32 = (bool)atoi(pz);
        if (isint32)
        {
            iValue = (uint32)atoi(py);
            sLog.outDebug(handler->GetOregonString(LANG_SET_UINT), GUID_LOPART(guid), Opcode, iValue);
            target->SetUInt32Value(Opcode, iValue);
            handler->PSendSysMessage(LANG_SET_UINT_FIELD, GUID_LOPART(guid), Opcode, iValue);
        }
        else
        {
            fValue = (float)atof(py);
            sLog.outDebug(handler->GetOregonString(LANG_SET_FLOAT), GUID_LOPART(guid), Opcode, fValue);
            target->SetFloatValue(Opcode, fValue);
            handler->PSendSysMessage(LANG_SET_FLOAT_FIELD, GUID_LOPART(guid), Opcode, fValue);
        }

        return true;
    }

    static bool HandleGetValue(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* px = strtok((char*)args, " ");
        char* pz = strtok(NULL, " ");

        if (!px)
            return false;

        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint64 guid = target->GetGUID();

        uint32 Opcode = (uint32)atoi(px);
        if (Opcode >= target->GetValuesCount())
        {
            handler->PSendSysMessage(LANG_TOO_BIG_INDEX, Opcode, GUID_LOPART(guid), target->GetValuesCount());
            return false;
        }
        uint32 iValue;
        float fValue;
        bool isint32 = true;
        if (pz)
            isint32 = (bool)atoi(pz);

        if (isint32)
        {
            iValue = target->GetUInt32Value(Opcode);
            sLog.outDebug(handler->GetOregonString(LANG_GET_UINT), GUID_LOPART(guid), Opcode, iValue);
            handler->PSendSysMessage(LANG_GET_UINT_FIELD, GUID_LOPART(guid), Opcode, iValue);
        }
        else
        {
            fValue = target->GetFloatValue(Opcode);
            sLog.outDebug(handler->GetOregonString(LANG_GET_FLOAT), GUID_LOPART(guid), Opcode, fValue);
            handler->PSendSysMessage(LANG_GET_FLOAT_FIELD, GUID_LOPART(guid), Opcode, fValue);
        }

        return true;
    }

    static bool HandleMod32Value(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* px = strtok((char*)args, " ");
        char* py = strtok(NULL, " ");

        if (!px || !py)
            return false;

        uint32 Opcode = (uint32)atoi(px);
        int Value = atoi(py);

        if (Opcode >= handler->GetSession()->GetPlayer()->GetValuesCount())
        {
            handler->PSendSysMessage(LANG_TOO_BIG_INDEX, Opcode, handler->GetSession()->GetPlayer()->GetGUIDLow(), handler->GetSession()->GetPlayer()->GetValuesCount());
            return false;
        }

        sLog.outDebug(handler->GetOregonString(LANG_CHANGE_32BIT), Opcode, Value);

        int CurrentValue = (int)handler->GetSession()->GetPlayer()->GetUInt32Value(Opcode);

        CurrentValue += Value;
        handler->GetSession()->GetPlayer()->SetUInt32Value(Opcode, (uint32)CurrentValue);

        handler->PSendSysMessage(LANG_CHANGE_32BIT_FIELD, Opcode, CurrentValue);

        return true;
    }

    static bool HandleAnimCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 anim_id = atoi((char*)args);
        handler->GetSession()->GetPlayer()->HandleEmoteCommand(anim_id);
        return true;
    }

    static bool HandleGetLootRecipient(ChatHandler* handler, const char* /*args*/)
    {
        Creature* target = handler->getSelectedCreature();
        if (!target)
            return false;

        handler->PSendSysMessage("loot recipient: %s", target->hasLootRecipient() ? (target->GetLootRecipient() ? target->GetLootRecipient()->GetName() : "offline") : "no loot recipient");
        return true;
    }

    static bool HandleDebugArenaCommand(ChatHandler* handler, const char* /*args*/)
    {
        sBattlegroundMgr.ToggleArenaTesting();
        return true;
    }

    static bool HandleDebugBattlegroundCommand(ChatHandler* handler, const char* /*args*/)
    {
        sBattlegroundMgr.ToggleTesting();
        return true;
    }

    static bool HandleDebugThreatList(ChatHandler* handler, const char* /*args*/)
    {
        Creature* target = handler->getSelectedCreature();
        if (!target || target->IsTotem() || target->IsPet())
            return false;

        ThreatContainer::StorageType tlist = target->getThreatManager().getThreatList();
        ThreatContainer::StorageType::iterator itr;
        uint32 cnt = 0;
        handler->PSendSysMessage("Threat list of %s (guid %u)", target->GetName(), target->GetGUIDLow());
        for (itr = tlist.begin(); itr != tlist.end(); ++itr)
        {
            Unit* unit = (*itr)->getTarget();
            if (!unit)
                continue;
            ++cnt;
            handler->PSendSysMessage("   %u.   %s   (guid %u)  - threat %f", cnt, unit->GetName(), unit->GetGUIDLow(), (*itr)->getThreat());
        }
        handler->SendSysMessage("End of threat list.");
        return true;
    }

    static bool HandleSetInstanceDataCommand(ChatHandler* handler, const char* args)
    {
        if (!args || !handler->GetSession()->GetPlayer())
            return false;

        InstanceData* pInstance = handler->GetSession()->GetPlayer()->GetInstanceData();
        if (!pInstance)
        {
            handler->PSendSysMessage("You are not in scripted instance.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* id = strtok((char*)args, " ");
        char* data = strtok(NULL, " ");

        if (!id || !data)
            return false;

        uint32 _id = uint32(atoi(id));
        uint32 _data = uint32(atoi(data));

        pInstance->SetData(_id, _data);
        return true;
    }

    static bool HandleGetInstanceDataCommand(ChatHandler* handler, const char* args)
    {
        if (!args || !handler->GetSession()->GetPlayer())
            return false;

        InstanceData* pInstance = handler->GetSession()->GetPlayer()->GetInstanceData();
        if (!pInstance)
        {
            handler->PSendSysMessage("You are not in scripted instance.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* id = strtok((char*)args, " ");

        if (!id)
            return false;

        uint32 _id = uint32(atoi(id));

        handler->PSendSysMessage("Result: %u", pInstance->GetData(_id));
        return true;
    }

    static bool HandleSpellCrashTestCommand(ChatHandler* handler, const char* /*args*/)
    {
        /* Casts ALL spells. Useful for testing/founding out if/what spell causes server to crash */

        Unit* player = handler->GetSession()->GetPlayer();

        for (uint32 i = 31000; i <= 53085; ++i)
        {
            if (SpellEntry const* spellInfo = sSpellStore.LookupEntry(i))
            {
                sLog.outDebugInLine("Testing spell %u ... ", i);
                Creature* trigger = player->SummonTrigger(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), 600, NULL);
                trigger->CastSpell(trigger, spellInfo, true);
                trigger->DisappearAndDie(false);
                sLog.outDebug("OK!");
            }
        }

        handler->PSendSysMessage("ALL OK!");
        return true;
    }

    static bool HandlePartyResultCommand(ChatHandler* handler, const char* args)
    {
        if (!handler->GetSession())
            return false;

        int operation = atoi(args);
        int result = atoi(std::max<const char*>(strchr(args, ' ') + 1, ""));

        handler->GetSession()->SendPartyResult(PartyOperation(operation), "", PartyResult(result));
        return true;
    }

    static bool HandleDebugAnimationCommand(ChatHandler* handler, const char* args)
    {
        if (!handler->GetSession() || !handler->GetSession()->GetPlayer())
            return false;

        WorldPacket data(SMSG_GAMEOBJECT_CUSTOM_ANIM, 12);
        data << (handler->GetSession()->GetPlayer()->GetSelection() ? handler->GetSession()->GetPlayer()->GetSelection() : handler->GetSession()->GetPlayer()->GetGUID());
        data << uint32(atoi(args));
        handler->GetSession()->SendPacket(&data);

        return true;
    }

    static bool HandleDebugPlayCinematicCommand(ChatHandler* handler, const char* args)
    {
        // USAGE: .debug play cinematic #cinematicid
        // #cinematicid - ID decimal number from CinemaicSequences.dbc (1st column)
        if (!*args)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 dwId = atoi((char*)args);

        if (!sCinematicSequencesStore.LookupEntry(dwId))
        {
            handler->PSendSysMessage(LANG_CINEMATIC_NOT_EXIST, dwId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Dump camera locations
        if (CinematicSequencesEntry const* cineSeq = sCinematicSequencesStore.LookupEntry(dwId))
        {
            std::unordered_map<uint32, FlyByCameraCollection>::const_iterator itr = sFlyByCameraStore.find(cineSeq->cinematicCamera);
            if (itr != sFlyByCameraStore.end())
            {
                handler->PSendSysMessage("Waypoints for sequence %u, camera %u", dwId, cineSeq->cinematicCamera);
                uint32 count = 1;
                for (FlyByCamera cam : itr->second)
                {
                    handler->PSendSysMessage("%02u - %7ums [%f, %f, %f] Facing %f (%f degrees)", count, cam.timeStamp, cam.locations.x, cam.locations.y, cam.locations.z, cam.locations.w, cam.locations.w * (180 / M_PI));
                    count++;
                }
                handler->PSendSysMessage("%u waypoints dumped", itr->second.size());
            }
        }

        handler->GetSession()->GetPlayer()->SendCinematicStart(dwId);
        return true;
    }

    //Play sound
    static bool HandleDebugPlaySoundCommand(ChatHandler* handler, const char* args)
    {
        // USAGE: .debug playsound #soundid
        // #soundid - ID decimal number from SoundEntries.dbc (1st column)
        if (!*args)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 dwSoundId = atoi((char*)args);

        if (!sSoundEntriesStore.LookupEntry(dwSoundId))
        {
            handler->PSendSysMessage(LANG_SOUND_NOT_EXIST, dwSoundId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Unit* unit = handler->getSelectedUnit();
        if (!unit)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (handler->GetSession()->GetPlayer()->GetSelection())
            unit->PlayDistanceSound(dwSoundId, handler->GetSession()->GetPlayer());
        else
            unit->PlayDirectSound(dwSoundId, handler->GetSession()->GetPlayer());

        handler->PSendSysMessage(LANG_YOU_HEAR_SOUND, dwSoundId);
        return true;
    }

};

void AddSC_debug_commandscript()
{
    new debug_commandscript();
}