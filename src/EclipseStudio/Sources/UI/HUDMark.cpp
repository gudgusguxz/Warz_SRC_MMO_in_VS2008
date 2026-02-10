#include "r3dPCH.h"
#include "r3d.h"

#include "HUDMark.h"

#include "LangMngr.h"
#include "..\multiplayer\ClientGameLogic.h"
#include "..\ObjectsCode\AI\AI_Player.H"

#include "HUDDisplay.h"
#include "HUDPause.h"
#include "HUDSafelock.h"
#include "HUDAttachments.h"
#include "HUDVault.h"
#include "HUDStore.h"
#include "HUDTrade.h"
#include "HUDRepair.h"
#include "HUDCrafting.h"

extern HUDDisplay*	hudMain;
extern HUDPause*	hudPause;
extern HUDSafelock* hudSafelock;
extern HUDVault*	hudVault;
extern HUDAttachments*	hudAttm;
extern HUDTrade* hudTrade;
extern HUDStore*	hudStore;
extern HUDRepair* hudRepair;
extern HUDCrafting* hudCraft;

HUDMark::HUDMark()
: m_bInited ( false )
{
}

HUDMark::~HUDMark()
{
}

bool HUDMark::Init()
{
	m_bInited = true;
	return true;
}

bool HUDMark::Unload()
{
	m_bInited = false;
	return true;
}

void HUDMark::Update()
{
	if (gClientLogic().localPlayer_)
	{
		if (!gClientLogic().localPlayer_->bDead && !hudMain->isChatInputActive() && !hudAttm->isActive() && !hudVault->isActive() && !hudStore->isActive() && !hudSafelock->isActive() && !hudTrade->isActive() && !hudRepair->isActive() && !hudCraft->isActive())
		{
			if(Mouse->WasPressed(r3dMouse::mCenterButton))
			{
				addMarker(MarkType_1);
			}
		}
	}
}

void HUDMark::Draw()
{
}

void HUDMark::addMarker(int id)
{
	if (gClientLogic().localPlayer_ == NULL)
		return;

	// anti spam check
	if (gClientLogic().localPlayer_)
	{		
		static float lastTimeMessage = 0;
		if (r3dGetTime() - lastTimeMessage < 2.5f)
		{
			if (hudMain) hudMain->showMessage(gLangMngr.getString("InfoMsg_CooldownActive"));
			return; // anti spam
		}
		lastTimeMessage = r3dGetTime();
	}

	if (id == MarkType_1 || id == MarkType_2 || id == MarkType_3)
	{
		const float MAX_CASTING_DISTANCE = 10000.f;
		r3dPoint3D dir;
		if (g_camera_mode->GetInt() == 1)
			r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH*0.32f, &dir);
		else
			r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH2, &dir);

		PxRaycastHit hit2;
		PxSceneQueryFilterData filter2(PxFilterData(COLLIDABLE_STATIC_MASK | (1 << PHYSCOLL_NETWORKPLAYER), 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC);
		g_pPhysicsWorld->raycastSingle(PxVec3(gCam.x, gCam.y, gCam.z), PxVec3(dir.x, dir.y, dir.z), 1000.0f, PxSceneQueryFlag::eDISTANCE, hit2, filter2);
		PhysicsCallbackObject* target = NULL;

		if (hit2.shape && (target = static_cast<PhysicsCallbackObject*>(hit2.shape->getActor().userData)))
		{
			GameObject* gameObj = target->isGameObject();
			if (gameObj && gameObj->isObjType(OBJTYPE_Human))
			{
				char tempStr[256];
				float distance = (gameObj->GetPosition() - gClientLogic().localPlayer_->GetPosition()).Length();
				sprintf(tempStr, "%s / %d m", gClientLogic().localPlayer_->CurLoadout.Gamertag, (int)distance);
				showHUDIcon(gClientLogic().localPlayer_, MarkType_3, gameObj->GetPosition(), tempStr);

				// send msg to server
				PKT_C2C_MarkerData_s n;
				n.id = MarkType_3;
				n.pos = gameObj->GetPosition();
				r3dscpy(n.markerdetail, tempStr);
				p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
				return;
			}
		}

		PxRaycastHit hit;
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK | (1 << PHYSCOLL_NETWORKPLAYER), 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
		if (g_pPhysicsWorld->raycastSingle(PxVec3(gCam.x, gCam.y, gCam.z), PxVec3(dir.x, dir.y, dir.z), MAX_CASTING_DISTANCE, PxSceneQueryFlag::eIMPACT, hit, filter))
		{
			r3dVector pos(hit.impact.x, hit.impact.y, hit.impact.z);
			char tempStr[256];
			float distance = (pos - gClientLogic().localPlayer_->GetPosition()).Length();
			sprintf(tempStr, "%s / %d m", gClientLogic().localPlayer_->CurLoadout.Gamertag, (int)distance);
			showHUDIcon(gClientLogic().localPlayer_, id, pos, tempStr);

			// send msg to server
			PKT_C2C_MarkerData_s n;
			n.id = id;
			n.pos = pos;
			r3dscpy(n.markerdetail, tempStr);
			p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));

		}
	}
}

void HUDMark::showHUDIcon(class obj_Player* from, int id, const r3dVector& pos, const char* markerDetail)
{

	if (id == MarkType_1)
	{
		if (hudMain)hudMain->addHUDIcon(HUDDisplay::HUDIcon_Attack, 15.0f, pos, markerDetail);		
	}
	else if (id == MarkType_2)
	{
		if (hudMain)hudMain->addHUDIcon(HUDDisplay::HUDIcon_Defend, 15.0f, pos, markerDetail);		
	}
	else if (id == MarkType_3)
	{
		if (hudMain)hudMain->addHUDIcon(HUDDisplay::HUDIcon_Spotted, 15.0f, pos, markerDetail);		
	}
}
