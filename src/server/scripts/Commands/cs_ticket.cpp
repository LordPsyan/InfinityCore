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
#include "TicketMgr.h"

class ticket_commandscript : public CommandScript
{
public:
    ticket_commandscript() : CommandScript("ticket_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> ticketCommandTable =
        {
            { "list",           SEC_MODERATOR,      false, &HandleGMTicketListCommand,              "" },
            { "onlinelist",     SEC_MODERATOR,      false, &HandleGMTicketListOnlineCommand,        "" },
            { "viewname",       SEC_MODERATOR,      false, &HandleGMTicketGetByNameCommand,         "" },
            { "viewid",         SEC_MODERATOR,      false, &HandleGMTicketGetByIdCommand,           "" },
            { "close",          SEC_MODERATOR,      false, &HandleGMTicketCloseByIdCommand,         "" },
            { "closedlist",     SEC_MODERATOR,      false, &HandleGMTicketListClosedCommand,        "" },
            { "delete",         SEC_ADMINISTRATOR,  false, &HandleGMTicketDeleteByIdCommand,        "" },
            { "assign",         SEC_MODERATOR,      false, &HandleGMTicketAssignToCommand,          "" },
            { "unassign",       SEC_MODERATOR,      false, &HandleGMTicketUnAssignCommand,          "" },
            { "comment",        SEC_MODERATOR,      false, &HandleGMTicketCommentCommand,           "" }
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "ticket",         SEC_MODERATOR,      false, nullptr,                                 "", ticketCommandTable }
        };
        return commandTable;
    };

    static bool HandleGMTicketListCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->SendSysMessage(LANG_COMMAND_TICKETSHOWLIST);
        for (GmTicketList::iterator itr = ticketmgr.GM_TicketList.begin(); itr != ticketmgr.GM_TicketList.end(); ++itr)
        {
            if ((*itr)->closed != 0)
                continue;
            std::string gmname;
            std::stringstream ss;
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTGUID, (*itr)->guid);
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTNAME, (*itr)->name.c_str());
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTAGECREATE, (secsToTimeString(time(NULL) - (*itr)->createtime, true, false)).c_str());
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTAGE, (secsToTimeString(time(NULL) - (*itr)->timestamp, true, false)).c_str());
            if (sObjectMgr.GetPlayerNameByGUID((*itr)->assignedToGM, gmname))
                ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTASSIGNEDTO, gmname.c_str());
            handler->SendSysMessage(ss.str().c_str());
        }
        return true;
    }

    static bool HandleGMTicketListOnlineCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->SendSysMessage(LANG_COMMAND_TICKETSHOWONLINELIST);
        for (GmTicketList::iterator itr = ticketmgr.GM_TicketList.begin(); itr != ticketmgr.GM_TicketList.end(); ++itr)
        {
            if ((*itr)->closed != 0 || !sObjectMgr.GetPlayer((*itr)->playerGuid))
                continue;

            std::string gmname;
            std::stringstream ss;
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTGUID, (*itr)->guid);
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTNAME, (*itr)->name.c_str());
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTAGECREATE, (secsToTimeString(time(NULL) - (*itr)->createtime, true, false)).c_str());
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTAGE, (secsToTimeString(time(NULL) - (*itr)->timestamp, true, false)).c_str());
            if (sObjectMgr.GetPlayerNameByGUID((*itr)->assignedToGM, gmname))
                ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTASSIGNEDTO, gmname.c_str());
            handler->SendSysMessage(ss.str().c_str());
        }
        return true;
    }

    static bool HandleGMTicketGetByNameCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        GM_Ticket* ticket = ticketmgr.GetGMTicketByName(args);
        if (!ticket)
        {
            handler->SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
            return true;
        }

        ticket->viewed = true;
        Player* plr = sObjectMgr.GetPlayer(ticket->playerGuid);
        if (plr && plr->IsInWorld())
        {
            // tell client to update display of ticket status
            WorldPacket data(SMSG_GM_TICKET_STATUS_UPDATE, 4);
            data << uint32(1);
            plr->GetSession()->SendPacket(&data);
        }
        ticketmgr.SaveGMTicket(ticket); // update database

        std::string gmname;
        std::stringstream ss;
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTGUID, ticket->guid);
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTNAME, ticket->name.c_str());
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTAGECREATE, (secsToTimeString(time(NULL) - ticket->createtime, true, false)).c_str());
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTAGE, (secsToTimeString(time(NULL) - ticket->timestamp, true, false)).c_str());
        if (sObjectMgr.GetPlayerNameByGUID(ticket->assignedToGM, gmname))
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTASSIGNEDTO, gmname.c_str());
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTMESSAGE, ticket->message.c_str());
        if (ticket->comment != "")
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTCOMMENT, ticket->comment.c_str());
        handler->SendSysMessage(ss.str().c_str());
        return true;
    }

    static bool HandleGMTicketGetByIdCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint64 tguid = atoi(args);
        GM_Ticket* ticket = ticketmgr.GetGMTicket(tguid);
        if (!ticket)
        {
            handler->SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
            return true;
        }

        ticket->viewed = true;
        Player* plr = sObjectMgr.GetPlayer(ticket->playerGuid);
        if (plr && plr->IsInWorld())
        {
            // tell client to update display of ticket status
            WorldPacket data(SMSG_GM_TICKET_STATUS_UPDATE, 4);
            data << uint32(1);
            plr->GetSession()->SendPacket(&data);
        }
        ticketmgr.SaveGMTicket(ticket); // update database

        std::string gmname;
        std::stringstream ss;
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTGUID, ticket->guid);
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTNAME, ticket->name.c_str());
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTAGECREATE, (secsToTimeString(time(NULL) - ticket->createtime, true, false)).c_str());
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTAGE, (secsToTimeString(time(NULL) - ticket->timestamp, true, false)).c_str());
        if (sObjectMgr.GetPlayerNameByGUID(ticket->assignedToGM, gmname))
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTASSIGNEDTO, gmname.c_str());
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTMESSAGE, ticket->message.c_str());
        if (ticket->comment != "")
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTCOMMENT, ticket->comment.c_str());
        handler->SendSysMessage(ss.str().c_str());
        return true;
    }

    static bool HandleGMTicketCloseByIdCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint64 tguid = atoi(args);
        GM_Ticket* ticket = ticketmgr.GetGMTicket(tguid);
        if (!ticket || ticket->closed != 0)
        {
            handler->SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
            return true;
        }
        if (ticket->assignedToGM != 0 && ticket->assignedToGM != handler->GetSession()->GetPlayer()->GetGUID())
        {
            handler->PSendSysMessage(LANG_COMMAND_TICKETCANNOTCLOSE, ticket->guid);
            return true;
        }
        std::stringstream ss;
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTGUID, ticket->guid);
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTNAME, ticket->name.c_str());
        ss << PGetParseString(handler, LANG_COMMAND_TICKETCLOSED, handler->GetSession()->GetPlayer()->GetName());
        handler->SendGlobalGMSysMessage(ss.str().c_str());
        Player* plr = sObjectMgr.GetPlayer(ticket->playerGuid);
        ticketmgr.RemoveGMTicket(ticket->guid, handler->GetSession()->GetPlayer()->GetGUID());

        if (!plr || !plr->IsInWorld())
            return true;

        if ((float)rand_chance() < sWorld.getConfig(CONFIG_CHANCE_OF_GM_SURVEY))
        {
            // send survey
            WorldPacket data(SMSG_GM_TICKET_STATUS_UPDATE, 4);
            data << uint32(3); // 3 displays survey
            plr->GetSession()->SendPacket(&data);
        }
        else
        {
            // send abandon ticket
            WorldPacket data(SMSG_GMTICKET_DELETETICKET, 4);
            data << uint32(9);
            plr->GetSession()->SendPacket(&data);
        }

        return true;
    }

    static bool HandleGMTicketListClosedCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->SendSysMessage(LANG_COMMAND_TICKETSHOWCLOSEDLIST);
        for (GmTicketList::iterator itr = ticketmgr.GM_TicketList.begin(); itr != ticketmgr.GM_TicketList.end(); ++itr)
        {
            if ((*itr)->closed == 0)
                continue;

            std::string gmname;
            std::stringstream ss;
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTGUID, (*itr)->guid);
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTNAME, (*itr)->name.c_str());
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTAGECREATE, (secsToTimeString(time(NULL) - (*itr)->createtime, true, false)).c_str());
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTAGE, (secsToTimeString(time(NULL) - (*itr)->timestamp, true, false)).c_str());
            if (sObjectMgr.GetPlayerNameByGUID((*itr)->assignedToGM, gmname))
                ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTASSIGNEDTO, gmname.c_str());
            handler->SendSysMessage(ss.str().c_str());
        }
        return true;
    }

    static bool HandleGMTicketDeleteByIdCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;
        uint64 ticketGuid = atoi(args);
        GM_Ticket* ticket = ticketmgr.GetGMTicket(ticketGuid);

        if (!ticket)
        {
            handler->SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
            return true;
        }
        if (ticket->closed == 0)
        {
            handler->SendSysMessage(LANG_COMMAND_TICKETCLOSEFIRST);
            return true;
        }

        std::stringstream ss;
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTGUID, ticket->guid);
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTNAME, ticket->name.c_str());
        ss << PGetParseString(handler, LANG_COMMAND_TICKETDELETED, handler->GetSession()->GetPlayer()->GetName());
        handler->SendGlobalGMSysMessage(ss.str().c_str());
        Player* plr = sObjectMgr.GetPlayer(ticket->playerGuid);
        ticketmgr.DeleteGMTicketPermanently(ticket->guid);
        if (plr && plr->IsInWorld())
        {
            // Force abandon ticket
            WorldPacket data(SMSG_GMTICKET_DELETETICKET, 4);
            data << uint32(9);
            plr->GetSession()->SendPacket(&data);
        }

        ticket = NULL;
        return true;
    }

    static bool HandleGMTicketAssignToCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* tguid = strtok((char*)args, " ");
        uint64 ticketGuid = atoi(tguid);
        char* targetgm = strtok(NULL, " ");

        if (!targetgm)
            return false;

        std::string targm = targetgm;
        if (!normalizePlayerName(targm))
            return false;

        Player* cplr = handler->GetSession()->GetPlayer();
        GM_Ticket* ticket = ticketmgr.GetGMTicket(ticketGuid);

        if (!ticket || ticket->closed != 0)
        {
            handler->SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
            return true;
        }

        uint64 tarGUID = sObjectMgr.GetPlayerGUIDByName(targm.c_str());
        uint64 accid = sObjectMgr.GetPlayerAccountIdByGUID(tarGUID);
        uint32 gmlevel = sAccountMgr->GetSecurity(accid, realmID);

        if (!tarGUID || gmlevel == SEC_PLAYER)
        {
            handler->SendSysMessage(LANG_COMMAND_TICKETASSIGNERROR_A);
            return true;
        }

        if (ticket->assignedToGM == tarGUID)
        {
            handler->PSendSysMessage(LANG_COMMAND_TICKETASSIGNERROR_B, ticket->guid);
            return true;
        }

        std::string gmname;
        sObjectMgr.GetPlayerNameByGUID(tarGUID, gmname);
        if (ticket->assignedToGM != 0 && ticket->assignedToGM != cplr->GetGUID())
        {
            handler->PSendSysMessage(LANG_COMMAND_TICKETALREADYASSIGNED, ticket->guid, gmname.c_str());
            return true;
        }

        ticket->escalated = true;
        Player* plr = sObjectMgr.GetPlayer(ticket->playerGuid);
        if (plr && plr->IsInWorld())
        {
            // tell client to update display of ticket status
            WorldPacket data(SMSG_GM_TICKET_STATUS_UPDATE, 4);
            data << uint32(1);
            plr->GetSession()->SendPacket(&data);
        }

        ticket->assignedToGM = tarGUID;
        ticketmgr.UpdateGMTicket(ticket);
        std::stringstream ss;
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTGUID, ticket->guid);
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTNAME, ticket->name.c_str());
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTASSIGNEDTO, gmname.c_str());
        handler->SendGlobalGMSysMessage(ss.str().c_str());
        return true;
    }

    static bool HandleGMTicketUnAssignCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint64 ticketGuid = atoi(args);
        Player* cplr = handler->GetSession()->GetPlayer();
        GM_Ticket* ticket = ticketmgr.GetGMTicket(ticketGuid);

        if (!ticket || ticket->closed != 0)
        {
            handler->SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
            return true;
        }
        if (ticket->assignedToGM == 0)
        {
            handler->PSendSysMessage(LANG_COMMAND_TICKETNOTASSIGNED, ticket->guid);
            return true;
        }

        std::string gmname;
        sObjectMgr.GetPlayerNameByGUID(ticket->assignedToGM, gmname);
        Player* plr = sObjectMgr.GetPlayer(ticket->assignedToGM);
        if (plr && plr->IsInWorld() && plr->GetSession()->GetSecurity() > cplr->GetSession()->GetSecurity())
        {
            handler->SendSysMessage(LANG_COMMAND_TICKETUNASSIGNSECURITY);
            return true;
        }

        std::stringstream ss;
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTGUID, ticket->guid);
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTNAME, ticket->name.c_str());
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTASSIGNEDTO, gmname.c_str());
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTUNASSIGNED, cplr->GetName());
        handler->SendGlobalGMSysMessage(ss.str().c_str());

        ticket->escalated = false;
        Player* player = sObjectMgr.GetPlayer(ticket->playerGuid);
        if (player && player->IsInWorld())
        {
            // tell client to update display of ticket status
            WorldPacket data(SMSG_GM_TICKET_STATUS_UPDATE, 4);
            data << uint32(1);
            player->GetSession()->SendPacket(&data);
        }

        ticket->assignedToGM = 0;
        ticketmgr.UpdateGMTicket(ticket);
        return true;
    }

    static bool HandleGMTicketCommentCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* tguid = strtok((char*)args, " ");
        uint64 ticketGuid = atoi(tguid);
        char* comment = strtok(NULL, "\n");

        if (!comment)
            return false;

        Player* cplr = handler->GetSession()->GetPlayer();
        GM_Ticket* ticket = ticketmgr.GetGMTicket(ticketGuid);

        if (!ticket || ticket->closed != 0)
        {
            handler->SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
            return true;
        }
        if (ticket->assignedToGM != 0 && ticket->assignedToGM != cplr->GetGUID())
        {
            handler->PSendSysMessage(LANG_COMMAND_TICKETALREADYASSIGNED, ticket->guid);
            return true;
        }

        std::string gmname;
        sObjectMgr.GetPlayerNameByGUID(ticket->assignedToGM, gmname);
        ticket->comment = comment;
        ticketmgr.UpdateGMTicket(ticket);
        std::stringstream ss;
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTGUID, ticket->guid);
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTNAME, ticket->name.c_str());
        if (sObjectMgr.GetPlayerNameByGUID(ticket->assignedToGM, gmname))
            ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTASSIGNEDTO, gmname.c_str());
        ss << PGetParseString(handler, LANG_COMMAND_TICKETLISTADDCOMMENT, cplr->GetName(), ticket->comment.c_str());
        handler->SendGlobalGMSysMessage(ss.str().c_str());
        return true;
    }

    static std::string PGetParseString(ChatHandler* handler, int32 entry, ...)
    {
        const char* format = handler->GetOregonString(entry);
        va_list ap;
        char str[1024];
        va_start(ap, entry);
        vsnprintf(str, 1024, format, ap);
        va_end(ap);
        return (std::string)str;
    }

};

void AddSC_ticket_commandscript()
{
    new ticket_commandscript();
}