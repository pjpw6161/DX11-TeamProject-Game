#pragma once

#include "Client_Define.h"
#include "NPC.h"

BEGIN(Client)

class CBlackSmith final : public CNPC
{
public:
	enum BLACKSMITH_ANIM
	{
		IDLE,
		IDLE02,
		REACTION01,
		REACTION02,
		TALK01,
		WALK01,
		ANIM_END
	};

private:
	CBlackSmith(_dev pDevice, _context pContext);
	CBlackSmith(const CBlackSmith& rhs);
	virtual ~CBlackSmith() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_bTalking = { false };

public:
	static CBlackSmith* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END