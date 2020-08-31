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

class gm_commandscript : public CommandScript
{
public:
    gm_commandscript() : CommandScript("gm_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> gmCommandTable =
        {
            { "chat",           SEC_MODERATOR,      false, &HandleGMChatCommand,              "" },
            { "ingame",         SEC_PLAYER,         true,  &HandleGMListIngameCommand,        "" },
            { "list",           SEC_ADMINISTRATOR,  true,  &HandleGMListFullCommand,          "" },
            { "visible",        SEC_MODERATOR,      false, &HandleVisibleCommand,             "" },
            { "fly",            SEC_ADMINISTRATOR,  false, &HandleFlyModeCommand,             "" },
            { "",               SEC_MODERATOR,      false, &HandleGMmodeCommand,              "" }
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "gm",             SEC_MODERATOR,      false, nullptr,                           "", gmCommandTable }
        };
        return commandTable;
    }

    static bool HandleGMChatCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
        {
            WorldSession* session = handler->GetSession();
            if (session->GetPlayer()->isGMChat())
                session->SendNotification(LANG_GM_CHAT_ON);
            else
                session->SendNotification(LANG_GM_CHAT_OFF);
            return true;
        }

        std::string argstr = (char*)args;

        if (argstr == "on")
        {
            handler->GetSession()->GetPlayer()->SetGMChat(true);
            handler->GetSession()->SendNotification(LANG_GM_CHAT_ON);
            return true;
        }

        if (argstr == "off")
        {
            handler->GetSession()->GetPlayer()->SetGMChat(false);
            handler->GetSession()->SendNotification(LANG_GM_CHAT_OFF);
            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }

    static bool HandleGMListIngameCommand(ChatHandler* handler, char const* /*args*/)
    {
        bool gmOnline = false;

        ObjectAccessor::Guard guard(*HashMapHolder<Player>::GetLock());
        HashMapHolder<Player>::MapType& m = ObjectAccessor::Instance().GetPlayers();
        for (HashMapHolder<Player>::MapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        {
            if (itr->second->GetSession()->GetSecurity() &&
                (itr->second->IsGameMaster() || sWorld.getConfig(CONFIG_GM_IN_GM_LIST)) &&
                (!handler->GetSession() || itr->second->IsVisibleGloballyFor(handler->GetSession()->GetPlayer())))
            {
                if (!gmOnline)
                {
                    handler->SendSysMessage(LANG_GMS_ON_SRV);
                    gmOnline = true;
                }

                handler->SendSysMessage(itr->second->GetName());
            }
        }

        if (!gmOnline)
            handler->SendSysMessage(LANG_GMS_NOT_LOGGED);

        return true;
    }

    static bool HandleGMListFullCommand(ChatHandler* handler, char const* /*args*/)
    {
        // Get the accounts with GM Level >0
        QueryResult_AutoPtr result = LoginDatabase.Query("SELECT a.username,aa.gmlevel FROM account a, account_access aa WHERE a.id=aa.id AND aa.gmlevel > 0");
        if (result)
        {
            handler->SendSysMessage(LANG_GMLIST);
            handler->SendSysMessage(" ======================== ");
            handler->SendSysMessage(LANG_GMLIST_HEADER);
            handler->SendSysMessage(" ======================== ");

            // Circle through them. Display username and GM level
            do
            {
                Field* fields = result->Fetch();
                handler->PSendSysMessage("|%15s|%6s|", fields[0].GetString(), fields[1].GetString());
            } while (result->NextRow());

            handler->PSendSysMessage(" ======================== ");
        }
        else
            handler->PSendSysMessage(LANG_GMLIST_EMPTY);
        return true;
    }

    static bool HandleVisibleCommand(ChatHandler* handler, char const* args)
    {
        Player* _player = handler->GetSession()->GetPlayer();

        if (!*args)
        {
            handler->PSendSysMessage(LANG_YOU_ARE, _player->isGMVisible() ? handler->GetOregonString(LANG_VISIBLE) : handler->GetOregonString(LANG_INVISIBLE));
            return true;
        }

        std::string argstr = (char*)args;

        if (argstr == "on")
        {
            if (_player->HasAura(SPELL_INVISIBILITY, 0))
                _player->RemoveAurasDueToSpell(SPELL_INVISIBILITY);

            _player->SetGMVisible(true);
            _player->UpdateObjectVisibility();
            handler->GetSession()->SendNotification(LANG_INVISIBLE_VISIBLE);
            return true;
        }

        if (argstr == "off")
        {
            _player->AddAura(SPELL_INVISIBILITY, _player);
            _player->SetGMVisible(false);
            _player->UpdateObjectVisibility();
            handler->GetSession()->SendNotification(LANG_INVISIBLE_INVISIBLE);
            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }

    static bool HandleFlyModeCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Unit* unit = handler->getSelectedUnit();
        if (!unit || (unit->GetTypeId() != TYPEID_PLAYER))
            unit = handler->GetSession()->GetPlayer();

        WorldPacket data(12);
        if (strncmp(args, "on", 3) == 0)
            data.SetOpcode(SMSG_MOVE_SET_CAN_FLY);
        else if (strncmp(args, "off", 4) == 0)
            data.SetOpcode(SMSG_MOVE_UNSET_CAN_FLY);
        else
        {
            handler->SendSysMessage(LANG_USE_BOL);
            return false;
        }
        data << unit->GetPackGUID();
        data << uint32(0);                                      // unknown
        unit->SendMessageToSet(&data, true);
        handler->PSendSysMessage(LANG_COMMAND_FLYMODE_STATUS, unit->GetName(), args);
        return true;
    }

    static bool HandleGMmodeCommand(ChatHandler* handler, const char* args)
    {
        Player* _player = handler->GetSession()->GetPlayer();

        if (!*args)
        {
            handler->GetSession()->SendNotification(_player->IsGameMaster() ? LANG_GM_ON : LANG_GM_OFF);
            return true;
        }

        std::string argstr = (char*)args;

        if (argstr == "on")
        {
            _player->SetGameMaster(true);
            handler->GetSession()->SendNotification(LANG_GM_ON);
            _player->UpdateTriggerVisibility();
#ifdef _DEBUG_VMAPS
            VMAP::IVMapManager* vMapManager = VMAP::VMapFactory::createOrGetVMapManager();
            vMapManager->processCommand("stoplog");
#endif
            return true;
        }

        if (argstr == "off")
        {
            _player->SetGameMaster(false);
            handler->GetSession()->SendNotification(LANG_GM_OFF);
            _player->UpdateTriggerVisibility();
#ifdef _DEBUG_VMAPS
            VMAP::IVMapManager* vMapManager = VMAP::VMapFactory::createOrGetVMapManager();
            vMapManager->processCommand("startlog");
#endif
            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }

};

void AddSC_gm_commandscript()
{
    new gm_commandscript();
}