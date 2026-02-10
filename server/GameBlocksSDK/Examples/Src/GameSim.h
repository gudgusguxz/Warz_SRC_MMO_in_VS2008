#ifndef GAME_SIM_H_
#define GAME_SIM_H_

#include <sstream>
#include "GameSimTypes.h"

#define GAMESIM_PLAYER_COUNT 32

namespace FirstPersonShooter
{

// contains the core data that will be required for a player by the
// the GameBlocks API
struct PlayerInfo
{
	PlayerInfo() 
		: address("123.0.0.")
		, joinTime(0)
		, ping((int)playerIdCounter)
		, team((int)(playerIdCounter%2))
		, locX((float)playerIdCounter)
		, locY(float(playerIdCounter+1))
		, locZ(float(playerIdCounter+2))
		, viewLocX(float(playerIdCounter))
		, viewLocY(float(playerIdCounter+1))
		, viewLocZ(float(playerIdCounter+2))
		, dirX(-(float)playerIdCounter)
		, dirY(-float(playerIdCounter+1))
		, dirZ(-float(playerIdCounter+2))
		, moverType(0)
		, moverState(1)
	{
		// player name using id
		std::stringstream stream;
		stream << "PlayerName" << (int)playerIdCounter;
		name = stream.str();

		// player id
		id = playerIdCounter;

		// player ip using id
		stream.str("");
		stream << address << playerIdCounter;
		address = stream.str();

		playerIdCounter++;
	}

	static SimPlayerId playerIdCounter;
	SimString name;
	SimPlayerId id;
	SimString address;
	int         joinTime;
	int         ping;
	int         team;

	// basic position and direction information
	float				locX;
	float				locY;
	float				locZ;
	float				dirX;
	float				dirY;
	float				dirZ;

	// the aimbot module also requires the view location for view-based
	// pattern analysis
	float				viewLocX;
	float				viewLocY;
	float				viewLocZ;

	// mover type (e.g. infantry, jeep, helicopter, ghost)
	int					moverType;
	// state (e.g. falling, walking, sprinting, crawling, jumping)
	int					moverState;
};

class GameSim
{
public:
	GameSim();

	bool Tick(float deltaTime);
	void Reset();
	time_t TimeNow();

	// leaving this public for easy example usage
	PlayerInfo players[GAMESIM_PLAYER_COUNT];

	const PlayerInfo * GetPlayerInfoFromId(const SimPlayerId & id) const;

	// some example game interface functions which we would want to call in order
	// to process incoming GameBlocks actions.
	void MessagePlayer(const PlayerInfo & player, const SimString & message);
	void KickPlayer(const PlayerInfo & player, const SimString & reason, float bandDays);
	void MessageAllPlayers(const SimString & message);
	void SendPlayerScreenshotToGameBlocks(const PlayerInfo & player);
	void KillPlayer(const PlayerInfo & player, const SimString & reason);
	void CustomAction(float val);

private:

	// below are custom handlers for the game events which we want to 
	// report to game blocks.

	// Example functions which are triggered when something happens in the
	// game.
	// These are called by the game tick when the respective event occurs.
	// We use them to trigger outgoing GameBlocks events.
	void HandleKill(const PlayerInfo & aggressor, const PlayerInfo & victim, 
		int prevHealth, int bodyPart, int wasTeamKill,
		int weaponType, int wasKillerZoomed, float dist, int wasVictimSpotted);
	void HandleJoin(const PlayerInfo & player);
	void HandleLeave(const PlayerInfo & player);
	void HandleProjectileFired(const PlayerInfo & player, int weaponId, 
		float projDirX, float projDirY, float projDirZ);
	void HandleProjectileImpact(const PlayerInfo & player, int weaponId, 
		int didHit, float dist, int bodyPart, int damage, int impactCount);
	// this one is for if player A targets and alerts his team about player B 
	// via a "spotting" game feature.
	void HandlePlayerSpotted(const PlayerInfo & spotter, const PlayerInfo & spotted);
	void HandlePlayerSpawn(const PlayerInfo & player);
	void HandleChat(const PlayerInfo & player, 
		const SimString & message, int isTeamOnly);
	void HandleKeyPressed(const PlayerInfo & player, int key);
	void HandleMatchFinished(int matchLengthSeconds, int winningTeam);

	// helpers
	void JoinAllPlayers();
	void LeaveAllPlayers();

	time_t timeNow;
	int  state;
	bool didMessagePlayer;
	bool didKickPlayer;
	bool didGlobalMessage;
	bool didSendScreenshot;
	bool didCustomAction;
};

} // namespace FirstPersonShooter

#endif // GAME_SIM_H_
