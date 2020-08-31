/* Copyright (C) 2008-2010 Trinity <http://www.trinitycore.org/>
 *
 * Thanks to the original authors: ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_SCRIPTMGR_H
#define SC_SCRIPTMGR_H

#include "Common.h"

#include "DBCStructure.h"
#include "ObjectMgr.h"
#include "Battleground.h"
#include "OutdoorPvPMgr.h"
#include "SharedDefines.h"
#include "Chat.h"
#include "Weather.h"
#include "AuctionHouseMgr.h"
#include "ConditionMgr.h"
#include "Player.h"
#include "Transports.h"
#include "WorldSession.h"

class Player;
class BattlegroundScript;
class Creature;
class CreatureAI;
class InstanceData;
class SpellScript;
class Quest;
class Item;
class Transport;
class GameObject;
class SpellCastTargets;
class Map;
class Unit;
class WorldObject;
struct ItemPrototype;
class Spell;
class ScriptMgr;
class WorldSocket;
class GuildScript;

#define VISIBLE_RANGE       (166.0f)                        //MAX visible range (size of grid)
#define DEFAULT_TEXT        "<Trinity Script Text Entry Missing!>"

// Generic scripting text function.
void DoScriptText(int32 textEntry, WorldObject* pSource, Unit *pTarget = NULL);


class ScriptObject
{
    friend class ScriptMgr;

public:

    // Called when the script is initialized. Use it to initialize any properties of the script.
    virtual void OnInitialize() { }

    // Called when the script is deleted. Use it to free memory, etc.
    virtual void OnTeardown() { }

    // Do not override this in scripts; it should be overridden by the various script type classes. It indicates
    // whether or not this script type must be assigned in the database.
    virtual bool IsDatabaseBound() const { return false; }

    const std::string& GetName() const { return _name; }

    const char* ToString() const { return _name.c_str(); }

protected:

    ScriptObject(const char* name)
        : _name(std::string(name))
    {
        // Allow the script to do startup routines.
        OnInitialize();
    }

    virtual ~ScriptObject()
    {
        // Allow the script to do cleanup routines.
        OnTeardown();
    }

private:

    const std::string _name;
};

template<class TObject> class UpdatableScript
{
protected:

    UpdatableScript()
    {
    }

public:

    virtual void OnUpdate(TObject* obj, uint32 diff) { }
};

class SpellHandlerScript : public ScriptObject
{
protected:

    SpellHandlerScript(const char* name);

public:

    bool IsDatabaseBound() const { return true; }

    // Should return a fully valid SpellScript pointer.
    virtual SpellScript* GetSpellScript() const = 0;
};

class AuraHandlerScript : public ScriptObject
{
protected:

    AuraHandlerScript(const char* name);

public:

    bool IsDatabaseBound() const { return true; }

    // Should return a fully valid AuraScript pointer.
    // virtual AuraScript* GetAuraScript() const = 0;
};

class ServerScript : public ScriptObject
{
protected:

    ServerScript(const char* name);

public:

    // Called when reactive socket I/O is started (WorldSocketMgr).
    virtual void OnNetworkStart() { }

    // Called when reactive I/O is stopped.
    virtual void OnNetworkStop() { }

    // Called when a remote socket establishes a connection to the server. Do not store the socket object.
    virtual void OnSocketOpen(WorldSocket* socket) { }

    // Called when a socket is closed. Do not store the socket object, and do not rely on the connection
    // being open; it is not.
    virtual void OnSocketClose(WorldSocket* socket, bool wasNew) { }

    // Called when a packet is sent to a client. The packet object is a copy of the original packet, so reading
    // and modifying it is safe.
    virtual void OnPacketSend(WorldSocket* socket, WorldPacket& packet) { }

    // Called when a (valid) packet is received by a client. The packet object is a copy of the original packet, so
    // reading and modifying it is safe.
    virtual void OnPacketReceive(WorldSocket* socket, WorldPacket& packet) { }

    // Called when an invalid (unknown opcode) packet is received by a client. The packet is a reference to the orignal
    // packet; not a copy. This allows you to actually handle unknown packets (for whatever purpose).
    virtual void OnUnknownPacketReceive(WorldSocket* socket, WorldPacket& packet) { }
};

class WorldScript : public ScriptObject, public UpdatableScript<void>
{
protected:

    WorldScript(const char* name);

public:

	virtual void OnLoadCustomDatabaseTable() { }

    // Called when the open/closed state of the world changes.
    virtual void OnOpenStateChange(bool open) { }

    // Called after the world configuration is (re)loaded.
    virtual void OnConfigLoad(bool reload) { }

    // Called before the message of the day is changed.
    virtual void OnMotdChange(std::string& newMotd) { }

    // Called when a world shutdown is initiated.
    virtual void OnShutdown(ShutdownExitCode code, ShutdownMask mask) { }

    // Called when a world shutdown is cancelled.
    virtual void OnShutdownCancel() { }

	// Called when the world is started.
	virtual void OnStartup() { }

    // Called on every world tick (don't execute too heavy code here).
    virtual void OnUpdate(void* null, uint32 diff) { }
};

class GroupScript : public ScriptObject
{
protected:

    GroupScript(char const* name);

public:

    // Called when a member is added to a group.
    virtual void OnAddMember(Group* /*group*/, Player* /*guid*/) { }

    // Called when a member is invited to join a group.
    virtual void OnInviteMember(Group* /*group*/, Player* /*guid*/) { }

    // Called when a member is joined a group.
    virtual void OnMemberJoin(Group* /*group*/, Player* /*player*/) { }

    // Called when a member is removed from a group.
    virtual void OnRemoveMember(Group* /*group*/, Player* /*guid*/, RemoveMethod /*method*/, uint64 /*kicker*/, char const* /*reason*/) { }

    // Called when the leader of a group is changed.
    virtual void OnChangeLeader(Group* /*group*/, Player* /*newLeader*/, Player* /*oldLeader*/) { }

    // Called when a group is disbanded.
    virtual void OnCreate(Group* /*group*/, Player* /*Leader*/) { }

    // Called when a group is disbanded.
    virtual void OnDisband(Group* /*group*/, Player* /*Leader*/) { }
};

