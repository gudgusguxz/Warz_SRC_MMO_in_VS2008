#include "r3dPCH.h"
#include "r3d.h"

#include "MasterServerConfig.h"

	CMasterServerConfig* gServerConfig = NULL;

static const char* configFile = "MasterServer.cfg";

CMasterServerConfig::CMasterServerConfig()
{
  const char* group      = "MasterServer";

  if(_access(configFile, 0) != 0) {
    r3dError("can't open config file %s\n", configFile);
  }

  masterPort_  = r3dReadCFG_I(configFile, group, "masterPort", SBNET_MASTER_PORT);
  clientPort_  = r3dReadCFG_I(configFile, group, "clientPort", GBNET_CLIENT_PORT);
  masterCCU_   = r3dReadCFG_I(configFile, group, "masterCCU",  3000);

  #define CHECK_I(xx) if(xx == 0)  r3dError("missing %s value in %s", #xx, configFile);
  #define CHECK_S(xx) if(xx == "") r3dError("missing %s value in %s", #xx, configFile);
  CHECK_I(masterPort_);
  CHECK_I(clientPort_);
  #undef CHECK_I
  #undef CHECK_S

  serverId_    = r3dReadCFG_I(configFile, group, "serverId", 0);
  if(serverId_ == 0)
  {
	MessageBox(NULL, "you must define serverId in MasterServer.cfg", "", MB_OK);
	r3dError("no serverId");
  }
  if(serverId_ > 255 || serverId_ < 1)
  {
	MessageBox(NULL, "bad serverId", "", MB_OK);
	r3dError("bad serverId");
  }

  minSupersToStartGame_ = r3dReadCFG_I(configFile, group, "minServers", 10);
  
  LoadConfig();
  
  // give time to spawn our hosted games (except for dev server)
  nextRentGamesCheck_ = r3dGetTime() + 60.0f;
  if(serverId_ >= MASTERSERVER_DEV_ID) nextRentGamesCheck_ = r3dGetTime() + 5.0f;
  
  return;
}

void CMasterServerConfig::LoadConfig()
{
  r3dCloseCFG_Cur();
  
  numPermGames_ = 0;

  LoadPermGamesConfig();
  Temp_Load_WarZGames();
  
  OnGameListUpdated();
}

