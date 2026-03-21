#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "../Default/framework.h"
#include <process.h>
#pragma comment(lib, "Engine.lib")

namespace ShaderEdit
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

	enum LEVEL_ID
	{
		LEVEL_STATIC,
		LEVEL_LOADING,
		LEVEL_ShaderTest,
		LEVEL_END
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
		StaticPass_End,
	};

	enum VNTPass
	{
		VNTPass_Terrain,
		VNTPass_Terrain_Editor,
		VNTPass_Terrain_Effect,
	};

	enum VTPass
	{
		VTPass_UI,
		VTPass_UI_Alpha,
		VTPass_UI_Color_Alpha,
		VTPass_Button,
		VTPass_Background,
		VTPass_Background_Mask,
		VTPass_Mask_Texture,
		VTPass_Inv_Mask_Texture,
		VTPass_Mask_Color,
		VTPass_Mask_ColorAlpha,
		VTPass_HP,
		VTPass_Hit,
		VTPass_Sprite,
		VTPass_SpriteMaskTexture,
		VTPass_SpriteMaskColor,
		VTPass_Hell,
		VTPass_Mask_Texture_Dissolve,
		VTPass_Inv_Mask_Texture_Dissolve,
		VTPass_Mask_Color_Dissolve,
		VTPass_Sprite_Dissolve,
		VTPass_SpriteMaskTexture_Dissolve,
		VTPass_SpriteMaskColor_Dissolve,
		VTPass_End
	};

	enum InstancingPass
	{
		InstPass_Particle_TextureMask,
		InstPass_Particle_Sprite_Color,
		InstPass_Particle_MaskColor,
		InstPass_Particle_TextureMask_Dissolve,
		InstPass_Particle_Sprite_Color_Dissolve,
		InstPass_Particle_MaskColor_Dissolve,
		InstPass_Particle_Sprite_Texture,
		InstPass_Particle_Sprite_Texture_Dissolve,
		InstPass_Particle_Sprite_Texture_RandomIndex,
		InstPass_Particle_Sprite_Texture_RandomIndex_Dissolve,
		InstPass_Particle_Sprite_Color_RandomIndex,
		InstPass_Particle_Sprite_Color_RandomIndex_Dissolve,
		InstPass_End,
	};
#pragma endregion

	enum PART_TYPE
	{
		PT_HAIR,
		PT_FACE,
		PT_BODY,
		PT_HELMET,
		PT_WEAPON,
		PT_END
	};

	enum WEAPON_TYPE
	{
		WP_BOW,
		WP_SWORD,
		WP_END
	};

	enum WEAPON_INDEX
	{
		BOW0,
		BOW1,
		BOW2,
		SWORD0,
		SWORD1,
		SWORD2,
		WP_INDEX_END
	};
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace ShaderEdit;