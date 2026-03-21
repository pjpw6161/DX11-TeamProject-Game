#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CPet abstract : public CGameObject
{
protected:
	CPet(_dev pDevice, _context pContext);
	CPet(const CPet& rhs);
	virtual ~CPet() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual _vec4 Compute_PlayerPos();
	virtual _float Compute_PlayerDistance();

protected:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

protected:
	wstring	m_strModelTag = {};
	ANIM_DESC m_Animation{};
	_mat m_EffectMatrix{};

protected:
	_float m_fPosLerpRatio = {};
	_float m_fLookLerpRatio = {};


public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END