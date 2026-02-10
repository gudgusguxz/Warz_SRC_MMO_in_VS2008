#include "r3dpch.h"
#include "r3d.h"

#include "GameCommon.h"
#include "Ammo.h"
#include "ObjectsCode\Effects\obj_ParticleSystem.h"
#include "ArrowExplosive.h"
#include "..\world\DecalChief.h"
#include "..\world\MaterialTypes.h"
#include "ExplosionVisualController.h"

#include "..\AI\AI_Player.H"
#include "WeaponConfig.h"
#include "Weapon.h"

#include "multiplayer/P2PMessages.h"
#include "..\..\multiplayer\ClientGameLogic.h"

IMPLEMENT_CLASS(obj_ArrowExplosive, "obj_ArrowExplosive", "Object");
AUTOREGISTER_CLASS(obj_ArrowExplosive);
IMPLEMENT_CLASS(obj_ArrowExplosiveGrenade, "obj_ArrowExplosiveGrenade", "Object");
AUTOREGISTER_CLASS(obj_ArrowExplosiveGrenade);

obj_ArrowExplosive::obj_ArrowExplosive()
{
	m_Ammo = NULL;
	m_Weapon = 0;
	m_ParticleTracer = NULL;
	m_DisableDistanceTraveled = true;
}

r3dMesh* obj_ArrowExplosive::GetObjectMesh()
{
	r3d_assert(m_Ammo);
	return getModel();
}

r3dMesh*
obj_ArrowExplosive::GetObjectLodMesh() 
{
	r3d_assert(m_Ammo);
	return getModel();
}

BOOL obj_ArrowExplosive::OnCreate()
{
	const GameObject* owner = GameWorld().GetObject(ownerID);
	if(!owner)
		return FALSE;
		
	m_isSerializable = false;

	ReadPhysicsConfig();
	PhysicsConfig.group = PHYSCOLL_PROJECTILES;

	PhysicsConfig.isKinematic = true; 
	PhysicsConfig.isDynamic = true;

	r3d_assert(m_Ammo);
	r3d_assert(m_Weapon);
	parent::OnCreate();

	m_CreationTime = r3dGetTime();
	m_CreationPos = GetPosition();

	if(m_Ammo->getParticleTracer())
		m_ParticleTracer = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", m_Ammo->getParticleTracer(), GetPosition() );

	r3dBoundBox bboxLocal ;

	bboxLocal.Org.Assign( -0.5f, -0.25f, -0.25f );
	bboxLocal.Size.Assign(1.0f, 0.1f, 0.1f);

	SetBBoxLocal( bboxLocal ) ;

	UpdateTransform();

	m_AppliedVelocity = m_FireDirection*m_Weapon->m_AmmoSpeed;// * 2;

	m_DistanceTraveled = 0;
	m_isFlying = true;

	return TRUE;
}

BOOL obj_ArrowExplosive::OnDestroy()
{
	if(m_ParticleTracer)
		m_ParticleTracer->bKillDelayed = 1;

	return parent::OnDestroy();
}

void obj_ArrowExplosive::onHit(const r3dPoint3D& pos, const r3dPoint3D& norm, const r3dPoint3D& fly_dir)
{
	setActiveFlag(0);

	const GameObject* owner = GameWorld().GetObject(ownerID);
	if(!owner)
		return;

	DecalParams params;
	params.Dir		= norm;
	params.Pos		= pos;
	params.TypeID	= GetDecalID( r3dHash::MakeHash(""), r3dHash::MakeHash(m_Weapon->m_PrimaryAmmo->getDecalSource()) );
	if( params.TypeID != INVALID_DECAL_ID )
		g_pDecalChief->Add( params );

	SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash(m_Weapon->m_PrimaryAmmo->getDecalSource()), GetPosition(), r3dPoint3D(0,1,0));

	gExplosionVisualController.AddExplosion(GetPosition(), m_Weapon->m_AmmoArea);

	r3d_assert(owner);
	r3dVector center = GetPosition()-fly_dir*0.25f;
	if(owner->NetworkLocal)
	{
		PKT_C2C_PlayerHitNothing_s n;
		p2pSendToHost(owner, &n, sizeof(n), true);

		obj_Player* plr = (obj_Player*)owner;
		for(int i=0; i<NUM_WEAPONS_ON_PLAYER; ++i)
		{
			if(plr->m_Weapons[i] && plr->m_Weapons[i]->getItemID() == m_Weapon->m_itemID)
			{
				gClientLogic().ApplyExplosionDamage(center, m_Weapon->m_AmmoArea, i);
				break;
			}
		}
	}
}

