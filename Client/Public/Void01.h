#pragma once

#include "Client_Define.h"
#include "Monster.h"
#include "CommonTrail.h"

BEGIN(Client)

class CVoid01 final : public CMonster
{
public:
	enum VOID01_ANIM
	{
		ATTACK01,
		ATTACK02,
		ATTACK03,
		HIT_ADD,
		HIT_L,
		HIT_R,
		IDLE,
		KNOCKBACK,
		KNOCKBACK_HOLD,
		ROAR,
		RUN,
		STUN,
		WALK,
		ANIM_END
	};

	enum VOID01_STATE
	{
		STATE_IDLE,
		STATE_READY,
		STATE_CHASE,
		STATE_ATTACK,
		STATE_HIT,
		STATE_DIE,
		STATE_END
	};

private:
	CVoid01(_dev pDevice, _context pContext);
	CVoid01(const CVoid01& rhs);
	virtual ~CVoid01() = default;

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
	VOID01_STATE m_ePreState = STATE_END;
	VOID01_STATE m_eCurState = STATE_END;

private:
	CCommonTrail* m_pLeftTrail = { nullptr };
	CCommonTrail* m_pRightTrail = { nullptr };

private:
	_float m_fIdleTime = {};

private:
	_uint m_iAttackPattern = {};
	_bool m_bSelectAttackPattern = { false };

private:
	_bool m_bAttacked = { false };

	_bool m_bDamaged = { false };
	_bool m_bSlow = { false };

private:
	_uint m_iHitPercentage = {};

private:
	static const _float m_fChaseRange;
	static const _float m_fAttackRange;

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CVoid01* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END