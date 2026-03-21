#pragma once

#include "Client_Define.h"
#include "Monster.h"
#include "CommonTrail.h"

BEGIN(Client)

class CNastron07 final : public CMonster
{
public:
	enum NASTRON07_ANIM
	{
		ATTACK01,
		ATTACK02,
		ATTACK03,
		DIE,
		HELL_END_SC02_MON_NASTRON_07_01_A,
		HELL_END_SC02_MON_NASTRON_07_01_B,
		HIT_ADD,
		HIT_L,
		HIT_R,
		IDLE,
		KNOCKDOWN,
		ROAR,
		RUN,
		STUN,
		WALK,
		ANIM_END
	};

	enum NASTRON07_STATE
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
	CNastron07(_dev pDevice, _context pContext);
	CNastron07(const CNastron07& rhs);
	virtual ~CNastron07() = default;

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
	NASTRON07_STATE m_ePreState = STATE_END;
	NASTRON07_STATE m_eCurState = STATE_END;

private:
	CCommonTrail* m_pLeftTrail1 = { nullptr };
	CCommonTrail* m_pLeftTrail2 = { nullptr };
	CCommonTrail* m_pLeftTrail3 = { nullptr };

	CCommonTrail* m_pRightTrail1 = { nullptr };
	CCommonTrail* m_pRightTrail2 = { nullptr };
	CCommonTrail* m_pRightTrail3 = { nullptr };

private:
	_float m_fIdleTime = {};

	_uint m_iAttackPattern = {};
	_bool m_bSelectAttackPattern = { false };

private:
	_bool m_bAttacking = { false };

	_bool m_bAttacked = { false };
	_bool m_bAttacked2 = { false };
	_bool m_bAttacked3 = { false };

	_bool m_bDamaged = { false };
	_bool m_bSlow = { false };

private:
	static const _float m_fChaseRange;
	static const _float m_fAttackRange;

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CNastron07* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END