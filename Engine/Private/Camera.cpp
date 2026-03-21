#include "Camera.h"

CCamera::CCamera(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CCamera::CCamera(const CCamera& rhs)
	: CGameObject(rhs)
	, m_isPlayCutScene(rhs.m_isPlayCutScene)
{
}

HRESULT CCamera::Init_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Init(void* pArg)
{
	if (!pArg)
	{
		return E_FAIL;
	}

	Camera_Desc* pCameraDesc = reinterpret_cast<Camera_Desc*>(pArg);

	m_fFovY = pCameraDesc->fFovY;
	m_fAspect = pCameraDesc->fAspect;
	m_fNear = pCameraDesc->fNear;
	m_fFar = pCameraDesc->fFar;

	m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&pCameraDesc->vCameraPos));
	m_pTransformCom->LookAt(XMLoadFloat4(&pCameraDesc->vFocusPos));
	m_pGameInstance->Set_Transform(TransformType::View, m_pTransformCom->Get_World_Inverse());
	m_pGameInstance->Set_Transform(TransformType::Proj, XMMatrixPerspectiveFovLH(m_fFovY, m_fAspect, m_fNear, m_fFar));
	return S_OK;
}

void CCamera::Tick(_float fTimeDelta)
{
	m_pGameInstance->Set_OldViewMatrix(m_pGameInstance->Get_Transform(TransformType::View));
	m_pGameInstance->Set_Transform(TransformType::View, m_pTransformCom->Get_World_Inverse());
	m_pGameInstance->Set_Transform(TransformType::Proj, XMMatrixPerspectiveFovLH(m_fFovY, m_fAspect, m_fNear, m_fFar));
}

void CCamera::Late_Tick(_float fTimeDelta)
{
}

void CCamera::Free()
{
	__super::Free();
}
