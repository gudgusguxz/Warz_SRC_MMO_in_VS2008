#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"

#include "FriendsServerLogic.h"

	FriendsServerLogic gFriendsServerLogic;

FriendsServerLogic::FriendsServerLogic()
{
  InitializeCriticalSection(&csNetwork);

  isConnected_      = false;
  friendsServerId_  = 0;
  connectStartTime_ = -999;
	versionChecked_   = false;
}

FriendsServerLogic::~FriendsServerLogic()
{
  g_net.Deinitialize();
  
  DeleteCriticalSection(&csNetwork);
}

void FriendsServerLogic::OnNetPeerConnected(DWORD peerId)
{
  //r3dOutToLog("connected to friends server peer%02d\n", peerId);
	versionChecked_ = false;
  if(net_ == NULL)
    return;
  
  // send validation packet immediately
  FSPKT_ValidateConnectingPeer_s n;
  n.version = FSNET_VERSION;
  n.key1    = FSNET_KEY1;
  n.key2    = FSNET_KEY2;
  net_->SendToHost(&n, sizeof(n), true);

  // set connected flag after sending validation packet.
  isConnected_ = true;
  
  return;
}

void FriendsServerLogic::OnNetPeerDisconnected(DWORD peerId)
{
  //r3dOutToLog("disconnected from friends server\n");

  isConnected_      = false;
  friendsServerId_  = 0;
  connectStartTime_ = -999;
	versionChecked_   = false;
}

void FriendsServerLogic::OnNetData(DWORD peerId, const r3dNetPacketHeader* packetData, int packetSize)
{
	if(packetData == NULL)
		return;
  if(packetSize < (int)sizeof(r3dNetPacketHeader))
  {
    r3dOutToLog("FriendsNet: packet too small (%d), dropping\n", packetSize);
    return;
  }
	if(net_ == NULL)
		return;

  switch(packetData->EventID) 
  {
    default:
    r3dOutToLog("FriendsNet: invalid packetId %d size %d\n", packetData->EventID, packetSize);
      return;
      
    case FSPKT_S2C_FriendServerInfo:
    {
    if(packetSize < (int)sizeof(FSPKT_S2C_FriendServerInfo_s))
    {
      r3dOutToLog("FriendsNet: bad FriendServerInfo size %d\n", packetSize);
      return;
    }
      const FSPKT_S2C_FriendServerInfo_s& n = *(FSPKT_S2C_FriendServerInfo_s*)packetData;
      
      friendsServerId_ = n.serverId;
      r3d_assert(friendsServerId_);

      break;
    }
      
    case FSPKT_ValidateConnectingPeer:
    {
	  if(packetSize < (int)sizeof(FSPKT_ValidateConnectingPeer_s))
	  {
		  r3dOutToLog("FriendsNet: bad ValidateConnectingPeer size %d\n", packetSize);
		  return;
	  }
      const FSPKT_ValidateConnectingPeer_s& n = *(FSPKT_ValidateConnectingPeer_s*)packetData;

      //check version
      if(n.version != FSNET_VERSION)
      {
        r3dOutToLog("Wrong friends server version. Please get update\n");
        net_->DisconnectPeer(peerId);
        OnNetPeerDisconnected(peerId);
		return;
      }
		versionChecked_ = true;

      break;
    }
    
    case FSPKT_S2C_Ping:
    {
	  if(packetSize < (int)sizeof(FSPKT_S2C_Ping_s))
	  {
		  r3dOutToLog("FriendsNet: bad ping size %d\n", packetSize);
		  return;
	  }
      const FSPKT_S2C_Ping_s& n = *(FSPKT_S2C_Ping_s*)packetData;
      
      // pong
      FSPKT_C2S_Pong_s n2;
      net_->SendToHost(&n2, sizeof(n2));

      break;
    }
  }

  return;
}

void FriendsServerLogic::Tick()
{
  R3DPROFILE_FUNCTION("FriendsServerLogic::Tick");
  EnterCriticalSection(&csNetwork);
  if(net_) {
    net_->Update();
  }
  LeaveCriticalSection(&csNetwork);

  return;
}

int FriendsServerLogic::ConnectAsync(const char* host, int port)
{
  // hide inside critical section, because it can be called from other thread in FrontEndNew
  r3dCSHolder cs1(csNetwork);

  // create network if first time
  if(net_ == NULL) {
    g_net.Initialize(this, "friendsNet");
    g_net.CreateClient(0);
  }

  if(isConnected_)
    return 1;
    
  // check if still connecting from previous attempt
  if(r3dGetTime() < connectStartTime_ + 60.0f)
    return 1;

  // initiate connect
  connectStartTime_ = r3dGetTime();
  int res = net_->Connect(host, port);
  return res;
}

void FriendsServerLogic::Disconnect()
{
  // hide inside critical section, because it can be called from other thread in FrontEndNew
  r3dCSHolder cs1(csNetwork);

  g_net.Deinitialize();
  isConnected_ = false;
}
