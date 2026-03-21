#include "Loader.h"
#include "GameInstance.h"

#include "LoaderIncludes.h"

CLoader::CLoader(_dev pDevice, _context pContext)
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

	CLoader* pLoader = static_cast<CLoader*>(pArg);

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

HRESULT CLoader::Init(LEVEL_ID eNextLevel)
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

HRESULT CLoader::Begin_Thread()
{
	EnterCriticalSection(&m_Critical_Section);

	return S_OK;
}

HRESULT CLoader::End_Thread()
{
	LeaveCriticalSection(&m_Critical_Section);

	return S_OK;
}

HRESULT CLoader::Show_LoadingText()
{
	SetWindowText(g_hWnd, m_strLoadingText.c_str());

	return S_OK;
}

_bool CLoader::isFinished()
{
	return m_isFinished;
}

HRESULT CLoader::Loading_LevelResources()
{
	HRESULT hr = 0;

	switch (m_eNextLevel)
	{

	case MapEditor::LEVEL_EDITOR:
		hr = Load_Editor();
		break;

	}

	if (FAILED(hr))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Load_Editor()
{


	m_strLoadingText = L"Editor : Loading Texture";
#pragma region Texture

	// Terrain Textures
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

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Brush"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Brush/Brush.png")))))
	{
		return E_FAIL;
	}
#pragma endregion

#pragma endregion

	m_strLoadingText = L"Editor : Loading Object Model";
#pragma region Model
	//_matrix Pivot = XMMatrixRotationAxis(XMVectorSet(-1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));


	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Collider"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Model/Sphere/Mesh/SM_EFF_Sphere_01.mo.hyuntrastatmesh"))))
	{
		return E_FAIL;
	}

	_mat Pivot = _mat::CreateScale(0.003f);

	// Map Model
	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Map/Midgard/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), true, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	Pivot = _mat::CreateScale(0.001f);
	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Map/Dungeon/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();
			if (strPrototypeTag == L"Prototype_Model_BrickMap")
			{
				Pivot = _mat::CreateScale(0.1f);
			}
			else if (strPrototypeTag == L"Prototype_Model_DragonMap")
			{
				Pivot = _mat::CreateScale(0.5f);
			}
			else if (strPrototypeTag == L"Prototype_Model_BossRoom")
			{
				Pivot = _mat::CreateScale(0.005f);
			}
			else if (strPrototypeTag == L"Prototype_Model_Survival_Map")
			{
				Pivot = _mat::CreateScale(1.f);
			}
			else if (strPrototypeTag == L"Prototype_Model_SescoMap")
			{
				Pivot = _mat::CreateScale(0.005f);
			}
			else
				Pivot = _mat::CreateScale(0.001f);

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), true, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	 Pivot = _mat::CreateScale(0.01f);

	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Map/Tutorial/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), true, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	Pivot = _mat::CreateScale(0.003f);
	 //Object Model
	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Object/Tutorial/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), true, Pivot))))
			{
				return E_FAIL;
			}
		}
	}

	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Object/Midgard/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Object/Dungeon/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), true, Pivot))))
			{
				return E_FAIL;
			}
		}
	}

	Pivot = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Environment/Tree/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), true, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	//Pivot = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	Pivot = XMMatrixScaling(0.005f, 0.005f, 0.005f);

	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Environment/Grass/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			//if (strPrototypeTag == L"Prototype_Model_MidGrass1" || strPrototypeTag == L"Prototype_Model_PlaneGrass")
			//{
			//	Pivot = XMMatrixScaling(0.005f, 0.005f, 0.005f);
			//}else
			//	Pivot = XMMatrixScaling(0.01f, 0.01f, 0.01f);

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	Pivot = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Environment/Rock/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), true, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Environment/Interaction/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), true, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/Model/Interaction/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}

#pragma endregion

	m_strLoadingText = L"Editor : Loading Monster Model";
