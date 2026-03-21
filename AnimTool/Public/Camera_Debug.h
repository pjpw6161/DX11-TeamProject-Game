#pragma once
#include "AnimTool_Define.h"
#include "Camera.h"

BEGIN(AnimTool)

class CCamera_Debug final : public CCamera
{
private:
	CCamera_Debug(_dev pDevice, _context pContext);
	CCamera_Debug(const CCamera_Debug& rhs);
	virtual ~CCamera_Debug() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	_bool m_isMoveMode{ true };
	_float m_fMouseSensor{ 0.1f };
	_float m_fSpeed{ 10.f };
	_bool m_bTimeStop{};
	_float m_fOriginTimeRatio{};

public:
	static CCamera_Debug* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END