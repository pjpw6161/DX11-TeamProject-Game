#pragma once
#include "Effect_Define.h"
#include "Base.h"
#include "GameInstance.h"

BEGIN(Effect)

struct OldParticleDesc
{
	SimpleMath::Vector3 vMinPos{}, vMaxPos;
	SimpleMath::Vector2 vSpeedRange{};
	SimpleMath::Vector2 vLifeTime{};
	SimpleMath::Vector2 vScaleRange{};
	SimpleMath::Vector3 vMinDir{}, vMaxDir{};
	bool isLoop{};
};

struct OldEffectInfo
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
	float fPartiDissolveRatio{};

	bool hasLight{};
	LIGHT_DESC Light_Desc{};

	std::wstring strDiffuseTexture{};
	std::wstring strMaskTexture{};
	std::wstring strDissolveTexture{};
	std::wstring strUnDissolveTexture{};
	std::string strModel{};

	bool isFollow{};
	SimpleMath::Matrix* pMatrix{};
};

class CImgui_Manager : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)
private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	const _bool& Has_Light();

	const _color& Get_BGColor();

public:
	HRESULT Init(_dev pDevice, _context pContext, vector<string>* pTextureList, vector<string>* pModelList);
	void Tick(_float fTimeDelta);
	void Effect_Tick(_float fTimeDelta);
	void Effect_LateTick(_float fTimeDelta);
	HRESULT Render();

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };
	CGameInstance* m_pGameInstance{ nullptr };
	//CVIBuffer_Rect* m_pVIBuffer_Rect{ nullptr };
	//CVIBuffer_Instancing_Point* m_pVIBuffer_Instancing{ nullptr };

private:
	class CEffect_Dummy* m_pEffect{ nullptr };
	list<class CEffect_Dummy*> m_AddEffect{};
	//class CShader* m_pInstanceShader{ nullptr };
	//class CShader* m_pVtxTexShader{ nullptr };
	_int m_iCurrent_Type{};
	_int m_iSelected_Texture{};
	_int m_iSelected_MaskTexture{};

	const _char** m_pItemList_Texture{ nullptr };
	_int m_iNumTextures{};
	CTexture** m_pTextures{ nullptr };

	_int m_iSelected_Model{};
	const _char** m_pItemList_Model{ nullptr };
	_int m_iNumModels{};
	CModel** m_pModels{ nullptr };

	_vec4 m_vColor{ 1.f };
	_uint m_iShaderType{};
	_float m_fEffectLifeTime{ 1.f };
	_int m_iNumInstance{ 30 };
	_bool m_hasDiffTexture{};
	_bool m_hasMask{};
	_bool m_hasLight{};
	ParticleDesc m_ParticleInfo{};

	_mat m_DummyMatrix{};

	filesystem::path m_CurrFilePath{};

	_color m_vBGColor{ 0.2f, 0.2f, 0.25f, 1.f };

private:
	HRESULT Ready_Layers();

	const _int& Compute_TextureIndex(const wstring& strTexture);
	const _int& Compute_ModelIndex(const string& strModel);

	EffectInfo Load_Data(_bool isAdd = false, _bool isQuick = false);
	void Load_OldData();
	HRESULT Export_Data(EffectInfo& Info);
	HRESULT Override_Data(EffectInfo& Info);

public:
	virtual void Free() override;
};

END