void CMasterServerConfig::Temp_Load_WarZGames()
{
  char group[128];
  strcpy_s(group, sizeof(group), "WarZGames");

  int numGames    = r3dReadCFG_I(configFile, group, "numGames", 0);
  int maxPlayers  = r3dReadCFG_I(configFile, group, "maxPlayers", 100);
  int numCliffGames = r3dReadCFG_I(configFile, group, "numCliffGames", 0);
  int numTrialGames = r3dReadCFG_I(configFile, group, "numTrialGames", 0);
  int numPremiumGames = r3dReadCFG_I(configFile, group, "numPremiumGames", 0);
  int numVeteranGames = r3dReadCFG_I(configFile, group, "numVeteranGames", 0);
  int numPTEGamesColorado = r3dReadCFG_I(configFile, group, "numPTEGames", 0);
  int numPTEGamesCali = r3dReadCFG_I(configFile, group, "numPTEGamesCali", 0);
  int numPTEGamesStrongholds = r3dReadCFG_I(configFile, group, "numPTEGamesStronghold", 0);
  int numAircraftCarrier = r3dReadCFG_I(configFile, group, "numAircraftCarrier", 0);
  int numAircraftCarrierSNPMode = r3dReadCFG_I(configFile, group, "numAircraftCarrierSNPMode", 0);
  int numAircraftCarrierNoSNPMode = r3dReadCFG_I(configFile, group, "numAircraftCarrierNoSNPMode", 0);
  int numTradeZone = r3dReadCFG_I(configFile, group, "numTradeZone", 0);
  int numNoDrop = r3dReadCFG_I(configFile, group, "numNoDrop", 0);

  int numPTEPlayersColorado = 200;
  int numPTEPlayersCali = 100;
  int numPTEPlayersStrongholds = 50;
  
  for(int i=0; i<numGames; i++) //Colorado
  {
    GBGameInfo ginfo;
    ginfo.mapId      = GBGameInfo::MAPID_WZ_Colorado;
    ginfo.flags      = GBGameInfo::SFLAGS_Nameplates | 
					   GBGameInfo::SFLAGS_CrossHair | 
					   GBGameInfo::SFLAGS_Tracers | 
					   GBGameInfo::SFLAGS_TrialsAllowed;
    ginfo.maxPlayers = maxPlayers;
    ginfo.channel    = 2; // official server

    sprintf_s(ginfo.name, sizeof(ginfo.name), "Siam TH SERVER %03d", i + 1);
    AddPermanentGame(10000 + i, ginfo, GBNET_REGION_US_West);
  }

  for(int i=0; i<numPremiumGames; i++) //Colorado
  {
	  GBGameInfo ginfo;
	  ginfo.mapId      = GBGameInfo::MAPID_WZ_Colorado;
	  ginfo.flags      = GBGameInfo::SFLAGS_Nameplates | 
						 GBGameInfo::SFLAGS_CrossHair | 
						 GBGameInfo::SFLAGS_Tracers | 
						 GBGameInfo::SFLAGS_TrialsAllowed;
	  ginfo.maxPlayers = maxPlayers;
	  ginfo.channel	   = 4; // premium server

	  sprintf_s(ginfo.name, sizeof(ginfo.name), "Siam TH SERVER PVE %03d", i + 1);
	  AddPermanentGame(14000 + i, ginfo, GBNET_REGION_US_West);
  }

  for(int i=0; i<numTradeZone; i++) //WZ_Trade_Map
  {
    GBGameInfo ginfo;
    ginfo.mapId      = GBGameInfo::MAPID_WZ_Trade_Map;
    ginfo.flags      = GBGameInfo::SFLAGS_Nameplates | 
						GBGameInfo::SFLAGS_CrossHair | 
						GBGameInfo::SFLAGS_Tracers | 
						GBGameInfo::SFLAGS_TrialsAllowed;
    ginfo.maxPlayers = maxPlayers;
    ginfo.channel    = 7; // trade server

    sprintf_s(ginfo.name, sizeof(ginfo.name), "Siam PVP ZONE %03d", i + 1);
    AddPermanentGame(21000 + i, ginfo, GBNET_REGION_US_West);
  }

  for(int i=0; i<numTrialGames; i++)
  {
	  GBGameInfo ginfo;
	  ginfo.mapId      = GBGameInfo::MAPID_WZ_Colorado;
	  ginfo.flags      = GBGameInfo::SFLAGS_Nameplates | 
						 GBGameInfo::SFLAGS_CrossHair | 
						 GBGameInfo::SFLAGS_Tracers | 
						 GBGameInfo::SFLAGS_TrialsAllowed;
	  ginfo.maxPlayers = maxPlayers;
	  ginfo.channel	   = 1; // trial server

	  sprintf_s(ginfo.name, sizeof(ginfo.name), "WZ Trial Server %03d", i + 1);
	  AddPermanentGame(12000 + i, ginfo, GBNET_REGION_US_West);
  }

  for(int i=0; i<numPTEGamesColorado; i++)
  {
	  GBGameInfo ginfo;
	  ginfo.mapId      = GBGameInfo::MAPID_WZ_Colorado;
	  ginfo.flags      = GBGameInfo::SFLAGS_Nameplates | 
						 GBGameInfo::SFLAGS_CrossHair | 
						 GBGameInfo::SFLAGS_Tracers | 
						 GBGameInfo::SFLAGS_TrialsAllowed;
	  ginfo.maxPlayers = numPTEPlayersColorado;
	  ginfo.channel	   = 6; 

	  sprintf_s(ginfo.name, sizeof(ginfo.name), "WZ PTE Colorado %03d", i + 1);
	  AddPermanentGame(15000 + i, ginfo, GBNET_REGION_US_West);
  }

/*
  California isn't in release
  for(int i=0; i<numPTEGamesCali; i++)
  {
	  GBGameInfo ginfo;
	  ginfo.mapId      = GBGameInfo::MAPID_WZ_California;
	  ginfo.flags      = GBGameInfo::SFLAGS_Nameplates | 
						 GBGameInfo::SFLAGS_CrossHair | 
						 GBGameInfo::SFLAGS_Tracers;
	  ginfo.maxPlayers = numPTEPlayersCali;
	  ginfo.channel    = 6; 

	  sprintf(ginfo.name, "WZ PTE California %03d", i + 1);
	  AddPermanentGame(15100 + i, ginfo, GBNET_REGION_US_West);
  }
*/  

  for(int i=0; i<numPTEGamesStrongholds; i++)
  {
	  GBGameInfo ginfo;
	  ginfo.mapId      = GBGameInfo::MAPID_WZ_Cliffside;
	  ginfo.flags      = GBGameInfo::SFLAGS_Nameplates | 
						 GBGameInfo::SFLAGS_CrossHair | 
						 GBGameInfo::SFLAGS_Tracers | 
						 GBGameInfo::SFLAGS_TrialsAllowed;
	  ginfo.maxPlayers = numPTEPlayersStrongholds;
	  ginfo.channel    = 6; 

	  sprintf_s(ginfo.name, sizeof(ginfo.name), "WZ PTE Stronghold %03d", i + 1);
	  AddPermanentGame(15200 + i, ginfo, GBNET_REGION_US_West);
  }

  for(int i=0; i<numVeteranGames; i++)
  {
	  GBGameInfo ginfo;
	  ginfo.mapId      = GBGameInfo::MAPID_WZ_Colorado;
	  ginfo.flags      = GBGameInfo::SFLAGS_Nameplates | 
						 GBGameInfo::SFLAGS_CrossHair | 
						 GBGameInfo::SFLAGS_Tracers | 
						 GBGameInfo::SFLAGS_TrialsAllowed;
	  ginfo.maxPlayers = maxPlayers;
	  ginfo.gameTimeLimit = 50; // X hours limit
	  ginfo.channel    = 7; // veteran server

	  sprintf_s(ginfo.name, sizeof(ginfo.name), "WZ Veteran Server %03d", i + 1);
	  AddPermanentGame(16000 + i, ginfo, GBNET_REGION_US_West);
  }

  // stronghold cliffside games
  for(int i=0; i<numCliffGames; ++i) 
  {
	  GBGameInfo ginfo;
	  ginfo.mapId      = GBGameInfo::MAPID_WZ_Cliffside;
	  ginfo.flags      = GBGameInfo::SFLAGS_Nameplates | 
						 GBGameInfo::SFLAGS_CrossHair | 
						 GBGameInfo::SFLAGS_Tracers | 
						 GBGameInfo::SFLAGS_TrialsAllowed;
	  ginfo.maxPlayers = 100;
	  ginfo.channel	   = 5; // strongholds

	  sprintf_s(ginfo.name, sizeof(ginfo.name), "StrongHold %03d", i+1);
	  AddPermanentGame(18000+i, ginfo, GBNET_REGION_US_West);
  }

  //MAPID_WZ_AircraftCarrier
  for(int i=0; i<numAircraftCarrier; i++)
  {
    GBGameInfo ginfo;
    ginfo.mapId      = GBGameInfo::MAPID_WZ_AircraftCarrier;
    ginfo.flags      = GBGameInfo::SFLAGS_Nameplates | 
					   GBGameInfo::SFLAGS_CrossHair | 
					   GBGameInfo::SFLAGS_Tracers | 
					   GBGameInfo::SFLAGS_TrialsAllowed;
    ginfo.maxPlayers = maxPlayers;
    ginfo.channel    = 6; // dev event server

    sprintf_s(ginfo.name, sizeof(ginfo.name), "NORMAL MODE %02d", i + 1);
    AddPermanentGame(19000 + i, ginfo, GBNET_REGION_US_West);
  }

  //MAPID_WZ_AircraftCarrier Snipers Mode
  for (int i = 0; i<numAircraftCarrierSNPMode; i++)
  {
	  GBGameInfo ginfo;
	  ginfo.mapId = GBGameInfo::MAPID_WZ_AircraftCarrier;
	  ginfo.flags = GBGameInfo::SFLAGS_Nameplates | 
					GBGameInfo::SFLAGS_CrossHair | 
					GBGameInfo::SFLAGS_Tracers | 
					GBGameInfo::SFLAGS_TrialsAllowed | 
					GBGameInfo::SFLAGS_DisableASR;
	  ginfo.maxPlayers = maxPlayers;
	  ginfo.channel = 6;// dev event server

	  sprintf_s(ginfo.name, sizeof(ginfo.name), "SNIPERS MODE %02d", i + 1);
	  AddPermanentGame(19500 + i, ginfo, GBNET_REGION_US_West);
  }

  //MAPID_WZ_AircraftCarrier No Snipers Mode
  for (int i = 0; i<numAircraftCarrierNoSNPMode; i++)
  {
	  GBGameInfo ginfo;
	  ginfo.mapId = GBGameInfo::MAPID_WZ_AircraftCarrier;
	  ginfo.flags = GBGameInfo::SFLAGS_Nameplates | 
					GBGameInfo::SFLAGS_CrossHair | 
					GBGameInfo::SFLAGS_Tracers | 
					GBGameInfo::SFLAGS_TrialsAllowed | 
					GBGameInfo::SFLAGS_DisableSNP;
	  ginfo.maxPlayers = maxPlayers;
	  ginfo.channel = 6;// dev event server

	  sprintf_s(ginfo.name, sizeof(ginfo.name), "NO SNIPERS MODE %02d", i + 1);
	  AddPermanentGame(20000 + i, ginfo, GBNET_REGION_US_West);
  }

  //MAPID_WZ_AircraftCarrier No Drop Mode
  for(int i=0; i<numNoDrop; i++)
  {
    GBGameInfo ginfo;
    ginfo.mapId      = GBGameInfo::MAPID_WZ_AircraftCarrier;
    ginfo.flags      = GBGameInfo::SFLAGS_Nameplates | 
						GBGameInfo::SFLAGS_CrossHair | 
						GBGameInfo::SFLAGS_Tracers | 
						GBGameInfo::SFLAGS_TrialsAllowed;
    ginfo.maxPlayers = 100;
    ginfo.channel    = 6; // dev event server

    sprintf_s(ginfo.name, sizeof(ginfo.name), "NO DROP MODE %03d", i + 1);
    AddPermanentGame(22000 + i, ginfo, GBNET_REGION_US_West);
  }
  

  return;
}

