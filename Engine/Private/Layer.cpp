#include "Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

CComponent* CLayer::Get_Component(const wstring& strComponentTag, _uint iIndex)
{
	auto it = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
	{
		++it;
	}

	if (it == m_GameObjects.end())
	{
		return nullptr;
	}

	return (*it)->Find_Component(strComponentTag);
}

HRESULT CLayer::Add_GameObejct(CGameObject* pGameObejct)
{
	if (!pGameObejct)
	{
		return E_FAIL;
	}

	m_GameObjects.push_back(pGameObejct);

	return S_OK;
}
_uint CLayer::Get_LayerSize()
{
	return (_uint)m_GameObjects.size();
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (pGameObject)
		{
			pGameObject->Tick(fTimeDelta);
		}
	}
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (pGameObject)
		{
			pGameObject->Late_Tick(fTimeDelta);
		}
	}
}

void CLayer::Release_DeadObjects()
{
	for (auto& it = m_GameObjects.begin(); it != m_GameObjects.end();)
	{
		if ((*it)->isDead())
		{
			Safe_Release(*it);
			it = m_GameObjects.erase(it);
		}
		else
		{
			++it;
		}
	}
}

CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
	{
		Safe_Release(pGameObject);
	}

	m_GameObjects.clear();
}
