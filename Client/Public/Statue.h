#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

class CStatue final : public CMonster
{
private:
	CStatue(_dev pDevice, _context pContext);
	CStatue(const CStatue& rhs);
	virtual ~CStatue() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Set_Damage(_int iDamage, _uint iDamageType = 0) override;

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

private:
	_bool m_bDamaged = { false };

public:
	static CStatue* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END