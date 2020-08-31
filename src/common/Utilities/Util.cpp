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

#include "Util.h"

#include "utf8.h"
#include "SFMT.h"
#include <ace/TSS_T.h>
#include <ace/INET_Addr.h>
#include <array>

typedef ACE_TSS<SFMTRand> SFMTRandTSS;
static SFMTRandTSS sfmtRand;

int32 irand (int32 min, int32 max)
{
    return int32(sfmtRand->IRandom(min, max));
}

uint32 urand (uint32 min, uint32 max)
{
    return sfmtRand->URandom(min, max);
}

float frand(float min, float max)
{
    return float(sfmtRand->Random() * (max - min) + min);
}

uint32 rand32()
{
    return sfmtRand->BRandom();
}

double rand_norm()
{
    return sfmtRand->Random();
}

double rand_chance()
{
    return sfmtRand->Random() * 100.0;
}

Tokens StrSplit(const std::string& src, const std::string& sep)
{
    Tokens r;
    std::string s;
    for (std::string::const_iterator i = src.begin(); i != src.end(); i++)
    {
        if (sep.find(*i) != std::string::npos)
        {
            if (s.length()) r.push_back(s);
            s = "";
        }
        else
            s += *i;
    }
    if (s.length()) r.push_back(s);
    return r;
}

void stripLineInvisibleChars(std::string& str)
{
    static std::string const invChars = " \t\7\n";

    size_t wpos = 0;

    bool space = false;
    for (size_t pos = 0; pos < str.size(); ++pos)
    {
        if (invChars.find(str[pos]) != std::string::npos)
        {
            if (!space)
            {
                str[wpos++] = ' ';
                space = true;
            }
        }
        else
        {
            if (wpos != pos)
                str[wpos++] = str[pos];
            else
                ++wpos;
            space = false;
        }
    }

    if (wpos < str.size())
        str.erase(wpos, str.size());
    if (str.find("|TInterface") != std::string::npos)
        str.clear();
}

std::string secsToTimeString(uint32 timeInSecs, bool shortText, bool hoursOnly)
{
    uint32 secs    = timeInSecs % MINUTE;
    uint32 minutes = timeInSecs % HOUR / MINUTE;
    uint32 hours   = timeInSecs % DAY  / HOUR;
    uint32 days    = timeInSecs / DAY;

    std::ostringstream ss;
    if (days)
        ss << days << (shortText ? "d" : " Day(s) ");
    if (hours || hoursOnly)
        ss << hours << (shortText ? "h" : " Hour(s) ");
    if (!hoursOnly)
    {
        if (minutes)
            ss << minutes << (shortText ? "m" : " Minute(s) ");
        if (secs || (!days && !hours && !minutes) )
            ss << secs << (shortText ? "s" : " Second(s).");
    }

    return ss.str();
}

uint32 TimeStringToSecs(const std::string& timestring)
{
    uint32 secs       = 0;
    uint32 buffer     = 0;
    uint32 multiplier = 0;

    for (std::string::const_iterator itr = timestring.begin(); itr != timestring.end(); itr++ )
    {
        if (isdigit(*itr))
        {
            std::string str;                                //very complicated typecast char->const char*; is there no better way?
            str += *itr;
            const char* tmp = str.c_str();

            buffer *= 10;
            buffer += atoi(tmp);
        }
        else
        {
            switch (*itr)
            {
            case 'd':
                multiplier = DAY;
                break;
            case 'h':
                multiplier = HOUR;
                break;
            case 'm':
                multiplier = MINUTE;
                break;
            case 's':
                multiplier = 1;
                break;
            default :
                return 0;                         //bad format
            }
            buffer *= multiplier;
            secs += buffer;
            buffer = 0;
        }
    }

    return secs;
}

std::string TimeToTimestampStr(time_t t)
{
    tm* aTm = localtime(&t);
    //       YYYY   year
    //       MM     month (2 digits 01-12)
    //       DD     day (2 digits 01-31)
    //       HH     hour (2 digits 00-23)
    //       MM     minutes (2 digits 00-59)
    //       SS     seconds (2 digits 00-59)
    char buf[72];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d_%02d-%02d-%02d", aTm->tm_year + 1900, aTm->tm_mon + 1, aTm->tm_mday, aTm->tm_hour, aTm->tm_min, aTm->tm_sec);
    return std::string(buf);
}

// Check if the string is a valid ip address representation
bool IsIPAddress(char const* ipaddress)
{
    if (!ipaddress)
        return false;

    // Let the big boys do it.
    // Drawback: all valid ip address formats are recognized e.g.: 12.23,121234,0xABCD)
    return inet_addr(ipaddress) != INADDR_NONE;
}

