#ifndef __ARROWEXPLOSIVE_H__
#define __ARROWEXPLOSIVE_H__

#include "GameCommon.h"
#include "Ammo.h"

class obj_ArrowExplosive : public AmmoShared
{
	DECLARE_CLASS(obj_ArrowExplosive, AmmoShared)
	friend Ammo;
	r3dPoint3D	m_CollisionPoint;
	r3dPoint3D  m_CollisionNormal;
	float		m_DistanceTraveled;
	bool		m_isFlying;
	bool		m_DisableDistanceTraveled;

	void		onHit(const r3dPoint3D& pos, const r3dPoint3D& norm, const r3dPoint3D& fly_dir);
public:
	obj_ArrowExplosive();

	virtual	BOOL		OnCreate();
	virtual BOOL		OnDestroy();
	virtual	void		OnCollide(PhysicsCallbackObject *tobj, CollisionInfo &trace);
	virtual	BOOL		Update();

	virtual r3dMesh*	GetObjectMesh();
	virtual r3dMesh*	GetObjectLodMesh() OVERRIDE;
};


class obj_ArrowExplosiveGrenade : public obj_ArrowExplosive
{
	DECLARE_CLASS(obj_ArrowExplosiveGrenade, obj_ArrowExplosive)
public:
	obj_ArrowExplosiveGrenade() { m_DisableDistanceTraveled = true; }
};

#endif	
