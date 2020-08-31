#include "Chat.h"
#include "Language.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SystemConfig.h"

class server_commandscript : public CommandScript
{
public:
    server_commandscript() : CommandScript("server_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> serverIdleRestartCommandTable =
        {
            { "cancel",         SEC_ADMINISTRATOR,  true,  &HandleServerShutDownCancelCommand,      "" },
            { ""   ,            SEC_ADMINISTRATOR,  true,  &HandleServerIdleRestartCommand,         "" }
        };

        static std::vector<ChatCommand> serverIdleShutdownCommandTable =
        {
            { "cancel",         SEC_ADMINISTRATOR,  true,  &HandleServerShutDownCancelCommand,      "" },
            { ""   ,            SEC_ADMINISTRATOR,  true,  &HandleServerIdleShutDownCommand,        "" }
        };

        static std::vector<ChatCommand> serverRestartCommandTable =
        {
            { "cancel",         SEC_ADMINISTRATOR,  true,  &HandleServerShutDownCancelCommand,      "" },
            { ""   ,            SEC_ADMINISTRATOR,  true,  &HandleServerRestartCommand,             "" }
        };

        static std::vector<ChatCommand> serverShutdownCommandTable =
        {
            { "cancel",         SEC_ADMINISTRATOR,  true,  &HandleServerShutDownCancelCommand,      "" },
            { ""   ,            SEC_ADMINISTRATOR,  true,  &HandleServerShutDownCommand,            "" }
        };

        static std::vector<ChatCommand> serverSetCommandTable =
        {
            { "difftime",       SEC_CONSOLE,        true,  &HandleServerSetDiffTimeCommand,         "" },
            { "motd",           SEC_ADMINISTRATOR,  true,  &HandleServerSetMotdCommand,             "" },
            { "logmask",        SEC_CONSOLE,        true,  &HandleServerSetLogMaskCommand,          "" }
        };

        static std::vector<ChatCommand> serverCommandTable =
        {
            { "corpses",        SEC_GAMEMASTER,     true,  &HandleServerCorpsesCommand,             "" },
            { "exit",           SEC_CONSOLE,        true,  &HandleServerExitCommand,                "" },
            { "idlerestart",    SEC_ADMINISTRATOR,  true,  nullptr,                                 "", serverIdleRestartCommandTable },
            { "idleshutdown",   SEC_ADMINISTRATOR,  true,  nullptr,                                 "", serverIdleShutdownCommandTable },
            { "info",           SEC_PLAYER,         true,  &HandleServerInfoCommand,                "" },
            { "motd",           SEC_PLAYER,         true,  &HandleServerMotdCommand,                "" },
            { "restart",        SEC_ADMINISTRATOR,  true,  nullptr,                                 "", serverRestartCommandTable },
            { "shutdown",       SEC_ADMINISTRATOR,  true,  nullptr,                                 "", serverShutdownCommandTable },
            { "set",            SEC_ADMINISTRATOR,  true,  nullptr,                                 "", serverSetCommandTable },
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "server",         SEC_PLAYER,         true,  nullptr,                                 "", serverCommandTable }
        };
        return commandTable;
    }

    static bool HandleServerShutDownCancelCommand(ChatHandler* /*handler*/,const char* /*args*/)
    {
        sWorld.ShutdownCancel();
        return true;
    }

    static bool HandleServerIdleRestartCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* time_str = strtok((char*)args, " ");
        char* exitcode_str = strtok(NULL, "");

        int32 time = atoi(time_str);

        // Prevent interpret wrong arg value as 0 secs shutdown time
        if ((time == 0 && (time_str[0] != '0' || time_str[1] != '\0')) || time < 0)
            return false;

        if (exitcode_str)
        {
            int32 exitcode = atoi(exitcode_str);

            // Handle atoi() errors
            if (exitcode == 0 && (exitcode_str[0] != '0' || exitcode_str[1] != '\0'))
                return false;

            // Exit code should be in range of 0-125, 126-255 is used
            // in many shells for their own return codes and code > 255
            // is not supported in many others
            if (exitcode < 0 || exitcode > 125)
                return false;

            sWorld.ShutdownServ(time, SHUTDOWN_MASK_RESTART | SHUTDOWN_MASK_IDLE, exitcode);
        }
        else
            sWorld.ShutdownServ(time, SHUTDOWN_MASK_RESTART | SHUTDOWN_MASK_IDLE, RESTART_EXIT_CODE);
        return true;
    }

    // Triggering corpses expire check in world
    static bool HandleServerCorpsesCommand(ChatHandler* /*handler*/, const char* /*args*/)
    {
        ObjectAccessor::Instance().RemoveOldCorpses();
        return true;
    }

    static bool HandleServerIdleShutDownCommand(ChatHandler* /*handler*/, const char* args)
    {
        if (!*args)
            return false;

        char* time_str = strtok((char*)args, " ");
        char* exitcode_str = strtok(NULL, "");

        int32 time = atoi(time_str);

        // Prevent interpret wrong arg value as 0 secs shutdown time
        if ((time == 0 && (time_str[0] != '0' || time_str[1] != '\0')) || time < 0)
            return false;

        if (exitcode_str)
        {
            int32 exitcode = atoi(exitcode_str);

            // Handle atoi() errors
            if (exitcode == 0 && (exitcode_str[0] != '0' || exitcode_str[1] != '\0'))
                return false;

            // Exit code should be in range of 0-125, 126-255 is used
            // in many shells for their own return codes and code > 255
            // is not supported in many others
            if (exitcode < 0 || exitcode > 125)
                return false;

            sWorld.ShutdownServ(time, SHUTDOWN_MASK_IDLE, exitcode);
        }
        else
            sWorld.ShutdownServ(time, SHUTDOWN_MASK_IDLE, SHUTDOWN_EXIT_CODE);
        return true;
    }

    static bool HandleServerRestartCommand(ChatHandler* /*handler*/, const char* args)
    {
        if (!*args)
            return false;

        char* time_str = strtok((char*)args, " ");
        char* exitcode_str = strtok(NULL, "");

        int32 time = atoi(time_str);

        // Prevent interpret wrong arg value as 0 secs shutdown time
        if ((time == 0 && (time_str[0] != '0' || time_str[1] != '\0')) || time < 0)
            return false;

        if (exitcode_str)
        {
            int32 exitcode = atoi(exitcode_str);

            // Handle atoi() errors
            if (exitcode == 0 && (exitcode_str[0] != '0' || exitcode_str[1] != '\0'))
                return false;

            // Exit code should be in range of 0-125, 126-255 is used
            // in many shells for their own return codes and code > 255
            // is not supported in many others
            if (exitcode < 0 || exitcode > 125)
                return false;

            sWorld.ShutdownServ(time, SHUTDOWN_MASK_RESTART, exitcode);
        }
        else
            sWorld.ShutdownServ(time, SHUTDOWN_MASK_RESTART, RESTART_EXIT_CODE);
        return true;
    }

    static bool HandleServerShutDownCommand(ChatHandler* /*handler*/, const char* args)
    {
        if (!*args)
            return false;

        char* time_str = strtok((char*)args, " ");
        char* exitcode_str = strtok(NULL, "");

        int32 time = atoi(time_str);

        // Prevent interpret wrong arg value as 0 secs shutdown time
        if ((time == 0 && (time_str[0] != '0' || time_str[1] != '\0')) || time < 0)
            return false;

        if (exitcode_str)
        {
            int32 exitcode = atoi(exitcode_str);

            // Handle atoi() errors
            if (exitcode == 0 && (exitcode_str[0] != '0' || exitcode_str[1] != '\0'))
                return false;

            // Exit code should be in range of 0-125, 126-255 is used
            // in many shells for their own return codes and code > 255
            // is not supported in many others
            if (exitcode < 0 || exitcode > 125)
                return false;

            sWorld.ShutdownServ(time, 0, exitcode);
        }
        else
            sWorld.ShutdownServ(time, 0, SHUTDOWN_EXIT_CODE);
        return true;
    }

    // set diff time record interval
    static bool HandleServerSetDiffTimeCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* NewTimeStr = strtok((char*)args, " ");
        if (!NewTimeStr)
            return false;

        int32 NewTime = atoi(NewTimeStr);
        if (NewTime < 0)
            return false;

        sWorld.SetRecordDiffInterval(NewTime);
        printf("Record diff every %u ms\n", NewTime);
        return true;
    }

    // Define the 'Message of the day' for the realm
    static bool HandleServerSetMotdCommand(ChatHandler* handler, const char* args)
    {
        sWorld.SetMotd(args);
        handler->PSendSysMessage(LANG_MOTD_NEW, args);
        return true;
    }

    // Sets/gets the mask for logging
    static bool HandleServerSetLogMaskCommand(ChatHandler* handler, const char* args)
    {
        // no arguments, retrieve current log masks
        if (!*args)
        {
            handler->PSendSysMessage("Current logging mask: %lu", sLog.GetLogMask());
            handler->PSendSysMessage("Current logging db mask: %lu", sLog.GetDBLogMask());
            return true;
        }

        // first argument
        unsigned long mask = strtoul(args, NULL, 0); // recognize base
        sLog.SetLogMask(mask);

        handler->PSendSysMessage("Logging mask set to %lu", mask);

        // second argument (if set)
        if ((args = strchr(args, ' ')))
        {
            ++args;
            mask = strtoul(args, NULL, 0); // recognize base
            sLog.SetDBLogMask(mask);

            handler->PSendSysMessage("Logging db mask set to %lu", mask);
        }
        return true;
    }

    // Exit the realm
    static bool HandleServerExitCommand(ChatHandler* handler, const char* /*args*/)
    {
        handler->SendSysMessage(LANG_COMMAND_EXIT);
        World::StopNow(SHUTDOWN_EXIT_CODE);
        return true;
    }

    static bool HandleServerInfoCommand(ChatHandler* handler, const char* /*args*/)
    {
        uint32 activeClientsNum = sWorld.GetActiveSessionCount();
        uint32 queuedClientsNum = sWorld.GetQueuedSessionCount();
        uint32 maxActiveClientsNum = sWorld.GetMaxActiveSessionCount();
        uint32 maxQueuedClientsNum = sWorld.GetMaxQueuedSessionCount();
        std::string str = secsToTimeString(sWorld.GetUptime());
        uint32 updateTime = sWorld.GetUpdateTime();

        handler->PSendSysMessage(_FULLVERSION);
        //if (m_session)
        //    full = _FULLVERSION(REVISION_DATE,REVISION_TIME,"|cffffffff|Hurl:" REVISION_ID "|h" REVISION_ID "|h|r");
        //else
        //    full = _FULLVERSION(REVISION_DATE,REVISION_TIME,REVISION_ID);

        //SendSysMessage(full);
        //PSendSysMessage(LANG_USING_WORLD_DB,sWorld.GetDBVersion());
        handler->PSendSysMessage(LANG_CONNECTED_USERS, activeClientsNum, maxActiveClientsNum, queuedClientsNum, maxQueuedClientsNum);
        handler->PSendSysMessage(LANG_UPTIME, str.c_str());
        handler->PSendSysMessage(LANG_UPDATE_DIFF, updateTime);
        //! Can't use sWorld->ShutdownMsg here in case of console command
        if (sWorld.IsShuttingDown())
            handler->PSendSysMessage(LANG_SHUTDOWN_TIMELEFT, secsToTimeString(sWorld.GetShutDownTimeLeft()).c_str());

        return true;
    }

    // Display the 'Message of the day' for the realm
    static bool HandleServerMotdCommand(ChatHandler* handler, const char* /*args*/)
    {
        handler->PSendSysMessage(LANG_MOTD_CURRENT, sWorld.GetMotd());
        return true;
    }
};

void AddSC_server_commandscript()
{
    new server_commandscript();
}