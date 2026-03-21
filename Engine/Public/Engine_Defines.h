#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)
#pragma warning (disable : 4267)

// 외부 라이브러리 헤더
#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>
#include <fx11/d3dx11effect.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/Effects.h>
using namespace DirectX;
#include <SimpleMath.h>

#include <Xinput.h>
#include <vfw.h>

#include <PxPhysicsAPI.h>
using namespace physx;

// 외부 라이브러리 lib
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "d3d11.lib")
#ifdef _DEBUG
#pragma comment(lib, "DirectXTK.lib")  
#pragma comment(lib, "PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib, "PhysXPvdSDK_static_64.lib")
#else
#pragma comment(lib, "DirectXTK_Release.lib")
#pragma comment(lib, "PhysXExtensions_static_64_Release.lib")
#pragma comment(lib, "PhysXCharacterKinematic_static_64_Release.lib")
#pragma comment(lib, "PhysXPvdSDK_static_64_Release.lib")
#endif // _DEBUG
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "PhysXCommon_64.lib")
#pragma comment(lib, "PhysX_64.lib")
#pragma comment(lib, "PhysXFoundation_64.lib")
#pragma comment(lib, "PhysXCooking_64.lib")
#pragma comment (lib, "Vfw32.lib")

#ifdef _DEBUG
#pragma comment(lib, "Effects11d.lib")
#else //Release
#pragma comment(lib, "Effects11.lib")
#endif

#include <array>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <queue>
#include <string>
#include <unordered_map>
#include <ctime>
#include <fstream>
#include <filesystem>
#include <random>
#include <functional>
#include <sstream>
#include <iostream>

#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"

// 사운드 재생을 위한 헤더
#include <io.h>
#include "fmod.h"
#include "fmod.hpp"
#pragma comment(lib, "fmod_vc.lib")

#ifdef _DEBUG
#ifndef _TOOL

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif
#endif // _DEBUG

namespace Engine
{
	enum class MouseState
	{
		x, y, wheel, end
	};
	enum MOUSE_KEY_STATE
	{
		DIM_LBUTTON,
		DIM_RBUTTON,
		DIM_MBUTTON,
		DIM_END
	};

	enum GAMPAD_KEY_STATE
	{
		XINPUT_UP = 0x0001,
		XINPUT_DOWN = 0x0002,
		XINPUT_LEFT = 0x0004,
		XINPUT_RIGHT = 0x0008,
		XINPUT_START = 0x0010,
		XINPUT_BACK = 0x0020,
		XINPUT_LS = 0x0040,
		XINPUT_RS = 0x0080,
		XINPUT_LB = 0x0100,
		XINPUT_RB = 0x0200,
		XINPUT_A = 0x1000,
		XINPUT_B = 0x2000,
		XINPUT_X = 0x4000,
		XINPUT_Y = 0x8000,
		XINPUT_LT,
		XINPUT_RT,
	};

	enum class TextureType
	{
		None,
		Diffuse,
		Specular,
		Ambient,
		Emissive,
		Height,
		Normals,
		Shininess,
		Opacity,
		Displacement,
		LightMap,
		Reflection,
		Base_Color,
		Normal_Camera,
		Emission_Color,
		Metalness,
		Diffuse_Roughness,
		Ambient_Occlusion,
		End
	};

	enum DeferredPass
	{
		DefPass_Debug,
		DefPass_DebugArray,
		DefPass_Light_Directional,
		DefPass_Light_Point,
		DefPass_Water,
		DefPass_Deferred,
		DefPass_Blend,
		DefPass_SSAO,
		DefPass_HDR,
		DefPass_Stencil,
		DefPass_Outline,
		DefPass_Blur,
		DefPass_Distortion,
		DefPass_JustDraw,
	};

	enum Effect_Type
	{
		ET_PARTICLE,
		ET_RECT,
		ET_MESH,
		ET_DISTORTION,
		ET_PARTI_DISTORTION,
		ET_END
	};

	enum OutlineColor 
	{
		OutlineColor_None,
		OutlineColor_White,
		OutlineColor_Yellow,
		OutlineColor_Red,
		OutlineColor_End
	};
}

using namespace std;
using namespace Engine;
