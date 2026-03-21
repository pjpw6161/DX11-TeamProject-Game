#pragma once

#include "Client_Define.h"
#include "Monster.h"
#include "CommonTrail.h"

BEGIN(Client)

class CVoid05 final : public CMonster
{
public:
	enum VOID05_ANIM
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
		ROAR,
		RUN,
		STUN,
		TU02_SC02_MON_01,
		TU02_SC02_MON_ATTACK_LOOP,
		TU02_SC03_MON01,
		TU_SC11_01_MON,
		TU_SC18_02_MON,
		TURN_L,
		TURN_R,
		VOID_DIE_POSE_01,
		VOID_DIE_POSE_02,
		VOID_DIE_POSE_03,
		WALK,
		ANIM_END
	};

	enum VOID05_STATE
	{
		STATE_IDLE,
		STATE_DIG,
		STATE_CHASE,
		STATE_ATTACK,
		STATE_HIT,
		STATE_DIE,
		STATE_END
	};

private:
	CVoid05(_dev pDevice, _context pContext);
	CVoid05(const CVoid05& rhs);
	virtual ~CVoid05() = default;

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
	VOID05_STATE m_ePreState = STATE_END;
	VOID05_STATE m_eCurState = STATE_END;

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
	_bool m_bAttacked2 = { false };

	_bool m_bDamaged = { false };
	_bool m_bSlow = { false };

private:
	VOID05_ANIM m_eIdleAnim = { ANIM_END };

private:
	static const _float m_fChaseRange;
	static const _float m_fAttackRange;

private:
	static _uint m_iIndex;

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CVoid05* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END