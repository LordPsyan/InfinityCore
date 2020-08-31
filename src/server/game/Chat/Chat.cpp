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

#include "Common.h"
#include "ObjectMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Database/DatabaseEnv.h"

#include "CellImpl.h"
#include "Chat.h"
#include "GridNotifiersImpl.h"
#include "Language.h"
#include "Log.h"
#include "Opcodes.h"
#include "Player.h"
#include "UpdateMask.h"
#include "MapManager.h"
#include "SpellMgr.h"
#include "ScriptMgr.h"

#ifdef ELUNA
#include "LuaEngine.h"
#endif

#include "InstanceSaveMgr.h"

bool ChatHandler::load_command_table = true;

std::vector<ChatCommand> const& ChatHandler::getCommandTable()
{
    static std::vector<ChatCommand> commandTableCache;

    if (LoadCommandTable())
    {
        SetLoadCommandTable(false);

        std::vector<ChatCommand> cmds = sScriptMgr.GetChatCommands();
        commandTableCache.swap(cmds);

        QueryResult_AutoPtr result = WorldDatabase.Query("SELECT name,security,help FROM command");
        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                std::string name = fields[0].GetCppString();

                SetDataForCommandInTable(commandTableCache, name.c_str(), fields[1].GetUInt16(), fields[2].GetString(), name);
            } while (result->NextRow());
        }
    }

    return commandTableCache;
}

std::string ChatHandler::GetTimeString(uint32 time)
{
    uint16 days = time / DAY, hours = (time % DAY) / HOUR, minute = (time % HOUR) / MINUTE;
    std::ostringstream ss;
    if (days) ss << days << "d ";
    if (hours) ss << hours << "h ";
    ss << minute << "m";
    return ss.str();
}

std::string ChatHandler::GetNameLink(Player* chr) const
{
    return playerLink(chr->GetName());
}

const char* ChatHandler::GetOregonString(int32 entry) const
{
    return m_session->GetOregonString(entry);
}

bool ChatHandler::isAvailable(ChatCommand const& cmd) const
{
    // check security level only for simple  command (without child commands)
    return m_session->GetSecurity() >= cmd.SecurityLevel;
}

bool ChatHandler::hasStringAbbr(const char* name, const char* part)
{
    // non "" command
    if (*name)
    {
        // "" part from non-"" command
        if (!*part)
            return false;

        for (;;)
        {
            if (!*part)
                return true;
            else if (!*name)
                return false;
            else if (tolower(*name) != tolower(*part))
                return false;
            ++name;
            ++part;
        }
    }
    // allow with any for ""

    return true;
}

void ChatHandler::SendSysMessage(const char* str)
{
    WorldPacket data;

    // need copy to prevent corruption by strtok call in LineFromMessage original string
    char* buf = strdup(str);
    char* pos = buf;

    while (char* line = LineFromMessage(pos))
    {
        FillSystemMessageData(&data, line);
        m_session->SendPacket(&data);
    }

    free(buf);
}

void ChatHandler::SendGlobalSysMessage(const char* str)
{
    // Chat output
    WorldPacket data;

    // need copy to prevent corruption by strtok call in LineFromMessage original string
    char* buf = strdup(str);
    char* pos = buf;

    while (char* line = LineFromMessage(pos))
    {
        FillSystemMessageData(&data, line);
        sWorld.SendGlobalMessage(&data);
    }

    free(buf);
}

void ChatHandler::SendGlobalGMSysMessage(const char* str)
{
    // Chat output
    WorldPacket data;

    // need copy to prevent corruption by strtok call in LineFromMessage original string
    char* buf = strdup(str);
    char* pos = buf;

    while (char* line = LineFromMessage(pos))
    {
        FillSystemMessageData(&data, line);
        sWorld.SendGlobalGMMessage(&data);
    }
    free(buf);
}

void ChatHandler::SendSysMessage(int32 entry)
{
    SendSysMessage(GetOregonString(entry));
}

void ChatHandler::PSendSysMessage(int32 entry, ...)
{
    const char* format = GetOregonString(entry);
    va_list ap;
    char str[1024];
    va_start(ap, entry);
    vsnprintf(str, 1024, format, ap);
    va_end(ap);
    SendSysMessage(str);
}

void ChatHandler::PSendSysMessage(const char* format, ...)
{
    va_list ap;
    char str[1024];
    va_start(ap, format);
    vsnprintf(str, 1024, format, ap);
    va_end(ap);
    SendSysMessage(str);
}

