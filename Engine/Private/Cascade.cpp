#include "Cascade.h"
#include "GameInstance.h"

CCascade::CCascade(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCascade::Init(_uint iIndex)
{
	m_iIndex = iIndex;
	return S_OK;
}

HRESULT CCascade::Update(_vec3* FrustumCorners, _float* ClipZ)
{
	//_float fAspect = 1280.f / 720.f;
	//_float fNear = 0.1f;
	//_float fFar = 1100.f;
	//_float fFov = XMConvertToRadians(60.f);

	//_float tanHalfVFov = tanf(fFov / 2.f);
	//_float tanHalfHFov = tanHalfVFov * fAspect;

	//_float fXNear = ClipZ[m_iIndex] * tanHalfHFov;
	//_float fXFar = ClipZ[m_iIndex + 1] * tanHalfHFov;
	//_float fYNear = ClipZ[m_iIndex] * tanHalfVFov;
	//_float fYFar = ClipZ[m_iIndex + 1] * tanHalfVFov;

	//m_FrustumCorners[0] = { fXNear,fYNear,ClipZ[m_iIndex] };
	//m_FrustumCorners[1] = { -fXNear,fYNear,ClipZ[m_iIndex] };
	//m_FrustumCorners[2] = { fXNear,-fYNear,ClipZ[m_iIndex] };
	//m_FrustumCorners[3] = { -fXNear,-fYNear,ClipZ[m_iIndex] };
	//m_FrustumCorners[4] = { fXNear,fYNear,ClipZ[m_iIndex + 1] };
	//m_FrustumCorners[5] = { -fXNear,fYNear,ClipZ[m_iIndex + 1] };
	//m_FrustumCorners[6] = { fXNear,-fYNear,ClipZ[m_iIndex + 1] };
	//m_FrustumCorners[7] = { -fXNear,-fYNear,ClipZ[m_iIndex + 1] };


	for (_uint i = 0; i < 4; ++i) {
		_vec3 CornerRay = FrustumCorners[i + 4] - FrustumCorners[i];
		_vec3 NearCornerRay = CornerRay * ClipZ[m_iIndex];
		_vec3 FarCornerRay = CornerRay * ClipZ[m_iIndex + 1];
		m_FrustumCorners[i + 4] = FrustumCorners[i] + FarCornerRay;
		m_FrustumCorners[i] = FrustumCorners[i] + NearCornerRay;
	}


	_vec3 FrustumCenter = _vec3(0.f, 0.f, 0.f);
	for (size_t i = 0; i < 8; i++)
		FrustumCenter += m_FrustumCorners[i];
	FrustumCenter /= 8.f;


	_float SphereRadius = 0.f;

	for (size_t i = 0; i < 8; i++)
	{
		_float Dist = (m_FrustumCorners[i] - FrustumCenter).Length();
		SphereRadius = max(SphereRadius, Dist);
	}

	SphereRadius = ceilf(SphereRadius * 16.f) / 16.f;

	_vec3 vMaxes = _vec3(SphereRadius);
	_vec3 vMins = -vMaxes;

	_uint iLevel = m_pGameInstance->Get_CurrentLevelIndex();

	LIGHT_DESC* Light = m_pGameInstance->Get_LightDesc(0, L"Light_Main");

	_vec3 CascadeExtents = vMaxes - vMins;

	m_ProjMat = XMMatrixOrthographicLH(CascadeExtents.x, CascadeExtents.y, 0.01f, CascadeExtents.z);

	if (nullptr == Light) {
		m_ViewMat = m_pGameInstance->Get_Transform(TransformType::View);
		return S_OK;
	}
	_vec3 vLightDirection = _vec3(Light->vDirection).Get_Normalized();

	m_ShadowCamPos = FrustumCenter + (vLightDirection * vMins.z);

	m_ShadowCamPos.w = 1.f;

	_vec3 vLookAt = FrustumCenter;

	m_ViewMat = XMMatrixLookAtLH(m_ShadowCamPos, vLookAt, _vec3(0.f, 1.f, 0.f));



	return S_OK;
}

CCascade* CCascade::Create(_dev pDevice, _context pContext, _uint iIndex)
{
	CCascade* pInstance = new CCascade(pDevice, pContext);

	if (FAILED(pInstance->Init(iIndex)))
	{
		MSG_BOX("Failed to Create : CCascade");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCascade::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