void obj_ArrowExplosive::OnCollide(PhysicsCallbackObject *tobj, CollisionInfo &trace)
{
	return;
}

void MatrixGetYawPitchRoll ( const D3DXMATRIX & , float &, float &, float & );

class ArrowexplosiveFilterCallback : public PxSceneQueryFilterCallback
{
	const GameObject* owner;
public:
	ArrowexplosiveFilterCallback(const GameObject* _owner) : owner(_owner) {};

	virtual PxSceneQueryHitType::Enum preFilter(const PxFilterData& filterData, PxShape* shape, PxSceneQueryFilterFlags& filterFlags)
	{
		if(shape)
		{
			PxRigidActor& actor = shape->getActor();
			PhysicsCallbackObject* target = static_cast<PhysicsCallbackObject*>(actor.userData);
			if(target)
			{
				GameObject *gameObj = target->isGameObject();
				if(gameObj)
				{
					if(gameObj == owner)
						return PxSceneQueryHitType::eNONE;
				}
			}
		}
		return PxSceneQueryHitType::eBLOCK;
	}

	virtual PxSceneQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxSceneQueryHit& hit)
	{
		return PxSceneQueryHitType::eBLOCK;
	}
};

BOOL obj_ArrowExplosive::Update()
{
	parent::Update();
	if(m_CreationTime+30.0f < r3dGetTime())
	{
		const GameObject* owner = GameWorld().GetObject(ownerID);
		if(owner && owner->NetworkLocal)
		{
			PKT_C2C_PlayerHitNothing_s n;
			p2pSendToHost(owner, &n, sizeof(n), true);
		}
		return FALSE;
	}

	r3dVector dir = (GetPosition() - oldstate.Position);
	if(dir.Length()==0)
		dir = m_FireDirection;

	if(m_isFlying)
	{
		m_AppliedVelocity += r3dVector(0, -9.81f, 0) * m_Weapon->m_AmmoMass * r3dGetFrameTime();
		r3dPoint3D motion = m_AppliedVelocity * r3dGetFrameTime();
		float motionLen = motion.Length();
		motion.Normalize();
		m_DistanceTraveled += motionLen;

		PxU32 collisionFlag = COLLIDABLE_STATIC_MASK;
		const GameObject* owner = GameWorld().GetObject(ownerID);
		collisionFlag |= (1<<PHYSCOLL_NETWORKPLAYER); 

		PxSphereGeometry sphere(0.2f);
		PxTransform pose(PxVec3(GetPosition().x, GetPosition().y, GetPosition().z), PxQuat(0,0,0,1));

		PxSweepHit hit;
		PxSceneQueryFilterData filter(PxFilterData(collisionFlag, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC|PxSceneQueryFilterFlag::ePREFILTER);
		ArrowexplosiveFilterCallback callback(owner);
		if(g_pPhysicsWorld->PhysXScene->sweepSingle(sphere, pose, PxVec3(motion.x, motion.y, motion.z), motionLen, PxSceneQueryFlag::eINITIAL_OVERLAP|PxSceneQueryFlag::eIMPACT|PxSceneQueryFlag::eNORMAL, hit, filter, &callback))
		{
			m_CollisionPoint = r3dPoint3D(hit.impact.x, hit.impact.y, hit.impact.z);
			m_CollisionNormal = r3dPoint3D(hit.normal.x, hit.normal.y, hit.normal.z);
			if(m_DistanceTraveled > 10.0f || m_DisableDistanceTraveled)
			{
				onHit(m_CollisionPoint, m_CollisionNormal, dir);
				return FALSE;
			}
			else
			{
				m_isFlying = false;
				r3d_assert(PhysicsObject->getPhysicsActor()->isRigidDynamic());
				PhysicsObject->getPhysicsActor()->isRigidDynamic()->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, false);
				PhysicsObject->addImpulse(m_AppliedVelocity/5);
			}
			
		}
	}

	if(m_isFlying)
	{
		SetPosition(GetPosition() + m_AppliedVelocity * r3dGetFrameTime());
	}

	if(m_CreationTime+3.0f>r3dGetTime()) 
	{
		m_AppliedVelocity += m_AppliedVelocity * r3dGetFrameTime();
	}

	if(m_ParticleTracer)
		m_ParticleTracer->SetPosition(GetPosition());

	return TRUE;
}