void CMasterServerConfig::LoadPermGamesConfig()
{
  numPermGames_ = 0;

//#ifdef _DEBUG
//  r3dOutToLog("Permanet games disabled in DEBUG");
//  return;
//#endif
  
  for(int i=0; i<250; i++)
  {
    char group[128];
    sprintf_s(group, sizeof(group), "PermGame%d", i+1);

    char map[512] = "";
    char data[512] = "";
    char name[512];
	int ClanLimit = 0;

    int DisableASR;
    int DisableSNP;
    int MeleeOnly;
	int	NoDrop; //NARA::Nodrop
	int RespawnFast; //Respawnfast

    r3dscpy(map,  r3dReadCFG_S(configFile, group, "map", ""));
    r3dscpy(data, r3dReadCFG_S(configFile, group, "data", ""));
    r3dscpy(name, r3dReadCFG_S(configFile, group, "name", ""));

    DisableASR = r3dReadCFG_I(configFile, group, "DisableASR", 0);
    DisableSNP = r3dReadCFG_I(configFile, group, "DisableSNP", 0);
    MeleeOnly = r3dReadCFG_I(configFile, group, "MeleeOnly", 0);
	NoDrop = r3dReadCFG_I(configFile, group, "NoDrop", 0); //NARA::Nodrop
	RespawnFast = r3dReadCFG_I(configFile, group, "RespawnFast", 0); //Respawnfast
	ClanLimit = r3dReadCFG_I(configFile, group, "ClanLimit", 0);

    if(name[0] == 0)
      sprintf_s(name, sizeof(name), "PermGame%d", i+1);

    if(*map == 0)
      continue;
    
    ParsePermamentGame(i, name, map, data, NoDrop, DisableASR, DisableSNP, MeleeOnly, RespawnFast, ClanLimit); //Respawnfast //NARA::Nodrop
  }

  return;  
}

