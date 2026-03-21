#pragma once

#include "Client_Define.h"
#include "Monster.h"
#include "CommonSurfaceTrail.h"

BEGIN(Client)

class CVoid23 final : public CMonster
{
public:
	enum VOID23_ANIM
	{
		ATTACK00,
		ATTACK01,
		ATTACK01_B,
		ATTACK02,
		ATTACK02_B,
		ATTACK03,
		ATTACK03_B,
		ATTACK03_C,
		ATTACK04,
		ATTACK04_B,
		ATTACK05,
		ATTACK05_B,
		ATTACK06_B,
		ATTACK07_B,
		ATTACK08,
		ATTACK08_B,
		ATTACKDIE_B,
		B_ATTACK01,//
		B_ATTACK02,//
		B_ATTACK03,//
		B_ATTACK04,//
		B_ATTACK05,//
		B_ATTACK06,
		B_ATTACK07,
		DIE,
		DIE_B,
		HIT_ADD,
		HIT_ADD_B,
		HIT_L,
		HIT_L_B,
		HIT_R,
		HIT_R_B,
		IDLE,
		IDLE_B,
		KNOCKDOWN,
		KNOCKDOWN_B,
		NIFLHEIMROAR_B,
		ROAR,
		ROAR_B,
		ROAR_NIFLHEIMDUNGEON,
		RUN,
		RUN_B,
		STUN,
		STUN_B,
		VOID_23_RAGE,
		VOID_23_RAGE02_B,
		VOID_23_RAGE_B,
		VOID_23_RAGE_SHORT,
		WALK,
		WALK_B,
		ANIM_END
	};

	enum VOID23_STATE
	{
		STATE_IDLE,
		STATE_WALK,
		STATE_CHASE,
		STATE_ATTACK,
		STATE_HIT,
		STATE_KNOCKDOWN,
		STATE_DIE,
		STATE_END
	};

private:
	CVoid23(_dev pDevice, _context pContext);
	CVoid23(const CVoid23& rhs);
	virtual ~CVoid23() = default;

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
	VOID23_STATE m_ePreState = STATE_END;
	VOID23_STATE m_eCurState = STATE_END;

private:
	CCommonSurfaceTrail* m_pSwordTrail = { nullptr };

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
	static CVoid23* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END