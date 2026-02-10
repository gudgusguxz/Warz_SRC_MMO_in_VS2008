// GAMEBLOCKS_BEGIN

#include "GameBlocksWrapper.h"
#include <assert.h>
#include <iostream>
#include "GameSim.h"

#ifdef GAMEBLOCKS_ENABLED
#include <GBClient/Inc/GBClient.h>
#endif

/// [GameBlocks Config]

// game server port, gameblocks server port, and gameblocks server address
// should be configurable either by using GBClient's xml config loading
// LoadConfigFromXml function, or via the game engine's io system.

// The Game Server Port can be used to identify each Game Server GBClient process
// to for example support multiple Game Servers on a single IP address (machine).
#define GB_EXAMPLE_GAME_SERVER_PORT 1000
// NOTE: Change the below to your Title's GameBlocks remote server information in
// order to successfully connect to the Server!
#define GB_EXAMPLE_GAMEBLOCKS_PORT			8740
#define GB_EXAMPLE_GAMEBLOCKS_ADDRESS		"127.0.0.1"
#define GB_EXAMPLE_GAMEBLOCKS_SERVER_URL	"dice.bf4.example.gameblocks.info"
// toggle this to switch between ip and url name above for connection method
//#define GB_EXAMPLE_USE_DIRECT_IP_ADDRESS

/// [GameBlocks Config]

/// [WeaponCheat Config]
// defines how many weapon fire events are stored in the local history
// and sent to GameBlocks.
#define GB_WEAPONCHEAT_SAMPLE_COUNT					25
/// [WeaponCheat Config]

/// [Aimbot Config]
// Although the client passes aimbot data every game server frame,
// we actually choose the rate which the sampling is stored at here.
// These numbers determine the amount of samples stored locally and
// then sent to GameBlocks when sending is triggered.
// The below values match the defaults, but the example uses them
// to illustrate how to manually set the aimbot detector history properties.
#define GB_AIMBOT_SAMPLE_RATE							 0.1f //seconds
#define GB_AIMBOT_SAMPLE_TIME								5  // seconds
/// [Aimbot Config]