class PlayerScript : public ScriptObject
{
protected:

    PlayerScript(char const* name);

public:
    // Called when player completes quest
    virtual void OnPlayerCompleteQuest(Player* /*player*/, Quest const* /*quest*/) { }

    // Called when player loots money
    virtual void OnLootMoney(Player* /*player*/, uint32 /*amount*/) { }

    // Called When a player Loots an item
    virtual void OnLootItem(Player* /*player*/, Item* /*item*/, uint32 /*count*/, uint64 /*itemGUID*/) { }

    // Called when player creates an item
    virtual void OnCreateItem(Player* /*player*/, Item* /*item*/, uint32 /*count*/) { }

    // Called when player recieves item from quest reward
    virtual void OnQuestRewardItem(Player* /*player*/, Item* /*item*/, uint32 /*count*/) { }

    // Called for player::update
    virtual void OnBeforeUpdate(Player* /*player*/, uint32 /*p_time*/) { }

    // Called when a player kills another player
    virtual void OnPVPKill(Player* /*killer*/, Player* /*killed*/) { }

    // Called when a player kills a creature
    virtual void OnCreatureKill(Player* /*killer*/, Creature* /*killed*/) { }

    // Called when a player is killed by a creature
    virtual void OnPlayerKilledByCreature(Creature* /*killer*/, Player* /*killed*/) { }

    // Called when a player's level changes (after the level is applied)
    virtual void OnLevelChanged(Player* /*player*/, uint8 /*oldLevel*/, uint8 /*newLevel*/) { }

    // Called when a player's free talent points change (right before the change is applied)
    virtual void OnFreeTalentPointsChanged(Player* /*player*/, uint32 /*points*/) { }

    // Called when a player's talent points are reset (right before the reset is done)
    virtual void OnTalentsReset(Player* /*player*/, bool /*noCost*/) { }

    // Called when a player's money is modified (before the modification is done)
    virtual void OnMoneyChanged(Player* /*player*/, int32& /*amount*/) { }

    // Called when a player's money is at limit (amount = money tried to add)
    virtual void OnMoneyLimit(Player* /*player*/, int32 /*amount*/) { }

    // Called when a player gains XP (before anything is given)
    virtual void OnGiveXP(Player* /*player*/, uint32& /*amount*/, Unit* /*victim*/) { }

    // Called when a player's reputation changes (before it is actually changed)
    virtual void OnReputationChange(Player* /*player*/, uint32 /*factionId*/, int32& /*standing*/, bool /*incremental*/) { }

    // Called when a duel is requested
    virtual void OnDuelRequest(Player* /*target*/, Player* /*challenger*/) { }

