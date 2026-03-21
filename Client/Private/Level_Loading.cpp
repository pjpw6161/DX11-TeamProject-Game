#include "Level_Loading.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_Custom.h"
#include "Level_Select.h"
#include "Level_GamePlay.h"
#include "Level_Village.h"
#include "Level_Dungeon.h"
#include "Loading.h"
#include "Loading_Horse.h"

CLevel_Loading::CLevel_Loading(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Init(LEVEL_ID eNextLevel)
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_LOADING);

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOADING, TEXT("Layer_Loading"), TEXT("Prototype_GameObject_Loading"))))
	{
		return E_FAIL;
	}


	m_eNextLevel = eNextLevel;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel);
	if (not m_pLoader)
	{
		return E_FAIL;
	}

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	m_pLoader->Show_LoadingText();

	if (m_pGameInstance->Is_SoundManager_Ready())
	{
		for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
		{
			if (m_pGameInstance->Get_IsPlayingSound(i))
			{
				m_pGameInstance->FadeoutSound(i, fTimeDelta, 1.f, false);
			}
		}
	}

	if (m_pLoader->isFinished())// && m_pGameInstance->Key_Down(DIK_SPACE))
	{
		CLevel* pLevel = nullptr;

		switch (m_eNextLevel)
		{
		case Client::LEVEL_LOGO:
			pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case Client::LEVEL_SELECT:
			pLevel = CLevel_Select::Create(m_pDevice, m_pContext);
			break;
		case Client::LEVEL_CUSTOM:
			pLevel = CLevel_Custom::Create(m_pDevice, m_pContext);
			break;
		case Client::LEVEL_GAMEPLAY:
			pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
			break;
		//case Client::LEVEL_CREATECHARACTER:
		//	pLevel = CLevel_CreateCharacter::Create(m_pDevice, m_pContext);
		//	break;
		//case Client::LEVEL_FOREST:
		//	pLevel = CLevel_Tutorial::Create(m_pDevice, m_pContext);
		//	break;
		case Client::LEVEL_VILLAGE:
			pLevel = CLevel_Village::Create(m_pDevice, m_pContext);
			break;
		//case Client::LEVEL_DUNGEON:
		//	pLevel = CLevel_Dungeon::Create(m_pDevice, m_pContext);
		//	break;
		}

		for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
		{
			if (m_pGameInstance->Get_IsPlayingSound(i))
			{
				m_pGameInstance->FadeinSound(i, fTimeDelta, 0.5f);
			}
		}
		
		if (not pLevel)
		{
			return;
		}

		if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, pLevel)))
		{
			MSG_BOX("Failed to Open Level");
		}
		else
		{
			return;
		}
	}
}

HRESULT CLevel_Loading::Render()
{
	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	if (not m_pGameInstance)
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOADING, strLayerTag, TEXT("Prototype_GameObject_Loading_Screen"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_LoadingBar(const wstring& strLayerTag)
{
	if (not m_pGameInstance)
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOADING, strLayerTag, TEXT("Prototype_GameObject_BackGround"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_Icon(const wstring& strLayerTag)
{
	if (not m_pGameInstance)
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOADING, strLayerTag, TEXT("Prototype_GameObject_Loading_Icon"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(_dev pDevice, _context pContext, LEVEL_ID eNextLevel)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Init(eNextLevel)))
	{
		MSG_BOX("Failed to Create : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
