#include "Chat.h"
#include "GameEventMgr.h"
#include "Language.h"
#include "Player.h"
#include "ScriptMgr.h"

class event_commandscript : public CommandScript
{
public:
    event_commandscript() : CommandScript("event_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> eventCommandTable =
        {
            { "activelist",     SEC_GAMEMASTER,     true,  &HandleEventActiveListCommand,     "" },
            { "start",          SEC_GAMEMASTER,     true,  &HandleEventStartCommand,          "" },
            { "stop",           SEC_GAMEMASTER,     true,  &HandleEventStopCommand,           "" },
            { "",               SEC_GAMEMASTER,     true,  &HandleEventInfoCommand,           "" }
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "event",          SEC_GAMEMASTER,     false, nullptr,                  "", eventCommandTable }
        };
        return commandTable;
    }

    static bool HandleEventActiveListCommand(ChatHandler* handler, const char* /*args*/)
    {
        uint32 counter = 0;

        GameEventMgr::GameEventDataMap const& events = sGameEventMgr.GetEventMap();
        GameEventMgr::ActiveEvents const& activeEvents = sGameEventMgr.GetActiveEventList();

        char const* active = handler->GetOregonString(LANG_ACTIVE);

        for (GameEventMgr::ActiveEvents::const_iterator itr = activeEvents.begin(); itr != activeEvents.end(); ++itr)
        {
            uint32 event_id = *itr;
            GameEventData const& eventData = events[event_id];

            if (handler->GetSession())
                handler->PSendSysMessage(LANG_EVENT_ENTRY_LIST_CHAT, event_id, event_id, eventData.description.c_str(), active);
            else
                handler->PSendSysMessage(LANG_EVENT_ENTRY_LIST_CONSOLE, event_id, eventData.description.c_str(), active);

            ++counter;
        }

        if (counter == 0)
            handler->SendSysMessage(LANG_NOEVENTFOUND);

        return true;
    }

    static bool HandleEventStartCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // id or [name] Shift-click form |color|Hgameevent:id|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hgameevent");
        if (!cId)
            return false;

        int32 event_id = atoi(cId);

        GameEventMgr::GameEventDataMap const& events = sGameEventMgr.GetEventMap();

        if (event_id < 1 || uint32(event_id) >= events.size())
        {
            handler->SendSysMessage(LANG_EVENT_NOT_EXIST);
            handler->SetSentErrorMessage(true);
            return false;
        }

        GameEventData const& eventData = events[event_id];
        if (!eventData.isValid())
        {
            handler->SendSysMessage(LANG_EVENT_NOT_EXIST);
            handler->SetSentErrorMessage(true);
            return false;
        }

        GameEventMgr::ActiveEvents const& activeEvents = sGameEventMgr.GetActiveEventList();
        if (activeEvents.find(event_id) != activeEvents.end())
        {
            handler->PSendSysMessage(LANG_EVENT_ALREADY_ACTIVE, event_id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        sGameEventMgr.StartEvent(event_id, true);
        return true;
    }

    static bool HandleEventStopCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // id or [name] Shift-click form |color|Hgameevent:id|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hgameevent");
        if (!cId)
            return false;

        int32 event_id = atoi(cId);

        GameEventMgr::GameEventDataMap const& events = sGameEventMgr.GetEventMap();

        if (event_id < 1 || uint32(event_id) >= events.size())
        {
            handler->SendSysMessage(LANG_EVENT_NOT_EXIST);
            handler->SetSentErrorMessage(true);
            return false;
        }

        GameEventData const& eventData = events[event_id];
        if (!eventData.isValid())
        {
            handler->SendSysMessage(LANG_EVENT_NOT_EXIST);
            handler->SetSentErrorMessage(true);
            return false;
        }

        GameEventMgr::ActiveEvents const& activeEvents = sGameEventMgr.GetActiveEventList();

        if (activeEvents.find(event_id) == activeEvents.end())
        {
            handler->PSendSysMessage(LANG_EVENT_NOT_ACTIVE, event_id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        sGameEventMgr.StopEvent(event_id, true);
        return true;
    }

    static bool HandleEventInfoCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // id or [name] Shift-click form |color|Hgameevent:id|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hgameevent");
        if (!cId)
            return false;

        uint32 event_id = atoi(cId);

        GameEventMgr::GameEventDataMap const& events = sGameEventMgr.GetEventMap();

        if (event_id >= events.size())
        {
            handler->SendSysMessage(LANG_EVENT_NOT_EXIST);
            handler->SetSentErrorMessage(true);
            return false;
        }

        GameEventData const& eventData = events[event_id];
        if (!eventData.isValid())
        {
            handler->SendSysMessage(LANG_EVENT_NOT_EXIST);
            handler->SetSentErrorMessage(true);
            return false;
        }

        GameEventMgr::ActiveEvents const& activeEvents = sGameEventMgr.GetActiveEventList();
        bool active = activeEvents.find(event_id) != activeEvents.end();
        char const* activeStr = active ? handler->GetOregonString(LANG_ACTIVE) : "";

        std::string startTimeStr = TimeToTimestampStr(eventData.start);
        std::string endTimeStr = TimeToTimestampStr(eventData.end);

        uint32 delay = sGameEventMgr.NextCheck(event_id);
        time_t nextTime = time(NULL) + delay;
        std::string nextStr = nextTime >= eventData.start && nextTime < eventData.end ? TimeToTimestampStr(time(NULL) + delay) : "-";

        std::string occurenceStr = secsToTimeString(eventData.occurence * MINUTE);
        std::string lengthStr = secsToTimeString(eventData.length * MINUTE);

        handler->PSendSysMessage(LANG_EVENT_INFO, event_id, eventData.description.c_str(), activeStr,
            startTimeStr.c_str(), endTimeStr.c_str(), occurenceStr.c_str(), lengthStr.c_str(),
            nextStr.c_str());
        return true;
    }
};

void AddSC_event_commandscript()
{
    new event_commandscript();
}