#pragma once
#include "SimpleMath.h"

namespace Engine
{
	enum class ModelType
	{
		Static,
		Anim,
		Collision,
		End
	};

	using GRAPHIC_DESC = struct tagGraphic_Desc
	{
		HWND hWnd{};
		HINSTANCE hInst{};
		unsigned int iWinSizeX{}, iWinSizeY{};
		bool isWindowed{};
	};

	using LIGHT_DESC = struct tagLightDesc
	{
		enum TYPE { Directional, Point, End };

		TYPE eType{End};
		SimpleMath::Vector4 vDirection{};
		SimpleMath::Vector4 vPosition{};
		// Range, Constant, Linear, Quadratic
		SimpleMath::Vector4 vAttenuation{};

		SimpleMath::Vector4 vDiffuse{};
		SimpleMath::Vector4 vAmbient{};
		SimpleMath::Vector4 vSpecular{};
	};

	using Model_Material = struct tagMaterial
	{
		class CTexture* pMaterials[18]{};
	};

	using KEYFRAME = struct tagKeyFrame
	{
		float fTime{};
		SimpleMath::Vector4 vScaling{};
		SimpleMath::Vector4 vRotation{};
		SimpleMath::Vector4 vPosition{};
	};

	struct ParticleDesc
	{
		SimpleMath::Vector3 vMinPos{}, vMaxPos{};
		SimpleMath::Vector2 vSpeedRange{};
		SimpleMath::Vector2 vLifeTime{};
		SimpleMath::Vector2 vScaleRange{};
		SimpleMath::Vector3 vMinDir{}, vMaxDir{};
		bool isLoop{};
	};

	typedef struct tagSSAOValue {
		float fIntensity = 2.f;
		float fRadius = 0.01f;
		float fScale = 2.f;
		float fBias = 0.3f;
	}SSAO_DESC;

	typedef struct tagHDRValue {
		float fMiddleGray = 6.5f;
		float fLumWhiteSqr = 0.8f;
		float fPadding0 = 0.f;
		float fPadding1 = 0.f;
	}HDR_DESC;

	typedef struct DownScaleParams {
		unsigned int ScaleX;
		unsigned int ScaleY;
		unsigned int Padding0 = 0;
		unsigned int Padding1 = 0;
	}DSPARAM;

#define GAUSSIAN_RADIUS 7

	typedef struct BlurParams
	{
		int radius;     // must be <= MAX_GAUSSIAN_RADIUS
		int direction;  // 0 = horizontal, 1 = vertical
		unsigned int ScaleX;
		unsigned int ScaleY;
		float fBlurPower;
		unsigned int iPadding0 = 0;
		unsigned int iPadding1 = 0;
		unsigned int iPadding2 = 0;
	}BLURPARAM;

	struct EffectInfo
	{
		unsigned int iType{};
		bool isSprite{};
		bool isFixedIndex{};
		XMINT2 vNumSprites{};
		float fSpriteDuration{};
		int iFixedSpriteIndex{};
		ParticleDesc PartiDesc{};
		unsigned int iNumInstances{};
		float fLifeTime{};
		SimpleMath::Vector4 vColor{};
		unsigned int iPassIndex{};
		SimpleMath::Vector3 vSize{};
		SimpleMath::Vector3 vPosOffset{};
		SimpleMath::Vector3 vSizeDelta{};
		bool bApplyGravity{};
		SimpleMath::Vector3 vGravityDir{};
		float fDissolveDuration{};
		bool bSkipBloom{};
		float fUnDissolveDuration{};
		SimpleMath::Vector2 vUVInit{};
		SimpleMath::Vector2 vUVDelta{};
		bool isRandomSprite{};
		bool isUVLoop{};
		float fAlphaInit{};
		float fAlphaDelta{};
		float fRectRotationAngle{};
		bool isBillboard{};
		SimpleMath::Vector3 vBillboardRotation{};
		float fPartiAppearRatio{};
		float fPartiDissolveRatio{};

		bool hasLight{};
		LIGHT_DESC Light_Desc{};

		std::wstring strDiffuseTexture{};
		std::wstring strMaskTexture{};
		std::wstring strDissolveTexture{};
		std::wstring strUnDissolveTexture{};
		std::string strModel{};

		std::wstring strEffectTag{};
		bool isFollow{};
		SimpleMath::Matrix* pMatrix{};
	};

	struct ANIM_DESC
	{
		unsigned int iAnimIndex{};
		bool isLoop{};
		float fAnimSpeedRatio{ 1.f };
		bool bSkipInterpolation{};
		float fInterpolationTime{ 0.2f };
		float fDurationRatio{ 1.f };
		bool bRestartAnimation{ false };
		float fStartAnimPos{};
		bool bRewindAnimation{ false };
	};


	using ANIMTIME_DESC = struct tagAnimTimeDesc {
		int iAnimIndex = 0;
		unsigned int iCurrFrame = 0;
		unsigned int iNextFrame = 0;
		float fRatio = 0.f;
		float fTime = 0.f;
		float fSpeed = 1.f;
		XMFLOAT2 vPadding;
	};

	using PLAYANIM_DESC = struct tagPlayAnimDesc {
		void PLAYANIM_DESC() {
			eCurrent.iAnimIndex = 0;
			eNext.iAnimIndex = -1;
		}

		void ResetNextAnim() {
			eNext.iAnimIndex = -1;
			eNext.iCurrFrame = 0;
			eNext.iNextFrame = 0;
			eNext.fTime = 0;
			SwitchTime = 0.f;
			SwitchRatio = 0.f;
		}

		float SwitchDuration = 0.2f;
		float SwitchRatio = 0.f;
		float SwitchTime = 0.f;
		float fPadding = 0.f;
		ANIMTIME_DESC eCurrent;
		ANIMTIME_DESC eNext;
	};

#define MAX_INSTANCE 300
	using INSTANCED_PLAYANIM_DESC = struct tagInstancedPlayAnimDesc
	{
		PLAYANIM_DESC PlayAnim[MAX_INSTANCE];

	};

