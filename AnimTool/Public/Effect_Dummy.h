#pragma once
#include "AnimTool_Define.h"
#include "BlendObject.h"
#include "Effect_Manager.h"

BEGIN(AnimTool)

class CEffect_Dummy final : public CBlendObject
{
private:
	CEffect_Dummy(_dev pDevice, _context pContext);
	CEffect_Dummy(const CEffect_Dummy& rhs);
	virtual ~CEffect_Dummy() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CVIBuffer_Instancing_Point* m_pParticle{ nullptr };
	CVIBuffer_Rect* m_pRect{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CTexture* m_pMaskTextureCom{};
	CTexture* m_pDiffTextureCom{};
	CTexture* m_pDissolveTextureCom{};
	CTexture* m_pUnDissolveTextureCom{};
	CModel* m_pModelCom{};

private:
	EffectInfo m_Effect{};
	_mat m_WorldMatrix{};
	_mat m_OffsetMatrix{};
	_float m_fTimer{};
	_float m_fSpriteTimer{};
	_int m_iSpriteIndex{};
	_vec3 m_vScaleAcc{ 1.f };
	_float m_fUnDissolveRatio{};
	_float m_fDissolveRatio{};
	_vec2 m_vUV{};
	_float m_fAlpha{};

	static _int m_iLightID;
	wstring m_strLightTag{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffect_Dummy* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END