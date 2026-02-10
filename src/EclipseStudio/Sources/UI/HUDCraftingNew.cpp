#include "r3dPCH.h"
#include "r3d.h"

#include "../../../Eternity/sf/Console/config.h"
#include "HUDCraftingNew.h"
#include "LangMngr.h"

#include "FrontendShared.h"

#include "../multiplayer/clientgamelogic.h"
#include "../ObjectsCode/AI/AI_Player.H"
#include "../ObjectsCode/weapons/Weapon.h"
#include "../ObjectsCode/weapons/WeaponArmory.h"
#include "../GameLevel.h"

HUDCraftingNew::HUDCraftingNew()
{
	isActive_ = false;
	isInit = false;
	prevKeyboardCaptureMovie = NULL;
}

HUDCraftingNew::~HUDCraftingNew()
{
}

void HUDCraftingNew::eventCraftingItem(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==1);
	
	recordID = args[0].GetUInt();
	
	Scaleform::GFx::Value var[2];
	obj_Player* plr = gClientLogic().localPlayer_;
	r3d_assert(plr);
	{
		const CraftingNewConfig* cfg = g_pWeaponArmory->getCraftingNewConfig(recordID);
		r3d_assert(cfg);  
		bool notEnough = false;
		for(uint32_t i = 0; i < cfg->numComponents; ++i)
		{
			if(!plr->CurLoadout.hasItemWithQuantity(cfg->components[i].ComponentCraftingItemID, cfg->components[i].ComponentCraftingQuantity))
			{
				notEnough = true;
				break;
			}
		}
		if(notEnough)
		{
			var[0].SetString("Failed to Crafting item, not enough of components crafting items!");
			var[1].SetBoolean(true);
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
			return;
		}
	}

	isLockedUI = true;
	var[0].SetString("");
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
	
	{
		PKT_C2S_CraftingNewItem_s n;
		n.recordID= recordID;
		p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
	}
}

void HUDCraftingNew::eventSetisProcess(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==2);
	
	bool IsProcess = args[0].GetBool();
	bool CraftStatus = args[1].GetBool();

	if(!IsProcess && CraftStatus || !CraftStatus)
	{
		PKT_C2S_CraftingNewItemAdd_s n;
		n.recordID = recordID;
 		n.Result = CraftingResult;
 		p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
	}
	PopulateBackpackInfo();
	isLockedUI = IsProcess;
}

