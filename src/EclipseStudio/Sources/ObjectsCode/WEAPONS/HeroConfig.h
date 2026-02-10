#pragma once

#include "BaseItemConfig.h"

class HeroConfig : public BaseItemConfig
{
private:
	mutable r3dMesh* m_HairMeshes; 
	mutable r3dMesh* m_GearFPSMeshes;
	mutable r3dgameVector(r3dMesh*) m_HeadMeshes;
	mutable r3dgameVector(r3dMesh*) m_LegsMeshes;
	mutable r3dgameVector(r3dMesh*) m_BodyMeshes;
	mutable r3dgameVector(r3dMesh*) m_BodyMeshesFPS;

	char*				m_BaseModelName;
public:

	float m_damagePerc;
	float m_damageMax;

	int   m_ProtectionLevel;
    bool  m_CharHair;
	bool  m_FemaleHero;
public:
	HeroConfig(uint32_t id) : BaseItemConfig(id)
	{
		m_BaseModelName = NULL;
		m_CharHair = false;
		m_HairMeshes = NULL;
		m_FemaleHero = false;
		m_GearFPSMeshes = NULL;
	}
	virtual ~HeroConfig() 
	{
		free(m_BaseModelName);
	}

	virtual bool loadBaseFromXml(pugi::xml_node& xmlGear);

	// called when unloading level
	virtual void resetMesh();

	r3dMesh*	getHairMesh() const ;
	r3dMesh*	getHeadMesh(int index) const ;
	r3dMesh*	getLegMesh(int index) const ;
	r3dMesh*	getBodyMesh(int index, bool isFPS, const char* GearFPSModelName = "" ) const;

	size_t		getNumHeads() const;
	size_t		getNumLegs() const;
	size_t		getNumBodys() const;
};
