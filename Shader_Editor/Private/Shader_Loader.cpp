#include "pch.h"
#include "Shader_Loader.h"

#include "GameInstance.h"

#include "Shader_Camera.h"
#include "Shader_Player.h"
#include "Shader_Terrain.h"
#include "Select_Map.h"

CShader_Loader::CShader_Loader(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

_uint APIENTRY ThreadEntry(void* pArg)
{
	if (FAILED(CoInitializeEx(nullptr, 0)))
		return E_FAIL;

	CShader_Loader* pLoader = static_cast<CShader_Loader*>(pArg);

	if (FAILED(pLoader->Begin_Thread()))
	{
		CoUninitialize();
		return FALSE;
	}

	if (FAILED(pLoader->Loading_LevelResources()))
	{
		CoUninitialize();
		return FALSE;
	}

	if (FAILED(pLoader->End_Thread()))
	{
		CoUninitialize();
		return FALSE;
	}

	CoUninitialize();
	return TRUE;
}

HRESULT CShader_Loader::Init(LEVEL_ID eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, ThreadEntry, this, 0, nullptr));

	if (not m_hThread)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShader_Loader::Begin_Thread()
{
	EnterCriticalSection(&m_Critical_Section);

	return S_OK;
}

HRESULT CShader_Loader::End_Thread()
{
	LeaveCriticalSection(&m_Critical_Section);

	return S_OK;
}

HRESULT CShader_Loader::Show_LoadingText()
{
	SetWindowText(g_hWnd, m_strLoadingText.c_str());

	return S_OK;
}

_bool CShader_Loader::isFinished()
{
	return m_isFinished;
}

HRESULT CShader_Loader::Loading_LevelResources()
{
	HRESULT hr = 0;

	switch (m_eNextLevel)
	{
	case ShaderEdit::LEVEL_ShaderTest:
		hr = Load_Test();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CShader_Loader::Load_Test()
{
	m_strLoadingText = L"Logo : Loading Sounds";
	if (FAILED(m_pGameInstance->Init_SoundManager()))
	{
		return E_FAIL;
	}

#pragma region TerrainTexture

	string strInputFilePath = "../../Client/Bin/Resources/Textures/Terrain/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Component_Texture_Terrain");

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CTexture::Create(m_pDevice, m_pContext, entry.path().wstring()))))
			{
				return E_FAIL;
			}
		}
	}


#pragma endregion

#pragma region TerrainBuffer

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region VTFShader

	// VTF Test
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_ShaderTest, TEXT("Prototype_Component_Shader_VTF"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VTFModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_ShaderTest, TEXT("Prototype_Component_Shader_RTVTF"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_RT_VTFModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

#pragma endregion

#pragma region TerrainObject

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Terrain"),
		CShader_Terrain::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}


#pragma endregion

#pragma region SelectMap(캐릭터 선택 선박 안)Model

	_mat PivotMat = _mat::CreateScale(0.006f) * _mat::CreateRotationX(XMConvertToRadians(90.f));

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_ShaderTest, TEXT("Prototype_Model_Select_Map"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/StaticMesh/Select_Map/Mesh/map.hyuntrastatmesh", false, PivotMat))))
		return E_FAIL;

#pragma endregion


#pragma region PlayerModel_And_Object

	CRealtimeVTFModel* pModel = CRealtimeVTFModel::Create(m_pDevice, m_pContext,
		"../../Client/Bin/Resources/AnimMesh/VTFPlayer/Main/basemodel.hyuntraanimmesh");


	strInputFilePath = "../../Client/Bin/Resources/AnimMesh/VTFPlayer/Part/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (FAILED(pModel->Seting_Parts(entry.path().string())))
				return E_FAIL;
		}
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Player"), pModel)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Player"), CShader_Player::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region SelectMapObject

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Select_Map"),
		CSelect_Map::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

#pragma endregion



	m_isFinished = true;

	return S_OK;
}

CShader_Loader* CShader_Loader::Create(_dev pDevice, _context pContext, LEVEL_ID eNextLevel)
{
	CShader_Loader* pInstance = new CShader_Loader(pDevice, pContext);

	if (FAILED(pInstance->Init(eNextLevel)))
	{
		MSG_BOX("Failed to Create : CShader_Loader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader_Loader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);

	CloseHandle(m_hThread);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
