#include "Level_AnimTool.h"
#include "Camera.h"
#include "Player.h"

CLevel_AnimTool::CLevel_AnimTool(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_AnimTool::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_TOOL);
	m_pGameInstance->PlayBGM(TEXT("Prologue_BGM_Loop"), 0.1f);
	m_pGameInstance->Play_Sound(TEXT("AMB_Voidness_Rain_Area_SFX_01"), 0.3f, true);

	if (FAILED(Ready_Camera()))
	{
		MSG_BOX("Failed to Ready Camera");
		return E_FAIL;
	}

	if (FAILED(Ready_Light()))
	{
		MSG_BOX("Failed to Ready Light");
		return E_FAIL;
	}

	return S_OK;
}

void CLevel_AnimTool::Tick(_float fTimeDelta)
{
	m_pGameInstance->StopSound(0);
	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
	{
		DestroyWindow(g_hWnd);
	}
}

HRESULT CLevel_AnimTool::Render()
{
	return S_OK;
}

HRESULT CLevel_AnimTool::Ready_Camera()
{
	if (not m_pGameInstance)
	{
		return E_FAIL;
	}

	wstring strLayerTag = TEXT("Layer_Camera");

	CCamera::Camera_Desc CamDesc;
	CamDesc.vCameraPos = _float4(0.f, 5.f, -5.f, 1.f);
	CamDesc.vFocusPos = _float4(0.f, 0.f, 0.f, 1.f);
	CamDesc.fFovY = XMConvertToRadians(60.f);
	CamDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CamDesc.fNear = 0.1f;
	CamDesc.fFar = 1100.f;

	/*if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_Main"), &CamDesc)))
	{
		return E_FAIL;
	}*/

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_Debug"), &CamDesc)))
	{
		return E_FAIL;
	}

	//m_pGameInstance->Set_CameraModeIndex(CM_MAIN);

	return S_OK;
}

HRESULT CLevel_AnimTool::Ready_Light()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(-1.f, -2.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);

	return m_pGameInstance->Add_Light(LEVEL_TOOL, TEXT("Light_Main"), LightDesc);
}

CLevel_AnimTool* CLevel_AnimTool::Create(_dev pDevice, _context pContext)
{
	CLevel_AnimTool* pInstance = new CLevel_AnimTool(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to create : CLevel_AnimTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_AnimTool::Free()
{
	__super::Free();
}
