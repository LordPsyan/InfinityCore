#include "ScriptMgr.h"
#include "Chat.h"
#include "ChannelMgr.h"
#include "Language.h"
#include "Player.h"

class message_commandscript : public CommandScript
{
public:
    message_commandscript() : CommandScript("message_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> commandTable =
        {
            { "nameannounce",   SEC_GAMEMASTER,      true,   &HandleNameAnnounceCommand,         "" },
            { "gmnameannounce", SEC_GAMEMASTER,      true,   &HandleGMNameAnnounceCommand,       "" },
            { "announce",       SEC_GAMEMASTER,      true,   &HandleAnnounceCommand,             "" },
            { "gmannounce",     SEC_GAMEMASTER,      true,   &HandleGMAnnounceCommand,           "" },
            { "notify",         SEC_GAMEMASTER,      true,   &HandleNotifyCommand,               "" },
            { "gmnotify",       SEC_GAMEMASTER,      true,   &HandleGMNotifyCommand,             "" },
            { "whispers",       SEC_MODERATOR,       false,  &HandleWhispersCommand,             "" }
        };
        return commandTable;
    }

    static bool HandleNameAnnounceCommand(ChatHandler* handler, const char* args)
    {
        WorldPacket data;
        if (!*args)
            return false;

        sWorld.SendWorldText(LANG_ANNOUNCE_COLOR, handler->GetSession()->GetPlayer()->GetName(), args);
        return true;
    }

    static bool HandleGMNameAnnounceCommand(ChatHandler* handler, const char* args)
    {
        WorldPacket data;
        if (!*args)
            return false;

        sWorld.SendGMText(LANG_GM_ANNOUNCE_COLOR, handler->GetSession()->GetPlayer()->GetName(), args);
        return true;
    }

    // global announce
    static bool HandleAnnounceCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        sWorld.SendWorldText(LANG_SYSTEMMESSAGE, args);
        return true;
    }

    // announce to logged in GMs
    static bool HandleGMAnnounceCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        sWorld.SendGMText(LANG_GM_BROADCAST, args);
        return true;
    }

    //notification player at the screen
    static bool HandleNotifyCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string str = handler->GetOregonString(LANG_GLOBAL_NOTIFY);
        str += args;

        WorldPacket data(SMSG_NOTIFICATION, (str.size() + 1));
        data << str;
        sWorld.SendGlobalMessage(&data);

        return true;
    }

    //notification GM at the screen
    static bool HandleGMNotifyCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string str = handler->GetOregonString(LANG_GM_NOTIFY);
        str += args;

        WorldPacket data(SMSG_NOTIFICATION, (str.size() + 1));
        data << str;
        sWorld.SendGlobalGMMessage(&data);

        return true;
    }

    static bool HandleWhispersCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
        {
            handler->PSendSysMessage(LANG_COMMAND_WHISPERACCEPTING, handler->GetSession()->GetPlayer()->isAcceptWhispers() ? handler->GetOregonString(LANG_ON) : handler->GetOregonString(LANG_OFF));
            return true;
        }

        std::string argstr = (char*)args;
        // whisper on
        if (argstr == "on")
        {
            handler->GetSession()->GetPlayer()->SetAcceptWhispers(true);
            handler->SendSysMessage(LANG_COMMAND_WHISPERON);
            return true;
        }

        // whisper off
        if (argstr == "off")
        {
            handler->GetSession()->GetPlayer()->SetAcceptWhispers(false);
            handler->SendSysMessage(LANG_COMMAND_WHISPEROFF);
            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }
};

void AddSC_message_commandscript()
{
    new message_commandscript();
}