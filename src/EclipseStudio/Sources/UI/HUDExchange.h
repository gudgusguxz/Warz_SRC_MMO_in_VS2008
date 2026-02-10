#pragma once

#include "APIScaleformGfx.h"

class HUDExchange
{
	bool	isActive_;
	bool	isInit;

	bool	isLockedUI;
	int		lockedOp;

private:
	r3dScaleformMovie gfxMovie;
	r3dScaleformMovie* prevKeyboardCaptureMovie;

public:
	HUDExchange();
	~HUDExchange();

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
	void	unlockUI();

	void	UpdateExchangeItemID();

	void	eventReturnToGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount); 
	void	eventExchangeItem(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount); 
};
