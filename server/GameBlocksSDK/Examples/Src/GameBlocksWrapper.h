// In this example, all portions of the actual GameBlocks integration are
// surrounded by GAMEBLOCKS_BEGIN and GAMEBLOCKS_END comments.  This practice allows
// for easy searching for the integration in the client project source code.
// GAMEBLOCKS_BEGIN

#ifndef GAMEBLOCKS_WRAPPER_H_
#define GAMEBLOCKS_WRAPPER_H_

#include <GBClient/Inc/GBPublicId.h>
#include <GBClient/Inc/GBTitleInterface.h>

// following included for ease of use by any game engine classes using the wrapper
#include <GBClient/Inc/GBReservedEvents.h>
#include <PSShared/Inc/ReservedEventNames.h>

// For this example we're using basic c++ types, but
// this header and the simulation types are hear to point
// out that the integration will require conversion of your project's
// types to the input types required by the GameBlocks API.
#include "GameSimTypes.h"

namespace FirstPersonShooter {

// For example purposes we are providing access to game objects through a GameSim object.
class GameSim;
// And providing a struct with the core player information required by GameBlocks.
struct PlayerInfo;
// And using this GBTitleInterface derived class for easy callback management.
class GameBlocksTitle;

/// [GameBlocksWrapper glue class]
// GameBlocks Wrapper Class
// FirstPersonShooter example class which glues the game layer to the 
// GameBlocks API.
//
// Handles things like:
// 1) An interface using game-specific types, converts game-specific types 
// to and from GameBlocks API types in cpp.
// 2) Processing of incoming events.
// 3) Sending of reserved GameBlocks events which need to be sent periodically.
// 4) Provides an interface for the rest of the game to use for 
// game event-based interaction which needs to trigger GameBlocks events.
// 5) General GameBlocks setup/update/teardown.
// 6) Abstracts the GameBlocks API from the client API to minimize potential
// code base impact due to future API changes.
//
class GameBlocksWrapper: public GameBlocks::GBTitleInterface
/// [GameBlocksWrapper glue class]
{
public:

	//---------------------------------------------------------------------------
	// Outgoing Event Triggers - Inherited from GBTitleInterface

	virtual void SendPlayerCount();

	virtual void SendMaxPlayerCount();

	virtual void SendPlayerList();

	virtual void SendPlayerLocationList();

	virtual void ProcessAimbotFrame();

	//---------------------------------------------------------------------------
	// Incoming Event Handlers- Inherited from GBTitleInterface

	virtual void HandleActionKick(const GameBlocks::GBPublicPlayerId & playerId, 
		const char * message, float banDaysRemaining);

	virtual void HandleActionMessagePlayer(const GameBlocks::GBPublicPlayerId & playerId, 
		const char * message);

	virtual void HandleActionGlobalMessage(const char * message);

	virtual void HandleActionSendScreenShot(const GameBlocks::GBPublicPlayerId & playerId);

	virtual void HandleCustomAction();

//---------------------------------------------------------------------------
// Lifecycle Functionality

	// NOTE: simple example singleton.  not thread-safe.
	static GameBlocksWrapper * Instance();

	// Initializes the gameblocks api and also begins the connection process.
	// returns 0 on success and -1 on failure
	int ConnectAsync();

	// The example GameBlocks glue class needs to be able to access game
	// systems, e.g. player information.  For example purposes SetGameSim is used
	// to provide access to game systems within the cpp.  In a real integration, 
	// the glue cpp file would use whatever Game API is available to retrieve game
	// state information.
	void SetGameSim(GameSim * gameSim);

	// The integration handles throttling of outgoing data.  The GameBlocks client
	// does not provide a timing API, so pass in delta time.
	void Tick(float deltaTime);

	// returns 1 if connected 0 if not
	int IsConnected() const; // not used by the example, however exposed for potential use.

	void Cleanup();

	//---------------------------------------------------------------------------
	// Reserved Event interface

	// sends the event to trigger the cheat detection module to analyze
	// the two given players.  see the GameBlocks documents for usage
	void SendAimBotDetectorData(const SimPlayerId & aggressorId, const SimPlayerId & targetId);

	// May be called per weapon fire for weapon cheat detection.
	// Please consult with the GameBlocks team on which weapon(s) this
	// should be called for.
	void RegisterProjectileFire(const SimPlayerId & aggressorId,
		int weaponId, int moverType, 
		float camDirX, float camDirY, float camDirZ,
		float projDirX, float projDirY, float projDirZ,
		int shooterPosture, int shooterSpeed, int shooterZoom);

	// May be called per weapon impact for weapon cheat detection.
	// Please consult with the GameBlocks team on which weapon(s) this
	// should be called for.
	void RegisterProjectileImpact(const SimPlayerId & aggressorId,
		int weaponId, int didHit, float dist, int bodyPart, int damage, int impactCount);

	void SendWeaponCheatDetectorData(const SimPlayerId & aggressorId);

	void SetMap(const SimString & mapName);

	void SendPlayerJoin(const SimPlayerId & playerId, const SimString & playerName);
	void SendPlayerLeave(const SimPlayerId & playerId, const SimString & playerName);

	void SendChat(const SimPlayerId & playerId, const SimString & message, int isTeamOnly);

	void SendKeyPress(const SimPlayerId & playerId, int key);

	void SendMatchFinishedEvents(int matchLengthSeconds, int winningTeam);

	//---------------------------------------------------------------------------
	// Generic Event Sending interface

	void PrepareEventForSending(const SimString & eventName, const SimPlayerId & playerId);

	// push as many key value pairs as required onto the event
	void AddKeyValueInt(const SimString & key, int value);
	void AddKeyValueString(const SimString & key, const SimString & value);
	void AddKeyValueFloat(const SimString & key, float value);
	void AddKeyValueVector3D(const SimString & key, const SimVect & value);

	bool SendEvent();

//---------------------------------------------------------------------------
private:
	GameBlocksWrapper();
	~GameBlocksWrapper();

	// example singleton
	static GameBlocksWrapper *		s_instance;

	// If multiple GameBlocks clients are being run on one box
	// and thus have the same ip address, an easy way to identify each Game
	// Server/ is to use the individual Game Server ports.  These get sent
	// to GameBlocks as Source Ids.
	int GetGameServerPort() const;
	int GetGameBlocksPort() const;
	SimString GetGameBlocksAddress() const;
	SimString GetGameBlocksServerUrl() const;

	// a ref to game simulation so that we can retrieve game information
	// and make callbacks for game actions.
	GameSim * m_gameSim;

	// the actual API client
	GameBlocks::GBClient * m_gbClient;
};

} // namespace FirstPersonShooter

#endif // #define GAMEBLOCKS_WRAPPER_H_

// GAMEBLOCKS_END