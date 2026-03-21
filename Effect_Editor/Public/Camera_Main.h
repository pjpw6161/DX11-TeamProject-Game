#pragma once
#include "Effect_Define.h"
#include "Camera.h"

BEGIN(Effect)

class CCamera_Main final : public CCamera
{
private:
	CCamera_Main(_dev pDevice, _context pContext);
	CCamera_Main(const CCamera_Main& rhs);
	virtual ~CCamera_Main() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	_float m_fMouseSensor{ 0.08f };
	_float m_fTargetDistance{ 4.f };
	_bool m_bTimeStop{};
	_float m_fOriginTimeRatio{};
	_float m_fSpeed{ 1.f };
	_bool m_isMoveMode{};

public:
	static CCamera_Main* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END