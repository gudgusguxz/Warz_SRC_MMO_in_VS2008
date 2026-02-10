// ---------------------------------------------------------------------------
// Rocket.h
// Rocket Class from WarInc ported to WarZ by SAIGA308
//
// Author: Online Warmongers LLC, Arktos Entertainment Group
// Ported to WarZ by: SAIGA308
// Port date: Jan-04-2014
//
// NOTE FROM SAIGA308
// TODO: This just adds obj_Rocket and obj_RocketGrenade class. You still have
// to do a lot of coding to make this fully work in game. This is just a starting
// point for you to do what you want with (i.e. RPG).
//
// Entries still need to be made in P2PMessages, AI_Player, AI_PlayerAnim,
// Weapon, WeaponArmory, WeaponConfig, classes.
//
// Additional entries for stores would need to be made in UserProfile, 
// FrontEndShared, HUDPause, HUD_Character, HUD_TPSGame, UIItemInventory
// classes respectively.
//
// I in no way want credit for this, nor do I want to see anyone fighting over
// credit. If you want glory, code your own game from scratch. Hacking a few
// hundred lines into someone elses code and then bitching about "your work"
// being stolen and not receiving credit is as stupid as it gets. If you are
// one of these people, go fuck yourself!
// ---------------------------------------------------------------------------
#ifndef __ROCKET_H__
#define __ROCKET_H__

#include "GameCommon.h"
#include "Ammo.h"

class obj_Rocket : public AmmoShared
{
	DECLARE_CLASS(obj_Rocket, AmmoShared)
	friend Ammo;
	r3dPoint3D	m_CollisionPoint;
	r3dPoint3D  m_CollisionNormal;
	//bool		m_hasCollision;
	float		m_DistanceTraveled;
	bool		m_isFlying;
	bool		m_DisableDistanceTraveled;

	void		onHit(const r3dPoint3D& pos, const r3dPoint3D& norm, const r3dPoint3D& fly_dir);
public:
	obj_Rocket();

	virtual	BOOL		OnCreate();
	virtual BOOL		OnDestroy();
	virtual	void		OnCollide(PhysicsCallbackObject *tobj, CollisionInfo &trace);
	virtual	BOOL		Update();

	virtual r3dMesh*	GetObjectMesh();
	virtual r3dMesh*	GetObjectLodMesh() OVERRIDE;
};

// Rocket Grenade
// Same as rocket, but doesn't have distance traveled requirement
class obj_RocketGrenade : public obj_Rocket
{
	DECLARE_CLASS(obj_RocketGrenade, obj_Rocket)
public:
	obj_RocketGrenade() { m_DisableDistanceTraveled = true; }
};

#endif	// __ROCKET_H__