static int StringToGBMapID(char* str)
{
  if(stricmp(str, "MAPID_WZ_Colorado") == 0)
    return GBGameInfo::MAPID_WZ_Colorado;
  if(stricmp(str, "MAPID_WZ_Cliffside") == 0)
    return GBGameInfo::MAPID_WZ_Cliffside;
  if(stricmp(str, "MAPID_WZ_California") == 0)
    return GBGameInfo::MAPID_WZ_California;
  if(stricmp(str, "MAPID_WZ_Caliwood") == 0)
	  return GBGameInfo::MAPID_WZ_Caliwood;
  if(stricmp(str, "MAPID_WZ_AircraftCarrier") == 0)
	  return GBGameInfo::MAPID_WZ_AircraftCarrier;
  if(stricmp(str, "MAPID_WZ_Trade_Map") == 0)
	  return GBGameInfo::MAPID_WZ_Trade_Map;

  if(stricmp(str, "MAPID_Editor_Particles") == 0)
    return GBGameInfo::MAPID_Editor_Particles;
  if(stricmp(str, "MAPID_ServerTest") == 0)
    return GBGameInfo::MAPID_ServerTest;
    
/////////////////////////////////////////

	if(stricmp(str, "MAPID_PVP01") == 0)
		 return GBGameInfo::MAPID_PVP01;
	if(stricmp(str, "MAPID_PVP02") == 0)
		 return GBGameInfo::MAPID_PVP02;
	if(stricmp(str, "MAPID_PVP03") == 0)
		 return GBGameInfo::MAPID_PVP03;
	if(stricmp(str, "MAPID_PVP04") == 0)
		 return GBGameInfo::MAPID_PVP04;
	if(stricmp(str, "MAPID_PVP05") == 0)
		 return GBGameInfo::MAPID_PVP05;
	if(stricmp(str, "MAPID_PVP06") == 0)
		 return GBGameInfo::MAPID_PVP06;
	if(stricmp(str, "MAPID_PVP07") == 0)
		 return GBGameInfo::MAPID_PVP07;
	if(stricmp(str, "MAPID_PVP08") == 0)
		 return GBGameInfo::MAPID_PVP08;
	if(stricmp(str, "MAPID_PVP09") == 0)
		 return GBGameInfo::MAPID_PVP09;
	if(stricmp(str, "MAPID_PVP10") == 0)
		 return GBGameInfo::MAPID_PVP10;
	if(stricmp(str, "MAPID_PVP11") == 0)
		 return GBGameInfo::MAPID_PVP11;
	if(stricmp(str, "MAPID_PVP12") == 0)
		 return GBGameInfo::MAPID_PVP12;
	if(stricmp(str, "MAPID_PVP13") == 0)
		 return GBGameInfo::MAPID_PVP13;
	if(stricmp(str, "MAPID_PVP14") == 0)
		 return GBGameInfo::MAPID_PVP14;
	if(stricmp(str, "MAPID_PVP15") == 0)
		 return GBGameInfo::MAPID_PVP15;
	if(stricmp(str, "MAPID_PVP16") == 0)
		 return GBGameInfo::MAPID_PVP16;
	if(stricmp(str, "MAPID_PVP17") == 0)
		 return GBGameInfo::MAPID_PVP17;
	if(stricmp(str, "MAPID_PVP18") == 0)
		 return GBGameInfo::MAPID_PVP18;
	if(stricmp(str, "MAPID_PVP19") == 0)
		 return GBGameInfo::MAPID_PVP19;
	if(stricmp(str, "MAPID_PVP20") == 0)
		 return GBGameInfo::MAPID_PVP20;
	if(stricmp(str, "MAPID_PVP21") == 0)
		 return GBGameInfo::MAPID_PVP21;
	if(stricmp(str, "MAPID_PVP22") == 0)
		 return GBGameInfo::MAPID_PVP22;
	if(stricmp(str, "MAPID_PVP23") == 0)
		 return GBGameInfo::MAPID_PVP23;
	if(stricmp(str, "MAPID_PVP24") == 0)
		 return GBGameInfo::MAPID_PVP24;
	if(stricmp(str, "MAPID_PVP25") == 0)
		 return GBGameInfo::MAPID_PVP25;

  r3dError("bad GBMapID %s\n", str);
  return 0;
}