namespace FirstPersonShooter {

GameBlocksWrapper * GameBlocksWrapper::s_instance = NULL;

GameBlocksWrapper * GameBlocksWrapper::Instance()
{
	if(s_instance == NULL)
	{
		s_instance = new GameBlocksWrapper();
	}
	return s_instance;
}

GameBlocksWrapper::GameBlocksWrapper(void)
	: m_gameSim(NULL)
{
#ifdef GAMEBLOCKS_ENABLED
	bool bEnabled = true;

	// NOTE - your integration benefits from adding a local data-driven
	// toggle to enable or disable the integration.  For example:
	//GConfig->GetBool("GameBlocks", "bGameBlocksEnabled", bEnabled);

	// NOTE - your integration should also confirm that this is a server instance.
	//bEnabled = bEnabled && IsServer();

	if (bEnabled)
	{
		/// [GBTitleInterface Init]
		m_gbClient = new GameBlocks::GBClient();
		m_gbClient->SetTitleInterface(this);
		/// [GBTitleInterface Init]
	}
	else
	{
		m_gbClient = NULL;
	}
#else
	m_gbClient = NULL;
#endif
}

GameBlocksWrapper::~GameBlocksWrapper(void)
{
#ifdef GAMEBLOCKS_ENABLED
	if (m_gbClient)
	{
		delete m_gbClient;
		m_gbClient = NULL;
	}
#endif
}

void GameBlocksWrapper::SetGameSim(GameSim * newGameSim)
{
#ifdef GAMEBLOCKS_ENABLED
	m_gameSim = newGameSim;
#endif // GAMEBLOCKS_ENABLED
}

// game server port is a useful way to uniquely identify a game server as it is 
// common to run multiple game servers per a single IP address with each 
// game server accepting connections on different ports.
// should be data driven configurable on the game server so that
// it can be updated live if necessary.
int GameBlocksWrapper::GetGameServerPort() const
{
	return GB_EXAMPLE_GAME_SERVER_PORT;
}

// should be data driven configurable on the game server so that
// it can be updated live if necessary.
int GameBlocksWrapper::GetGameBlocksPort() const
{
	return GB_EXAMPLE_GAMEBLOCKS_PORT;
}

// should be data driven configurable on the game server so that
// it can be updated live if necessary.
SimString GameBlocksWrapper::GetGameBlocksAddress() const
{
	return GB_EXAMPLE_GAMEBLOCKS_ADDRESS;
}

SimString GameBlocksWrapper::GetGameBlocksServerUrl() const
{
	return GB_EXAMPLE_GAMEBLOCKS_SERVER_URL;
}

void GameBlocksWrapper::Tick(float deltaTime)
{
	// the client integration is wrapped with a preprocessor toggle,
	// to allow for easy complete removal of the integration code if necessary
	// by the clientside product (e.g. for a game which wishes to compile as
	// a game client only, without server functionality).
#ifdef GAMEBLOCKS_ENABLED
	if(m_gbClient)
	{
		m_gbClient->Tick();
	}
#endif // GAMEBLOCKS_ENABLED
}

int GameBlocksWrapper::IsConnected() const
{
#ifdef GAMEBLOCKS_ENABLED
	if (m_gbClient)
	{
		return (m_gbClient->Connected() == true) ? 1 : 0;
	}
#endif
	return 0;
}

int GameBlocksWrapper::ConnectAsync()
{
#ifdef GAMEBLOCKS_ENABLED
	// EXAMPLE FUNCTIONALITY:
	// Initialize socket subsystem
	// This should not be called if application already initialized sockets. 
	// GameBlocks must be initialized after socket system is initialized.
	m_gbClient->InitSocketsSubsystem();

	/// [Manual Connect]
	// Note that GameBlocks supports ConnectUsingConfig() function which loads
	// via the GBClient's xml loader.  Some games prefer to handle all configuration
	// via their own io config interface.  This example shows how to supply
	// the connection parameters directly in the case that they are being loaded
	// via the game engine's io config interface.
	const int gameBlocksPort = GetGameBlocksPort();
	const int gameServerPort = GetGameServerPort();
#ifdef GB_EXAMPLE_USE_DIRECT_IP_ADDRESS
	const SimString gameBlocksAddress = GetGameBlocksAddress(); // commented out but shows that connecting via direct ip is possible as well
	GameBlocks::EGBClientError result = m_gbClient->Connect(gameBlocksAddress.c_str(), 
		gameBlocksPort, gameServerPort);
#else
	const SimString gameBlocksUrl = GetGameBlocksServerUrl();
	GameBlocks::EGBClientError result = m_gbClient->ConnectUsingUrl(gameBlocksUrl.c_str(), 
		gameBlocksPort, gameServerPort);
	std::cout << "waiting for connect..." << std::endl;
#endif
	const bool connectionInProgress = (result == GameBlocks::GBCLIENT_ERROR_OK);
	/// [Manual Connect]
	if (connectionInProgress)
	{
		/// [Aimbot Enable]
		//  must be called in order to record and send cheat data.
		// call before adding data or sending events.
		m_gbClient->EnableAimBotDetector(GB_AIMBOT_SAMPLE_TIME, GB_AIMBOT_SAMPLE_RATE);
#ifdef VERIFY_GAMEBLOCKS_INTEGRATION
		// SetAimBotDetectorDebugPlayer is used during the final integration verification
		// phase so that the GameBlocks team can verify the incoming aimbot data.
		m_gbClient->SetAimBotDetectorDebugPlayer("1");
#endif
		/// [Aimbot Enable]

		/// [WeaponCheat Enable]
		//  must be called in order to record and send cheat data.
		// call before adding data or sending events.
		m_gbClient->EnableWeaponCheatDetector(GB_WEAPONCHEAT_SAMPLE_COUNT);
		/// [WeaponCheat Enable]
	}
	else
	{
		std::cout << std::endl << "Connect failed!" << std::endl;
	}
	return (connectionInProgress == true) ? 0 : -1;
#endif
	return -1;
}

void GameBlocksWrapper::Cleanup()
{
#ifdef GAMEBLOCKS_ENABLED
	if (IsConnected())
	{
		m_gbClient->Close();
	}
#endif // GAMEBLOCKS_ENABLED
}

void GameBlocksWrapper::PrepareEventForSending(const SimString & eventName,
	const SimPlayerId & playerId)
{
#ifdef GAMEBLOCKS_ENABLED
	if (IsConnected())
	{
		m_gbClient->PrepareEventForSending(eventName.c_str(), NULL, playerId);
	}
#endif
}

// push as many key value pairs as required onto the event
void GameBlocksWrapper::AddKeyValueInt(const SimString & key, int value)
{
#ifdef GAMEBLOCKS_ENABLED
	if (IsConnected())
	{
		m_gbClient->AddKeyValueInt(key.c_str(), value);
	}
#endif
}

void GameBlocksWrapper::AddKeyValueString(const SimString & key, const SimString & value)
{
#ifdef GAMEBLOCKS_ENABLED
	if (IsConnected())
	{
		m_gbClient->AddKeyValueString(key.c_str(), value.c_str());
	}
#endif
}

void GameBlocksWrapper::AddKeyValueFloat(const SimString & key, float value)
{
#ifdef GAMEBLOCKS_ENABLED
	if (IsConnected())
	{
		m_gbClient->AddKeyValueFloat(key.c_str(), value);
	}
#endif
}

void GameBlocksWrapper::AddKeyValueVector3D(const SimString & key, const SimVect & value)
{
#ifdef GAMEBLOCKS_ENABLED
	if (IsConnected())
	{
		m_gbClient->AddKeyValueVector3D(key.c_str(), value.x, value.y, value.z);
	}
#endif
}

bool GameBlocksWrapper::SendEvent()
{
#ifdef GAMEBLOCKS_ENABLED
	if (IsConnected())
	{
		return (m_gbClient->SendEvent() == GameBlocks::GBCLIENT_ERROR_OK);
	}
#endif
	return false;
}

/// [Aimbot Send]
// Game Server must call this when the Aimbot data needs to be sent
// to GameBlocks.  The Title Configuration document supplied with the
// integration package specifies in which event(s) this should be triggered.
void GameBlocksWrapper::SendAimBotDetectorData(const SimPlayerId & aggressorId, 
	const SimPlayerId & targetId)
{
#ifdef GAMEBLOCKS_ENABLED
	if (IsConnected())
	{
		Event_AimBotDetect_Send(m_gbClient, NULL, aggressorId, targetId);
		std::cout 
			<< "GameBlocksWrapper::SendAimBotDetectorData.  Sent AimBot data for killer id: " 
			<< aggressorId 
			<< std::endl;
	}
#endif
}
/// [Aimbot Send]

/// [WeaponCheat Projectile Spawn]
// Called when a weapon projectile is spawned.  Consult with the GameBlocks 
// team regarding which weapons this should be called for.
void GameBlocksWrapper::RegisterProjectileFire(const SimPlayerId & aggressorId,
														int weaponId, int moverType, 
														float camDirX, float camDirY, float camDirZ,
														float projDirX, float projDirY, float projDirZ,
														int shooterPosture, int shooterSpeed, int shooterZoom)
{
#ifdef GAMEBLOCKS_ENABLED
	if (IsConnected())
	{
		m_gbClient->WeaponCheatDetector_AddProjectile(aggressorId, weaponId, 
			moverType, camDirX, camDirY, camDirZ, projDirX, projDirY, projDirZ,
			shooterPosture, shooterSpeed, shooterZoom);
	}
#endif
}
/// [WeaponCheat Projectile Spawn]

/// [WeaponCheat Impact]
// Called when a weapon impacts an object.  Consult with the
// GameBlocks team regarding which weapons this should be called for.
void GameBlocksWrapper::RegisterProjectileImpact(const SimPlayerId & aggressorId,
	int weaponId, int didHit, float dist, int bodyPart, int damage, int impactCount)
{
#ifdef GAMEBLOCKS_ENABLED
	if (IsConnected())
	{
		m_gbClient->WeaponCheatDetector_AddProjImpact(aggressorId, weaponId, 
			(didHit == 1 ? true: false), dist, bodyPart, damage, impactCount);
	}
#endif
}
/// [WeaponCheat Impact]

/// [WeaponCheat Send]
// This triggers local WeaponCheat data to be sent to the GameBlocks server.
// The Title Configuration document supplied with the integration package 
// specifies in which event(s) this should be triggered.
void GameBlocksWrapper::SendWeaponCheatDetectorData(const SimPlayerId & aggressorId)
{
#ifdef GAMEBLOCKS_ENABLED
	if (IsConnected())
	{
		Event_WeaponCheatDetect_Send(m_gbClient, NULL, aggressorId);
		std::cout << "GameBlocksWrapper::SendWeaponCheatDetectorData.  Sent." << std::endl;
	}
#endif
}
/// [WeaponCheat Send]

void GameBlocksWrapper::SetMap(const SimString & mapName)
{
#ifdef GAMEBLOCKS_ENABLED
	if (m_gbClient)
	{
		m_gbClient->Source_SetProperty(NULL, "Map", mapName.c_str());
		std::cout 
			<< "GameBlocksWrapper::SetMap.  Map set: " 
			<< mapName 
			<< std::endl;
	}
#endif
}

void GameBlocksWrapper::SendPlayerJoin(const SimPlayerId & playerId, 
	const SimString & playerName)
{
#ifdef GAMEBLOCKS_ENABLED
	if (m_gbClient)
	{
		Event_PlayerJoin_Send(m_gbClient, NULL, playerId, playerName.c_str(), "0.0.0.0");
		std::cout 
			<< "GameBlocksWrapper::SendPlayerJoin.  Sent join id: " 
			<< playerId 
			<< std::endl;
	}
#endif
}

void GameBlocksWrapper::SendPlayerLeave(const SimPlayerId & playerId, 
	const SimString & playerName)
{
#ifdef GAMEBLOCKS_ENABLED
	if (m_gbClient)
	{
		Event_PlayerLeave_Send(m_gbClient, NULL, playerId, playerName.c_str());
		std::cout 
			<< "GameBlocksWrapper::SendPlayerLeave.  Sent leave id: " 
			<< playerId 
			<< std::endl;
	}
#endif
}

void GameBlocksWrapper::SendChat(const SimPlayerId & playerId, 
	const SimString & message, int isTeamOnly)
{
#ifdef GAMEBLOCKS_ENABLED
	if (m_gbClient)
	{
		const bool bTeamOnly = (isTeamOnly==1)?true:false;
		Event_Chat_Send(m_gbClient, NULL, playerId, message.c_str(), bTeamOnly);
		std::cout 
			<< "GameBlocksWrapper::SendPlayerLeave.  Sent chat id: " 
			<< playerId 
			<< ", message: "
			<< message
			<< std::endl;
	}
#endif
}

void GameBlocksWrapper::SendKeyPress(const SimPlayerId & playerId, int key)
{
#ifdef GAMEBLOCKS_ENABLED
	if (m_gbClient)
	{
		Event_KeyPress_Send(m_gbClient, NULL, playerId, key);
		std::cout 
			<< "GameBlocksWrapper::SendKeyPress.  Sent player id: " 
			<< playerId 
			<< ", key: "
			<< key
			<< std::endl;
	}
#endif
}

void GameBlocksWrapper::SendMatchFinishedEvents(int matchLengthSeconds, int winningTeam)
{
#ifdef GAMEBLOCKS_ENABLED
	if (m_gbClient && m_gameSim)
	{
		Event_MatchResultsList_Prepare(m_gbClient, NULL, matchLengthSeconds, winningTeam);
		for (int i = 0; i < GAMESIM_PLAYER_COUNT; i++)
		{
			const PlayerInfo & info = m_gameSim->players[i];
			const int playerTimeInMatch = 0;
			const int playerKills = 0;
			const int playerDeaths = 0;
			Event_MatchResultsList_Push(
				m_gbClient
				, info.id
				, playerTimeInMatch
				, info.ping
				, playerKills
				, playerDeaths
				, info.team
				, 0 // player score
				, 1 // player global rank
				, 0 // player match rank
				) ;
			Event_MatchResultsList_Send(m_gbClient);

			// example per match custom event which can be used by gameblocks
			// backend to look for unusually high score-per-minute players
			const int playerAvgScorePerMinute = 0; // for the match
			m_gbClient->PrepareEventForSending("ScorePerMinute", NULL, info.id);
			m_gbClient->AddKeyValueInt("Value", playerAvgScorePerMinute);
			m_gbClient->AddKeyValueInt("TimeInMatch", 1); // seconds, how long player was in match for
			m_gbClient->SendEvent();
		}
	}
#endif
}

// GBTitleInterface override
void GameBlocksWrapper::SendPlayerCount()
{
	if (m_gameSim)
	{
		// player count
		const int playerCount = GAMESIM_PLAYER_COUNT;
		GameBlocks::EGBClientError result = Event_PlayerCount_Send(m_gbClient, NULL, playerCount);
		if (result != GameBlocks::GBCLIENT_ERROR_OK)
		{
			std::cout << "GameBlocksWrapper PlayerCount SendEvent Socket Error: " 
				<< m_gbClient->GetLastSocketError() << std::endl;
		}
	}
}

void GameBlocksWrapper::SendMaxPlayerCount()
{
	if (m_gameSim)
	{
		// max player count
		const int maxPlayerCount = GAMESIM_PLAYER_COUNT;
		GameBlocks::EGBClientError result = Event_MaxPlayerCount_Send(m_gbClient, NULL, maxPlayerCount);
		if (result != GameBlocks::GBCLIENT_ERROR_OK)
		{
			std::cout << "GameBlocksWrapper MaxPlayerCount SendEvent Error:  " 
				<< m_gbClient->GetLastSocketError() << std::endl;
		}
	}
}

// GBTitleInterface override: list of active players
void GameBlocksWrapper::SendPlayerList()
{
	if (m_gbClient)
	{
		Event_PlayerList_Prepare(m_gbClient, NULL);

		if (m_gameSim)
		{
			for (int i = 0; i < GAMESIM_PLAYER_COUNT; i++)
			{
				const PlayerInfo & player = m_gameSim->players[i];

				// elapsed seconds
				const int elapsedSinceJoin = (int)(m_gameSim->TimeNow() - player.joinTime);
				Event_PlayerList_Push(m_gbClient, player.id, player.name.c_str(), 
					player.address.c_str(), player.ping, elapsedSinceJoin, player.team);
			}
		}

		const GameBlocks::EGBClientError result = Event_PlayerList_Send(m_gbClient);
		if (result != GameBlocks::GBCLIENT_ERROR_OK)
		{
			std::cout << "GameBlocksWrapper PlayerList SendEvent Error: " 
				<< result << std::endl;
		}
	}
}

// GBTitleInterface override
void GameBlocksWrapper::SendPlayerLocationList()
{
#ifdef GAMEBLOCKS_ENABLED
	if (m_gameSim)
	{
		Event_PlayerLocationList_Prepare(m_gbClient, NULL);

		for (int i = 0; i < GAMESIM_PLAYER_COUNT; i++)
		{
			const PlayerInfo & player = m_gameSim->players[i];

			Event_PlayerLocationList_Push(m_gbClient, player.id, 
				player.locX, player.locY, player.locZ, 
				0.f, 0.f, 0.f, // this sdk example does not move the player, so we will send a zero velocity
				player.moverType, player.moverState);
		}

		GameBlocks::EGBClientError result = Event_PlayerLocationList_Send(m_gbClient);
		if (result != GameBlocks::GBCLIENT_ERROR_OK)
		{
			std::cout << "GameBlocksWrapper PlayerList SendEvent Error: %i" << result << std::endl;
		}
	}
#endif
}

// GBTitleInterface override
void GameBlocksWrapper::ProcessAimbotFrame()
{
	/// [Aimbot Frame]
	// Handles adding all of the player movement information to local aimbot
	// detector storage.
#ifdef GAMEBLOCKS_ENABLED
	if (m_gameSim)
	{
		m_gbClient->AimBotDetector_BeginFrame();
		for (int i = 0; i < GAMESIM_PLAYER_COUNT; i++)
		{
			const PlayerInfo & player = m_gameSim->players[i];

			m_gbClient->AimBotDetector_Add(player.id, 0, player.moverType, player.locX, player.locY, player.locZ,
				player.viewLocX, player.viewLocY, player.viewLocZ,
				player.dirX, player.dirY, player.dirZ);
		} // for each player
		m_gbClient->AimBotDetector_EndFrame();
	}
#endif
	/// [Aimbot Frame]
}

// GBTitleInterface override
void GameBlocksWrapper::HandleActionKick(const GameBlocks::GBPublicPlayerId & playerId, 
	const char * message, float banDaysRemaining)
{
	std::cout << "GameBlocksTitle::HandleActionKick.  Received action from GameBlocks server." << std::endl;
	if (m_gameSim)
	{
		const PlayerInfo * playerInfo = m_gameSim->GetPlayerInfoFromId(playerId);
		if (playerInfo)
		{
			m_gameSim->KickPlayer(*playerInfo, SimString(message), banDaysRemaining);
		}
	}
	std::cout << std::endl;
}

// GBTitleInterface override
void GameBlocksWrapper::HandleActionMessagePlayer(const GameBlocks::GBPublicPlayerId & playerId, 
	const char * message)
{
	std::cout << "GameBlocksTitle::HandleActionMessagePlayer.  Received action from GameBlocks server." << std::endl;
	if (m_gameSim)
	{
		const PlayerInfo * playerInfo = m_gameSim->GetPlayerInfoFromId(playerId);
		if (playerInfo)
		{
			m_gameSim->MessagePlayer(*playerInfo, SimString(message));
		}
	}
	std::cout << std::endl;
}

// GBTitleInterface override
void GameBlocksWrapper::HandleActionGlobalMessage(const char * message)
{
	std::cout << "GameBlocksTitle::HandleActionGlobalMessage.  Received action from GameBlocks server." << std::endl;
	if (m_gameSim)
	{
		m_gameSim->MessageAllPlayers(message);
	}
	std::cout << std::endl;
}

// GBTitleInterface override
void GameBlocksWrapper::HandleActionSendScreenShot(const GameBlocks::GBPublicPlayerId & playerId)
{
	std::cout << "GameBlocksTitle::HandleActionSendScreenShot.  Received action from GameBlocks server." << std::endl;
	if (m_gameSim)
	{
		const PlayerInfo * playerInfo = m_gameSim->GetPlayerInfoFromId(playerId);
		if (playerInfo)
		{
			m_gameSim->SendPlayerScreenshotToGameBlocks(*playerInfo);
		}
	}
	std::cout << std::endl;
}

/// [GBTitleInterface CustomAction]
// GBTitleInterface override
void GameBlocksWrapper::HandleCustomAction()
{
	if (m_gbClient)
	{
		SimString name = m_gbClient->Incoming_GetName();
		std::cout 
			<< "GameBlocksTitle::HandleCustomEvent.  Received custom action \"" 
			<< name 
			<< "\" from GameBlocks server." << std::endl;
		if (name == "ExampleCustomAction")
		{
			float exampleFloat;
			m_gbClient->Incoming_GetPairValueFloat(exampleFloat, "ExampleKeyName");
			m_gameSim->CustomAction(exampleFloat);
		}
	}
	std::cout << std::endl;
}
/// [GBTitleInterface CustomAction]

} // namespace FirstPersonShooter

// GAMEBLOCKS_END