void HUDCraftingNew::setErrorMsg(const char* errMsg)
{
	if(isLockedUI)
	{
		isLockedUI = false;
		lockedOp = 0;
		gfxMovie.Invoke("_root.api.hideInfoMsg", "");
		Scaleform::GFx::Value var[3];
		var[0].SetString(errMsg);
		var[1].SetBoolean(true);
		var[2].SetString("ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
	}
}

void HUDCraftingNew::StartCraftingUI(int Result)
{
	r3d_assert(isLockedUI);
	CraftingResult = Result;
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");
	gfxMovie.Invoke("_root.api.Main.StartProcessBar", Result);

	lockedOp = 0;
}

bool HUDCraftingNew::Init()
{
	if(!gfxMovie.Load("Data\\Menu\\WarZ_HUD_CraftingNew.swf", false)) 
		return false;

#define MAKE_CALLBACK(FUNC) game_new r3dScaleformMovie::TGFxEICallback<HUDCraftingNew>(this, &HUDCraftingNew::FUNC)
	gfxMovie.RegisterEventHandler("eventCraftingItem", MAKE_CALLBACK(eventCraftingItem));
	gfxMovie.RegisterEventHandler("eventSetisProcess", MAKE_CALLBACK(eventSetisProcess));

	gfxMovie.SetCurentRTViewport( Scaleform::GFx::Movie::SM_ExactFit );

	{
		addItemCategoriesToUI(gfxMovie);
		addItemsAndCategoryToUI(gfxMovie);
	}

	isActive_ = false;
	isInit = true;
	isLockedUI = false;
	lockedOp = 0;
	return true;
}

void HUDCraftingNew::PopulateListinfo()
{
	if(!isInit) return;

	obj_Player* plr = gClientLogic().localPlayer_;
	if(!plr) return;

	gfxMovie.Invoke("_root.api.clearCrafting", "");
	
	Scaleform::GFx::Value var[5];
	r3dgameVector(const CraftingNewConfig*) ListInfoCrafting;
	g_pWeaponArmory->startItemSearch();
	while(g_pWeaponArmory->searchNextItem())
	{
		uint32_t record = g_pWeaponArmory->getCurrentSearchItemID();
		const CraftingNewConfig* CraftingConfig = g_pWeaponArmory->getCraftingNewConfig(record);
		if(CraftingConfig)
		{
			ListInfoCrafting.push_back(CraftingConfig);
		}
	}
	const size_t CraftingRecSize = ListInfoCrafting.size();
	for(size_t i = 0; i < CraftingRecSize; ++i)
	{
		const CraftingNewConfig* itm = ListInfoCrafting[i];
		{
			var[0].SetUInt(itm->m_itemID);
			var[1].SetUInt(itm->CraftingItemID);
			var[2].SetInt(itm->CraftingQuantity);
			var[3].SetInt(itm->CraftedPage);
			var[4].SetNumber(itm->Chance);
			gfxMovie.Invoke("_root.api.addCrafting", var, 5);
		}
		for(uint32_t k = 0; k < itm->numComponents; ++k)
		{
			var[0].SetUInt(itm->m_itemID);
			var[1].SetUInt(itm->components[k].ComponentCraftingItemID);
			var[2].SetUInt(itm->components[k].ComponentCraftingQuantity);
			gfxMovie.Invoke("_root.api.addCraftingComponent", var, 3);
		}
	}
}

bool HUDCraftingNew::Unload()
{
	gfxMovie.Unload();
	isActive_ = false;
	isInit = false;
	return true;
}

void HUDCraftingNew::Update()
{
}

void HUDCraftingNew::Draw()
{
	gfxMovie.UpdateAndDraw();
}

void HUDCraftingNew::Deactivate()
{
	if(isLockedUI)
		return;

	if(prevKeyboardCaptureMovie)
	{
		prevKeyboardCaptureMovie->SetKeyboardCapture();
		prevKeyboardCaptureMovie = NULL;
	}

	gfxMovie.Invoke("_root.api.hideCraftingNewScreen", "");

	{
		Scaleform::GFx::Value var[1];
		var[0].SetString("menu_close");
		gfxMovie.OnCommandCallback("eventSoundPlay", var, 1);
	}

	if( !g_cursor_mode->GetInt() )
	{
		r3dMouse::Hide();
	}

	isActive_ = false;
}

void HUDCraftingNew::Activate()
{
	r3d_assert(!isActive_);
	r3dMouse::Show();
	isActive_ = true;

	prevKeyboardCaptureMovie = gfxMovie.SetKeyboardCapture();

	PopulateListinfo();
	PopulateBackpackInfo();

	{
		Scaleform::GFx::Value var[1];
		var[0].SetString("menu_open");
		gfxMovie.OnCommandCallback("eventSoundPlay", var, 1);
	}

	gfxMovie.Invoke("_root.api.showCraftingNewScreen", "");
}

void HUDCraftingNew::PopulateBackpackInfo()
{
	gfxMovie.Invoke("_root.api.clearBackpackItem", "");
	{
		obj_Player* plr = gClientLogic().localPlayer_;
		r3d_assert(plr);
		wiCharDataFull& slot = plr->CurLoadout;

		Scaleform::GFx::Value var[2];
		for (int i = 0; i < slot.BackpackSize; i++)
		{
			if (slot.Items[i].itemID != 0)
			{
				var[0].SetUInt(slot.Items[i].itemID);
				var[1].SetInt(slot.Items[i].quantity);
				gfxMovie.Invoke("_root.api.addBackpackItem", var, 2);
			}
		}
	}
}
