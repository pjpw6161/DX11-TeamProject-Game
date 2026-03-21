#pragma once
#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Custom final : public CLevel
{
private:
	CLevel_Custom(_dev pDevice, _context pContext);
	virtual ~CLevel_Custom() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_hasBGMStarted{};

private:
	HRESULT Ready_Player();
	HRESULT Ready_UI();
	HRESULT Ready_Light();

public:
	static CLevel_Custom* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END