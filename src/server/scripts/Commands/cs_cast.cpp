#include "ScriptMgr.h"
#include "Chat.h"
#include "Creature.h"
#include "Language.h"
#include "Player.h"
#include "Spell.h"

class cast_commandscript : public CommandScript
{

public:
    cast_commandscript() : CommandScript("cast_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> castCommandTable =
        {
            { "back",           SEC_GAMEMASTER,  false, &HandleCastBackCommand,              "" },
            { "dist",           SEC_GAMEMASTER,  false, &HandleCastDistCommand,              "" },
            { "self",           SEC_GAMEMASTER,  false, &HandleCastSelfCommand,              "" },
            { "",               SEC_GAMEMASTER,  false, &HandleCastCommand,                  "" }
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "cast",           SEC_GAMEMASTER,  false, nullptr,                                "", castCommandTable }
        };
        return commandTable;
    }

    static bool HandleCastCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Unit* target = handler->getSelectedUnit();

        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spell = handler->extractSpellIdFromLink((char*)args);
        if (!spell)
            return false;

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell);
        if (!spellInfo)
        {
            handler->PSendSysMessage(LANG_COMMAND_NOSPELLFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!SpellMgr::IsSpellValid(spellInfo, handler->GetSession()->GetPlayer()))
        {
            handler->PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spell);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* trig_str = strtok(NULL, " ");
        if (trig_str)
        {
            int l = strlen(trig_str);
            if (strncmp(trig_str, "triggered", l) != 0)
                return false;
        }

        bool triggered = (trig_str != NULL);

        handler->GetSession()->GetPlayer()->CastSpell(target, spell, triggered);

        return true;
    }
    
    static bool HandleCastBackCommand(ChatHandler* handler, const char* args)
    {
        Creature* caster = handler->getSelectedCreature();

        if (!caster)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r
        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spell = handler->extractSpellIdFromLink((char*)args);
        if (!spell || !sSpellStore.LookupEntry(spell))
        {
            handler->PSendSysMessage(LANG_COMMAND_NOSPELLFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* trig_str = strtok(NULL, " ");
        if (trig_str)
        {
            int l = strlen(trig_str);
            if (strncmp(trig_str, "triggered", l) != 0)
                return false;
        }

        bool triggered = (trig_str != NULL);

        caster->SetFacingToObject(handler->GetSession()->GetPlayer());

        caster->CastSpell(handler->GetSession()->GetPlayer(), spell, triggered);

        return true;
    }

    static bool HandleCastDistCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spell = handler->extractSpellIdFromLink((char*)args);
        if (!spell)
            return false;

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell);
        if (!spellInfo)
        {
            handler->PSendSysMessage(LANG_COMMAND_NOSPELLFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!SpellMgr::IsSpellValid(spellInfo, handler->GetSession()->GetPlayer()))
        {
            handler->PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spell);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* distStr = strtok(NULL, " ");

        float dist = 0;

        if (distStr)
            sscanf(distStr, "%f", &dist);

        char* trig_str = strtok(NULL, " ");
        if (trig_str)
        {
            int l = strlen(trig_str);
            if (strncmp(trig_str, "triggered", l) != 0)
                return false;
        }

        bool triggered = (trig_str != NULL);

        float x, y, z;
        handler->GetSession()->GetPlayer()->GetClosePoint(x, y, z, dist);

        handler->GetSession()->GetPlayer()->CastSpell(x, y, z, spell, triggered);
        return true;
    }

    static bool HandleCastTargetCommand(ChatHandler* handler, const char* args)
    {
        Creature* caster = handler->getSelectedCreature();

        if (!caster)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!caster->GetVictim())
        {
            handler->SendSysMessage(LANG_SELECTED_TARGET_NOT_HAVE_VICTIM);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spell = handler->extractSpellIdFromLink((char*)args);
        if (!spell || !sSpellStore.LookupEntry(spell))
        {
            handler->PSendSysMessage(LANG_COMMAND_NOSPELLFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* trig_str = strtok(NULL, " ");
        if (trig_str)
        {
            int l = strlen(trig_str);
            if (strncmp(trig_str, "triggered", l) != 0)
                return false;
        }

        bool triggered = (trig_str != NULL);

        caster->SetFacingToObject(handler->GetSession()->GetPlayer());

        caster->CastSpell(caster->GetVictim(), spell, triggered);

        return true;
    }

    static bool HandleCastSelfCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Unit* target = handler->getSelectedUnit();

        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spell = handler->extractSpellIdFromLink((char*)args);
        if (!spell)
            return false;

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell);
        if (!spellInfo)
            return false;

        if (!SpellMgr::IsSpellValid(spellInfo, handler->GetSession()->GetPlayer()))
        {
            handler->PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spell);
            handler->SetSentErrorMessage(true);
            return false;
        }

        target->CastSpell(target, spell, false);

        return true;
    }
};

void AddSC_cast_commandscript()
{
    new cast_commandscript();
}