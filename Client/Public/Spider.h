#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

class CSpider final : public CMonster
{
public:
	enum SPIDER_ANIM
	{
		ATTACK01,
		ATTACK02,
		ATTACK03,
		ATTACK04,
		ATTACK05,
		DIE,
		HIT_ADD,
		IDLE,
		KNOCKDOWN,
		L_HIT,
		L_TURN,
		R_HIT,
		R_TURN,
		ROAR,
		RUN,
		SALEROBIA_ATTACK06,
		SALEROBIA_ATTACK09,
		SALEROBIA_RAGE,
		STUN,
		WALK,
		ANIM_END
	};

	enum SPIDER_STATE
	{
		STATE_RAGE,
		STATE_IDLE,
		STATE_WALK,
		STATE_CHASE,
		STATE_ATTACK,
		STATE_HIT,
		STATE_SUICIDE,
		STATE_DIE,
		STATE_END
	};

private:
	CSpider(_dev pDevice, _context pContext);
	CSpider(const CSpider& rhs);
	virtual ~CSpider() = default;

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
	SPIDER_STATE m_ePreState = STATE_END;
	SPIDER_STATE m_eCurState = STATE_END;

private:
	_float m_fIdleTime = {};

	_uint m_iAttackPattern = {};
	_bool m_bSelectAttackPattern = { false };

private:
	_bool m_bAppeared = { false };

private:
	_bool m_bAttacked = { false };

	_bool m_bDamaged = { false };
	_bool m_bSlow = { false };

private:
	static const _float m_fChaseRange;
	static const _float m_fAttackRange;

private:
	static _uint m_iSpiderID;
	static _bool m_bDirSelected[8];

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CSpider* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END