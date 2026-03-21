#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

class CModelTest final : public CMonster
{
private:
	CModelTest(_dev pDevice, _context pContext);
	CModelTest(const CModelTest& rhs);
	virtual ~CModelTest() = default;

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
	static CModelTest* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END