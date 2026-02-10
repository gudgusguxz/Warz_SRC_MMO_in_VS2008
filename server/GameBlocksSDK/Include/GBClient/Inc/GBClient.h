//=============================================================================
// GBClient.h
// Copyright 2013 GameBlocks LLC, All Rights Reserved.
//=============================================================================

#ifndef INCLUDE_GBCLIENT_H
#define INCLUDE_GBCLIENT_H

#include "GBPublicId.h"
#include "GBClientErrors.h"

// included for ease of integration:
#include <PSShared/Inc/ReservedEventNames.h>
#include <PSShared/Inc/GBKeyCode.h>
#include "GBReservedEvents.h"
#include "GBTitleInterface.h"

/// maximum length of an event pair public API string value
#define GBCLIENT_PAIR_STRING_VALUE_SIZE_MAX 140

#define GB_DllExport   __declspec( dllexport )

namespace NetCore
{
	class Connection;
	class Message;
}

namespace GameBlocks
{

class GBClientImpl;
class GBClientInternalInterface;
class GBEvent;
class GBTitleInterface;

/// @class GBClient
/// Main class used by a game server to connect to and interact with GameBlocks 
/// servers.  This class is not guaranteed to be thread safe.
///
class GBClient
{
public:
	GB_DllExport GBClient();
	GB_DllExport ~GBClient();

	/// Loads a GameBlocks client config xml file from given location.
	/// <p>
	/// See example project Config folders for xml format and settings.  Must be configured
	/// properly for your title's GameBlocks server.  
	/// Call before calling Connect() or behavior is undefined.
	/// @param filename     relative to location of executable which is running the GBClient
	/// @return error code
	/// @see @ref examples_page
	GB_DllExport EGBClientError LoadConfigFromXML(const char * filename);

	/// Wrapper for call to WSAStartup
	/// Normally a game will already have the socket subsystem initialized.
	/// This is provided for test client and other situations where a socket
	/// initialization is not readily available.
	GB_DllExport static void InitSocketsSubsystem();

	///
	/// Connects to the remote GameBlocks server using the connection
	/// data provided via LoadConfigFromXML(...)
	/// <p>
	/// This connection method is recommended to be able to easily reconfigure 
	/// deployed game servers.  Alternatively the game server can use its own
	/// file configuration system to instrument the direct Connect() call.
	///
	/// Enables any other components specified via LoadConfigFromXML.
	///
	/// WINSOCK must be initialized via WSAstartup before calling Connect.  Connect must
	/// must be called before calling Tick() or interacting with incoming/outgoing
	/// events.  If Connect succeeds but later the connection is lost, Ticking the GBClient
	/// will try to automatically reconnect via the reconnect functionality.  Connect 
	/// attempts to create a non blocking, lingering, streaming TCP socket connection.
	/// Call the Client's GetLastSocketError() function to check error codes.
	///
	/// @return error code
	///
	GB_DllExport EGBClientError ConnectUsingConfig();

	///
	/// Connects to the remote GameBlocks server with directly provided connection data.
	/// <p>
	/// Asynchronous.  If using this method it is recommended that the passed in properties
	/// are made available in a configuration file to allow for easy deployed changes.
	///
	/// Enables any other components specified via LoadConfigFromXML.
	///
	/// WINSOCK must be initialized via WSAstartup before calling Connect.  Connect must
	/// must be called before calling Tick() or interacting with incoming/outgoing
	/// events.  If Connect succeeds but later the connection is lost, Ticking the GBClient
	/// will try to automatically reconnect via the reconnect functionality.  Connect 
	/// attempts to create a non blocking, lingering, streaming TCP socket connection.
	/// Call the Client's GetLastSocketError() function to check error codes.
	///
	/// @param gbServerAddress    Address of the remote GameBlocks server.
	/// @param gbServerPort       GameBlocks remote server port.
	/// @param localPort				Outgoing TCP connection port.  Defaults to 0 for 
	///														first available.
	/// @param shouldRetryWithIncrPort If connectPort is greater than zero 
	///														and the connection attempt fails and this 
	///														parameter is set to true, then reconnect attempts 
	///														will increment the given port each attempt until 
	///														the connection passes.
	/// @param sourceId           The identifier for a Game Server Source.  Can
	///                           be set to NULL if only one Source exists per
	///                           ip address which is connected to GameBlocks, or if
	///                           individual unique Source IDs are supplied with 
	///                           each event which is sent to the GameBlocks server.
	/// @return error code
	///
	GB_DllExport EGBClientError Connect(const char * gbServerAddress, 
		unsigned short gbServerPort, 
		const GBPublicSourceId & sourceId = 0, const int localPort = 0, 
		const bool shouldRetryWithIncrPort = false);

