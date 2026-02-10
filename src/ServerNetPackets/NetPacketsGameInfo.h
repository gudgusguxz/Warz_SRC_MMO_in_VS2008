#pragma once

#pragma pack(push)
#pragma pack(1)

#define SBNET_MASTER_PORT	34000	// default port for master server
#define GBNET_CLIENT_PORT	34001	// default port for game browser master server (client requests)
#define SBNET_MASTER_WATCH_PORT	34005	// watch port for master server
#define SBNET_SUPER_WATCH_PORT	34006	// watch port for supervisor server
#define SBNET_SUPER_MASTER_PORT 34008	// port that superviser will open to start communication with master server
#define SBNET_SUPER_GAME_PORT	34009	// port that superviser will open to start communication with local games
#define SBNET_GAME_PORT		34010	// game servers listening port range start
#define SBNET_VOICE_PORT_ADD	  500	// voice server ports will start from SBNET_GAME_PORT + this (499 game server max)
#define SBNET_LISTEN_PORT_ADD	 1000	// game server outgoing ports will start from SBNET_GAME_PORT + this

enum EGBGameRegion
{
	GBNET_REGION_Unknown   = 0,
	GBNET_REGION_US_West   = 1,
	/*GBNET_REGION_US_East   = 2,
	GBNET_REGION_Europe    = 10,
	GBNET_REGION_Russia    = 20,
	GBNET_REGION_SouthAmerica = 30,*/
};

// MAKE SURE to increase GBGAMEINFO_VERSION after changing following structs
#define GBGAMEINFO_VERSION 0x00000011

struct GBGameInfo
{
	enum EMapId
	{
	  MAPID_Editor_Particles = 0,
	  MAPID_ServerTest,
	  MAPID_WZ_Colorado,
	  MAPID_WZ_Cliffside,
	  MAPID_WZ_California,
	  MAPID_WZ_Caliwood,
	  MAPID_WZ_AircraftCarrier,
	  MAPID_WZ_Trade_Map,

/////////////////////////////////////////

	  MAPID_PVP01,
	  MAPID_PVP02,
	  MAPID_PVP03,
	  MAPID_PVP04,
	  MAPID_PVP05,
	  MAPID_PVP06,
	  MAPID_PVP07,
	  MAPID_PVP08,
	  MAPID_PVP09,
	  MAPID_PVP10,
	  MAPID_PVP11,
	  MAPID_PVP12,
	  MAPID_PVP13,
	  MAPID_PVP14,
	  MAPID_PVP15,
	  MAPID_PVP16,
	  MAPID_PVP17,
	  MAPID_PVP18,
	  MAPID_PVP19,
	  MAPID_PVP20,
	  MAPID_PVP21,
	  MAPID_PVP22,
	  MAPID_PVP23,
	  MAPID_PVP24,
	  MAPID_PVP25,

	  // NOTE: do *NOT* add maps inside current IDs, add ONLY at the end
	  // otherwise current map statistics in DB will be broken
	  MAPID_MAX_ID,
	};
	
	enum EServerFlags
	{
	  SFLAGS_Passworded = 1 << 0,	// passworded server
	  SFLAGS_PVE        = 1 << 1,
	  SFLAGS_Nameplates = 1 << 2,
	  SFLAGS_CrossHair  = 1 << 3,
	  SFLAGS_Tracers    = 1 << 4,
	  SFLAGS_TrialsAllowed = 1 << 5,
	  SFLAGS_DisableASR = 1<<6,
	  SFLAGS_DisableSNP = 1<<7,
	  SFLAGS_MeleeOnly = 1<<8, 
	  SFLAGS_Respawnfast = 1 << 9,
	  SFLAGS_NoDrop = 1 << 10, //NARA::Nodrop
	  SFLAGS_ClanLimit = 1 << 11,
	};

	char	name[32];
	BYTE	mapId;
	WORD	maxPlayers;
	DWORD	flags;		// some game flags
	DWORD	gameServerId;	// unique server ID in our DB
	int	gameTimeLimit; // amount of gametime required to join this server (in hours)
	BYTE	region;		// game region
	BYTE	channel; // 1-TRIAL, 2-OFFICIAL, 3-PRIVATE, 4-PREMIUM, 5-STRONGHOLDS, 6-PUBLIC TEST ENVIRONMENT, 7-VETERAN SERVERS
	int     ClanLimitMax;

	GBGameInfo()
	{
	  sprintf(name, "g%08X", this);
	  mapId = 0xFF;
	  maxPlayers = 0;
	  flags = 0;
	  gameServerId = 0;
	  gameTimeLimit = 0;
	  channel = 0;
	  ClanLimitMax = 0;
	  region = GBNET_REGION_Unknown;
	}
	
	bool IsValid() const
	{
	  if(mapId == 0xFF) return false;
	  if(maxPlayers == 0) return false;
	  if(gameServerId == 0) return false;
	  if(channel == 0) return false;
	  return true;
	}

	bool IsGameworld() const
	{
	  return mapId == MAPID_WZ_Colorado || mapId == MAPID_ServerTest || mapId == MAPID_WZ_California || mapId == MAPID_WZ_Caliwood || mapId == MAPID_WZ_AircraftCarrier || mapId == MAPID_WZ_Trade_Map

/////////////////////////////////////////

		  || mapId == MAPID_PVP01
		  || mapId == MAPID_PVP02
		  || mapId == MAPID_PVP03
		  || mapId == MAPID_PVP04
		  || mapId == MAPID_PVP05
		  || mapId == MAPID_PVP06
		  || mapId == MAPID_PVP07
		  || mapId == MAPID_PVP08
		  || mapId == MAPID_PVP09
		  || mapId == MAPID_PVP10
		  || mapId == MAPID_PVP11
		  || mapId == MAPID_PVP12
		  || mapId == MAPID_PVP13
		  || mapId == MAPID_PVP14
		  || mapId == MAPID_PVP15
		  || mapId == MAPID_PVP16
		  || mapId == MAPID_PVP17
		  || mapId == MAPID_PVP18
		  || mapId == MAPID_PVP19
		  || mapId == MAPID_PVP20
		  || mapId == MAPID_PVP21
		  || mapId == MAPID_PVP22
		  || mapId == MAPID_PVP23
		  || mapId == MAPID_PVP24
		  || mapId == MAPID_PVP25;

		  ;
	}
	
	bool IsRentedGame() const
	{
	  return gameServerId >= 100000;
	}

	bool isSameChannel(int in_channel) const
	{
		return in_channel == channel;
	}
	
	bool FromString(const char* arg)
	{
		int v[14];
		int args = sscanf(arg, "%d %d %d %d %d %d %d %d",
			&v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7]);
		if (args != 8) return false;

		mapId = (BYTE)v[0];
		maxPlayers = (WORD)v[1];
		flags = (DWORD)v[2];
		gameServerId = (DWORD)v[3];
		region = (BYTE)v[4];
		gameTimeLimit = v[5];
		channel = (BYTE)v[6];
		ClanLimitMax = v[7];
		return true;
	}

	void ToString(char* arg) const
	{
		sprintf(arg, "%d %d %d %d %d %d %d %d",
			mapId,
			maxPlayers,
			flags,
			gameServerId,
			region,
			gameTimeLimit,
			channel,
			ClanLimitMax);
	}
};

#pragma pack(pop)