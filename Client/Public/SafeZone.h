#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

BEGIN(Client)

class CSafeZone : public CBlendObject
{
private:
	CSafeZone(_dev pDevice, _context pContext);
	CSafeZone(const CSafeZone& rhs);
	virtual ~CSafeZone() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	CCollider* m_pColliderCom{ nullptr };
	class CEffect_Dummy* m_pEffects[2]{};

#ifdef _DEBUG
	CRenderer* m_pRendererCom{ nullptr };
#endif // _DEBUG

private:
	_mat m_EffectMatrices[2]{};
	_float m_fLifeTimer{};
	_float m_fLifeTimeLimit{};

public:
	HRESULT Add_Components();

public:
	static CSafeZone* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END