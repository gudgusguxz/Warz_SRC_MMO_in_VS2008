#pragma once

#include "GameCommon.h"

class obj_ServerFishing : public GameObject
{
	DECLARE_CLASS(obj_ServerFishing, GameObject)

public:
	float		useRadius;

public:
	obj_ServerFishing();
	~obj_ServerFishing();

	virtual BOOL	OnCreate();
	virtual	void	ReadSerializedData(pugi::xml_node& node);
};

class FishingZoneMgr
{
public:
	enum { MAX_FISHING_ZONE = 256 }; // 256 should be more than enough, if not, will redo into vector
	obj_ServerFishing* FishingZone_[MAX_FISHING_ZONE];
	int		numFishingZone_;

	void RegisterFishingZone(obj_ServerFishing* fbox) 
	{
	r3d_assert(numFishingZone_ < MAX_FISHING_ZONE);
		FishingZone_[numFishingZone_++] = fbox;
	}

public:
	FishingZoneMgr() { numFishingZone_ = 0; }
	~FishingZoneMgr() {}
};

extern	FishingZoneMgr gFishingZoneMngr;
