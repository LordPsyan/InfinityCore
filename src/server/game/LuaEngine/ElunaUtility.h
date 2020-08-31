/*
* Copyright (C) 2010 - 2016 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.md for more information
*/

#ifndef _ELUNA_UTIL_H
#define _ELUNA_UTIL_H

#include <unordered_map>
#include <unordered_set>
#include "Common.h"
#include "SharedDefines.h"
#include "ObjectGuid.h"
#include "Database/QueryResult.h"

#include <ace/RW_Thread_Mutex.h>
#include <ace/Guard_T.h>

typedef QueryResult ElunaQuery;
#define ELUNA_LOG_INFO(...)     sLog.outString(__VA_ARGS__);
#define ELUNA_LOG_ERROR(...)    sLog.outError(__VA_ARGS__);
#define ELUNA_LOG_DEBUG(...)    sLog.outDebug(__VA_ARGS__);
#define GET_GUID                GetGUID
#define GetGameObjectTemplate   GetGameObjectInfo
#define GetTemplate             GetProto

#ifndef MAKE_NEW_GUID
#define MAKE_NEW_GUID(l, e, h)  ObjectGuid(h, e, l)
#endif
#ifndef GUID_ENPART
#define GUID_ENPART(guid)       ObjectGuid(guid).GetEntry()
#endif
#ifndef GUID_LOPART
#define GUID_LOPART(guid)       ObjectGuid(guid).GetCounter()
#endif
#ifndef GUID_HIPART
#define GUID_HIPART(guid)       ObjectGuid(guid).GetHigh()
#endif

class Unit;
class WorldObject;
struct FactionTemplateEntry;

namespace ElunaUtil
{
    uint32 GetCurrTime();

    uint32 GetTimeDiff(uint32 oldMSTime);

    class ObjectGUIDCheck
    {
    public:
        ObjectGUIDCheck(ObjectGuid guid);
        bool operator()(WorldObject* object);

        ObjectGuid _guid;
    };

    // Binary predicate to sort WorldObjects based on the distance to a reference WorldObject
    class ObjectDistanceOrderPred
    {
    public:
        ObjectDistanceOrderPred(WorldObject const* pRefObj, bool ascending = true);
        bool operator()(WorldObject const* pLeft, WorldObject const* pRight) const;

        WorldObject const* m_refObj;
        const bool m_ascending;
    };

    // Doesn't get self
    class WorldObjectInRangeCheck
    {
    public:
        WorldObjectInRangeCheck(bool nearest, WorldObject const* obj, float range,
            uint16 typeMask = 0, uint32 entry = 0, uint32 hostile = 0, uint32 dead = 0);
        WorldObject const& GetFocusObject() const;
        bool operator()(WorldObject* u);

        WorldObject const* const i_obj;
        Unit const* i_obj_unit;
        FactionTemplateEntry const* i_obj_fact;
        uint32 const i_hostile; // 0 both, 1 hostile, 2 friendly
        uint32 const i_entry;
        float i_range;
        uint16 const i_typeMask;
        uint32 const i_dead; // 0 both, 1 alive, 2 dead
        bool const i_nearest;
    };

    /*
     * Usage:
     * Inherit this class, then when needing lock, use
     * ReadGuard guard(GetLock());
     * or
     * WriteGuard guard(GetLock());
     *
     * The lock is automatically released at end of scope
     */
    class RWLockable
    {
    public:

        typedef ACE_RW_Thread_Mutex LockType;
        typedef ACE_Read_Guard<LockType> ReadGuard;
        typedef ACE_Write_Guard<LockType> WriteGuard;

        LockType& GetLock() { return _lock; }

    private:
        LockType _lock;
    };

    /*
     * Encodes `data` in Base-64 and store the result in `output`.
     */
    void EncodeData(const unsigned char* data, size_t input_length, std::string& output);

    /*
     * Decodes `data` from Base-64 and returns a pointer to the result, or `NULL` on error.
     *
     * The returned result buffer must be `delete[]`ed by the caller.
     */
    unsigned char* DecodeData(const char* data, size_t *output_length);
};

#endif
