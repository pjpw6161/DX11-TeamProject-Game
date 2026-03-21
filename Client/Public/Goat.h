#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

class CGoat final : public CMonster
{
public:
	enum GOAT_ANIM
	{
		ATTACK01,
		ATTACK02,
		ATTACK03,
		DIE,
		HIT_ADD,
		HIT_L,
		HIT_R,
		IDLE,
		INTERACTION01,
		INTERACTION02,
		KNOCKDOWN,
		ROAR,
		RUN,
		STUN,
		WALK,
		ANIM_END
	};

	enum GOAT_STATE
	{
		STATE_IDLE,
		STATE_ROAM,
		STATE_CHASE,
		STATE_ATTACK,
		STATE_HIT,
		STATE_DIE,
		STATE_END
	};

private:
	CGoat(_dev pDevice, _context pContext);
	CGoat(const CGoat& rhs);
	virtual ~CGoat() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Set_Damage(_int iDamage, _uint iDamageType = 0) override;

public:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

private:
	GOAT_STATE m_ePreState = STATE_END;
	GOAT_STATE m_eCurState = STATE_END;

private:
	_float m_fIdleTime = {};

	_uint m_iRoamingPattern = {};
	_uint m_iAttackPattern = {};
	_bool m_bSelectAttackPattern = { false };

private:
	_bool m_bAttacking = { false };

	_bool m_bAttacked = { false };

	_bool m_bDamaged = { false };

private:
	static const _float m_fChaseRange;
	static const _float m_fAttackRange;

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CGoat* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END