    // Called when a duel starts (after 3s countdown)
    virtual void OnDuelStart(Player* /*player1*/, Player* /*player2*/) { }

    // Called when a duel ends
    virtual void OnDuelEnd(Player* /*winner*/, Player* /*loser*/, DuelCompleteType /*type*/) { }

    // The following methods are called when a player sends a chat message.
    virtual void OnChat(Player* /*player*/, uint32 /*type*/, uint32 /*lang*/, std::string& /*msg*/) { }

    virtual void OnChat(Player* /*player*/, uint32 /*type*/, uint32 /*lang*/, std::string& /*msg*/, Player* /*receiver*/) { }

    virtual void OnChat(Player* /*player*/, uint32 /*type*/, uint32 /*lang*/, std::string& /*msg*/, Group* /*group*/) { }

    virtual void OnChat(Player* /*player*/, uint32 /*type*/, uint32 /*lang*/, std::string& /*msg*/, Guild* /*guild*/) { }

    virtual void OnChat(Player* /*player*/, uint32 /*type*/, uint32 /*lang*/, std::string& /*msg*/, Channel* /*channel*/) { }

    // Both of the below are called on emote opcodes.
    virtual void OnEmote(Player* /*player*/, uint32 /*emote*/) { }

    virtual void OnTextEmote(Player* /*player*/, uint32 /*textEmote*/, uint32 /*emoteNum*/, ObjectGuid /*guid*/) { }

    // Called in Spell::Cast.
    virtual void OnSpellCast(Player* /*player*/, Spell* /*spell*/, bool /*skipCheck*/) { }

    // Called when a player logs in.
    virtual void OnLogin(Player* /*player*/, bool /*firstLogin*/) { }

    // Called when a player logs out.
    virtual void OnLogout(Player* /*player*/) { }

    // Called when a player is created.
    virtual void OnCreate(Player* /*player*/) { }

    // Called When player is loading data from DB
    virtual void OnPlayerLoadFromDB(Player* /*player*/) { }

    // Called when a player is deleted.
    virtual void OnDelete(ObjectGuid /*guid*/, uint32 /*accountId*/) { }

    // Called when a player delete failed
    virtual void OnFailedDelete(ObjectGuid /*guid*/, uint32 /*accountId*/) { }

    // Called when a player is about to be saved.
    virtual void OnSave(Player* /*player*/) { }

    // Called when a player is bound to an instance
    virtual void OnBindToInstance(Player* /*player*/, DungeonDifficulty /*difficulty*/, uint32 /*mapId*/, bool /*permanent*/, uint8 /*extendState*/) { }

    // Called when a player switches to a new zone
    virtual void OnUpdateZone(Player* /*player*/, uint32 /*newZone*/, uint32 /*newArea*/) { }

    // Called when a player changes to a new map (after moving to new map)
    virtual void OnMapChanged(Player* /*player*/) { }

    // Called when a player obtains progress on a quest's objective
    virtual void OnQuestObjectiveProgress(Player* /*player*/, Quest const* /*quest*/, uint32 /*objectiveIndex*/, uint16 /*progress*/) { }

    // Called after a player's quest status has been changed
    virtual void OnQuestStatusChange(Player* /*player*/, uint32 /*questId*/) { }

    // Called when a player presses release when he died
    virtual void OnPlayerRepop(Player* /*player*/) { }

    // Called when a player selects an option in a player gossip window
    virtual void OnGossipSelect(Player* /*player*/, uint32 /*menu_id*/, uint32 /*sender*/, uint32 /*action*/) { }

    // Called when a player selects an option in a player gossip window
    virtual void OnGossipSelectCode(Player* /*player*/, uint32 /*menu_id*/, uint32 /*sender*/, uint32 /*action*/, const char* /*code*/) { }
};


class GuildScript : public ScriptObject
{
protected:
    GuildScript(const char* name);

public:

    bool IsDatabaseBound() const { return false; }
    
    // Called when a guild is created.
    virtual void OnCreate(Guild* /*guild*/, Player* /*leader*/, const std::string& /*name*/) { }

    // Called when a guild is disbanded.
    virtual void OnDisband(Guild* /*guild*/) { }

    // Called when a member is added to the guild.
    virtual void OnAddMember(Guild* /*guild*/, Player* /*player*/, uint32& /*plRank*/) { }

    // Called when a member is removed from the guild.
    virtual void OnRemoveMember(Guild* /*guild*/, Player* /*player*/, bool /*isDisbanding*/) { }
};

