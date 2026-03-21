#include "Component_Manager.h"
#include "Component.h"

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Init(_uint iNumLevels, _dev pDevice, _context pContext)
{
    if (m_pPrototypes)
    {
        return E_FAIL;
    }

    m_pPrototypes = new PROTOTYPES[iNumLevels];

    m_iNumLevels = iNumLevels;

    Add_Prototype(0, TEXT("Prototype_Component_Transform"), CTransform::Create(pDevice, pContext));

    return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const wstring& strPrototype, CComponent* pPrototype)
{
    if (Find_Prototype(iLevelIndex, strPrototype))
    {
        MSG_BOX("PrototypeTag Already Exist!");
        return E_FAIL;
    }

    m_pPrototypes[iLevelIndex].emplace(strPrototype, pPrototype);

    return S_OK;
}

CComponent* CComponent_Manager::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
    CComponent* pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
    if (!pPrototype)
    {
        return nullptr;
    }

    return pPrototype->Clone(pArg);
}

void CComponent_Manager::Clear(_uint iLevelIndex)
{
    if (iLevelIndex >= m_iNumLevels)
    {
        return;
    }

    for (auto& Pair : m_pPrototypes[iLevelIndex])
    {
        Safe_Release(Pair.second);
    }

    m_pPrototypes[iLevelIndex].clear();
}

CComponent* CComponent_Manager::Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
    auto& it = m_pPrototypes[iLevelIndex].find(strPrototypeTag);
    if (it == m_pPrototypes[iLevelIndex].end())
    {
        return nullptr;
    }

    return it->second;
}

CComponent_Manager* CComponent_Manager::Create(_uint iNumLevels, _dev pDevice, _context pContext)
{
    CComponent_Manager* pInstance = new CComponent_Manager();

    if (FAILED(pInstance->Init(iNumLevels, pDevice, pContext)))
    {
        MSG_BOX("Failed to Create : CComponent_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CComponent_Manager::Free()
{
    for (size_t i = 0; i < m_iNumLevels; i++)
    {
        for (auto& Pair : m_pPrototypes[i])
        {
            Safe_Release(Pair.second);
        }
        m_pPrototypes[i].clear();
    }
    Safe_Delete_Array(m_pPrototypes);
}
