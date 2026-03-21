#pragma once
#include "Component.h"

BEGIN(Engine)

enum RenderGroup
{
	RG_Priority,
	RG_Shadow,
	RG_NonBlend,
	RG_NonBlend_Instance,
	RG_AnimNonBlend_Instance,
	RG_Blur,
	RG_NonLight,
	RG_Blend,
	RG_Distortion,
	RG_Water,
	RG_UI,
	RG_Cursor,
	RG_End
};

struct RadialParams {
	_vec2 vCenterTexPos{};
	_float fRadialBlur_Power{};
	_uint isTex{};

	_vec4 vCenterPos{};
};

class ENGINE_DLL CRenderer final : public CComponent
{
private:
	CRenderer(_dev pDevice, _context pContext);
	CRenderer(const CRenderer& rhs) = delete;
	virtual ~CRenderer() = default;

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;

public:
	HRESULT Add_RenderGroup(RenderGroup eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Draw_RenderGroup();

	const void Set_TurnOneBloom(_bool TurnOnBloom) {
		m_TurnOnBloom = TurnOnBloom;
	}

	const void Set_RadialBlur_World(_vec4 vRadialCenter);

	const void Set_RadialBlur_Texcoord(_vec2 vRadialCenter) {
		m_RBParams.vCenterTexPos = vRadialCenter;
		m_RBParams.isTex = true;
	}

	const void Set_RadialBlur_Power(_float fBlurPower) {
		m_RBParams.fRadialBlur_Power = fBlurPower;
		if (0.f > m_RBParams.fRadialBlur_Power)
			m_RBParams.fRadialBlur_Power = 0.f;
	}


#ifdef _DEBUG
	HRESULT Add_DebugComponent(class CComponent* pDebugComponent);
#endif // _DEBUG


private:
	list<class CGameObject*> m_RenderObjects[RG_End]{};

#ifdef _DEBUG
	list<class CComponent*> m_DebugComponents{};
#endif // _DEBUG

private:
	class CVIBuffer_Rect* m_pVIBuffer{ nullptr };
	class CShader* m_pShader{ nullptr };

	_float44 m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};

	ID3D11DepthStencilView* m_pShadowDSV{ nullptr };

	_bool m_bMotionBlur{ false };

#pragma region Æò±Õ ÈÖµµ ±¸ÇÏ´Â ½¦ÀÌ´õ¶û ·£´õÅ¸°Ùµé

	class CCompute_Shader* m_pLumShader = nullptr;
	class CCompute_RenderTarget* m_pLumRT = nullptr;

	class CCompute_Shader* m_pDownShader = nullptr;
	class CCompute_RenderTarget* m_pDownRT = nullptr;

	class CCompute_Shader* m_pDown2Shader = nullptr;
	class CCompute_RenderTarget* m_pDownRT1 = nullptr;

	class CCompute_Shader* m_pDown3Shader = nullptr;
	class CCompute_RenderTarget* m_pDownRT2 = nullptr;

	class CCompute_Shader* m_pGetAvgLumShader = nullptr;
	class CCompute_RenderTarget* m_pLumValue = nullptr;

#pragma endregion

#pragma region ºí·¯ÇÏ´Â ½¦ÀÌ´õ¶û ·£´õÅ¸°Ùµé

	class CCompute_Shader* m_pBlurShader = nullptr;
	BLURPARAM m_BLParam;

	class CCompute_Shader* m_pDownScaleShader = nullptr;
	DSPARAM m_DSParam[3];

	class CCompute_RenderTarget* m_pHalfRT = nullptr;
	class CCompute_RenderTarget* m_pQuarterRT = nullptr;
	class CCompute_RenderTarget* m_pEightRT = nullptr;

	class CCompute_RenderTarget* m_pHalfBlurRT = nullptr;
	class CCompute_RenderTarget* m_pQuarterBlurRT = nullptr;
	class CCompute_RenderTarget* m_pEightBlurRT = nullptr;

	class CShader* m_pGetBlurShader{ nullptr };

#pragma endregion

#pragma region ·¹µð¾ó ºí·¯

	class CCompute_Shader* m_pRadialShader = nullptr;
	class CCompute_RenderTarget* m_pRadialRT = nullptr;
	RadialParams m_RBParams;

#pragma endregion

#pragma region ¸ð¼Ç ºí·¯

	class CCompute_Shader* m_pMotionShader = nullptr;
	class CCompute_RenderTarget* m_pMotionRT = nullptr;

#pragma endregion


	class CTexture* m_pNoiseNormal = nullptr;

	class CShadowMap* m_pShadowMap = nullptr;

	_uint2 m_WinSize{};

#pragma region ½¦ÀÌ´õ ¼öÄ¡Á¶Àý

private:
	_bool m_TurnOnSSAO = true;
	_bool m_TurnOnToneMap = true;
	_bool m_TurnOnBloom = true;
	_bool m_TurnOnRim = false;

	_float m_fSSAOBlurPower = 1.f;

	_float m_fEffectBlurPower = 1.5f;

	_float m_fHDRBloomPower = 1.f;

	SSAO_DESC m_SSAO;
	HDR_DESC m_HDR;


public:
	SSAO_DESC Get_SSAO() const {
		return m_SSAO;
	}

	void Set_SSAO(SSAO_DESC Desc) {
		m_SSAO = Desc;
	}

	HDR_DESC Get_HDR() const {
		return m_HDR;
	}

	void Set_HDR(HDR_DESC Desc) {
		m_HDR = Desc;
	}

	_float Get_SSAOBlurPower() const {
		return m_fSSAOBlurPower;
	}

	void Set_SSAOBlurPower(_float fBlurPower) {
		m_fSSAOBlurPower = fBlurPower;
	}


	_float Get_EffectBlurPower() const {
		return m_fEffectBlurPower;
	}

	void Set_EffectBlurPower(_float fBlurPower) {
		m_fEffectBlurPower = fBlurPower;
	}


	_float Get_HDRBlurPower() const {
		return m_fHDRBloomPower;
	}

	void Set_HDRBlurPower(_float fBlurPower) {
		m_fHDRBloomPower = fBlurPower;
	}

#pragma endregion

private:
	HRESULT Ready_ShadowDSV();

	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonBlend();
	HRESULT Render_NonBlend_Instance();
	HRESULT Render_AnimNonBlend_Instance();
	HRESULT Render_Refraction();
	HRESULT Render_Reflection();
	HRESULT Render_Water();
	HRESULT Render_LightAcc();
	HRESULT Render_Deferred();
	HRESULT Render_HDR();
	HRESULT Render_Outline();
	HRESULT Render_Blend();
	HRESULT Render_Distortion();
	HRESULT Render_NoneBlendFinal();
	HRESULT Render_BlendFinal();
	HRESULT Render_Final();
	HRESULT Render_UI();
#ifdef _DEBUG
private:
	HRESULT Render_Debug();
#endif // _DEBUG

private:
	HRESULT Get_AvgLuminance();
	HRESULT Get_BlurTex(ID3D11ShaderResourceView* pSRV, const wstring& MRT_Tag, _float fBlurPower, _bool isBloom = false);
	HRESULT Add_Instance(InstanceID InstanceID, Instance_Data& pMeshInstancing);
	HRESULT Clear_Instance();
	HRESULT FinishCommand();
	map<InstanceID, class CVIBuffer_Mesh_Instance*>	m_InstanceBuffers;

	ID3D11CommandList* m_pCommandList = nullptr;
	_context m_pDeferrd = nullptr;


public:
	static CRenderer* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END