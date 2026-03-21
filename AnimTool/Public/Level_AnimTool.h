#pragma once
#include "AnimTool_Define.h"
#include "Level.h"

BEGIN(AnimTool)

class CLevel_AnimTool final : public CLevel
{
private:
	CLevel_AnimTool(_dev pDevice, _context pContext);
	virtual ~CLevel_AnimTool() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Camera();
	HRESULT Ready_Light();
	HRESULT Ready_Player();

public:
	static CLevel_AnimTool* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END