	///
	/// Resolves the given Url to an Ip and then calls Connect()
	/// <p>
	/// Asynchronous.  See the documentation for Connect() for more information.
	/// If this function succeeds, then GetConnectionStatus() will return
	/// GB_CONNECT_STATUS_URL_RESOLVE_IN_PROGRESS until either a valid IP is resolved
	/// at which point the GetConnectionStatus() will change to 
	/// GB_CONNECT_STATUS_SOCKET_CONNECTION_IN_PROGRESS, or if the DNS Url name
	/// resolution fails then GetConnectionStatus() will change to
	/// GB_CONNECT_STATUS_DISCONNECTED.
	///
	/// @param gbServerUrl        DNS name of the remote GameBlocks server.
	/// @param gbServerPort       GameBlocks remote server port.
	/// @param sourceId           The identifier for a Game Server Source.  Can
	///                           be set to NULL if only one Source exists per
	///                           ip address which is connected to GameBlocks, or if
	///                           individual unique Source IDs are supplied with 
	///                           each event which is sent to the GameBlocks server.
	/// @param localPort					Outgoing TCP connection port.  Defaults to 0 for 
	///														first available.
	/// @param shouldRetryWithIncrPort If connectPort is greater than zero 
	///														and the connection attempt fails and this 
	///														parameter is set to true, then reconnect attempts 
	///														will increment the given port each attempt until 
	///														the connection passes.
	/// @return error code
	///
	GB_DllExport EGBClientError ConnectUsingUrl(const char * gbServerUrl, 
		unsigned short gbServerPort, 
		const GBPublicSourceId & sourceId = 0, const int localPort = 0, 
		const bool shouldRetryWithIncrPort = false);

	///
	/// Returns the last socket error from any potential socket command
	///
	/// @return 0 if no previous error.  See Windows documentation for WSAGetLastError
	/// error codes for other return types.
	///
	GB_DllExport int GetLastSocketError();

	///
	/// Closes the client connection to the GameBlocks server.
	/// <p>
	/// Do not send or receive events after closing.
	///
	GB_DllExport void Close();

	/// Sets the title interface, the GBClient provides callbacks to the 
	/// title interface for outgoing reserved event processing, reserved event 
	/// sending, and incoming reserved/custom events.
	/// NOTE: If a title interface is provided then the incoming event
	/// queue will be pumped and cleared during tick.  If no title interface
	/// is supplied then the product integration must manage the incoming
	/// event queue.
	/// NOTE: Setting a title interface to null will clear out any timing managed
	/// by the title interface manager.
	/// See the FPS example for a use case.
	GB_DllExport void SetTitleInterface(GBTitleInterface * titleInterface);

	///
	/// Must be called frequently (every game frame if possible) to service
	/// events and the connection to the gameblocks server.  Exits early
	/// if not connected via a call to Connect().
	/// <p>
	///
	GB_DllExport void Tick();

	///
	/// Query whether the connection to the gameblocks server is active and established.
	/// <p>
	///
	/// @return true if connection is active, false if it is closed or pending
	///
	GB_DllExport bool Connected();

	///
	/// Query whether the connection is attempting to open via a non blocking call
	///
	/// @return true if Connect() has been called and a socket connnect is in
	/// progress, false if Connect() was not called, the client has been closed,
	/// or the connect() non blocking attempt failed.
	///
	GB_DllExport bool ConnectionPending();

	///
	/// General Connection status query.  Includes failure status for async
	/// connection failures.
	///
	GB_DllExport EGBConnectStatus GetConnectionStatus() const;

