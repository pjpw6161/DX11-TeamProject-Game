#pragma once

#include "Client_Define.h"
#include "Monster.h"
#include "CommonSurfaceTrail.h"

BEGIN(Client)

class CThief04 final : public CMonster
{
public:
	enum THIEF04_ANIM
	{
		ATTACK01,
		ATTACK01_02B,
		ATTACK02,
		ATTACK02_02B,
		ATTACK03,
		ATTACK03_02B,
		ATTACK04,
		ATTACK04_02B,
		ATTACK05,
		ATTACK05_02B,
		DIE,
		DIE_02B,
		HIT_ADD,
		HIT_ADD_02B,
		IDLE,
		IDLE_02B,
		KNOCKDOWN,
		KNOCKDOWN_02B,
		L_HIT,
		L_HIT_02B,
		L_TURN,
		L_TURN_02B,
		R_HIT,
		R_HIT_02B,
		R_TURN,
		R_TURN_02B,
		ROAR,
		ROAR_02B,
		RUN,
		RUN_02B,
		STUN,
		STUN_02B,
		WALK,
		WALK_02B,
		ANIM_END
	};

	enum THIEF04_STATE
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
	CThief04(_dev pDevice, _context pContext);
	CThief04(const CThief04& rhs);
	virtual ~CThief04() = default;

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
	THIEF04_STATE m_ePreState = STATE_END;
	THIEF04_STATE m_eCurState = STATE_END;

private:
	CCommonSurfaceTrail* m_pAxeTrail = { nullptr };
	CCommonSurfaceTrail* m_pKnifeTrail = { nullptr };

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
	static CThief04* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END