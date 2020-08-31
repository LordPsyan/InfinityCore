#include "Player.h"
#include "ScriptMgr.h"
#include "Pet.h"
#include "Language.h"
#include "Chat.h"

class pet_commandscript : CommandScript
{
public:
    pet_commandscript() : CommandScript("pet_commandscript") {}

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> petCommandTable =
        {
            { "create",         SEC_GAMEMASTER,     false, HandleCreatePetCommand,           ""},
            { "learn",          SEC_GAMEMASTER,     false, HandlePetLearnCommand,            ""},
            { "unlearn",        SEC_GAMEMASTER,     false, HandlePetUnlearnCommand,          ""},
            { "tp",             SEC_GAMEMASTER,     false, HandlePetTpCommand,               ""},
            { NULL,             0,                  false, NULL,                             ""}
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "pet",          SEC_MODERATOR,   true, nullptr,                                 "", petCommandTable }
        };
        return commandTable;
    }

    static bool HandleCreatePetCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();
        Creature* creatureTarget = handler->getSelectedCreature();

        if (!creatureTarget || creatureTarget->IsPet() || creatureTarget->GetTypeId() == TYPEID_PLAYER)
        {
            handler->PSendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        CreatureInfo const* cInfo = sObjectMgr.GetCreatureTemplate(creatureTarget->GetEntry());
        // Creatures with family CREATURE_FAMILY_NONE crashes the server
        if (cInfo->family == CREATURE_FAMILY_NONE)
        {
            handler->PSendSysMessage("This creature cannot be tamed. Family id: 0 (CREATURE_FAMILY_NONE).");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetPetGUID())
        {
            handler->PSendSysMessage("You already have a pet");
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Everything looks OK, create new pet
        Pet* pet = new Pet(player, HUNTER_PET);

        if (!pet)
            return false;

        if (!pet->CreateBaseAtCreature(creatureTarget))
        {
            delete pet;
            handler->PSendSysMessage("Error 1");
            return false;
        }

        creatureTarget->setDeathState(JUST_DIED);
        creatureTarget->RemoveCorpse();
        creatureTarget->SetHealth(0); // just for nice GM-mode view

        pet->SetUInt64Value(UNIT_FIELD_CREATEDBY, player->GetGUID());
        pet->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, player->GetFaction());

        if (!pet->InitStatsForLevel(creatureTarget->getLevel()))
        {
            sLog.outError("InitStatsForLevel() in EffectTameCreature failed! Pet deleted.");
            handler->PSendSysMessage("Error 2");
            delete pet;
            return false;
        }

        // prepare visual effect for levelup
        pet->SetUInt32Value(UNIT_FIELD_LEVEL, creatureTarget->getLevel() - 1);

        pet->GetCharmInfo()->SetPetNumber(sObjectMgr.GeneratePetNumber(), true);
        // this enables pet details window (Shift+P)
        pet->InitPetCreateSpells();
        pet->SetHealth(pet->GetMaxHealth());

        pet->GetMap()->AddToMap(pet->ToCreature());

        // visual effect for levelup
        pet->SetUInt32Value(UNIT_FIELD_LEVEL, creatureTarget->getLevel());

        player->SetMinion(pet, true);
        pet->SavePetToDB(PET_SAVE_AS_CURRENT);
        player->PetSpellInitialize();

        return true;
    }

    static bool HandlePetLearnCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* plr = handler->GetSession()->GetPlayer();
        Pet* pet = plr->GetPet();

        if (!pet)
        {
            handler->PSendSysMessage("You have no pet");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 spellId = handler->extractSpellIdFromLink((char*)args);

        if (!spellId || !sSpellStore.LookupEntry(spellId))
            return false;

        // Check if pet already has it
        if (pet->HasSpell(spellId))
        {
            handler->PSendSysMessage("Pet already has spell: %u", spellId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Check if spell is valid
        SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);
        if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo))
        {
            handler->PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spellId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        pet->learnSpell(spellId);

        handler->PSendSysMessage("Pet has learned spell %u", spellId);
        return true;
    }

    static bool HandlePetUnlearnCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* plr = handler->GetSession()->GetPlayer();
        Pet* pet = plr->GetPet();

        if (!pet)
        {
            handler->PSendSysMessage("You have no pet");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 spellId = handler->extractSpellIdFromLink((char*)args);

        if (pet->HasSpell(spellId))
            pet->removeSpell(spellId);
        else
            handler->PSendSysMessage("Pet doesn't have that spell");

        return true;
    }

    static bool HandlePetTpCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* plr = handler->GetSession()->GetPlayer();
        Pet* pet = plr->GetPet();

        if (!pet)
        {
            handler->PSendSysMessage("You have no pet");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 tp = atol(args);

        pet->SetTP(tp);

        handler->PSendSysMessage("Pet's tp changed to %u", tp);
        return true;
    }
};

void AddSC_pet_commandscript()
{
    new pet_commandscript();
}