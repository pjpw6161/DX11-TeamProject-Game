#include "Level_GamePlay.h"
#include "Level_Loading.h"
#include "Camera.h"
#include "Monster.h"
#include "NPC.h"
#include "Map.h"
#include "Player.h"
#include "Effect_Manager.h"
#include "Trigger_Manager.h"
#include "UI_Manager.h"
#include "Pop_Skill.h"
#include "Pop_LevelUp.h"
#include "Camera_Manager.h"
#include "BrickWall.h"

//원명의 꼽사리
#include "Lake.h"

CLevel_GamePlay::CLevel_GamePlay(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_GAMEPLAY);
	m_pGameInstance->StopAll();

	if (FAILED(CUI_Manager::Get_Instance()->Init()))
	{
		return E_FAIL;
	}


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

	// Environment
	if (FAILED(Ready_Environment()))
	{
		MSG_BOX("Failed to Ready Environment");
		return E_FAIL;
	}

	// Object
	if (FAILED(Ready_Object()))
	{
		MSG_BOX("Failed to Ready Object");
		return E_FAIL;
	}

	// 몬스터 파싱
	if (FAILED(Ready_Tutorial_Monster()))
	{
		MSG_BOX("Failed to Ready NPCvsMon");
		return E_FAIL;
	}

	// Monster_Test
	//if (FAILED(Ready_ModelTest()))
	//{
	//	MSG_BOX("Failed to Ready ModelTest");
	//	return E_FAIL;
	//}

	if (FAILED(Ready_Monster_Test()))
	{
		MSG_BOX("Failed to Ready Monster_Test");
		return E_FAIL;
	}

	// NPC_Test
	if (FAILED(Ready_NPC_Test()))
	{
		MSG_BOX("Failed to Ready NPC");
		return E_FAIL;
	}

	if (FAILED(Ready_NPC_Dummy_Test()))
	{
		MSG_BOX("Failed to Ready NPC_Dummy");
		return E_FAIL;
	}

	// Pet_Test
	if (FAILED(Ready_Pet()))
	{
		MSG_BOX("Failed to Ready Pet");
		return E_FAIL;
	}

	// UI
	if (FAILED(Ready_UI()))
	{
		MSG_BOX("Failed to Ready UI");
		return E_FAIL;
	}
	//if (FAILED(Ready_TestTrigger()))
	//{
	//	MSG_BOX("Failed to Ready TestTrigger");
	//	return E_FAIL;
	//}

	EffectInfo EffectDesc = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Rain");
	EffectDesc.pMatrix = &m_RainMatrix;
	EffectDesc.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(EffectDesc);
	
	m_pGameInstance->Set_FogNF(_vec2(5.f, 300.f));
	m_pGameInstance->Set_FogColor(_color(0.1f));
	CUI_Manager::Get_Instance()->Set_Coin(100000);


	CFadeBox::FADE_DESC Desc = {};
	Desc.fOut_Duration = 3.f;
	CUI_Manager::Get_Instance()->Add_FadeBox(Desc);

	m_pGameInstance->Play_Video(TEXT("Tutorial0.wmv"));
	m_pGameInstance->Set_StopKey(DIK_RETURN);

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_B))
	{

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_BrickBall"), TEXT("Prototype_GameObject_BrickBall"))))
		{
			return;
		}

	}
	if (!m_bReadyTutorial)
	{
		m_pGameInstance->PlayBGM(TEXT("Prologue_BGM_Loop"), 0.2f);
		m_pGameInstance->Play_Sound(TEXT("AMB_Voidness_Rain_Area_SFX_01"), 0.6f, true);
		m_pGameInstance->Play_Sound(TEXT("waves"), 0.2f, true);
		for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
		{
			if (m_pGameInstance->Get_IsLoopingSound(i))
			{
				m_pGameInstance->FadeinSound(i, fTimeDelta);
			}
		}
		m_bReadyTutorial = true;
	}

	if (!CUI_Manager::Get_Instance()->Is_InvenActive())
	{
		m_RainMatrix = _mat::CreateTranslation(_vec3(m_pGameInstance->Get_CameraPos()));
		//m_RainMatrix = _mat::CreateTranslation(_vec3(50.f, 3.f, 50.f));
	}

	if (CCamera_Manager::Get_Instance()->Get_CameraState() == CS_SKILLBOOK)
	{
		m_RainMatrix = _mat();
	}

	if (m_fWaveTimer > 5.f)
	{
		/*	int random = rand() % 3;
			switch (random)
			{
			case 0:m_pGameInstance->Play_Sound(TEXT("waves0"), 0.8f, false);
				break;
			case 1:m_pGameInstance->Play_Sound(TEXT("waves1"), 0.8f, false);
				break;
			case 2:m_pGameInstance->Play_Sound(TEXT("waves2"), 0.8f, false);
				break;
			default:
				break;
			}*/


		EffectInfo EffectDesc = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Wave_Init");
		m_WaveMatrix[0] = _mat::CreateTranslation(_vec3(95.f, 4.f, 127.5f));
		EffectDesc.pMatrix = &m_WaveMatrix[0];
		EffectDesc.isFollow = true;
		CEffect_Manager::Get_Instance()->Add_Layer_Effect(EffectDesc);

		EffectDesc = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Wave_End");
		m_WaveMatrix[1] = _mat::CreateTranslation(_vec3(93.f, 4.f, 129.5f));
		EffectDesc.pMatrix = &m_WaveMatrix[1];
		EffectDesc.isFollow = true;
		CEffect_Manager::Get_Instance()->Add_Layer_Effect(EffectDesc);

		EffectDesc = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Wave_Init");
		m_WaveMatrix[2] = _mat::CreateTranslation(_vec3(108.f, 4.f, 100.f));
		EffectDesc.pMatrix = &m_WaveMatrix[2];
		EffectDesc.isFollow = true;
		CEffect_Manager::Get_Instance()->Add_Layer_Effect(EffectDesc);

		EffectDesc = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Wave_End");
		m_WaveMatrix[3] = _mat::CreateTranslation(_vec3(110.f, 4.f, 98.f));
		EffectDesc.pMatrix = &m_WaveMatrix[3];
		EffectDesc.isFollow = true;
		CEffect_Manager::Get_Instance()->Add_Layer_Effect(EffectDesc);

		m_isWave = false;
		m_fWaveTimer = {};
		m_fWaveGravity = {};
	}
	if (m_fWaveTimer > 0.5f)
	{

		//if (not m_isWave)
		//{
		//	EffectInfo EffectDesc = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Wave_End");
		//	EffectDesc.pMatrix = &m_WaveMatrix;
		//	EffectDesc.isFollow = true;
		//	CEffect_Manager::Get_Instance()->Add_Layer_Effect(EffectDesc);
		//	m_fWaveGravity = {};
		//}

		m_WaveMatrix[0].Position_vec3(m_WaveMatrix[0].Position_vec3() - _vec3(0.f, m_fWaveGravity, 0.f) * fTimeDelta);
		m_WaveMatrix[1].Position_vec3(m_WaveMatrix[1].Position_vec3() - _vec3(0.f, m_fWaveGravity, 0.f) * fTimeDelta);
		m_WaveMatrix[2].Position_vec3(m_WaveMatrix[2].Position_vec3() - _vec3(0.f, m_fWaveGravity, 0.f) * fTimeDelta);
		m_WaveMatrix[3].Position_vec3(m_WaveMatrix[3].Position_vec3() - _vec3(0.f, m_fWaveGravity, 0.f) * fTimeDelta);
		m_fWaveGravity += 0.981f;

		m_isWave = true;
	}

	m_fWaveTimer += fTimeDelta;

	m_TornadoMatrix.Position_vec3(_vec3());
	m_TornadoMatrix *= _mat::CreateRotationY(fTimeDelta * -5.f) * _mat::CreateTranslation(_vec3(141.f, 0.f, 200.f));

	if (m_pGameInstance->Is_Level_ShutDown(LEVEL_GAMEPLAY) or m_pGameInstance->Key_Down(DIK_NUMPAD9) or m_pGameInstance->Key_Down(DIK_PRIOR))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_VILLAGE))))
		{
			return;
		}

		return;
	}

	if (m_pGameInstance->Key_Down(DIK_U))
	{
		//m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Missile"), TEXT("Prototype_GameObject_XBeam"));

		//CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
		//_mat EffectMat = _mat::CreateTranslation(_vec3(pPlayerTransform->Get_State(State::Pos)));
		//EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Shield");
		//Info.pMatrix = &EffectMat;
		//CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
	}
	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
	{
		DestroyWindow(g_hWnd);
	}
}

