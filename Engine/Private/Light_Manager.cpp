#include "Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iLevelIndex, const wstring& strLightTag)
{
	CLight* pLight = Find_Light(iLevelIndex, strLightTag);
	if (not pLight)
	{
		return nullptr;
	}

	return pLight->Get_LightDesc();
}

HRESULT CLight_Manager::Add_Light(_uint iLevelIndex, const wstring& strLightTag, const LIGHT_DESC& LightDesc)
{
	if (Find_Light(iLevelIndex, strLightTag))
	{
		MSG_BOX("Light Tag Already Exist!");
		return E_FAIL;
	}

	CLight* pLight = CLight::Create(LightDesc);
	if (!pLight)
	{
		return E_FAIL;
	}

	m_pLights[iLevelIndex].emplace(strLightTag, pLight);

	return S_OK;
}

HRESULT CLight_Manager::Delete_Light(_uint iLevelIndex, const wstring& strLightTag, _float fDimmerDuration)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		return E_FAIL;
	}

	auto iter = m_pLights[iLevelIndex].find(strLightTag);
	if (iter == m_pLights[iLevelIndex].end())
	{
		return E_FAIL;
	}

	iter->second->Dim(fDimmerDuration);

	return S_OK;
}

HRESULT CLight_Manager::Bind_ViewProjMatrix(_uint iLevelIndex, const wstring& strLightTag, CShader* pShader, const _char* pViewVariableName, const _char* pProjVariableName)
{
	CLight* pLight = Find_Light(iLevelIndex, strLightTag);
	if (not pLight)
	{
		return E_FAIL;
	}

	return pLight->Bind_ViewProjMatrix(pShader, pViewVariableName, pProjVariableName);
}

HRESULT CLight_Manager::Init(_uint iNumLevels)
{
	if (m_pLights)
	{
		return E_FAIL;
	}

	m_pLights = new map<const wstring, CLight*>[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

void CLight_Manager::Tick(_uint iLevelIndex, _float fTimeDelta)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		MSG_BOX("Wrong Level Index");
		return;
	}

	for (auto iter = m_pLights[0].begin(); iter != m_pLights[0].end();)
	{
		iter->second->Tick(fTimeDelta);
		if (iter->second->Is_Dead())
		{
			Safe_Release(iter->second);
			iter = m_pLights[0].erase(iter);
		}
		else
		{
			++iter;
		}
	}
	for (auto iter = m_pLights[iLevelIndex].begin(); iter != m_pLights[iLevelIndex].end();)
	{
		iter->second->Tick(fTimeDelta);
		if (iter->second->Is_Dead())
		{
			Safe_Release(iter->second);
			iter = m_pLights[iLevelIndex].erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CLight_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		return;
	}

	for (auto& pLight : m_pLights[iLevelIndex])
	{
		Safe_Release(pLight.second);
	}

	m_pLights[iLevelIndex].clear();
}

HRESULT CLight_Manager::Render(_uint iLevelIndex, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		return E_FAIL;
	}

	for (auto& pLight : m_pLights[0])
	{
		if (FAILED(pLight.second->Render(pShader, pVIBuffer)))
		{
			return E_FAIL;
		}
	}

	for (auto& pLight : m_pLights[iLevelIndex])
	{
		if (FAILED(pLight.second->Render(pShader, pVIBuffer)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

CLight* CLight_Manager::Find_Light(_uint iLevelIndex, const wstring& strLightTag)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		return nullptr;
	}

	auto iter = m_pLights[iLevelIndex].find(strLightTag);
	if (iter == m_pLights[iLevelIndex].end())
	{
		return nullptr;
	}

	return iter->second;
}

CLight_Manager* CLight_Manager::Create(_uint iNumLevels)
{
	CLight_Manager* pInstance = new CLight_Manager();

	if (FAILED(pInstance->Init(iNumLevels)))
	{
		MSG_BOX("Failed to Create : CLight_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight_Manager::Free()
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& pLight : m_pLights[i])
		{
			Safe_Release(pLight.second);
		}
		m_pLights[i].clear();
	}
	Safe_Delete_Array(m_pLights);
}
