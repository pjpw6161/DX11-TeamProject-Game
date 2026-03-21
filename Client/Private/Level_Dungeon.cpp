#include "Level_Dungeon.h"
#include "Camera.h"
#include "Monster.h"
#include "NPC_Dummy.h"
#include "Map.h"
#include "Player.h"


CLevel_Dungeon::CLevel_Dungeon(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Dungeon::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_VILLAGE);
	m_pGameInstance->StopAll();
	m_pGameInstance->PlayBGM(TEXT("BGM_1st_Village"));

	if (FAILED(Ready_Player()))
	{
		MSG_BOX("Failed to Ready Player");
		return E_FAIL;
	}


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


	// Map
	if (FAILED(Ready_Map()))
	{
		MSG_BOX("Failed to Ready Map");
		return E_FAIL;
	}


	//if (FAILED(Ready_UI()))
	//{
	//	MSG_BOX("Failed to Ready UI");
	//	return E_FAIL;
	//}


	//if (FAILED(Ready_Object()))
	//{
	//	MSG_BOX("Failed to Ready Object");
	//	return E_FAIL;
	//}
	//if (FAILED(Ready_Environment()))
	//{
	//	MSG_BOX("Failed to Ready Object");
	//	return E_FAIL;
	//}

	m_pGameInstance->Set_HellHeight(-5000.f);

	m_pGameInstance->Set_FogNF(_vec2(50.f, 2000.f));
	m_pGameInstance->Set_FogColor(_color(1.f));

	return S_OK;
}

void CLevel_Dungeon::Tick(_float fTimeDelta)
{

	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
	{
		DestroyWindow(g_hWnd);
	}
}