bool ChatHandler::ExecuteCommandInTable(std::vector<ChatCommand> const& table, const char* text, std::string const& fullcmd)
{
    char const* oldtext = text;
    std::string cmd = "";

    while (*text != ' ' && *text != '\0')
    {
        cmd += *text;
        ++text;
    }

    while (*text == ' ') ++text;

    for (uint32 i = 0; i < table.size(); ++i)
    {
        if (!hasStringAbbr(table[i].Name, cmd.c_str()))
            continue;

        bool match = false;
        if (strlen(table[i].Name) > cmd.length())
        {
            for (uint32 j = 0; j < table.size() ; ++j)
            {
                if (!hasStringAbbr(table[j].Name, cmd.c_str()))
                    continue;

                if (strcmp(table[j].Name, cmd.c_str()) == 0)
                {
                    match = true;
                    break;
                }
            }
        }
        if (match)
            continue;

        // select subcommand from child commands list
        if (!table[i].ChildCommands.empty())
        {
            if (!ExecuteCommandInTable(table[i].ChildCommands, text, fullcmd.c_str()))
            {
                if (text && text[0] != '\0')
                    SendSysMessage(LANG_NO_SUBCMD);
                //else
                    //SendSysMessage(LANG_CMD_SYNTAX);

                ShowHelpForCommand(table[i].ChildCommands, text);
            }

            return true;
        }

#ifdef ELUNA
        if (!sEluna->OnCommand(m_session ? m_session->GetPlayer() : NULL, fullcmd.c_str()))
            return true;
#endif

        // must be available and have handler
        if (!table[i].Handler || !isAvailable(table[i]))
            continue;

        SetSentErrorMessage(false);
        // table[i].Name == "" is special case: send original command to handler
        if ((table[i].Handler)(this, table[i].Name[0] != '\0' ? text : oldtext))
        {
            if (table[i].SecurityLevel > SEC_PLAYER)
            {
                if (!m_session)
                    return true;

                Player* p = m_session->GetPlayer();
                ObjectGuid sel_guid = p->GetSelection();
                sLog.outCommand(m_session->GetAccountId(), "Command: %s [Player: %s (Account: %u) X: %f Y: %f Z: %f Map: %u Selected: %s]",
                    fullcmd.c_str(), p->GetName(), m_session->GetAccountId(), p->GetPositionX(), p->GetPositionY(), p->GetPositionZ(), p->GetMapId(),
                    sel_guid.GetString().c_str());
            }
        }
        // some commands have custom error messages. Don't send the default one in these cases.
        else if (!sentErrorMessage)
        {
            if (!table[i].Help.empty())
                SendSysMessage(table[i].Help.c_str());
            else
                SendSysMessage(LANG_CMD_SYNTAX);
        }

        return true;
    }

    return false;
}

bool ChatHandler::SetDataForCommandInTable(std::vector<ChatCommand>& table, char const* text, uint32 security, std::string const& help, std::string const& fullcommand)
{
    std::string cmd = "";

    while (*text != ' ' && *text != '\0')
    {
        cmd += *text;
        ++text;
    }

    while (*text == ' ') ++text;

    for (uint32 i = 0; i < table.size(); i++)
    {
        // for data fill use full explicit command names
        if (table[i].Name != cmd.c_str())
            continue;

        // select subcommand from child commands list (including "")
        if (!table[i].ChildCommands.empty())
        {
            if (SetDataForCommandInTable(table[i].ChildCommands, text, security, help, fullcommand))
                return true;
            else if (*text)
                return false;

            // fail with "" subcommands, then use normal level up command instead
        }
        // expected subcommand by full name DB content
        else if (*text)
        {
            sLog.outError("Table `command` have unexpected subcommand '%s' in command '%s', skip.", text, fullcommand.c_str());
            return false;
        }

        //if (table[i].SecurityLevel != security)
        //    sLog->outDetail("Table `command` overwrite for command '%s' default security (%u) by %u", fullcommand.c_str(), table[i].SecurityLevel, security);

        table[i].SecurityLevel = security;
        table[i].Help = help;
        return true;
    }

    // in case "" command let process by caller
    if (!cmd.empty())
    {
        if (&table == &getCommandTable())
            sLog.outError("Table `command` have non-existing command '%s', skip.", cmd.c_str());
        else
            sLog.outError("Table `command` have non-existing subcommand '%s' in command '%s', skip.", cmd.c_str(), fullcommand.c_str());
    }

    return false;
}

int ChatHandler::ParseCommands(const char* text)
{
    ASSERT(text);
    ASSERT(*text);

    std::string fullcmd = text;

    // chat case (.command or !command format)
    if (m_session)
    {
        if (text[0] != '!' && text[0] != '.')
            return 0;
    }

    // ignore single . and ! in line
    if (strlen(text) < 2)
        return 0;
    // original `text` can't be used. It content destroyed in command code processing.

    // ignore messages staring from many dots.
    if ((text[0] == '.' && text[1] == '.') || (text[0] == '!' && text[1] == '!'))
        return 0;

    // skip first . or ! (in console allowed use command with . and ! and without its)
    if (text[0] == '!' || text[0] == '.')
        ++text;

    if (!ExecuteCommandInTable(getCommandTable(), text, fullcmd.c_str()))
    {

        if (m_session && m_session->GetSecurity() == SEC_PLAYER)
            return 0;

        SendSysMessage(LANG_NO_CMD);
    }
    return 1;
}

