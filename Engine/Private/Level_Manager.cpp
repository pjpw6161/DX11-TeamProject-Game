#include "Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"

CLevel_Manager::CLevel_Manager()
{
}

const _uint& CLevel_Manager::Get_CurrentLevelIndex() const
{
	return m_iCurrentLevelIndex;
}

HRESULT CLevel_Manager::Init()
{
	return S_OK;
}

HRESULT CLevel_Manager::Open_Level(_uint iLevelIndex, CLevel* pNextLevel)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (nullptr != m_pCurrentLevel)
	{
		pGameInstance->Clear(m_iCurrentLevelIndex);
	}

	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNextLevel;
	m_iCurrentLevelIndex = iLevelIndex;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_Manager::Tick(_float fTimeDelta)
{
	if (!m_pCurrentLevel)
	{
		return;
	}

	m_pCurrentLevel->Tick(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (!m_pCurrentLevel)
	{
		return E_FAIL;
	}

	return m_pCurrentLevel->Render();
}

CLevel_Manager* CLevel_Manager::Create()
{
	CLevel_Manager* pInstance = new CLevel_Manager();

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CLevel_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
