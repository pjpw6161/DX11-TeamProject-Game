#pragma once

#include "Client_Define.h"
#include "NPC.h"

BEGIN(Client)

class CDog final : public CNPC
{
public:
	enum DOG_ANIM
	{
		IDLE01,
		IDLE02,
		RUN,
		WALK,
		ANIM_END
	};

	enum DOG_STATE
	{
		STATE_IDLE,
		STATE_SIT,
		STATE_WALK,
		STATE_RUN,
		STATE_END
	};

private:
	CDog(_dev pDevice, _context pContext);
	CDog(const CDog& rhs);
	virtual ~CDog() = default;

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
	DOG_STATE m_ePreState = STATE_END;
	DOG_STATE m_eCurState = STATE_END;

private:
	_float m_fIdleTime = {};

private:
	_uint m_iPattern = {};
	_bool m_bSelectPattern = { false };

public:
	static CDog* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END