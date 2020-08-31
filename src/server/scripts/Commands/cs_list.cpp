#include "ScriptMgr.h"
#include "Chat.h"
#include "Language.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"

class list_commandscript : public CommandScript
{
public:
    list_commandscript() : CommandScript("list_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> listCommandTable =
        {
            { "creature",       SEC_MODERATOR,  true,  &HandleListCreatureCommand,          "" },
            { "item",           SEC_MODERATOR,  true,  &HandleListItemCommand,              "" },
            { "object",         SEC_MODERATOR,  true,  &HandleListObjectCommand,            "" },
            { "gobject",        SEC_MODERATOR,  true,  &HandleListObjectCommand,            "" },
            { "auras",          SEC_MODERATOR,  false, &HandleListAurasCommand,             "" }
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "list",          SEC_MODERATOR,   true, nullptr,                                 "", listCommandTable }
        };
        return commandTable;
    }

    static bool HandleListCreatureCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // number or [name] Shift-click form |color|Hcreature_entry:creature_id|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hcreature_entry");
        if (!cId)
            return false;

        uint32 cr_id = atol(cId);
        if (!cr_id)
        {
            handler->PSendSysMessage(LANG_COMMAND_INVALIDCREATUREID, cr_id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        CreatureInfo const* cInfo = sObjectMgr.GetCreatureTemplate(cr_id);
        if (!cInfo)
        {
            handler->PSendSysMessage(LANG_COMMAND_INVALIDCREATUREID, cr_id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* c_count = strtok(NULL, " ");
        int count = c_count ? atol(c_count) : 10;

        if (count < 0)
            return false;

        uint32 cr_count = 0;
        QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT COUNT(guid) FROM creature WHERE id='%u'", cr_id);
        if (result)
            cr_count = (*result)[0].GetUInt32();

        if (handler->GetSession())
        {
            Player* pl = handler->GetSession()->GetPlayer();
            result = WorldDatabase.PQuery("SELECT guid, position_x, position_y, position_z, map, (POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2)) AS order_ FROM creature WHERE id = '%u' ORDER BY order_ ASC LIMIT %u",
                pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), cr_id, uint32(count));
        }
        else
            result = WorldDatabase.PQuery("SELECT guid, position_x, position_y, position_z, map FROM creature WHERE id = '%u' LIMIT %u",
                cr_id, uint32(count));

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 guid = fields[0].GetUInt32();
                float x = fields[1].GetFloat();
                float y = fields[2].GetFloat();
                float z = fields[3].GetFloat();
                int mapid = fields[4].GetUInt16();

                if (handler->GetSession())
                    handler->PSendSysMessage(LANG_CREATURE_LIST_CHAT, guid, guid, cInfo->Name, x, y, z, mapid);
                else
                    handler->PSendSysMessage(LANG_CREATURE_LIST_CONSOLE, guid, cInfo->Name, x, y, z, mapid);
            } while (result->NextRow());
        }

        handler->PSendSysMessage(LANG_COMMAND_LISTCREATUREMESSAGE, cr_id, cr_count);
        return true;
    }

    static bool HandleListItemCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* cId = handler->extractKeyFromLink((char*)args, "Hitem");
        if (!cId)
            return false;

        uint32 item_id = atol(cId);
        if (!item_id)
        {
            handler->PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, item_id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        ItemTemplate const* itemProto = sObjectMgr.GetItemTemplate(item_id);
        if (!itemProto)
        {
            handler->PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, item_id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* c_count = strtok(NULL, " ");
        int count = c_count ? atol(c_count) : 10;

        if (count < 0)
            return false;

        // inventory case
        uint32 inv_count = 0;
        QueryResult_AutoPtr result = CharacterDatabase.PQuery("SELECT COUNT(item_template) FROM character_inventory WHERE item_template='%u'", item_id);
        if (result)
            inv_count = (*result)[0].GetUInt32();

        result = CharacterDatabase.PQuery(
            //          0        1             2             3        4                  5
            "SELECT ci.item, cibag.slot AS bag, ci.slot, ci.guid, characters.account,characters.name "
            "FROM character_inventory AS ci LEFT JOIN character_inventory AS cibag ON (cibag.item=ci.bag),characters "
            "WHERE ci.item_template='%u' AND ci.guid = characters.guid LIMIT %u ",
            item_id, uint32(count));

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 item_guid = fields[0].GetUInt32();
                uint32 item_bag = fields[1].GetUInt32();
                uint32 item_slot = fields[2].GetUInt32();
                uint32 owner_guid = fields[3].GetUInt32();
                uint32 owner_acc = fields[4].GetUInt32();
                std::string owner_name = fields[5].GetCppString();

                char const* item_pos = 0;
                if (Player::IsEquipmentPos(item_bag, item_slot))
                    item_pos = "[equipped]";
                else if (Player::IsInventoryPos(item_bag, item_slot))
                    item_pos = "[in inventory]";
                else if (Player::IsBankPos(item_bag, item_slot))
                    item_pos = "[in bank]";
                else
                    item_pos = "";

                handler->PSendSysMessage(LANG_ITEMLIST_SLOT,
                    item_guid, owner_name.c_str(), owner_guid, owner_acc, item_pos);
            } while (result->NextRow());

            int64 res_count = result->GetRowCount();

            if (count > res_count)
                count -= res_count;
            else if (count)
                count = 0;
        }

        // mail case
        uint32 mail_count = 0;
        result = CharacterDatabase.PQuery("SELECT COUNT(item_template) FROM mail_items WHERE item_template='%u'", item_id);
        if (result)
            mail_count = (*result)[0].GetUInt32();

        if (count > 0)
        {
            result = CharacterDatabase.PQuery(
                //          0                     1            2              3               4            5               6
                "SELECT mail_items.item_guid, mail.sender, mail.receiver, char_s.account, char_s.name, char_r.account, char_r.name "
                "FROM mail,mail_items,characters as char_s,characters as char_r "
                "WHERE mail_items.item_template='%u' AND char_s.guid = mail.sender AND char_r.guid = mail.receiver AND mail.id=mail_items.mail_id LIMIT %u",
                item_id, uint32(count));
        }
        else
            result = QueryResult_AutoPtr(NULL);

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 item_guid = fields[0].GetUInt32();
                uint32 item_s = fields[1].GetUInt32();
                uint32 item_r = fields[2].GetUInt32();
                uint32 item_s_acc = fields[3].GetUInt32();
                std::string item_s_name = fields[4].GetCppString();
                uint32 item_r_acc = fields[5].GetUInt32();
                std::string item_r_name = fields[6].GetCppString();

                char const* item_pos = "[in mail]";

                handler->PSendSysMessage(LANG_ITEMLIST_MAIL,
                    item_guid, item_s_name.c_str(), item_s, item_s_acc, item_r_name.c_str(), item_r, item_r_acc, item_pos);
            } while (result->NextRow());

            int64 res_count = result->GetRowCount();

            if (count > res_count)
                count -= res_count;
            else if (count)
                count = 0;
        }

        // auction case
        uint32 auc_count = 0;
        result = CharacterDatabase.PQuery("SELECT COUNT(item_template) FROM auctionhouse WHERE item_template='%u'", item_id);
        if (result)
            auc_count = (*result)[0].GetUInt32();

        if (count > 0)
        {
            result = CharacterDatabase.PQuery(
                //           0                      1                       2                   3
                "SELECT  auctionhouse.itemguid, auctionhouse.itemowner, characters.account, characters.name "
                "FROM auctionhouse,characters WHERE auctionhouse.item_template='%u' AND characters.guid = auctionhouse.itemowner LIMIT %u",
                item_id, uint32(count));
        }
        else
            result = QueryResult_AutoPtr(NULL);

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 item_guid = fields[0].GetUInt32();
                uint32 owner = fields[1].GetUInt32();
                uint32 owner_acc = fields[2].GetUInt32();
                std::string owner_name = fields[3].GetCppString();

                char const* item_pos = "[in auction]";

                handler->PSendSysMessage(LANG_ITEMLIST_AUCTION, item_guid, owner_name.c_str(), owner, owner_acc, item_pos);
            } while (result->NextRow());
        }

        // guild bank case
        uint32 guild_count = 0;
        result = CharacterDatabase.PQuery("SELECT COUNT(item_entry) FROM guild_bank_item WHERE item_entry='%u'", item_id);
        if (result)
            guild_count = (*result)[0].GetUInt32();

        result = CharacterDatabase.PQuery(
            //      0             1           2
            "SELECT gi.item_guid, gi.guildid, guild.name "
            "FROM guild_bank_item AS gi, guild WHERE gi.item_entry='%u' AND gi.guildid = guild.guildid LIMIT %u ",
            item_id, uint32(count));

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 item_guid = fields[0].GetUInt32();
                uint32 guild_guid = fields[1].GetUInt32();
                std::string guild_name = fields[2].GetCppString();

                char const* item_pos = "[in guild bank]";

                handler->PSendSysMessage(LANG_ITEMLIST_GUILD, item_guid, guild_name.c_str(), guild_guid, item_pos);
            } while (result->NextRow());

            int64 res_count = result->GetRowCount();

            if (count > res_count)
                count -= res_count;
            else if (count)
                count = 0;
        }

        if (inv_count + mail_count + auc_count + guild_count == 0)
        {
            handler->SendSysMessage(LANG_COMMAND_NOITEMFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_COMMAND_LISTITEMMESSAGE, item_id, inv_count + mail_count + auc_count + guild_count, inv_count, mail_count, auc_count, guild_count);

        return true;
    }

    static bool HandleListObjectCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // number or [name] Shift-click form |color|Hgameobject_entry:go_id|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hgameobject_entry");
        if (!cId)
            return false;

        uint32 go_id = atol(cId);
        if (!go_id)
        {
            handler->PSendSysMessage(LANG_COMMAND_LISTOBJINVALIDID, go_id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        GameObjectInfo const* gInfo = sObjectMgr.GetGameObjectInfo(go_id);
        if (!gInfo)
        {
            handler->PSendSysMessage(LANG_COMMAND_LISTOBJINVALIDID, go_id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* c_count = strtok(NULL, " ");
        int count = c_count ? atol(c_count) : 10;

        if (count < 0)
            return false;

        uint32 obj_count = 0;
        QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT COUNT(guid) FROM gameobject WHERE id='%u'", go_id);
        if (result)
            obj_count = (*result)[0].GetUInt32();

        if (handler->GetSession())
        {
            Player* pl = handler->GetSession()->GetPlayer();
            result = WorldDatabase.PQuery("SELECT guid, position_x, position_y, position_z, map, id, (POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2)) AS order_ FROM gameobject WHERE id = '%u' ORDER BY order_ ASC LIMIT %u",
                pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), go_id, uint32(count));
        }
        else
            result = WorldDatabase.PQuery("SELECT guid, position_x, position_y, position_z, map, id FROM gameobject WHERE id = '%u' LIMIT %u",
                go_id, uint32(count));

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 guid = fields[0].GetUInt32();
                float x = fields[1].GetFloat();
                float y = fields[2].GetFloat();
                float z = fields[3].GetFloat();
                int mapid = fields[4].GetUInt16();
                uint32 entry = fields[5].GetUInt32();

                if (handler->GetSession())
                    handler->PSendSysMessage(LANG_GO_LIST_CHAT, guid, entry, guid, gInfo->name, x, y, z, mapid);
                else
                    handler->PSendSysMessage(LANG_GO_LIST_CONSOLE, guid, gInfo->name, x, y, z, mapid);
            } while (result->NextRow());
        }

        handler->PSendSysMessage(LANG_COMMAND_LISTOBJMESSAGE, go_id, obj_count);
        return true;
    }

    static bool HandleListAurasCommand(ChatHandler* handler, const char* /*args*/)
    {
        Unit* unit = handler->getSelectedUnit();
        if (!unit)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char const* talentStr = handler->GetOregonString(LANG_TALENT);
        char const* passiveStr = handler->GetOregonString(LANG_PASSIVE);

        Unit::AuraMap const& uAuras = unit->GetAuras();
        handler->PSendSysMessage(LANG_COMMAND_TARGET_LISTAURAS, uAuras.size());
        for (Unit::AuraMap::const_iterator itr = uAuras.begin(); itr != uAuras.end(); ++itr)
        {
            bool talent = GetTalentSpellCost(itr->second->GetId()) > 0;
            handler->PSendSysMessage(LANG_COMMAND_TARGET_AURADETAIL, itr->second->GetId(), itr->second->GetEffIndex(),
                itr->second->GetModifier()->m_auraname, itr->second->GetAuraDuration(), itr->second->GetAuraMaxDuration(),
                itr->second->GetSpellProto()->SpellName[handler->GetSession()->GetSessionDbcLocale()],
                (itr->second->IsPassive() ? passiveStr : ""), (talent ? talentStr : ""),
                IS_PLAYER_GUID(itr->second->GetCasterGUID()) ? "player" : "creature", GUID_LOPART(itr->second->GetCasterGUID()));
        }
        for (uint16 i = 0; i < TOTAL_AURAS; ++i)
        {
            Unit::AuraList const& uAuraList = unit->GetAurasByType(AuraType(i));
            if (uAuraList.empty()) continue;
            handler->PSendSysMessage(LANG_COMMAND_TARGET_LISTAURATYPE, uAuraList.size(), i);
            for (Unit::AuraList::const_iterator itr = uAuraList.begin(); itr != uAuraList.end(); ++itr)
            {
                bool talent = GetTalentSpellCost((*itr)->GetId()) > 0;
                handler->PSendSysMessage(LANG_COMMAND_TARGET_AURASIMPLE, (*itr)->GetId(), (*itr)->GetEffIndex(),
                    (*itr)->GetSpellProto()->SpellName[handler->GetSession()->GetSessionDbcLocale()], ((*itr)->IsPassive() ? passiveStr : ""), (talent ? talentStr : ""),
                    IS_PLAYER_GUID((*itr)->GetCasterGUID()) ? "player" : "creature", GUID_LOPART((*itr)->GetCasterGUID()));
            }
        }
        return true;
    }
};

void AddSC_list_commandscript()
{
    new list_commandscript();
}