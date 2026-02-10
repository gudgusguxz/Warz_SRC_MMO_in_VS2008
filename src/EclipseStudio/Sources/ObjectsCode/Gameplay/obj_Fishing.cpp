#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_Fishing.h"
#include "XMLHelpers.h"

extern bool g_bEditMode;

IMPLEMENT_CLASS(obj_Fishing, "obj_Fishing", "Object");
AUTOREGISTER_CLASS(obj_Fishing);

r3dgameVector(obj_Fishing*) obj_Fishing::LoadedFishinges;

namespace
{
	struct obj_FishingCompositeRenderable: public Renderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			obj_FishingCompositeRenderable *This = static_cast<obj_FishingCompositeRenderable*>(RThis);

			r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH);

			r3dDrawLine3D(This->Parent->GetPosition(), This->Parent->GetPosition() + r3dPoint3D(0, 20.0f, 0), Cam, 0.4f, r3dColor24::yellow);
			r3dDrawCircle3D(This->Parent->GetPosition(), This->Parent->useRadius, Cam, 0.1f, r3dColor::yellow);

			r3dRenderer->Flush();
			r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		}

		obj_Fishing *Parent;	
	};
}

obj_Fishing::obj_Fishing()
{
	useRadius = 10.0f;
}

obj_Fishing::~obj_Fishing()
{
}

#define RENDERABLE_OBJ_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_Fishing::AppendRenderables(RenderArray (& render_arrays  )[ rsCount ], const r3dCamera& Cam)
{
	parent::AppendRenderables(render_arrays, Cam);
#ifdef FINAL_BUILD
	return;
#else
	if(g_bEditMode)
	{
		obj_FishingCompositeRenderable rend;
		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_OBJ_USER_SORT_VALUE;
		render_arrays[ rsDrawDebugData ].PushBack( rend );
	}
#endif
}

void obj_Fishing::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);

	pugi::xml_node objNode = node.child("Fishing_Zone");
	GetXMLVal("useRadius", objNode, &useRadius);
}

void obj_Fishing::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);

	pugi::xml_node objNode = node.append_child();
	objNode.set_name("Fishing_Zone");
	SetXMLVal("useRadius", objNode, &useRadius);
}

BOOL obj_Fishing::Load(const char *fname)
{
#ifndef FINAL_BUILD
	if(!g_bEditMode)
#endif
	{
		return TRUE; // do not load meshes in deathmatch mode, not showing control points
	}
	const char* cpMeshName = "Data\\ObjectsDepot\\Capture_Points\\Flag_Pole_01.sco";

	if(!parent::Load(cpMeshName)) 
		return FALSE;

	return TRUE;
}

BOOL obj_Fishing::OnCreate()
{
	parent::OnCreate();

#ifndef FINAL_BUILD
	if(!g_bEditMode)
#endif
		ObjFlags |= OBJFLAG_SkipCastRay;

#ifndef FINAL_BUILD
	if(g_bEditMode) // to make it easier in editor to edit spawn points
	{
		setSkipOcclusionCheck(true);
		ObjFlags |= OBJFLAG_AlwaysDraw | OBJFLAG_ForceSleep ;
	}
#endif

	LoadedFishinges.push_back(this);

	return 1;
}

BOOL obj_Fishing::OnDestroy()
{
	LoadedFishinges.erase(std::find(LoadedFishinges.begin(), LoadedFishinges.end(), this));
	return parent::OnDestroy();
}

BOOL obj_Fishing::Update()
{
	return parent::Update();
}

//------------------------------------------------------------------------
#ifndef FINAL_BUILD
float obj_Fishing::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{		
		starty += imgui_Static ( scrx, starty, "Fishing Parameters" );
		starty += imgui_Value_Slider(scrx, starty, "Fishing Zone Radius", &useRadius, 0, 500.0f, "%.0f");
	}

	return starty-scry;
}
#endif
