#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "XMLHelpers.h"

#include "multiplayer/P2PMessages.h"
#include "ServerGameLogic.h"

#include "obj_ServerFishing.h"

IMPLEMENT_CLASS(obj_ServerFishing, "obj_Fishing", "Object");
AUTOREGISTER_CLASS(obj_ServerFishing);

FishingZoneMgr gFishingZoneMngr;

obj_ServerFishing::obj_ServerFishing()
{
	useRadius = 2.0f;
}

obj_ServerFishing::~obj_ServerFishing()
{
}

BOOL obj_ServerFishing::OnCreate()
{
	parent::OnCreate();

	gFishingZoneMngr.RegisterFishingZone(this);
	return 1;
}

// copy from client version
void obj_ServerFishing::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node objNode = node.child("Fishing_Zone");
	GetXMLVal("useRadius", objNode, &useRadius);
}