	GB_DllExport void			SetTimeoutPeriod(float timeoutPeriodSeconds);
	GB_DllExport float			GetTimeoutPeriod() const;

	/// Override for default GBTitleInterface PlayerList trigger callback.  
	/// Consult with the GameBlocks team when changing this value.
	/// @param newRate  Must be greater than 0, if not no change occurs to current rate.
	GB_DllExport void SetPlayerListTriggerRate(double newRate);

	/// Override for default GBTitleInterface PlayerLocationList trigger callback.  
	/// Consult with the GameBlocks team when changing this value.
	/// @param newRate  Must be greater than 0, if not no change occurs to current rate.
	GB_DllExport void SetPlayerLocationTriggerRate(double newRate);

	/// Override for default GBTitleInterface Aimbot frame trigger callback.  
	/// Consult with the GameBlocks team when changing this value.
	/// @param newRate  Must be greater than 0, if not no change occurs to current rate.
	GB_DllExport void SetAimbotTriggerRate(double newRate);

	/// returns number of outgoing packets that were sent
	GB_DllExport size_t GetSentOutgoingEventCount() const;

	/// returns number of outgoing packets queued to be sent.  the client must be ticked
	/// for these to be sent
	GB_DllExport size_t GetQueuedOutgoingEventCount() const;


	//----------------------------------------------------------------------------
	// Generic Outgoing Event Interface
	//

	/// Preparing an event clears out any previous key/value pairs which were added for sending.
	/// <p>
	/// To send an event, you must first prepare it, then push on key/value pairs as needed,
  /// then send the event.
	/// For example if a player joins, you can send an event named PlayerJoin with 1 key value
	/// pair:
	///   key: "PlayerName" value: "Player1"
	/// The Source and PlayerIds are packaged in the event.  Events with no pairs are supported.
	/// @param eventName The main name of the event.  Should match an event name supported
	/// by the GameBlocks back end
	/// @param sourceId  Defines which game server this event is originating from for this.  
	///                  NULL will use the default (supplied with the Connect() call).
	/// @param playerId Unique Player identifier for the player target this event
	/// corresponds to.  NULL can be used for events that do not map to a player.
	/// GBClient.  If your title only has one game server per GBClient connection, this
	/// can be set to NULL or 0 to use the connection address information as server identity.
	/// @return error code
	GB_DllExport EGBClientError PrepareEventForSending(const char * eventName, 
		const GBPublicSourceId & sourceId, const GBPublicPlayerId & playerId);

	/// Pushes a string/GBPublicId key value pair onto the prepared event.
	/// Use this when passing WeaponIds or other parameters which use GBPublicId
	/// in the Reserved API.
	/// @param key      pair name.
	/// @param value   value associated with the key. 32 bits.
	/// @return error code
	GB_DllExport EGBClientError AddKeyValuePredefinedId(const char * key, 
		const GBPredefinedId & value);

	/// Pushes an string/int key value pair onto the prepared event.
	/// @param key      pair name.
	/// @param value   value associated with the key
	/// @return error code
	GB_DllExport EGBClientError AddKeyValueInt(const char * key, int value);

	/// Pushes an string/string key value pair onto the prepared event.
	/// Length over GameBlocks::s_clientStringValueSizeMax is truncated.
	/// @param key      pair name.
	/// @param value   value associated with the key
	/// @return error code
	GB_DllExport EGBClientError AddKeyValueString(const char * key, const char * value);

	/// Pushes an string/float key value pair onto the prepared event.
	/// @param key      pair name.
	/// @param value   value associated with the key
	/// @return error code
	GB_DllExport EGBClientError AddKeyValueFloat(const char * key, float value);

	/// Pushes an string/3 dimensional vector key value pair onto the prepared event.
	/// @param key      pair name.
	/// @param f1       e.g. x vector value
	/// @param f2       e.g. y vector value
	/// @param f3       e.g. z vector value
	/// @return error code
	GB_DllExport EGBClientError AddKeyValueVector3D(const char * key, 
		float f1, float f2, float f3);