HRESULT CLevel_GamePlay::Render()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Camera()
{
	/*if (not m_pGameInstance)
	{
		return E_FAIL;
	}*/
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

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_CutScene"), &CamDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Light()
{
	LIGHT_DESC* Light = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, L"Light_Main");
	*Light = g_Light_Tutorial;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Player()
{
	// 플레이어 위치 세팅
	const TCHAR* pGetPath = TEXT("../Bin/Data/Prologue_Player_Pos.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MessageBox(g_hWnd, L"../Bin/Data/Prologue_Player_Pos.dat 파일이 없습니다..", L"불러오기 실패", MB_OK);
		return E_FAIL;
	}

	_vec4 Player_Pos{ 0.f };
	inFile.read(reinterpret_cast<char*>(&Player_Pos), sizeof(_vec4));

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	pPlayerTransform->Set_Position(_vec3(Player_Pos) + _vec3(0.f, 2.f, 0.f));

	CTransform* pCameraTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")));
	pCameraTransform->Set_Position(_vec3(Player_Pos) + _vec3(0.f, 2.f, 0.f));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Map()
{
	TERRAIN_INFO Terrain;
	Terrain.vTerrainSize = _uint2(300, 300);
	Terrain.isMesh = false;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"), &Terrain)))
	{
		return E_FAIL;
	}

	const TCHAR* pGetPath = TEXT("../Bin/Data/Prologue_MapData.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("맵 데이터 파일 불러오기 실패.");
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

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Map"), TEXT("Prototype_GameObject_Prologue_Map"), &MapInfo)))
		{
			MSG_BOX("맵 생성 실패");
			return E_FAIL;
		}
	}

	inFile.close();

	CLake::WATER_DESC Desc;
	Desc.fReflectionScale = 0.1f;
	Desc.fRefractionScale = 0.1f;
	Desc.vPos = _vec3(100.f, 1.f, 100.f);
	Desc.vSize = _vec2(200.f, 200.f);
	Desc.fWaterSpeed = 0.01f;
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, L"Layer_Map", L"Prototype_GameObject_Water", &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Environment()
{

	const TCHAR* pGetPath = TEXT("../Bin/Data/Prologue_EnvirData.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("환경 데이터 파일 불러오기 실패.");
		return E_FAIL;
	}

	_uint EnvirListSize;
	inFile.read(reinterpret_cast<char*>(&EnvirListSize), sizeof(_uint));


	for (_uint i = 0; i < EnvirListSize; ++i)
	{
		_ulong EnvirPrototypeSize;
		inFile.read(reinterpret_cast<char*>(&EnvirPrototypeSize), sizeof(_ulong));

		wstring EnvirPrototype;
		EnvirPrototype.resize(EnvirPrototypeSize);
		inFile.read(reinterpret_cast<char*>(&EnvirPrototype[0]), EnvirPrototypeSize * sizeof(wchar_t));

		_mat EnvirWorldMat;
		inFile.read(reinterpret_cast<char*>(&EnvirWorldMat), sizeof(_mat));

		ObjectInfo EnvirInfo{};
		EnvirInfo.strPrototypeTag = EnvirPrototype;
		EnvirInfo.m_WorldMatrix = EnvirWorldMat;
		EnvirInfo.eObjectType = Object_Environment;

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Prologue_Envir"), TEXT("Prototype_GameObject_Prologue_Envir"), &EnvirInfo)))
		{
			MSG_BOX("맵 생성 실패");
			return E_FAIL;
		}
	}

	EffectInfo Tornado = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Tornado");
	m_TornadoMatrix = _mat::CreateScale(2.f);
	Tornado.pMatrix = &m_TornadoMatrix;
	Tornado.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Tornado);

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Object()
{

	const TCHAR* pGetPath = TEXT("../Bin/Data/Prologue_ObjectData.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("오브젝트 불러오기 실패.");
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
		ObjectInfo.eObjectType = Object_Etc;

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Prologue_Object"), TEXT("Prototype_GameObject_Prologue_Object"), &ObjectInfo)))
		{
			MSG_BOX("오브젝트 생성 실패");
			return E_FAIL;
		}
	}
	inFile.close();
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_ModelTest()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_ModelTest"), TEXT("Prototype_GameObject_ModelTest"))))
	{
		return E_FAIL;
	}

	//for (size_t i = 0; i < 300; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_VTFTest"), TEXT("Prototype_GameObject_VTFTest"))))
	//	{
	//		return E_FAIL;
	//	}
	//	/*if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Rabbit"), TEXT("Prototype_GameObject_Rabbit"))))
	//	{
	//		MSG_BOX("Rabbit 생성 실패");
	//		return E_FAIL;
	//	}*/
	//}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Monster_Test()
{
	//for (size_t i = 0; i < 1; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Tentacle"), TEXT("Prototype_GameObject_Tentacle"))))
	//	{
	//		return E_FAIL;
	//	}

	//}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_NPC_Test()
{
	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_ItemMerchant"), TEXT("Prototype_GameObject_ItemMerchant"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_BlackSmith"), TEXT("Prototype_GameObject_BlackSmith"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Aur"), TEXT("Prototype_GameObject_Aur"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Cat"), TEXT("Prototype_GameObject_Cat"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Dog"), TEXT("Prototype_GameObject_Dog"))))
	//{
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_NPC_Dummy_Test()
{
	//NPC_INFO Info = {};
	//Info.strNPCPrototype = TEXT("Prototype_Model_Dwarf_Male_002");

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	//{
	//	return E_FAIL;
	//}

	//Info.strNPCPrototype = TEXT("Prototype_Model_Horse");

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	//{
	//	return E_FAIL;
	//}

	//Info.strNPCPrototype = TEXT("Prototype_Model_Female_003");

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	//{
	//	return E_FAIL;
	//}

	//Info.strNPCPrototype = TEXT("Prototype_Model_Female_004");

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	//{
	//	return E_FAIL;
	//}

	//Info.strNPCPrototype = TEXT("Prototype_Model_Female_006");

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	//{
	//	return E_FAIL;
	//}

	//Info.strNPCPrototype = TEXT("Prototype_Model_Male_013");

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	//{
	//	return E_FAIL;
	//}

	//Info.strNPCPrototype = TEXT("Prototype_Model_Male_015");

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	//{
	//	return E_FAIL;
	//}

	//Info.strNPCPrototype = TEXT("Prototype_Model_Male_018");

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	//{
	//	return E_FAIL;
	//}

	//Info.strNPCPrototype = TEXT("Prototype_Model_Female_013");

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	//{
	//	return E_FAIL;
	//}

	//Info.strNPCPrototype = TEXT("Prototype_Model_Female_027");

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	//{
	//	return E_FAIL;
	//}

	//Info.strNPCPrototype = TEXT("Prototype_Model_Male_Chi");

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	//{
	//	return E_FAIL;
	//}

	//Info.strNPCPrototype = TEXT("Prototype_Model_Male_016");

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	//{
	//	return E_FAIL;
	//}

	//Info.strNPCPrototype = TEXT("Prototype_Model_Male_020");

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	//{
	//	return E_FAIL;
	//}

	//Info.strNPCPrototype = TEXT("Prototype_Model_Male_027");

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	//{
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Groar_Boss()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Groar_Boss"), TEXT("Prototype_GameObject_Groar_Boss"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Pet()
{
	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Pet_Cat"), TEXT("Prototype_GameObject_Pet_Cat"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Pet_Dragon"), TEXT("Prototype_GameObject_Pet_Dragon"))))
	//{
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Tutorial_Monster()
{
	MonsterInfo Info{};
	const TCHAR* pGetPath = L"../Bin/Data/Prologue_MonsterData1.dat";

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("../Bin/Data/Prologue_MonsterData1.dat 몬스터 불러오기 실패.");
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

		if (Info.strMonsterPrototype == TEXT("Prototype_Model_NPCvsMon"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_NPCvsMon"), &Info)))
			{
				MSG_BOX("몬스터 생성 실패");
				return E_FAIL;
			}

		}
		else if (Info.strMonsterPrototype == TEXT("Prototype_Model_Void05"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Void05"), &Info)))
			{
				MSG_BOX("몬스터 생성 실패");
				return E_FAIL;
			}

		}
		else if (Info.strMonsterPrototype == TEXT("Prototype_Model_Void01"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Void01"), &Info)))
			{
				MSG_BOX("몬스터 생성 실패");
				return E_FAIL;
			}

		}

	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Void01"), TEXT("Prototype_GameObject_Void01"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_UI()
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

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Inven"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Coin"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Diamond"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Radar"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_SkillBook"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Targeted"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_VehicleBook"))))
	{
		return E_FAIL;
	}
	
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_InfinityTower"))))
	{
		return E_FAIL;
	}
	CBrickWall::WALL_DESC WallDesc{};
	WallDesc.rcRect = { (_long)1.f, (_long)0.f, (_long)0.f, (_long)0.f };
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_BrickWall"), &WallDesc)))
	{
		return E_FAIL;
	}




	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_TestTrigger()
{
	TriggerInfo Info{};
	const TCHAR* pGetPath = L"../Bin/Data/test_Trigger.dat";

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("../Bin/Data/test_Trigger.dat 트리거 불러오기 실패.");
		return E_FAIL;
	}
	_uint TriggerListSize;
	inFile.read(reinterpret_cast<char*>(&TriggerListSize), sizeof(_uint));


	for (_uint i = 0; i < TriggerListSize; ++i)
	{
		TriggerInfo TriggerInfo{};

		_uint iIndex{};
		inFile.read(reinterpret_cast<char*>(&iIndex), sizeof(_uint));
		TriggerInfo.iIndex = iIndex;

		_bool bCheck{};
		inFile.read(reinterpret_cast<char*>(&bCheck), sizeof(_bool));
		TriggerInfo.bLimited = bCheck;

		_ulong TriggerPrototypeSize;
		inFile.read(reinterpret_cast<char*>(&TriggerPrototypeSize), sizeof(_ulong));

		wstring TriggerPrototype;
		TriggerPrototype.resize(TriggerPrototypeSize);
		inFile.read(reinterpret_cast<char*>(&TriggerPrototype[0]), TriggerPrototypeSize * sizeof(wchar_t));

		_float TriggerSize{};
		inFile.read(reinterpret_cast<char*>(&TriggerSize), sizeof(_float));
		TriggerInfo.fSize = TriggerSize;

		_mat TriggerWorldMat;
		inFile.read(reinterpret_cast<char*>(&TriggerWorldMat), sizeof(_mat));

		TriggerInfo.WorldMat = TriggerWorldMat;

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Trigger"), TEXT("Prototype_GameObject_Trigger"), &TriggerInfo)))
		{
			MessageBox(g_hWnd, L"파일 로드 실패", L"파일 로드", MB_OK);
			return E_FAIL;
		}
	}

	inFile.close();

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(_dev pDevice, _context pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to create : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

}
