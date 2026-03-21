#include "Trigger_Manager.h"
#include "Trigger.h"
#include "Camera_CutScene.h"
#include "UI_Manager.h"
#include "Symbol.h"
#include "Camera_Manager.h"

IMPLEMENT_SINGLETON(CTrigger_Manager)

CTrigger_Manager::CTrigger_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}
HRESULT CTrigger_Manager::Init()
{
	return S_OK;
}

void CTrigger_Manager::Tick(_float fTimeDelta)
{
	CTrigger* pTrigger = nullptr;
	_bool bColl = false;
	for (auto& iter : m_pTrigger)
	{
		iter->Tick(fTimeDelta);

		//m_pGameInstance->Get_CurrentLevelIndex();
		m_isColl = iter->Get_Collision();
		//m_isPlayCutScene = false;
		if (m_isColl == true)
		{
			bColl = true;
			if (iter->Get_TriggerType() == VILLAGE_TRIGGER && iter->Get_Limited() == true)
			{
				m_isPlayCutScene = true;
				m_isInfinite = false;
				m_strFilePath = L"../Bin/Data/Village_CutScene.dat";
				CCamera_Manager::Get_Instance()->Set_CameraModeIndex(CM_CUTSCENE);
				CUI_Manager::Get_Instance()->Set_Symbol(CSymbol::VILLAGE);
				m_isInVillage = true;
				iter->Set_Limited(false);
			}
			else if (iter->Get_TriggerType() == FRONTDOOR_IN_TRIGGER)
			{
				pTrigger = iter;
				if (iter->Get_Limited() == false)
				{
					if (m_isInVillage)
					{
						CUI_Manager::Get_Instance()->Set_Symbol(CSymbol::FIELDSOUTH);
						for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
						{
							if (m_pGameInstance->Get_IsLoopingSound(i))
							{
								m_pGameInstance->FadeoutSound(i, fTimeDelta, 1.f, false);
							}
						}
					}
					iter->Set_Limited(true);
				}
				if (not m_pGameInstance->Get_IsPlayingSound(0))
				{
					m_pGameInstance->PlayBGM(TEXT("Midgard_Field"));
					m_pGameInstance->Play_Sound(TEXT("AMB_Midgard_Field_SFX_01"), 0.5f, true);
					for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
					{
						if (m_pGameInstance->Get_IsLoopingSound(i))
						{
							m_pGameInstance->FadeinSound(i, fTimeDelta);
						}
					}
					m_isInVillage = false;
				}
			}
			else if (iter->Get_TriggerType() == FRONTDOOR_OUT_TRIGGER)
			{
				pTrigger = iter;
				if (iter->Get_Limited() == false)
				{
					if (not m_isInVillage)
					{
						CUI_Manager::Get_Instance()->Set_Symbol(CSymbol::VILLAGE);
						for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
						{
							if (m_pGameInstance->Get_IsLoopingSound(i))
							{
								m_pGameInstance->FadeoutSound(i, fTimeDelta, 1.f, false);
							}
						}
					}
					iter->Set_Limited(true);
				}
				if (not m_pGameInstance->Get_IsPlayingSound(0))
				{
					m_pGameInstance->PlayBGM(TEXT("BGM_1st_Village"));
					for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
					{
						if (m_pGameInstance->Get_IsLoopingSound(i))
						{
							m_pGameInstance->FadeinSound(i, fTimeDelta);
						}
					}
					m_isInVillage = true;
				}
			}
			else if (iter->Get_TriggerType() == BACKDOOR_IN_TRIGGER)
			{
				pTrigger = iter;
				if (iter->Get_Limited() == false)
				{
					if (m_isInVillage)
					{
						CUI_Manager::Get_Instance()->Set_Symbol(CSymbol::FIELDEAST);
						for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
						{
							if (m_pGameInstance->Get_IsLoopingSound(i))
							{
								m_pGameInstance->FadeoutSound(i, fTimeDelta, 1.f, false);
							}
						}
					}
					iter->Set_Limited(true);
				}
				if (not m_pGameInstance->Get_IsPlayingSound(0))
				{
					m_pGameInstance->PlayBGM(TEXT("Midgard_Field"));
					m_pGameInstance->Play_Sound(TEXT("AMB_Midgard_Field_SFX_01"), 0.5f, true);
					for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
					{
						if (m_pGameInstance->Get_IsLoopingSound(i))
						{
							m_pGameInstance->FadeinSound(i, fTimeDelta);
						}
					}
					m_isInVillage = false;
				}
			}
			else if (iter->Get_TriggerType() == BACKDOOR_OUT_TRIGGER)
			{
				pTrigger = iter;
				if (iter->Get_Limited() == false)
				{
					if (not m_isInVillage)
					{
						CUI_Manager::Get_Instance()->Set_Symbol(CSymbol::VILLAGE);
						for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
						{
							if (m_pGameInstance->Get_IsLoopingSound(i))
							{
								m_pGameInstance->FadeoutSound(i, fTimeDelta, 1.f, false);
							}
						}
					}
					iter->Set_Limited(true);
				}
				if (not m_pGameInstance->Get_IsPlayingSound(0))
				{
					m_pGameInstance->PlayBGM(TEXT("BGM_1st_Village"));
					for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
					{
						if (m_pGameInstance->Get_IsLoopingSound(i))
						{
							m_pGameInstance->FadeinSound(i, fTimeDelta);
						}
					}
					m_isInVillage = true;
				}
			}
			else if (iter->Get_TriggerType() == BOSS_TRIGGER && iter->Get_Limited() == true)
			{
				pTrigger = iter;
				m_isCollBossTrigger = true;
				m_isPlayCutScene = true;
				m_strFilePath = L"../Bin/Data/Boss_First_CutScene.dat";
				m_isInfinite = true;
				CCamera_Manager::Get_Instance()->Set_CameraModeIndex(CM_CUTSCENE);
				iter->Set_Limited(false);
			}

		}
	}

	if (m_bStartSuicide == true)
	{
		m_isBreakLoop = true;
		m_isPlayCutScene = false;
		m_isInfinite = false;
		m_strFilePath = L"../Bin/Data/Boss_Second_CutScene.dat";
		CCamera_Manager::Get_Instance()->Set_CameraModeIndex(CM_CUTSCENE);
		m_bStartSuicide = false;
	}

	if (m_bAfterSuicide)
	{
		m_isBreakLoop = true;
		m_isPlayCutScene = false;
		m_isInfinite = false;
		m_strFilePath = L"../Bin/Data/Boss_Final_CutScene.dat";
		CCamera_Manager::Get_Instance()->Set_CameraModeIndex(CM_CUTSCENE);
		m_bAfterSuicide = false;
		m_isShaking = true;
	}


	if (bColl && pTrigger != nullptr)
	{
		for (auto& iter : m_pTrigger)
		{
			if (iter->Get_TriggerType() == BOSS_TRIGGER || iter->Get_TriggerType() == VILLAGE_TRIGGER)
			{
				continue;
			}
			if (iter != pTrigger)
			{
				iter->Set_Limited(false);
			}
		}
	}
}

