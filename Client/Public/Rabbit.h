#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

class CHPMonster;

class CRabbit final : public CMonster
{
public:
	enum RABBIT_ANIM
	{
		ATTACK01,
		ATTACK02,
		DIE,
		HIT_ADD,
		HIT_ADD_L,
		HIT_ADD_R,
		IDLE,
		KNOCKDOWN,
		ROAR,
		RUN,
		STUN,
		WALK,
		ANIM_END
	};

	enum RABBIT_STATE
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
	CRabbit(_dev pDevice, _context pContext);
	CRabbit(const CRabbit& rhs);
	virtual ~CRabbit() = default;

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
	RABBIT_STATE m_ePreState = STATE_END;
	RABBIT_STATE m_eCurState = STATE_END;

private:
	_float m_fIdleTime = {};
	_float m_fStunTime = {};

	_uint m_iRoamingPattern = {};
	_uint m_iAttackPattern = {};
	_bool m_bSelectAttackPattern = { false };

private:
	_bool m_bAttacking = { false };

	_bool m_bAttacked = { false };
	_bool m_bAttacked2 = { false };

	_bool m_bDamaged = { false };
	_bool m_bStun = { false };

private:
	static const _float m_fChaseRange;
	static const _float m_fAttackRange;

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CRabbit* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END