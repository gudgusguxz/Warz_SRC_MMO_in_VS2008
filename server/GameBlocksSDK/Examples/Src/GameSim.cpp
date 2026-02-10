#include "GameSim.h"

#include <iostream>

// GAMEBLOCKS_BEGIN
#include "GameBlocksWrapper.h"
#include <PSShared/Inc/GBKeyCode.h>
// GAMEBLOCKS_END

namespace FirstPersonShooter {

SimPlayerId PlayerInfo::playerIdCounter = 1;

GameSim::GameSim()
{
	Reset();
}

void GameSim::Reset()
{
	timeNow = 60;
	state = 0;
	didMessagePlayer = 0;
	didKickPlayer = false;
	didGlobalMessage = false;
	didSendScreenshot = false;
	didCustomAction = false;
}

time_t GameSim::TimeNow()
{
	return timeNow++;
}

const PlayerInfo * GameSim::GetPlayerInfoFromId(const SimPlayerId & id) const
{
#ifdef GAMEBLOCKS_ENABLED
	for (int i = 0; i < GAMESIM_PLAYER_COUNT; i++)
	{
		if (players[i].id == id)
		{
			return &players[i];
		}
	}
#endif
	return NULL;
}

void GameSim::JoinAllPlayers()
{
	for (int i = 0; i < GAMESIM_PLAYER_COUNT; i++)
	{
		HandleJoin(players[i]);
	}
}

void GameSim::LeaveAllPlayers()
{
	for (int i = 0; i < GAMESIM_PLAYER_COUNT; i++)
	{
		HandleLeave(players[i]);
	}
}

bool GameSim::Tick(float deltaTime)
{
	// game sim is finished if it received all responses for the
	// example
	if (didGlobalMessage 
		&& didKickPlayer 
		&& didMessagePlayer 
		&& didSendScreenshot 
		&& didCustomAction)
	{
		return false;
	}

	const int randId = rand() % GAMESIM_PLAYER_COUNT;
	int randNextId = randId + 1;
	if (randNextId == GAMESIM_PLAYER_COUNT)
	{
		randNextId = 0;
	}

	// simple example, we generate various game events by calling the event
	// handlers over a series of frames, and then we're done.
	switch (state)
	{
	case 0:
		JoinAllPlayers();
		break;
	case 1:
		HandlePlayerSpawn(players[randId]);
		break;
	case 2:
		HandleLeave(players[randId]);
		HandleJoin(players[randId]);
		break;
	case 3:
		HandlePlayerSpotted(players[randId], players[randNextId]);
		break;
	case 4:
		HandleKeyPressed(players[randId], GameBlocks::GB_VK_INSERT);
		break;
	case 5:
		HandleProjectileImpact(players[randId], 0, rand() % 2, 100.f, 0, 100, 1);
		break;
	case 6:
		{
			const PlayerInfo & victim = players[randNextId];
			HandleKill(players[randId], victim, 50, 0, 0, 0, 0, 10.f, 0);
			HandleChat(players[randId], "I killed someone!", 0);
			HandleMatchFinished(
				100, // match length in seconds
				1 // winning team id
				);
		}
		break;
	default:
		break;
	}
	state++;
	return true;
}

void GameSim::MessagePlayer(const PlayerInfo & player, const SimString & message)
{
	std::cout << "GameSim::MessagePlayer - game message a player here and display the following message: \""
		<< message << "\"" << std::endl;
	didMessagePlayer = true;
}

void GameSim::KickPlayer(const PlayerInfo & player, const SimString & reason, float bandDays)
{
	std::cout << "GameSim::KickPlayer - game would kick a player here and display the following message: \""
		<< reason << "\"" << std::endl;
	didKickPlayer = true;
}

void GameSim::MessageAllPlayers(const SimString & message)
{
	std::cout << "GameSim::MessageAllPlayers - game would message all player here and display the following message: \""
		<< message << "\"" << std::endl;
	didGlobalMessage = true;
}

void GameSim::KillPlayer(const PlayerInfo & player, const SimString & reason)
{
	std::cout << "GameSim::KillPlayer - game would kill a player here and display the following message: \""
		<< reason << "\"" << std::endl;
}

void GameSim::SendPlayerScreenshotToGameBlocks(const PlayerInfo & player)
{
	std::cout << "GameSim::SendPlayerScreenshotToGameBlocks - game server would notify "
		"a player client that it must send a screenshot to the game server which the "
		"game server would then send back to GameBlocks using the reserved "
		"screenshot event function Event_PlayerScreenShotJpg_Send" << std::endl;
	didSendScreenshot = true;
}

void GameSim::CustomAction(float val)
{
	std::cout 
		<< "GameSim::CustomAction - example function where game would do something to player with given value." 
		<< std::endl;
	didCustomAction = true;
}

void GameSim::HandleKill(const PlayerInfo & aggressor, 
	const PlayerInfo & victim, int prevHealth, int bodyPart, int wasTeamKill,
	int weaponType, int wasKillerZoomed, float dist, int wasVictimSpotted)
{
	// GAMEBLOCKS_BEGIN
	GameBlocksWrapper * wrapper = GameBlocksWrapper::Instance();

	// Send Kill Event

	wrapper->PrepareEventForSending("Kill", aggressor.id);

	// The Kill event requires a string ID for the Victim.
	// We can use the public player id type to convert from our id type to a string.
	wrapper->AddKeyValueString("VictimId", GameBlocks::GBPublicPlayerId(victim.id).c_str());
	wrapper->AddKeyValueInt("VictimType", victim.moverType);
	wrapper->AddKeyValueInt("VictimHealth", prevHealth);
	wrapper->AddKeyValueInt("BodyPart", bodyPart);
	wrapper->AddKeyValueInt("TeamKill", wasTeamKill);
	wrapper->AddKeyValueInt("WeaponType", weaponType);
	wrapper->AddKeyValueInt("Zoomed", wasKillerZoomed);
	wrapper->AddKeyValueFloat("Distance", dist);
	wrapper->AddKeyValueInt("wasVictimSpotted", wasVictimSpotted);

	wrapper->SendEvent();

	// Send Aimbot Detector Data Event

	// current practice is to send aim bot data per kill, since the period
	// of time directly before and up to a kill is going to be most likely to contain
	// aimbot-using patterns.
	wrapper->SendAimBotDetectorData(aggressor.id, victim.id);

	// same with weapon cheat info
	wrapper->SendWeaponCheatDetectorData(aggressor.id);
	// GAMEBLOCKS_END
}

void GameSim::HandleJoin(const PlayerInfo & player)
{
	// GAMEBLOCKS_BEGIN
	GameBlocksWrapper * wrapper = GameBlocksWrapper::Instance();
	wrapper->SendPlayerJoin(player.id, player.name);
	// GAMEBLOCKS_END
}

void GameSim::HandleLeave(const PlayerInfo & player)
{
	// GAMEBLOCKS_BEGIN
	GameBlocksWrapper * wrapper = GameBlocksWrapper::Instance();
	wrapper->SendPlayerLeave(player.id, player.name);
	// GAMEBLOCKS_END
}

void GameSim::HandleProjectileFired(const PlayerInfo & player, int weaponId, 
													 float projDirX, float projDirY, float projDirZ)
{
	// GAMEBLOCKS_BEGIN
	GameBlocksWrapper * wrapper = GameBlocksWrapper::Instance();
	wrapper->RegisterProjectileFire(player.id, weaponId, player.moverType,
		0.f, 0.f, 1.f, // camera direction stub
		0.f, 1.f, 0.f, // projectile direction stub data, 90 degree deviation
		1, // posture e.g. standing, prone, crouch
		2, // speed stub e.g. running walking
		3 // zoom state e.g. iron sight zoom, or hip fire
		);
	// GAMEBLOCKS_END
}

void GameSim::HandleProjectileImpact(const PlayerInfo & player, int weaponId, 
	int didHit, float dist, int bodyPart, int damage, int impactCount)
{
	// GAMEBLOCKS_BEGIN
	GameBlocksWrapper * wrapper = GameBlocksWrapper::Instance();
	wrapper->RegisterProjectileImpact(player.id, weaponId, didHit, dist, bodyPart, damage, impactCount);
	// GAMEBLOCKS_END
}

void GameSim::HandlePlayerSpotted( const PlayerInfo & spotter, const PlayerInfo & spotted)
{
	// GAMEBLOCKS_BEGIN
	GameBlocksWrapper * wrapper = GameBlocksWrapper::Instance();
	wrapper->PrepareEventForSending("PlayerSpotted", spotter.id);
	wrapper->AddKeyValueString("SpottedId", GameBlocks::GBPublicPlayerId(spotted.id).c_str());
	wrapper->AddKeyValueInt("MoverType", spotted.moverType);
	wrapper->SendEvent();
	// GAMEBLOCKS_END
}

void GameSim::HandlePlayerSpawn(const PlayerInfo & player)
{
	// GAMEBLOCKS_BEGIN
	GameBlocksWrapper * wrapper = GameBlocksWrapper::Instance();
	wrapper->PrepareEventForSending("PlayerSpawn", player.id);
	wrapper->AddKeyValueInt("Team", player.team);
	wrapper->SendEvent();
	// GAMEBLOCKS_END
}

void GameSim::HandleChat(const PlayerInfo & player, 
	const SimString & message, int isTeamOnly)
{
	// GAMEBLOCKS_BEGIN
	GameBlocksWrapper * wrapper = GameBlocksWrapper::Instance();
	wrapper->SendChat(player.id, message, isTeamOnly);
	// GAMEBLOCKS_END
}

void GameSim::HandleKeyPressed(const PlayerInfo & player, int key)
{
	// GAMEBLOCKS_BEGIN
	GameBlocksWrapper * wrapper = GameBlocksWrapper::Instance();

	// for example purposes, using direct gameblocks key code value.
	// the actual game integration will have to ensure that virtual keys are mapped
	// to the EGBKeyCode values.  See the API's GBKeyCode.h.
	const int gameSimSuspiciousKey = GameBlocks::GB_VK_INSERT;

	// Some games may send certain key presses which can support other data
	// analysis to identify certain cheats as being used.
	if (key = gameSimSuspiciousKey)
	wrapper->SendKeyPress(player.id, key);
	// GAMEBLOCKS_END
}

void GameSim::HandleMatchFinished(int matchLengthSeconds, int winningTeam)
{
	// GAMEBLOCKS_BEGIN
	GameBlocksWrapper * wrapper = GameBlocksWrapper::Instance();

	wrapper->SendMatchFinishedEvents(matchLengthSeconds, winningTeam);
	// GAMEBLOCKS_END
}

} // namespace FirstPersonShooter