	/// Pushes an string/2 dimensional vector key value pair onto the prepared event.
	/// @param key      pair name.
	/// @param f1       e.g. x vector value
	/// @param f2       e.g. y vector value
	/// @return error code
	GB_DllExport EGBClientError AddKeyValueVector2D(const char * key, float f1, float f2);

	/// Pushes a string/byte data buffer key value pair onto the prepared event
	/// @param key				pair name
	/// @param data				pointer to the data buffer, buffer contents are copied as part of the call
	/// @param dataNumBytes		Number of bytes in the data buffer
	/// @return error code
	GB_DllExport EGBClientError AddKeyValuePairData(const char * key, const char * data, const size_t dataNumBytes);

	/// Send the prepared event to the server.  Sending the event without
	/// preparing it first results in undefined behavior.  It's ok to send
	/// an event with no pairs.
	/// @return error code
	GB_DllExport EGBClientError SendEvent();

	//----------------------------------------------------------------------------
	// Generic Incoming Event Interface
	// For accessing event data sent to the client from the PSServer.
	//

	/// Deserializes and prepares top message for reading.
	/// <p>
	/// Must be called before getting incoming event properties, or results are undefined
	/// @return false is there are no events to prepare or if the event serialization fails.
	/// @return error code.  Check this to make sure event can be ready from.
	GB_DllExport EGBClientError Incoming_PrepareEventForReading();

	/// Creates and returns a pointer to a copy of the current prepared incoming event
	/// Events returned from this must be cleaned up by the caller
	/// <p>
	/// Must be called after Incoming_PrepareEventForReading called and returned true, otherwise results are undefined
	/// @return Pointer to a GBEvent that's a copy of the currently prepared Incoming event. Null if game blocks not enabled
	GB_DllExport GBEvent * Incoming_CreateEventCopy();

	/// Call after processing an incoming event.
	/// <p>
	/// Must be called to remove top event, to clear it and look at the next event.
	GB_DllExport void			Incoming_PopMessage();

	/// Number of events in the incoming event queue.
	/// <p>
	/// Incoming_PopMessage() must be called to pump this queue.  The queue should
	/// be serviced every frame if possible for best results, or at least once 
	/// per second to provide timely user feedback, and shouldn't be allowed to 
	/// go unserviced for long to avoid a buildup in memory of unserviced incoming
	/// messages.
	/// @return number of available incoming messages
	GB_DllExport size_t			Incoming_EventCount() const;

	/// incoming event name
	/// @return event name
	GB_DllExport const char *	Incoming_GetName() const;

	/// Intended target for this event
	/// <p>
	/// If your GBClient services several Game Servers for example, this will be 
	/// the source ID corresponding to the target Game Server.  
	/// NOTE: The returned reference is valid until Incoming_PopMessage is called.
	/// @return source ID as defined by outgoing events via PrepareEvent.
	GB_DllExport const GBPublicSourceId & Incoming_GetSourceId() const;

	/// get main player that this message relates to
	/// <p>
	/// For actions directed to players, will return the PlayerID which would
	/// be contained in the event.
	/// @return PlayerID as defined by PlayerList and PlayerJoin functionality,
	/// and as defined in events which indicated the special PlayerID pair.
	GB_DllExport const GBPublicPlayerId & Incoming_GetPlayerId() const;

	/// incoming event pair count
	/// @return number of pairs contained in this event
	GB_DllExport int				Incoming_GetNumPairs() const;

	/// check if pair has int type
	/// <p>
	/// For error checking support to ensure event is properly formed.
	/// @param pairIndex index of the pair within the event
	/// @return true if matches expected pair Value type
	GB_DllExport bool			Incoming_HasPairValueInt(int pairIndex) const;

	/// check if pair has Bool type
	/// <p>
	/// For error checking support to ensure event is properly formed.
	/// @param pairIndex index of the pair within the event
	/// @return true if matches expected pair Value type
	GB_DllExport bool			Incoming_HasPairValueFloat(int pairIndex) const;

	/// check if pair has string type
	/// <p>
	/// For error checking support to ensure event is properly formed.
	/// @param pairIndex index of the pair within the event
	/// @return true if matches expected pair Value type
	GB_DllExport bool			Incoming_HasPairValueString(int pairIndex) const;

