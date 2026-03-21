#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

BEGIN(Client)

class CSickle : public CBlendObject
{
private:
	CSickle(_dev pDevice, _context pContext);
	CSickle(const CSickle& rhs);
	virtual ~CSickle() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	CCollider* m_pColliderCom{ nullptr };
	class CEffect_Dummy* m_pEffects[3]{};

#ifdef _DEBUG
	CRenderer* m_pRendererCom{ nullptr };
#endif // _DEBUG

private:
	_mat m_EffectMatrices[3]{};
	_float m_fLifeTimer{};
	_float m_fLifeTimeLimit{};

public:
	HRESULT Add_Components();

public:
	static CSickle* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END