//
//#pragma region Monster
//	strInputFilePath = "../../Client/Bin/Resources/AnimMesh/Monster/0_ModelTest/Mesh/";
//	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
//	{
//		if (entry.is_regular_file())
//		{
//			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_ModelTest"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
//			{
//				return E_FAIL;
//			}
//
//		}
//	}
//
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Rabbit"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Rabbit/Hit/Mesh/Rabbit.hyuntraanimmesh"))))
//	{
//		return E_FAIL;
//	}
//
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Furgoat"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Furgoat/Hit/Mesh/Furgoat.hyuntraanimmesh"))))
//	{
//		return E_FAIL;
//	}
//
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Nastron03"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Nastron03/Mesh/Nastron03.hyuntraanimmesh"))))
//	{
//		return E_FAIL;
//	}
//
//	Pivot = _mat::CreateScale(0.01f) * _mat::CreateRotationX(XMConvertToRadians(-90.f));
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_NPCvsMon"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/NPCvsMon/Mesh/NPCvsMon.hyuntraanimmesh", false, Pivot))))
//	{
//		return E_FAIL;
//	}
//
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Thief04"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Thief04/Mesh/Thief04.hyuntraanimmesh"))))
//	{
//		return E_FAIL;
//	}
//
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_TrilobiteA"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/TrilobiteA/Mesh/TrilobiteA.hyuntraanimmesh"))))
//	{
//		return E_FAIL;
//	}
//
//	Pivot = _mat::CreateScale(1.5f);
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Void01"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Void01/Hit/Mesh/Void01.hyuntraanimmesh", false, Pivot))))
//	{
//		return E_FAIL;
//	}
//
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Void05"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Void05/Mesh/Void05.hyuntraanimmesh"))))
//	{
//		return E_FAIL;
//	}
//
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Imp"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Imp/Mesh/Imp.hyuntraanimmesh"))))
//	{
//		return E_FAIL;
//	}
//
//	_mat Pivot1 = _mat::CreateScale(0.5f);
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Void09"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Void09/Mesh/Void09.hyuntraanimmesh", false, Pivot1))))
//	{
//		return E_FAIL;
//	}
//
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Void20"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Void20/Mesh/Void20.hyuntraanimmesh", false, Pivot1))))
//	{
//		return E_FAIL;
//	}
//
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Void23"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Void23/Mesh/Void23.hyuntraanimmesh", false, Pivot1))))
//	{
//		return E_FAIL;
//	}
//
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Nastron07"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Nastron07/Mesh/Nastron07.hyuntraanimmesh", false, Pivot1))))
//	{
//		return E_FAIL;
//	}
//
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Spider"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Spider/Mesh/Spider.hyuntraanimmesh"))))
//	{
//		return E_FAIL;
//	}
//
//	// Dead
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Void01_Die"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Dead/Void01/Mesh/Void01_Die.hyuntraanimmesh"))))
//	{
//		return E_FAIL;
//	}
//
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Void05_Die"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Dead/Void05/Mesh/Void05_Die.hyuntraanimmesh"))))
//	{
//		return E_FAIL;
//	}
//
//	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Void09_Die"),
//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Dead/Void09/Mesh/Void09_Die.hyuntraanimmesh", false, Pivot1))))
//	{
//		return E_FAIL;
//	}
//
//
//#pragma endregion Monster

#pragma region NPC

	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Cat"),
	//	CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/NPC/Cat/Mesh/Cat.hyuntraanimmesh"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Dog"),
	//	CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/NPC/Dog/Mesh/Dog.hyuntraanimmesh"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_ItemMerchant"),
	//	CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/NPC/ItemMerchant/Mesh/ItemMerchant.hyuntraanimmesh"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_BlackSmith"),
	//	CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/NPC/BlackSmith/Mesh/BlackSmith.hyuntraanimmesh"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Roskva"),
	//	CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/NPC/Roskva/Mesh/Roskva.hyuntraanimmesh"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Aur"),
	//	CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/NPC/Aur/Mesh/Aur.hyuntraanimmesh"))))
	//{
	//	return E_FAIL;
	//}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Guard"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Model/NPC/Mesh/Guard.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_StoneTower"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Model/NPC/Mesh/StoneTower.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}
	strInputFilePath = "../../Client/Bin/Resources/AnimMesh/NPC/NPC_Dummy/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;

			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}

#pragma endregion NPC

#pragma  region Boss

	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Groar"),
	//	CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/NPC/Groar/Mesh/Groar.hyuntraanimmesh"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_GroarScene01"),
	//	CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Boss/GroarScene01/Mesh/GroarScene01.hyuntraanimmesh"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_GroarScene02"),
	//	CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Boss/GroarScene02/Mesh/GroarScene02.hyuntraanimmesh"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Groar_Boss"),
	//	CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Boss/Groar_Boss/Mesh/Groar_Boss.hyuntraanimmesh"))))
	//{
	//	return E_FAIL;
	//}

	//_mat TentaclePivot = _mat::CreateRotationX(XMConvertToRadians(290.f));
	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Groar_Tentacle"),
	//	CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Boss/Groar_Tentacle/Mesh/Groar_Tentacle.hyuntraanimmesh", false, TentaclePivot))))
	//{
	//	return E_FAIL;
	//}

#pragma  endregion Boss


#pragma region ETC
	// Pivot = _mat::CreateScale(0.01f) * _mat::CreateRotationY(90.f);

	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Camera"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Model/Etc/Mesh/Camera.hyuntrastatmesh", false, Pivot))))
	//{
	//	return E_FAIL;
	//}
#pragma endregion


	m_strLoadingText = L"Editor : Loading Prototype";
#pragma region Prototype

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera_Debug"), CCamera_Debug::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera_Main"), CCamera_Main::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera_CutScene"), CCamera_CutScene::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Dummy"), CDummy::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Map"), CMap::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera_Point"), CCutScene_Point::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera_Curve"), CCutScene_Curve::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
#pragma endregion

	m_strLoadingText = L"Editor : Loading Complete!";
	m_isFinished = true;


	return S_OK;
}


CLoader* CLoader::Create(_dev pDevice, _context pContext, LEVEL_ID eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Init(eNextLevel)))
	{
		MSG_BOX("Failed to Create : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);

	CloseHandle(m_hThread);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
