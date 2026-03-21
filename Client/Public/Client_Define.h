#pragma once

#include "../Default/framework.h"
#include <process.h>
#include "Engine_Defines.h"
#include "Transform.h"
#include <Windows.h>
#include <Commdlg.h>
#include "Model.h"
#pragma comment(lib, "Engine.lib")
#pragma comment(lib, "Imm32.lib")

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;
	static const POINT g_ptCenter = { static_cast<signed long>(g_iWinSizeX * 0.5f), static_cast<signed long>(g_iWinSizeY * 0.5f) };

	static const LIGHT_DESC g_Light_Custom
	{
		LIGHT_DESC::Directional,
		_vec4(0.f, 0.f, -1.f, 0.f), // Direction
		_vec4(), // Position
		_vec4(), // Attenuation

		_vec4(1.f), // Diffuse
		_vec4(0.5f), // Ambient
		_vec4(0.2f) // Specular
	};

	static const LIGHT_DESC g_Light_Tutorial
	{
		LIGHT_DESC::Directional,
		_vec4(-1.f, -2.f, -1.f, 0.f), // Direction
		_vec4(), // Position
		_vec4(), // Attenuation

		_vec4(0.2f), // Diffuse
		_vec4(0.01f), // Ambient
		_vec4(1.f) // Specular
	};

	static const LIGHT_DESC g_Light_Village
	{
		LIGHT_DESC::Directional,
		_vec4(-1.f, -2.f, -1.f, 0.f), // Direction
		_vec4(), // Position
		_vec4(), // Attenuation

		_vec4(1.f), // Diffuse
		_vec4(0.3f), // Ambient
		_vec4(0.5f) // Specular
	};

	static const LIGHT_DESC g_Light_Dungeon
	{
		LIGHT_DESC::Directional,
		_vec4(-1.f, -2.f, -1.f, 0.f), // Direction
		_vec4(), // Position
		_vec4(), // Attenuation

		_vec4(0.05f), // Diffuse
		_vec4(0.01f), // Ambient
		_vec4(0.1f) // Specular
	};

	static const LIGHT_DESC g_Light_Dragon
	{
		LIGHT_DESC::Directional,
		_vec4(-1.f, -2.f, -1.f, 0.f), // Direction
		_vec4(), // Position
		_vec4(), // Attenuation

		_vec4(0.936f, 0.728f, 0.650f, 0.f), // Diffuse
		_vec4(0.5f), // Ambient
		_vec4(1.f, 0.4f, 0.15f, 0.f) // Specular
	};

	static const LIGHT_DESC g_Light_Survival
	{
		LIGHT_DESC::Directional,
		_vec4(-1.f, -2.f, -1.f, 0.f), // Direction
		_vec4(), // Position
		_vec4(), // Attenuation

		_vec4(0.45f), // Diffuse
		_vec4(0.5f), // Ambient
		_vec4(0.1f) // Specular
	};

	enum LEVEL_ID
	{
		LEVEL_STATIC,
		LEVEL_LOADING,
		LEVEL_LOGO,
		LEVEL_SELECT,
		LEVEL_CUSTOM,
		LEVEL_GAMEPLAY,
		LEVEL_VILLAGE,
		LEVEL_END
	};

	enum CAMERA_MODE
	{
		CM_DEBUG,
		CM_MAIN,
		CM_SELECT,
		CM_CUSTOM,
		CM_CUTSCENE,
		CM_END
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
		CS_BRICKGAME,
		CS_STATEEND
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
		BOW3,
		BOW4,
		BOW_UNEQUIP,
		SWORD0,
		SWORD1,
		SWORD2,
		SWORD3,
		SWORD4,
		SWORD_UNEQUIP,
		WP_INDEX_END
	};

	enum ATTACK_TYPE  //데미지주는 타입
	{
		AT_Sword_Common,// 경직
		AT_Sword_Skill1,// 경직
		AT_Sword_Skill2,// 경직
		AT_Sword_Skill3,// 경직
		AT_Sword_Skill4,// 경직
		AT_Bow_Common,// 밀려나게
		AT_Bow_Skill1,// 밀려나게
		AT_Bow_Skill2,// 경직
		AT_Bow_Skill3_Start,
		AT_Bow_Skill3,// 이속 느려지게
		AT_Bow_Skill4,// 경직
		AT_OutLine, //위치 변경 금지
		AT_Bow_SkillR,// 경직	
		AT_Critical,
		AT_End
	};

	struct WEAPONPART_DESC
	{
		_uint iNumVariations{};
		ANIM_DESC* Animation{};
		CTransform* pParentTransform{ nullptr };
	};
	enum PART_TYPE
	{
		PT_HAIR,
		PT_FACE,
		PT_BODY,
		PT_HELMET,
		PT_SWORD,
		PT_BOW,
		PT_END
	};

	enum VEHICLE_TYPE
	{
		VC_GROUND,
		VC_FLY,
		VC_END
	};

	enum SortDepth
	{
		D_MOUSE,
		D_FADE,
		D_ALERT,
		D_WINDOW,
		D_LOADING,
		D_INVEN,
		D_SHOP,
		D_QUEST,
		D_MENU,
		D_TALK,
		D_JOBMARK,
		D_BAR,
		D_SCREEN,
		D_NAMETAG,
		D_END
	};

	enum INVEN_TYPE
	{
		INVEN_ALL,
		INVEN_WEARABLE,
		INVEN_EQUIP,
		INVEN_EXPENDABLES,
		INVEN_END
	};
	enum ITEM_TYPE
	{
		ITEM_TOP,
		ITEM_BODY,
		ITEM_HAND,
		ITEM_FOOT,
		ITEM_POTION,
		ITEM_SWORD,
		ITEM_BOW,
		ITEM_INGREDIENT,
		ITEM_PET,
		ITEM_NOTYPE,
		ITEMTYPEEND
	};

	enum ITEM_USAGE
	{
		IT_NOUSAGE,
		IT_HPPOTION,
		IT_MPPOTION,
		IT_VEHICLECARD,
		USAGE_END
	};

	enum ITEM_TIER
	{
		TIER_COMMON,
		TIER_UNCOMMON,
		TIER_RARE,
		TIER_UNIQUE,
		TIER_LEGENDARY,
		TIER_END
	};

	enum WEARABLE_TYPE
	{
		W_TOP,
		W_CHEST,
		W_HAND,
		W_FOOT,
		W_SWORD,
		W_BOW,
		W_PET,
		W_END
	};

	enum TUTO_SEQ
	{
		T_OPENINVEN,
		T_EQUIP,
		T_EXIT,
		T_OPENSKILL,
		T_EQUIPSKILL,
		T_SKILLEXIT,
		TUTO_END
	};

