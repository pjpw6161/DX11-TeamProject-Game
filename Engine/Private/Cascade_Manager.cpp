#include "Cascade_Manager.h"
#include "GameInstance.h"
#include "Cascade.h"

CCascade_Manager::CCascade_Manager(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCascade_Manager::Init()
{
	m_pCascades.reserve(3);

	for (size_t i = 0; i < 3; i++)
	{
		CCascade* pCascade = CCascade::Create(m_pDevice, m_pContext, i);
		m_pCascades.push_back(pCascade);
	}

	m_CascadeDesc.ClipZ[0] = 0.f;
	m_CascadeDesc.ClipZ[1] = 0.1f;
	m_CascadeDesc.ClipZ[2] = 0.15f;
	m_CascadeDesc.ClipZ[3] = 1.f;

	return S_OK;
}

HRESULT CCascade_Manager::Update_Cascade()
{
	_vec3 FrustumCorners[8] = {
		_vec3(-1.f, 1.f, 0.f),
		_vec3(1.f, 1.f, 0.f),
		_vec3(1.f, -1.f, 0.f),
		_vec3(-1.f, -1.f, 0.f),
		_vec3(-1.f, 1.f, 1.f),
		_vec3(1.f,1.f,1.f),
		_vec3(1.f,-1.f,1.f),
		_vec3(-1.f,-1.f,1.f)
	};

	_mat ProjInv = m_pGameInstance->Get_Transform_Inversed(TransformType::Proj);
	_mat ViewInv = m_pGameInstance->Get_Transform_Inversed(TransformType::View);

	for (_uint i = 0; i < 8; ++i) {
		FrustumCorners[i] = XMVector3TransformCoord(FrustumCorners[i], ProjInv);
		FrustumCorners[i] = XMVector3TransformCoord(FrustumCorners[i], ViewInv);
	}

	_uint iIndex = 0;
	for (auto& pCasCade : m_pCascades) {
		pCasCade->Update(FrustumCorners, m_CascadeDesc.ClipZ);
		m_CascadeDesc.LightView[iIndex] = pCasCade->Get_ViewMat();
		m_CascadeDesc.LightProj[iIndex] = pCasCade->Get_ProjMat();
		iIndex++;
	}


	return S_OK;
}

CCascade_Manager* CCascade_Manager::Create(_dev pDevice, _context pContext)
{
	CCascade_Manager* pInstance = new CCascade_Manager(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CCascade_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCascade_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	for (auto& pCascade : m_pCascades)
		Safe_Release(pCascade);

	m_pCascades.clear();
}
