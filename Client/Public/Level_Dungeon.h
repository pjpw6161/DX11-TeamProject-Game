#pragma once
#include "Client_Define.h"
#include "Level.h"


BEGIN(Client)

class CLevel_Dungeon final : public CLevel
{
private:
	CLevel_Dungeon(_dev pDevice, _context pContext);
	virtual ~CLevel_Dungeon() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Camera();
	HRESULT Ready_Light();
	HRESULT Ready_Player();
	HRESULT Ready_Map();

	HRESULT Ready_Object();

	HRESULT Ready_Environment();


	HRESULT Ready_NpcvsMon();


	//UI
	HRESULT Ready_UI();

public:
	static CLevel_Dungeon* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END