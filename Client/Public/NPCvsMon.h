#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

class CNPCvsMon final : public CMonster
{
private:
	CNPCvsMon(_dev pDevice, _context pContext);
	CNPCvsMon(const CNPCvsMon& rhs);
	virtual ~CNPCvsMon() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CNPCvsMon* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END