static EGBGameRegion StringToGBRegion(const char* str)
{
  if(stricmp(str, "GBNET_REGION_US_West") == 0)
    return GBNET_REGION_US_West;

  /*else if(stricmp(str, "GBNET_REGION_US_East") == 0)
    return GBNET_REGION_US_East;

   else if(stricmp(str, "GBNET_REGION_Europe") == 0)
    return GBNET_REGION_Europe;

   else if(stricmp(str, "GBNET_REGION_Russia") == 0)
    return GBNET_REGION_Russia;

   else if(stricmp(str, "GBNET_REGION_SouthAmerica") == 0)
    return GBNET_REGION_SouthAmerica;*/

  r3dError("bad GBGameRegion %s\n", str);
  return GBNET_REGION_Unknown;
}

void CMasterServerConfig::ParsePermamentGame(int gameServerId, const char* name, const char* map, const char* data, int NoDrop, int DisableASR, int DisableSNP, int MeleeOnly, int RespawnFast, int ClanLimit) //Respawnfast //NARA::Nodrop
{
  char mapid[128];
  char maptype[128];
  char region[128];
  int minGames;
  int maxGames;
  if(5 != sscanf_s(map, "%s %s %s %d %d", mapid, (unsigned)sizeof(mapid), maptype, (unsigned)sizeof(maptype), region, (unsigned)sizeof(region), &minGames, &maxGames)) {
    r3dError("bad map format: %s\n", map);
  }

  int maxPlayers;
  int minLevel = 0;
  int maxLevel = 0;
  int channel = 0;
  int gameTimeLimit = 0;
  if(5 != sscanf_s(data, "%d %d %d %d %d", &maxPlayers, &minLevel, &maxLevel, &channel, &gameTimeLimit)) {
    r3dError("bad data format: %s\n", data);
  }

  GBGameInfo ginfo;
  ginfo.mapId        = StringToGBMapID(mapid);
  ginfo.maxPlayers   = maxPlayers;
  ginfo.flags        = GBGameInfo::SFLAGS_Nameplates | GBGameInfo::SFLAGS_CrossHair | GBGameInfo::SFLAGS_Tracers;
  if(channel == 1)
	  ginfo.flags |= GBGameInfo::SFLAGS_TrialsAllowed;
  if ( DisableASR == 1)
        ginfo.flags |= GBGameInfo::SFLAGS_DisableASR;
  if ( DisableSNP == 1)
        ginfo.flags |= GBGameInfo::SFLAGS_DisableSNP;
  if ( MeleeOnly == 1)
        ginfo.flags |= GBGameInfo::SFLAGS_MeleeOnly;
  if ( NoDrop == 1) //NARA::Nodrop
		ginfo.flags |= GBGameInfo::SFLAGS_NoDrop;
  if ( RespawnFast == 1) //Respawnfast
		ginfo.flags |= GBGameInfo::SFLAGS_Respawnfast;
  if (ClanLimit > 0)
  {
	  ginfo.flags |= GBGameInfo::SFLAGS_ClanLimit;
	  ginfo.ClanLimitMax = ClanLimit;
  }

  ginfo.channel		 = channel; 
  ginfo.gameTimeLimit = gameTimeLimit;
  r3dscpy(ginfo.name, name);

  r3dOutToLog("permgame: ID:%d, %s, %s, ClanLimit:%d \n",
	  gameServerId, name, mapid, ClanLimit);
  
  EGBGameRegion eregion = StringToGBRegion(region);
  AddPermanentGame(gameServerId, ginfo, eregion);
}