HRESULT CLevel_Dungeon::Render()
{
	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Camera()
{
	if (not m_pGameInstance)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Light()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(-1.f, -2.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);

	return m_pGameInstance->Add_Light(LEVEL_VILLAGE, TEXT("Light_Main"), LightDesc);
}

HRESULT CLevel_Dungeon::Ready_Player()
{
	// 플레이어 위치 설정
	const TCHAR* pGetPath = TEXT("../Bin/Data/Village_Player_Pos.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MessageBox(g_hWnd, L"../Bin/Data/Player_Pos.dat 파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
		return E_FAIL;
	}

	_vec4 Player_Pos{ 0.f };
	inFile.read(reinterpret_cast<char*>(&Player_Pos), sizeof(_vec4));

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	pPlayerTransform->Set_Position(_vec3(Player_Pos) + _vec3(0.f, 2.f, 0.f));


	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Map()
{
	const TCHAR* pGetPath = TEXT("../Bin/Data/Village_MapData.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MessageBox(g_hWnd, L"맵 파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
		return E_FAIL;
	}

	_uint MapListSize;
	inFile.read(reinterpret_cast<char*>(&MapListSize), sizeof(_uint));


	for (_uint i = 0; i < MapListSize; ++i)
	{
		_ulong MapPrototypeSize;
		inFile.read(reinterpret_cast<char*>(&MapPrototypeSize), sizeof(_ulong));

		wstring MapPrototype;
		MapPrototype.resize(MapPrototypeSize);
		inFile.read(reinterpret_cast<char*>(&MapPrototype[0]), MapPrototypeSize * sizeof(wchar_t));

		_mat MapWorldMat;
		inFile.read(reinterpret_cast<char*>(&MapWorldMat), sizeof(_mat));

		MapInfo MapInfo{};
		MapInfo.Prototype = MapPrototype;
		MapInfo.m_Matrix = MapWorldMat;

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Map"), TEXT("Prototype_GameObject_Village_Map"), &MapInfo)))
		{
			MessageBox(g_hWnd, L"맵 불러오기 실패", L"파일 로드", MB_OK);
			return E_FAIL;
		}
	}
	return S_OK;
}


HRESULT CLevel_Dungeon::Ready_Object()
{

	const TCHAR* pGetPath = TEXT("../Bin/Data/Village_ObjectData.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("오브젝트 파일을 찾지 못했습니다.");
		return E_FAIL;
	}

	_uint ObjectListSize;
	inFile.read(reinterpret_cast<char*>(&ObjectListSize), sizeof(_uint));


	for (_uint i = 0; i < ObjectListSize; ++i)
	{
		_ulong ObjectPrototypeSize;
		inFile.read(reinterpret_cast<char*>(&ObjectPrototypeSize), sizeof(_ulong));

		wstring ObjectPrototype;
		ObjectPrototype.resize(ObjectPrototypeSize);
		inFile.read(reinterpret_cast<char*>(&ObjectPrototype[0]), ObjectPrototypeSize * sizeof(wchar_t));

		_mat ObjectWorldMat;
		inFile.read(reinterpret_cast<char*>(&ObjectWorldMat), sizeof(_mat));

		ObjectInfo ObjectInfo{};
		ObjectInfo.strPrototypeTag = ObjectPrototype;
		ObjectInfo.m_WorldMatrix = ObjectWorldMat;
		ObjectInfo.eObjectType = Object_Building;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Village_Object"), TEXT("Prototype_GameObject_Village_Etc_Object"), &ObjectInfo)))
		{
			MSG_BOX("오브젝트 불러오기 실패");
			return E_FAIL;
		}
	}
	return S_OK;
}


HRESULT CLevel_Dungeon::Ready_Environment()
{

	const TCHAR* pGetPath = TEXT("../Bin/Data/Village_EnvirData.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("환경오브젝트 파일을 찾지 못했습니다.");
		return E_FAIL;
	}

	_uint ObjectListSize;
	inFile.read(reinterpret_cast<char*>(&ObjectListSize), sizeof(_uint));


	for (_uint i = 0; i < ObjectListSize; ++i)
	{
		_ulong ObjectPrototypeSize;
		inFile.read(reinterpret_cast<char*>(&ObjectPrototypeSize), sizeof(_ulong));

		wstring ObjectPrototype;
		ObjectPrototype.resize(ObjectPrototypeSize);
		inFile.read(reinterpret_cast<char*>(&ObjectPrototype[0]), ObjectPrototypeSize * sizeof(wchar_t));

		_mat ObjectWorldMat;
		inFile.read(reinterpret_cast<char*>(&ObjectWorldMat), sizeof(_mat));

		ObjectInfo ObjectInfo{};
		ObjectInfo.strPrototypeTag = ObjectPrototype ;
		ObjectInfo.m_WorldMatrix = ObjectWorldMat;
		ObjectInfo.eObjectType = Object_Environment;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Envir_Object"), TEXT("Prototype_GameObject_Village_Envir_Object"), &ObjectInfo)))
		{
			MSG_BOX("환경오브젝트 불러오기 실패");
			return E_FAIL;
		}
	}
	return S_OK;
}


HRESULT CLevel_Dungeon::Ready_NpcvsMon()
{
	MonsterInfo Info{};
	const TCHAR* pGetPath = L"../Bin/Data/Prologue_MonsterData.dat";

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MessageBox(g_hWnd, L"../Bin/Data/Prologue_MonsterData.dat 파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
		return E_FAIL;
	}

	_uint MonsterListSize;
	inFile.read(reinterpret_cast<char*>(&MonsterListSize), sizeof(_uint));


	for (_uint i = 0; i < MonsterListSize; ++i)
	{
		_ulong MonsterPrototypeSize;
		inFile.read(reinterpret_cast<char*>(&MonsterPrototypeSize), sizeof(_ulong));

		wstring MonsterPrototype;
		MonsterPrototype.resize(MonsterPrototypeSize);
		inFile.read(reinterpret_cast<char*>(&MonsterPrototype[0]), MonsterPrototypeSize * sizeof(wchar_t));

		_mat MonsterWorldMat;
		inFile.read(reinterpret_cast<char*>(&MonsterWorldMat), sizeof(_mat));

		Info.strMonsterPrototype = MonsterPrototype;
		Info.MonsterWorldMat = MonsterWorldMat;

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Monster"), MonsterPrototype, &Info)))
		{
			MessageBox(g_hWnd, L"파일 로드 실패", L"파일 로드", MB_OK);
			return E_FAIL;
		}

	}
	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_UI()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Player_HP"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Player_MP"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Player_Job"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_SkillBlock"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_ItemBlock"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_ExpBar"))))
	{
		return E_FAIL;
	}
	/*'
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Pop_QuestIn"))))
	{
		return E_FAIL;
	}
	*/

	return S_OK;
}

CLevel_Dungeon* CLevel_Dungeon::Create(_dev pDevice, _context pContext)
{
	CLevel_Dungeon* pInstance = new CLevel_Dungeon(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to create : CLevel_Dungeon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Dungeon::Free()
{
	__super::Free();
}