bool ChatHandler::isValidChatMessage(const char* message)
{
    /*

    valid examples:
    |cffa335ee|Hitem:812:0:0:0:0:0:0:0:70|h[Glowing Brightwood Staff]|h|r
    |cff808080|Hquest:2278:47|h[The Platinum Discs]|h|r
    |cff4e96f7|Htalent:2232:-1|h[Taste for Blood]|h|r
    |cff71d5ff|Hspell:21563|h[Command]|h|r
    |cffffd000|Henchant:3919|h[Engineering: Rough Dynamite]|h|r

    | will be escaped to ||
    */

    if (strlen(message) > 255)
        return false;

    const char validSequence[6] = "cHhhr";
    const char* validSequenceIterator = validSequence;

    // more simple checks
    if (sWorld.getConfig(CONFIG_CHAT_STRICT_LINK_CHECKING_SEVERITY) < 3)
    {
        const std::string validCommands = "cHhr|";

        while (*message)
        {
            // find next pipe command
            message = strchr(message, '|');

            if (!message)
                return true;

            ++message;
            char commandChar = *message;
            if (validCommands.find(commandChar) == std::string::npos)
                return false;

            ++message;
            // validate sequence
            if (sWorld.getConfig(CONFIG_CHAT_STRICT_LINK_CHECKING_SEVERITY) == 2)
            {
                if (commandChar == *validSequenceIterator)
                {
                    if (validSequenceIterator == validSequence + 4)
                        validSequenceIterator = validSequence;
                    else
                        ++validSequenceIterator;
                }
                else
                    return false;
            }
        }
        return true;
    }

    std::istringstream reader(message);
    char buffer[256];

    uint32 color = 0;

    ItemTemplate const* linkedItem;
    Quest const* linkedQuest;
    SpellEntry const* linkedSpell = NULL;

    while (!reader.eof())
    {
        if (validSequence == validSequenceIterator)
        {
            linkedItem = NULL;
            linkedQuest = NULL;
            linkedSpell = NULL;

            reader.ignore(255, '|');
        }
        else if (reader.get() != '|')
        {
#ifdef OREGON_DEBUG
            sLog.outBasic("ChatHandler::isValidChatMessage sequence aborted unexpectedly");
#endif
            return false;
        }

        // pipe has always to be followed by at least one char
        if (reader.peek() == '\0')
        {
#ifdef OREGON_DEBUG
            sLog.outBasic("ChatHandler::isValidChatMessage pipe followed by \\0");
#endif
            return false;
        }

        // no further pipe commands
        if (reader.eof())
            break;

        char commandChar;
        reader >> commandChar;

        // | in normal messages is escaped by ||
        if (commandChar != '|')
        {
            if (commandChar == *validSequenceIterator)
            {
                if (validSequenceIterator == validSequence + 4)
                    validSequenceIterator = validSequence;
                else
                    ++validSequenceIterator;
            }
            else
            {
#ifdef OREGON_DEBUG
                sLog.outBasic("ChatHandler::isValidChatMessage invalid sequence, expected %c but got %c", *validSequenceIterator, commandChar);
#endif
                return false;
            }
        }
        else if (validSequence != validSequenceIterator)
        {
            // no escaped pipes in sequences
#ifdef OREGON_DEBUG
            sLog.outBasic("ChatHandler::isValidChatMessage got escaped pipe in sequence");
#endif
            return false;
        }

        switch (commandChar)
        {
        case 'c':
            color = 0;
            // validate color, expect 8 hex chars
            for (int i = 0; i < 8; i++)
            {
                char c;
                reader >> c;
                if (!c)
                {
#ifdef OREGON_DEBUG
                    sLog.outBasic("ChatHandler::isValidChatMessage got \\0 while reading color in |c command");
#endif
                    return false;
                }

                color <<= 4;
                // check for hex char
                if (c >= '0' && c <= '9')
                {
                    color |= c - '0';
                    continue;
                }
                if (c >= 'a' && c <= 'f')
                {
                    color |= 10 + c - 'a';
                    continue;
                }
#ifdef OREGON_DEBUG
                sLog.outBasic("ChatHandler::isValidChatMessage got non hex char '%c' while reading color", c);
#endif
                return false;
            }
            break;
        case 'H':
            // read chars up to colon  = link type
            reader.getline(buffer, 256, ':');

            if (strcmp(buffer, "item") == 0)
            {
                // read item entry
                reader.getline(buffer, 256, ':');

                linkedItem = sObjectMgr.GetItemTemplate(atoi(buffer));
                if (!linkedItem)
                {
#ifdef OREGON_DEBUG
                    sLog.outBasic("ChatHandler::isValidChatMessage got invalid itemID %u in |item command", atoi(buffer));
#endif
                    return false;
                }

                if (color != ItemQualityColors[linkedItem->Quality])
                {
#ifdef OREGON_DEBUG
                    sLog.outBasic("ChatHandler::isValidChatMessage linked item has color %u, but user claims %u", ItemQualityColors[linkedItem->Quality],
                        color);
#endif
                    return false;
                }

                char c = reader.peek();

                // ignore enchants etc.
                while ((c >= '0' && c <= '9') || c == ':')
                {
                    reader.ignore(1);
                    c = reader.peek();
                }
            }
            else if (strcmp(buffer, "quest") == 0)
            {
                // no color check for questlinks, each client will adapt it anyway
                uint32 questid = 0;
                // read questid
                char c = reader.peek();
                while (c >= '0' && c <= '9')
                {
                    reader.ignore(1);
                    questid *= 10;
                    questid += c - '0';
                    c = reader.peek();
                }

                linkedQuest = sObjectMgr.GetQuestTemplate(questid);

                if (!linkedQuest)
                {
#ifdef OREGON_DEBUG
                    sLog.outBasic("ChatHandler::isValidChatMessage Questtemplate %u not found", questid);
#endif
                    return false;
                }
                c = reader.peek();
                // level
                while (c != '|' && c != '\0')
                {
                    reader.ignore(1);
                    c = reader.peek();
                }
            }
            else if (strcmp(buffer, "talent") == 0)
            {
                // talent links are always supposed to be blue
                if (color != CHAT_LINK_COLOR_TALENT)
                    return false;

                // read talent entry
                reader.getline(buffer, 256, ':');
                TalentEntry const* talentInfo = sTalentStore.LookupEntry(atoi(buffer));
                if (!talentInfo)
                    return false;

                linkedSpell = sSpellStore.LookupEntry(talentInfo->RankID[0]);
                if (!linkedSpell)
                    return false;

                char c = reader.peek();
                // skillpoints? whatever, drop it
                while (c != '|' && c != '\0')
                {
                    reader.ignore(1);
                    c = reader.peek();
                }
            }
            else if (strcmp(buffer, "spell") == 0)
            {
                if (color != CHAT_LINK_COLOR_SPELL)
                    return false;

                uint32 spellid = 0;
                // read spell entry
                char c = reader.peek();
                while (c >= '0' && c <= '9')
                {
                    reader.ignore(1);
                    spellid *= 10;
                    spellid += c - '0';
                    c = reader.peek();
                }
                linkedSpell = sSpellStore.LookupEntry(spellid);
                if (!linkedSpell)
                    return false;
            }
            else if (strcmp(buffer, "enchant") == 0)
            {
                if (color != CHAT_LINK_COLOR_ENCHANT)
                    return false;

                uint32 spellid = 0;
                // read spell entry
                char c = reader.peek();
                while (c >= '0' && c <= '9')
                {
                    reader.ignore(1);
                    spellid *= 10;
                    spellid += c - '0';
                    c = reader.peek();
                }
                linkedSpell = sSpellStore.LookupEntry(spellid);
                if (!linkedSpell)
                    return false;
            }
            else
            {
#ifdef OREGON_DEBUG
                sLog.outBasic("ChatHandler::isValidChatMessage user sent unsupported link type '%s'", buffer);
#endif
                return false;
            }
            break;
        case 'h':
            // if h is next element in sequence, this one must contain the linked text :)
            if (*validSequenceIterator == 'h')
            {
                // links start with '['
                if (reader.get() != '[')
                {
#ifdef OREGON_DEBUG
                    sLog.outBasic("ChatHandler::isValidChatMessage link caption doesn't start with '['");
#endif
                    return false;
                }
                reader.getline(buffer, 256, ']');

                // verify the link name
                if (linkedSpell)
                {
                    // spells with that flag have a prefix of "$PROFESSION: "
                    if (linkedSpell->Attributes & SPELL_ATTR0_TRADESPELL)
                    {
                        // lookup skillid
                        SkillLineAbilityMap::const_iterator lower = sSpellMgr.GetBeginSkillLineAbilityMap(linkedSpell->Id);
                        SkillLineAbilityMap::const_iterator upper = sSpellMgr.GetEndSkillLineAbilityMap(linkedSpell->Id);

                        if (lower == upper)
                            return false;

                        SkillLineAbilityEntry const* skillInfo = lower->second;

                        if (!skillInfo)
                            return false;

                        SkillLineEntry const* skillLine = sSkillLineStore.LookupEntry(skillInfo->skillId);
                        if (!skillLine)
                            return false;

                        for (uint8 i = 0; i < MAX_LOCALE; ++i)
                        {
                            uint32 skillLineNameLength = strlen(skillLine->name[i]);
                            if (skillLineNameLength > 0 && strncmp(skillLine->name[i], buffer, skillLineNameLength) == 0)
                            {
                                // found the prefix, remove it to perform spellname validation below
                                // -2 = strlen(": ")
                                uint32 spellNameLength = strlen(buffer) - skillLineNameLength - 2;
                                memmove(buffer, buffer + skillLineNameLength + 2, spellNameLength + 1);
                            }
                        }
                    }
                    bool foundName = false;
                    for (uint8 i = 0; i < MAX_LOCALE; ++i)
                    {
                        if (*linkedSpell->SpellName[i] && strcmp(linkedSpell->SpellName[i], buffer) == 0)
                        {
                            foundName = true;
                            break;
                        }
                    }
                    if (!foundName)
                        return false;
                }
                else if (linkedQuest)
                {
                    if (linkedQuest->GetTitle() != buffer)
                    {
                        QuestLocale const* ql = sObjectMgr.GetQuestLocale(linkedQuest->GetQuestId());

                        if (!ql)
                        {
#ifdef OREGON_DEBUG
                            sLog.outBasic("ChatHandler::isValidChatMessage default questname didn't match and there is no locale");
#endif
                            return false;
                        }

                        bool foundName = false;
                        for (uint8 i = 0; i < ql->Title.size(); i++)
                        {
                            if (ql->Title[i] == buffer)
                            {
                                foundName = true;
                                break;
                            }
                        }
                        if (!foundName)
                        {
#ifdef OREGON_DEBUG
                            sLog.outBasic("ChatHandler::isValidChatMessage no quest locale title matched");
#endif
                            return false;
                        }
                    }
                }
                else if (linkedItem)
                {
                    if (strcmp(linkedItem->Name1, buffer) != 0)
                    {
                        ItemLocale const* il = sObjectMgr.GetItemLocale(linkedItem->ItemId);

                        if (!il)
                        {
#ifdef OREGON_DEBUG
                            sLog.outBasic("ChatHandler::isValidChatMessage linked item name doesn't is wrong and there is no localization");
#endif
                            return false;
                        }

                        bool foundName = false;
                        for (uint8 i = 0; i < il->Name.size(); ++i)
                        {
                            if (il->Name[i] == buffer)
                            {
                                foundName = true;
                                break;
                            }
                        }
                        if (!foundName)
                        {
#ifdef OREGON_DEBUG
                            sLog.outBasic("ChatHandler::isValidChatMessage linked item name wasn't found in any localization");
#endif
                            return false;
                        }
                    }
                }
                // that place should never be reached - if nothing linked has been set in |H
                // it will return false before
                else
                    return false;
            }
            break;
        case 'r':
        case '|':
            // no further payload
            break;
        default:
#ifdef OREGON_DEBUG
            sLog.outBasic("ChatHandler::isValidChatMessage got invalid command |%c", commandChar);
#endif
            return false;
        }
    }

    // check if every opened sequence was also closed properly
#ifdef OREGON_DEBUG
    if (validSequence != validSequenceIterator)
        sLog.outBasic("ChatHandler::isValidChatMessage EOF in active sequence");
#endif
    return validSequence == validSequenceIterator;
}