class FormulaScript : public ScriptObject
{
protected:

    FormulaScript(const char* name);

public:

    // Called after calculating honor.
    virtual void OnHonorCalculation(float& honor, uint8 level, uint32 count) { }

    // Called after calculating honor.
    virtual void OnHonorCalculation(uint32& honor, uint8 level, uint32 count) { }

    // Called after gray level calculation.
    virtual void OnGetGrayLevel(uint8& grayLevel, uint8 playerLevel) { }

    // Called after calculating experience color.
    virtual void OnGetColorCode(XPColorChar& color, uint8 playerLevel, uint8 mobLevel) { }

    // Called after calculating zero difference.
    virtual void OnGetZeroDifference(uint8& diff, uint8 playerLevel) { }

    // Called after calculating base experience gain.
    virtual void OnGetBaseGain(uint32& gain, uint8 playerLevel, uint8 mobLevel, ContentLevels content) { }

    // Called after calculating experience gain.
    virtual void OnGetGain(uint32& gain, Player* player, Unit* unit) { }

    virtual void OnGetGroupRate(float& rate, uint32 count, bool isRaid) { }
};

template<class TMap> class MapScript : public UpdatableScript<TMap>
{
    MapEntry const* _mapEntry;

protected:

    MapScript(uint32 mapId)
        : _mapEntry(sMapStore.LookupEntry(mapId))
    {
        if (!_mapEntry)
            sLog.outError("Invalid MapScript for %u; no such map ID.", mapId);
    }

public:

    // Gets the MapEntry structure associated with this script. Can return NULL.
    MapEntry const* GetEntry() { return _mapEntry; }

    // Called when the map is created.
    virtual void OnCreate(TMap* map) { }

    // Called just before the map is destroyed.
    virtual void OnDestroy(TMap* map) { }

    // Called when a grid map is loaded.
    virtual void OnLoadGridMap(TMap* map, uint32 gx, uint32 gy) { }

    // Called when a grid map is unloaded.
    virtual void OnUnloadGridMap(TMap* map, uint32 gx, uint32 gy) { }

    // Called when a player enters the map.
    virtual void OnPlayerEnter(TMap* map, Player* player) { }

    // Called when a player leaves the map.
    virtual void OnPlayerLeave(TMap* map, Player* player) { }

    // Called on every map update tick.
    virtual void OnUpdate(TMap* map, uint32 diff) { }
};

class WorldMapScript : public ScriptObject, public MapScript<Map>
{
protected:

    WorldMapScript(const char* name, uint32 mapId);

};

class InstanceMapScript : public ScriptObject, public MapScript<InstanceMap>
{
protected:

    InstanceMapScript(const char* name, uint32 mapId = 0);


public:

    bool IsDatabaseBound() const { return true; }

    // Gets an InstanceData object for this instance.
    virtual InstanceData* GetInstanceScript(InstanceMap* map) const { return NULL; }
};

class BattlegroundMapScript : public ScriptObject, public MapScript<BattlegroundMap>
{
protected:

    BattlegroundMapScript(const char* name, uint32 mapId);

  
};

class ItemScript : public ScriptObject
{
protected:

    ItemScript(const char* name);

public:

    bool IsDatabaseBound() const { return true; }

    // Called when a dummy spell effect is triggered on the item.
    virtual bool OnDummyEffect(Unit* caster, uint32 spellId, uint32 effIndex, Item* target) { return false; }

    // Called when a player accepts a quest from the item.
    virtual bool OnQuestAccept(Player* player, Item* item, Quest const* quest) { return false; }

    // Called when a player uses the item.
    virtual bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) { return false; }

    // Called when the item expires (is destroyed).
    virtual bool OnExpire(Player* player, ItemPrototype const* proto) { return false; }

    // Called when a player selects an option in an item gossip window
    virtual void OnGossipSelect(Player* /*player*/, Item* /*item*/, uint32 /*sender*/, uint32 /*action*/) { }

    // Called when a player selects an option in an item gossip window
    virtual void OnGossipSelectCode(Player* /*player*/, Item* /*item*/, uint32 /*sender*/, uint32 /*action*/, const char* /*code*/) { }
};


class CreatureScript : public ScriptObject, public UpdatableScript<Creature>
{
protected:

    CreatureScript(const char* name);

public:

    bool IsDatabaseBound() const { return true; }

