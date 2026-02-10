#pragma once

#include "APIScaleformGfx.h"

class HUDMark
{

public:
	HUDMark();
	~HUDMark();
	
	enum MarkerTypeEnum
	{
		MarkType = 0,
		MarkType_1,
		MarkType_2,
		MarkType_3,
	};

	bool 	Init();
	bool	IsInited () const { return m_bInited; }
	bool 	Unload();

	void 	Update();
	void 	Draw();

	void	showHUDIcon(class obj_Player* from, int id, const r3dVector& pos, const char* markerDetail);
private:
	void	addMarker(int id);
	bool	m_bInited;

};
