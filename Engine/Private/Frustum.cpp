#include "Frustum.h"
#include "GameInstance.h"

CFrustum::CFrustum()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Init()
{
	m_pPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_pPoints[1] = _float3(1.f, 1.f, 0.f);
	m_pPoints[2] = _float3(1.f, -1.f, 0.f);
	m_pPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_pPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_pPoints[5] = _float3(1.f, 1.f, 1.f);
	m_pPoints[6] = _float3(1.f, -1.f, 1.f);
	m_pPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Tick()
{
	_matrix matProjInv = m_pGameInstance->Get_Transform_Inversed(TransformType::Proj);
	_matrix matViewInv = m_pGameInstance->Get_Transform_Inversed(TransformType::View);

	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat3(&m_pPoints_World[i], XMVector3TransformCoord(XMLoadFloat3(&m_pPoints[i]), matProjInv));
		XMStoreFloat3(&m_pPoints_World[i], XMVector3TransformCoord(XMLoadFloat3(&m_pPoints_World[i]), matViewInv));
	}

	Make_Planes(m_pPoints_World, m_pPlanes_World);
}

_bool CFrustum::IsIn_Fov_World(_fvector vPos, _float fRange)
{
	_float DotProduct{};
	for (size_t i = 0; i < 6; i++)
	{
		DotProduct = XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_pPlanes_World[i]), vPos));
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_pPlanes_World[i]), vPos)) > fRange)
		{
			return false;
		}
	}

	return true;
}

void CFrustum::Transform_ToLocalSpace(_fmatrix vWorldInversed)
{
	_float3 vPoints_Local[8]{};

	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat3(&vPoints_Local[i], XMVector3TransformCoord(XMLoadFloat3(&vPoints_Local[i]), vWorldInversed));
	}

	Make_Planes(vPoints_Local, m_pPlanes_Local);
}

_bool CFrustum::IsIn_Fov_Local(_fvector vPos, _float fRange)
{
	_float DotProduct{};
	for (size_t i = 0; i < 6; i++)
	{
		DotProduct = XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_pPlanes_Local[i]), vPos));
		if (DotProduct > fRange)
		{
			return false;
		}
	}

	return true;
}

HRESULT CFrustum::Make_Planes(const _float3* pPoints, _Inout_ _float4* pPlanes)
{
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[6])));
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[3])));

	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[1])));
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat3(&pPoints[3]), XMLoadFloat3(&pPoints[2]), XMLoadFloat3(&pPoints[6])));

	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[7])));
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[2])));

	return S_OK;
}

CFrustum* CFrustum::Create()
{
	CFrustum* pInstance = new CFrustum();

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CFrustum");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrustum::Free()
{
	Safe_Release(m_pGameInstance);
}