    // Called when a dummy spell effect is triggered on the creature.
    virtual bool OnDummyEffect(Unit* caster, uint32 spellId, uint32 effIndex, Creature* target) { return false; }

    // Called when a player opens a gossip dialog with the creature.
    virtual bool OnGossipHello(Player* player, Creature* creature) { return false; }

    // Called when a player selects a gossip item in the creature's gossip menu.
    virtual bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) { return false; }

    // Called when a player selects a gossip with a code in the creature's gossip menu.
    virtual bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code) { return false; }

    // Called when a player accepts a quest from the creature.
    virtual bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) { return false; }

    // Called when a player selects a quest in the creature's quest menu.
    virtual bool OnQuestSelect(Player* player, Creature* creature, Quest const* quest) { return false; }

    // Called when a player completes a quest with the creature.
    virtual bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest) { return false; }

    // Called when a player selects a quest reward.
    virtual bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 opt) { return false; }

    // Called when the dialog status between a player and the creature is requested.
    virtual uint32 OnDialogStatus(Player* player, Creature* creature) { return 0; }

    // Called when a CreatureAI object is needed for the creature.
    virtual CreatureAI* GetAI(Creature* creature) const { return NULL; }
};

class GameObjectScript : public ScriptObject, public UpdatableScript<GameObject>
{
protected:

    GameObjectScript(const char* name);

public:

    bool IsDatabaseBound() const { return true; }

    // Called when a dummy spell effect is triggered on the gameobject.
    virtual bool OnDummyEffect(Unit* caster, uint32 spellId, uint32 effIndex, GameObject* target) { return false; }

    // Called when a player opens a gossip dialog with the gameobject.
    virtual bool OnGossipHello(Player* player, GameObject* go) { return false; }

    // Called when a player selects a gossip item in the gameobject's gossip menu.
    virtual bool OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action) { return false; }

    // Called when a player selects a gossip with a code in the gameobject's gossip menu.
    virtual bool OnGossipSelectCode(Player* player, GameObject* go, uint32 sender, uint32 action, const char* code) { return false; }

    // Called when a player accepts a quest from the gameobject.
    virtual bool OnQuestAccept(Player* player, GameObject* go, Quest const* quest) { return false; }

    // Called when a player selects a quest reward.
    virtual bool OnQuestReward(Player* player, GameObject* go, Quest const* quest, uint32 opt) { return false; }

    // Called when a player completes a quest with the creature.
    virtual bool OnQuestComplete(Player* player, GameObject* go, Quest const* quest) { return false; }

    // Called when the dialog status between a player and the gameobject is requested.
    virtual uint32 OnDialogStatus(Player* player, GameObject* go) { return 0; }

    // Called when the gameobject is destroyed (destructible buildings only).
    virtual void OnDestroyed(Player* player, GameObject* go, uint32 eventId) { }
};

class AreaTriggerScript : public ScriptObject
{
protected:

    AreaTriggerScript(const char* name);

public:

    bool IsDatabaseBound() const { return true; }

    // Called when the area trigger is activated by a player.
    virtual bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) { return false; }
};

class BGScript : public ScriptObject
{
protected:
    BGScript(const char* name);

public:

    virtual void OnBGAssignTeam(Player* player, Battleground* bg, uint32& team) {}
    virtual void OnPlayerJoinBG(Player* player, Battleground* bg) {}
    virtual void OnPlayerLeaveBG(Player* player, Battleground* bg) {}
};

class BattlegroundScript : public ScriptObject
{
protected:

    BattlegroundScript(const char* name);

public:
    bool IsDatabaseBound() const { return true; }

    // Should return a fully valid BattleGround object for the type ID.
    virtual Battleground* OnGetBattleground() = 0;
};

class OutdoorPvPScript : public ScriptObject
{
protected:

    OutdoorPvPScript(const char* name);

public:

    bool IsDatabaseBound() const { return true; }

    // Should return a fully valid OutdoorPvP object for the type ID.
    virtual OutdoorPvP* OnGetOutdoorPvP() = 0;
};

class CommandScript : public ScriptObject
{
protected:

    CommandScript(const char* name);

public:

    // Should return a pointer to a valid command table (ChatCommand array) to be used by ChatHandler.
    virtual std::vector<ChatCommand> GetCommands() const = 0;
};

class WeatherScript : public ScriptObject, public UpdatableScript<Weather>
{
protected:

    WeatherScript(const char* name);

public:

    bool IsDatabaseBound() const { return true; }

