#pragma once
#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(_dev pDevice, _context pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_hasBGMStarted{};

public:
	static CLevel_Logo* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END