#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

class CDead final : public CMonster
{
public:
	enum DEAD_MONSTER
	{
		VOID01,
		VOID05,
		VOID09,
		DEAD_END
	};

	using DEAD_DESC = struct DeadDesc
	{
		DEAD_MONSTER eDead;
		_vec4 vPos;
	};

private:
	CDead(_dev pDevice, _context pContext);
	CDead(const CDead& rhs);
	virtual ~CDead() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float m_fLifeTime = {};

private:
	DEAD_MONSTER m_eDead = { DEAD_END };
	_bool m_hasVideoStarted{};

public:
	static CDead* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END