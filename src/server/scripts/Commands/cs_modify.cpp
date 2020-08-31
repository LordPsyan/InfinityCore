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

static uint32 ReputationRankStrIndex[MAX_REPUTATION_RANK] =
{
    LANG_REP_HATED,    LANG_REP_HOSTILE, LANG_REP_UNFRIENDLY, LANG_REP_NEUTRAL,
    LANG_REP_FRIENDLY, LANG_REP_HONORED, LANG_REP_REVERED,    LANG_REP_EXALTED
};

class modify_commandscript : public CommandScript
{
public:
    modify_commandscript() : CommandScript("modify_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> modifyCommandTable =
        {
            { "hp",             SEC_MODERATOR,      false, &HandleMODHPCommand,                     "" },
            { "mana",           SEC_MODERATOR,      false, &HandleMODManaCommand,                   "" },
            { "rage",           SEC_MODERATOR,      false, &HandleMODRageCommand,                   "" },
            { "energy",         SEC_MODERATOR,      false, &HandleMODEnergyCommand,                 "" },
            { "money",          SEC_MODERATOR,      false, &HandleMODMoneyCommand,                  "" },
            { "speed",          SEC_MODERATOR,      false, &HandleMODSpeedCommand,                  "" },
            { "swim",           SEC_MODERATOR,      false, &HandleMODSwimCommand,                   "" },
            { "scale",          SEC_MODERATOR,      false, &HandleMODScaleCommand,                  "" },
            { "bit",            SEC_MODERATOR,      false, &HandleMODBitCommand,                    "" },
            { "bwalk",          SEC_MODERATOR,      false, &HandleMODBWalkCommand,                  "" },
            { "fly",            SEC_MODERATOR,      false, &HandleMODFlyCommand,                    "" },
            { "aspeed",         SEC_MODERATOR,      false, &HandleMODASpeedCommand,                 "" },
            { "faction",        SEC_MODERATOR,      false, &HandleMODFactionCommand,                "" },
            { "spell",          SEC_MODERATOR,      false, &HandleMODSpellCommand,                  "" },
            { "talentpoints",   SEC_MODERATOR,      false, &HandleMODTPCommand,                     "" },
            { "mount",          SEC_MODERATOR,      false, &HandleMODMountCommand,                  "" },
            { "honor",          SEC_MODERATOR,      false, &HandleMODHonorCommand,                  "" },
            { "reputation",     SEC_MODERATOR,      false, &HandleMODReputationCommand,             "" },
            { "arena",          SEC_MODERATOR,      false, &HandleMODArenaCommand,                  "" },
            { "drunk",          SEC_MODERATOR,      false, &HandleMODDrunkCommand,                  "" },
            { "standstate",     SEC_MODERATOR,      false, &HandleMODStandStateCommand,             "" },
            { "demorph",        SEC_MODERATOR,      false, &HandleMODDeMorphCommand,                "" },
            { "morph",          SEC_MODERATOR,      false, &HandleMODMorphCommand,                  "" },
            { "gender",         SEC_ADMINISTRATOR,  false, &HandleMODGender,                        "" },
            { "phase",          SEC_GAMEMASTER,     false, &HandleModifyPhaseCommand,         "" },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "modify",         SEC_MODERATOR,      false, nullptr,                                 "", modifyCommandTable }
        };
        return commandTable;
    };


    static bool HandleModifyPhaseCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 phasemask = (uint32)atoi((char*)args);

        Unit* target = handler->getSelectedUnit();
        if (!target)
            target = handler->GetSession()->GetPlayer();


        target->SetPhaseMask(phasemask, true, false);

        return true;
    }


    static bool HandleMODHPCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        int32 hp = atoi((char*)args);
        int32 hpm = atoi((char*)args);

        if (hp < 1 || hpm < 1 || hpm < hp)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayerOrSelf();
        if (target == NULL)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->IsAlive())
        {
            handler->SendSysMessage(LANG_NO_SELECTION);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_HP, target->GetName(), hp, hpm);
        if (handler->needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_YOURS_HP_CHANGED, handler->GetName(), hp, hpm);

        target->SetMaxHealth(hpm);
        target->SetHealth(hp);

        return true;
    }

    static bool HandleMODManaCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        int32 mana = atoi((char*)args);
        int32 manam = atoi((char*)args);

        if (mana <= 0 || manam <= 0 || manam < mana)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayerOrSelf();
        if (target == NULL)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_MANA, target->GetName(), mana, manam);
        if (handler->needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_YOURS_MANA_CHANGED, handler->GetName(), mana, manam);

        target->SetMaxPower(POWER_MANA, manam);
        target->SetPower(POWER_MANA, mana);

        return true;
    }

    static bool HandleMODRageCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        int32 rage = atoi((char*)args) * 10;
        int32 ragem = atoi((char*)args) * 10;

        if (rage <= 0 || ragem <= 0 || ragem < rage)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayerOrSelf();
        if (target == NULL)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_RAGE, target->GetName(), rage / 10, ragem / 10);
        if (handler->needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_YOURS_RAGE_CHANGED, handler->GetName(), rage / 10, ragem / 10);

        target->SetMaxPower(POWER_RAGE, ragem);
        target->SetPower(POWER_RAGE, rage);

        return true;
    }

    static bool HandleMODEnergyCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        int32 energy = atoi((char*)args) * 10;
        int32 energym = atoi((char*)args) * 10;

        if (energy <= 0 || energym <= 0 || energym < energy)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayerOrSelf();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_ENERGY, target->GetName(), energy / 10, energym / 10);
        if (handler->needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_YOURS_ENERGY_CHANGED, handler->GetName(), energy / 10, energym / 10);

        target->SetMaxPower(POWER_ENERGY, energym);
        target->SetPower(POWER_ENERGY, energy);

        sLog.outDetail(handler->GetOregonString(LANG_CURRENT_ENERGY), target->GetMaxPower(POWER_ENERGY));

        return true;
    }

    static bool HandleMODMoneyCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayerOrSelf();
        if (target == NULL)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        int32 addmoney = atoi((char*)args);

        uint32 moneyuser = target->GetMoney();

        if (addmoney < 0)
        {
            int32 newmoney = moneyuser + addmoney;

            sLog.outDetail(handler->GetOregonString(LANG_CURRENT_MONEY), moneyuser, addmoney, newmoney);
            if (newmoney <= 0)
            {
                handler->PSendSysMessage(LANG_YOU_TAKE_ALL_MONEY, target->GetName());
                if (handler->needReportToTarget(target))
                    ChatHandler(target).PSendSysMessage(LANG_YOURS_ALL_MONEY_GONE, handler->GetName());

                target->SetMoney(0);
            }
            else
            {
                handler->PSendSysMessage(LANG_YOU_TAKE_MONEY, abs(addmoney), target->GetName());
                if (handler->needReportToTarget(target))
                    ChatHandler(target).PSendSysMessage(LANG_YOURS_MONEY_TAKEN, handler->GetName(), abs(addmoney));
                target->SetMoney(newmoney);
            }
        }
        else
        {
            handler->PSendSysMessage(LANG_YOU_GIVE_MONEY, addmoney, target->GetName());
            if (handler->needReportToTarget(target))
                ChatHandler(target).PSendSysMessage(LANG_YOURS_MONEY_GIVEN, handler->GetName(), addmoney);
            target->ModifyMoney(addmoney);
        }

        sLog.outDetail(handler->GetOregonString(LANG_NEW_MONEY), moneyuser, addmoney, target->GetMoney());

        return true;
    }

    static bool HandleMODSpeedCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        float modSpeed = (float)atof((char*)args);

        if (modSpeed > 10 || modSpeed < 0.1)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayerOrSelf();
        if (target == NULL)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target->IsInFlight())
        {
            handler->PSendSysMessage(LANG_CHAR_IN_FLIGHT, target->GetName());
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_SPEED, modSpeed, target->GetName());
        if (handler->needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_YOURS_SPEED_CHANGED, handler->GetName(), modSpeed);

        target->SetSpeed(MOVE_RUN, modSpeed, true);

        return true;
    }

    static bool HandleMODSwimCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        float modSpeed = (float)atof((char*)args);

        if (modSpeed > 10.0f || modSpeed < 0.1f)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayerOrSelf();
        if (target == NULL)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target->IsInFlight())
        {
            handler->PSendSysMessage(LANG_CHAR_IN_FLIGHT, target->GetName());
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_SWIM_SPEED, modSpeed, target->GetName());
        if (handler->needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_YOURS_SWIM_SPEED_CHANGED, handler->GetName(), modSpeed);

        target->SetSpeed(MOVE_SWIM, modSpeed, true);

        return true;
    }

    static bool HandleMODScaleCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        float Scale = (float)atof((char*)args);
        if (Scale > 10.0f || Scale < 0.1f)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Unit* target = handler->getSelectedUnit();
        if (target == NULL)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target->GetTypeId() == TYPEID_PLAYER)
        {
            handler->PSendSysMessage(LANG_YOU_CHANGE_SIZE, Scale, target->GetName());
            if (handler->needReportToTarget(target->ToPlayer()))
                ChatHandler(target->ToPlayer()).PSendSysMessage(LANG_YOURS_SIZE_CHANGED, handler->GetName(), Scale);
        }

        target->SetObjectScale(Scale);

        return true;
    }

    static bool HandleMODBitCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayerOrSelf();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* pField = strtok((char*)args, " ");
        if (!pField)
            return false;

        char* pBit = strtok(NULL, " ");
        if (!pBit)
            return false;

        uint16 field = atoi(pField);
        uint32 bit = atoi(pBit);

        if (field < 1 || field >= PLAYER_END)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }
        if (bit < 1 || bit > 32)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target->HasFlag(field, (1 << (bit - 1))))
        {
            target->RemoveFlag(field, (1 << (bit - 1)));
            handler->PSendSysMessage(LANG_REMOVE_BIT, bit, field);
        }
        else
        {
            target->SetFlag(field, (1 << (bit - 1)));
            handler->PSendSysMessage(LANG_SET_BIT, bit, field);
        }
        return true;
    }

    static bool HandleMODBWalkCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        float modSpeed = (float)atof((char*)args);

        if (modSpeed > 10.0f || modSpeed < 0.1f)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayerOrSelf();
        if (target == NULL)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target->IsInFlight())
        {
            handler->PSendSysMessage(LANG_CHAR_IN_FLIGHT, target->GetName());
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_BACK_SPEED, modSpeed, target->GetName());
        if (handler->needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_YOURS_BACK_SPEED_CHANGED, handler->GetName(), modSpeed);

        target->SetSpeed(MOVE_RUN_BACK, modSpeed, true);

        return true;
    }

    static bool HandleMODFlyCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        float modSpeed = (float)atof((char*)args);

        if (modSpeed > 50.0f || modSpeed < 0.1f)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayerOrSelf();
        if (target == NULL)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_FLY_SPEED, modSpeed, target->GetName());
        if (handler->needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_YOURS_FLY_SPEED_CHANGED, handler->GetName(), modSpeed);

        target->SetSpeed(MOVE_FLIGHT, modSpeed, true);

        return true;
    }

    static bool HandleMODASpeedCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        float modSpeed = (float)atof((char*)args);


        if (modSpeed > 10 || modSpeed < 0.1)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayerOrSelf();
        if (target == NULL)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target->IsInFlight())
        {
            handler->PSendSysMessage(LANG_CHAR_IN_FLIGHT, target->GetName());
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_ASPEED, modSpeed, target->GetName());
        if (handler->needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_YOURS_ASPEED_CHANGED, handler->GetName(), modSpeed);

        target->SetSpeed(MOVE_WALK, modSpeed, true);
        target->SetSpeed(MOVE_RUN, modSpeed, true);
        target->SetSpeed(MOVE_SWIM, modSpeed, true);
        //target->SetSpeed(MOVE_TURN,    modSpeed, true);
        target->SetSpeed(MOVE_FLIGHT, modSpeed, true);
        return true;
    }

    static bool HandleMODFactionCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* pfactionid = handler->extractKeyFromLink((char*)args, "Hfaction");

        Creature* target = handler->getSelectedCreature();
        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!pfactionid)
        {
            if (target)
            {
                uint32 factionid = target->GetFaction();
                uint32 flag = target->GetUInt32Value(UNIT_FIELD_FLAGS);
                uint32 npcflag = target->GetUInt32Value(UNIT_NPC_FLAGS);
                uint32 dyflag = target->GetUInt32Value(UNIT_DYNAMIC_FLAGS);
                handler->PSendSysMessage(LANG_CURRENT_FACTION, target->GetGUIDLow(), factionid, flag, npcflag, dyflag);
            }
            return true;
        }

        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 factionid = atoi(pfactionid);
        uint32 flag;

        char* pflag = strtok(NULL, " ");
        if (!pflag)
            flag = target->GetUInt32Value(UNIT_FIELD_FLAGS);
        else
            flag = atoi(pflag);

        char* pnpcflag = strtok(NULL, " ");

        uint32 npcflag;
        if (!pnpcflag)
            npcflag = target->GetUInt32Value(UNIT_NPC_FLAGS);
        else
            npcflag = atoi(pnpcflag);

        char* pdyflag = strtok(NULL, " ");

        uint32  dyflag;
        if (!pdyflag)
            dyflag = target->GetUInt32Value(UNIT_DYNAMIC_FLAGS);
        else
            dyflag = atoi(pdyflag);

        if (!sFactionTemplateStore.LookupEntry(factionid))
        {
            handler->PSendSysMessage(LANG_WRONG_FACTION, factionid);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_FACTION, target->GetGUIDLow(), factionid, flag, npcflag, dyflag);

        target->SetFaction(factionid);
        target->SetUInt32Value(UNIT_FIELD_FLAGS, flag);
        target->SetUInt32Value(UNIT_NPC_FLAGS, npcflag);
        target->SetUInt32Value(UNIT_DYNAMIC_FLAGS, dyflag);

        return true;
    }

    static bool HandleMODSpellCommand(ChatHandler* handler, char const* args)
    {
        if (!*args) return false;
        char* pspellflatid = strtok((char*)args, " ");
        if (!pspellflatid)
            return false;

        char* pop = strtok(NULL, " ");
        if (!pop)
            return false;

        char* pval = strtok(NULL, " ");
        if (!pval)
            return false;

        uint16 mark;

        char* pmark = strtok(NULL, " ");

        uint8 spellflatid = atoi(pspellflatid);
        uint8 op = atoi(pop);
        uint16 val = atoi(pval);
        if (!pmark)
            mark = 65535;
        else
            mark = atoi(pmark);

        Player* target = handler->getSelectedPlayerOrSelf();
        if (target == NULL)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_CHANGE_SPELLFLATID, spellflatid, val, mark, target->GetName());
        if (handler->needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_YOURS_SPELLFLATID_CHANGED, handler->GetName(), spellflatid, val, mark);

        WorldPacket data(SMSG_SET_FLAT_SPELL_MODIFIER, (1 + 1 + 2 + 2));
        data << uint8(spellflatid);
        data << uint8(op);
        data << uint16(val);
        data << uint16(mark);
        target->GetSession()->SendPacket(&data);

        return true;
    }

    static bool HandleMODTPCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        int tp = atoi((char*)args);
        if (tp < 0)
            return false;

        Player* target = handler->getSelectedPlayerOrSelf();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        target->SetFreeTalentPoints(tp);
        return true;
    }

    static bool HandleMODMountCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint16 mId = 1147;
        float speed = (float)15;
        uint32 num = 0;

        num = atoi((char*)args);
        switch (num)
        {
        case 1:
            mId = 14340;
            break;
        case 2:
            mId = 4806;
            break;
        case 3:
            mId = 6471;
            break;
        case 4:
            mId = 12345;
            break;
        case 5:
            mId = 6472;
            break;
        case 6:
            mId = 6473;
            break;
        case 7:
            mId = 10670;
            break;
        case 8:
            mId = 10719;
            break;
        case 9:
            mId = 10671;
            break;
        case 10:
            mId = 10672;
            break;
        case 11:
            mId = 10720;
            break;
        case 12:
            mId = 14349;
            break;
        case 13:
            mId = 11641;
            break;
        case 14:
            mId = 12244;
            break;
        case 15:
            mId = 12242;
            break;
        case 16:
            mId = 14578;
            break;
        case 17:
            mId = 14579;
            break;
        case 18:
            mId = 14349;
            break;
        case 19:
            mId = 12245;
            break;
        case 20:
            mId = 14335;
            break;
        case 21:
            mId = 207;
            break;
        case 22:
            mId = 2328;
            break;
        case 23:
            mId = 2327;
            break;
        case 24:
            mId = 2326;
            break;
        case 25:
            mId = 14573;
            break;
        case 26:
            mId = 14574;
            break;
        case 27:
            mId = 14575;
            break;
        case 28:
            mId = 604;
            break;
        case 29:
            mId = 1166;
            break;
        case 30:
            mId = 2402;
            break;
        case 31:
            mId = 2410;
            break;
        case 32:
            mId = 2409;
            break;
        case 33:
            mId = 2408;
            break;
        case 34:
            mId = 2405;
            break;
        case 35:
            mId = 14337;
            break;
        case 36:
            mId = 6569;
            break;
        case 37:
            mId = 10661;
            break;
        case 38:
            mId = 10666;
            break;
        case 39:
            mId = 9473;
            break;
        case 40:
            mId = 9476;
            break;
        case 41:
            mId = 9474;
            break;
        case 42:
            mId = 14374;
            break;
        case 43:
            mId = 14376;
            break;
        case 44:
            mId = 14377;
            break;
        case 45:
            mId = 2404;
            break;
        case 46:
            mId = 2784;
            break;
        case 47:
            mId = 2787;
            break;
        case 48:
            mId = 2785;
            break;
        case 49:
            mId = 2736;
            break;
        case 50:
            mId = 2786;
            break;
        case 51:
            mId = 14347;
            break;
        case 52:
            mId = 14346;
            break;
        case 53:
            mId = 14576;
            break;
        case 54:
            mId = 9695;
            break;
        case 55:
            mId = 9991;
            break;
        case 56:
            mId = 6448;
            break;
        case 57:
            mId = 6444;
            break;
        case 58:
            mId = 6080;
            break;
        case 59:
            mId = 6447;
            break;
        case 60:
            mId = 4805;
            break;
        case 61:
            mId = 9714;
            break;
        case 62:
            mId = 6448;
            break;
        case 63:
            mId = 6442;
            break;
        case 64:
            mId = 14632;
            break;
        case 65:
            mId = 14332;
            break;
        case 66:
            mId = 14331;
            break;
        case 67:
            mId = 8469;
            break;
        case 68:
            mId = 2830;
            break;
        case 69:
            mId = 2346;
            break;
        default:
            handler->SendSysMessage(LANG_NO_MOUNT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler-> getSelectedPlayerOrSelf();
        if (target == NULL)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_GIVE_MOUNT, target->GetName());
        if (handler->needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_MOUNT_GIVED, handler-> GetName());

        target->SetUInt32Value(UNIT_FIELD_FLAGS, 0x001000);
        target->Mount(mId);

        WorldPacket data(SMSG_FORCE_RUN_SPEED_CHANGE, (8 + 4 + 1 + 4));
        data << target->GetPackGUID();
        data << (uint32)0;
        data << (uint8)0;                                       //new 2.1.0
        data << float(speed);
        target->SendMessageToSet(&data, true);

        data.Initialize(SMSG_FORCE_SWIM_SPEED_CHANGE, (8 + 4 + 4));
        data << target->GetPackGUID();
        data << (uint32)0;
        data << float(speed);
        target->SendMessageToSet(&data, true);

        return true;
    }

    static bool HandleMODHonorCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayerOrSelf();
        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        int32 amount = (uint32)atoi(args);

        target->ModifyHonorPoints(amount);

        handler->PSendSysMessage(LANG_COMMAND_MODIFY_HONOR, target->GetName(), target->GetHonorPoints());

        return true;
    }

    static bool HandleMODReputationCommand(ChatHandler* handler, char const* args)
    {
        if (!*args) return false;

        Player* target = NULL;
        target = handler->getSelectedPlayer();

        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* factionTxt = handler->extractKeyFromLink((char*)args, "Hfaction");
        if (!factionTxt)
            return false;

        uint32 factionId = atoi(factionTxt);

        int32 amount = 0;
        char* rankTxt = strtok(NULL, " ");
        if (!rankTxt)
            return false;

        amount = atoi(rankTxt);
        if ((amount == 0) && (rankTxt[0] != '-') && !isdigit(rankTxt[0]))
        {
            std::string rankStr = rankTxt;
            std::wstring wrankStr;
            if (!Utf8toWStr(rankStr, wrankStr))
                return false;
            wstrToLower(wrankStr);

            int r = 0;
            amount = -42000;
            for (; r < MAX_REPUTATION_RANK; ++r)
            {
                std::string rank = handler->GetOregonString(ReputationRankStrIndex[r]);
                if (rank.empty())
                    continue;

                std::wstring wrank;
                if (!Utf8toWStr(rank, wrank))
                    continue;

                wstrToLower(wrank);

                if (wrank.substr(0, wrankStr.size()) == wrankStr)
                {
                    char* deltaTxt = strtok(NULL, " ");
                    if (deltaTxt)
                    {
                        int32 delta = atoi(deltaTxt);
                        if ((delta < 0) || (delta > ReputationMgr::PointsInRank[r] - 1))
                        {
                            handler->PSendSysMessage(LANG_COMMAND_FACTION_DELTA, (ReputationMgr::PointsInRank[r] - 1));
                            handler->SetSentErrorMessage(true);
                            return false;
                        }
                        amount += delta;
                    }
                    break;
                }
                amount += ReputationMgr::PointsInRank[r];
            }
            if (r >= MAX_REPUTATION_RANK)
            {
                handler->PSendSysMessage(LANG_COMMAND_FACTION_INVPARAM, rankTxt);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionId);

        if (!factionEntry)
        {
            handler->PSendSysMessage(LANG_COMMAND_FACTION_UNKNOWN, factionId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (factionEntry->reputationListID < 0)
        {
            handler->PSendSysMessage(LANG_COMMAND_FACTION_NOREP_ERROR, factionEntry->name[handler->GetSession()->GetSessionDbcLocale()], factionId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        target->GetReputationMgr().SetReputation(factionEntry, amount);
        handler->PSendSysMessage(LANG_COMMAND_MODIFY_REP, factionEntry->name[handler->GetSession()->GetSessionDbcLocale()], factionId, target->GetName(), target->GetReputationMgr().GetReputation(factionEntry));
        return true;
    }

    static bool HandleMODArenaCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        int32 amount = (uint32)atoi(args);

        target->ModifyArenaPoints(amount);

        handler->PSendSysMessage(LANG_COMMAND_MODIFY_ARENA, target->GetName(), target->GetArenaPoints());

        return true;
    }

    static bool HandleMODDrunkCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)    return false;

        uint32 drunklevel = (uint32)atoi(args);
        if (drunklevel > 100)
            drunklevel = 100;

        uint16 drunkMod = drunklevel * 0xFFFF / 100;

        Player* target = handler->getSelectedPlayer();
        if (!target)
            target = handler->GetSession()->GetPlayer();

        if (target)
            target->SetDrunkValue(drunkMod);

        return true;
    }

    static bool HandleMODStandStateCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 anim_id = atoi((char*)args);
        handler->GetSession()->GetPlayer()->SetUInt32Value(UNIT_NPC_EMOTESTATE, anim_id);

        return true;
    }

    static bool HandleMODDeMorphCommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* target = handler->getSelectedUnit();

        if (!target)
            target = handler->getSelectedPlayerOrSelf();

        target->DeMorph();

        return true;
    }

    static bool HandleMODMorphCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint16 display_id = (uint16)atoi((char*)args);

        Unit* target = handler->getSelectedUnit();
        if (!target)
            target = handler->GetSession()->GetPlayer();

        target->SetDisplayId(display_id);

        return true;
    }

    static bool HandleMODGender(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->getSelectedPlayer();

        if (!player)
        {
            handler->PSendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char const* gender_str = (char*)args;
        int gender_len = strlen(gender_str);

        uint32 displayId = player->GetNativeDisplayId();
        char const* gender_full = NULL;
        uint32 new_displayId = displayId;
        Gender gender;

        if (!strncmp(gender_str, "male", gender_len))            // MALE
        {
            if (player->getGender() == GENDER_MALE)
                return true;

            gender_full = "male";
            new_displayId = player->getRace() == RACE_BLOODELF ? displayId + 1 : displayId - 1;
            gender = GENDER_MALE;
        }
        else if (!strncmp(gender_str, "female", gender_len))    // FEMALE
        {
            if (player->getGender() == GENDER_FEMALE)
                return true;

            gender_full = "female";
            new_displayId = player->getRace() == RACE_BLOODELF ? displayId - 1 : displayId + 1;
            gender = GENDER_FEMALE;
        }
        else
        {
            handler->SendSysMessage(LANG_MUST_MALE_OR_FEMALE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Set gender
        player->SetByteValue(UNIT_FIELD_BYTES_0, 2, gender);
        player->SetByteValue(PLAYER_BYTES_3, 0, gender);

        // Change display ID
        player->SetDisplayId(new_displayId);
        player->SetNativeDisplayId(new_displayId);

        handler->PSendSysMessage(LANG_YOU_CHANGE_GENDER, player->GetName(), gender_full);
        if (handler->needReportToTarget(player))
            ChatHandler(player).PSendSysMessage(LANG_YOUR_GENDER_CHANGED, gender_full, handler->GetName());

        return true;
    }

};

void AddSC_modify_commandscript()
{
    new modify_commandscript();
}