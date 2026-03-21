#include "imgui.h"
#include "ImGuizmo.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "GameObject.h"
#include "Animation.h"
#include "Bone.h"
#include "Layer.h"
#include "Effect_Dummy.h"

#include "Player.h"

#include <commdlg.h>

CImgui_Manager::CImgui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImgui_Manager::Initialize_Prototype(const GRAPHIC_DESC& GraphicDesc)
{
	m_hWnd = GraphicDesc.hWnd;
	m_iWinSizeX = GraphicDesc.iWinSizeX;
	m_iWinSizeY = GraphicDesc.iWinSizeY;
	//몬스터 모델
	string strInputFilePath = "../../Client/Bin/Resources/AnimMesh/Boss/";
	_uint iNumMonsterModels{};
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension().string() != ".hyuntraanimmesh")
			{
				continue;
			}
			iNumMonsterModels++;
			m_MonsterNames.push_back(entry.path().stem().string());
		}
	}
	strInputFilePath = "../../Client/Bin/Resources/AnimMesh/Monster/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension().string() != ".hyuntraanimmesh")
			{
				continue;
			}
			iNumMonsterModels++;
			m_MonsterNames.push_back(entry.path().stem().string());
		}
	}
	strInputFilePath = "../../Client/Bin/Resources/AnimMesh/Monster_NoUse/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension().string() != ".hyuntraanimmesh")
			{
				continue;
			}
			iNumMonsterModels++;
			m_MonsterNames.push_back(entry.path().stem().string());
		}
	}
	strInputFilePath = "../../Client/Bin/Resources/AnimMesh/NPC/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension().string() != ".hyuntraanimmesh")
			{
				continue;
			}
			iNumMonsterModels++;
			m_MonsterNames.push_back(entry.path().stem().string());
		}
	}
	strInputFilePath = "../../Client/Bin/Resources/AnimMesh/Pet/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension().string() != ".hyuntraanimmesh")
			{
				continue;
			}
			iNumMonsterModels++;
			m_MonsterNames.push_back(entry.path().stem().string());
		}
	}
	strInputFilePath = "../../Client/Bin/Resources/AnimMesh/Riding/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension().string() != ".hyuntraanimmesh")
			{
				continue;
			}
			iNumMonsterModels++;
			m_MonsterNames.push_back(entry.path().stem().string());
		}
	}
	strInputFilePath = "../../Client/Bin/Resources/AnimMesh/Skill_Model/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension().string() != ".hyuntraanimmesh")
			{
				continue;
			}
			iNumMonsterModels++;
			m_MonsterNames.push_back(entry.path().stem().string());
		}
	}

	m_szMonsterNames = new const _char * [iNumMonsterModels];

	for (_uint i = 0; i < iNumMonsterModels; i++)
	{
		m_szMonsterNames[i] = m_MonsterNames[i].c_str();
	}
	//이펙트
	strInputFilePath = "../../Client/Bin/EffectData/";
	_uint iNumEffectFiles{};
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension().string() != ".effect")
			{
				continue;
			}
			iNumEffectFiles++;
			m_EffectFiles.push_back(entry.path().stem().string());
		}
	}

	m_szEffectFiles = new const _char * [iNumEffectFiles];

	for (_uint i = 0; i < iNumEffectFiles; i++)
	{
		m_szEffectFiles[i] = m_EffectFiles[i].c_str();
	}
	//사운드
	strInputFilePath = "../../Client/Bin/Resources/Sound/";
	_uint iNumSoundFiles{};
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			iNumSoundFiles++;
			m_SoundFiles.push_back(entry.path().stem().string());
		}
	}

	m_szSoundFiles = new const _char * [iNumSoundFiles];

	for (_uint i = 0; i < iNumSoundFiles; i++)
	{
		m_szSoundFiles[i] = m_SoundFiles[i].c_str();
	}

	return S_OK;
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
	GetCursorPos(&m_ptMouse);
	ScreenToClient(m_hWnd, &m_ptMouse);

#pragma region CreateObject

	if (m_IsCreateModel)
	{
		if (not m_pPlayer)
		{
			m_pPlayer = (CPlayer*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Player"));
			CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
			m_vPreScale = pTargetTransform->Get_Scale();
			m_vPreRight = pTargetTransform->Get_State(State::Right);
			m_vPreUp = pTargetTransform->Get_State(State::Up);
			m_vPreLook = pTargetTransform->Get_State(State::Look);
			m_vPrePosition = pTargetTransform->Get_State(State::Pos);
		}
	}
#pragma endregion

	if (m_pGameInstance->Key_Down(DIK_X))
	{
		m_fTimeDelta = 0.f;
	}
	else if (m_pGameInstance->Key_Down(DIK_C))
	{
		m_fTimeDelta = fTimeDelta * m_fTimeRatio;
	}
	
	if (m_pGameInstance->Key_Down(DIK_B))
	{
		CEffect_Manager::Get_Instance()->Clear(LEVEL_TOOL);
	}

	if (m_pPlayer)
	{
		m_pPlayer->Set_ModelType((CPlayer::TYPE)m_eModelType);
		m_pPlayer->Set_ModelIndex(m_iCurrentModelIndex);
		m_pPlayer->Tick(m_fTimeDelta);
	}
}

void CImgui_Manager::Late_Tick(_float fTimeDelta)
{
	if (m_pPlayer)
	{
		m_pPlayer->Late_Tick(m_fTimeDelta);
	}
}

HRESULT CImgui_Manager::Render()
{
	bool bDemo = true;
	ImGui::ShowDemoWindow(&bDemo);
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGuiMenu();

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
	ImGuizmo::SetRect(0, 0, (_float)(m_iWinSizeX), (_float)(m_iWinSizeY));
	ImGuizmoMenu();

	ImGui::Render();

	return S_OK;
}