	/// get pair key Name
	/// @param pairIndex index of the pair within the event
	/// @return pair Name
	GB_DllExport const char *	Incoming_GetPairKey(int pairIndex) const;

	/// get pair Value int
	/// @param val 
	/// @param pairIndex index of the pair within the event
	/// @return error code
	GB_DllExport EGBClientError Incoming_GetPairValueInt(int & val, 
		int pairIndex) const;

	/// get pair Value float
	/// @param val result value
	/// @param pairIndex index of the pair within the event
	/// @return error code
	GB_DllExport EGBClientError Incoming_GetPairValueFloat(/*out*/ float & val, 
		int pairIndex) const;

	/// get pair Value string
	/// Note: The result string will be at most GameBlocks::s_clientStringValueSizeMax in size.
	/// @param buffer string buffer to store result
	/// @param size length of result buffer
	/// @param pairIndex index of the pair within the event
	/// @return error code
	GB_DllExport EGBClientError Incoming_GetPairValueString(/*out*/ char * buffer,
		size_t size, const int pairIndex) const;

	/// get pair Value int by name
	/// @param val 
	/// @param name name of pair key
	/// @return error code
	GB_DllExport EGBClientError Incoming_GetPairValueInt(/*out*/int & val, 
		const char * name) const;

	/// get pair Value float by name
	/// @param val result value
	/// @param name name of pair key
	/// @return error code
	GB_DllExport EGBClientError Incoming_GetPairValueFloat(/*out*/float & val, 
		const char * name) const;

	/// get pair Value string by name
 	/// Note: The result string will be at most GameBlocks::s_clientStringValueSizeMax in size.
	/// @param buffer string buffer to store result
	/// @param size length of result buffer
	/// @param name name of pair key
	/// @return error code
	GB_DllExport EGBClientError	Incoming_GetPairValueString(/*out*/ char * buffer, 
		size_t size, const char * name) const;

	//----------------------------------------------------------------------------
	// AimBot Detector interface

	///
	/// @fn void EnableAimBotDetector();
	/// Explicity Enable the AimBotDetector
	/// <p>
	///
	GB_DllExport void EnableAimBotDetector();

	///
	/// @fn void EnableAimBotDetector(size_t historyTimeLength, float historyRateMax);
	/// Enable the AimBotDetector
	/// <p>
	/// Allows manual configuration and enabling of AimBotDetector when using the
	/// manual Connect call.  Consult your GameBlocks support team for appropriate values.
	/// @param historyTimeLength Length in seconds of per player movement history.
	/// @param historyRateMax Rate in seconds for how often detector will store player frame data
	///
	GB_DllExport void EnableAimBotDetector(size_t historyTimeLength, float historyRateMax);

	/// Explicitly disable the AimBotDetector
	/// <p>
	/// Note that the detector stores internal game state, and it isn't intended
	/// to be toggled off and on with accurate results.  This function is provided for
	/// testing purposes.
	GB_DllExport void AimBotDisableDetector();

	/// Enables sending of Aimbot verification data for a specific player.
	/// <p>
	/// SetAimBotDetectorDebugPlayer is used during the final integration verification
	/// phase so that the GameBlocks team can verify the incoming aimbot data.
	/// This should only be enabled while tuning and verifying the Aimbot integration
	/// with the GameBlocks team.
	GB_DllExport void SetAimBotDetectorDebugPlayer(const GBPublicPlayerId & playerId);

	/// Marks the beginning of a frame for AimBotDetector.
	/// <p>
	/// This must be called once per frame when using the AimBotDetector before
	/// calling AimBotDetector_Add() or AimBotDetector_EndFrame()
	GB_DllExport void AimBotDetector_BeginFrame();

