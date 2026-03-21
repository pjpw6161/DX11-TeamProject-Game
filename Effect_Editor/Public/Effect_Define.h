#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "../Default/framework.h"
#include <process.h>

#pragma comment(lib, "Engine.lib")

namespace Effect
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

	enum LEVEL_ID
	{
		LEVEL_STATIC,
		LEVEL_EDITOR,
		LEVEL_LOGO,
		LEVEL_CREATECHARACTER,
		LEVEL_FOREST,
		LEVEL_VILLAGE,
		LEVEL_CLOUD,
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
		StaticPass_Water,
		StaticPass_Worldmap_Water,
		StaticPass_Worldmap_Cloud,
		StaticPsss_Rim,
		StaticPass_DiffEffect,
		StaticPass_SingleColorAlpha,
		StaticPass_MaskAlpha,
		StaticPass_DiffAlpha,
		StaticPass_Distortion,
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
		VTPass_Dust,
		VTPass_Dissolve,
		VTPass_MP,
		VTPass_BLUR,
		VTPass_Bright,
		VTPass_ScrollAlpha,
		VTPass_HPNoMask,
		VTPass_NineSlice,
		VTPass_FadeVertical,
		VTPass_FadeHorizontal,
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
		InstPass_Particle_TextureMask_Trail,
		InstPass_Particle_Sprite_Color_Trail,
		InstPass_Particle_MaskColor_Trail,
		InstPass_Particle_TextureMask_Dissolve_Trail,
		InstPass_Particle_Sprite_Color_Dissolve_Trail,
		InstPass_Particle_MaskColor_Dissolve_Trail,
		InstPass_Particle_Sprite_Texture_Trail,
		InstPass_Particle_Sprite_Texture_Dissolve_Trail,
		InstPass_Particle_Sprite_Texture_RandomIndex_Trail,
		InstPass_Particle_Sprite_Texture_RandomIndex_Dissolve_Trail,
		InstPass_Particle_Sprite_Color_RandomIndex_Trail,
		InstPass_Particle_Sprite_Color_RandomIndex_Dissolve_Trail,
		InstPass_Particle_Sprite_DiffMask,
		InstPass_Particle_Sprite_DiffMask_Dissolve,
		InstPass_Particle_Sprite_DiffMask_RandomIndex,
		InstPass_Particle_Sprite_DiffMask_RandomIndex_Dissolve,
		InstPass_Particle_Sprite_DiffMask_Trail,
		InstPass_Particle_Sprite_DiffMask_Dissolve_Trail,
		InstPass_Particle_Sprite_DiffMask_RandomIndex_Trail,
		InstPass_Particle_Sprite_DiffMask_RandomIndex_Dissolve_Trail,
		InstPass_Particle_MaskColor_Alpha,
		InstPass_Particle_Distortion,
		InstPass_End,
	};
#pragma endregion
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace Effect;