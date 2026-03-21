#pragma once

#include "Client_Define.h"
#include "Monster.h"
#include "CommonSurfaceTrail.h"

BEGIN(Client)

class CVoid20 final : public CMonster
{
public:
	enum VOID20_ANIM
	{
		ATTACK01,
		ATTACK02,
		ATTACK03,
		ATTACK04,
		DIE,
		HIT_ADD,
		HIT_L,
		HIT_R,
		IDLE,
		KNOCKDOWN,
		ROAR,
		RUN,
		STUN,
		TURN_L,
		TURN_R,
		WALK,
		ANIM_END
	};

	enum VOID20_STATE
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
	CVoid20(_dev pDevice, _context pContext);
	CVoid20(const CVoid20& rhs);
	virtual ~CVoid20() = default;

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
	VOID20_STATE m_ePreState = STATE_END;
	VOID20_STATE m_eCurState = STATE_END;

private:
	CCommonSurfaceTrail* m_pSwordTrailL = { nullptr };
	CCommonSurfaceTrail* m_pSwordTrailR = { nullptr };

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

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CVoid20* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END