	/// Adds a frame of data for a given player.
	/// <p>
	/// Must be called once per frame, per active player.  Do not call for players
	/// which are not currently spawned or in some other way not part of the 
	/// active game world.  Must be called after AimBotDetector_BeginFrame() 
	/// and before AimBotDetector_EndFrame()
	/// @param playerId  Unique player identifier
	/// @param weaponId  Weapon type. If string is passed, will be hashed to integer. 32 bits.
	/// @param moverType Player mover type. Must correspond to values used with 
	/// Event_PlayerLocationList_Push. If string is passed, will be hashed to integer. 32 bits.
	/// @param locX  Player center X
	/// @param locY  Player center Y
	/// @param locZ  Player center Z
	/// @param viewLocX  Player camera center X
	/// @param viewLocY  Player camera center Y
	/// @param viewLocZ  Player camera center Z
	/// @param dirX  Player camera direction X
	/// @param dirY  Player camera direction Y
	/// @param dirZ  Player camera direction Z
	GB_DllExport void AimBotDetector_Add(const GBPublicPlayerId & playerId, 
		const GBPredefinedId & weaponId, 
		const GBPredefinedId & moverType, 
		float locX, float locY, float locZ,
		float viewLocX,float viewLocY,float viewLocZ,
		float dirX, float dirY, float dirZ);

	/// Marks the end of a frame for AimBotDetector.
	/// <p>
	/// This must be called once per frame when using the AimBotDetector after
	/// calling AimBotDetector_BeginFrame() and AimBotDetector_Add()
	GB_DllExport void AimBotDetector_EndFrame();

	/// Triggers the sending of a packet of AimBotDetector data to the GameBlocks
	/// server.
	/// <p>
	/// This function is used by the helper Event_AimBotDetect_Send() and should
	/// not be called explicitly by the client.
	/// @param sourceId  Unique Source Id
	/// @param playerId  Unique player identifier of aggressor
	/// @param targetId  Unique player identifier for target
	EGBClientError AimBotDetector_Send(const GBPublicSourceId & sourceId, const GBPublicPlayerId & playerId, 
		const GBPublicPlayerId & targetId);

	//----------------------------------------------------------------------------
	// WeaponCheat Detector interface

	///
	/// @fn void EnableWeaponCheatDetector();
	/// Enable the WeaponCheatDetector
	/// <p>
	/// This gets called automatically if the config-based Connect call is used.
	/// This can also be called manually if the manual Connect call is used to
	/// enable the detector using default values.
	///
	GB_DllExport void EnableWeaponCheatDetector();

	///
	/// @fn void EnableWeaponCheatDetector(size_t sampleCount);
	/// Enable the WeaponCheatDetector
	/// <p>
	/// Allows manual configuration and enabling of WeaponCheatDetector when using the
	/// manual Connect call.  Consult your GameBlocks support team for appropriate values.
	/// @param sampleCount Number of weapon events to store and send.
	///
	GB_DllExport void EnableWeaponCheatDetector(size_t sampleCount);

	///
	/// @fn void WeaponCheatDetector_AddProjImpact(const GBPublicPlayerId & playerId, const GBPredefinedId & weaponId, bool didHit, float dist, int bodyPart, int damage, int impactCount);
	/// Add the information for a weapon impact event for processing.  
	/// <p>
	/// Consult with the GameBlocks integration requirements for whether or not 
	/// this should be called. If required, call this every time a weapon 
	/// projectile impact occurs.
	/// @param playerId  Unique player identifier of aggressor
	/// @param weaponId  Unique Weapon Identifier.  Negative values are allowed. 32 bits.
	/// @param didHit  True if weapon hit a target (player or AI), false if weapon
	/// missed (environmental impact or other).
	/// @param dist Distance to impacted object.
	/// @param bodyPart Id of body part which was hit.  -1 if no body part was 
	/// hit. Breaking up your targets into sections identified by bodyPart allows 
	/// for higher fidelity pattern detection based on Aim consistency analysis.
	/// @param damage Amount of damage inflicted by impact.
	/// @param impactCount Number of times, including the current impact, that 
	/// this projectile has impacted objects. Required for projectiles which can 
	/// pass through multiple objects or hit multiple objects at once.
	///
	GB_DllExport void WeaponCheatDetector_AddProjImpact(const GBPublicPlayerId & playerId, 
		const GBPredefinedId & weaponId, bool didHit, float dist, int bodyPart, 
		int damage, int impactCount);