    // Called when the weather changes in the zone this script is associated with.
    virtual void OnChange(Weather* weather, WeatherState state, float grade) { }
};

class AuctionHouseScript : public ScriptObject
{
protected:

    AuctionHouseScript(const char* name);

public:

    // Called when an auction is added to an auction house.
    virtual void OnAuctionAdd(AuctionHouseObject* ah, AuctionEntry* entry) { }

    // Called when an auction is removed from an auction house.
    virtual void OnAuctionRemove(AuctionHouseObject* ah, AuctionEntry* entry) { }

    // Called when an auction was succesfully completed.
    virtual void OnAuctionSuccessful(AuctionHouseObject* ah, AuctionEntry* entry) { }

    // Called when an auction expires.
    virtual void OnAuctionExpire(AuctionHouseObject* ah, AuctionEntry* entry) { }
};

class ConditionScript : public ScriptObject
{
protected:

    ConditionScript(const char* name);
public:

    bool IsDatabaseBound() const { return true; }

    // Called when a single condition is checked for a player.
    bool OnConditionCheck(Condition* condition, Player* player, Unit* targetOverride) { return true; }
};

class DynamicObjectScript : public ScriptObject, public UpdatableScript<DynamicObject>
{
protected:

    DynamicObjectScript(const char* name);
};

class TransportScript : public ScriptObject, public UpdatableScript<Transport>
{
protected:

    TransportScript(const char* name);

public:

    bool IsDatabaseBound() const { return true; }

    // Called when a player boards the transport.
    virtual void OnAddPassenger(Transport* transport, Player* player) { }

    // Called when a creature boards the transport.
    virtual void OnAddCreaturePassenger(Transport* transport, Creature* creature) { }

    // Called when a player exits the transport.
    virtual void OnRemovePassenger(Transport* transport, Player* player) { }

    // Called when a transport moves.
    virtual void OnRelocate(Transport* transport, uint32 waypointId, uint32 mapId, float x, float y, float z) { }
};

class MovementHandlerScript : public ScriptObject
{
protected:
    MovementHandlerScript(const char* name);

public:
    virtual void OnPlayerMove(Player* /*player*/, MovementInfo /*movementInfo*/, uint32 /*opcode*/) {}
};

// Placed here due to ScriptRegistry::AddScript dependency.
#define sScriptMgr (*ACE_Singleton<ScriptMgr, ACE_Null_Mutex>::instance())

// Manages registration, loading, and execution of scripts.
class ScriptMgr
{
    friend class ACE_Singleton<ScriptMgr, ACE_Null_Mutex>;
    friend class ScriptObject;
public:
    ScriptMgr();
    virtual~ScriptMgr();

    uint32 _scriptCount;
    void Unload();

    void LoadDatabase();
    void FillSpellSummary();

public: /* Initialization */

    void ScriptsInit();
    const char* ScriptsVersion() const { return "Integrated Trinity Scripts"; }

    void IncrementScriptCount() { ++_scriptCount; }
    uint32 GetScriptCount() const { return _scriptCount; }

public: /* ServerScript */

    void OnNetworkStart();
    void OnNetworkStop();
    void OnSocketOpen(WorldSocket* socket);
    void OnSocketClose(WorldSocket* socket, bool wasNew);
    void OnPacketReceive(WorldSocket* socket, WorldPacket& packet);
    void OnPacketSend(WorldSocket* socket, WorldPacket& packet);
    void OnUnknownPacketReceive(WorldSocket* socket, WorldPacket& packet);

public: /* WorldScript */

	void OnLoadCustomDatabaseTable();
    void OnOpenStateChange(bool open);
    void OnConfigLoad(bool reload);
    void OnMotdChange(std::string& newMotd);
    void OnShutdown(ShutdownExitCode code, ShutdownMask mask);
    void OnShutdownCancel();
	void OnStartup();
    void OnWorldUpdate(uint32 diff);

public: /* FormulaScript */

    void OnHonorCalculation(float& honor, uint8 level, uint32 count);
    void OnHonorCalculation(uint32& honor, uint8 level, uint32 count);
    void OnGetGrayLevel(uint8& grayLevel, uint8 playerLevel);
    void OnGetColorCode(XPColorChar& color, uint8 playerLevel, uint8 mobLevel);
    void OnGetZeroDifference(uint8& diff, uint8 playerLevel);
    void OnGetBaseGain(uint32& gain, uint8 playerLevel, uint8 mobLevel, ContentLevels content);
    void OnGetGain(uint32& gain, Player* player, Unit* unit);
    void OnGetGroupRate(float& rate, uint32 count, bool isRaid);

public: /* MapScript */

