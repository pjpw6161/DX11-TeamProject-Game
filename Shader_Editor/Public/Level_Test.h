#pragma once

#include "Shader_Define.h"
#include "Level.h"

BEGIN(ShaderEdit)

class CLevel_Test final :
    public CLevel
{
private:
	CLevel_Test(_dev pDevice, _context pContext);
	virtual ~CLevel_Test() = default;

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
	static CLevel_Test* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END