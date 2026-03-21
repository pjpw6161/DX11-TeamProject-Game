#include "Level_Logo.h"
#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_LOGO);
	m_pGameInstance->StopAll();
	m_pGameInstance->PlayBGM(TEXT("BGM_Credit_01"));

	if (FAILED(Ready_Logo()))
	{
		MSG_BOX("Failed to Ready Logo");
		return E_FAIL;
	}
	return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
	//if (not m_hasBGMStarted and m_pGameInstance->Is_SoundManager_Ready())
	//{
	//	m_pGameInstance->PlayBGM(TEXT("Buzz"), 0.2f);
	//	m_hasBGMStarted = true;
	//}

	if (m_pGameInstance->Key_Down(DIK_RETURN))
	{
		
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_SELECT))))
		{
			return;
		}

		return;
	}

	m_pGameInstance->Set_HellHeight(-2000.f);

	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
	{
		DestroyWindow(g_hWnd);
	}
}

HRESULT CLevel_Logo::Render()
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Logo()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Background"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Logo_Odin"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Logo_Dust"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Logo"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Logo_Stone"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Logo_Text"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(_dev pDevice, _context pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to create : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
