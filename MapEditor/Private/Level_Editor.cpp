#include "Level_Editor.h"
#include "Camera.h"
#include "Camera_Manager.h"

CLevel_Editor::CLevel_Editor(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Editor::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_EDITOR);

	m_pGameInstance->Set_HellHeight(-2000.f);

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

void CLevel_Editor::Tick(_float fTimeDelta)
{

}

HRESULT CLevel_Editor::Render()
{
	return S_OK;
}

HRESULT CLevel_Editor::Ready_Camera()
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
	CamDesc.fFar = 3000.f;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_Main"), &CamDesc)))
	{
		return E_FAIL;
	}

	CCamera_Manager::Get_Instance()->Set_CameraModeIndex(CM_MAIN);

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_Debug"), &CamDesc)))
	//{
	//	return E_FAIL;
	//}

	CCamera_Manager::Get_Instance()->Set_CameraModeIndex(CM_MAIN);

	return S_OK;
}

HRESULT CLevel_Editor::Ready_Light()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(-1.f, -2.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);

	return m_pGameInstance->Add_Light(LEVEL_EDITOR, TEXT("Light_Main"), LightDesc);
}



CLevel_Editor* CLevel_Editor::Create(_dev pDevice, _context pContext)
{
	CLevel_Editor* pInstance = new CLevel_Editor(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to create : CLevel_Editor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Editor::Free()
{
	__super::Free();
}
