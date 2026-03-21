#pragma once
#include "AnimTool_Define.h"
#include "Camera.h"

BEGIN(AnimTool)

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
	CTransform* m_pPlayerTransform{ nullptr };
	_bool m_isMoveMode{ true };
	_float m_fMouseSensor{ 0.08f };
	_float m_fSpeed{ 10.f };
	_float m_fPlayerDistance{ 4.f };
	_float m_fShakeAcc{ 100.f };

private:
	_float m_fSceneTimer{};
	_uint m_iSceneIndex{};
	CUTSCENE* m_pScene{ nullptr };

public:
	static CCamera_Main* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END