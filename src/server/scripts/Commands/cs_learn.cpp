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

class learn_commandscript : public CommandScript
{
public:
    learn_commandscript() : CommandScript("learn_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> learnAllMyCommandTable = 
        {
            { "class",          SEC_GAMEMASTER,      false, &HandleLearnAllMyClassCommand,          "" },
            { "spells",         SEC_GAMEMASTER,      false, &HandleLearnAllMySpellsCommand,         "" },
            { "talents",        SEC_GAMEMASTER,      false, &HandleLearnAllMyTalentsCommand,        "" },
        };
        static std::vector<ChatCommand> learnAllCommandTable =
        {
            { "my",             SEC_GAMEMASTER,      false, nullptr,                                "", learnAllMyCommandTable },
            { "gm",             SEC_GAMEMASTER,      false, &HandleLearnAllGMCommand,               "" },
            { "crafts",         SEC_GAMEMASTER,      false, &HandleLearnAllCraftsCommand,           "" },
            { "default",        SEC_GAMEMASTER,      false, &HandleLearnAllDefaultCommand,          "" },
            { "lang",           SEC_GAMEMASTER,      false, &HandleLearnAllLangCommand,             "" },
            { "recipes",        SEC_GAMEMASTER,      false, &HandleLearnAllRecipesCommand,          "" },
        };
        static std::vector<ChatCommand> learnCommandTable =
        {
            { "all",            SEC_GAMEMASTER,     false, nullptr,                                 "", learnAllCommandTable },
            { "",               SEC_GAMEMASTER,     false, &HandleLearnCommand,                     "" },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "learn",          SEC_GAMEMASTER,     false, nullptr,                                 "", learnCommandTable },
            { "unlearn",        SEC_GAMEMASTER,     false, &HandleUnLearnCommand,                   "" },
        };
        return commandTable;
    };

    static bool HandleLearnAllMyClassCommand(ChatHandler* handler, char const* args)
    {
        HandleLearnAllMyTalentsCommand(handler,"");
        HandleLearnAllMySpellsCommand(handler,"");
        return true;
    }

    static bool HandleLearnAllMySpellsCommand(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        uint8 level = player->getLevel();
        uint32 teamID = player->GetTeamId();
        uint32 trainerID;

        switch (player->getClass())
        {
        case CLASS_WARRIOR:
            if (teamID == TEAM_ALLIANCE)
                trainerID = 17504;
            else
                trainerID = 985;
            break;
        case CLASS_ROGUE:
            if (teamID == TEAM_ALLIANCE)
                trainerID = 13283;
            else
                trainerID = 3401;
            break;
        case CLASS_SHAMAN:
            if (teamID == TEAM_ALLIANCE)
                trainerID = 20407;
            else
                trainerID = 13417;
            break;
        case CLASS_PRIEST:
            if (teamID == TEAM_ALLIANCE)
                trainerID = 11406;
            else
                trainerID = 16658;
            break;
        case CLASS_MAGE:
            if (teamID == TEAM_ALLIANCE)
                trainerID = 7312;
            else
                trainerID = 16653;
            break;
        case CLASS_WARLOCK:
            if (teamID == TEAM_ALLIANCE)
                trainerID = 5172;
            else
                trainerID = 16648;
            break;
        case CLASS_HUNTER:
            if (teamID == TEAM_ALLIANCE)
                trainerID = 5516;
            else
                trainerID = 3039;
            break;
        case CLASS_DRUID:
            if (teamID == TEAM_ALLIANCE)
                trainerID = 5504;
            else
                trainerID = 16655;
            break;
        case CLASS_PALADIN:
            if (teamID == TEAM_ALLIANCE)
                trainerID = 928;
            else
                trainerID = 16681;
            break;
        default:
            sLog.outDebug("HandleLearnAllMySpellsCommand Failed. Invalid Class %u.", player->getClass());
            return false;
        }

        QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT spell FROM npc_trainer WHERE reqlevel BETWEEN 1 AND %i AND entry = %i", level, trainerID);

        if (!result)
        {
            sLog.outErrorDb("0 spells found for HandleLearnAllMySpellsCommand function.");
            return false;
        }

        do
        {
            Field* fields = result->Fetch();

            uint32 spellID = fields[0].GetUInt32();

            SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellID);

            if (!spellInfo)
                continue;

            // skip wrong class/race skills
            if (!player->IsSpellFitByClassAndRace(spellInfo->Id))
                continue;

            // Skip known spells
            if (player->HasSpell(spellInfo->Id))
                continue;

            // Skip spells with first rank learned as talent (and all talents then also)
            if (GetTalentSpellCost(sSpellMgr.GetFirstSpellInChain(spellInfo->Id)) > 0)
                continue;

            // Skip broken spells
            if (!SpellMgr::IsSpellValid(spellInfo, player, false))
                continue;

            player->LearnSpell(spellInfo->Id);
        } while (result->NextRow());

        handler->SendSysMessage(LANG_COMMAND_LEARN_CLASS_SPELLS);
        return true;
    }

    static bool HandleLearnAllMyTalentsCommand(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        uint32 classMask = player->getClassMask();

        for (uint32 i = 0; i < sTalentStore.GetNumRows(); i++)
        {
            TalentEntry const* talentInfo = sTalentStore.LookupEntry(i);
            if (!talentInfo)
                continue;

            TalentTabEntry const* talentTabInfo = sTalentTabStore.LookupEntry(talentInfo->TalentTab);
            if (!talentTabInfo)
                continue;

            if ((classMask & talentTabInfo->ClassMask) == 0)
                continue;

            // search highest talent rank
            uint32 spellId = 0;
            int rank = 4;
            for (; rank >= 0; --rank)
            {
                if (talentInfo->RankID[rank] != 0)
                {
                    spellId = talentInfo->RankID[rank];
                    break;
                }
            }

            if (!spellId)                                        // ??? none spells in talent
                continue;

            SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);
            if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, handler->GetSession()->GetPlayer(), false))
                continue;

            // learn highest rank of talent
            player->LearnSpellHighestRank(spellId);
        }

        player->SetFreeTalentPoints(0);

        handler->SendSysMessage(LANG_COMMAND_LEARN_CLASS_TALENTS);
        return true;
    }

    static bool HandleLearnAllRecipesCommand(ChatHandler* handler, char const* args)
    {
        //  Learns all recipes of specified profession and sets skill to max
    //  Example: .learn all_recipes enchanting

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            return false;
        }

        if (!*args)
            return false;

        std::wstring wnamepart;

        if (!Utf8toWStr(args, wnamepart))
            return false;

        // converting string that we try to find to lower case
        wstrToLower(wnamepart);

        uint32 classmask = handler->GetSession()->GetPlayer()->getClassMask();

        for (uint32 i = 0; i < sSkillLineStore.GetNumRows(); ++i)
        {
            SkillLineEntry const* skillInfo = sSkillLineStore.LookupEntry(i);
            if (!skillInfo)
                continue;

            if (skillInfo->categoryId != SKILL_CATEGORY_PROFESSION &&
                skillInfo->categoryId != SKILL_CATEGORY_SECONDARY)
                continue;

            int loc = handler->GetSession()->GetSessionDbcLocale();
            std::string name = skillInfo->name[loc];

            if (Utf8FitTo(name, wnamepart))
            {
                for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
                {
                    SkillLineAbilityEntry const* skillLine = sSkillLineAbilityStore.LookupEntry(j);
                    if (!skillLine)
                        continue;

                    if (skillLine->skillId != i || skillLine->forward_spellid)
                        continue;

                    // skip racial skills
                    if (skillLine->racemask != 0)
                        continue;

                    // skip wrong class skills
                    if (skillLine->classmask && (skillLine->classmask & classmask) == 0)
                        continue;

                    SpellEntry const* spellInfo = sSpellStore.LookupEntry(skillLine->spellId);
                    if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, handler->GetSession()->GetPlayer(), false))
                        continue;

                    if (!target->HasSpell(spellInfo->Id))
                        handler->GetSession()->GetPlayer()->LearnSpell(skillLine->spellId);
                }

                uint16 maxLevel = target->GetPureMaxSkillValue(skillInfo->id);
                target->SetSkill(skillInfo->id, maxLevel, maxLevel);
                handler->PSendSysMessage(LANG_COMMAND_LEARN_ALL_RECIPES, name.c_str());
                return true;
            }
        }

        return false;
    }

    static bool HandleLearnAllLangCommand(ChatHandler* handler, char const* args)
    {
        // skipping UNIVERSAL language (0)
        for (uint8 i = 1; i < LANGUAGES_COUNT; ++i)
            handler->GetSession()->GetPlayer()->LearnSpell(lang_description[i].spell_id);

        handler->SendSysMessage(LANG_COMMAND_LEARN_ALL_LANG);
        return true;
    }

    static bool HandleLearnAllDefaultCommand(ChatHandler* handler, char const* args)
    {
        char* pName = strtok((char*)args, "");
        Player* player = NULL;
        if (pName)
        {
            std::string name = pName;

            if (!normalizePlayerName(name))
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }

            player = sObjectMgr.GetPlayer(name.c_str());
        }
        else
            player = handler->getSelectedPlayer();

        if (!player)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        player->LearnDefaultSpells();
        player->LearnQuestRewardedSpells();

        handler->PSendSysMessage(LANG_COMMAND_LEARN_ALL_DEFAULT_AND_QUEST, player->GetName());
        return true;
    }

    static bool HandleLearnAllCraftsCommand(ChatHandler* handler, char const* args)
    {
        uint32 classmask = handler->GetSession()->GetPlayer()->getClassMask();

        for (uint32 i = 0; i < sSkillLineStore.GetNumRows(); ++i)
        {
            SkillLineEntry const* skillInfo = sSkillLineStore.LookupEntry(i);
            if (!skillInfo)
                continue;

            if (skillInfo->categoryId == SKILL_CATEGORY_PROFESSION || skillInfo->categoryId == SKILL_CATEGORY_SECONDARY)
            {
                for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
                {
                    SkillLineAbilityEntry const* skillLine = sSkillLineAbilityStore.LookupEntry(j);
                    if (!skillLine)
                        continue;

                    // skip racial skills
                    if (skillLine->racemask != 0)
                        continue;

                    // skip wrong class skills
                    if (skillLine->classmask && (skillLine->classmask & classmask) == 0)
                        continue;

                    if (skillLine->skillId != i || skillLine->forward_spellid)
                        continue;

                    SpellEntry const* spellInfo = sSpellStore.LookupEntry(skillLine->spellId);
                    if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, handler->GetSession()->GetPlayer(), false))
                        continue;

                    handler->GetSession()->GetPlayer()->LearnSpell(skillLine->spellId);
                }
            }
        }

        handler->SendSysMessage(LANG_COMMAND_LEARN_ALL_CRAFT);
        return true;
    }

    static bool HandleLearnAllGMCommand(ChatHandler* handler, char const* args)
    {
        static const char* gmSpellList[] =
        {
            "24347",                                            // Become A Fish, No Breath Bar
            "35132",                                            // Visual Boom
            "38488",                                            // Attack 4000-8000 AOE
            "38795",                                            // Attack 2000 AOE + Slow Down 90%
            "15712",                                            // Attack 200
            "1852",                                             // GM Spell Silence
            "31899",                                            // Kill
            "31924",                                            // Kill
            "29878",                                            // Kill My Self
            "26644",                                            // More Kill

            "28550",                                            //Invisible 24
            "23452",                                            //Invisible + Target
            "0"
        };

        uint16 gmSpellIter = 0;
        while (strcmp(gmSpellList[gmSpellIter], "0"))
        {
            uint32 spell = atol((char*)gmSpellList[gmSpellIter++]);

            SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell);
            if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, handler->GetSession()->GetPlayer()))
            {
                handler->PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spell);
                continue;
            }

            handler->GetSession()->GetPlayer()->LearnSpell(spell);
        }

        handler->SendSysMessage(LANG_LEARNING_GM_SKILLS);
        return true;
    }

    static bool HandleLearnCommand(ChatHandler* handler, char const* args)
    {
        Player* targetPlayer = handler->getSelectedPlayerOrSelf();

        if (!targetPlayer)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spell = handler->extractSpellIdFromLink((char*)args);
        if (!spell || !sSpellStore.LookupEntry(spell))
            return false;

        if (targetPlayer->HasSpell(spell))
        {
            if (targetPlayer == handler->GetSession()->GetPlayer())
                handler->SendSysMessage(LANG_YOU_KNOWN_SPELL);
            else
                handler->PSendSysMessage(LANG_TARGET_KNOWN_SPELL, targetPlayer->GetName());
            handler->SetSentErrorMessage(true);
            return false;
        }

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell);
        if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, handler->GetSession()->GetPlayer()))
        {
            handler->PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spell);
            handler->SetSentErrorMessage(true);
            return false;
        }

        targetPlayer->LearnSpell(spell);

        return true;
    }

    static bool HandleUnLearnCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r
        uint32 min_id = handler->extractSpellIdFromLink((char*)args);
        if (!min_id)
            return false;

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r
        char* tail = strtok(NULL, "");

        uint32 max_id = handler->extractSpellIdFromLink(tail);

        if (!max_id)
        {
            // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r
            max_id = min_id + 1;
        }
        else
        {
            if (max_id < min_id)
                std::swap(min_id, max_id);

            max_id = max_id + 1;
        }

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        for (uint32 spell = min_id; spell < max_id; spell++)
        {
            if (target->HasSpell(spell))
                target->RemoveSpell(spell);
            else
                handler->SendSysMessage(LANG_FORGET_SPELL);
        }

        return true;
    }

};

void AddSC_learn_commandscript()
{
    new learn_commandscript();
}