bool ChatHandler::ShowHelpForSubCommands(std::vector<ChatCommand> const& table, char const* cmd, char const* subcmd)
{
    std::string list;
    for (uint32 i = 0; i < table.size(); ++i)
    {
        // must be available (ignore handler existence for show command with possible available subcommands)
        if (!isAvailable(table[i]))
            continue;

        // for empty subcmd show all available
        if (*subcmd && !hasStringAbbr(table[i].Name, subcmd))
            continue;

        if (m_session)
            list += "\n    ";
        else
            list += "\n\r    ";

        list += table[i].Name;

        if (!table[i].ChildCommands.empty())
            list += " ...";
    }

    if (list.empty())
        return false;

    if (&table == &getCommandTable())
    {
        SendSysMessage(LANG_AVIABLE_CMD);
        PSendSysMessage("%s", list.c_str());
    }
    else
        PSendSysMessage(LANG_SUBCMDS_LIST, cmd, list.c_str());

    return true;
}

bool ChatHandler::ShowHelpForCommand(std::vector<ChatCommand> const& table, const char* cmd)
{
    if (*cmd)
    {
        for (uint32 i = 0; i < table.size(); ++i)
        {
            // must be available (ignore handler existence for show command with possible available subcommands)
            if (!isAvailable(table[i]))
                continue;

            if (!hasStringAbbr(table[i].Name, cmd))
                continue;

            // have subcommand
            char const* subcmd = (*cmd) ? strtok(NULL, " ") : "";

            if (!table[i].ChildCommands.empty() && subcmd && *subcmd)
            {
                if (ShowHelpForCommand(table[i].ChildCommands, subcmd))
                    return true;
            }

            if (!table[i].Help.empty())
                SendSysMessage(table[i].Help.c_str());

            if (!table[i].ChildCommands.empty())
                if (ShowHelpForSubCommands(table[i].ChildCommands, table[i].Name, subcmd ? subcmd : ""))
                    return true;

            return !table[i].Help.empty();
        }
    }
    else
    {
        for (uint32 i = 0; i < table.size(); ++i)
        {
            // must be available (ignore handler existence for show command with possible available subcommands)
            if (!isAvailable(table[i]))
                continue;

            if (strlen(table[i].Name))
                continue;

            if (!table[i].Help.empty())
                SendSysMessage(table[i].Help.c_str());

            if (!table[i].ChildCommands.empty())
                if (ShowHelpForSubCommands(table[i].ChildCommands, "", ""))
                    return true;

            return !table[i].Help.empty();
        }
    }

    return ShowHelpForSubCommands(table, "", cmd);
}

