#pragma once

#include "Client_Define.h"
#include "NPC.h"

BEGIN(Client)

class CCat final : public CNPC
{
public:
	enum CAT_ANIM
	{
		IDLE01,
		IDLE02,
		RUN,
		WALK,
		ANIM_END
	};

	enum CAT_STATE
	{
		STATE_IDLE,
		STATE_SIT,
		STATE_WALK,
		STATE_RUN,
		STATE_END
	};

private:
	CCat(_dev pDevice, _context pContext);
	CCat(const CCat& rhs);
	virtual ~CCat() = default;

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
	CAT_STATE m_ePreState = STATE_END;
	CAT_STATE m_eCurState = STATE_END;

private:
	_float m_fIdleTime = {};

private:
	_uint m_iPattern = {};
	_bool m_bSelectPattern = { false };

public:
	static CCat* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END