	///
	/// @fn void WeaponCheatDetector_AddProjectile(const GBPublicPlayerId & playerId, const GBPredefinedId & weaponId, const GBPredefinedId & moverType, const float camDirX, const float camDirY, const float camDirZ,	const float projDirX, const float projDirY, const float projDirZ, const GBPredefinedId & shooterPosture, const GBPredefinedId & shooterSpeed, const GBPredefinedId & shooterZoom);
	/// Add the information for a projectile fired event for processing.
	/// <p>
	/// Consult with the GameBlocks integration requirements for whether or not 
	/// this should be called. If required, call this every time a weapon fires
	/// a projectile.
	/// @param playerId  Unique player identifier of aggressor
	/// @param weaponId  Unique Weapon Identifier.  Negative values are allowed. 32 bits.
	/// @param moverType  See player location list moverType. 32 bits.
	/// @param camDirX  Shooter's camera direction X value when the projectile was spawned.
	/// @param camDirY  Shooter's camera direction Y value when the projectile was spawned.
	/// @param camDirZ  Shooter's camera direction Z value when the projectile was spawned.
	/// @param projDirX  Projectile's forward direction vector X value.
	/// @param projDirY  Projectile's forward direction vector Y value.
	/// @param projDirZ  Projectile's forward direction vector Z value.
 	/// @param shooterPosture  Used for players which can be in different posture states, e.g. Stand/crouch/prone. 32 bits.
	/// @param shooterSpeed Player speed state, when applicable, e.g. walk/run. 32 bits.
	/// @param shooterZoom Player zoom state, when applicable, e.g. zoomed in or hip fire. 32 bits.
	///
	GB_DllExport void WeaponCheatDetector_AddProjectile(const GBPublicPlayerId & playerId, 
		const GBPredefinedId & weaponId, const GBPredefinedId & moverType, 
		const float camDirX, const float camDirY, const float camDirZ,
		const float projDirX, const float projDirY, const float projDirZ, 
		const GBPredefinedId & shooterPosture, const GBPredefinedId & shooterSpeed, 
		const GBPredefinedId & shooterZoom);

	/// Triggers the sending of a packet of WeaponCheatDetector data to the GameBlocks
	/// server.
	/// <p>
	/// This function is used by the helper Event_WeaponCheatDetect_Send() and should
	/// not be called explicitly by the client.  Note this function does nothing 
	/// if playerId == targetId.
	/// @param sourceId  Unique Source Id
	/// @param playerId  Unique player identifier of aggressor
	EGBClientError WeaponCheatDetector_Send(const GBPublicSourceId & sourceId, const GBPublicPlayerId & playerId);

	//----------------------------------------------------------------------------
	// Game Server Source Interface

	/// Sets the current value associated with a game server source property.
	///
	/// Necessary for components of GameBlocks which behave differently depending
	/// on the overall mode that a server is in.
	/// Call whenever a server property changes.
	/// @param sourceId  Unique Source Id
	/// @param stateName Name of the state. This function can be called multiple 
	/// times with different states.  Calling with the same stateName more than 
	/// once will update the value to the value contained in the last call.
	/// @param val State value.  Coordinate the value with the GameBlocks team.
	/// Can be used for example to specify Game Server game mode, map name, or 
	/// game tuning settings.  GBSourceProperty accepts many types and is 
	/// saved to a string.
	GB_DllExport EGBClientError Source_SetProperty(const GBPublicSourceId & sourceId, 
		const char * stateName, const GBSourceProperty & val);

	/// Notifies GameBlocks that a source is no longer connected.
	///
	/// This only needs to be called if this GBClient's implementation supports
	/// multiple sources.  This does not need to be called on disconnect -
	/// all sources are cleaned up if the GameBlocks client connection is closed.
	/// @param sourceId  Unique Source Id
	GB_DllExport EGBClientError Source_DidDisconnect(const GBPublicSourceId & sourceId);

	//----------------------------------------------------------------------------
	// For Internal Use

	GB_DllExport GBClientInternalInterface * GetInternalInterface() const;

private:
	GBClientImpl * m_impl;
	GBClientInternalInterface * m_internalInterface;
};

} // namespace gameblocks

#endif // INCLUDE_GBCLIENT_H
