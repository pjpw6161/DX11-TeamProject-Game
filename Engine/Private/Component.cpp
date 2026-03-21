#include "Component.h"
#include "GameInstance.h"

CComponent::CComponent(_dev pDevice, _context pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
    , m_pGameInstance(CGameInstance::Get_Instance())
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pDevice);
}

CComponent::CComponent(const CComponent& rhs)
    : m_pDevice(rhs.m_pDevice)
    , m_pContext(rhs.m_pContext)
    , m_pGameInstance(rhs.m_pGameInstance)
    , m_hasCloned(true)
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pDevice);
}

HRESULT CComponent::Init_Prototype()
{
    return S_OK;
}

HRESULT CComponent::Init(void* pArg)
{
    return S_OK;
}

HRESULT CComponent::Render()
{
    return S_OK;
}

void CComponent::Free()
{
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
