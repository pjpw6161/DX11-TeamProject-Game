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
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	if (not m_pPlayerTransform)
	{
		return E_FAIL;
	}
	Safe_AddRef(m_pPlayerTransform);
	
	if (not m_pTransformCom)
	{
		MSG_BOX("Can't Find Player!! : Camera Main");
	}

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

void CCamera_Main::Tick(_float fTimeDelta)
{
	m_pGameInstance->Set_CameraNF(_float2(m_fNear, m_fFar));

	_long dwMouseMove;

	if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta / m_pGameInstance->Get_TimeRatio() * dwMouseMove * m_fMouseSensor);
	}

	if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(State::Right), fTimeDelta / m_pGameInstance->Get_TimeRatio() * dwMouseMove * m_fMouseSensor);
	}


	_float fRStickMove{};
	if (fRStickMove = m_pGameInstance->Gamepad_RStick().x)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta / m_pGameInstance->Get_TimeRatio() * fRStickMove);
	}
	if (fRStickMove = m_pGameInstance->Gamepad_RStick().y)
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(State::Right), fTimeDelta / m_pGameInstance->Get_TimeRatio() * -fRStickMove);
	}

	if (m_pGameInstance->Get_MouseMove(MouseState::wheel) > 0)
	{
		m_fPlayerDistance -= 1.f;
	}
	else if (m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
	{
		m_fPlayerDistance += 1.f;
	}

	// 	y = sin(x * 10.0f) * powf(0.5f, x)

	_float fShakeAmount = sin(m_fShakeAcc * 15.f) * powf(0.5f, m_fShakeAcc) * 0.2f;

	m_pTransformCom->Set_State(State::Pos,
		m_pPlayerTransform->Get_CenterPos()
		- (m_pTransformCom->Get_State(State::Look) * m_fPlayerDistance)
		+ (m_pTransformCom->Get_State(State::Up) * m_fPlayerDistance * 0.25f));

	_vec4 vLook = m_pTransformCom->Get_State(State::Look);
	PxRaycastBuffer Buffer{};
	_vec4 vRayDir{};
	_vec4 vMyPos = m_pTransformCom->Get_State(State::Pos);
	_vec4 PlayerCenter = m_pPlayerTransform->Get_CenterPos();
	vRayDir = vMyPos - PlayerCenter;
	vRayDir.Normalize();
	_float fDist = XMVectorGetX(XMVector3Length(vRayDir)) - 0.4f;
	if (m_pGameInstance->Raycast(m_pPlayerTransform->Get_CenterPos() + vRayDir * 0.5f, vRayDir, fDist, Buffer))
	{
		m_pTransformCom->Set_State(State::Pos, PxVec3ToVector(Buffer.block.position, 1.f));
	}

	_vec4 vShakePos = m_pTransformCom->Get_State(State::Pos);
	vShakePos += XMVectorSet(fShakeAmount, -fShakeAmount, 0.f, 0.f);
	m_pTransformCom->Set_State(State::Pos, vShakePos);

	m_fShakeAcc += fTimeDelta * 10.f;
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

	Safe_Release(m_pPlayerTransform);
}
