#pragma once

#include "APIScaleformGfx.h"

class HUDCraftingNew
{
	bool	isActive_;
	bool	isInit;

	bool	isLockedUI;
	int		lockedOp;

private:
	r3dScaleformMovie gfxMovie;
	r3dScaleformMovie* prevKeyboardCaptureMovie;

	void PopulateListinfo();
	int		CraftingResult;
	uint32_t recordID;
public:
	HUDCraftingNew();
	~HUDCraftingNew();

	bool 	Init();
	bool 	Unload();

	bool	IsInited() const { return isInit; }

	void 	Update();
	void 	Draw();

	bool	isActive() const { return isActive_; }
	void	Activate();
	void	Deactivate();

	bool	isUILocked() const { return isLockedUI; }
	
	void	setErrorMsg(const char* errMsg);
	void	StartCraftingUI(int Result);
	void	PopulateBackpackInfo();
	void	eventCraftingItem(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventSetisProcess(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
};