    void OnCreateMap(Map* map);
    void OnDestroyMap(Map* map);
    void OnLoadGridMap(Map* map, uint32 gx, uint32 gy);
    void OnUnloadGridMap(Map* map, uint32 gx, uint32 gy);
    void OnPlayerEnter(Map* map, Player* player);
    void OnPlayerLeave(Map* map, Player* player);
    void OnMapUpdate(Map* map, uint32 diff);

public: /* InstanceMapScript */

    InstanceData* CreateInstanceData(InstanceMap* map);

public: /* ItemScript */

    bool OnDummyEffect(Unit* caster, uint32 spellId, uint32 effIndex, Item* target);
    bool OnQuestAccept(Player* player, Item* item, Quest const* quest);
    bool OnItemUse(Player* player, Item* item, SpellCastTargets const& targets);
    void OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action);
    void OnGossipSelectCode(Player* player, Item* item, uint32 sender, uint32 action, const char* code);


public: /* CreatureScript */

    bool OnDummyEffect(Unit* caster, uint32 spellId, uint32 effIndex, Creature* target);
    bool OnGossipHello(Player* player, Creature* creature);
    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action);
    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code);
    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest);
    bool OnQuestSelect(Player* player, Creature* creature, Quest const* quest);
    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest);
    bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 opt);
    uint32 GetDialogStatus(Player* player, Creature* creature);
    CreatureAI* GetCreatureAI(Creature* creature);
    void OnCreatureUpdate(Creature* creature, uint32 diff);

public: /* GameObjectScript */

    bool OnGossipHello(Player* player, GameObject* go);
    bool OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action);
    bool OnGossipSelectCode(Player* player, GameObject* go, uint32 sender, uint32 action, const char* code);
    bool OnQuestAccept(Player* player, GameObject* go, Quest const* quest);
    bool OnQuestReward(Player* player, GameObject* go, Quest const* quest, uint32 opt);
    bool OnQuestComplete(Player* player, GameObject* gameobject, Quest const* quest);
    uint32 GetDialogStatus(Player* player, GameObject* go);
    void OnGameObjectDestroyed(Player* player, GameObject* go, uint32 eventId);
    void OnGameObjectUpdate(GameObject* go, uint32 diff);

public: /* AreaTriggerScript */

    bool OnTrigger(Player* player, AreaTriggerEntry const* trigger);


public: /* OutdoorPvPScript */


public: /* CommandScript */

    std::vector<ChatCommand> GetChatCommands();

public: /* WeatherScript */


public: /* AuctionHouseScript */

    void OnAuctionAdd(AuctionHouseObject* ah, AuctionEntry* entry);
    void OnAuctionRemove(AuctionHouseObject* ah, AuctionEntry* entry);
    void OnAuctionSuccessful(AuctionHouseObject* ah, AuctionEntry* entry);
    void OnAuctionExpire(AuctionHouseObject* ah, AuctionEntry* entry);
    
public: /* DynamicObjectScript */

    void OnDynamicObjectUpdate(DynamicObject* dynobj, uint32 diff);

public: /* GroupScript */

    void OnGroupAddMember(Group* group, Player* guid);
    void OnGroupInviteMember(Group* group, Player* guid);
    void OnGroupMemberJoin(Group* group, Player* player);
    void OnGroupRemoveMember(Group* group, Player* guid, RemoveMethod method, uint64 kicker, char const* reason);
    void OnGroupChangeLeader(Group* group, Player* newLeaderGuid, Player* oldLeaderGuid);
    void OnGroupCreate(Group* group, Player* leader);
    void OnGroupDisband(Group* group, Player* leader);

