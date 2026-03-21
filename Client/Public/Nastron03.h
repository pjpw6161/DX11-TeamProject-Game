#pragma once

#include "Client_Define.h"
#include "Monster.h"
#include "CommonTrail.h"

BEGIN(Client)

class CNastron03 final : public CMonster
{
public:
	enum NASTRON03_ANIM
	{
		ATTACK01,
		ATTACK02,
		ATTACK03,
		DIE,
		DIE1,
		HIT_ADD,
		HIT_L,
		HIT_R,
		IDLE,
		KNOCKDOWN,
		NASTRON_03_A_ATTACK01,
		NASTRON_03_A_ATTACK02,
		NASTRON_03_A_ATTACK03,
		ROAR,
		STUN,
		WALK,
		ANIM_END
	};

	enum NASTRON03_STATE
	{
		STATE_IDLE,
		STATE_WALK,
		STATE_CHASE,
		STATE_ATTACK,
		STATE_HIT,
		STATE_DIE,
		STATE_END
	};

private:
	CNastron03(_dev pDevice, _context pContext);
	CNastron03(const CNastron03& rhs);
	virtual ~CNastron03() = default;

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

public:
	void Update_Trail(_float fTimeDelta);

private:
	NASTRON03_STATE m_ePreState = STATE_END;
	NASTRON03_STATE m_eCurState = STATE_END;

private:
	CCommonTrail* m_pSwordTrail = { nullptr };

private:
	_float m_fIdleTime = {};
	
	_uint m_iAttackPattern = {};
	_bool m_bSelectAttackPattern = { false };

private:
	_bool m_bAttacking = { false };

	_bool m_bAttacked = { false };
	_bool m_bAttacked2 = { false };

	_bool m_bDamaged = { false };
	_bool m_bSlow = { false };

private:
	static const _float m_fChaseRange;
	static const _float m_fAttackRange;

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CNastron03* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END