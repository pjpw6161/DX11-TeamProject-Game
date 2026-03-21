#pragma once

#include "Client_Define.h"
#include "Monster.h"
#include "CommonSurfaceTrail.h"

BEGIN(Client)

class CVoid09 final : public CMonster
{
public:
	enum VOID09_ANIM
	{
		ATTACK01,//
		ATTACK01_S,
		ATTACK02,//
		ATTACK02_S,
		ATTACK03,//
		ATTACK03_S,
		ATTACK03_W,
		ATTACK04,//
		ATTACK05,
		ATTACK05_S,
		ATTACK06,
		DIE,
		HIT_ADD,
		IDLE,
		KNOCKDOWN,
		L_HIT,
		L_TURN,
		PETTI_RAGE,
		R_HIT,
		R_TURN,
		ROAR,
		RUN,
		STUN,
		WALK,
		ANIM_END
	};

	enum VOID09_STATE
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
	CVoid09(_dev pDevice, _context pContext);
	CVoid09(const CVoid09& rhs);
	virtual ~CVoid09() = default;

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
	VOID09_STATE m_ePreState = STATE_END;
	VOID09_STATE m_eCurState = STATE_END;

private:
	CCommonSurfaceTrail* m_pSwordTrail = { nullptr };

private:
	_float m_fIdleTime = {};

	_uint m_iAttackPattern = {};
	_bool m_bSelectAttackPattern = { false };

private:
	_bool m_bAttacking = { false };

	_bool m_bAttacked = { false };

	_bool m_bDamaged = { false };
	_bool m_bSlow = { false };

private:
	static const _float m_fChaseRange;
	static const _float m_fAttackRange;

	static _uint m_iIndex;

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CVoid09* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END