//Note: target_guid used only in CHAT_MSG_WHISPER_INFORM mode (in this case channelName ignored)
void ChatHandler::FillMessageData(WorldPacket* data, WorldSession* session, uint8 type, uint32 language, const char* channelName, uint64 target_guid, const char* message, Unit* speaker)
{
    uint32 messageLength = (message ? strlen(message) : 0) + 1;

    data->Initialize(SMSG_MESSAGECHAT, 100);                // guess size
    *data << uint8(type);
    if ((type != CHAT_MSG_CHANNEL && type != CHAT_MSG_WHISPER) || language == LANG_ADDON)
        *data << uint32(language);
    else
        *data << uint32(LANG_UNIVERSAL);

    switch (type)
    {
    case CHAT_MSG_SAY:
    case CHAT_MSG_PARTY:
    case CHAT_MSG_RAID:
    case CHAT_MSG_GUILD:
    case CHAT_MSG_OFFICER:
    case CHAT_MSG_YELL:
    case CHAT_MSG_WHISPER:
    case CHAT_MSG_CHANNEL:
    case CHAT_MSG_RAID_LEADER:
    case CHAT_MSG_RAID_WARNING:
    case CHAT_MSG_BG_SYSTEM_NEUTRAL:
    case CHAT_MSG_BG_SYSTEM_ALLIANCE:
    case CHAT_MSG_BG_SYSTEM_HORDE:
    case CHAT_MSG_BATTLEGROUND:
    case CHAT_MSG_BATTLEGROUND_LEADER:
        target_guid = session ? session->GetPlayer()->GetGUID() : 0;
        break;
    case CHAT_MSG_MONSTER_SAY:
    case CHAT_MSG_MONSTER_PARTY:
    case CHAT_MSG_MONSTER_YELL:
    case CHAT_MSG_MONSTER_WHISPER:
    case CHAT_MSG_MONSTER_EMOTE:
    case CHAT_MSG_RAID_BOSS_WHISPER:
    case CHAT_MSG_RAID_BOSS_EMOTE:
    {
        *data << uint64(speaker->GetGUID());
        *data << uint32(0);                             // 2.1.0
        *data << uint32(strlen(speaker->GetName()) + 1);
        *data << speaker->GetName();
        uint64 listener_guid = 0;
        *data << uint64(listener_guid);
        if (listener_guid && !IS_PLAYER_GUID(listener_guid))
        {
            *data << uint32(1);                         // string listener_name_length
            *data << uint8(0);                          // string listener_name
        }
        *data << uint32(messageLength);
        *data << message;
        *data << uint8(0);
        return;
    }
    default:
        if (type != CHAT_MSG_REPLY && type != CHAT_MSG_IGNORED && type != CHAT_MSG_DND && type != CHAT_MSG_AFK)
            target_guid = 0;                            // only for CHAT_MSG_WHISPER_INFORM used original value target_guid
        break;
    }

    *data << uint64(target_guid);                           // there 0 for BG messages
    *data << uint32(0);                                     // can be chat msg group or something

    if (type == CHAT_MSG_CHANNEL)
    {
        ASSERT(channelName);
        *data << channelName;
    }

    *data << uint64(target_guid);
    *data << uint32(messageLength);
    *data << message;
    if (session != 0 && type != CHAT_MSG_REPLY && type != CHAT_MSG_DND && type != CHAT_MSG_AFK)
        *data << uint8(session->GetPlayer()->GetChatTag());
    else
        *data << uint8(0);
}

