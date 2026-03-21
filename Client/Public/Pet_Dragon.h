#pragma once

#include "Client_Define.h"
#include "Pet.h"

BEGIN(Client)

class CPet_Dragon final : public CPet
{
public:
	enum PET_DRAGON_ANIM
	{
		COLLECT,
		COLLECT001,
		COLLECT002,
		PET_04_EMOTION,
		PET_04_IDLE,
		PET_05_EMOTION,
		PET_05_IDLE,
		PET_06_EMOTION,
		PET_06_IDLE,
		RUN,
		RUN001,
		RUN002,
		TELEPORT_END,
		TELEPORT_END001,
		TELEPORT_END002,
		TELEPORT_START,
		TELEPORT_START001,
		TELEPORT_START002,
		ANIM_END
	};

	enum PET_DRAGON_STATE
	{
		STATE_IDLE,
		STATE_CHASE,
		STATE_EMOTION,
		STATE_INVEN,
		STATE_END
	};

private:
	CPet_Dragon(_dev pDevice, _context pContext);
	CPet_Dragon(const CPet_Dragon& rhs);
	virtual ~CPet_Dragon() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

private:
	PET_DRAGON_STATE m_ePreState = STATE_END;
	PET_DRAGON_STATE m_eCurState = STATE_END;

private:
	_float m_fIdleTime = {};

private:
	_bool m_bInvenOn = { false };

public:
	static CPet_Dragon* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END