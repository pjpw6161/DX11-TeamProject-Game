#pragma once
#include "Client_Define.h"
#include "Objects.h"
#include "Trigger_Manager.h"

BEGIN(Client)

class CEnvironment_Object final : public CObjects
{
private:
	CEnvironment_Object(_dev pDevice, _context pContext);
	CEnvironment_Object(const CEnvironment_Object& rhs);
	virtual ~CEnvironment_Object() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Add_Collider();

private:
	CTrigger_Manager* m_pTrigger_Manager{ nullptr };
	PlaceType m_ePlaceType{ PLACE_END };
	CCollider* m_pCollider{ false };
public:
	static CEnvironment_Object* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END