	using TRIGGEREFFECT_DESC = struct tagTriggerEffectDesc {
		int iStartAnimIndex;
		float fStartAnimPos;
		std::vector<int> iEndAnimIndices;
		std::vector<float> fEndAnimPoses;
		bool IsFollow{};
		std::wstring strEffectName{};
		unsigned int iBoneIndex{};
		SimpleMath::Matrix OffsetMatrix{};
		bool IsDeleteRotateToBone{};
		bool IsClientTrigger{};

		bool HasCreated{};
	};

	using TRIGGERSOUND_DESC = struct tagTriggerSoundDesc {
		int iStartAnimIndex{};
		float fStartAnimPos{};
		std::vector<int> iEndAnimIndices;
		std::vector<float> fEndAnimPoses;
		std::vector<std::wstring> strSoundNames;
		float fInitVolume{};
		float fFadeoutSecond = 1.f;
		bool IsClientTrigger{};
		float fStartPosRatio{};
		bool IsFadeinSound{};
		float fFadeinSecond = 1.f;
		
		int iChannel = -1;
		bool HasPlayed{};
		bool IsEnding{};
	};

	using VTXPOS = struct ENGINE_DLL tagVertex_Position
	{
		XMFLOAT3		vPosition{};

		static const unsigned int iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];

	};

	using CASCADE_DESC = struct tagCascadeDesc {
		SimpleMath::Matrix LightView[3];
		SimpleMath::Matrix LightProj[3];
		float ClipZ[4];
	};

	using VTXPOSCOLOR = struct ENGINE_DLL tagVertex_Position_Color
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vColor{};

		static const unsigned int iNumElements{ 2 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXPOSTEX = struct ENGINE_DLL tagVertex_Position_Texcoord
	{
		XMFLOAT3 vPosition{};
		XMFLOAT2 vTexcoord{};

		static const unsigned int iNumElements{ 2 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXNORTEX = struct ENGINE_DLL tagVertex_Position_Normal_Texcoord
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT2 vTexcoord{};

		static const unsigned int iNumElements{ 3 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXCUBETEX = struct ENGINE_DLL tagVertex_Position_Normal_Texcoord_Cube
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT3 vTexcoord{};

		static const unsigned int iNumElements{ 3 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXSTATICMESH = struct ENGINE_DLL tagVertex_Static_Mesh
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT2 vTexcoord{};
		XMFLOAT3 vTangent{};

		static const unsigned int iNumElements{ 4 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXANIMMESH = struct ENGINE_DLL tagVertex_Anim_Mesh
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT2 vTexcoord{};
		XMFLOAT3 vTangent{};

		XMUINT4 vBlendIndices{};
		XMFLOAT4 vBlendWeights{};

		static const unsigned int iNumElements{ 6 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXINSTANCING = struct tagVertex_Instancing
	{
		SimpleMath::Vector4 vRight{};
		SimpleMath::Vector4 vUp{};
		SimpleMath::Vector4 vLook{};
		SimpleMath::Vector4 vPos{};
		SimpleMath::Vector4 vPrevPos{};
		float fIndex{};
		float fDissolveRatio{};

		float fSpeed{};
		SimpleMath::Vector2 vLifeTime{};
		SimpleMath::Vector4 vOriginPos{};
		SimpleMath::Vector4 vDirection{};
		SimpleMath::Vector4 vOriginDir{};
	};

	using VTXPOINT = struct ENGINE_DLL tagVertex_Point
	{
		XMFLOAT3 vPosition{};
		XMFLOAT2 vPSize{};

		static const unsigned int iNumElements{ 2 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXTRAIL = struct ENGINE_DLL tagVertex_Trail
	{
		XMFLOAT3 vPosition{};
		XMFLOAT2 vPSize{};
		XMFLOAT4 vColor{};

		static const unsigned int iNumElements{ 3 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXTRAILSURFACE = struct ENGINE_DLL tagVertex_Trail_Surface
	{
		XMFLOAT3 vTopPosition{};
		XMFLOAT3 vBottomPosition{};
		float fAlpha{};
		float fTexcoordX{};

		static const unsigned int iNumElements{ 4 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXPOINT_INSTANCING = struct ENGINE_DLL tagVertex_Point_Instancing
	{
		static const unsigned int iNumElements{ 9 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	struct TERRAIN_INFO
	{
		XMUINT2 vTerrainSize{};
		bool isMesh{ false };
	};

	struct Instance_Data
	{
		SimpleMath::Matrix mMatrix{};
		//SimpleMath::Matrix mOldMatrix{};
		unsigned int m_iID{};
	};

	using VTXMESH_INSTANCING = struct ENGINE_DLL tagVertex_Mesh_Instancing
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT2 vTexcoord{};
		XMFLOAT3 vTangent{};
		XMMATRIX mMatrix{};

		unsigned int iInstanceID{};

		static const unsigned int iNumElements{ 9 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};

	using VTXANIMMESH_INSTANCING = struct ENGINE_DLL tagVertex_Anim_Mesh_Instancing
	{
		XMFLOAT3 vPosition{};
		XMFLOAT3 vNormal{};
		XMFLOAT2 vTexcoord{};
		XMFLOAT3 vTangent{};

		XMUINT4 vBlendIndices{};
		XMFLOAT4 vBlendWeights{};
		XMMATRIX mMatrix{};

		unsigned int iInstanceID{};

		static const unsigned int iNumElements{ 11 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	};
}