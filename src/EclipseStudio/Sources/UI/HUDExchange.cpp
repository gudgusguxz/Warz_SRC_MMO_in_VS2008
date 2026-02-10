#include "r3dPCH.h"
#include "r3d.h"

#include "../../../Eternity/sf/Console/config.h"
#include "HUDExchange.h"
#include "LangMngr.h"

#include "FrontendShared.h"

#include "../multiplayer/clientgamelogic.h"
#include "../ObjectsCode/AI/AI_Player.H"
#include "../ObjectsCode/weapons/Weapon.h"
#include "../ObjectsCode/weapons/WeaponArmory.h"
#include "../GameLevel.h"

#include "CkHttpRequest.h"
#include "CkHttpResponse.h"
#include "backend/HttpDownload.h"
#include "backend/WOBackendAPI.h"

HUDExchange::HUDExchange()
{
	isActive_ = false;
	isInit = false;
	prevKeyboardCaptureMovie = NULL;
}

HUDExchange::~HUDExchange()
{
}

void HUDExchange::eventExchangeItem(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==1);
	uint32_t recordID = args[0].GetUInt();
	Scaleform::GFx::Value var[2];

	obj_Player* plr = gClientLogic().localPlayer_;
	r3d_assert(plr);
	// check if we have enough components
	{
		const ExchangeConfig* cfg = g_pWeaponArmory->getExchangeConfig(recordID);
		r3d_assert(cfg);  
		bool notEnough = false;
		for(uint32_t i=0; i<cfg->numComponents; ++i)
		{
			if(!plr->CurLoadout.hasItemWithQuantity(cfg->components[i].NeededItemID, cfg->components[i].NeededQuantity))
			{
				notEnough = true;
				break;
			}
		}
		if(notEnough)
		{
			var[0].SetString(gLangMngr.getString("$Exchange_Failed"));
			var[1].SetBoolean(true);
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
			return;
		}
	}

	// lock UI
	isLockedUI = true;
	var[0].SetString("");
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

 	PKT_C2S_ExchangeItem_s n;
 	n.recordID= recordID;
 	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
}

void HUDExchange::eventReturnToGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	Deactivate();
}

void HUDExchange::setErrorMsg(const char* errMsg)
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

void HUDExchange::unlockUI()
{
	r3d_assert(isLockedUI);
	isLockedUI = false;
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	lockedOp = 0;
}

bool HUDExchange::Init()
{
	if(!gfxMovie.Load("Data\\Menu\\WarZ_HUD_Event.swf", false)) 
		return false;

#define MAKE_CALLBACK(FUNC) game_new r3dScaleformMovie::TGFxEICallback<HUDExchange>(this, &HUDExchange::FUNC)
	gfxMovie.RegisterEventHandler("eventExchangeItem", MAKE_CALLBACK(eventExchangeItem));
	gfxMovie.RegisterEventHandler("eventReturnToGame", MAKE_CALLBACK(eventReturnToGame));

	gfxMovie.SetCurentRTViewport( Scaleform::GFx::Movie::SM_ExactFit );

	// add items
	{
		addItemCategoriesToUI(gfxMovie);
		addItemsAndCategoryToUI(gfxMovie);
	}

	isActive_ = false;
	isInit = true;
	isLockedUI = false;
	lockedOp = 0;

	r3dgameVector(const ExchangeConfig*) AllExchanges;
	Scaleform::GFx::Value var[5];
	g_pWeaponArmory->startItemSearch();
	while(g_pWeaponArmory->searchNextItem())
	{
		uint32_t itemID = g_pWeaponArmory->getCurrentSearchItemID();
		const ExchangeConfig* ExchangeRecConfig = g_pWeaponArmory->getExchangeConfig(itemID);
		if(ExchangeRecConfig)
		{
			AllExchanges.push_back(ExchangeRecConfig);
		}
	}
	const size_t ExchangeRecSize = AllExchanges.size();
	for(size_t i = 0; i < ExchangeRecSize; ++i)
	{
		const ExchangeConfig* cfg = AllExchanges[i];
		
		var[0].SetUInt(cfg->m_itemID);
		var[1].SetUInt(cfg->ExchangedItemID);
		var[2].SetUInt(cfg->ExchangedQuantity);

		int UseCount = 0;
		int MaxUse = 0;
		
		var[3].SetInt(UseCount);
		var[4].SetInt(MaxUse);
		gfxMovie.Invoke("_root.api.addExchange", var, 5);

		for(uint32_t k=0; k<cfg->numComponents; ++k)
		{
			var[1].SetUInt(cfg->components[k].NeededItemID);
			var[2].SetUInt(cfg->components[k].NeededQuantity);
			gfxMovie.Invoke("_root.api.addExchangeComponent", var, 3);
		}
	}
	gfxMovie.Invoke("_root.api.Main.refreshRecipeList", "");

	return true;
}

bool HUDExchange::Unload()
{
	gfxMovie.Unload();
	isActive_ = false;
	isInit = false;
	return true;
}

void HUDExchange::Update()
{
}

void HUDExchange::Draw()
{
	gfxMovie.UpdateAndDraw();
}

void HUDExchange::Deactivate()
{
	if(isLockedUI)
		return;

	if(prevKeyboardCaptureMovie)
	{
		prevKeyboardCaptureMovie->SetKeyboardCapture();
		prevKeyboardCaptureMovie = NULL;
	}

	gfxMovie.Invoke("_root.api.hideCraftScreen", "");

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

extern const char* getReputationString(int reputation);
void HUDExchange::Activate()
{
	r3d_assert(!isActive_);
	r3dMouse::Show();
	isActive_ = true;

	prevKeyboardCaptureMovie = gfxMovie.SetKeyboardCapture(); // for mouse scroll events
	UpdateExchangeItemID();

	gfxMovie.Invoke("_root.api.showExchangeMenuScreen", "");
}

void HUDExchange::UpdateExchangeItemID()
{
	CWOBackendReq req(&gUserProfile, "api_Exchange.aspx");
	req.AddParam("func", "read");
	if(req.Issue())
	{
		pugi::xml_document xmlFile;
		req.ParseXML(xmlFile);
		pugi::xml_node xmlExchange = xmlFile.child("Exchange");
		xmlExchange = xmlExchange.first_child();

		while(!xmlExchange.empty())
		{
			int ItemID = xmlExchange.attribute("itemID").as_int();
			int UseCount = xmlExchange.attribute("UseCount").as_int();
			int MaxUse = xmlExchange.attribute("MaxUse").as_int();

			r3dOutToLog("xmlExchange %d %d/%d\n", ItemID, UseCount, MaxUse);

			Scaleform::GFx::Value var[3];
			var[0].SetInt(ItemID);
			var[1].SetInt(UseCount);
			var[2].SetInt(MaxUse);
			gfxMovie.Invoke("_root.api.Main.UpdateExchangeItemID", var, 3);

			xmlExchange = xmlExchange.next_sibling();
		}
	}
}