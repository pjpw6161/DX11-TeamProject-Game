#pragma once

#include "Client_Define.h"
#include "NPC.h"

BEGIN(Client)

class CAur final : public CNPC
{
public:
	enum AUR_ANIM
	{
		CHEER,
		EMO01,
		HANGING01,
		HANGING02,
		IDLE01,
		IDLE02,
		SIT01,
		TALK01,
		TALK02,
		WALK,
		ANIM_END
	};

private:
	CAur(_dev pDevice, _context pContext);
	CAur(const CAur& rhs);
	virtual ~CAur() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_bTalking = { false };

public:
	static CAur* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END