#pragma region Objects
	enum ObjectType
	{
		Object_Building,
		Object_Environment,
		Object_Etc,
		Object_End
	};

	struct ObjectInfo
	{
		wstring strPrototypeTag{};
		_vec4 vPos{ _vec4(0.f, 0.f, 0.f, 1.f) };
		_vec4 vLook{ _vec4(0.f, 0.f, 1.f, 0.f) };
		_mat m_WorldMatrix{};
		ObjectType eObjectType{ };
		_uint m_iIndex{};
	};
	enum Monster_AttType
	{
		MonAtt_Hit,
		Parrying_Succescc,	
		MonAtt_KnockDown,
		MonAtt_Stun,
		MonAtt_Poison,
		MonAtt_Pull,
		MonAtt_End
	};
#pragma endregion

	struct UiInfo
	{
		_uint iLevel{};
		_vec2 vPos{};
		_vec2 vSize{};
		wstring strTexture{};
	};

	struct AttachmentInfo
	{
		CTransform* pParent{ nullptr };
		_mat* pSocket{ nullptr };
		_mat PivotMatrix{};
	};

	typedef struct tagItemInfo
	{
		wstring strName;
		ITEM_USAGE eItemUsage;
		_uint iInvenType;
		_uint iItemType;
		_uint iItemTier;

		wstring strTexture;
		_uint iIndex;

		_uint iStatus;
		_uint iPurchase;
		_uint iSale;
		_uint iPartIndex{};
		_uint iLimited;

	}ITEM;

	typedef struct tagSkillInfo
	{
		wstring strName;
		_uint iSkillType;
		_uint iMp;
		_uint iCoolTime;
		_uint iSkillIdx;
		_bool isSkillIn;
		wstring strTexture;
		_uint iModelSkillIndex;
	}SKILLINFO;

#pragma region Shader Passes
	enum AnimMeshPass
	{
		AnimPass_Default,
		AnimPass_OutLine,
		AnimPass_Shadow,
		AnimPass_Dissolve,
		AnimPass_Rim,
		AnimPass_DissolveNoCull,
		AnimPass_Color,
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
		StaticPass_End,
	};

	enum VTFModelPass
	{
		VTFPass_Default,
		VTFPass_Dissolve,
		VTFPass_Motion_Blur,
		VTFPass_LerpDissolve,
		VTFPass_LerpBlur,
		VTFPass_Main_Rim,

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
		VTPass_LerpColorNAlpha,
		VTPass_HPBoss,
		VTPass_MaskColorMove,
		VTPass_ChangeBright,
		VTPass_Move,
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
		InstPass_End,
	};
#pragma endregion
#pragma region �ƾ�
	struct SectionInfo
	{
		//_vec4 vStartCutScene{};
		//_vec4 vEndCutScene{};
		_mat mCutSceneMatrix{};
		_uint iSectionType{};
		class CCutScene_Curve** ppCurve{ nullptr };
	};
}
#pragma endregion

#pragma region Ʈ����
struct TriggerInfo
{
	_bool bLimited{};
	_int iIndex{};
	_float fSize{};
	_mat WorldMat{};
};

enum TriggerType
{
	VILLAGE_TRIGGER,
	FRONTDOOR_IN_TRIGGER,
	FRONTDOOR_OUT_TRIGGER,
	BACKDOOR_IN_TRIGGER,
	BACKDOOR_OUT_TRIGGER,
	BOSS_TRIGGER,
	TRIGGER_END
};

enum PlaceType
{
	VILLAGE,
	FIELD,
	DUNGEON,
	TOWER,
	PLACE_END
};

#pragma endregion



extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace Client;