Player* ChatHandler::getSelectedPlayer()
{
    if (!m_session)
        return NULL;

    uint64 guid = m_session->GetPlayer()->GetSelection();

    if (guid == 0)
        return m_session->GetPlayer();

    return sObjectMgr.GetPlayer(guid);
}

Player* ChatHandler::getSelectedPlayerOrSelf()
{
    if (!m_session)
        return NULL;

    uint64 selected = m_session->GetPlayer()->GetTarget();
    if (!selected)
        return m_session->GetPlayer();

    // first try with selected target
    Player* targetPlayer = ObjectAccessor::FindPlayer(selected);
    // if the target is not a player, then return self
    if (!targetPlayer)
        targetPlayer = m_session->GetPlayer();

    return targetPlayer;
}

Unit* ChatHandler::getSelectedUnit()
{
    if (!m_session)
        return NULL;

    uint64 guid = m_session->GetPlayer()->GetSelection();

    if (guid == 0)
        return m_session->GetPlayer();

    return ObjectAccessor::GetUnit(*m_session->GetPlayer(), guid);
}

Creature* ChatHandler::getSelectedCreature()
{
    if (!m_session)
        return NULL;

    return ObjectAccessor::GetCreatureOrPet(*m_session->GetPlayer(), m_session->GetPlayer()->GetSelection());
}