HRESULT CImgui_Manager::ImGuiMenu()
{
#pragma endregion
	ImGui::Begin("MENU");

	ImGui::RadioButton("MONSTER", &m_eModelType, TYPE_MONSTER); ImGui::SameLine();
	ImGui::RadioButton("SELECT", &m_eModelType, TYPE_SELECT); ImGui::SameLine();
	ImGui::RadioButton("PLAYER", &m_eModelType, TYPE_PLAYER);

	if (m_ePreModelType != m_eModelType)
	{
		m_ePreModelType = m_eModelType;
		m_iCurrentModelIndex = 0;
		m_iCurTriggerIndex = 0;
		m_iCurSoundNameIndex = 0;
		CEffect_Manager::Get_Instance()->Clear(LEVEL_TOOL);
	}

	if (m_eModelType == TYPE_MONSTER)
	{
		static const char* szCurrentModel = m_szMonsterNames[0];

		if (m_ePreModelType != m_eModelType)
		{
			szCurrentModel = m_szMonsterNames[0];
		}

		if (ImGui::BeginCombo("LIST", szCurrentModel))
		{
			for (_uint i = 0; i < m_MonsterNames.size(); i++)
			{
				_bool bSelectedModel = (szCurrentModel == m_MonsterNames[i]);
				if (ImGui::Selectable(m_szMonsterNames[i], bSelectedModel))
				{
					szCurrentModel = m_szMonsterNames[i];
					m_iCurrentModelIndex = i;
					m_iCurTriggerIndex = 0;
					m_iCurSoundNameIndex = 0;
				}
			}
			ImGui::EndCombo();
		}

	}
	else if (m_eModelType == TYPE_SELECT)
	{
		const char* szModelTag[4] = { "Select_Priest","Select_Rogue","Select_Sorceress","Select_Warrior" };
		static const char* szCurrentModel = "Select_Priest";
		if (m_ePreModelType != m_eModelType)
		{
			szCurrentModel = "Select_Priest";
		}

		if (ImGui::BeginCombo("LIST", szCurrentModel))
		{
			for (_uint i = 0; i < IM_ARRAYSIZE(szModelTag); i++)
			{
				_bool bSelectedModel = (szCurrentModel == szModelTag[i]);
				if (ImGui::Selectable(szModelTag[i], bSelectedModel))
				{
					szCurrentModel = szModelTag[i];
					m_iCurrentModelIndex = i;
					m_iCurTriggerIndex = 0;
					m_iCurSoundNameIndex = 0;
				}
			}
			ImGui::EndCombo();
		}
	}

	ImGui::SeparatorText("TRIGGER");
	ImGui::RadioButton("EFFECT", &m_eTriggerType, TRIGGER_EFFECT); ImGui::SameLine();
	ImGui::RadioButton("SOUND", &m_eTriggerType, TRIGGER_SOUND);
	if (m_ePreTriggerType != m_eTriggerType)
	{
		m_ePreTriggerType = m_eTriggerType;
		m_iCurTriggerIndex = 0;
		m_iSelectFile = 0;
		m_iCurSoundNameIndex = 0;
	}

	if (m_pPlayer)
	{
		if (ImGui::Button("SAVE##1"))
		{
			SaveFile();
		}
		ImGui::SameLine();
		if (ImGui::Button("LOAD##1"))
		{
			m_iCurTriggerIndex = 0;
			LoadFile();
		}
		ImGui::SameLine();
		if (ImGui::Button("UPDATE##1"))
		{
			UpdateFile();
		}

		if (ImGui::Button("ADD##1"))
		{	//트리거 정보 저장
#pragma region Player
			if (m_eModelType == TYPE_PLAYER)
			{
				CRealtimeVTFModel* pCurModel = m_pPlayer->Get_CurrentPlayerModel();
				_uint iCurrentAnimPos = static_cast<_uint>(pCurModel->Get_CurrentAnimPos());
				if (m_eTriggerType == TRIGGER_EFFECT)
				{
					_uint iSelectEffectFile = m_iSelectFile;
					_tchar szEffectName[MAX_PATH]{};
					MultiByteToWideChar(CP_UTF8, 0, m_szEffectFiles[iSelectEffectFile], (_int)strlen(m_szEffectFiles[iSelectEffectFile]), szEffectName, MAX_PATH);

					TRIGGEREFFECT_DESC EffectDesc{};
					EffectDesc.OffsetMatrix = _mat::Identity;
					EffectDesc.strEffectName = szEffectName;
					EffectDesc.iStartAnimIndex = m_AnimDesc.iAnimIndex;
					EffectDesc.fStartAnimPos = static_cast<_float>(iCurrentAnimPos);
					EffectDesc.iEndAnimIndices.push_back(-1);
					EffectDesc.fEndAnimPoses.push_back(0.f);
					EffectDesc.iBoneIndex = m_iCurrentBoneIndex;
					EffectDesc.IsFollow = true;
					pCurModel->Add_TriggerEffect(EffectDesc);
				}
				else if (m_eTriggerType == TRIGGER_SOUND)
				{
					_uint iSelectSoundFile = m_iSelectFile;
					_tchar szSoundName[MAX_PATH]{};
					MultiByteToWideChar(CP_UTF8, 0, m_szSoundFiles[iSelectSoundFile], (_int)strlen(m_szSoundFiles[iSelectSoundFile]), szSoundName, MAX_PATH);

					TRIGGERSOUND_DESC SoundDesc{};
					SoundDesc.strSoundNames.push_back(szSoundName);
					SoundDesc.iStartAnimIndex = m_AnimDesc.iAnimIndex;
					SoundDesc.fStartAnimPos = static_cast<_float>(iCurrentAnimPos);
					SoundDesc.fInitVolume = 0.5f;
					pCurModel->Add_TriggerSound(SoundDesc);
				}
			}
#pragma endregion
			else
			{
				CModel* pCurModel = m_pPlayer->Get_CurrentModel();
				_uint iCurrentAnimPos = static_cast<_uint>(pCurModel->Get_CurrentAnimPos());
				if (m_eTriggerType == TRIGGER_EFFECT)
				{
					_uint iSelectEffectFile = m_iSelectFile;
					_tchar szEffectName[MAX_PATH]{};
					MultiByteToWideChar(CP_UTF8, 0, m_szEffectFiles[iSelectEffectFile], (_int)strlen(m_szEffectFiles[iSelectEffectFile]), szEffectName, MAX_PATH);

					TRIGGEREFFECT_DESC EffectDesc{};
					EffectDesc.OffsetMatrix = _mat::Identity;
					EffectDesc.strEffectName = szEffectName;
					EffectDesc.iStartAnimIndex = m_AnimDesc.iAnimIndex;
					EffectDesc.fStartAnimPos = static_cast<_float>(iCurrentAnimPos);
					EffectDesc.iEndAnimIndices.push_back(-1);
					EffectDesc.fEndAnimPoses.push_back(0.f);
					EffectDesc.iBoneIndex = m_iCurrentBoneIndex;
					EffectDesc.IsFollow = true;
					pCurModel->Add_TriggerEffect(EffectDesc);
				}
				else if (m_eTriggerType == TRIGGER_SOUND)
				{
					_uint iSelectSoundFile = m_iSelectFile;
					_tchar szSoundName[MAX_PATH]{};
					MultiByteToWideChar(CP_UTF8, 0, m_szSoundFiles[iSelectSoundFile], (_int)strlen(m_szSoundFiles[iSelectSoundFile]), szSoundName, MAX_PATH);

					TRIGGERSOUND_DESC SoundDesc{};
					SoundDesc.strSoundNames.push_back(szSoundName);
					SoundDesc.iStartAnimIndex = m_AnimDesc.iAnimIndex;
					SoundDesc.fStartAnimPos = static_cast<_float>(iCurrentAnimPos);        
					SoundDesc.fInitVolume = 0.5f;
					pCurModel->Add_TriggerSound(SoundDesc);
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("DELETE"))
		{	//트리거 정보 삭제
#pragma region Player
			if (m_eModelType == TYPE_PLAYER)
			{
				CRealtimeVTFModel* pCurModel = m_pPlayer->Get_CurrentPlayerModel();
				if (m_eTriggerType == TRIGGER_EFFECT)
				{
					pCurModel->Delete_TriggerEffect(m_iCurTriggerIndex);
					if (m_iCurTriggerIndex != 0)
					{
						m_iCurTriggerIndex--;
					}
				}
				else if (m_eTriggerType == TRIGGER_SOUND)
				{
					TRIGGERSOUND_DESC* pSoundDesc = pCurModel->Get_TriggerSound(m_iCurTriggerIndex);
					if (pSoundDesc->iChannel != -1)
					{
						m_pGameInstance->StopSound(pSoundDesc->iChannel);
					}
					pCurModel->Delete_TriggerSound(m_iCurTriggerIndex);
					if (m_iCurTriggerIndex != 0)
					{
						m_iCurTriggerIndex--;
					}
				}
			}
#pragma endregion
			else
			{
				CModel* pCurModel = m_pPlayer->Get_CurrentModel();
				if (m_eTriggerType == TRIGGER_EFFECT)
				{
					pCurModel->Delete_TriggerEffect(m_iCurTriggerIndex);
					if (m_iCurTriggerIndex != 0)
					{
						m_iCurTriggerIndex--;
					}
				}
				else if (m_eTriggerType == TRIGGER_SOUND)
				{
					TRIGGERSOUND_DESC* pSoundDesc = pCurModel->Get_TriggerSound(m_iCurTriggerIndex);
					if (pSoundDesc->iChannel != -1)
					{
						m_pGameInstance->StopSound(pSoundDesc->iChannel);
					}
					pCurModel->Delete_TriggerSound(m_iCurTriggerIndex);
					if (m_iCurTriggerIndex != 0)
					{
						m_iCurTriggerIndex--;
					}
				}
			}
		}
	}

	ImGui::End();

	//ImGuizmo
#pragma region Animation Menu
	if (m_pPlayer)
	{
		ImGui::Begin("ANIMATION MENU");
		ImGui::PushItemWidth(300.f);
#pragma region ImGuizmo
		/*ImGui::RadioButton("STATE", &m_iManipulateType, TYPE_STATE); ImGui::SameLine();
		ImGui::RadioButton("RESET", &m_iManipulateType, TYPE_RESET);
		m_eManipulateType = (MANIPULATETYPE)(m_iManipulateType);*/
		if (ImGui::Button("SCALE"))
		{
			if (m_eManipulateType == TYPE_STATE)
			{
				m_eStateType = ImGuizmo::OPERATION::SCALE;
			}
			/*else if (m_eManipulateType == TYPE_RESET)
			{
				CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
				pTargetTransform->Set_Scale(m_vPreScale);
				m_vCurrentScale = m_vPreScale;
			}*/
		}
		ImGui::SameLine();
		if (ImGui::Button("ROTATION"))
		{
			if (m_eManipulateType == TYPE_STATE)
			{
				m_eStateType = ImGuizmo::OPERATION::ROTATE;
			}
			/*else if (m_eManipulateType == TYPE_RESET)
			{
				CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
				pTargetTransform->Set_State(State::Right, XMVector3Normalize(XMLoadFloat4(&m_vPreRight)) * m_vCurrentScale.x);
				pTargetTransform->Set_State(State::Up, XMVector3Normalize(XMLoadFloat4(&m_vPreUp)) * m_vCurrentScale.y);
				pTargetTransform->Set_State(State::Look, XMVector3Normalize(XMLoadFloat4(&m_vPreLook)) * m_vCurrentScale.z);
			}*/
		}
		ImGui::SameLine();
		if (ImGui::Button("POSITION"))
		{
			if (m_eManipulateType == TYPE_STATE)
			{
				m_eStateType = ImGuizmo::OPERATION::TRANSLATE;
			}
			/*else if (m_eManipulateType == TYPE_RESET)
			{
				CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
				pTargetTransform->Set_State(State::Pos, XMLoadFloat4(&m_vPrePosition));
			}*/
		}
		/*if (m_eManipulateType == TYPE_RESET)
		{
			ImGui::SameLine();
			if (ImGui::Button("ALL"))
			{
				CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
				pTargetTransform->Set_State(State::Right, XMLoadFloat4(&m_vPreRight));
				pTargetTransform->Set_State(State::Up, XMLoadFloat4(&m_vPreUp));
				pTargetTransform->Set_State(State::Look, XMLoadFloat4(&m_vPreLook));
				pTargetTransform->Set_State(State::Pos, XMLoadFloat4(&m_vPrePosition));
				m_vCurrentScale = m_vPreScale;
			}
		}*/
		ImGui::SameLine();
		ImGui::PushItemWidth(90.f);
		ImGui::InputFloat("TIME##1", &m_fTimeRatio, 0.01f, 0.f, "%.2f");
		ImGui::PopItemWidth();
#pragma endregion

#pragma region Player
		if (m_eModelType == TYPE_PLAYER)
		{
			CRealtimeVTFModel* pCurModel = m_pPlayer->Get_CurrentPlayerModel();
			if (pCurModel != nullptr)
			{
				_uint iNumAnimations = pCurModel->Get_NumAnim();
				vector<CAnimation*> pAnimations = pCurModel->Get_Animations();

				m_AnimationNames.clear();

				auto iter = pAnimations.begin();
				for (_uint i = 0; i < iNumAnimations; i++)
				{
					m_AnimationNames.push_back((*iter)->Get_Name());
					++iter;
				}

				m_iCurrentAnimIndex = 0;
				if (m_AnimationNames.size() != 0)
				{
					m_iCurrentAnimIndex = pCurModel->Get_CurrentAnimationIndex();
					if (ImGui::ListBox("ANIMATION", &m_iCurrentAnimIndex, m_AnimationNames.data(), m_AnimationNames.size()))
					{
						m_AnimDesc.iAnimIndex = m_iCurrentAnimIndex;
						m_AnimDesc.bSkipInterpolation = true;
						m_AnimDesc.fAnimSpeedRatio = 1.7f;
						pCurModel->Set_Animation(m_AnimDesc);
					}
				}

				string strNumAnims = "ALLANIMS: " + to_string(pCurModel->Get_NumAnim());
				ImGui::Text(strNumAnims.c_str()); ImGui::SameLine();
				string strCurAnim = "CURRENTANIM : " + to_string(m_iCurrentAnimIndex);
				ImGui::Text(strCurAnim.c_str());

				_int iCurrentAnimPos = static_cast<_int>(pCurModel->Get_CurrentAnimPos());
				iter = pAnimations.begin();
				for (_int i = 0; i < m_iCurrentAnimIndex; i++)
				{
					++iter;
				}

				if (ImGui::SliderInt("ANIMPOS", &iCurrentAnimPos, 0, static_cast<_int>((*iter)->Get_Duration())))
				{
					(*iter)->Set_CurrentAnimPos(static_cast<_float>(iCurrentAnimPos));
				}

				if (ImGui::InputInt("ANIMP0S", &iCurrentAnimPos, 1))
				{
					if (iCurrentAnimPos > static_cast<_int>((*iter)->Get_Duration()))
					{
						iCurrentAnimPos = static_cast<_int>((*iter)->Get_Duration());
					}
					else if (iCurrentAnimPos < 0)
					{
						iCurrentAnimPos = 0;
					}
					(*iter)->Set_CurrentAnimPos(static_cast<_float>(iCurrentAnimPos));
				}

				if (m_pGameInstance->Key_Down(DIK_Z))
				{
					if (m_AnimDesc.bRewindAnimation)
					{
						m_AnimDesc.bRewindAnimation = false;
					}
					else
					{
						m_AnimDesc.bRewindAnimation = true;
					}
					pCurModel->Set_Animation(m_AnimDesc);
				}
			}
		}
#pragma endregion
		else
		{
			CModel* pCurModel = m_pPlayer->Get_CurrentModel();
			if (pCurModel != nullptr)
			{
				_uint iNumAnimations = pCurModel->Get_NumAnim();
				vector<CAnimation*> pAnimations = pCurModel->Get_Animations();

				m_AnimationNames.clear();

				auto iter = pAnimations.begin();
				for (_uint i = 0; i < iNumAnimations; i++)
				{
					m_AnimationNames.push_back((*iter)->Get_Name());
					++iter;
				}

				m_iCurrentAnimIndex = 0;
				if (m_AnimationNames.size() != 0)
				{
					m_iCurrentAnimIndex = pCurModel->Get_CurrentAnimationIndex();
					if (ImGui::ListBox("ANIMATION", &m_iCurrentAnimIndex, m_AnimationNames.data(), m_AnimationNames.size()))
					{
						m_AnimDesc.iAnimIndex = m_iCurrentAnimIndex;
						m_AnimDesc.bSkipInterpolation = false;
						m_AnimDesc.fAnimSpeedRatio = 1.7f;
						pCurModel->Set_Animation(m_AnimDesc);
					}
				}

				string strNumAnims = "ALLANIMS: " + to_string(pCurModel->Get_NumAnim());
				ImGui::Text(strNumAnims.c_str()); ImGui::SameLine();
				string strCurAnim = "CURRENTANIM : " + to_string(m_iCurrentAnimIndex);
				ImGui::Text(strCurAnim.c_str());

				_int iCurrentAnimPos = static_cast<_int>(pCurModel->Get_CurrentAnimPos());
				iter = pAnimations.begin();
				for (_int i = 0; i < m_iCurrentAnimIndex; i++)
				{
					++iter;
				}

				if (ImGui::SliderInt("ANIMPOS", &iCurrentAnimPos, 0, static_cast<_int>((*iter)->Get_Duration())))
				{
					(*iter)->Set_CurrentAnimPos(static_cast<_float>(iCurrentAnimPos));
				}

				if (ImGui::InputInt("ANIMP0S", &iCurrentAnimPos, 1))
				{
					if (iCurrentAnimPos > static_cast<_int>((*iter)->Get_Duration()))
					{
						iCurrentAnimPos = static_cast<_int>((*iter)->Get_Duration());
					}
					else if (iCurrentAnimPos < 0)
					{
						iCurrentAnimPos = 0;
					}
					(*iter)->Set_CurrentAnimPos(static_cast<_float>(iCurrentAnimPos));
				}

				if (m_pGameInstance->Key_Down(DIK_Z))
				{
					if (m_AnimDesc.bRewindAnimation)
					{
						m_AnimDesc.bRewindAnimation = false;
					}
					else
					{
						m_AnimDesc.bRewindAnimation = true;
					}
					pCurModel->Set_Animation(m_AnimDesc);
				}
			}
		}
		
		ImGui::PopItemWidth();
		ImGui::End();
	}
#pragma endregion

	if (m_pPlayer)
	{
#pragma region Player

		if (m_eModelType == TYPE_PLAYER)
		{
			CRealtimeVTFModel* pCurModel = m_pPlayer->Get_CurrentPlayerModel();
#pragma region Effect_Trigger
			if (pCurModel->Get_NumTriggerEffect() != 0 && m_eTriggerType == TRIGGER_EFFECT)
			{
				ImGui::Begin("MATRIX MENU");

				if (pCurModel != nullptr)
				{
					TRIGGEREFFECT_DESC* pEffectDesc = pCurModel->Get_TriggerEffect(m_iCurTriggerIndex);

					_uint iNumBones = pCurModel->Get_NumBones();
					vector<CBone*> Bones = pCurModel->Get_Bones();

					m_BoneNames.clear();

					auto iter = Bones.begin();
					for (_uint i = 0; i < iNumBones; i++)
					{
						m_BoneNames.push_back((*iter)->Get_BoneName());
						++iter;
					}
					if (m_BoneNames.size() != 0)
					{
						m_iCurrentBoneIndex = pEffectDesc->iBoneIndex;
						ImGui::PushItemWidth(270.f);
						if (ImGui::ListBox("BONE", &m_iCurrentBoneIndex, m_BoneNames.data(), m_BoneNames.size()))
						{
							pEffectDesc->iBoneIndex = m_iCurrentBoneIndex;
						}
						ImGui::PopItemWidth();
					}

					string strNumBones = "ALLBONES : " + to_string(iNumBones);
					ImGui::Text(strNumBones.c_str()); ImGui::SameLine();
					string strCurBone = "CURRENTBONE : " + to_string(m_iCurrentBoneIndex);
					ImGui::Text(strCurBone.c_str());


					ImGui::PushItemWidth(90.f);
					_vec3 vScale{}, vPosition{};
					vScale.x = pEffectDesc->OffsetMatrix.Right().Length();
					vScale.y = pEffectDesc->OffsetMatrix.Up().Length();
					vScale.z = pEffectDesc->OffsetMatrix.Look().Length();
					vPosition = pEffectDesc->OffsetMatrix.Position();

					ImGui::SeparatorText("SIZE");
					ImGui::InputFloat("X##1", &vScale.x, 0.01f, 0.f, "%.2f"); ImGui::SameLine();
					ImGui::InputFloat("Y##1", &vScale.y, 0.01f, 0.f, "%.2f"); ImGui::SameLine();
					ImGui::InputFloat("Z##1", &vScale.z, 0.01f, 0.f, "%.2f");
					if (vScale.x <= 0.01f)
					{
						vScale.x = 0.1f;
					}
					if (vScale.y <= 0.01f)
					{
						vScale.y = 0.1f;
					}
					if (vScale.z <= 0.01f)
					{
						vScale.z = 0.1f;
					}
					ImGui::SeparatorText("OFFSET");
					ImGui::InputFloat("X##2", &vPosition.x, 0.01f, 0.f, "%.2f"); ImGui::SameLine();
					ImGui::InputFloat("Y##2", &vPosition.y, 0.01f, 0.f, "%.2f"); ImGui::SameLine();
					ImGui::InputFloat("Z##2", &vPosition.z, 0.01f, 0.f, "%.2f");
					ImGui::PopItemWidth();
					pEffectDesc->OffsetMatrix.Right(pEffectDesc->OffsetMatrix.Right().Get_Normalized() * vScale.x);
					pEffectDesc->OffsetMatrix.Up(pEffectDesc->OffsetMatrix.Up().Get_Normalized() * vScale.y);
					pEffectDesc->OffsetMatrix.Look(pEffectDesc->OffsetMatrix.Look().Get_Normalized() * vScale.z);
					pEffectDesc->OffsetMatrix.Position_vec3(vPosition);

					ImGui::SeparatorText("OFFSET");
					if (ImGui::Button("SAVE##2"))
					{
						m_OffsetMatrix = pEffectDesc->OffsetMatrix;
					}
					ImGui::SameLine();
					if (ImGui::Button("LOAD##2"))
					{
						pEffectDesc->OffsetMatrix = m_OffsetMatrix;
					}
				}

				ImGui::End();

				ImGui::Begin("TRIGGER MENU");
				ImGui::PushItemWidth(300.f);

				//이펙트 이름 띄우기
				vector<TRIGGEREFFECT_DESC> EffectDescs = pCurModel->Get_TriggerEffects();
				_char** ppEffectTriggerList = new _char * [EffectDescs.size()];

				for (size_t i = 0; i < EffectDescs.size(); i++)
				{
					ppEffectTriggerList[i] = new _char[MAX_PATH];
					strcpy_s(ppEffectTriggerList[i], MAX_PATH, m_AnimationNames[EffectDescs[i].iStartAnimIndex]);
				}
				if (ImGui::ListBox("EFFECT##1", &m_iCurTriggerIndex, ppEffectTriggerList, EffectDescs.size()))
				{
				}
				//이펙트 파일 이름 띄우기
				_char* pEffectName = new _char[MAX_PATH];
				int bufferSize = WideCharToMultiByte(CP_UTF8, 0, EffectDescs[m_iCurTriggerIndex].strEffectName.c_str(), -1, nullptr, 0, nullptr, nullptr);
				WideCharToMultiByte(CP_UTF8, 0, EffectDescs[m_iCurTriggerIndex].strEffectName.c_str(), -1, pEffectName, bufferSize, nullptr, nullptr);
				ImGui::SeparatorText("EFFECT");
				ImGui::Text("NAME :"); ImGui::SameLine();
				ImGui::Text(pEffectName);
				//릭 제거
				for (size_t i = 0; i < EffectDescs.size(); i++)
				{
					Safe_Delete_Array(ppEffectTriggerList[i]);
				}
				Safe_Delete_Array(ppEffectTriggerList);
				Safe_Delete_Array(pEffectName);
				//
				TRIGGEREFFECT_DESC* pEffectDesc = m_pPlayer->Get_CurrentPlayerModel()->Get_TriggerEffect(m_iCurTriggerIndex);
				if (ImGui::Button("START"))
				{
					pEffectDesc->iStartAnimIndex = m_pPlayer->Get_CurrentPlayerModel()->Get_CurrentAnimationIndex();
					_uint iCurrentAnimPos = static_cast<_uint>(m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos());
					pEffectDesc->fStartAnimPos = static_cast<_float>(iCurrentAnimPos);
				}

				string strStartEffectIndex = "ANIMINDEX : " + to_string(pEffectDesc->iStartAnimIndex);
				ImGui::Text(strStartEffectIndex.c_str()); ImGui::SameLine();
				string strStartEffectPos = "ANIMPOS : " + to_string(static_cast<_int>(pEffectDesc->fStartAnimPos));
				ImGui::Text(strStartEffectPos.c_str());

				if (ImGui::Button("END"))
				{
					_uint iEndIndex = static_cast<_uint>(pEffectDesc->iEndAnimIndices.size()) - 1;
					pEffectDesc->iEndAnimIndices[iEndIndex] = m_pPlayer->Get_CurrentPlayerModel()->Get_CurrentAnimationIndex();
					_uint iCurrentAnimPos = static_cast<_uint>(m_pPlayer->Get_CurrentPlayerModel()->Get_CurrentAnimPos());
					pEffectDesc->fEndAnimPoses[iEndIndex] = static_cast<_float>(iCurrentAnimPos);
				}
				ImGui::SameLine();
				if (ImGui::Button("ADD##2"))
				{
					pEffectDesc->iEndAnimIndices.push_back(-1);
					_uint iEndAnimPos = static_cast<_uint>(0.f);
					pEffectDesc->fEndAnimPoses.push_back(static_cast<_float>(iEndAnimPos));
				}
				ImGui::SameLine();
				if (ImGui::Button("DELETE##2"))
				{
					if (pEffectDesc->iEndAnimIndices.size() > 1)
					{
						pEffectDesc->iEndAnimIndices.pop_back();
						pEffectDesc->fEndAnimPoses.pop_back();
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("INIT"))
				{
					if (pEffectDesc->iEndAnimIndices.size() == 1)
					{
						pEffectDesc->iEndAnimIndices[0] = -1;
						_uint iEndAnimPos = static_cast<_uint>(0.f);
						pEffectDesc->fEndAnimPoses[0] = static_cast<_float>(iEndAnimPos);
					}
				}

				for (size_t i = 0; i < pEffectDesc->iEndAnimIndices.size(); i++)
				{
					string strEndEffectIndex = "ANIMINDEX  : " + to_string(pEffectDesc->iEndAnimIndices[i]);
					ImGui::Text(strEndEffectIndex.c_str()); ImGui::SameLine();
					string strEndEffectPos = "ANIMPOS : " + to_string(static_cast<_int>(pEffectDesc->fEndAnimPoses[i]));
					ImGui::Text(strEndEffectPos.c_str());
				}

				ImGui::SeparatorText("BONE");
				if (ImGui::Button("FOLLOW"))
				{
					if (pEffectDesc->IsFollow == false)
					{
						pEffectDesc->IsFollow = true;
					}
					else if (pEffectDesc->IsFollow == true)
					{
						pEffectDesc->IsFollow = false;
					}
				}
				ImGui::SameLine();
				if (pEffectDesc->IsFollow)
				{
					ImGui::Text("TRUE");
				}
				else
				{
					ImGui::Text("FALSE");
				}
				ImGui::SameLine();
				if (ImGui::Button("DELETE ROTATE"))
				{
					if (pEffectDesc->IsDeleteRotateToBone == false)
					{
						pEffectDesc->IsDeleteRotateToBone = true;
					}
					else if (pEffectDesc->IsDeleteRotateToBone == true)
					{
						pEffectDesc->IsDeleteRotateToBone = false;
					}
				}
				ImGui::SameLine();
				if (pEffectDesc->IsDeleteRotateToBone)
				{
					ImGui::Text("TRUE");
				}
				else
				{
					ImGui::Text("FALSE");
				}

				if (ImGui::Button("CLIENT TRIGGER"))
				{
					if (pEffectDesc->IsClientTrigger == false)
					{
						pEffectDesc->IsClientTrigger = true;
					}
					else if (pEffectDesc->IsClientTrigger == true)
					{
						pEffectDesc->IsClientTrigger = false;
					}
				}
				ImGui::SameLine();
				if (pEffectDesc->IsClientTrigger)
				{
					ImGui::Text("TRUE");
				}
				else
				{
					ImGui::Text("FALSE");
				}

				//이펙트 다시 보기
				if (m_pGameInstance->Key_Pressing(DIK_V))
				{
					pEffectDesc->HasCreated = false;
				}

				ImGui::PopItemWidth();
				ImGui::End();
			}
#pragma endregion

#pragma region Sound_Trigger
			if (pCurModel->Get_NumTriggerSound() != 0 && m_eTriggerType == TRIGGER_SOUND)
			{
				ImGui::Begin("TRIGGER MENU");
				ImGui::PushItemWidth(300.f);

				vector<TRIGGERSOUND_DESC> SoundDescs = pCurModel->Get_TriggerSounds();
				//사운드 이름 띄우기
				_char** ppSoundTriggerList = new _char * [SoundDescs.size()];

				for (size_t i = 0; i < SoundDescs.size(); i++)
				{
					ppSoundTriggerList[i] = new _char[MAX_PATH];
					strcpy_s(ppSoundTriggerList[i], MAX_PATH, m_AnimationNames[SoundDescs[i].iStartAnimIndex]);
				}
				if (ImGui::ListBox("SOUND##1", &m_iCurTriggerIndex, ppSoundTriggerList, SoundDescs.size()))
				{
					m_iCurSoundNameIndex = 0;
				}
				//사운드 파일 이름 띄우기
				_char** ppSoundNameList = new _char * [SoundDescs[m_iCurTriggerIndex].strSoundNames.size()] {};

				for (size_t i = 0; i < SoundDescs[m_iCurTriggerIndex].strSoundNames.size(); i++)
				{
					ppSoundNameList[i] = new _char[MAX_PATH];
					int bufferSize = WideCharToMultiByte(CP_UTF8, 0, SoundDescs[m_iCurTriggerIndex].strSoundNames[i].c_str(), -1, nullptr, 0, nullptr, nullptr);
					WideCharToMultiByte(CP_UTF8, 0, SoundDescs[m_iCurTriggerIndex].strSoundNames[i].c_str(), -1, ppSoundNameList[i], bufferSize, nullptr, nullptr);
				}
				if (ImGui::ListBox("SOUND##2", &m_iCurSoundNameIndex, ppSoundNameList, SoundDescs[m_iCurTriggerIndex].strSoundNames.size()))
				{
				}
				//릭 제거
				for (size_t i = 0; i < SoundDescs.size(); i++)
				{
					Safe_Delete_Array(ppSoundTriggerList[i]);
				}
				Safe_Delete_Array(ppSoundTriggerList);
				for (size_t i = 0; i < SoundDescs[m_iCurTriggerIndex].strSoundNames.size(); i++)
				{
					Safe_Delete_Array(ppSoundNameList[i]);
				}
				Safe_Delete_Array(ppSoundNameList);
				
				TRIGGERSOUND_DESC* pSoundDesc = m_pPlayer->Get_CurrentPlayerModel()->Get_TriggerSound(m_iCurTriggerIndex);
				ImGui::SeparatorText("SOUND");
				if (ImGui::Button("START"))
				{
					pSoundDesc->iStartAnimIndex = m_pPlayer->Get_CurrentPlayerModel()->Get_CurrentAnimationIndex();
					_uint iCurrentAnimPos = static_cast<_uint>(m_pPlayer->Get_CurrentPlayerModel()->Get_CurrentAnimPos());
					pSoundDesc->fStartAnimPos = static_cast<_float>(iCurrentAnimPos);
				}
				ImGui::SameLine();
				ImGui::Text("/");
				ImGui::SameLine();
				if (ImGui::Button("ADD##2"))
				{
					_uint iSelectSoundFile = m_iSelectFile;
					_tchar szSoundName[MAX_PATH]{};
					MultiByteToWideChar(CP_UTF8, 0, m_szSoundFiles[iSelectSoundFile], (_int)strlen(m_szSoundFiles[iSelectSoundFile]), szSoundName, MAX_PATH);

					pSoundDesc->strSoundNames.push_back(szSoundName);
				}
				ImGui::SameLine();
				if (ImGui::Button("DELETE##2"))
				{
					pSoundDesc->strSoundNames.pop_back();
				}

				string strStartEffectIndex = "ANIMINDEX : " + to_string(pSoundDesc->iStartAnimIndex);
				ImGui::Text(strStartEffectIndex.c_str()); ImGui::SameLine();
				string strStartEffectPos = "ANIMPOS : " + to_string(static_cast<_int>(pSoundDesc->fStartAnimPos));
				ImGui::Text(strStartEffectPos.c_str());

				if (ImGui::Button("END"))
				{
					if (pSoundDesc->iEndAnimIndices.size() > 0)
					{
						_uint iEndIndex = static_cast<_uint>(pSoundDesc->iEndAnimIndices.size()) - 1;
						pSoundDesc->iEndAnimIndices[iEndIndex] = m_pPlayer->Get_CurrentPlayerModel()->Get_CurrentAnimationIndex();
						_uint iCurrentAnimPos = static_cast<_uint>(m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos());
						pSoundDesc->fEndAnimPoses[iEndIndex] = static_cast<_float>(iCurrentAnimPos);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("ADD##3"))
				{
					pSoundDesc->iEndAnimIndices.push_back(-1);
					_uint iEndAnimPos = static_cast<_uint>(0.f);
					pSoundDesc->fEndAnimPoses.push_back(static_cast<_float>(iEndAnimPos));
				}
				ImGui::SameLine();
				if (ImGui::Button("DELETE##3"))
				{
					if (pSoundDesc->iEndAnimIndices.size() > 1)
					{
						pSoundDesc->iEndAnimIndices.pop_back();
						pSoundDesc->fEndAnimPoses.pop_back();
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("INIT"))
				{
					if (pSoundDesc->iEndAnimIndices.size() == 1)
					{
						pSoundDesc->iEndAnimIndices[0] = -1;
						_uint iEndAnimPos = static_cast<_uint>(0.f);
						pSoundDesc->fEndAnimPoses[0] = static_cast<_float>(iEndAnimPos);
					}
				}

				for (size_t i = 0; i < pSoundDesc->iEndAnimIndices.size(); i++)
				{
					string strEndEffectIndex = "ANIMINDEX  : " + to_string(pSoundDesc->iEndAnimIndices[i]);
					ImGui::Text(strEndEffectIndex.c_str()); ImGui::SameLine();
					string strEndEffectPos = "ANIMPOS : " + to_string(static_cast<_int>(pSoundDesc->fEndAnimPoses[i]));
					ImGui::Text(strEndEffectPos.c_str());
				}

				ImGui::PopItemWidth();
				ImGui::PushItemWidth(100.f);

				ImGui::InputFloat("FADEOUT SECOND##1", &pSoundDesc->fFadeoutSecond, 0.05f, 0.f, "%.2f");
				if (pSoundDesc->fFadeoutSecond <= 0.f)
				{
					pSoundDesc->fFadeoutSecond = 0.1f;
				}

				ImGui::PopItemWidth();

				ImGui::SeparatorText("SOUND");
				ImGui::PushItemWidth(100.f);

				if (ImGui::InputFloat("VOLUME##1", &pSoundDesc->fInitVolume, 0.05f, 0.f, "%.2f"))
				{
					if (pSoundDesc->iChannel != -1)
					{
						m_pGameInstance->Set_ChannelVolume(pSoundDesc->iChannel, pSoundDesc->fInitVolume);
					}
				}
				if (pSoundDesc->fInitVolume >= 1.f)
				{
					pSoundDesc->fInitVolume = 1.f;
				}
				else if (pSoundDesc->fInitVolume <= 0.f)
				{
					pSoundDesc->fInitVolume = 0.f;
				}

				ImGui::InputFloat("STARTPOS##1", &pSoundDesc->fStartPosRatio, 0.05f, 0.f, "%.2f");
				if (pSoundDesc->fStartPosRatio >= 1.f)
				{
					pSoundDesc->fStartPosRatio = 1.f;
				}
				else if (pSoundDesc->fStartPosRatio <= 0.f)
				{
					pSoundDesc->fStartPosRatio = 0.f;
				}

				if (ImGui::Button("FADEIN SOUND##1"))
				{
					if (pSoundDesc->IsFadeinSound == false)
					{
						pSoundDesc->IsFadeinSound = true;
					}
					else if (pSoundDesc->IsFadeinSound == true)
					{
						pSoundDesc->IsFadeinSound = false;
					}
				}
				ImGui::SameLine();
				if (pSoundDesc->IsFadeinSound)
				{
					ImGui::Text("TRUE");
				}
				else
				{
					ImGui::Text("FALSE");
				}

				ImGui::InputFloat("FADEIN SECOND##1", &pSoundDesc->fFadeinSecond, 0.05f, 0.f, "%.2f");
				if (pSoundDesc->fFadeinSecond <= 0.f)
				{
					pSoundDesc->fFadeinSecond = 0.1f;
				}

				ImGui::SeparatorText("SOUND");
				if (ImGui::Button("CLIENT TRIGGER"))
				{
					if (pSoundDesc->IsClientTrigger == false)
					{
						pSoundDesc->IsClientTrigger = true;
					}
					else if (pSoundDesc->IsClientTrigger == true)
					{
						pSoundDesc->IsClientTrigger = false;
					}
				}

				ImGui::SameLine();
				if (pSoundDesc->IsClientTrigger)
				{
					ImGui::Text("TRUE");
				}
				else
				{
					ImGui::Text("FALSE");
				}

				ImGui::SeparatorText("CHANNEL");
				string strChannel = "CHANNEL : " + to_string(pSoundDesc->iChannel);
				ImGui::Text(strChannel.c_str());

				//사운드 다시 듣기
				if (m_pGameInstance->Key_Down(DIK_V))
				{
					if (pSoundDesc->iChannel != -1)
					{
						m_pGameInstance->StopSound(pSoundDesc->iChannel);
						pSoundDesc->iChannel = -1;
					}
					pSoundDesc->iChannel = m_pGameInstance->Play_Sound(pSoundDesc->strSoundNames[0], pSoundDesc->fInitVolume, false, pSoundDesc->fStartPosRatio);
					if (pSoundDesc->IsFadeinSound)
					{
						m_pGameInstance->FadeinSound(pSoundDesc->iChannel, 0.016f, pSoundDesc->fFadeinSecond);
					}
				}

				ImGui::PopItemWidth();
				ImGui::End();
			}
#pragma endregion
		}
#pragma endregion
		else
		{
			CModel* pCurModel = m_pPlayer->Get_CurrentModel();
#pragma region Effect_Trigger
			if (pCurModel->Get_NumTriggerEffect() != 0 && m_eTriggerType == TRIGGER_EFFECT)
			{
				ImGui::Begin("MATRIX MENU");

				CModel* pCurModel = m_pPlayer->Get_CurrentModel();
				if (pCurModel != nullptr)
				{
					TRIGGEREFFECT_DESC* pEffectDesc = pCurModel->Get_TriggerEffect(m_iCurTriggerIndex);

					_uint iNumBones = pCurModel->Get_NumBones();
					vector<CBone*> Bones = pCurModel->Get_Bones();

					m_BoneNames.clear();

					auto iter = Bones.begin();
					for (_uint i = 0; i < iNumBones; i++)
					{
						m_BoneNames.push_back((*iter)->Get_BoneName());
						++iter;
					}
					if (m_BoneNames.size() != 0)
					{
						m_iCurrentBoneIndex = pEffectDesc->iBoneIndex;
						ImGui::PushItemWidth(270.f);
						if (ImGui::ListBox("BONE", &m_iCurrentBoneIndex, m_BoneNames.data(), m_BoneNames.size()))
						{
							pEffectDesc->iBoneIndex = m_iCurrentBoneIndex;
						}
						ImGui::PopItemWidth();
					}

					string strNumBones = "ALLBONES : " + to_string(iNumBones);
					ImGui::Text(strNumBones.c_str()); ImGui::SameLine();
					string strCurBone = "CURRENTBONE : " + to_string(m_iCurrentBoneIndex);
					ImGui::Text(strCurBone.c_str());


					ImGui::PushItemWidth(90.f);
					_vec3 vScale{}, vPosition{};
					vScale.x = pEffectDesc->OffsetMatrix.Right().Length();
					vScale.y = pEffectDesc->OffsetMatrix.Up().Length();
					vScale.z = pEffectDesc->OffsetMatrix.Look().Length();
					vPosition = pEffectDesc->OffsetMatrix.Position();

					ImGui::SeparatorText("SIZE");
					ImGui::InputFloat("X##1", &vScale.x, 0.01f, 0.f, "%.2f"); ImGui::SameLine();
					ImGui::InputFloat("Y##1", &vScale.y, 0.01f, 0.f, "%.2f"); ImGui::SameLine();
					ImGui::InputFloat("Z##1", &vScale.z, 0.01f, 0.f, "%.2f");
					if (vScale.x <= 0.01f)
					{
						vScale.x = 0.1f;
					}
					if (vScale.y <= 0.01f)
					{
						vScale.y = 0.1f;
					}
					if (vScale.z <= 0.01f)
					{
						vScale.z = 0.1f;
					}
					ImGui::SeparatorText("OFFSET");
					ImGui::InputFloat("X##2", &vPosition.x, 0.01f, 0.f, "%.2f"); ImGui::SameLine();
					ImGui::InputFloat("Y##2", &vPosition.y, 0.01f, 0.f, "%.2f"); ImGui::SameLine();
					ImGui::InputFloat("Z##2", &vPosition.z, 0.01f, 0.f, "%.2f");
					ImGui::PopItemWidth();
					pEffectDesc->OffsetMatrix.Right(pEffectDesc->OffsetMatrix.Right().Get_Normalized() * vScale.x);
					pEffectDesc->OffsetMatrix.Up(pEffectDesc->OffsetMatrix.Up().Get_Normalized() * vScale.y);
					pEffectDesc->OffsetMatrix.Look(pEffectDesc->OffsetMatrix.Look().Get_Normalized() * vScale.z);
					pEffectDesc->OffsetMatrix.Position_vec3(vPosition);

					ImGui::SeparatorText("OFFSET");
					if (ImGui::Button("SAVE##2"))
					{
						m_OffsetMatrix = pEffectDesc->OffsetMatrix;
					}
					ImGui::SameLine();
					if (ImGui::Button("LOAD##2"))
					{
						pEffectDesc->OffsetMatrix = m_OffsetMatrix;
					}
				}

				ImGui::End();

				ImGui::Begin("TRIGGER MENU");
				ImGui::PushItemWidth(300.f);

				//이펙트 이름 띄우기
				vector<TRIGGEREFFECT_DESC> EffectDescs = pCurModel->Get_TriggerEffects();
				_char** ppEffectTriggerList = new _char * [EffectDescs.size()];

				for (size_t i = 0; i < EffectDescs.size(); i++)
				{
					ppEffectTriggerList[i] = new _char[MAX_PATH];
					strcpy_s(ppEffectTriggerList[i], MAX_PATH, m_AnimationNames[EffectDescs[i].iStartAnimIndex]);
				}
				if (ImGui::ListBox("EFFECT##1", &m_iCurTriggerIndex, ppEffectTriggerList, EffectDescs.size()))
				{
				}
				//이펙트 파일 이름 띄우기
				_char* pEffectName = new _char[MAX_PATH];
				int bufferSize = WideCharToMultiByte(CP_UTF8, 0, EffectDescs[m_iCurTriggerIndex].strEffectName.c_str(), -1, nullptr, 0, nullptr, nullptr);
				WideCharToMultiByte(CP_UTF8, 0, EffectDescs[m_iCurTriggerIndex].strEffectName.c_str(), -1, pEffectName, bufferSize, nullptr, nullptr);
				ImGui::SeparatorText("EFFECT");
				ImGui::Text("NAME :"); ImGui::SameLine();
				ImGui::Text(pEffectName);
				//릭 제거
				for (size_t i = 0; i < EffectDescs.size(); i++)
				{
					Safe_Delete_Array(ppEffectTriggerList[i]);
				}
				Safe_Delete_Array(ppEffectTriggerList);
				Safe_Delete_Array(pEffectName);
				//
				TRIGGEREFFECT_DESC* pEffectDesc = m_pPlayer->Get_CurrentModel()->Get_TriggerEffect(m_iCurTriggerIndex);
				if (ImGui::Button("START"))
				{
					pEffectDesc->iStartAnimIndex = m_pPlayer->Get_CurrentModel()->Get_CurrentAnimationIndex();
					_uint iCurrentAnimPos = static_cast<_uint>(m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos());
					pEffectDesc->fStartAnimPos = static_cast<_float>(iCurrentAnimPos);
				}

				string strStartEffectIndex = "ANIMINDEX : " + to_string(pEffectDesc->iStartAnimIndex);
				ImGui::Text(strStartEffectIndex.c_str()); ImGui::SameLine();
				string strStartEffectPos = "ANIMPOS : " + to_string(static_cast<_int>(pEffectDesc->fStartAnimPos));
				ImGui::Text(strStartEffectPos.c_str());

				if (ImGui::Button("END"))
				{
					_uint iEndIndex = static_cast<_uint>(pEffectDesc->iEndAnimIndices.size()) - 1;
					pEffectDesc->iEndAnimIndices[iEndIndex] = m_pPlayer->Get_CurrentModel()->Get_CurrentAnimationIndex();
					_uint iCurrentAnimPos = static_cast<_uint>(m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos());
					pEffectDesc->fEndAnimPoses[iEndIndex] = static_cast<_float>(iCurrentAnimPos);
				}
				ImGui::SameLine();
				if (ImGui::Button("ADD##2"))
				{
					pEffectDesc->iEndAnimIndices.push_back(-1);
					_uint iEndAnimPos = static_cast<_uint>(0.f);
					pEffectDesc->fEndAnimPoses.push_back(static_cast<_float>(iEndAnimPos));
				}
				ImGui::SameLine();
				if (ImGui::Button("DELETE##2"))
				{
					if (pEffectDesc->iEndAnimIndices.size() > 1)
					{
						pEffectDesc->iEndAnimIndices.pop_back();
						pEffectDesc->fEndAnimPoses.pop_back();
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("INIT"))
				{
					if (pEffectDesc->iEndAnimIndices.size() == 1)
					{
						pEffectDesc->iEndAnimIndices[0] = -1;
						_uint iEndAnimPos = static_cast<_uint>(0.f);
						pEffectDesc->fEndAnimPoses[0] = static_cast<_float>(iEndAnimPos);
					}
				}

				for (size_t i = 0; i < pEffectDesc->iEndAnimIndices.size(); i++)
				{
					string strEndEffectIndex = "ANIMINDEX  : " + to_string(pEffectDesc->iEndAnimIndices[i]);
					ImGui::Text(strEndEffectIndex.c_str()); ImGui::SameLine();
					string strEndEffectPos = "ANIMPOS : " + to_string(static_cast<_int>(pEffectDesc->fEndAnimPoses[i]));
					ImGui::Text(strEndEffectPos.c_str());
				}

				ImGui::SeparatorText("BONE");
				if (ImGui::Button("FOLLOW"))
				{
					if (pEffectDesc->IsFollow == false)
					{
						pEffectDesc->IsFollow = true;
					}
					else if (pEffectDesc->IsFollow == true)
					{
						pEffectDesc->IsFollow = false;
					}
				}
				ImGui::SameLine();
				if (pEffectDesc->IsFollow)
				{
					ImGui::Text("TRUE");
				}
				else
				{
					ImGui::Text("FALSE");
				}
				ImGui::SameLine();
				if (ImGui::Button("DELETE ROTATE"))
				{
					if (pEffectDesc->IsDeleteRotateToBone == false)
					{
						pEffectDesc->IsDeleteRotateToBone = true;
					}
					else if (pEffectDesc->IsDeleteRotateToBone == true)
					{
						pEffectDesc->IsDeleteRotateToBone = false;
					}
				}
				ImGui::SameLine();
				if (pEffectDesc->IsDeleteRotateToBone)
				{
					ImGui::Text("TRUE");
				}
				else
				{
					ImGui::Text("FALSE");
				}

				if (ImGui::Button("CLIENT TRIGGER"))
				{
					if (pEffectDesc->IsClientTrigger == false)
					{
						pEffectDesc->IsClientTrigger = true;
					}
					else if (pEffectDesc->IsClientTrigger == true)
					{
						pEffectDesc->IsClientTrigger = false;
					}
				}
				ImGui::SameLine();
				if (pEffectDesc->IsClientTrigger)
				{
					ImGui::Text("TRUE");
				}
				else
				{
					ImGui::Text("FALSE");
				}

				//이펙트 다시 보기
				if (m_pGameInstance->Key_Pressing(DIK_V))
				{
					pEffectDesc->HasCreated = false;
				}

				ImGui::PopItemWidth();
				ImGui::End();
			}
#pragma endregion

#pragma region Sound_Trigger
			if (pCurModel->Get_NumTriggerSound() != 0 && m_eTriggerType == TRIGGER_SOUND)
			{
				ImGui::Begin("TRIGGER MENU");
				ImGui::PushItemWidth(300.f);

				CModel* pCurModel = m_pPlayer->Get_CurrentModel();
				vector<TRIGGERSOUND_DESC> SoundDescs = pCurModel->Get_TriggerSounds();
				//사운드 이름 띄우기
				_char** ppSoundTriggerList = new _char * [SoundDescs.size()];

				for (size_t i = 0; i < SoundDescs.size(); i++)
				{
					ppSoundTriggerList[i] = new _char[MAX_PATH];
					strcpy_s(ppSoundTriggerList[i], MAX_PATH, m_AnimationNames[SoundDescs[i].iStartAnimIndex]);
				}
				if (ImGui::ListBox("SOUND##1", &m_iCurTriggerIndex, ppSoundTriggerList, SoundDescs.size()))
				{
					m_iCurSoundNameIndex = 0;
				}
				//사운드 파일 이름 띄우기
				_char** ppSoundNameList = new _char * [SoundDescs[m_iCurTriggerIndex].strSoundNames.size()] {};

				for (size_t i = 0; i < SoundDescs[m_iCurTriggerIndex].strSoundNames.size(); i++)
				{
					ppSoundNameList[i] = new _char[MAX_PATH];
					int bufferSize = WideCharToMultiByte(CP_UTF8, 0, SoundDescs[m_iCurTriggerIndex].strSoundNames[i].c_str(), -1, nullptr, 0, nullptr, nullptr);
					WideCharToMultiByte(CP_UTF8, 0, SoundDescs[m_iCurTriggerIndex].strSoundNames[i].c_str(), -1, ppSoundNameList[i], bufferSize, nullptr, nullptr);
				}
				if (ImGui::ListBox("SOUND##2", &m_iCurSoundNameIndex, ppSoundNameList, SoundDescs[m_iCurTriggerIndex].strSoundNames.size()))
				{
				}
				//릭 제거
				for (size_t i = 0; i < SoundDescs.size(); i++)
				{
					Safe_Delete_Array(ppSoundTriggerList[i]);
				}
				Safe_Delete_Array(ppSoundTriggerList);
				for (size_t i = 0; i < SoundDescs[m_iCurTriggerIndex].strSoundNames.size(); i++)
				{
					Safe_Delete_Array(ppSoundNameList[i]);
				}
				Safe_Delete_Array(ppSoundNameList);

				TRIGGERSOUND_DESC* pSoundDesc = m_pPlayer->Get_CurrentModel()->Get_TriggerSound(m_iCurTriggerIndex);
				ImGui::SeparatorText("SOUND");
				if (ImGui::Button("START"))
				{
					pSoundDesc->iStartAnimIndex = m_pPlayer->Get_CurrentModel()->Get_CurrentAnimationIndex();
					_uint iCurrentAnimPos = static_cast<_uint>(m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos());
					pSoundDesc->fStartAnimPos = static_cast<_float>(iCurrentAnimPos);
				}
				ImGui::SameLine();
				ImGui::Text("/");
				ImGui::SameLine();
				if (ImGui::Button("ADD##2"))
				{
					_uint iSelectSoundFile = m_iSelectFile;
					_tchar szSoundName[MAX_PATH]{};
					MultiByteToWideChar(CP_UTF8, 0, m_szSoundFiles[iSelectSoundFile], (_int)strlen(m_szSoundFiles[iSelectSoundFile]), szSoundName, MAX_PATH);

					pSoundDesc->strSoundNames.push_back(szSoundName);
				}
				ImGui::SameLine();
				if (ImGui::Button("DELETE##2"))
				{
					pSoundDesc->strSoundNames.pop_back();
				}

				string strStartEffectIndex = "ANIMINDEX : " + to_string(pSoundDesc->iStartAnimIndex);
				ImGui::Text(strStartEffectIndex.c_str()); ImGui::SameLine();
				string strStartEffectPos = "ANIMPOS : " + to_string(static_cast<_int>(pSoundDesc->fStartAnimPos));
				ImGui::Text(strStartEffectPos.c_str());

				if (ImGui::Button("END"))
				{
					if (pSoundDesc->iEndAnimIndices.size() > 0)
					{
						_uint iEndIndex = static_cast<_uint>(pSoundDesc->iEndAnimIndices.size()) - 1;
						pSoundDesc->iEndAnimIndices[iEndIndex] = m_pPlayer->Get_CurrentModel()->Get_CurrentAnimationIndex();
						_uint iCurrentAnimPos = static_cast<_uint>(m_pPlayer->Get_CurrentAnim()->Get_CurrentAnimPos());
						pSoundDesc->fEndAnimPoses[iEndIndex] = static_cast<_float>(iCurrentAnimPos);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("ADD##3"))
				{
					pSoundDesc->iEndAnimIndices.push_back(-1);
					_uint iEndAnimPos = static_cast<_uint>(0.f);
					pSoundDesc->fEndAnimPoses.push_back(static_cast<_float>(iEndAnimPos));
				}
				ImGui::SameLine();
				if (ImGui::Button("DELETE##3"))
				{
					if (pSoundDesc->iEndAnimIndices.size() > 1)
					{
						pSoundDesc->iEndAnimIndices.pop_back();
						pSoundDesc->fEndAnimPoses.pop_back();
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("INIT"))
				{
					if (pSoundDesc->iEndAnimIndices.size() == 1)
					{
						pSoundDesc->iEndAnimIndices[0] = -1;
						_uint iEndAnimPos = static_cast<_uint>(0.f);
						pSoundDesc->fEndAnimPoses[0] = static_cast<_float>(iEndAnimPos);
					}
				}

				for (size_t i = 0; i < pSoundDesc->iEndAnimIndices.size(); i++)
				{
					string strEndEffectIndex = "ANIMINDEX  : " + to_string(pSoundDesc->iEndAnimIndices[i]);
					ImGui::Text(strEndEffectIndex.c_str()); ImGui::SameLine();
					string strEndEffectPos = "ANIMPOS : " + to_string(static_cast<_int>(pSoundDesc->fEndAnimPoses[i]));
					ImGui::Text(strEndEffectPos.c_str());
				}

				ImGui::PopItemWidth();
				ImGui::PushItemWidth(100.f);

				ImGui::InputFloat("FADEOUT SECOND##1", &pSoundDesc->fFadeoutSecond, 0.05f, 0.f, "%.2f");
				if (pSoundDesc->fFadeoutSecond <= 0.f)
				{
					pSoundDesc->fFadeoutSecond = 0.1f;
				}

				ImGui::PopItemWidth();

				ImGui::SeparatorText("SOUND");
				ImGui::PushItemWidth(100.f);

				if(ImGui::InputFloat("VOLUME##1", &pSoundDesc->fInitVolume, 0.05f, 0.f, "%.2f"))
				{
					if (pSoundDesc->iChannel != -1)
					{
						m_pGameInstance->Set_ChannelVolume(pSoundDesc->iChannel, pSoundDesc->fInitVolume);
					}
				}
				if (pSoundDesc->fInitVolume >= 1.f)
				{
					pSoundDesc->fInitVolume = 1.f;
				}
				else if (pSoundDesc->fInitVolume <= 0.f)
				{
					pSoundDesc->fInitVolume = 0.f;
				}

				ImGui::InputFloat("STARTPOS##1", &pSoundDesc->fStartPosRatio, 0.05f, 0.f, "%.2f");
				if (pSoundDesc->fStartPosRatio >= 1.f)
				{
					pSoundDesc->fStartPosRatio = 1.f;
				}
				else if (pSoundDesc->fStartPosRatio <= 0.f)
				{
					pSoundDesc->fStartPosRatio = 0.f;
				}

				if (ImGui::Button("FADEIN SOUND##1"))
				{
					if (pSoundDesc->IsFadeinSound == false)
					{
						pSoundDesc->IsFadeinSound = true;
					}
					else if (pSoundDesc->IsFadeinSound == true)
					{
						pSoundDesc->IsFadeinSound = false;
					}
				}
				ImGui::SameLine();
				if (pSoundDesc->IsFadeinSound)
				{
					ImGui::Text("TRUE");
				}
				else
				{
					ImGui::Text("FALSE");
				}

				ImGui::InputFloat("FADEIN SECOND##1", &pSoundDesc->fFadeinSecond, 0.05f, 0.f, "%.2f");
				if (pSoundDesc->fFadeinSecond <= 0.f)
				{
					pSoundDesc->fFadeinSecond = 0.1f;
				}

				ImGui::SeparatorText("SOUND");
				if (ImGui::Button("CLIENT TRIGGER"))
				{
					if (pSoundDesc->IsClientTrigger == false)
					{
						pSoundDesc->IsClientTrigger = true;
					}
					else if (pSoundDesc->IsClientTrigger == true)
					{
						pSoundDesc->IsClientTrigger = false;
					}
				}
				ImGui::SameLine();
				if (pSoundDesc->IsClientTrigger)
				{
					ImGui::Text("TRUE");
				}
				else
				{
					ImGui::Text("FALSE");
				}

				ImGui::SeparatorText("CHANNEL");
				string strChannel = "CHANNEL : " + to_string(pSoundDesc->iChannel);
				ImGui::Text(strChannel.c_str());

				//사운드 다시 듣기
				if (m_pGameInstance->Key_Down(DIK_V))
				{
					if (pSoundDesc->iChannel != -1)
					{
						m_pGameInstance->StopSound(pSoundDesc->iChannel);
						pSoundDesc->iChannel = -1;
					}
					pSoundDesc->iChannel = m_pGameInstance->Play_Sound(pSoundDesc->strSoundNames[0], pSoundDesc->fInitVolume, false, pSoundDesc->fStartPosRatio);
					if (pSoundDesc->IsFadeinSound)
					{
						m_pGameInstance->FadeinSound(pSoundDesc->iChannel, 0.016f, pSoundDesc->fFadeinSecond);
					}
				}

				ImGui::PopItemWidth();
				ImGui::End();
			}
#pragma endregion
		}
	}

	ImGui::Begin("TRIGGER DATAFILE");
	ImGui::PushItemWidth(250.f);

	if (m_eTriggerType == TRIGGER_EFFECT)
	{
		if (ImGui::ListBox("EFFECT##2", &m_iSelectFile, m_szEffectFiles, m_EffectFiles.size()))
		{
		}
	}
	else if (m_eTriggerType == TRIGGER_SOUND)
	{
		if (ImGui::ListBox("SOUND##3", &m_iSelectFile, m_szSoundFiles, m_SoundFiles.size()))
		{
			if (m_pPlayer)
			{
#pragma region Player
				if (m_eModelType == TYPE_PLAYER)
				{
					if (m_pPlayer->Get_CurrentPlayerModel()->Get_NumTriggerSound() > 0)
					{
						TRIGGERSOUND_DESC* pSoundDesc = m_pPlayer->Get_CurrentPlayerModel()->Get_TriggerSound(m_iCurTriggerIndex);
						if (pSoundDesc->iChannel != -1)
						{
							m_pGameInstance->StopSound(pSoundDesc->iChannel);
						}

						_uint iSelectEffectFile = m_iSelectFile;
						_tchar szEffectName[MAX_PATH]{};
						MultiByteToWideChar(CP_UTF8, 0, m_szSoundFiles[iSelectEffectFile], (_int)strlen(m_szSoundFiles[iSelectEffectFile]), szEffectName, MAX_PATH);

						if (m_iCurSoundNameIndex != 0)
						{
							pSoundDesc->strSoundNames[m_iCurSoundNameIndex] = szEffectName;
						}
					}
				}
#pragma endregion
				else
				{
					if (m_pPlayer->Get_CurrentModel()->Get_NumTriggerSound() > 0)
					{
						TRIGGERSOUND_DESC* pSoundDesc = m_pPlayer->Get_CurrentModel()->Get_TriggerSound(m_iCurTriggerIndex);
						if (pSoundDesc->iChannel != -1)
						{
							m_pGameInstance->StopSound(pSoundDesc->iChannel);
						}

						_uint iSelectEffectFile = m_iSelectFile;
						_tchar szEffectName[MAX_PATH]{};
						MultiByteToWideChar(CP_UTF8, 0, m_szSoundFiles[iSelectEffectFile], (_int)strlen(m_szSoundFiles[iSelectEffectFile]), szEffectName, MAX_PATH);

						if (m_iCurSoundNameIndex != 0)
						{
							pSoundDesc->strSoundNames[m_iCurSoundNameIndex] = szEffectName;
						}
					}
				}
			}
		}
	}

	ImGui::PopItemWidth();
	ImGui::End();

	return S_OK;
}

HRESULT CImgui_Manager::ImGuizmoMenu()
{
	_mat ViewMatrix = m_pGameInstance->Get_Transform(TransformType::View);
	_mat ProjMatrix = m_pGameInstance->Get_Transform(TransformType::Proj);

	if (m_pGameInstance->Key_Down(DIK_1))
	{
		m_eSelect = SELECT_PLAYER;
	}
	else if (m_pGameInstance->Key_Down(DIK_2))
	{
		m_eSelect = SELECT_EFFECT;
	}

	if (m_pPlayer && m_eSelect == SELECT_PLAYER)
	{
		CTransform* pTargetTransform = (CTransform*)(m_pPlayer->Find_Component(TEXT("Com_Transform")));
		if (pTargetTransform != nullptr)
		{
			_mat TargetMatrix = pTargetTransform->Get_World_Matrix();
			_mat PreMatrix = pTargetTransform->Get_World_Matrix();
			ImGuizmo::Manipulate(&ViewMatrix.m[0][0], &ProjMatrix.m[0][0], m_eStateType, ImGuizmo::MODE::WORLD, &TargetMatrix.m[0][0]);
			if (ImGuizmo::IsUsing())
			{
				if (m_eStateType == ImGuizmo::OPERATION::SCALE)
				{
					m_vCurrentScale = _vec3(TargetMatrix.Right().Length(),
						TargetMatrix.Up().Length(),
						TargetMatrix.Backward().Length());

					_vec4 Right = PreMatrix.Right();
					Right.Normalize();
					_vec4 Up = PreMatrix.Up();
					Up.Normalize();
					_vec4 Look = PreMatrix.Look();
					Look.Normalize();

					pTargetTransform->Set_State(State::Right, Right * m_vCurrentScale.x);
					pTargetTransform->Set_State(State::Up, Up * m_vCurrentScale.y);
					pTargetTransform->Set_State(State::Look, Look * m_vCurrentScale.z);
				}
				else
				{
					pTargetTransform->Set_State(State::Right, TargetMatrix.Right());
					pTargetTransform->Set_State(State::Up, TargetMatrix.Up());
					pTargetTransform->Set_State(State::Look, TargetMatrix.Look());
					pTargetTransform->Set_State(State::Pos, TargetMatrix.Position());
				}
			}
		}
	}

	if (m_pPlayer)
	{
		if (m_eModelType == TYPE_PLAYER)
		{
			CRealtimeVTFModel* pCurModel = m_pPlayer->Get_CurrentPlayerModel();
			if (pCurModel->Get_NumTriggerEffect() != 0)
			{
				if (m_eSelect == SELECT_EFFECT && m_eTriggerType == TRIGGER_EFFECT)
				{
					TRIGGEREFFECT_DESC* pEffectDesc = pCurModel->Get_TriggerEffect(m_iCurTriggerIndex);
					_mat TargetMatrix = pEffectDesc->OffsetMatrix;
					_mat PreMatrix = pEffectDesc->OffsetMatrix;

					ImGuizmo::Manipulate(&ViewMatrix.m[0][0], &ProjMatrix.m[0][0], m_eStateType, ImGuizmo::MODE::WORLD, &TargetMatrix.m[0][0]);
					if (ImGuizmo::IsUsing())
					{
						if (m_eStateType == ImGuizmo::OPERATION::SCALE)
						{
							m_vCurrentScale = _vec3(TargetMatrix.Right().Length(),
								TargetMatrix.Up().Length(),
								TargetMatrix.Backward().Length());

							_vec4 Right = PreMatrix.Right();
							Right.Normalize();
							_vec4 Up = PreMatrix.Up();
							Up.Normalize();
							_vec4 Look = PreMatrix.Look();
							Look.Normalize();

							pEffectDesc->OffsetMatrix.Right(Right * m_vCurrentScale.x);
							pEffectDesc->OffsetMatrix.Up(Up * m_vCurrentScale.y);
							pEffectDesc->OffsetMatrix.Look(Look * m_vCurrentScale.z);
						}
						else
						{
							pEffectDesc->OffsetMatrix.Right(TargetMatrix.Right());
							pEffectDesc->OffsetMatrix.Up(TargetMatrix.Up());
							pEffectDesc->OffsetMatrix.Look(TargetMatrix.Look());
							pEffectDesc->OffsetMatrix.Position(TargetMatrix.Position());
						}
					}
				}
			}
		}
		else
		{
			CModel* pCurModel = m_pPlayer->Get_CurrentModel();
			if (pCurModel->Get_NumTriggerEffect() != 0)
			{
				if (m_eSelect == SELECT_EFFECT && m_eTriggerType == TRIGGER_EFFECT)
				{
					TRIGGEREFFECT_DESC* pEffectDesc = pCurModel->Get_TriggerEffect(m_iCurTriggerIndex);
					_mat TargetMatrix = pEffectDesc->OffsetMatrix;
					_mat PreMatrix = pEffectDesc->OffsetMatrix;

					ImGuizmo::Manipulate(&ViewMatrix.m[0][0], &ProjMatrix.m[0][0], m_eStateType, ImGuizmo::MODE::WORLD, &TargetMatrix.m[0][0]);
					if (ImGuizmo::IsUsing())
					{
						if (m_eStateType == ImGuizmo::OPERATION::SCALE)
						{
							m_vCurrentScale = _vec3(TargetMatrix.Right().Length(),
								TargetMatrix.Up().Length(),
								TargetMatrix.Backward().Length());

							_vec4 Right = PreMatrix.Right();
							Right.Normalize();
							_vec4 Up = PreMatrix.Up();
							Up.Normalize();
							_vec4 Look = PreMatrix.Look();
							Look.Normalize();

							pEffectDesc->OffsetMatrix.Right(Right * m_vCurrentScale.x);
							pEffectDesc->OffsetMatrix.Up(Up * m_vCurrentScale.y);
							pEffectDesc->OffsetMatrix.Look(Look * m_vCurrentScale.z);
						}
						else
						{
							pEffectDesc->OffsetMatrix.Right(TargetMatrix.Right());
							pEffectDesc->OffsetMatrix.Up(TargetMatrix.Up());
							pEffectDesc->OffsetMatrix.Look(TargetMatrix.Look());
							pEffectDesc->OffsetMatrix.Position(TargetMatrix.Position());
						}
					}
				}
			}
		}
	}

	return S_OK;
}


HRESULT CImgui_Manager::SaveFile()
{
	CModel* pCurrentModel = m_pPlayer->Get_CurrentModel();
	if (not pCurrentModel)
	{
		return S_OK;
	}
	_char szFilePath[MAX_PATH] = "";
	_char szDirectory[MAX_PATH] = "";
	_char szFileName[MAX_PATH] = "";
	_splitpath_s(pCurrentModel->Get_FilePath(), nullptr, 0, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

	_char szEffectExt[MAX_PATH] = ".effecttrigger";
	_char szSoundExt[MAX_PATH] = ".soundtrigger";

	_char szPlayerFilePath[MAX_PATH] = "../../Client/Bin/Resources/AnimMesh/VTFPlayer/Main/basemodel";
	if (m_eModelType == TYPE_PLAYER)
	{
		strcpy_s(szFilePath, MAX_PATH, szPlayerFilePath);
	}
	else
	{
		strcpy_s(szFilePath, MAX_PATH, szDirectory);
		strcat_s(szFilePath, MAX_PATH, szFileName);
	}

	if (m_eTriggerType == TRIGGER_EFFECT)
	{
		strcat_s(szFilePath, MAX_PATH, szEffectExt);
	}
	else if (m_eTriggerType == TRIGGER_SOUND)
	{
		strcat_s(szFilePath, MAX_PATH, szSoundExt);
	}

	ofstream Fileout(szFilePath, ios::binary);

	if (Fileout.is_open())
	{
#pragma region Player
		if (m_eModelType == TYPE_PLAYER)
		{
			CRealtimeVTFModel* pCurModel = m_pPlayer->Get_CurrentPlayerModel();
			//이펙트 트리거 저장
			if (m_eTriggerType == TRIGGER_EFFECT)
			{
				vector<TRIGGEREFFECT_DESC> EffectDescs = pCurModel->Get_TriggerEffects();
				_uint iNumTriggerEffect = EffectDescs.size();
				Fileout.write(reinterpret_cast<_char*>(&iNumTriggerEffect), sizeof(_uint));
				for (_uint i = 0; i < iNumTriggerEffect; i++)
				{
					_int iStartAnimIndex = EffectDescs[i].iStartAnimIndex;
					Fileout.write(reinterpret_cast<_char*>(&iStartAnimIndex), sizeof(_int));
					_float fStartAnimPos = EffectDescs[i].fStartAnimPos;
					Fileout.write(reinterpret_cast<_char*>(&fStartAnimPos), sizeof(_float));

					_uint iNumEnd = static_cast<_uint>(EffectDescs[i].iEndAnimIndices.size());
					Fileout.write(reinterpret_cast<_char*>(&iNumEnd), sizeof(_uint));
					for (_uint j = 0; j < iNumEnd; j++)
					{
						_int iEndAnimIndex = EffectDescs[i].iEndAnimIndices[j];
						Fileout.write(reinterpret_cast<_char*>(&iEndAnimIndex), sizeof(_int));
						_float fEndAnimPos = EffectDescs[i].fEndAnimPoses[j];
						Fileout.write(reinterpret_cast<_char*>(&fEndAnimPos), sizeof(_float));
					}
					_bool IsFollow = EffectDescs[i].IsFollow;
					Fileout.write(reinterpret_cast<_char*>(&IsFollow), sizeof(_bool));

					size_t iNameSize{};
					iNameSize = (EffectDescs[i].strEffectName.size() + 1) * sizeof(_tchar);
					Fileout.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
					Fileout.write(reinterpret_cast<const _char*>(EffectDescs[i].strEffectName.data()), iNameSize);

					_uint iBoneIndex = EffectDescs[i].iBoneIndex;
					Fileout.write(reinterpret_cast<_char*>(&iBoneIndex), sizeof(_uint));
					EffectDescs[i].OffsetMatrix._44 = 1.f;
					_mat OffsetMatrix = EffectDescs[i].OffsetMatrix;
					Fileout.write(reinterpret_cast<_char*>(&OffsetMatrix), sizeof(_mat));
					_bool IsDeleteRotateToBone = EffectDescs[i].IsDeleteRotateToBone;
					Fileout.write(reinterpret_cast<_char*>(&IsDeleteRotateToBone), sizeof(_bool));
					_bool IsClientTrigger = EffectDescs[i].IsClientTrigger;
					Fileout.write(reinterpret_cast<_char*>(&IsClientTrigger), sizeof(_bool));
				}
			}//사운드 트리거 저장
			else if (m_eTriggerType == TRIGGER_SOUND)
			{
				vector<TRIGGERSOUND_DESC> SoundDescs = pCurModel->Get_TriggerSounds();
				_uint iNumTriggerSound = SoundDescs.size();
				Fileout.write(reinterpret_cast<_char*>(&iNumTriggerSound), sizeof(_uint));
				for (_uint i = 0; i < iNumTriggerSound; i++)
				{
					_int iStartAnimIndex = SoundDescs[i].iStartAnimIndex;
					Fileout.write(reinterpret_cast<_char*>(&iStartAnimIndex), sizeof(_int));
					_float fStartAnimPos = SoundDescs[i].fStartAnimPos;
					Fileout.write(reinterpret_cast<_char*>(&fStartAnimPos), sizeof(_float));

					_uint iNumEnd = static_cast<_uint>(SoundDescs[i].iEndAnimIndices.size());
					Fileout.write(reinterpret_cast<_char*>(&iNumEnd), sizeof(_uint));
					for (_uint j = 0; j < iNumEnd; j++)
					{
						_int iEndAnimIndex = SoundDescs[i].iEndAnimIndices[j];
						Fileout.write(reinterpret_cast<_char*>(&iEndAnimIndex), sizeof(_int));
						_float fEndAnimPos = SoundDescs[i].fEndAnimPoses[j];
						Fileout.write(reinterpret_cast<_char*>(&fEndAnimPos), sizeof(_float));
					}

					_uint iNumName = static_cast<_uint>(SoundDescs[i].strSoundNames.size());
					Fileout.write(reinterpret_cast<_char*>(&iNumName), sizeof(_uint));
					for (_uint j = 0; j < iNumName; j++)
					{
						size_t iNameSize{};
						iNameSize = (SoundDescs[i].strSoundNames[j].size() + 1) * sizeof(_tchar);
						Fileout.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
						Fileout.write(reinterpret_cast<const _char*>(SoundDescs[i].strSoundNames[j].data()), iNameSize);
					}

					_float fInitVolume = SoundDescs[i].fInitVolume;
					Fileout.write(reinterpret_cast<_char*>(&fInitVolume), sizeof(_float));
					_float fFadeoutSecond = SoundDescs[i].fFadeoutSecond;
					Fileout.write(reinterpret_cast<_char*>(&fFadeoutSecond), sizeof(_float));
					_bool IsClientTrigger = SoundDescs[i].IsClientTrigger;
					Fileout.write(reinterpret_cast<_char*>(&IsClientTrigger), sizeof(_bool));
					_float fStartPosRatio = SoundDescs[i].fStartPosRatio;
					Fileout.write(reinterpret_cast<_char*>(&fStartPosRatio), sizeof(_float));
					_bool IsFadeinSound = SoundDescs[i].IsFadeinSound;
					Fileout.write(reinterpret_cast<_char*>(&IsFadeinSound), sizeof(_bool));
					_float fFadeinSecond = SoundDescs[i].fFadeinSecond;
					Fileout.write(reinterpret_cast<_char*>(&fFadeinSecond), sizeof(_float));
				}
			}
		}
#pragma endregion
		else
		{
			CModel* pCurModel = m_pPlayer->Get_CurrentModel();
			//이펙트 트리거 저장
			if (m_eTriggerType == TRIGGER_EFFECT)
			{
				vector<TRIGGEREFFECT_DESC> EffectDescs = pCurModel->Get_TriggerEffects();
				_uint iNumTriggerEffect = EffectDescs.size();
				Fileout.write(reinterpret_cast<_char*>(&iNumTriggerEffect), sizeof(_uint));
				for (_uint i = 0; i < iNumTriggerEffect; i++)
				{
					_int iStartAnimIndex = EffectDescs[i].iStartAnimIndex;
					Fileout.write(reinterpret_cast<_char*>(&iStartAnimIndex), sizeof(_int));
					_float fStartAnimPos = EffectDescs[i].fStartAnimPos;
					Fileout.write(reinterpret_cast<_char*>(&fStartAnimPos), sizeof(_float));

					_uint iNumEnd = static_cast<_uint>(EffectDescs[i].iEndAnimIndices.size());
					Fileout.write(reinterpret_cast<_char*>(&iNumEnd), sizeof(_uint));
					for (_uint j = 0; j < iNumEnd; j++)
					{
						_int iEndAnimIndex = EffectDescs[i].iEndAnimIndices[j];
						Fileout.write(reinterpret_cast<_char*>(&iEndAnimIndex), sizeof(_int));
						_float fEndAnimPos = EffectDescs[i].fEndAnimPoses[j];
						Fileout.write(reinterpret_cast<_char*>(&fEndAnimPos), sizeof(_float));
					}
					_bool IsFollow = EffectDescs[i].IsFollow;
					Fileout.write(reinterpret_cast<_char*>(&IsFollow), sizeof(_bool));

					size_t iNameSize{};
					iNameSize = (EffectDescs[i].strEffectName.size() + 1) * sizeof(_tchar);
					Fileout.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
					Fileout.write(reinterpret_cast<const _char*>(EffectDescs[i].strEffectName.data()), iNameSize);

					_uint iBoneIndex = EffectDescs[i].iBoneIndex;
					Fileout.write(reinterpret_cast<_char*>(&iBoneIndex), sizeof(_uint));
					EffectDescs[i].OffsetMatrix._44 = 1.f;
					_mat OffsetMatrix = EffectDescs[i].OffsetMatrix;
					Fileout.write(reinterpret_cast<_char*>(&OffsetMatrix), sizeof(_mat));
					_bool IsDeleteRotateToBone = EffectDescs[i].IsDeleteRotateToBone;
					Fileout.write(reinterpret_cast<_char*>(&IsDeleteRotateToBone), sizeof(_bool));
					_bool IsClientTrigger = EffectDescs[i].IsClientTrigger;
					Fileout.write(reinterpret_cast<_char*>(&IsClientTrigger), sizeof(_bool));
				}
			}//사운드 트리거 저장
			else if (m_eTriggerType == TRIGGER_SOUND)
			{
				vector<TRIGGERSOUND_DESC> SoundDescs = pCurModel->Get_TriggerSounds();
				_uint iNumTriggerSound = SoundDescs.size();
				Fileout.write(reinterpret_cast<_char*>(&iNumTriggerSound), sizeof(_uint));
				for (_uint i = 0; i < iNumTriggerSound; i++)
				{
					_int iStartAnimIndex = SoundDescs[i].iStartAnimIndex;
					Fileout.write(reinterpret_cast<_char*>(&iStartAnimIndex), sizeof(_int));
					_float fStartAnimPos = SoundDescs[i].fStartAnimPos;
					Fileout.write(reinterpret_cast<_char*>(&fStartAnimPos), sizeof(_float));

					_uint iNumEnd = static_cast<_uint>(SoundDescs[i].iEndAnimIndices.size());
					Fileout.write(reinterpret_cast<_char*>(&iNumEnd), sizeof(_uint));
					for (_uint j = 0; j < iNumEnd; j++)
					{
						_int iEndAnimIndex = SoundDescs[i].iEndAnimIndices[j];
						Fileout.write(reinterpret_cast<_char*>(&iEndAnimIndex), sizeof(_int));
						_float fEndAnimPos = SoundDescs[i].fEndAnimPoses[j];
						Fileout.write(reinterpret_cast<_char*>(&fEndAnimPos), sizeof(_float));
					}

					_uint iNumName = static_cast<_uint>(SoundDescs[i].strSoundNames.size());
					Fileout.write(reinterpret_cast<_char*>(&iNumName), sizeof(_uint));
					for (_uint j = 0; j < iNumName; j++)
					{
						size_t iNameSize{};
						iNameSize = (SoundDescs[i].strSoundNames[j].size() + 1) * sizeof(_tchar);
						Fileout.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
						Fileout.write(reinterpret_cast<const _char*>(SoundDescs[i].strSoundNames[j].data()), iNameSize);
					}

					_float fInitVolume = SoundDescs[i].fInitVolume;
					Fileout.write(reinterpret_cast<_char*>(&fInitVolume), sizeof(_float));
					_float fFadeoutSecond = SoundDescs[i].fFadeoutSecond;
					Fileout.write(reinterpret_cast<_char*>(&fFadeoutSecond), sizeof(_float));
					_bool IsClientTrigger = SoundDescs[i].IsClientTrigger;
					Fileout.write(reinterpret_cast<_char*>(&IsClientTrigger), sizeof(_bool));
					_float fStartPosRatio = SoundDescs[i].fStartPosRatio;
					Fileout.write(reinterpret_cast<_char*>(&fStartPosRatio), sizeof(_float));
					_bool IsFadeinSound = SoundDescs[i].IsFadeinSound;
					Fileout.write(reinterpret_cast<_char*>(&IsFadeinSound), sizeof(_bool));
					_float fFadeinSecond = SoundDescs[i].fFadeinSecond;
					Fileout.write(reinterpret_cast<_char*>(&fFadeinSecond), sizeof(_float));
				}
			}
		}
		
		MessageBox(g_hWnd, L"파일 저장 완료", L"파일 저장", MB_OK);
	}

	return S_OK;
}

HRESULT CImgui_Manager::LoadFile()
{
	CModel* pCurrentModel = m_pPlayer->Get_CurrentModel();
	if (not pCurrentModel)
	{
		return S_OK;
	}

	_char szFilePath[MAX_PATH] = "";
	_char szDirectory[MAX_PATH] = "";
	_char szFileName[MAX_PATH] = "";
	_splitpath_s(pCurrentModel->Get_FilePath(), nullptr, 0, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

	_char szEffectExt[MAX_PATH] = ".effecttrigger";
	_char szSoundExt[MAX_PATH] = ".soundtrigger";

	_char szPlayerFilePath[MAX_PATH] = "../../Client/Bin/Resources/AnimMesh/VTFPlayer/Main/basemodel";
	if (m_eModelType == TYPE_PLAYER)
	{
		strcpy_s(szFilePath, MAX_PATH, szPlayerFilePath);
	}
	else
	{
		strcpy_s(szFilePath, MAX_PATH, szDirectory);
		strcat_s(szFilePath, MAX_PATH, szFileName);
	}

	if (m_eTriggerType == TRIGGER_EFFECT)
	{
		strcat_s(szFilePath, MAX_PATH, szEffectExt);
	}
	else if (m_eTriggerType == TRIGGER_SOUND)
	{
		strcat_s(szFilePath, MAX_PATH, szSoundExt);
	}

	ifstream Filein(szFilePath, ios::binary);

	if (Filein.is_open())
	{
#pragma region Player
		if (m_eModelType == TYPE_PLAYER)
		{
			CRealtimeVTFModel* pCurModel = m_pPlayer->Get_CurrentPlayerModel();
			//이펙트 트리거 불러오기
			if (m_eTriggerType == TRIGGER_EFFECT)
			{
				pCurModel->Reset_TriggerEffects();

				_uint iNumTriggerEffect = { 0 };
				Filein.read(reinterpret_cast<char*>(&iNumTriggerEffect), sizeof _uint);
				for (_uint i = 0; i < iNumTriggerEffect; i++)
				{
					TRIGGEREFFECT_DESC EffectDesc{};
					Filein.read(reinterpret_cast<_char*>(&EffectDesc.iStartAnimIndex), sizeof(_int));
					Filein.read(reinterpret_cast<_char*>(&EffectDesc.fStartAnimPos), sizeof(_float));

					_uint iNumEnd{};
					Filein.read(reinterpret_cast<_char*>(&iNumEnd), sizeof(_uint));
					for (_uint j = 0; j < iNumEnd; j++)
					{
						_int iEndAnimIndex{};
						Filein.read(reinterpret_cast<_char*>(&iEndAnimIndex), sizeof(_int));
						EffectDesc.iEndAnimIndices.push_back(iEndAnimIndex);
						_float fEndAnimPos{};
						Filein.read(reinterpret_cast<_char*>(&fEndAnimPos), sizeof(_float));
						EffectDesc.fEndAnimPoses.push_back(fEndAnimPos);
					}
					Filein.read(reinterpret_cast<_char*>(&EffectDesc.IsFollow), sizeof(_bool));

					size_t iNameSize{};
					_tchar* pBuffer{};
					Filein.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
					Filein.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
					EffectDesc.strEffectName = pBuffer;
					Safe_Delete_Array(pBuffer);

					Filein.read(reinterpret_cast<_char*>(&EffectDesc.iBoneIndex), sizeof(_uint));
					Filein.read(reinterpret_cast<_char*>(&EffectDesc.OffsetMatrix), sizeof(_mat));
					Filein.read(reinterpret_cast<_char*>(&EffectDesc.IsDeleteRotateToBone), sizeof(_bool));
					Filein.read(reinterpret_cast<_char*>(&EffectDesc.IsClientTrigger), sizeof(_bool));

					pCurModel->Add_TriggerEffect(EffectDesc);
				}
			}//사운드 트리거 불러오기
			else if (m_eTriggerType == TRIGGER_SOUND)
			{
				pCurModel->Reset_TriggerSounds();

				_uint iNumTriggerSound = { 0 };
				Filein.read(reinterpret_cast<char*>(&iNumTriggerSound), sizeof _uint);
				for (_uint i = 0; i < iNumTriggerSound; i++)
				{
					TRIGGERSOUND_DESC SoundDesc{};

					Filein.read(reinterpret_cast<_char*>(&SoundDesc.iStartAnimIndex), sizeof(_int));
					Filein.read(reinterpret_cast<_char*>(&SoundDesc.fStartAnimPos), sizeof(_float));

					_uint iNumEnd{};
					Filein.read(reinterpret_cast<_char*>(&iNumEnd), sizeof(_uint));
					for (_uint j = 0; j < iNumEnd; j++)
					{
						_int iEndAnimIndex{};
						Filein.read(reinterpret_cast<_char*>(&iEndAnimIndex), sizeof(_int));
						SoundDesc.iEndAnimIndices.push_back(iEndAnimIndex);
						_float fEndAnimPos{};
						Filein.read(reinterpret_cast<_char*>(&fEndAnimPos), sizeof(_float));
						SoundDesc.fEndAnimPoses.push_back(fEndAnimPos);
					}

					_uint iNumName{};
					Filein.read(reinterpret_cast<_char*>(&iNumName), sizeof(_uint));
					for (_uint i = 0; i < iNumName; i++)
					{
						size_t iNameSize{};
						_tchar* pBuffer{};
						Filein.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
						pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
						Filein.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
						SoundDesc.strSoundNames.push_back(pBuffer);
						Safe_Delete_Array(pBuffer);
					}

					Filein.read(reinterpret_cast<_char*>(&SoundDesc.fInitVolume), sizeof(_float));
					Filein.read(reinterpret_cast<_char*>(&SoundDesc.fFadeoutSecond), sizeof(_float));
					Filein.read(reinterpret_cast<_char*>(&SoundDesc.IsClientTrigger), sizeof(_bool));
					Filein.read(reinterpret_cast<_char*>(&SoundDesc.fStartPosRatio), sizeof(_float));
					Filein.read(reinterpret_cast<_char*>(&SoundDesc.IsFadeinSound), sizeof(_bool));
					Filein.read(reinterpret_cast<_char*>(&SoundDesc.fFadeinSecond), sizeof(_float));

					pCurModel->Add_TriggerSound(SoundDesc);
				}
			}
		}
#pragma endregion
		else
		{
			CModel* pCurModel = m_pPlayer->Get_CurrentModel();
			//이펙트 트리거 불러오기
			if (m_eTriggerType == TRIGGER_EFFECT)
			{
				pCurModel->Reset_TriggerEffects();

				_uint iNumTriggerEffect = { 0 };
				Filein.read(reinterpret_cast<char*>(&iNumTriggerEffect), sizeof _uint);
				for (_uint i = 0; i < iNumTriggerEffect; i++)
				{
					TRIGGEREFFECT_DESC EffectDesc{};
					Filein.read(reinterpret_cast<_char*>(&EffectDesc.iStartAnimIndex), sizeof(_int));
					Filein.read(reinterpret_cast<_char*>(&EffectDesc.fStartAnimPos), sizeof(_float));

					_uint iNumEnd{};
					Filein.read(reinterpret_cast<_char*>(&iNumEnd), sizeof(_uint));
					for (_uint j = 0; j < iNumEnd; j++)
					{
						_int iEndAnimIndex{};
						Filein.read(reinterpret_cast<_char*>(&iEndAnimIndex), sizeof(_int));
						EffectDesc.iEndAnimIndices.push_back(iEndAnimIndex);
						_float fEndAnimPos{};
						Filein.read(reinterpret_cast<_char*>(&fEndAnimPos), sizeof(_float));
						EffectDesc.fEndAnimPoses.push_back(fEndAnimPos);
					}
					Filein.read(reinterpret_cast<_char*>(&EffectDesc.IsFollow), sizeof(_bool));

					size_t iNameSize{};
					_tchar* pBuffer{};
					Filein.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
					Filein.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
					EffectDesc.strEffectName = pBuffer;
					Safe_Delete_Array(pBuffer);

					Filein.read(reinterpret_cast<_char*>(&EffectDesc.iBoneIndex), sizeof(_uint));
					Filein.read(reinterpret_cast<_char*>(&EffectDesc.OffsetMatrix), sizeof(_mat));
					Filein.read(reinterpret_cast<_char*>(&EffectDesc.IsDeleteRotateToBone), sizeof(_bool));
					Filein.read(reinterpret_cast<_char*>(&EffectDesc.IsClientTrigger), sizeof(_bool));

					pCurModel->Add_TriggerEffect(EffectDesc);
				}
			}//사운드 트리거 불러오기
			else if (m_eTriggerType == TRIGGER_SOUND)
			{
				pCurModel->Reset_TriggerSounds();

				_uint iNumTriggerSound = { 0 };
				Filein.read(reinterpret_cast<char*>(&iNumTriggerSound), sizeof _uint);
				for (_uint i = 0; i < iNumTriggerSound; i++)
				{
					TRIGGERSOUND_DESC SoundDesc{};

					Filein.read(reinterpret_cast<_char*>(&SoundDesc.iStartAnimIndex), sizeof(_int));
					Filein.read(reinterpret_cast<_char*>(&SoundDesc.fStartAnimPos), sizeof(_float));

					_uint iNumEnd{};
					Filein.read(reinterpret_cast<_char*>(&iNumEnd), sizeof(_uint));
					for (_uint j = 0; j < iNumEnd; j++)
					{
						_int iEndAnimIndex{};
						Filein.read(reinterpret_cast<_char*>(&iEndAnimIndex), sizeof(_int));
						SoundDesc.iEndAnimIndices.push_back(iEndAnimIndex);
						_float fEndAnimPos{};
						Filein.read(reinterpret_cast<_char*>(&fEndAnimPos), sizeof(_float));
						SoundDesc.fEndAnimPoses.push_back(fEndAnimPos);
					}

					_uint iNumName{};
					Filein.read(reinterpret_cast<_char*>(&iNumName), sizeof(_uint));
					for (_uint i = 0; i < iNumName; i++)
					{
						size_t iNameSize{};
						_tchar* pBuffer{};
						Filein.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
						pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
						Filein.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
						SoundDesc.strSoundNames.push_back(pBuffer);
						Safe_Delete_Array(pBuffer);
					}

					Filein.read(reinterpret_cast<_char*>(&SoundDesc.fInitVolume), sizeof(_float));
					Filein.read(reinterpret_cast<_char*>(&SoundDesc.fFadeoutSecond), sizeof(_float));
					Filein.read(reinterpret_cast<_char*>(&SoundDesc.IsClientTrigger), sizeof(_bool));
					Filein.read(reinterpret_cast<_char*>(&SoundDesc.fStartPosRatio), sizeof(_float));
					Filein.read(reinterpret_cast<_char*>(&SoundDesc.IsFadeinSound), sizeof(_bool));
					Filein.read(reinterpret_cast<_char*>(&SoundDesc.fFadeinSecond), sizeof(_float));

					pCurModel->Add_TriggerSound(SoundDesc);
				}
			}
		}

		MessageBox(g_hWnd, L"파일 로드 완료", L"파일 로드", MB_OK);
	}

	return S_OK;
}

HRESULT CImgui_Manager::UpdateFile()
{
	vector<PRE_TRIGGEREFFECT_DESC> PreEffectDescs{};
	vector<PRE_TRIGGERSOUND_DESC> PreSoundDescs{};

	vector<TRIGGEREFFECT_DESC> EffectDescs{};
	vector<TRIGGERSOUND_DESC> SoundDescs{};

	string strInputFilePath = "../../Client/Bin/Resources/AnimMesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (m_eTriggerType == TRIGGER_EFFECT)
			{
				if (entry.path().extension().string() != ".effecttrigger")
				{
					continue;
				}

				PreEffectDescs.clear();
				EffectDescs.clear();

				ifstream Filein(entry.path().c_str(), ios::binary);

				if (Filein.is_open())
				{
					_uint iNumTriggerEffect = { 0 };
					Filein.read(reinterpret_cast<char*>(&iNumTriggerEffect), sizeof _uint);
					for (_uint i = 0; i < iNumTriggerEffect; i++)
					{
						PRE_TRIGGEREFFECT_DESC PreEffectDesc{};

						Filein.read(reinterpret_cast<_char*>(&PreEffectDesc.iStartAnimIndex), sizeof(_int));
						Filein.read(reinterpret_cast<_char*>(&PreEffectDesc.fStartAnimPos), sizeof(_float));

						_uint iNumEnd{};
						Filein.read(reinterpret_cast<_char*>(&iNumEnd), sizeof(_uint));
						for (_uint j = 0; j < iNumEnd; j++)
						{
							_int iEndAnimIndex{};
							Filein.read(reinterpret_cast<_char*>(&iEndAnimIndex), sizeof(_int));
							PreEffectDesc.iEndAnimIndices.push_back(iEndAnimIndex);
							_float fEndAnimPos{};
							Filein.read(reinterpret_cast<_char*>(&fEndAnimPos), sizeof(_float));
							PreEffectDesc.fEndAnimPoses.push_back(fEndAnimPos);
						}
						Filein.read(reinterpret_cast<_char*>(&PreEffectDesc.IsFollow), sizeof(_bool));

						size_t iNameSize{};
						_tchar* pBuffer{};
						Filein.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
						pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
						Filein.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
						PreEffectDesc.strEffectName = pBuffer;
						Safe_Delete_Array(pBuffer);

						Filein.read(reinterpret_cast<_char*>(&PreEffectDesc.iBoneIndex), sizeof(_uint));
						Filein.read(reinterpret_cast<_char*>(&PreEffectDesc.OffsetMatrix), sizeof(_mat));
						Filein.read(reinterpret_cast<_char*>(&PreEffectDesc.IsDeleteRotateToBone), sizeof(_bool));
						Filein.read(reinterpret_cast<_char*>(&PreEffectDesc.IsClientTrigger), sizeof(_bool));

						PreEffectDescs.push_back(PreEffectDesc);
					}

					Filein.close();
				}

				for (size_t i = 0; i < PreEffectDescs.size(); i++)
				{
					TRIGGEREFFECT_DESC EffectDesc{};

					EffectDesc.iStartAnimIndex = PreEffectDescs[i].iStartAnimIndex;
					EffectDesc.fStartAnimPos = PreEffectDescs[i].fStartAnimPos;
					for (size_t j = 0; j < PreEffectDescs[i].iEndAnimIndices.size(); j++)
					{
						EffectDesc.iEndAnimIndices.push_back(PreEffectDescs[i].iEndAnimIndices[j]);
						EffectDesc.fEndAnimPoses.push_back(PreEffectDescs[i].fEndAnimPoses[j]);
					}
					EffectDesc.IsFollow = PreEffectDescs[i].IsFollow;
					EffectDesc.strEffectName = PreEffectDescs[i].strEffectName;
					EffectDesc.iBoneIndex = PreEffectDescs[i].iBoneIndex;
					EffectDesc.OffsetMatrix = PreEffectDescs[i].OffsetMatrix;
					EffectDesc.IsDeleteRotateToBone = PreEffectDescs[i].IsDeleteRotateToBone;
					EffectDesc.IsClientTrigger = PreEffectDescs[i].IsClientTrigger;

					EffectDescs.push_back(EffectDesc);
				}

				filesystem::path strFilePath = entry.path();
				ofstream Fileout(strFilePath.c_str(), ios::binary);

				if (Fileout.is_open())
				{
					_uint iNumTriggerEffect = EffectDescs.size();
					Fileout.write(reinterpret_cast<_char*>(&iNumTriggerEffect), sizeof(_uint));
					for (_uint i = 0; i < iNumTriggerEffect; i++)
					{
						_int iStartAnimIndex = EffectDescs[i].iStartAnimIndex;
						Fileout.write(reinterpret_cast<_char*>(&iStartAnimIndex), sizeof(_int));
						_float fStartAnimPos = EffectDescs[i].fStartAnimPos;
						Fileout.write(reinterpret_cast<_char*>(&fStartAnimPos), sizeof(_float));

						_uint iNumEnd = static_cast<_uint>(EffectDescs[i].iEndAnimIndices.size());
						Fileout.write(reinterpret_cast<_char*>(&iNumEnd), sizeof(_uint));
						for (_uint j = 0; j < iNumEnd; j++)
						{
							_int iEndAnimIndex = EffectDescs[i].iEndAnimIndices[j];
							Fileout.write(reinterpret_cast<_char*>(&iEndAnimIndex), sizeof(_int));
							_float fEndAnimPos = EffectDescs[i].fEndAnimPoses[j];
							Fileout.write(reinterpret_cast<_char*>(&fEndAnimPos), sizeof(_float));
						}
						_bool IsFollow = EffectDescs[i].IsFollow;
						Fileout.write(reinterpret_cast<_char*>(&IsFollow), sizeof(_bool));

						size_t iNameSize{};
						iNameSize = (EffectDescs[i].strEffectName.size() + 1) * sizeof(_tchar);
						Fileout.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
						Fileout.write(reinterpret_cast<const _char*>(EffectDescs[i].strEffectName.data()), iNameSize);

						_uint iBoneIndex = EffectDescs[i].iBoneIndex;
						Fileout.write(reinterpret_cast<_char*>(&iBoneIndex), sizeof(_uint));
						EffectDescs[i].OffsetMatrix._44 = 1.f;
						_mat OffsetMatrix = EffectDescs[i].OffsetMatrix;
						Fileout.write(reinterpret_cast<_char*>(&OffsetMatrix), sizeof(_mat));
						_bool IsDeleteRotateToBone = EffectDescs[i].IsDeleteRotateToBone;
						Fileout.write(reinterpret_cast<_char*>(&IsDeleteRotateToBone), sizeof(_bool));
						_bool IsClientTrigger = EffectDescs[i].IsClientTrigger;
						Fileout.write(reinterpret_cast<_char*>(&IsClientTrigger), sizeof(_bool));
					}

					Fileout.close();
				}
			}
			else if (m_eTriggerType == TRIGGER_SOUND)
			{
				if (entry.path().extension().string() != ".soundtrigger")
				{
					continue;
				}

				PreSoundDescs.clear();
				SoundDescs.clear();

				ifstream Filein(entry.path().c_str(), ios::binary);

				if (Filein.is_open())
				{
					_uint iNumTriggerSound = { 0 };
					Filein.read(reinterpret_cast<char*>(&iNumTriggerSound), sizeof _uint);
					for (_uint i = 0; i < iNumTriggerSound; i++)
					{
						PRE_TRIGGERSOUND_DESC PreSoundDesc{};

						Filein.read(reinterpret_cast<_char*>(&PreSoundDesc.iStartAnimIndex), sizeof(_int));
						Filein.read(reinterpret_cast<_char*>(&PreSoundDesc.fStartAnimPos), sizeof(_float));

						_uint iNumEnd{};
						Filein.read(reinterpret_cast<_char*>(&iNumEnd), sizeof(_uint));
						for (_uint j = 0; j < iNumEnd; j++)
						{
							_int iEndAnimIndex{};
							Filein.read(reinterpret_cast<_char*>(&iEndAnimIndex), sizeof(_int));
							PreSoundDesc.iEndAnimIndices.push_back(iEndAnimIndex);
							_float fEndAnimPos{};
							Filein.read(reinterpret_cast<_char*>(&fEndAnimPos), sizeof(_float));
							PreSoundDesc.fEndAnimPoses.push_back(fEndAnimPos);
						}

						_uint iNumName{};
						Filein.read(reinterpret_cast<_char*>(&iNumName), sizeof(_uint));
						for (_uint i = 0; i < iNumName; i++)
						{
							size_t iNameSize{};
							_tchar* pBuffer{};
							Filein.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
							pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
							Filein.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
							PreSoundDesc.strSoundNames.push_back(pBuffer);
							Safe_Delete_Array(pBuffer);
						}

						Filein.read(reinterpret_cast<_char*>(&PreSoundDesc.fInitVolume), sizeof(_float));
						Filein.read(reinterpret_cast<_char*>(&PreSoundDesc.fFadeoutSecond), sizeof(_float));
						Filein.read(reinterpret_cast<_char*>(&PreSoundDesc.IsClientTrigger), sizeof(_bool));
						Filein.read(reinterpret_cast<_char*>(&PreSoundDesc.fStartPosRatio), sizeof(_float));
						Filein.read(reinterpret_cast<_char*>(&PreSoundDesc.IsFadeinSound), sizeof(_bool));
						Filein.read(reinterpret_cast<_char*>(&PreSoundDesc.fFadeinSecond), sizeof(_float));

						PreSoundDescs.push_back(PreSoundDesc);
					}

					Filein.close();
				}

				for (size_t i = 0; i < PreSoundDescs.size(); i++)
				{
					TRIGGERSOUND_DESC SoundDesc{};

					SoundDesc.iStartAnimIndex = PreSoundDescs[i].iStartAnimIndex;
					SoundDesc.fStartAnimPos = PreSoundDescs[i].fStartAnimPos;
					for (size_t j = 0; j < PreSoundDescs[i].iEndAnimIndices.size(); j++)
					{
						SoundDesc.iEndAnimIndices.push_back(PreSoundDescs[i].iEndAnimIndices[j]);
						SoundDesc.fEndAnimPoses.push_back(PreSoundDescs[i].fEndAnimPoses[j]);
					}
					for (size_t j = 0; j < PreSoundDescs[i].strSoundNames.size(); j++)
					{
						SoundDesc.strSoundNames.push_back(PreSoundDescs[i].strSoundNames[j]);
					}
					SoundDesc.fInitVolume = PreSoundDescs[i].fInitVolume;
					SoundDesc.fFadeoutSecond = PreSoundDescs[i].fFadeoutSecond;
					SoundDesc.IsClientTrigger = PreSoundDescs[i].IsClientTrigger;
					SoundDesc.fStartPosRatio = PreSoundDescs[i].fStartPosRatio;
					SoundDesc.IsFadeinSound = PreSoundDescs[i].IsFadeinSound;
					SoundDesc.fFadeinSecond = PreSoundDescs[i].fFadeinSecond;

					SoundDescs.push_back(SoundDesc);
				}

				filesystem::path strFilePath = entry.path();
				ofstream Fileout(strFilePath.c_str(), ios::binary);

				if (Fileout.is_open())
				{
					_uint iNumTriggerSound = SoundDescs.size();
					Fileout.write(reinterpret_cast<_char*>(&iNumTriggerSound), sizeof(_uint));
					for (_uint i = 0; i < iNumTriggerSound; i++)
					{
						_int iStartAnimIndex = SoundDescs[i].iStartAnimIndex;
						Fileout.write(reinterpret_cast<_char*>(&iStartAnimIndex), sizeof(_int));
						_float fStartAnimPos = SoundDescs[i].fStartAnimPos;
						Fileout.write(reinterpret_cast<_char*>(&fStartAnimPos), sizeof(_float));

						_uint iNumEnd = static_cast<_uint>(SoundDescs[i].iEndAnimIndices.size());
						Fileout.write(reinterpret_cast<_char*>(&iNumEnd), sizeof(_uint));
						for (_uint j = 0; j < iNumEnd; j++)
						{
							_int iEndAnimIndex = SoundDescs[i].iEndAnimIndices[j];
							Fileout.write(reinterpret_cast<_char*>(&iEndAnimIndex), sizeof(_int));
							_float fEndAnimPos = SoundDescs[i].fEndAnimPoses[j];
							Fileout.write(reinterpret_cast<_char*>(&fEndAnimPos), sizeof(_float));
						}

						_uint iNumName = static_cast<_uint>(SoundDescs[i].strSoundNames.size());
						Fileout.write(reinterpret_cast<_char*>(&iNumName), sizeof(_uint));
						for (_uint j = 0; j < iNumName; j++)
						{
							size_t iNameSize{};
							iNameSize = (SoundDescs[i].strSoundNames[j].size() + 1) * sizeof(_tchar);
							Fileout.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
							Fileout.write(reinterpret_cast<const _char*>(SoundDescs[i].strSoundNames[j].data()), iNameSize);
						}

						_float fInitVolume = SoundDescs[i].fInitVolume;
						Fileout.write(reinterpret_cast<_char*>(&fInitVolume), sizeof(_float));
						_float fFadeoutSecond = SoundDescs[i].fFadeoutSecond;
						Fileout.write(reinterpret_cast<_char*>(&fFadeoutSecond), sizeof(_float));
						_bool IsClientTrigger = SoundDescs[i].IsClientTrigger;
						Fileout.write(reinterpret_cast<_char*>(&IsClientTrigger), sizeof(_bool));
						_float fStartPosRatio = SoundDescs[i].fStartPosRatio;
						Fileout.write(reinterpret_cast<_char*>(&fStartPosRatio), sizeof(_float));
						_bool IsFadeinSound = SoundDescs[i].IsFadeinSound;
						Fileout.write(reinterpret_cast<_char*>(&IsFadeinSound), sizeof(_bool));
						_float fFadeinSecond = SoundDescs[i].fFadeinSecond;
						Fileout.write(reinterpret_cast<_char*>(&fFadeinSecond), sizeof(_float));
					}

					Fileout.close();
				}
			}
		}
	}

	MessageBox(g_hWnd, L"파일 저장 완료", L"파일 저장", MB_OK);

	return S_OK;
}

CImgui_Manager* CImgui_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const GRAPHIC_DESC& GraphicDesc)
{
	CImgui_Manager* pInstance = new CImgui_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(GraphicDesc)))
	{
		MSG_BOX("Failed to Created : CImgui_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CImgui_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pPlayer);

	Safe_Delete_Array(m_szMonsterNames);
	Safe_Delete_Array(m_szEffectFiles);
	Safe_Delete_Array(m_szSoundFiles);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
