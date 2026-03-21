#pragma once

#include "../Default/framework.h"
#include <process.h>
#include "Engine_Defines.h"
#include "Transform.h"

#pragma comment(lib, "Engine.lib")
#pragma comment(lib, "Imm32.lib")

namespace MapEditor
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;
	static const POINT g_ptCenter = { static_cast<signed long>(g_iWinSizeX * 0.5f), static_cast<signed long>(g_iWinSizeY * 0.5f) };

	enum LEVEL_ID
	{
		LEVEL_STATIC,
		LEVEL_LOADING,
		LEVEL_EDITOR,
		LEVEL_END
	};

	enum CAMERA_MODE
	{
		CM_DEBUG,
		CM_MAIN,
		CM_CUTSCENE,
		CM_END
	};

	enum SoundChannel
	{
		SCH_BGM,
		SCH_EFFECT_PLAYER,
		SCH_SHOOK,
		SCH_PAK,
		SCH_JUMP,
		SCH_STEP_L,
		SCH_STEP_R,
		SCH_EFFECT_SKILL,
		SCH_EFFECT_SKILL1,
		SCH_EFFECT_SKILL2,
		SCH_EFFECT_SKILL3,
		SCH_EFFECT_SKILL4,
		SCH_EFFECT_SKILL5,
		SCH_EFFECT_SKILL6,
		SCH_EFFECT_SKILL7,
		SCH_EFFECT_SKILL8,
		SCH_EFFECT_SKILL9,
		SCH_EFFECT_SKILL10,
		SCH_EFFECT_SKILL11,
		SCH_EFFECT,
		SCH_EFFECT2,
		SCH_EFFECT3,
		SCH_AMBIENT,
		SCH_BUTTON,
		SCH_NPC,
		SCH_NPC2,
		SCH_EFFECT_MONSTER0,
		SCH_EFFECT_MONSTER1,
		SCH_EFFECT_MONSTER2,
		SCH_EFFECT_MONSTER3,
		SCH_EFFECT_MONSTER4,
		SCH_EFFECT_MONSTER5,
		SCH_EFFECT_MONSTER6,
		SCH_EFFECT_MONSTER7,
		SCH_EFFECT_MONSTER8,
		SCH_EFFECT_MONSTER9,
		SCH_EFFECT_MONSTER10,
		SCH_EFFECT_MONSTER11,
		SCH_EFFECT_MONSTER12,
		SCH_EFFECT_MONSTER13,
		SCH_EFFECT_MONSTER14,
		SCH_EFFECT_MONSTER15,
		SCH_EFFECT_MONSTER16,
		SCH_EFFECT_MONSTER17,
		SCH_EFFECT_MONSTER18,
		SCH_EFFECT_MONSTER19,
		SCH_MAX
	};

	struct ObjectInfo
	{
		wstring strPrototypeTag{};
		_float4 vPos{_float4(0.f, 0.f, 0.f, 1.f)};
		_float4 vLook{_float4(0.f, 0.f, 1.f, 0.f)};
	};

	struct EffectInfo
	{
		_float4 vColor{};
		_float fScale{};
		_float4 vPos{};
		_uint iType{};
	};

	struct AttachmentInfo
	{
		CTransform* pParent{ nullptr };
		_float44* pSocket{ nullptr };
		_float44 PivotMatrix{};
	};

#pragma region Shader Passes
	enum AnimMeshPass
	{
		AnimPass_Default,
		AnimPass_OutLine,
		AnimPass_Shadow,
		AnimPass_Dissolve,
	};

	enum StaticMeshPass
	{
		StaticPass_Default,
		StaticPass_Wire,
		StaticPass_NonLight,
		StaticPass_OutLine,
		StaticPass_AlphaTestMeshes,
		StaticPass_Sky,
		StaticPass_COLMesh,
		StaticPass_SingleColorFx,
		StaticPass_SingleColorDissolve,
		StaticPass_Fireball,
		StaticPass_MaskEffect,
		StaticPass_MaskDissolve,
		StaticPass_MaskEffectClamp,
		StaticPass_SingleColoredEffectFrontCull,
		StaticPass_Shadow,
	};

	enum VNTPass
	{
		VNTPass_Terrain,
		VNTPass_Terrain_Editor,
	};

	enum VTPass
	{
		VTPass_UI,
		VTPass_UI_Alpha,
		VTPass_UI_Color_Alpha,
		VTPass_Button,
		VTPass_Background,
		VTPass_Mask_Texture,
		VTPass_Inv_Mask_Texture,
		VTPass_Mask_Color,
		VTPass_Mask_ColorAlpha,
		VTPass_HP,
		VTPass_Hit,
		VTPass_Sprite,
		VTPass_Hell,
	};

	enum InstancingPass
	{
		InstPass_Particle_TextureMask,
		InstPass_Particle_Sprite_Color,
		InstPass_Particle_MaskColor,
	};

	enum CAMERA_STATE
	{
		CS_DEFAULT,
		CS_ZOOM,
		CS_INVEN,
		CS_SHOP,
		CS_SKILLBOOK,
		CS_ENDFULLSCREEN,
		CS_WORLDMAP,
		CS_STATEEND
	};

	struct SectionInfo
	{
		_vec4 vStartCutScene{};
		_vec4 vEndCutScene{};
		wstring strSectionName{};
		_float fCurveSpeed{ 0.f };
		//class CCutScene_Curve** ppCurve{ nullptr };
		_uint iSectionType{};
	};
#pragma endregion

}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace MapEditor;