char* ChatHandler::extractKeyFromLink(char* text, char const* linkType, char** something1)
{
    // skip empty
    if (!text)
        return NULL;

    // skip spaces
    while (*text == ' ' || *text == '\t' || *text == '\b')
        ++text;

    if (!*text)
        return NULL;

    // return non link case
    if (text[0] != '|')
        return strtok(text, " ");

    // [name] Shift-click form |color|linkType:key|h[name]|h|r
    // or
    // [name] Shift-click form |color|linkType:key:something1:...:somethingN|h[name]|h|r

    char* check = strtok(text, "|");                        // skip color
    if (!check)
        return NULL;                                        // end of data

    char* cLinkType = strtok(NULL, ":");                    // linktype
    if (!cLinkType)
        return NULL;                                        // end of data

    if (strcmp(cLinkType, linkType) != 0)
    {
        strtok(NULL, " ");                                  // skip link tail (to allow continue strtok(NULL,s) use after retturn from function
        SendSysMessage(LANG_WRONG_LINK_TYPE);
        return NULL;
    }

    char* cKeys = strtok(NULL, "|");                        // extract keys and values
    char* cKeysTail = strtok(NULL, "");

    char* cKey = strtok(cKeys, ":|");                       // extract key
    if (something1)
        *something1 = strtok(NULL, ":|");                   // extract something

    strtok(cKeysTail, "]");                                 // restart scan tail and skip name with possible spaces
    strtok(NULL, " ");                                      // skip link tail (to allow continue strtok(NULL,s) use after return from function
    return cKey;
}

char* ChatHandler::extractKeyFromLink(char* text, char const* const* linkTypes, int* found_idx, char** something1)
{
    // skip empty
    if (!text)
        return NULL;

    // skip spaces
    while (*text == ' ' || *text == '\t' || *text == '\b')
        ++text;

    if (!*text)
        return NULL;

    // return non link case
    if (text[0] != '|')
        return strtok(text, " ");

    // [name] Shift-click form |color|linkType:key|h[name]|h|r
    // or
    // [name] Shift-click form |color|linkType:key:something1:...:somethingN|h[name]|h|r

    char* check = strtok(text, "|");                        // skip color
    if (!check)
        return NULL;                                        // end of data

    char* cLinkType = strtok(NULL, ":");                    // linktype
    if (!cLinkType)
        return NULL;                                        // end of data

    for (int i = 0; linkTypes[i]; ++i)
    {
        if (strcmp(cLinkType, linkTypes[i]) == 0)
        {
            char* cKeys = strtok(NULL, "|");                // extract keys and values
            char* cKeysTail = strtok(NULL, "");

            char* cKey = strtok(cKeys, ":|");               // extract key
            if (something1)
                *something1 = strtok(NULL, ":|");           // extract something

            strtok(cKeysTail, "]");                         // restart scan tail and skip name with possible spaces
            strtok(NULL, " ");                              // skip link tail (to allow continue strtok(NULL,s) use after return from function
            if (found_idx)
                *found_idx = i;
            return cKey;
        }
    }

    strtok(NULL, " ");                                      // skip link tail (to allow continue strtok(NULL,s) use after return from function
    SendSysMessage(LANG_WRONG_LINK_TYPE);
    return NULL;
}

char const* fmtstring(char const* format, ...)
{
    va_list        argptr;
#define    MAX_FMT_STRING    32000
    static char        temp_buffer[MAX_FMT_STRING];
    static char        string[MAX_FMT_STRING];
    static int        index = 0;
    char*    buf;
    int len;

    va_start(argptr, format);
    vsnprintf(temp_buffer, MAX_FMT_STRING, format, argptr);
    va_end(argptr);

    len = strlen(temp_buffer);

    if (len >= MAX_FMT_STRING)
        return "ERROR";

    if (len + index >= MAX_FMT_STRING - 1)
        index = 0;

    buf = &string[index];
    memcpy(buf, temp_buffer, len + 1);

    index += len + 1;

    return buf;
}

