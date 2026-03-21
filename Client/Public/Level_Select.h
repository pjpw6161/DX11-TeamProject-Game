#pragma once
#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Select final : public CLevel
{
private:
	CLevel_Select(_dev pDevice, _context pContext);
	virtual ~CLevel_Select() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Select();
	HRESULT Ready_Model();

	HRESULT Ready_Npc();
	HRESULT Ready_Map();
	HRESULT Ready_Camera();
	HRESULT Ready_Light();

public:
	static CLevel_Select* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END