public: /* PlayerScript */

    void OnPlayerCompleteQuest(Player* player, Quest const* quest);
    void OnPVPKill(Player* killer, Player* killed);
    void OnCreatureKill(Player* killer, Creature* killed);
    void OnPlayerKilledByCreature(Creature* killer, Player* killed);
    void OnPlayerLevelChanged(Player* player, uint8 oldLevel, uint8 newLevel);
    void OnPlayerFreeTalentPointsChanged(Player* player, uint32 newPoints);
    void OnPlayerTalentsReset(Player* player, bool noCost);
    void OnPlayerMoneyChanged(Player* player, int32& amount);
    void OnPlayerMoneyLimit(Player* player, int32 amount);
    void OnGivePlayerXP(Player* player, uint32& amount, Unit* victim);
    void OnPlayerReputationChange(Player* player, uint32 factionID, int32& standing, bool incremental);
    void OnPlayerDuelRequest(Player* target, Player* challenger);
    void OnPlayerDuelStart(Player* player1, Player* player2);
    void OnPlayerDuelEnd(Player* winner, Player* loser, DuelCompleteType type);
    void OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg);
    void OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg, Player* receiver);
    void OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg, Group* group);
    void OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg, Guild* guild);
    void OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg, Channel* channel);
    void OnPlayerEmote(Player* player, uint32 emote);
    void OnPlayerTextEmote(Player* player, uint32 textEmote, uint32 emoteNum, ObjectGuid guid);
    void OnPlayerSpellCast(Player* player, Spell* spell, bool skipCheck);
    void OnPlayerLogin(Player* player, bool firstLogin);
    void OnPlayerLogout(Player* player);
    void OnPlayerCreate(Player* player);
    void OnPlayerLoadFromDB(Player* player);
    void OnBeforePlayerUpdate(Player* player, uint32 p_time);
    void OnLootMoney(Player* player, uint32 amount);
    void OnLootItem(Player* player, Item* item, uint32 count, uint64 lootGUID);
    void OnCreateItem(Player* player, Item* item, uint32 count);
    void OnQuestRewardItem(Player* player, Item* item, uint32 count);
    void OnPlayerDelete(ObjectGuid guid, uint32 accountId);
    void OnPlayerFailedDelete(ObjectGuid guid, uint32 accountId);
    void OnPlayerSave(Player* player);
    void OnPlayerBindToInstance(Player* player, DungeonDifficulty difficulty, uint32 mapid, bool permanent, uint8 extendState);
    void OnPlayerUpdateZone(Player* player, uint32 newZone, uint32 newArea);
    void OnQuestObjectiveProgress(Player* player, Quest const* quest, uint32 objectiveIndex, uint16 progress);
    void OnQuestStatusChange(Player* player, uint32 questId);
    void OnPlayerRepop(Player* player);
    void OnGossipSelectCode(Player* player, uint32 menu_id, uint32 sender, uint32 action, const char* code);
    void OnGossipSelect(Player* player, uint32 menu_id, uint32 sender, uint32 action);

public: /* TransportScript */

    void OnAddPassenger(Transport* transport, Player* player);
    void OnAddCreaturePassenger(Transport* transport, Creature* creature);
    void OnRemovePassenger(Transport* transport, Player* player);
    void OnTransportUpdate(Transport* transport, uint32 diff);
    void OnRelocate(Transport* transport, uint32 waypointId, uint32 mapId, float x, float y, float z);

public: /*MovementInfo*/

    void OnPlayerMove(Player* player, MovementInfo movementInfo, uint32 opcode);

public: /* BGScript */

    void OnBGAssignTeam(Player* player, Battleground* bg, uint32& team);
    void OnPlayerJoinBG(Player* player, Battleground* bg);
    void OnPlayerLeaveBG(Player* player, Battleground* bg);

public : /* GuildScript*/

    void OnGuildRemoveMember(Guild* guild, Player* player, bool isDisbanding);
    void OnGuildAddMember(Guild* guild, Player* player, uint32& plRank);
    void OnGuildCreate(Guild* guild, Player* leader, const std::string& name);
    void OnGuildDisband(Guild* guild);

public: /* ScriptRegistry */

    // This is the global static registry of scripts.
    template<class TScript> class ScriptRegistry
    {
        // Counter used for code-only scripts.
        static uint32 _scriptIdCounter;

    public:

        typedef std::map<uint32, TScript*> ScriptMap;
        typedef typename ScriptMap::iterator ScriptMapIterator;
        // The actual list of scripts. This will be accessed concurrently, so it must not be modified
        // after server startup.
        static ScriptMap ScriptPointerList;

        // Gets a script by its ID (assigned by ObjectMgr).
        static TScript* GetScriptById(uint32 id)
        {
            for (ScriptMapIterator it = ScriptPointerList.begin(); it != ScriptPointerList.end(); ++it)
                if (it->first == id)
                    return it->second;

            return NULL;
        }

        // Attempts to add a new script to the list.
        static void AddScript(TScript* const script);
    };
};

#endif