GameObject* ChatHandler::GetObjectGlobalyWithGuidOrNearWithDbGuid(uint32 lowguid, uint32 entry)
{
    if (!m_session)
        return NULL;

    Player* pl = m_session->GetPlayer();

    GameObject* obj = pl->GetMap()->GetGameObject(MAKE_NEW_GUID(lowguid, entry, HIGHGUID_GAMEOBJECT));

    if (!obj && sObjectMgr.GetGOData(lowguid))                   // guid is DB guid of object
    {
        // search near player then
        CellCoord p(Oregon::ComputeCellCoord(pl->GetPositionX(), pl->GetPositionY()));
        Cell cell(p);

        Oregon::GameObjectWithDbGUIDCheck go_check(*pl, lowguid);
        Oregon::GameObjectSearcher<Oregon::GameObjectWithDbGUIDCheck> checker(pl, obj, go_check);

        TypeContainerVisitor<Oregon::GameObjectSearcher<Oregon::GameObjectWithDbGUIDCheck>, GridTypeMapContainer > object_checker(checker);
        cell.Visit(p, object_checker, *pl->GetMap(), *pl, pl->GetGridActivationRange());
    }

    return obj;
}

static char const* const spellTalentKeys[] =
{
    "Hspell",
    "Htalent",
    0
};

uint32 ChatHandler::extractSpellIdFromLink(char* text)
{
    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r
    // number or [name] Shift-click form |color|Htalent:talent_id,rank|h[name]|h|r
    int type = 0;
    char* rankS = NULL;
    char* idS = extractKeyFromLink(text, spellTalentKeys, &type, &rankS);
    if (!idS)
        return 0;

    uint32 id = (uint32)atol(idS);

    // spell
    if (type == 0)
        return id;

    // talent
    TalentEntry const* talentEntry = sTalentStore.LookupEntry(id);
    if (!talentEntry)
        return 0;

    int32 rank = rankS ? (uint32)atol(rankS) : 0;
    if (rank >= 5)
        return 0;

    if (rank < 0)
        rank = 0;

    return talentEntry->RankID[rank];
}

GameTele const* ChatHandler::extractGameTeleFromLink(char* text)
{
    // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
    char* cId = extractKeyFromLink(text, "Htele");
    if (!cId)
        return NULL;

    // id case (explicit or from shift link)
    if (cId[0] >= '0' || cId[0] >= '9')
        if (uint32 id = atoi(cId))
            return sObjectMgr.GetGameTele(id);

    return sObjectMgr.GetGameTele(cId);
}

char* ChatHandler::extractQuotedArg(char* args)
{
    if (!args || !*args)
        return NULL;

    if (*args == '"')
        return strtok(args + 1, "\"");
    else
    {
        char* space = strtok(args, "\"");
        if (!space)
            return NULL;
        return strtok(NULL, "\"");
    }
}

const char* ChatHandler::GetName() const
{
    return m_session->GetPlayer()->GetName();
}

bool ChatHandler::needReportToTarget(Player* chr) const
{
    Player* pl = m_session->GetPlayer();
    return pl != chr && pl->IsVisibleGloballyFor(chr);
}

const char* CliHandler::GetOregonString(int32 entry) const
{
    return sObjectMgr.GetOregonStringForDBCLocale(entry);
}

bool CliHandler::isAvailable(ChatCommand const& cmd) const
{
    // skip non-console commands in console case
    return cmd.AllowConsole;
}

std::string CliHandler::GetNameLink() const
{
    return GetOregonString(LANG_CONSOLE_COMMAND);
}

void CliHandler::SendSysMessage(const char* str)
{
    m_print(m_callbackArg, str);
    m_print(m_callbackArg, "\n");
}

const char* CliHandler::GetName() const
{
    return GetOregonString(LANG_CONSOLE_COMMAND);
}

bool CliHandler::needReportToTarget(Player* /*chr*/) const
{
    return true;
}

bool ChatHandler::GetPlayerGroupAndGUIDByName(const char* cname, Player*& plr, Group*& group, uint64& guid, bool offline)
{
    plr = NULL;
    guid = 0;

    if (cname)
    {
        std::string name = cname;
        if (!name.empty())
        {
            if (!normalizePlayerName(name))
            {
                PSendSysMessage(LANG_PLAYER_NOT_FOUND);
                SetSentErrorMessage(true);
                return false;
            }

            plr = sObjectMgr.GetPlayer(name.c_str());
            if (offline)
                guid = sObjectMgr.GetPlayerGUIDByName(name.c_str());
        }
    }

    if (plr)
    {
        group = plr->GetGroup();
        if (!guid || !offline)
            guid = plr->GetGUID();
    }
    else
    {
        if (getSelectedPlayer())
            plr = getSelectedPlayer();
        else
            plr = m_session->GetPlayer();

        if (!guid || !offline)
            guid = plr->GetGUID();
        group = plr->GetGroup();
    }

    return true;
}
