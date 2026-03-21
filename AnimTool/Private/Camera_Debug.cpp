#include "Camera_Debug.h"

CCamera_Debug::CCamera_Debug(_dev pDevice, _context pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Debug::CCamera_Debug(const CCamera_Debug& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Debug::Init_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Debug::Init(void* pArg)
{
	if (not pArg)
	{
		MSG_BOX("null Arg : CCamera_Debug");
		return E_FAIL;
	}

	if (FAILED(__super::Init(pArg)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CCamera_Debug::Tick(_float fTimeDelta)
{
	m_pGameInstance->Set_CameraNF(_float2(m_fNear, m_fFar));
	fTimeDelta /= m_pGameInstance->Get_TimeRatio();

	if (m_pGameInstance->Key_Down(DIK_P))
	{
		m_pTransformCom->Set_Position(_vec3(0.f, 5.f, 5.f));
	}

	if (m_pGameInstance->Key_Down(DIK_I))
	{
		if (m_bTimeStop)
		{
			m_bTimeStop = false;
			m_pGameInstance->Set_TimeRatio(m_fOriginTimeRatio);
		}
		else
		{
			m_bTimeStop = true;
			m_fOriginTimeRatio = m_pGameInstance->Get_TimeRatio();
		}
	}

	if (m_pGameInstance->Key_Down(DIK_U))
	{
		m_fSpeed = { 10.f };
	}

	if (m_bTimeStop)
	{
		m_pGameInstance->Set_TimeRatio(0.001f);
	}

	if (m_pGameInstance->Mouse_Pressing(DIM_MBUTTON))
	{
		_long dwMouseMove;

		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * dwMouseMove * m_fMouseSensor);
		}

		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(State::Right), fTimeDelta * dwMouseMove * m_fMouseSensor);
		}

	}

	_float fRStickMove{};
	if (fRStickMove = m_pGameInstance->Gamepad_RStick().x)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * fRStickMove);
	}
	if (fRStickMove = m_pGameInstance->Gamepad_RStick().y)
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(State::Right), fTimeDelta * -fRStickMove);
	}

	_float fSpeedRatio{};
	if (m_fSpeed < 1.5f)
	{
		fSpeedRatio = 0.2f;
	}
	else if (m_fSpeed < 10.f)
	{
		fSpeedRatio = 1.f;
	}
	else if (m_fSpeed < 50.f)
	{
		fSpeedRatio = 10.f;
	}
	else
	{
		fSpeedRatio = 30.f;
	}

	if (m_pGameInstance->Get_MouseMove(MouseState::wheel) > 0)
	{
		m_fSpeed += fSpeedRatio;
		if (m_fSpeed > 10.f)
		{
			m_fSpeed = 10.f;
		}
	}
	else if (m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
	{
		m_fSpeed -= fSpeedRatio;
		if (m_fSpeed < 0.7f)
		{
			m_fSpeed = 0.7f;
		}
	}
	else
	{
		m_pTransformCom->Set_Speed(m_fSpeed);
	}


	if (m_pGameInstance->Key_Pressing(DIK_W) || m_pGameInstance->Gamepad_LStick().y > 0.5f)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing(DIK_S) || m_pGameInstance->Gamepad_LStick().y < -0.5f)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing(DIK_A) || m_pGameInstance->Gamepad_LStick().x < -0.5f)
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing(DIK_D) || m_pGameInstance->Gamepad_LStick().x > 0.5f)
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing(DIK_Q))
	{
		m_pTransformCom->Go_Up(fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing(DIK_E))
	{
		m_pTransformCom->Go_Down(fTimeDelta);
	}

	__super::Tick(fTimeDelta);
}

void CCamera_Debug::Late_Tick(_float fTimeDelta)
{
}

CCamera_Debug* CCamera_Debug::Create(_dev pDevice, _context pContext)
{
	CCamera_Debug* pInstance = new CCamera_Debug(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCamera_Debug");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Debug::Clone(void* pArg)
{
	CCamera_Debug* pInstance = new CCamera_Debug(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCamera_Debug");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Debug::Free()
{
	__super::Free();
}
