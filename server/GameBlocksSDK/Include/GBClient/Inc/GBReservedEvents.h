//=============================================================================
// GBClient.h
// Copyright 2013 GameBlocks LLC, All Rights Reserved.
//=============================================================================

#ifndef RESERVEDEVENTS_H_
#define RESERVEDEVENTS_H_

#include "GBPublicId.h"
#include "GBClientErrors.h"

/// @file GBReservedEvents.h
/// Definitions for all events which are required for core GameBlocks
/// functionality.  The following events must be sent by your
/// client or behavior of the GameBlocks back end is undefined.
///
/// ****NOTE**** View the GameBlocks Namespace in the documentation
/// to view the documentation for the below functions.

/// @namespace GameBlocks All GBClient functionality is wrapped in this namespace.
namespace GameBlocks
{

#ifndef DllExport
#define DllExport   __declspec(dllexport)
#endif

class GBClient;

/// @fn EGBClientError Event_PlayerList_Prepare(GBClient * gbClient, const GBPublicSourceId & sourceId);
/// PlayerList must be sent periodically.
/// This event is verbose, and does not need to be sent every frame.
/// It updates GameBlocks on overall game state.  Every 30 seconds is a recommended rate.
/// PlayerList is used by the rule system to identify
/// player state in the game and is used by several core
/// components.  Prepare must be called before Push or
/// send.
/// @param gbClient     a connected GameBlocks client
/// @param sourceId     identifier for game server which this event is for
/// @return error code
DllExport EGBClientError Event_PlayerList_Prepare(GBClient * gbClient, const GBPublicSourceId & sourceId);

///
/// @fn EGBClientError Event_PlayerList_Push(GBClient * gbClient, const GBPublicPlayerId & playerId, const char * playerName, const char * address, const int ping, const int elapsedSinceJoin, const int team);
/// <p>
/// Call this function once for each active player.  Call Event_PlayerList_Prepare
/// before pushing players.  Call Event_PlayerListSend after finishing pushing
/// all active players.
/// @param gbClient     the GameBlocks client interface object for your implementation.
/// must be valid and should be connected.
/// @param playerId the game's unique player ID which persists across sessions
/// @param playerName a friendly name for the player, e.g. current name in game,
/// does not need to be unique.
/// @param address  ip address
/// @param ping     ping from player client to game server.  can be arbitrary.
/// @param elapsedSinceJoin time in seconds since joining the game server
/// @param team     unique team number identifier
/// @return error code
/// @see Event_PlayerList_Prepare
///
DllExport EGBClientError Event_PlayerList_Push(GBClient * gbClient, const GBPublicPlayerId & playerId, 
    const char * playerName, const char * address, const int ping, 
    const int elapsedSinceJoin, const int team);

///
/// @fn EGBClientError Event_PlayerList_Send(GBClient * gbClient);
/// Finalizes and sends the PlayerList
/// @param gbClient     the GameBlocks client interface object for your implementation.
/// must be valid and should be connected.
/// @return error code
///
DllExport EGBClientError Event_PlayerList_Send(GBClient * gbClient);

///
/// @fn EGBClientError Event_PlayerCount_Send(GBClient * gbClient, const GBPublicSourceId & sourceId, const int playerCount);
/// Send at the same frequency with your PlayerList e.g. lower frequency
/// @param gbClient     the GameBlocks client interface object for your implementation.
/// must be valid and should be connected.
/// @param sourceId     identifier for game server which this event is for
/// @param playerCount		current number of players allowed on this server
/// @return error code
///
DllExport EGBClientError Event_PlayerCount_Send(GBClient * gbClient, const GBPublicSourceId & sourceId, 
    const int playerCount);

///
/// @fn EGBClientError Event_MaxPlayerCount_Send(GBClient * gbClient, const GBPublicSourceId & sourceId, const int maxPlayerCount);
/// Send at the same time as your Event_PlayerCount_Send
/// @param gbClient					the GameBlocks client interface object for your implementation.
/// must be valid and should be connected.
/// @param sourceId					identifier for game server which this event is for
/// @param maxPlayerCount		maximum number of players allowed on this server
/// @return error code
///
DllExport EGBClientError Event_MaxPlayerCount_Send(GBClient * gbClient, const GBPublicSourceId & sourceId, 
    const int maxPlayerCount);

///
/// @fn EGBClientError Event_GameVersion_Send(GBClient * gbClient, const GBPublicSourceId & sourceId, const char * version);
/// Useful event to keep in the database to identify the version number of the game installation
/// @param gbClient     the GameBlocks client interface object for your implementation.
/// must be valid and should be connected.
/// @param sourceId     identifier for game server which this event is for
/// @param version server build version number
/// @return error code
///
DllExport EGBClientError Event_GameVersion_Send(GBClient * gbClient, const GBPublicSourceId & sourceId, 
    const char * version);

///
/// @fn EGBClientError Event_PlayerJoin_Send(GBClient * gbClient, const GBPublicSourceId & sourceId, 
/// const GBPublicPlayerId & playerId, const char * playerName);
/// Ensure this is sent whenever a connection to a new player is made to the
/// game server and ensure it is always matched by a call to Event_PlayerLeave_Send.
/// @param gbClient     the GameBlocks client interface object for your implementation.
/// must be valid and should be connected.
/// @param sourceId     identifier for game server which this event is for
/// @param playerId			identifier for joining player
/// @param playerName   player name for joining player.  for informational use, does not need to be unique.
/// @param address			ip address
/// @return error code
///
DllExport EGBClientError Event_PlayerJoin_Send(GBClient * gbClient, const GBPublicSourceId & sourceId, 
    const GBPublicPlayerId & playerId, const char * playerName, const char * playerAddress);

///
/// @fn EGBClientError Event_AimBotDetect_Send(GBClient * gbClient, const GBPublicSourceId & sourceId, 
/// const GBPublicPlayerId & playerId, const GBPublicPlayerId & targetId);
/// Call whenever a player does something against another player which should
/// trigger a cheat  detection pattern analysis on the player's behavior.  This
/// is intensive.  Please consult your GameBlocks support team on maximum call rate.
/// Please consult GameBlocks team on the types of game events should trigger
/// this, and which types of players to send along with this event.
/// @param gbClient     the GameBlocks client interface object for your implementation.
/// must be valid and should be connected.
/// @param sourceId     identifier for game server which this event is for
/// @param playerId     player ID to analyze
/// @param targetId     ID of a target player related to the given playerId.
///                     This player should be related in a consistent relevant
///                     gameplay scenario to playerId, such as a player which
///                     was just killed by playerId.
/// @return error code
///
DllExport EGBClientError Event_AimBotDetect_Send(GBClient * gbClient, const GBPublicSourceId & sourceId, 
	const GBPublicPlayerId & playerId, const GBPublicPlayerId & targetId);

///
/// @fn EGBClientError Event_WeaponCheatDetect_Send(GBClient * gbClient, const GBPublicSourceId & sourceId, 
/// const GBPublicPlayerId & playerId);
/// Call whenever a player does something against another player which should
/// trigger a cheat  detection pattern analysis on the player's behavior.  This
/// is intensive.  Please consult your GameBlocks support team on maximum call rate.
/// Please consult GameBlocks team on the types of game events should trigger
/// this, and which types of players to send along with this event.
/// @param gbClient     the GameBlocks client interface object for your implementation.
/// must be valid and should be connected.
/// @param sourceId     identifier for game server which this event is for
/// @param playerId     player ID to analyze
/// @return error code
///
DllExport EGBClientError Event_WeaponCheatDetect_Send(GBClient * gbClient, const GBPublicSourceId & sourceId, 
	const GBPublicPlayerId & playerId);

///
/// @fn EGBClientError Event_PlayerLeave_Send(GBClient * gbClient, const GBPublicSourceId & sourceId
/// , const GBPublicPlayerId & playerId, const char * playerName);
/// Ensure this is always sent when a player officially has left a server
/// for any reason (quit, kick, disconnect, crash, etc - when the connection
/// to the game server is closed).
/// @param gbClient     the GameBlocks client interface object for your implementation.
/// must be valid and should be connected.
/// @param sourceId     identifier for game server which this event is for
/// @param playerId     identifier for leaving player
/// @param playerName   player name for leaving player.  for informational use, does not need to be unique.
/// @return error code
///
DllExport EGBClientError Event_PlayerLeave_Send(GBClient * gbClient, const GBPublicSourceId & sourceId
    , const GBPublicPlayerId & playerId, const char * playerName);

///
/// @fn EGBClientError Event_PlayerLocationList_Prepare(GBClient * gbClient, const GBPublicSourceId & sourceId);
/// This event is verbose and does not need to be sent every frame.
/// Once per second is a recommended rate.
/// Prepare must be called before Event_PlayerLocationList_Push() or Event_PlayerLocationList_Send().
/// @param gbClient     a connected GameBlocks client
/// @param sourceId     identifier for game server which this event is for
/// @return error code
///
DllExport EGBClientError Event_PlayerLocationList_Prepare(GBClient * gbClient, const GBPublicSourceId & sourceId);

///
/// @fn EGBClientError Event_PlayerLocationList_Push(GBClient * gbClient, const GBPublicPlayerId & playerId, float xLoc, float yLoc, float zLoc, int moveState);
/// <p>
/// Call this function once for each active player (i.e. a spawned, alive player).  
/// Call Event_PlayerLocationList_Prepare before pushing player locations.  
/// Call Event_PlayerLocationListSend after finishing pushing all active players.
/// @param gbClient     the GameBlocks client interface object for your implementation.
/// must be valid and should be connected.
/// @param playerId the game's unique player ID which persists across sessions
/// @param xLoc  player x location
/// @param yLoc  player y location
/// @param zLoc  player z location
/// @param xLoc  player x instantaneous velocity
/// @param yLoc  player y instantaneous velocity
/// @param zLoc  player z instantaneous velocity
/// @param moverType Type of object which is moving.  E.g. infantry, tank, 
///                  helicopter etc.  Default is 0.  Consult the GameBlocks team 
///                  for additional functionality for your title. If a string is 
///                  passed, will be hashed to an integer. 32 bits.
/// @param moveState Type of movement or state while moving.  Default is 0. Consult the GameBlocks team for  additional functionality for your title.
/// @return error code
///
DllExport EGBClientError Event_PlayerLocationList_Push(GBClient * gbClient, 
  const GBPublicPlayerId & playerId, 
	float xLoc, float yLoc, float zLoc, 
	float xVel, float yVel, float zVel, 
	const GBPredefinedId & moverType, int moveState);

///
/// @fn bool Event_PlayerLocationList_Send(GBClient * gbClient);
/// Finalizes and sends the PlayerLocationList.
/// @param gbClient     the GameBlocks client interface object for your implementation.
/// must be valid and should be connected.
/// @return error code
///
DllExport EGBClientError Event_PlayerLocationList_Send(GBClient * gbClient);

///
/// @fn DllExport EGBClientError Event_PlayerScreenShotJpg_Send(GBClient* gbClient, const GBPublicSourceId & sourceId, const GBPublicPlayerId & playerId, const char* imageData, const size_t imageDataNumBytes );
/// Sends jpg image data to GameBlocks servers.  The image data is limited to 155kb in size. 
/// @param gbClient				The GameBlocks client interface object for your implementation.  Must be valid and should be connected.
/// @param sourceId				Identifier for game server which this event is for
/// @param playerId				The game's unique player ID which persists across sessions
/// @param imageData			Pointer to the jpg image binary data
/// @param imageDataNumBytes	Byte length of the image binary data
/// @return error code
///
DllExport EGBClientError Event_PlayerScreenShotJpg_Send(GBClient* gbClient, const GBPublicSourceId & sourceId, const GBPublicPlayerId & playerId,
	const char* imageData, const size_t imageDataNumBytes );

///
/// @fn DllExport EGBClientError Event_Chat_Send(GBClient* gbClient, const GBPublicSourceId & sourceId, const GBPublicPlayerId & playerId, const char* message, bool isTeamOnly);
/// Sends chat information.
/// @param gbClient				The GameBlocks client interface object for your implementation.  Must be valid and should be connected.
/// @param sourceId				Identifier for game server which this event is for
/// @param playerId				The game's unique player ID which persists across sessions
/// @param message				Chat message contents.
/// @param isTeamOnly			True if only members on the same team can view, false if public chat.
/// @return error code
///
DllExport EGBClientError Event_Chat_Send(GBClient* gbClient, const GBPublicSourceId & sourceId, const GBPublicPlayerId & playerId,
	const char* message, bool isTeamOnly);

/// @fn EGBClientError Event_MatchResultsList_Prepare(GBClient * gbClient, const GBPublicSourceId & sourceId, unsigned int matchLengthInSeconds);
/// Match results should be sent when a match is finished.
/// @param gbClient     a connected GameBlocks client
/// @param sourceId     identifier for game server which this event is for
/// @param matchLengthInSeconds  how long the match lasted
/// @param winningTeam team id which won the match. set to -1 if tie.
/// @return error code
DllExport EGBClientError Event_MatchResultsList_Prepare(GBClient * gbClient, 
	const GBPublicSourceId & sourceId, int matchLengthInSeconds, 
	int winningTeam);

///
/// @fn EGBClientError Event_MatchResultsList_Push(GBClient * gbClient, const GBPublicPlayerId & playerId, unsigned int playerTimeInSeconds, int playerPing, int playerKills, int playerDeaths, int playerTeam,int playerScore,int playerGlobalRank,int playerMatchRank);
/// <p>
/// Call this function once for each active player.  Call Event_MatchResultsList_Prepare
/// before pushing players.  Call Event_PlayerListSend after finishing pushing
/// all active players.
/// @param gbClient     the GameBlocks client interface object for your implementation.
/// must be valid and should be connected.
/// @param playerId the game's unique player ID which persists across sessions
/// @param playerTimeInSeconds how long player was in match in seconds.
/// @param playerPing in ms
/// @param playerKills kill count for the match
/// @param playerDeaths death count for the match
/// @param playerTeam team at end of match
/// @param playerScore match score
/// @param playerGlobalRank global rank value
/// @param playerMatchRank match rank value.  duplicates can exist in the list, e.g. if rank is team specific.
/// @return error code
/// @see Event_PlayerList_Prepare
///
DllExport EGBClientError Event_MatchResultsList_Push(GBClient * gbClient, 
	const GBPublicPlayerId & playerId, 
	int playerTimeInSeconds,
	int playerPing,
	int playerKills,
	int playerDeaths,
	int playerTeam,
	int playerScore,
	int playerGlobalRank,
	int playerMatchRank
	);

///
/// @fn EGBClientError Event_MatchResultsList_Send(GBClient * gbClient);
/// Finalizes and sends the MatchResultsList
/// @param gbClient     the GameBlocks client interface object for your implementation.
/// must be valid and should be connected.
/// @return error code
///
DllExport EGBClientError Event_MatchResultsList_Send(GBClient * gbClient);

///
/// @fn EGBClientError Event_KeyPress_Send(GBClient * gbClient, 	const GBPublicSourceId & sourceId, const GBPublicPlayerId & playerId, int keyCode);
/// Registers a specific key being pressed.
/// To minimize event volume, only send for specific keys in specific
/// situations relevant to goals.
/// @param gbClient     the GameBlocks client interface object for your implementation.
/// must be valid and should be connected.
/// @param sourceId				Identifier for game server which this event is for
/// @param playerId				The game's unique player ID which persists across sessions
/// @param keyCode				The virtual GameBlocks::GB_VK_* keycode enum value corresponding to the physical key being pressed on the Player Client's keyboard.
/// @return error code
///
DllExport EGBClientError Event_KeyPress_Send(GBClient * gbClient, 
	const GBPublicSourceId & sourceId, 
	const GBPublicPlayerId & playerId, 
	int keyCode);

} // namespace gameblocks

#endif // GBRESERVEDEVENTS_H_