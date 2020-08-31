#include "Chat.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"

class honor_commandscript : public CommandScript
{
public:
    honor_commandscript() : CommandScript("honor_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> honorAddCommandTable =
        {
            { "kill",           SEC_GAMEMASTER,     false, &HandleHonorAddKillCommand,         "" },
            { "",               SEC_GAMEMASTER,     false, &HandleAddHonorCommand,             "" }
        };

        static std::vector<ChatCommand> honorCommandTable =
        {
            { "add",            SEC_GAMEMASTER,     false, nullptr,                            "", honorAddCommandTable },
            { "update",         SEC_GAMEMASTER,     false, &HandleUpdateHonorFieldsCommand,    "" }
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "honor",          SEC_GAMEMASTER,     false, nullptr,                            "", honorCommandTable }
        };
        return commandTable;
    }

    static bool HandleHonorAddKillCommand(ChatHandler* handler, const char* /*args*/)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->GetSession()->GetPlayer()->RewardHonor(target, 1);
        return true;
    }

    static bool HandleAddHonorCommand(ChatHandler* handler, const char* args)
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

        uint32 amount = (uint32)atoi(args);
        target->RewardHonor(NULL, 1, amount);
        return true;
    }


    static bool HandleUpdateHonorFieldsCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        target->UpdateHonorFields();
        return true;
    }
};

void AddSC_honor_commandscript()
{
    new honor_commandscript();
}