// create PID file
uint32 CreatePIDFile(const std::string& filename)
{
    FILE* pid_file = fopen (filename.c_str(), "w" );
    if (pid_file == NULL)
        return 0;

    #ifdef _WIN32
    DWORD pid = GetCurrentProcessId();
    #else
    pid_t pid = getpid();
    #endif

    fprintf(pid_file, "%u", pid );
    fclose(pid_file);

    return (uint32)pid;
}

size_t utf8length(std::string& utf8str)
{
    try
    {
        return utf8::distance(utf8str.c_str(), utf8str.c_str() + utf8str.size());
    }
    catch (std::exception)
    {
        utf8str = "";
        return 0;
    }
}

void utf8truncate(std::string& utf8str, size_t len)
{
    try
    {
        size_t wlen = utf8::distance(utf8str.c_str(), utf8str.c_str() + utf8str.size());
        if (wlen <= len)
            return;

        std::wstring wstr;
        wstr.resize(wlen);
        utf8::utf8to16(utf8str.c_str(), utf8str.c_str() + utf8str.size(), &wstr[0]);
        wstr.resize(len);
        char* oend = utf8::utf16to8(wstr.c_str(), wstr.c_str() + wstr.size(), &utf8str[0]);
        utf8str.resize(oend - (&utf8str[0]));               // remove unused tail
    }
    catch (std::exception)
    {
        utf8str = "";
    }
}

bool Utf8toWStr(char const* utf8str, size_t csize, wchar_t* wstr, size_t& wsize)
{
    try
    {
        size_t len = utf8::distance(utf8str, utf8str + csize);
        if (len > wsize)
        {
            wsize = 0;
            *wstr = L'\0';
            return false;
        }

        wsize = len;
        utf8::utf8to16(utf8str, utf8str + csize, wstr);
        wstr[len] = L'\0';
    }
    catch (std::exception)
    {
        wsize = 0;
        *wstr = L'\0';
        return false;
    }

    return true;
}

bool Utf8toWStr(const std::string& utf8str, std::wstring& wstr)
{
    wstr.clear();
    try
    {
        utf8::utf8to16(utf8str.c_str(), utf8str.c_str()+utf8str.size(), std::back_inserter(wstr));
    }
    catch(std::exception const&)
    {
        wstr.clear();
        return false;
    }

    return true;
}

bool WStrToUtf8(wchar_t* wstr, size_t size, std::string& utf8str)
{
    try
    {
        std::string utf8str2;
        utf8str2.resize(size * 4);                          // allocate for most long case

        char* oend = utf8::utf16to8(wstr, wstr + size, &utf8str2[0]);
        utf8str2.resize(oend - (&utf8str2[0]));             // remove unused tail
        utf8str = utf8str2;
    }
    catch (std::exception)
    {
        utf8str = "";
        return false;
    }

    return true;
}

bool WStrToUtf8(std::wstring wstr, std::string& utf8str)
{
    try
    {
        std::string utf8str2;
        utf8str2.resize(wstr.size() * 4);                   // allocate for most long case

        char* oend = utf8::utf16to8(wstr.c_str(), wstr.c_str() + wstr.size(), &utf8str2[0]);
        utf8str2.resize(oend - (&utf8str2[0]));              // remove unused tail
        utf8str = utf8str2;
    }
    catch (std::exception)
    {
        utf8str = "";
        return false;
    }

    return true;
}

typedef wchar_t const* const* wstrlist;

