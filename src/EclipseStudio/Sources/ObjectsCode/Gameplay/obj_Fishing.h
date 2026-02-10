#pragma once

#include "GameCommon.h"

class obj_Fishing : public MeshGameObject
{
	DECLARE_CLASS(obj_Fishing, MeshGameObject)
	
public:
	float		useRadius;
		
	static r3dgameVector(obj_Fishing*) LoadedFishinges;
public:
	obj_Fishing();
	virtual ~obj_Fishing();

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();
 #ifndef FINAL_BUILD
 	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
 #endif
	virtual	void		AppendRenderables(RenderArray (& render_arrays  )[ rsCount ], const r3dCamera& Cam);
	virtual void		WriteSerializedData(pugi::xml_node& node);
	virtual	void		ReadSerializedData(pugi::xml_node& node);

};
