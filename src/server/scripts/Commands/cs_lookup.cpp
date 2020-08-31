#include "AccountMgr.h"
#include "Chat.h"
#include "ObjectMgr.h"
#include "ObjectAccessor.h"
#include "ReputationMgr.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "SpellMgr.h"
#include "Player.h"
#include "GameEventMgr.h"
#include "World.h"
#include "Language.h"

static uint32 ReputationRankStrIndex[MAX_REPUTATION_RANK] =
{
    LANG_REP_HATED,    LANG_REP_HOSTILE, LANG_REP_UNFRIENDLY, LANG_REP_NEUTRAL,
    LANG_REP_FRIENDLY, LANG_REP_HONORED, LANG_REP_REVERED,    LANG_REP_EXALTED
};

class lookup_commandscript : public CommandScript
{
public:
    lookup_commandscript() : CommandScript("lookup_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> lookupPlayerCommandTable =
        {
            { "ip",             SEC_GAMEMASTER,     true,  &HandleLookupPlayerIpCommand,        "" },
            { "account",        SEC_GAMEMASTER,     true,  &HandleLookupPlayerAccountCommand,   "" },
            { "email",          SEC_GAMEMASTER,     true,  &HandleLookupPlayerEmailCommand,     "" }
        };

        static std::vector<ChatCommand> lookupSpellCommandTable =
        {
            { "",               SEC_MODERATOR,      true,  &HandleLookupSpellCommand,           "" }
        };

        static std::vector<ChatCommand> lookupCommandTable =
        {
            { "area",           SEC_MODERATOR,      true,  &HandleLookupAreaCommand,        "" },
            { "creature",       SEC_MODERATOR,      true,  &HandleLookupCreatureCommand,    "" },
            { "event",          SEC_MODERATOR,      true,  &HandleLookupEventCommand,       "" },
            { "faction",        SEC_MODERATOR,      true,  &HandleLookupFactionCommand,     "" },
            { "item",           SEC_MODERATOR,      true,  &HandleLookupItemCommand,        "" },
            { "itemset",        SEC_MODERATOR,      true,  &HandleLookupItemSetCommand,     "" },
            { "object",         SEC_MODERATOR,      true,  &HandleLookupObjectCommand,      "" },
            { "gobject",        SEC_MODERATOR,      true,  &HandleLookupObjectCommand,      "" },
            { "quest",          SEC_MODERATOR,      true,  &HandleLookupQuestCommand,       "" },
            { "skill",          SEC_MODERATOR,      true,  &HandleLookupSkillCommand,       "" },
            { "tele",           SEC_MODERATOR,      true,  &HandleLookupTeleCommand,        "" },
            { "title",          SEC_MODERATOR,      true,  &HandleLookupTitleCommand,       "" },
            { "player",         SEC_GAMEMASTER,     true,  nullptr, "", lookupPlayerCommandTable },
            { "spell",          SEC_MODERATOR,      true,  nullptr, "", lookupSpellCommandTable }
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "lookup",         SEC_MODERATOR,  true,  nullptr,                                 "", lookupCommandTable }
        };
        return commandTable;
    }

    static bool HandleLookupPlayerIpCommand(ChatHandler* handler, const char* args)
    {

        if (!*args)
            return false;

        std::string ip = strtok((char*)args, " ");
        char* limit_str = strtok(NULL, " ");
        int32 limit = limit_str ? atoi(limit_str) : -1;

        LoginDatabase.escape_string(ip);

        QueryResult_AutoPtr result = LoginDatabase.PQuery("SELECT id,username FROM account WHERE last_ip = '%s'", ip.c_str());

        return LookupPlayerSearchCommand(handler, result, limit);
    }

    static bool HandleLookupPlayerAccountCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string account = strtok((char*)args, " ");
        char* limit_str = strtok(NULL, " ");
        int32 limit = limit_str ? atoi(limit_str) : -1;

        if (!AccountMgr::normalizeString(account))
            return false;

        LoginDatabase.escape_string(account);

        QueryResult_AutoPtr result = LoginDatabase.PQuery("SELECT id,username FROM account WHERE username = '%s'", account.c_str());

        return LookupPlayerSearchCommand(handler, result, limit);
    }

    static bool LookupPlayerSearchCommand(ChatHandler* handler, QueryResult_AutoPtr result, int32 limit)
    {
        if (!result)
        {
            handler->PSendSysMessage(LANG_NO_PLAYERS_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        int i = 0;
        uint32 count = 0;
        uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);
        do
        {
            if (maxResults && count++ == maxResults)
            {
                handler->PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                return true;
            }

            Field* fields = result->Fetch();
            uint32 acc_id = fields[0].GetUInt32();
            std::string acc_name = fields[1].GetCppString();

            QueryResult_AutoPtr chars = CharacterDatabase.PQuery("SELECT guid,name FROM characters WHERE account = '%u'", acc_id);
            if (chars)
            {
                handler->PSendSysMessage(LANG_LOOKUP_PLAYER_ACCOUNT, acc_name.c_str(), acc_id);

                uint64 guid = 0;
                std::string name;

                do
                {
                    Field* charfields = chars->Fetch();
                    guid = charfields[0].GetUInt64();
                    name = charfields[1].GetCppString();

                    handler->PSendSysMessage(LANG_LOOKUP_PLAYER_CHARACTER, name.c_str(), guid);
                    ++i;

                } while (chars->NextRow() && (limit == -1 || i < limit));
            }
        } while (result->NextRow());

        return true;
    }

    static bool HandleLookupPlayerEmailCommand(ChatHandler* handler, const char* args)
    {

        if (!*args)
            return false;

        std::string email = strtok((char*)args, " ");
        char* limit_str = strtok(NULL, " ");
        int32 limit = limit_str ? atoi(limit_str) : -1;

        LoginDatabase.escape_string(email);

        QueryResult_AutoPtr result = LoginDatabase.PQuery("SELECT id,username FROM account WHERE email = '%s'", email.c_str());

        return LookupPlayerSearchCommand(handler, result, limit);
    }

    static bool HandleLookupAreaCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string namepart = args;
        std::wstring wnamepart;

        if (!Utf8toWStr(namepart, wnamepart))
            return false;

        bool found = false;
        uint32 count = 0;
        uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

        // converting string that we try to find to lower case
        wstrToLower(wnamepart);

        // Search in AreaTable.dbc
        for (uint32 areaflag = 0; areaflag < sAreaStore.GetNumRows(); ++areaflag)
        {
            AreaTableEntry const* areaEntry = sAreaStore.LookupEntry(areaflag);
            if (areaEntry)
            {
                int loc = handler->GetSession() ? int(handler->GetSession()->GetSessionDbcLocale()) : sWorld.GetDefaultDbcLocale();
                std::string name = areaEntry->area_name[loc];
                if (name.empty())
                    continue;

                if (!Utf8FitTo(name, wnamepart))
                {
                    loc = 0;
                    for (; loc < MAX_LOCALE; ++loc)
                    {
                        if (handler->GetSession() && loc == handler->GetSession()->GetSessionDbcLocale())
                            continue;

                        name = areaEntry->area_name[loc];
                        if (name.empty())
                            continue;

                        if (Utf8FitTo(name, wnamepart))
                            break;
                    }
                }

                if (loc < MAX_LOCALE)
                {
                    if (maxResults && count++ == maxResults)
                    {
                        handler->PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                        return true;
                    }

                    // send area in "id - [name]" format
                    std::ostringstream ss;
                    if (handler->GetSession())
                        ss << areaEntry->ID << " - |cffffffff|Harea:" << areaEntry->ID << "|h[" << name << " " << localeNames[loc] << "]|h|r";
                    else
                        ss << areaEntry->ID << " - " << name << " " << localeNames[loc];

                    handler->SendSysMessage(ss.str().c_str());

                    if (!found)
                        found = true;
                }
            }
        }

        if (!found)
            handler->SendSysMessage(LANG_COMMAND_NOAREAFOUND);

        return true;
    }

    static bool HandleLookupCreatureCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string namepart = args;
        std::wstring wnamepart;

        // converting string that we try to find to lower case
        if (!Utf8toWStr(namepart, wnamepart))
            return false;

        wstrToLower(wnamepart);

        bool found = false;
        uint32 count = 0;
        uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

        for (uint32 id = 0; id < sCreatureStorage.MaxEntry; ++id)
        {
            CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(id);
            if (!cInfo)
                continue;

            int loc_idx = handler->GetSession() ? handler->GetSession()->GetSessionDbLocaleIndex() : sObjectMgr.GetDBCLocaleIndex();
            if (loc_idx >= 0)
            {
                CreatureLocale const* cl = sObjectMgr.GetCreatureLocale(id);
                if (cl)
                {
                    if (cl->Name.size() > uint32(loc_idx) && !cl->Name[loc_idx].empty())
                    {
                        std::string name = cl->Name[loc_idx];

                        if (Utf8FitTo(name, wnamepart))
                        {
                            if (maxResults && count++ == maxResults)
                            {
                                handler->PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                                return true;
                            }

                            if (handler->GetSession())
                                handler->PSendSysMessage(LANG_CREATURE_ENTRY_LIST_CHAT, id, id, name.c_str());
                            else
                                handler->PSendSysMessage(LANG_CREATURE_ENTRY_LIST_CONSOLE, id, name.c_str());

                            if (!found)
                                found = true;

                            continue;
                        }
                    }
                }
            }

            std::string name = cInfo->Name;
            if (name.empty())
                continue;

            if (Utf8FitTo(name, wnamepart))
            {
                if (maxResults && count++ == maxResults)
                {
                    handler->PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                    return true;
                }

                if (handler->GetSession())
                    handler->PSendSysMessage(LANG_CREATURE_ENTRY_LIST_CHAT, id, id, name.c_str());
                else
                    handler->PSendSysMessage(LANG_CREATURE_ENTRY_LIST_CONSOLE, id, name.c_str());

                if (!found)
                    found = true;
            }
        }

        if (!found)
            handler->SendSysMessage(LANG_COMMAND_NOCREATUREFOUND);

        return true;
    }

    static bool HandleLookupSpellCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // can be NULL at console call
        Player* target = handler->getSelectedPlayer();

        std::string namepart = args;
        std::wstring wnamepart;

        if (!Utf8toWStr(namepart, wnamepart))
            return false;

        // converting string that we try to find to lower case
        wstrToLower(wnamepart);

        bool found = false;
        uint32 count = 0;
        uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

        // Search in Spell.dbc
        for (uint32 id = 0; id < sSpellStore.GetNumRows(); id++)
        {
            SpellEntry const* spellInfo = sSpellStore.LookupEntry(id);
            if (spellInfo)
            {
                int loc = handler->GetSession() ? int(handler->GetSession()->GetSessionDbcLocale()) : sWorld.GetDefaultDbcLocale();
                std::string name = spellInfo->SpellName[loc];
                if (name.empty())
                    continue;

                if (!Utf8FitTo(name, wnamepart))
                {
                    loc = 0;
                    for (; loc < MAX_LOCALE; ++loc)
                    {
                        if (handler->GetSession() && loc == handler->GetSession()->GetSessionDbcLocale())
                            continue;

                        name = spellInfo->SpellName[loc];
                        if (name.empty())
                            continue;

                        if (Utf8FitTo(name, wnamepart))
                            break;
                    }
                }

                if (loc < MAX_LOCALE)
                {
                    if (maxResults && count++ == maxResults)
                    {
                        handler->PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                        return true;
                    }

                    bool known = target && target->HasSpell(id);
                    bool learn = (spellInfo->Effect[0] == SPELL_EFFECT_LEARN_SPELL);

                    uint32 talentCost = GetTalentSpellCost(id);

                    bool talent = (talentCost > 0);
                    bool passive = IsPassiveSpell(id);
                    bool active = target && (target->HasAura(id, 0) || target->HasAura(id, 1) || target->HasAura(id, 2));

                    // unit32 used to prevent interpreting uint8 as char at output
                    // find rank of learned spell for learning spell, or talent rank
                    uint32 rank = talentCost ? talentCost : sSpellMgr.GetSpellRank(learn ? spellInfo->EffectTriggerSpell[0] : id);

                    // send spell in "id - [name, rank N] [talent] [passive] [learn] [known]" format
                    std::ostringstream ss;
                    if (handler->GetSession())
                        ss << id << " - |cffffffff|Hspell:" << id << "|h[" << name;
                    else
                        ss << id << " - " << name;

                    // include rank in link name
                    if (rank)
                        ss << handler->GetOregonString(LANG_SPELL_RANK) << rank;

                    if (handler->GetSession())
                        ss << " " << localeNames[loc] << "]|h|r";
                    else
                        ss << " " << localeNames[loc];

                    if (talent)
                        ss << handler->GetOregonString(LANG_TALENT);
                    if (passive)
                        ss << handler->GetOregonString(LANG_PASSIVE);
                    if (learn)
                        ss << handler->GetOregonString(LANG_LEARN);
                    if (known)
                        ss << handler->GetOregonString(LANG_KNOWN);
                    if (active)
                        ss << handler->GetOregonString(LANG_ACTIVE);

                    handler->SendSysMessage(ss.str().c_str());

                    if (!found)
                        found = true;
                }
            }
        }
        if (!found)
            handler->SendSysMessage(LANG_COMMAND_NOSPELLFOUND);
        return true;
    }

    static bool HandleLookupEventCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string namepart = args;
        std::wstring wnamepart;

        // converting string that we try to find to lower case
        if (!Utf8toWStr(namepart, wnamepart))
            return false;

        wstrToLower(wnamepart);

        bool found = false;
        uint32 count = 0;
        uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

        GameEventMgr::GameEventDataMap const& events = sGameEventMgr.GetEventMap();
        GameEventMgr::ActiveEvents const& activeEvents = sGameEventMgr.GetActiveEventList();

        for (uint32 id = 0; id < events.size(); ++id)
        {
            GameEventData const& eventData = events[id];

            std::string descr = eventData.description;
            if (descr.empty())
                continue;

            if (Utf8FitTo(descr, wnamepart))
            {
                if (maxResults && count++ == maxResults)
                {
                    handler->PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                    return true;
                }

                char const* active = activeEvents.find(id) != activeEvents.end() ? handler->GetOregonString(LANG_ACTIVE) : "";

                if (handler->GetSession())
                    handler->PSendSysMessage(LANG_EVENT_ENTRY_LIST_CHAT, id, id, eventData.description.c_str(), active);
                else
                    handler->PSendSysMessage(LANG_EVENT_ENTRY_LIST_CONSOLE, id, eventData.description.c_str(), active);

                if (!found)
                    found = true;
            }
        }

        if (!found)
            handler->SendSysMessage(LANG_NOEVENTFOUND);

        return true;
    }

    static bool HandleLookupFactionCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // Can be NULL at console call
        Player* target = handler->getSelectedPlayer();

        std::string namepart = args;
        std::wstring wnamepart;

        if (!Utf8toWStr(namepart, wnamepart))
            return false;

        // converting string that we try to find to lower case
        wstrToLower(wnamepart);

        uint32 counter = 0;                                     // Counter for figure out that we found smth.

        for (uint32 id = 0; id < sFactionStore.GetNumRows(); ++id)
        {
            FactionEntry const* factionEntry = sFactionStore.LookupEntry(id);
            if (factionEntry)
            {
                FactionState const* factionState = target ? target->GetReputationMgr().GetState(factionEntry) : NULL;

                int loc = handler->GetSession() ? int(handler->GetSession()->GetSessionDbcLocale()) : sWorld.GetDefaultDbcLocale();
                std::string name = factionEntry->name[loc];
                if (name.empty())
                    continue;

                if (!Utf8FitTo(name, wnamepart))
                {
                    loc = 0;
                    for (; loc < MAX_LOCALE; ++loc)
                    {
                        if (handler->GetSession() && loc == handler->GetSession()->GetSessionDbcLocale())
                            continue;

                        name = factionEntry->name[loc];
                        if (name.empty())
                            continue;

                        if (Utf8FitTo(name, wnamepart))
                            break;
                    }
                }

                if (loc < MAX_LOCALE)
                {
                    // send faction in "id - [faction] rank reputation [visible] [at war] [own team] [unknown] [invisible] [inactive]" format
                    // or              "id - [faction] [no reputation]" format
                    std::ostringstream ss;
                    if (handler->GetSession())
                        ss << id << " - |cffffffff|Hfaction:" << id << "|h[" << name << " " << localeNames[loc] << "]|h|r";
                    else
                        ss << id << " - " << name << " " << localeNames[loc];

                    if (factionState)                               // and then target != NULL also
                    {
                        ReputationRank rank = target->GetReputationMgr().GetRank(factionEntry);
                        std::string rankName = handler->GetOregonString(ReputationRankStrIndex[rank]);

                        ss << " " << rankName << "|h|r (" << target->GetReputationMgr().GetReputation(factionEntry) << ")";

                        if (factionState->Flags & FACTION_FLAG_VISIBLE)
                            ss << handler->GetOregonString(LANG_FACTION_VISIBLE);
                        if (factionState->Flags & FACTION_FLAG_AT_WAR)
                            ss << handler->GetOregonString(LANG_FACTION_ATWAR);
                        if (factionState->Flags & FACTION_FLAG_PEACE_FORCED)
                            ss << handler->GetOregonString(LANG_FACTION_PEACE_FORCED);
                        if (factionState->Flags & FACTION_FLAG_HIDDEN)
                            ss << handler->GetOregonString(LANG_FACTION_HIDDEN);
                        if (factionState->Flags & FACTION_FLAG_INVISIBLE_FORCED)
                            ss << handler->GetOregonString(LANG_FACTION_INVISIBLE_FORCED);
                        if (factionState->Flags & FACTION_FLAG_INACTIVE)
                            ss << handler->GetOregonString(LANG_FACTION_INACTIVE);
                    }
                    else
                        ss << handler->GetOregonString(LANG_FACTION_NOREPUTATION);

                    handler->SendSysMessage(ss.str().c_str());
                    counter++;
                }
            }
        }

        if (counter == 0)                                       // if counter == 0 then we found nth
            handler->SendSysMessage(LANG_COMMAND_FACTION_NOTFOUND);
        return true;
    }

    static bool HandleLookupItemCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string namepart = args;
        std::wstring wnamepart;

        // converting string that we try to find to lower case
        if (!Utf8toWStr(namepart, wnamepart))
            return false;

        wstrToLower(wnamepart);

        bool found = false;
        uint32 count = 0;
        uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

        // Search in item_template
        for (uint32 id = 0; id < sItemStorage.MaxEntry; id++)
        {
            ItemTemplate const* pProto = sItemStorage.LookupEntry<ItemTemplate >(id);
            if (!pProto)
                continue;

            int loc_idx = handler->GetSession() ? handler->GetSession()->GetSessionDbLocaleIndex() : sObjectMgr.GetDBCLocaleIndex();
            if (loc_idx >= 0)
            {
                ItemLocale const* il = sObjectMgr.GetItemLocale(pProto->ItemId);
                if (il)
                {
                    if (il->Name.size() > uint32(loc_idx) && !il->Name[loc_idx].empty())
                    {
                        std::string name = il->Name[loc_idx];

                        if (Utf8FitTo(name, wnamepart))
                        {
                            if (maxResults && count++ == maxResults)
                            {
                                handler->PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                                return true;
                            }

                            if (handler->GetSession())
                                handler->PSendSysMessage(LANG_ITEM_LIST_CHAT, id, id, name.c_str());
                            else
                                handler->PSendSysMessage(LANG_ITEM_LIST_CONSOLE, id, name.c_str());

                            if (!found)
                                found = true;

                            continue;
                        }
                    }
                }
            }

            std::string name = pProto->Name1;
            if (name.empty())
                continue;

            if (Utf8FitTo(name, wnamepart))
            {
                if (maxResults && count++ == maxResults)
                {
                    handler->PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                    return true;
                }

                if (handler->GetSession())
                    handler->PSendSysMessage(LANG_ITEM_LIST_CHAT, id, id, name.c_str());
                else
                    handler->PSendSysMessage(LANG_ITEM_LIST_CONSOLE, id, name.c_str());

                if (!found)
                    found = true;
            }
        }

        if (!found)
            handler->SendSysMessage(LANG_COMMAND_NOITEMFOUND);

        return true;
    }

    static bool HandleLookupItemSetCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string namepart = args;
        std::wstring wnamepart;

        if (!Utf8toWStr(namepart, wnamepart))
            return false;

        // converting string that we try to find to lower case
        wstrToLower(wnamepart);

        bool found = false;
        uint32 count = 0;
        uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

        // Search in ItemSet.dbc
        for (uint32 id = 0; id < sItemSetStore.GetNumRows(); id++)
        {
            ItemSetEntry const* set = sItemSetStore.LookupEntry(id);
            if (set)
            {
                int loc = handler->GetSession() ? int(handler->GetSession()->GetSessionDbcLocale()) : sWorld.GetDefaultDbcLocale();
                std::string name = set->name[loc];
                if (name.empty())
                    continue;

                if (!Utf8FitTo(name, wnamepart))
                {
                    loc = 0;
                    for (; loc < MAX_LOCALE; ++loc)
                    {
                        if (handler->GetSession() && loc == handler->GetSession()->GetSessionDbcLocale())
                            continue;

                        name = set->name[loc];
                        if (name.empty())
                            continue;

                        if (Utf8FitTo(name, wnamepart))
                            break;
                    }
                }

                if (loc < MAX_LOCALE)
                {
                    if (maxResults && count++ == maxResults)
                    {
                        handler->PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                        return true;
                    }

                    // send item set in "id - [namedlink locale]" format
                    if (handler->GetSession())
                        handler->PSendSysMessage(LANG_ITEMSET_LIST_CHAT, id, id, name.c_str(), localeNames[loc]);
                    else
                        handler->PSendSysMessage(LANG_ITEMSET_LIST_CONSOLE, id, name.c_str(), localeNames[loc]);

                    if (!found)
                        found = true;
                }
            }
        }
        if (!found)
            handler->SendSysMessage(LANG_COMMAND_NOITEMSETFOUND);
        return true;
    }

    static bool HandleLookupObjectCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        std::string namepart = args;
        std::wstring wnamepart;

        // converting string that we try to find to lower case
        if (!Utf8toWStr(namepart, wnamepart))
            return false;

        wstrToLower(wnamepart);

        bool found = false;
        uint32 count = 0;
        uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

        for (uint32 id = 0; id < sGOStorage.MaxEntry; id++)
        {
            GameObjectInfo const* gInfo = sGOStorage.LookupEntry<GameObjectInfo>(id);
            if (!gInfo)
                continue;

            int loc_idx = handler->GetSession() ? handler->GetSession()->GetSessionDbLocaleIndex() : sObjectMgr.GetDBCLocaleIndex();
            if (loc_idx >= 0)
            {
                GameObjectLocale const* gl = sObjectMgr.GetGameObjectLocale(id);
                if (gl)
                {
                    if (gl->Name.size() > uint32(loc_idx) && !gl->Name[loc_idx].empty())
                    {
                        std::string name = gl->Name[loc_idx];

                        if (Utf8FitTo(name, wnamepart))
                        {
                            if (maxResults && count++ == maxResults)
                            {
                                handler->PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                                return true;
                            }

                            if (handler->GetSession())
                                handler->PSendSysMessage(LANG_GO_ENTRY_LIST_CHAT, id, id, name.c_str());
                            else
                                handler->PSendSysMessage(LANG_GO_ENTRY_LIST_CONSOLE, id, name.c_str());

                            if (!found)
                                found = true;

                            continue;
                        }
                    }
                }
            }

            std::string name = gInfo->name;
            if (name.empty())
                continue;

            if (Utf8FitTo(name, wnamepart))
            {
                if (maxResults && count++ == maxResults)
                {
                    handler->PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                    return true;
                }

                if (handler->GetSession())
                    handler->PSendSysMessage(LANG_GO_ENTRY_LIST_CHAT, id, id, name.c_str());
                else
                    handler->PSendSysMessage(LANG_GO_ENTRY_LIST_CONSOLE, id, name.c_str());

                if (!found)
                    found = true;
            }
        }

        if (!found)
            handler->SendSysMessage(LANG_COMMAND_NOGAMEOBJECTFOUND);

        return true;
    }

    static bool HandleLookupQuestCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // can be NULL at console call
        Player* target = handler->getSelectedPlayer();

        std::string namepart = args;
        std::wstring wnamepart;

        // converting string that we try to find to lower case
        if (!Utf8toWStr(namepart, wnamepart))
            return false;

        wstrToLower(wnamepart);

        bool found = false;
        uint32 count = 0;
        uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

        ObjectMgr::QuestMap const& qTemplates = sObjectMgr.GetQuestTemplates();
        for (ObjectMgr::QuestMap::const_iterator iter = qTemplates.begin(); iter != qTemplates.end(); ++iter)
        {
            Quest* qinfo = iter->second;

            int loc_idx = handler->GetSession() ? handler->GetSession()->GetSessionDbLocaleIndex() : sObjectMgr.GetDBCLocaleIndex();
            if (loc_idx >= 0)
            {
                QuestLocale const* il = sObjectMgr.GetQuestLocale(qinfo->GetQuestId());
                if (il)
                {
                    if (il->Title.size() > uint32(loc_idx) && !il->Title[loc_idx].empty())
                    {
                        std::string title = il->Title[loc_idx];

                        if (Utf8FitTo(title, wnamepart))
                        {
                            if (maxResults && count++ == maxResults)
                            {
                                handler->PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                                return true;
                            }

                            char const* statusStr = "";

                            if (target)
                            {
                                QuestStatus status = target->GetQuestStatus(qinfo->GetQuestId());

                                if (status == QUEST_STATUS_COMPLETE)
                                {
                                    if (target->GetQuestRewardStatus(qinfo->GetQuestId()))
                                        statusStr = handler->GetOregonString(LANG_COMMAND_QUEST_REWARDED);
                                    else
                                        statusStr = handler->GetOregonString(LANG_COMMAND_QUEST_COMPLETE);
                                }
                                else if (status == QUEST_STATUS_INCOMPLETE)
                                    statusStr = handler->GetOregonString(LANG_COMMAND_QUEST_ACTIVE);
                            }

                            if (handler->GetSession())
                                handler->PSendSysMessage(LANG_QUEST_LIST_CHAT, qinfo->GetQuestId(), qinfo->GetQuestId(), title.c_str(), statusStr);
                            else
                                handler->PSendSysMessage(LANG_QUEST_LIST_CONSOLE, qinfo->GetQuestId(), title.c_str(), statusStr);

                            if (!found)
                                found = true;

                            continue;
                        }
                    }
                }
            }

            std::string title = qinfo->GetTitle();
            if (title.empty())
                continue;

            if (Utf8FitTo(title, wnamepart))
            {
                if (maxResults && count++ == maxResults)
                {
                    handler->PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                    return true;
                }

                char const* statusStr = "";

                if (target)
                {
                    QuestStatus status = target->GetQuestStatus(qinfo->GetQuestId());

                    if (status == QUEST_STATUS_COMPLETE)
                    {
                        if (target->GetQuestRewardStatus(qinfo->GetQuestId()))
                            statusStr = handler->GetOregonString(LANG_COMMAND_QUEST_REWARDED);
                        else
                            statusStr = handler->GetOregonString(LANG_COMMAND_QUEST_COMPLETE);
                    }
                    else if (status == QUEST_STATUS_INCOMPLETE)
                        statusStr = handler->GetOregonString(LANG_COMMAND_QUEST_ACTIVE);
                }

                if (handler->GetSession())
                    handler->PSendSysMessage(LANG_QUEST_LIST_CHAT, qinfo->GetQuestId(), qinfo->GetQuestId(), title.c_str(), statusStr);
                else
                    handler->PSendSysMessage(LANG_QUEST_LIST_CONSOLE, qinfo->GetQuestId(), title.c_str(), statusStr);

                if (!found)
                    found = true;
            }
        }

        if (!found)
            handler->SendSysMessage(LANG_COMMAND_NOQUESTFOUND);

        return true;
    }

    static bool HandleLookupSkillCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // can be NULL in console call
        Player* target = handler->getSelectedPlayerOrSelf();

        std::string namepart = args;
        std::wstring wnamepart;

        if (!Utf8toWStr(namepart, wnamepart))
            return false;

        // converting string that we try to find to lower case
        wstrToLower(wnamepart);

        bool found = false;
        uint32 count = 0;
        uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

        // Search in SkillLine.dbc
        for (uint32 id = 0; id < sSkillLineStore.GetNumRows(); id++)
        {
            SkillLineEntry const* skillInfo = sSkillLineStore.LookupEntry(id);
            if (skillInfo)
            {
                int loc = handler->GetSession() ? int(handler->GetSession()->GetSessionDbcLocale()) : sWorld.GetDefaultDbcLocale();
                std::string name = skillInfo->name[loc];
                if (name.empty())
                    continue;

                if (!Utf8FitTo(name, wnamepart))
                {
                    loc = 0;
                    for (; loc < MAX_LOCALE; ++loc)
                    {
                        if (handler->GetSession() && loc == handler->GetSession()->GetSessionDbcLocale())
                            continue;

                        name = skillInfo->name[loc];
                        if (name.empty())
                            continue;

                        if (Utf8FitTo(name, wnamepart))
                            break;
                    }
                }

                if (loc < MAX_LOCALE)
                {
                    if (maxResults && count++ == maxResults)
                    {
                        handler->PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                        return true;
                    }

                    char const* knownStr = "";
                    if (target && target->HasSkill(id))
                        knownStr = handler->GetOregonString(LANG_KNOWN);

                    // send skill in "id - [namedlink locale]" format
                    if (handler->GetSession())
                        handler->PSendSysMessage(LANG_SKILL_LIST_CHAT, id, id, name.c_str(), localeNames[loc], knownStr);
                    else
                        handler->PSendSysMessage(LANG_SKILL_LIST_CONSOLE, id, name.c_str(), localeNames[loc], knownStr);

                    if (!found)
                        found = true;
                }
            }
        }
        if (!found)
            handler->SendSysMessage(LANG_COMMAND_NOSKILLFOUND);
        return true;
    }

    //Find tele in game_tele order by name
    static bool HandleLookupTeleCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
        {
            handler->SendSysMessage(LANG_COMMAND_TELE_PARAMETER);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char const* str = strtok((char*)args, " ");
        if (!str)
            return false;

        std::string namepart = str;
        std::wstring wnamepart;

        if (!Utf8toWStr(namepart, wnamepart))
            return false;

        // converting string that we try to find to lower case
        wstrToLower(wnamepart);

        std::ostringstream reply;
        uint32 count = 0;
        uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);
        bool limitReached = false;

        GameTeleMap const& teleMap = sObjectMgr.GetGameTeleMap();
        for (GameTeleMap::const_iterator itr = teleMap.begin(); itr != teleMap.end(); ++itr)
        {
            GameTele const* tele = &itr->second;

            if (tele->wnameLow.find(wnamepart) == std::wstring::npos)
                continue;

            if (maxResults && count++ == maxResults)
            {
                limitReached = true;
                break;
            }

            if (handler->GetSession())
                reply << "  |cffffffff|Htele:" << itr->first << "|h[" << tele->name << "]|h|r\n";
            else
                reply << "  " << itr->first << " " << tele->name << "\n";
        }

        if (reply.str().empty())
            handler->SendSysMessage(LANG_COMMAND_TELE_NOLOCATION);
        else
            handler->PSendSysMessage(LANG_COMMAND_TELE_LOCATION, reply.str().c_str());

        if (limitReached)
            handler->PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);

        return true;
    }

    static bool HandleLookupTitleCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // can be NULL in console call
        Player* target = handler->getSelectedPlayer();

        // title name have single string arg for player name
        char const* targetName = target ? target->GetName() : "NAME";

        std::string namepart = args;
        std::wstring wnamepart;

        if (!Utf8toWStr(namepart, wnamepart))
            return false;

        // converting string that we try to find to lower case
        wstrToLower(wnamepart);

        uint32 counter = 0;                                     // Counter for figure out that we found smth.
        uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

        // Search in CharTitles.dbc
        for (uint32 id = 0; id < sCharTitlesStore.GetNumRows(); id++)
        {
            CharTitlesEntry const* titleInfo = sCharTitlesStore.LookupEntry(id);
            if (titleInfo)
            {
                int loc = handler->GetSession()->GetSessionDbcLocale();
                std::string name = titleInfo->name[loc];
                if (name.empty())
                    continue;

                if (!Utf8FitTo(name, wnamepart))
                {
                    loc = 0;
                    for (; loc < MAX_LOCALE; ++loc)
                    {
                        if (loc == handler->GetSession()->GetSessionDbcLocale())
                            continue;

                        name = titleInfo->name[loc];
                        if (name.empty())
                            continue;

                        if (Utf8FitTo(name, wnamepart))
                            break;
                    }
                }

                if (loc < MAX_LOCALE)
                {
                    if (maxResults && counter == maxResults)
                    {
                        handler->PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                        return true;
                    }

                    char const* knownStr = target && target->HasTitle(titleInfo) ? handler->GetOregonString(LANG_KNOWN) : "";

                    char const* activeStr = target && target->GetUInt32Value(PLAYER_CHOSEN_TITLE) == titleInfo->bit_index
                        ? handler->GetOregonString(LANG_ACTIVE)
                        : "";

                    char titleNameStr[80];
                    snprintf(titleNameStr, 80, name.c_str(), targetName);

                    // send title in "id (idx:idx) - [namedlink locale]" format
                    if (handler->GetSession())
                        handler->PSendSysMessage(LANG_TITLE_LIST_CHAT, id, titleInfo->bit_index, id, titleNameStr, localeNames[loc], knownStr, activeStr);
                    else
                        handler->PSendSysMessage(LANG_TITLE_LIST_CONSOLE, id, titleInfo->bit_index, titleNameStr, localeNames[loc], knownStr, activeStr);

                    ++counter;
                }
            }
        }
        if (counter == 0)                                       // if counter == 0 then we found nth
            handler->SendSysMessage(LANG_COMMAND_NOTITLEFOUND);
        return true;
    }
};

void AddSC_lookup_commandscript()
{
    new lookup_commandscript();
}