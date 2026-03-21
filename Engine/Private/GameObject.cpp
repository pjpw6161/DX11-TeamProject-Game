#include "GameObject.h"
#include "GameInstance.h"

_int CGameObject::m_iNextID = 0;

CGameObject::CGameObject(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);

}

CGameObject::CGameObject(const CGameObject& rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_pGameInstance(rhs.m_pGameInstance)
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);

	if (FAILED(Add_Component(0, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom))))
	{
		MSG_BOX("Failed to Import TransformCom");
	}

	m_iID = m_iNextID++;
}

const _bool& CGameObject::Is_Blur()
{
	return m_shouldRenderBlur;
}

HRESULT CGameObject::Init_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Init(void* pArg)
{
	return S_OK;
}

void CGameObject::Tick(_float fTimeDelta)
{
}

void CGameObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::Render_Instance()
{
	return S_OK;
}

HRESULT CGameObject::Render_Shadow()
{
	return S_OK;
}

HRESULT CGameObject::Render_Reflection(_float4 vClipPlane)
{
	return S_OK;
}

CComponent* CGameObject::Find_Component(const wstring& strComTag)
{
	auto& it = m_Components.find(strComTag);
	if (it == m_Components.end())
	{
		return nullptr;
	}

	return it->second;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strComTag, _Inout_ CComponent** ppComponent, void* pArg)
{
	if (!m_pGameInstance)
	{
		return E_FAIL;
	}

	if (Find_Component(strComTag))
	{
		MSG_BOX("ComTag Already Exist!");
		return E_FAIL;
	}

	CComponent* pComponent = m_pGameInstance->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
	if (!pComponent)
	{
		return E_FAIL;
	}

	*ppComponent = pComponent;

	m_Components.emplace(strComTag, pComponent);

	Safe_AddRef(pComponent);

	return S_OK;
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
	{
		Safe_Release(Pair.second);
	}
	m_Components.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