void CMasterServerConfig::AddPermanentGame(int gameServerId, const GBGameInfo& ginfo, EGBGameRegion region)
{
  r3d_assert(numPermGames_ < R3D_ARRAYSIZE(permGames_));
  permGame_s& pg = permGames_[numPermGames_++];

  r3d_assert(gameServerId);
  pg.ginfo = ginfo;
  pg.ginfo.gameServerId = gameServerId;
  pg.ginfo.region       = region;
  
  return;
}

void CMasterServerConfig::OnGameListUpdated()
{
  memset(&numGamesHosted, 0, sizeof(numGamesHosted));
  memset(&numGamesRented, 0, sizeof(numGamesRented));
  memset(&numStrongholdsRented, 0, sizeof(numStrongholdsRented));

  for(int i=0; i<numPermGames_; i++)
  {
    const GBGameInfo& ginfo = permGames_[i].ginfo;

    int regIdx = 0;
    if(ginfo.region == GBNET_REGION_US_West) regIdx = 0;
    /*else if(ginfo.region == GBNET_REGION_Europe) regIdx = 1;
    else if(ginfo.region == GBNET_REGION_Russia) regIdx = 2;
	else if(ginfo.region == GBNET_REGION_SouthAmerica) regIdx = 3;*/
    
    numGamesHosted[regIdx]++;
  }
  
  for(size_t i=0; i<rentGames_.size(); i++)
  {
    const GBGameInfo& ginfo = rentGames_[i].ginfo;

    int regIdx = 0;
    if(ginfo.region == GBNET_REGION_US_West) regIdx = 0;
    /*else if(ginfo.region == GBNET_REGION_Europe) regIdx = 1;
    else if(ginfo.region == GBNET_REGION_Russia) regIdx = 2;
	else if(ginfo.region == GBNET_REGION_SouthAmerica) regIdx = 3;*/
    
    if(ginfo.IsGameworld())
      numGamesRented[regIdx]++;
    else
      numStrongholdsRented[regIdx]++;
  }
  
  // create gameinfo<->gameServerId map
  rentByGameServerId_.clear();
  for(size_t i=0; i<rentGames_.size(); i++)
  {
    CMasterServerConfig::rentGame_s* rg = &gServerConfig->rentGames_[i];
    rentByGameServerId_.insert(TRentedGamesList::value_type(rg->ginfo.gameServerId, rg));
  }
  
  return;
}

CMasterServerConfig::rentGame_s* CMasterServerConfig::GetRentedGameInfo(DWORD gameServerId)
{
  TRentedGamesList::iterator it = rentByGameServerId_.find(gameServerId);
  if(it == rentByGameServerId_.end())
    return NULL;

  return it->second;
}
