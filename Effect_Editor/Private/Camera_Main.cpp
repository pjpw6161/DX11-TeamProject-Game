#include "Camera_Main.h"

CCamera_Main::CCamera_Main(_dev pDevice, _context pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Main::CCamera_Main(const CCamera_Main& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Main::Init_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Main::Init(void* pArg)
{
	if (not pArg)
	{
		MSG_BOX("null Arg : CCamera_Main");
		return E_FAIL;
	}

	if (FAILED(__super::Init(pArg)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CCamera_Main::Tick(_float fTimeDelta)
{
	fTimeDelta = fTimeDelta / m_pGameInstance->Get_TimeRatio();

	m_pGameInstance->Set_CameraNF(_vec2(m_fNear, m_fFar));

	if (m_pGameInstance->Key_Down(DIK_P))
	{
		if (m_isMoveMode)
		{
			m_isMoveMode = false;
		}
		else
		{
			m_isMoveMode = true;
		}
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

	if (m_isMoveMode)
	{
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

		if (m_pGameInstance->Key_Pressing(DIK_LCONTROL))
		{
			if (m_pGameInstance->Get_MouseMove(MouseState::wheel) > 0)
			{
				m_fSpeed += fSpeedRatio;
			}
			else if (m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
			{
				m_fSpeed -= fSpeedRatio;
				if (m_fSpeed < 0.2f)
				{
					m_fSpeed = 0.2f;
				}
			}
		}

		if (m_pGameInstance->Key_Pressing(DIK_LSHIFT) || m_pGameInstance->Gamepad_Pressing(XINPUT_B))
		{
			m_pTransformCom->Set_Speed(m_fSpeed * 2.f);
		}
		//else if (m_pGameInstance->Key_Pressing(DIK_LCONTROL) || m_pGameInstance->Gamepad_Pressing(XINPUT_A))
		//{
		//	m_pTransformCom->Set_Speed(m_fSpeed * 0.2f);
		//}
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
		if (m_pGameInstance->Key_Pressing(DIK_Z))
		{
			m_pTransformCom->Go_Down(fTimeDelta);
		}
	}
	else
	{
		if (m_pGameInstance->Key_Pressing(DIK_LCONTROL))
		{
			if (m_pGameInstance->Get_MouseMove(MouseState::wheel) > 0)
			{
				m_fTargetDistance -= 1.f;
			}
			else if (m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
			{
				m_fTargetDistance += 1.f;
			}
		}
		if (m_fTargetDistance <= 0.f)
		{
			m_fTargetDistance = 0.5f;
		}

		m_pTransformCom->Set_State(State::Pos, _vec4(0.f, 0.f, 0.f, 1.f) - (m_pTransformCom->Get_State(State::Look) * m_fTargetDistance));
	}
	__super::Tick(fTimeDelta);
}

void CCamera_Main::Late_Tick(_float fTimeDelta)
{
}

CCamera_Main* CCamera_Main::Create(_dev pDevice, _context pContext)
{
	CCamera_Main* pInstance = new CCamera_Main(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCamera_Main");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Main::Clone(void* pArg)
{
	CCamera_Main* pInstance = new CCamera_Main(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCamera_Main");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Main::Free()
{
	__super::Free();
}
