#pragma once

#include "Shader_Define.h"

#include "Camera.h"

BEGIN(ShaderEdit)

class CShader_Camera final :
    public CCamera
{
private:
	CShader_Camera(_dev pDevice, _context pContext);
	CShader_Camera(const CShader_Camera& rhs);
	virtual ~CShader_Camera() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	_float m_fMouseSensor{ 0.1f };
	_float m_fSpeed{ 10.f };
	_bool m_bTimeStop{};
	_float m_fOriginTimeRatio{};

public:
	static CShader_Camera* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END