#include "Object_Manager.h"
#include "Layer.h"
#include "GameObject.h"

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Init(_uint iNumLevels)
{
	if (m_pLayers)
	{
		return E_FAIL;
	}

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (Find_Prototype(strPrototypeTag))
	{
		MSG_BOX("PrototypeTag Already Exist!");
		return E_FAIL;
	}

	m_Prototype.emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_Layer(_uint iLevelIndex, const wstring strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (!pPrototype)
	{
		return E_FAIL;
	}

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (!pGameObject)
	{
		return E_FAIL;
	}

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (pLayer)
	{
		pLayer->Add_GameObejct(pGameObject);
	}
	else
	{
		pLayer = CLayer::Create();
		if (!pLayer)
		{
			return E_FAIL;
		}

		if (FAILED(pLayer->Add_GameObejct(pGameObject)))
		{
			return E_FAIL;
		}

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}

	return S_OK;
}

_uint CObject_Manager::Get_LayerSize(_uint iLevelIndex, const wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
		return 0;

	return pLayer->Get_LayerSize();
}

CGameObject* CObject_Manager::Clone_Object(const wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (!pPrototype)
	{
		return nullptr;
	}

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (!pGameObject)
	{
		return nullptr;
	}

	return pGameObject;
}

void CObject_Manager::Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Tick(fTimeDelta);
		}
	}
}

void CObject_Manager::Late_Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Late_Tick(fTimeDelta);
		}
	}
}

void CObject_Manager::Release_DeadObjects()
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Release_DeadObjects();
		}
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		return;
	}

	for (auto& Pair : m_pLayers[iLevelIndex])
	{
		Safe_Release(Pair.second);
	}
	m_pLayers[iLevelIndex].clear();
}

CComponent* CObject_Manager::Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (!pLayer)
	{
		return nullptr;
	}

	return pLayer->Get_Component(strComponentTag, iIndex);
}

CGameObject* CObject_Manager::Find_Prototype(const wstring& strPrototypeTag)
{
	auto& it = m_Prototype.find(strPrototypeTag);

	if (it == m_Prototype.end())
	{
		return nullptr;
	}

	return it->second;
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		return nullptr;
	}

	auto& it = m_pLayers[iLevelIndex].find(strLayerTag);

	if (it == m_pLayers[iLevelIndex].end())
	{
		return nullptr;
	}

	return it->second;
}

CObject_Manager* CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager* pInstance = new CObject_Manager();

	if (FAILED(pInstance->Init(iNumLevels)))
	{
		MSG_BOX("Failed to Create : CObject_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject_Manager::Free()
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Safe_Release(Pair.second);
		}
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototype)
	{
		Safe_Release(Pair.second);
	}
	m_Prototype.clear();
}
