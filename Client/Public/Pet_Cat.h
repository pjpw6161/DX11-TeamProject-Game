#pragma once

#include "Client_Define.h"
#include "Pet.h"

BEGIN(Client)

class CPet_Cat final : public CPet
{
public:
	enum PET_CAT_ANIM
	{
		COLLECT,
		EMOTION,
		IDLE,
		RUN,
		TELEPORT_END,
		TELEPORT_START,
		ANIM_END
	};

	enum PET_CAT_STATE
	{
		STATE_IDLE,
		STATE_CHASE,
		STATE_EMOTION,
		STATE_INVEN,
		STATE_END
	};

private:
	CPet_Cat(_dev pDevice, _context pContext);
	CPet_Cat(const CPet_Cat& rhs);
	virtual ~CPet_Cat() = default;

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
	PET_CAT_STATE m_ePreState = STATE_END;
	PET_CAT_STATE m_eCurState = STATE_END;

private:
	_float m_fIdleTime = {};

private:
	_bool m_bInvenOn = { false };

public:
	static CPet_Cat* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END