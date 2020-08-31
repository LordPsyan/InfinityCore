#include "ScriptMgr.h"
#include "Chat.h"
#include "ChannelMgr.h"
#include "Language.h"
#include "Player.h"

class cheat_commandscript : public CommandScript
{
public:
    cheat_commandscript() : CommandScript("cheat_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> cheatCommandTable =
        {
            { "god",            SEC_GAMEMASTER,     false, &HandleGodModeCheatCommand,         "" },
            { "casttime",       SEC_GAMEMASTER,     false, &HandleCasttimeCheatCommand,        "" },
            { "cooldown",       SEC_GAMEMASTER,     false, &HandleCoolDownCheatCommand,        "" },
            { "power",          SEC_GAMEMASTER,     false, &HandlePowerCheatCommand,           "" },
            { "waterwalk",      SEC_GAMEMASTER,     false, &HandleWaterwalkCheatCommand,       "" },
            { "explore",        SEC_GAMEMASTER,     false, &HandleExploreCheatCommand,         "" },
            { "hover",          SEC_GAMEMASTER,     false, &HandleHoverCommand,                "" },
            { "taxi",           SEC_GAMEMASTER,     false, &HandleTaxiCheatCommand,            "" }
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "cheat",          SEC_GAMEMASTER,     false, nullptr,         "", cheatCommandTable },

        };
        return commandTable;
    }

    static bool HandleGodModeCheatCommand(ChatHandler* handler, const char* args)
    {
        if (!handler->GetSession() || !handler->GetSession()->GetPlayer())
            return false;

        std::string argstr = (char*)args;

        if (!*args)
            argstr = (handler->GetSession()->GetPlayer()->GetCommandStatus(CHEAT_GOD)) ? "off" : "on";

        if (argstr == "off")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOff(CHEAT_GOD);
            handler->SendSysMessage("Godmode is OFF. You can take damage.");
            return true;
        }
        else if (argstr == "on")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOn(CHEAT_GOD);
            handler->SendSysMessage("Godmode is ON. You won't take damage.");
            return true;
        }

        return false;
    }

    static bool HandleCasttimeCheatCommand(ChatHandler* handler, const char* args)
    {
        if (!handler->GetSession() || !handler->GetSession()->GetPlayer())
            return false;

        std::string argstr = (char*)args;

        if (!*args)
            argstr = (handler->GetSession()->GetPlayer()->GetCommandStatus(CHEAT_CASTTIME)) ? "off" : "on";

        if (argstr == "off")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOff(CHEAT_CASTTIME);
            handler->SendSysMessage("CastTime Cheat is OFF. Your spells will have a casttime.");
            return true;
        }
        else if (argstr == "on")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOn(CHEAT_CASTTIME);
            handler->SendSysMessage("CastTime Cheat is ON. Your spells won't have a casttime.");
            return true;
        }

        return false;
    }

    static bool HandleCoolDownCheatCommand(ChatHandler* handler, const char* args)
    {
        if (!handler->GetSession() || !handler->GetSession()->GetPlayer())
            return false;

        std::string argstr = (char*)args;

        if (!*args)
            argstr = (handler->GetSession()->GetPlayer()->GetCommandStatus(CHEAT_COOLDOWN)) ? "off" : "on";

        if (argstr == "off")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOff(CHEAT_COOLDOWN);
            handler->SendSysMessage("Cooldown Cheat is OFF. You are on the global cooldown.");
            return true;
        }
        else if (argstr == "on")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOn(CHEAT_COOLDOWN);
            handler->SendSysMessage("Cooldown Cheat is ON. You are not on the global cooldown.");
            return true;
        }

        return false;
    }

    static bool HandlePowerCheatCommand(ChatHandler* handler, const char* args)
    {
        if (!handler->GetSession() || !handler->GetSession()->GetPlayer())
            return false;

        std::string argstr = (char*)args;

        if (!*args)
            argstr = (handler->GetSession()->GetPlayer()->GetCommandStatus(CHEAT_POWER)) ? "off" : "on";

        if (argstr == "off")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOff(CHEAT_POWER);
            handler->SendSysMessage("Power Cheat is OFF. You need mana/rage/energy to use spells.");
            return true;
        }
        else if (argstr == "on")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOn(CHEAT_POWER);
            handler->SendSysMessage("Power Cheat is ON. You don't need mana/rage/energy to use spells.");
            return true;
        }

        return false;
    }

    static bool HandleWaterwalkCheatCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer()->GetSelectedPlayer();
        if (!player)
        {
            handler->PSendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (strncmp(args, "on", 3) == 0)
            player->SetWaterWalking(true);               // ON
        else if (strncmp(args, "off", 4) == 0)
            player->SetWaterWalking(false);              // OFF
        else
        {
            handler->SendSysMessage(LANG_USE_BOL);
            return false;
        }

        handler->PSendSysMessage(LANG_YOU_SET_WATERWALK, args, player->GetName());
        if (handler->needReportToTarget(player))
            ChatHandler(player).PSendSysMessage(LANG_YOUR_WATERWALK_SET, args, handler->GetName());
        return true;
    }

    static bool HandleExploreCheatCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        int flag = atoi((char*)args);

        Player* chr = handler->getSelectedPlayer();
        if (chr == NULL)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (flag != 0)
        {
            handler->PSendSysMessage(LANG_YOU_SET_EXPLORE_ALL, chr->GetName());
            if (handler->needReportToTarget(chr))
                ChatHandler(chr).PSendSysMessage(LANG_YOURS_EXPLORE_SET_ALL, handler->GetName());
        }
        else
        {
            handler->PSendSysMessage(LANG_YOU_SET_EXPLORE_NOTHING, chr->GetName());
            if (handler->needReportToTarget(chr))
                ChatHandler(chr).PSendSysMessage(LANG_YOURS_EXPLORE_SET_NOTHING, handler->GetName());
        }

        for (uint8 i = 0; i < 128; ++i)
        {
            if (flag != 0)
                handler->GetSession()->GetPlayer()->SetFlag(PLAYER_EXPLORED_ZONES_1 + i, 0xFFFFFFFF);
            else
                handler->GetSession()->GetPlayer()->RemoveFlag(PLAYER_EXPLORED_ZONES_1 + i, 0xFFFFFFFF);
        }

        return true;
    }

    static bool HandleHoverCommand(ChatHandler* handler, const char* args)
    {
        char* px = strtok((char*)args, " ");
        uint32 flag;
        if (!px)
            flag = 1;
        else
            flag = atoi(px);

        handler->GetSession()->GetPlayer()->SetHover(flag);

        if (flag)
            handler->SendSysMessage(LANG_HOVER_ENABLED);
        else
            handler->SendSysMessage(LANG_HOVER_DISABLED);

        return true;
    }

    static bool HandleTaxiCheatCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
        {
            handler->SendSysMessage(LANG_USE_BOL);
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string argstr = (char*)args;

        Player* target = handler->getSelectedPlayerOrSelf();

        if (argstr == "on")
        {
            target->SetTaxiCheater(true);
            handler->PSendSysMessage(LANG_YOU_GIVE_TAXIS, target->GetName());
            if (handler->needReportToTarget(target))
                ChatHandler(target).PSendSysMessage(LANG_YOURS_TAXIS_ADDED, handler->GetName());
            return true;
        }

        if (argstr == "off")
        {
            target->SetTaxiCheater(false);
            handler->PSendSysMessage(LANG_YOU_REMOVE_TAXIS, target->GetName());
            if (handler->needReportToTarget(target))
                ChatHandler(target).PSendSysMessage(LANG_YOURS_TAXIS_REMOVED, handler->GetName());

            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }
};

void AddSC_cheat_commandscript()
{
    new cheat_commandscript();
}