void CTrigger_Manager::Limited_CutScene(_bool isLimited)
{
	m_isLimited = isLimited;
}

void CTrigger_Manager::Set_SkyTextureIndex(const _uint iIndex)
{
	m_iSkyTextureIndex = iIndex;
}

const _uint& CTrigger_Manager::Get_SkyTextureIndex() const
{
	return m_iSkyTextureIndex;
}

void CTrigger_Manager::Teleport(const TeleportSpot eSpot, _float fTimeDelta)
{
	const TCHAR* pGetPath{};

	switch (eSpot)
	{
	case Client::TS_Dungeon:
	{
		for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
		{
			if (m_pGameInstance->Get_IsLoopingSound(i))
			{
				m_pGameInstance->StopSound(i);
			}
		}
		m_pGameInstance->PlayBGM(TEXT("BGM_6th_Field_01"));
		for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
		{
			if (m_pGameInstance->Get_IsLoopingSound(i))
			{
				m_pGameInstance->FadeinSound(i, fTimeDelta, 0.5f);
			}
		}

		pGetPath = TEXT("../Bin/Data/DungeonPos.dat");

		// 임시
		CTransform* pCamTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")));
		pCamTransform->Set_State(State::Pos, _vec4(2067.11f, -12.8557f, 2086.95f, 1.f));
		pCamTransform->LookAt_Dir(_vec4(0.97706846f, -0.21286753f, 0.004882995f, 0.f));

		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
		//pPlayerTransform->Set_Position(_vec3(Player_Pos) + _vec3(0.f, 4.f, 0.f));
		pPlayerTransform->Set_Position(_vec3(2070.81f, -14.8443f, 2086.87f));
		pPlayerTransform->LookAt_Dir(_vec4(0.99763946f, 0.014162573f, 0.067186668f, 0.f));

		m_pGameInstance->Set_HellHeight(-30.f);
		LIGHT_DESC* Light = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, L"Light_Main");
		*Light = g_Light_Dungeon;
		m_iSkyTextureIndex = 10;
		m_eCurrentSpot = TS_Dungeon;
		break;
	}
	case Client::TS_Village:
	{
		m_isInVillage = true;
		for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
		{
			if (m_pGameInstance->Get_IsLoopingSound(i))
			{
				m_pGameInstance->StopSound(i);
			}
		}
		m_pGameInstance->PlayBGM(TEXT("BGM_1st_Village"));
		for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
		{
			if (m_pGameInstance->Get_IsLoopingSound(i))
			{
				m_pGameInstance->FadeinSound(i, fTimeDelta, 0.5f);
			}
		}

		pGetPath = TEXT("../Bin/Data/Village_Player_Pos.dat");

		// 임시
		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MSG_BOX("파일을 찾지 못했습니다.");
			return;
		}

		//_vec4 Player_Pos{ 0.f };
		//inFile.read(reinterpret_cast<char*>(&Player_Pos), sizeof(_vec4));

		//CTransform* pCamTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")));
		//pCamTransform->Set_State(State::Pos, _vec4(-17.9027f, 18.f, 125.f, 1.f));
		//pCamTransform->LookAt_Dir(_vec4(-0.541082f, 0.548757f, 0.637257f, 0.f));
		//CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
		//pPlayerTransform->Set_Position(_vec3(Player_Pos));
		//pPlayerTransform->LookAt_Dir(_vec4(-0.541082f, 0.f, 0.637257f, 0.f));

		_mat Player_Matrix{};
		inFile.read(reinterpret_cast<char*>(&Player_Matrix), sizeof(_mat));

		//CTransform* pCamTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")));
		//pCamTransform->Set_State(State::Pos, Player_Matrix.Position() + _vec4(0.0f, 3.f, 0.f, 1.f));
		//pCamTransform->LookAt_Dir(Player_Matrix.Look() + _vec4(0.f, 0.5f, 0.f, 0.f));
		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
		pPlayerTransform->Set_Position(_vec3(Player_Matrix.Position() + _vec3(0.f, 2.f, 0.f)));
		pPlayerTransform->LookAt_Dir(Player_Matrix.Look());

		m_pGameInstance->Set_HellHeight(-70.f);
		LIGHT_DESC* Light = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, L"Light_Main");
		*Light = g_Light_Village;
		m_iSkyTextureIndex = 12;
		m_eCurrentSpot = TS_Village;
		break;
	}
	case Client::TS_Minigame:
	{
		m_isInVillage = false;

		pGetPath = TEXT("../Bin/Data/Minigame_Player_Pos.dat");

		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MSG_BOX("파일을 찾지 못했습니다.");
			return;
		}

		_mat Player_Matrix{};
		inFile.read(reinterpret_cast<char*>(&Player_Matrix), sizeof(_mat));

		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
		pPlayerTransform->Set_Position(_vec3(-2000.70496f, 11.4677677f, -1999.06152f));//_vec3(Player_Matrix.Position() + _vec3(0.f, 2.f, 0.f)));
		pPlayerTransform->LookAt_Dir(Player_Matrix.Look());

		_vec4 vPosition = pPlayerTransform->Get_State(State::Pos);
		//m_pGameInstance->Set_HellHeight(-70.f);
		//LIGHT_DESC* Light = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, L"Light_Main");
		//*Light = g_Light_Village;
		//m_iSkyTextureIndex = 12;
		m_eCurrentSpot = TS_Minigame;
		break;
	}
	case Client::TS_DragonMap:
	{
		m_isInVillage = false;

		pGetPath = TEXT("../Bin/Data/DragonMap_Player_Pos.dat");

		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MSG_BOX("파일을 찾지 못했습니다.");
			return;
		}

		_mat Player_Matrix{};
		inFile.read(reinterpret_cast<char*>(&Player_Matrix), sizeof(_mat));

		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
		pPlayerTransform->Set_Position(_vec3(Player_Matrix.Position() + _vec3(0.f, 2.f, 0.f)));
		pPlayerTransform->LookAt_Dir(Player_Matrix.Look());

		m_pGameInstance->Set_HellHeight(-70.f);
		LIGHT_DESC* Light = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, L"Light_Main");
		*Light = g_Light_Dragon;

		m_iSkyTextureIndex = 11;
		m_eCurrentSpot = TS_DragonMap;
		break;
	}

	case Client::TS_BossRoom:
	{
		m_isInVillage = false;

		pGetPath = TEXT("../Bin/Data/BossRoom_Player_Pos.dat");

		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MSG_BOX("파일을 찾지 못했습니다.");
			return;
		}

		_mat Player_Matrix{};
		inFile.read(reinterpret_cast<char*>(&Player_Matrix), sizeof(_mat));

		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
		pPlayerTransform->Set_Position(_vec3(Player_Matrix.Position() + _vec3(0.f, 2.f, 0.f)));
		pPlayerTransform->LookAt_Dir(Player_Matrix.Look());

		//m_pGameInstance->Set_HellHeight(-70.f);
		//LIGHT_DESC* Light = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, L"Light_Main");
		//*Light = g_Light_Village;
		//m_iSkyTextureIndex = 12;
		m_eCurrentSpot = TS_BossRoom;
		break;
	}

	case Client::TS_MiniDungeon:
	{
		m_isInVillage = false;

		pGetPath = TEXT("../Bin/Data/MiniDungeon_Player_Pos.dat");

		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MSG_BOX("파일을 찾지 못했습니다.");
			return;
		}

		_mat Player_Matrix{};
		inFile.read(reinterpret_cast<char*>(&Player_Matrix), sizeof(_mat));

		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
		pPlayerTransform->Set_Position(_vec3(Player_Matrix.Position() + _vec3(0.f, 2.f, 0.f)));
		pPlayerTransform->LookAt_Dir(Player_Matrix.Look());

		//m_pGameInstance->Set_HellHeight(-70.f);
		//LIGHT_DESC* Light = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, L"Light_Main");
		//*Light = g_Light_Village;
		//m_iSkyTextureIndex = 12;
		m_eCurrentSpot = TS_MiniDungeon;
		break;
	}

	case Client::TS_SurvivalMap:
	{
		m_isInVillage = false;

		pGetPath = TEXT("../Bin/Data/Survival_Map_Player_Pos.dat");

		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MSG_BOX("파일을 찾지 못했습니다.");
			return;
		}

		_mat Player_Matrix{};
		inFile.read(reinterpret_cast<char*>(&Player_Matrix), sizeof(_mat));

		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
		pPlayerTransform->Set_Position(_vec3(Player_Matrix.Position() + _vec3(0.f, 2.f, 0.f)));
		pPlayerTransform->LookAt_Dir(Player_Matrix.Look());

		//m_pGameInstance->Set_HellHeight(-70.f);
		LIGHT_DESC* Light = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, L"Light_Main");
		*Light = g_Light_Survival;
		
		//m_iSkyTextureIndex = 12;
		m_eCurrentSpot = TS_SurvivalMap;
		break;
	}


	case Client::TS_SescoMap:
	{
		m_isInVillage = false;

		pGetPath = TEXT("../Bin/Data/SescoMap_Player_Pos.dat");

		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MSG_BOX("파일을 찾지 못했습니다.");
			return;
		}

		_mat Player_Matrix{};
		inFile.read(reinterpret_cast<char*>(&Player_Matrix), sizeof(_mat));

		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
		pPlayerTransform->Set_Position(_vec3(Player_Matrix.Position() + _vec3(0.f, 2.f, 0.f)));
		pPlayerTransform->LookAt_Dir(Player_Matrix.Look());

		//m_pGameInstance->Set_HellHeight(-70.f);
		//LIGHT_DESC* Light = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, L"Light_Main");
		//*Light = g_Light_Village;
		//m_iSkyTextureIndex = 12;
		m_eCurrentSpot = TS_SescoMap;
		break;
	}


	}
	// 파일을 읽어서 위치, 룩을 세팅하는 코드를 넣습니당

	//std::ifstream inFile(pGetPath, std::ios::binary);

	//if (!inFile.is_open())
	//{
	//	MSG_BOX("파일을 찾지 못했습니다.");
	//	return;
	//}

	//_vec4 Player_Pos{ 0.f };
	//inFile.read(reinterpret_cast<char*>(&Player_Pos), sizeof(_vec4));

	//CTransform* pCamTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")));
	//pCamTransform->Set_State(State::Pos, _vec4(-17.9027f, 18.f, 125.f, 1.f));
	//pCamTransform->LookAt_Dir(_vec4(-0.541082f, 0.548757f, 0.637257f, 0.f));
	//CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	//pPlayerTransform->Set_Position(_vec3(Player_Pos) + _vec3(0.f, 2.f, 0.f));
	//pPlayerTransform->LookAt_Dir(_vec4(-0.541082f, 0.f, 0.637257f, 0.f));

	return;
}

HRESULT CTrigger_Manager::Ready_Trigger_Village()
{
	TriggerInfo Info{};
	const TCHAR* pGetPath = L"../Bin/Data/Village_Trigger.dat";

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("../Bin/Data/Village_Trigger.dat 트리거 불러오기 실패.");
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

		CTrigger* pTrigger = dynamic_cast<CTrigger*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Trigger"), &TriggerInfo));
		if (not pTrigger)
		{
			MSG_BOX("트리거 생성 실패.");
			return E_FAIL;
		}

		m_pTrigger.push_back(pTrigger);
	}

	inFile.close();

	return S_OK;
}

void CTrigger_Manager::Free()
{
	for (auto& pTrigger : m_pTrigger)
	{
		Safe_Release(pTrigger);
	}
	m_pTrigger.clear();

	Safe_Release(m_pGameInstance);
}