std::wstring GetMainPartOfName(std::wstring wname, uint32 declension)
{
    // supported only Cyrillic cases
    if (wname.empty() || !isCyrillicCharacter(wname[0]) || declension > 5)
        return wname;

    // Important: end length must be <= MAX_INTERNAL_PLAYER_NAME-MAX_PLAYER_NAME (3 currently)

    static std::wstring const a_End    = { wchar_t(0x0430), wchar_t(0x0000) };
    static std::wstring const o_End    = { wchar_t(0x043E), wchar_t(0x0000) };
    static std::wstring const ya_End   = { wchar_t(0x044F), wchar_t(0x0000) };
    static std::wstring const ie_End   = { wchar_t(0x0435), wchar_t(0x0000) };
    static std::wstring const i_End    = { wchar_t(0x0438), wchar_t(0x0000) };
    static std::wstring const yeru_End = { wchar_t(0x044B), wchar_t(0x0000) };
    static std::wstring const u_End    = { wchar_t(0x0443), wchar_t(0x0000) };
    static std::wstring const yu_End   = { wchar_t(0x044E), wchar_t(0x0000) };
    static std::wstring const oj_End   = { wchar_t(0x043E), wchar_t(0x0439), wchar_t(0x0000) };
    static std::wstring const ie_j_End = { wchar_t(0x0435), wchar_t(0x0439), wchar_t(0x0000) };
    static std::wstring const io_j_End = { wchar_t(0x0451), wchar_t(0x0439), wchar_t(0x0000) };
    static std::wstring const o_m_End  = { wchar_t(0x043E), wchar_t(0x043C), wchar_t(0x0000) };
    static std::wstring const io_m_End = { wchar_t(0x0451), wchar_t(0x043C), wchar_t(0x0000) };
    static std::wstring const ie_m_End = { wchar_t(0x0435), wchar_t(0x043C), wchar_t(0x0000) };
    static std::wstring const soft_End = { wchar_t(0x044C), wchar_t(0x0000) };
    static std::wstring const j_End    = { wchar_t(0x0439), wchar_t(0x0000) };

    static std::array<std::array<std::wstring const*, 7>, 6> const dropEnds = {{
        { &a_End,  &o_End,    &ya_End,   &ie_End,  &soft_End, &j_End,    nullptr },
        { &a_End,  &ya_End,   &yeru_End, &i_End,   nullptr,   nullptr,   nullptr },
        { &ie_End, &u_End,    &yu_End,   &i_End,   nullptr,   nullptr,   nullptr },
        { &u_End,  &yu_End,   &o_End,    &ie_End,  &soft_End, &ya_End,   &a_End  },
        { &oj_End, &io_j_End, &ie_j_End, &o_m_End, &io_m_End, &ie_m_End, &yu_End },
        { &ie_End, &i_End,    nullptr,   nullptr,  nullptr,   nullptr,   nullptr }
    }};

    std::size_t const thisLen = wname.length();
    std::array<std::wstring const*, 7> const& endings = dropEnds[declension];
    for (auto itr = endings.begin(), end = endings.end(); (itr != end) && *itr; ++itr)
    {
        std::wstring const& ending = **itr;
        std::size_t const endLen = ending.length();
        if (!(endLen <= thisLen))
            continue;

        if (wname.substr(thisLen-endLen, thisLen) == ending)
            return wname.substr(0, thisLen-endLen);
    }

    return wname;
}

bool utf8ToConsole(const std::string& utf8str, std::string& conStr)
{
    #if PLATFORM == PLATFORM_WINDOWS
    std::wstring wstr;
    if (!Utf8toWStr(utf8str, wstr))
        return false;

    conStr.resize(wstr.size());
    CharToOemBuffW(&wstr[0], &conStr[0], wstr.size());
    #else
    // not implemented yet
    conStr = utf8str;
    #endif

    return true;
}

bool consoleToUtf8(const std::string& conStr, std::string& utf8str)
{
    #if PLATFORM == PLATFORM_WINDOWS
    std::wstring wstr;
    wstr.resize(conStr.size());
    OemToCharBuffW(&conStr[0], &wstr[0], conStr.size());

    return WStrToUtf8(wstr, utf8str);
    #else
    // not implemented yet
    utf8str = conStr;
    return true;
    #endif
}

bool Utf8FitTo(const std::string& str, std::wstring search)
{
    std::wstring temp;

    if (!Utf8toWStr(str, temp))
        return false;

    // converting to lower case
    wstrToLower( temp );

    if (temp.find(search) == std::wstring::npos)
        return false;

    return true;
}

void hexEncodeByteArray(uint8* bytes, uint32 arrayLen, std::string& result)
{
    std::ostringstream ss;
    for (uint32 i = 0; i < arrayLen; ++i)
    {
        for (uint8 j = 0; j < 2; ++j)
        {
            unsigned char nibble = 0x0F & (bytes[i] >> ((1 - j) * 4));
            char encodedNibble;
            if (nibble < 0x0A)
                encodedNibble = '0' + nibble;
            else
                encodedNibble = 'A' + nibble - 0x0A;
            ss << encodedNibble;
        }
    }
    result = ss.str();
}

std::string ByteArrayToHexStr(uint8* bytes, uint32 length)
{
    std::ostringstream ss;
    for (uint32 i = 0; i < length; ++i)
    {
        char buffer[4];
        sprintf(buffer, "%02X", bytes[i]);
        ss << buffer;
    }

    return ss.str();
}
