#include "imgui.h"
#include "ImGuizmo.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGui_Manager.h"
#include "GameInstance.h"

#include "Dummy.h"
#include "Terrain.h"
#include "Map.h"
//#include "CutScene_Curve.h"
#include "Camera_CutScene.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImGui_Manager::Initialize_Prototype(const GRAPHIC_DESC& GraphicDesc)
{
	m_hWnd = GraphicDesc.hWnd;
	m_iWinSizeX = GraphicDesc.iWinSizeX;
	m_iWinSizeY = GraphicDesc.iWinSizeY;

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;

	Search_Map();
	Search_Object();
	Search_Monster();
	Search_NPC();
	Search_Envir();
	Search_Inter();

	//SectionName.reserve(MAX_PATH);
	SectionName.resize(MAX_PATH);

	return S_OK;
}

void CImGui_Manager::Tick(_float fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;

	Mouse_Pos();

	if (m_eItemType == ItemType::Terrain)
	{
		if (m_pTerrain)
		{
			_float fHight;

			if (fHight = m_pGameInstance->Get_MouseMove(MouseState::wheel) > 0)
			{
				TerrainHight += 1.f;
			}
			else if (fHight = m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
			{
				TerrainHight -= 1.f;
			}
			CVIBuffer_Terrain* pBuffer = static_cast<CVIBuffer_Terrain*>(m_pTerrain->Find_Component(TEXT("Com_VIBuffer")));
			Picking_On_Terrain();

			_vec3 CurrentMousePosition = m_TerrainPos;

			if (m_pGameInstance->Mouse_Pressing(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LCONTROL))
			{
				pBuffer->Modify_Terrain_Hight(pBuffer->Get_NumVerticesX(), pBuffer->Get_NumVerticesZ(), CurrentMousePosition, TerrainHight, BrushSize);
			}
			else if (m_pGameInstance->Mouse_Down(DIM_RBUTTON) && m_pGameInstance->Key_Pressing(DIK_LCONTROL))
			{

			}
		}
	}
	else
	{
		if (m_pSelectedDummy)
		{
			CTransform* pObjectsTransform = (CTransform*)m_pSelectedDummy->Find_Component(TEXT("Com_Transform"));

			_vector ObjRight = { m_ObjectMatrix._11, m_ObjectMatrix._12, m_ObjectMatrix._13, m_ObjectMatrix._14 };
			_vector ObjUp = { m_ObjectMatrix._21, m_ObjectMatrix._22, m_ObjectMatrix._23, m_ObjectMatrix._24 };
			_vector ObjLook = { m_ObjectMatrix._31, m_ObjectMatrix._32, m_ObjectMatrix._33, m_ObjectMatrix._34 };
			_vector ObjPosition = { m_ObjectMatrix._41, m_ObjectMatrix._42, m_ObjectMatrix._43, m_ObjectMatrix._44 };
			pObjectsTransform->Set_State(State::Right, ObjRight);
			pObjectsTransform->Set_State(State::Up, ObjUp);
			pObjectsTransform->Set_State(State::Look, ObjLook);
			pObjectsTransform->Set_State(State::Pos, ObjPosition);
		}
		else if (m_pSelectMap)
		{
			CTransform* pMapTransform = (CTransform*)m_pSelectMap->Find_Component(TEXT("Com_Transform"));

			_vector ObjRight = { m_MapMatrix._11, m_MapMatrix._12, m_MapMatrix._13, m_MapMatrix._14 };
			_vector ObjUp = { m_MapMatrix._21, m_MapMatrix._22, m_MapMatrix._23, m_MapMatrix._24 };
			_vector ObjLook = { m_MapMatrix._31, m_MapMatrix._32, m_MapMatrix._33, m_MapMatrix._34 };
			_vector ObjPosition = { m_MapMatrix._41, m_MapMatrix._42, m_MapMatrix._43, m_MapMatrix._44 };
			pMapTransform->Set_State(State::Right, ObjRight);
			pMapTransform->Set_State(State::Up, ObjUp);
			pMapTransform->Set_State(State::Look, ObjLook);
			pMapTransform->Set_State(State::Pos, ObjPosition);
		}
		else if (m_pSelectCamera)
		{
			//CTransform* pMapTransform = (CTransform*)m_pSelectCamera->Find_Component(TEXT("Com_Transform"));

			//_vector ObjRight = { m_CameraMatrix._11, m_CameraMatrix._12, m_CameraMatrix._13, m_CameraMatrix._14 };
			//_vector ObjUp = { m_CameraMatrix._21, m_CameraMatrix._22, m_CameraMatrix._23, m_CameraMatrix._24 };
			//_vector ObjLook = { m_CameraMatrix._31, m_CameraMatrix._32, m_CameraMatrix._33, m_CameraMatrix._34 };
			//_vector ObjPosition = { m_CameraMatrix._41, m_CameraMatrix._42, m_CameraMatrix._43, m_CameraMatrix._44 };
			//pMapTransform->Set_State(State::Right, ObjRight);
			//pMapTransform->Set_State(State::Up, ObjUp);
			//pMapTransform->Set_State(State::Look, ObjLook);
			//pMapTransform->Set_State(State::Pos, ObjPosition);
		}

		if (m_eItemType != ItemType::Environment && m_eItemType != ItemType::Camera)
		{
			if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LCONTROL))
			{
			
				if ((m_vMousePos.x >= 0.f && m_vMousePos.x < m_iWinSizeX) && (m_vMousePos.y >= 0.f && m_vMousePos.y < m_iWinSizeY))
				{
					m_PickingPos = _vec4(m_pGameInstance->PickingDepth(m_vMousePos.x, m_vMousePos.y).x, m_pGameInstance->PickingDepth(m_vMousePos.x, m_vMousePos.y).y, m_pGameInstance->PickingDepth(m_vMousePos.x, m_vMousePos.y).z, 1.f); ;
				}

				FastPicking();
			}

		}
		else if (m_eItemType == ItemType::Environment)
		{

			if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LCONTROL))
			{
				if (m_pSelectedDummy)
				{
					m_pSelectedDummy->Select(false);
					m_pSelectedDummy = nullptr;
				}
				PickingRayCast();
				m_PickingPos = _vec4(m_vRayCastPos.x, m_vRayCastPos.y, m_vRayCastPos.z, 1.f);
				m_vInstancePos.push_back(m_vRayCastPos);
				m_vInstanceNor.push_back(m_vRayCastNor);
				
				FastPicking();
			}

			if (m_pGameInstance->Mouse_Pressing(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LSHIFT))
			{

				//if ((m_vMousePos.x >= 0.f && m_vMousePos.x < m_iWinSizeX) && (m_vMousePos.y >= 0.f && m_vMousePos.y < m_iWinSizeY))
				//{
					fTimeDeltaAcc += fTimeDelta;
					if (fTimeDeltaAcc > 0.1f)
					{
						//m_PickingPos = _vec4(m_pGameInstance->PickingDepth(m_vMousePos.x, m_vMousePos.y).x, m_pGameInstance->PickingDepth(m_vMousePos.x, m_vMousePos.y).y, m_pGameInstance->PickingDepth(m_vMousePos.x, m_vMousePos.y).z, 1.f); ;
						PickingRayCast();
						m_PickingPos = _vec4(m_vRayCastPos.x, m_vRayCastPos.y, m_vRayCastPos.z, 1.f);
						m_vInstancePos.push_back(m_PickingPos);
						m_vInstanceNor.push_back(m_vRayCastNor);
						fTimeDeltaAcc = 0;
					}
				//}
			}
			if (m_pGameInstance->Mouse_Pressing(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_SPACE))
			{
				if (m_pSelectedDummy)
				{
					m_pSelectedDummy->Select(false);
					m_pSelectedDummy = nullptr;
				}
				FastPicking();
				Delete_Dummy();
			}

		}
		else if (m_eItemType == ItemType::Camera)
		{
			const _uint iMaxClick = 2;
			if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LCONTROL))
			{
				if ((m_vMousePos.x >= 0.f && m_vMousePos.x < m_iWinSizeX) && (m_vMousePos.y >= 0.f && m_vMousePos.y < m_iWinSizeY))
				{
					m_PickingPos = m_pGameInstance->PickingDepth(m_vMousePos.x, m_vMousePos.y);
					m_fCameraPickingPos[iClickCount] = _vec4(m_PickingPos.x, m_PickingPos.y + 10.f, m_PickingPos.z, 1.f);
					iClickCount++;

					if (iClickCount >= iMaxClick)
					{
						m_fCameraPickingPos[iMaxClick] = {_vec4(0.f, 0.f, 0.f, 1.f)};
						iClickCount = 0;
					}

				}
				FastPicking();
			}
		}

		if (m_pGameInstance->Mouse_Down(DIM_RBUTTON))
		{
			if (m_pSelectedDummy)
			{
				m_pSelectedDummy->Select(false);
				m_pSelectedDummy = nullptr;
			}

			if (m_pSelectMap)
			{
				m_pSelectMap->Select(false);
				m_pSelectMap = nullptr;
			}
			m_vInstancePos.clear();
			m_vInstanceNor.clear();
		}

	}
}
HRESULT CImGui_Manager::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiPos();
	ImGuiMenu();

	bool bDemo = true;
	ImGui::ShowDemoWindow(&bDemo);

	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
	ImGuizmo::SetRect(0, 0, (_float)(m_iWinSizeX), (_float)(m_iWinSizeY));

	if (m_iImGuizmoCheck)
		ImGuizmoMenu();

	ImGui::Render();

	return S_OK;
}

HRESULT CImGui_Manager::ImGuiMenu()
{
	ImGui::Begin("EDITOR");

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("EDITOR", tab_bar_flags))
	{
#pragma region 터레인
		if (ImGui::BeginTabItem("Terrain"))
		{
			m_eItemType = ItemType::Terrain;
			ImGui::InputInt2("Terrain Count", TerrainCount, 0);

			ImGui::SeparatorText("Terrain Hight");

			ImGui::InputFloat("Hight", &TerrainHight, 2);
			ImGui::InputFloat("Size", &BrushSize, 1);
			if (BrushSize <= 0.f)
				BrushSize = 1.f;
			else if (BrushSize > 100.f)
				BrushSize = 100.f;

			if (m_pTerrain)
				m_pTerrain->Set_BrushSize(BrushSize);

			if (!m_pTerrain)
			{
				if (ImGui::Button("Create"))
				{
					Create_Terrain();
				}

				if (ImGui::Button("Load_Create"))
				{
					Load_Map_Vertex_Info_For_Terrain();
				}
			}
			else
			{
				if (ImGui::Button("Modify"))
				{
					Modify_Terrain();
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					Delete_Terrain();
				}
			}

			ImGui::SeparatorText("Texture");

			//if (m_pTerrain)
			//{
			//	CTexture* TerrainTexture = static_cast<CTexture*>(m_pTerrain->Find_Component(TEXT("Com_Texture")));
			//	ImGui::ImageButton((TerrainTexture->Get_SRV()), ImVec2(128.f, 128.f));

			//}

			ImGui::SeparatorText("Save / Load");
			ImGui::EndTabItem();
		}
#pragma endregion

#pragma region 맵

		if (ImGui::BeginTabItem("MAP"))
		{
			m_eItemType = ItemType::Map;

			ImGui::SeparatorText("LIST");
			static int Map_current_idx = -1;
			ImGui::Text("Map");
			static int iSelectMap = 0;
			ImGui::RadioButton("ETC", &iSelectMap, 0); ImGui::SameLine();
			ImGui::RadioButton("Build", &iSelectMap, 1); ImGui::SameLine();
			ImGui::RadioButton("Dungeon", &iSelectMap, 2);
			if (iSelectMap == 0)
			{
				m_eType = TEXT("ETC");
			}
			else if (iSelectMap == 1)
			{
				m_eType = TEXT("Build");
			}
			else if (iSelectMap == 2)
			{
				m_eType = TEXT("Dungeon");
			}

			if (ImGui::BeginListBox("MAPS PATH", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (int n = 0; n < Maps[m_eType].size(); n++)
				{
					const bool is_selected = (Map_current_idx == n);
					if (ImGui::Selectable(Maps[m_eType][n], is_selected))
					{
						Map_current_idx = n;
					}
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndListBox();
			}	
			
			
			ImGui::Separator();
			ImGui::SeparatorText("MATRIX : ");
			ImGui::InputFloat4("Right", &m_MapMatrix.m[0][0]);
			ImGui::InputFloat4("Up", &m_MapMatrix.m[1][0]);
			ImGui::InputFloat4("Look", &m_MapMatrix.m[2][0]);
			ImGui::InputFloat4("Position", &m_MapMatrix.m[3][0]);
			ImGui::Separator();
			if (ImGui::Button("Delete"))
			{
				Delete_Map();
			}
			ImGui::SameLine();
			if (ImGui::Button("Create"))
			{
				if (Map_current_idx != -1)
				{
					Create_Map(Map_current_idx);
				}
			}

			ImGui::SeparatorText("Save Vertices");
			if (ImGui::Button("Save_Vertices"))
			{
				Save_Map_Vertex_Info();
			}
			ImGui::SeparatorText("Save / Load");

			if (ImGui::Button("Save_Map"))
			{
				Save_Map();
			}
			ImGui::SameLine();

			if (ImGui::Button("Load_Map"))
			{
				Load_Map();
			}
			ImGui::EndTabItem();
		}
#pragma endregion

#pragma region 오브젝트

		if (ImGui::BeginTabItem("Objects"))
		{
			m_eItemType = ItemType::Objects;
			/* 오브젝트 */
			static int iSelectObject = 0;
			ImGui::RadioButton("ETC", &iSelectObject, 0); ImGui::SameLine();
			ImGui::RadioButton("Build", &iSelectObject, 1); ImGui::SameLine();
			ImGui::RadioButton("Dungeon", &iSelectObject, 2);
			_int iObjectCount = m_ObjectsList.size();
			ImGui::InputInt("Count", &iObjectCount, 14);

			static int Object_current_idx = 0;
			ImGui::Text("Objects");

			if (iSelectObject == 0)
			{
				m_eType = TEXT("ETC");
			}
			else if (iSelectObject == 1)
			{
				m_eType = TEXT("Build");
			}
			else if (iSelectObject == 2)
			{
				m_eType = TEXT("Dungeon");
			}

			ImGui::NewLine();

			static ImGuiTextFilter Filter;
			Filter.Draw("Search##1");
			ImGui::SameLine();
			_bool shouldScrollToSelectedItem{};
			if (ImGui::Button("Scroll##1"))
			{
				shouldScrollToSelectedItem = true;
			}
			for (int i = 0; i < Objects[m_eType].size(); ++i)
			{
				if (Filter.PassFilter(Objects[m_eType][i]))
				{
					_bool isSelected = (i == m_iSelectIdx);

					if (ImGui::Selectable(Objects[m_eType][i], isSelected))
					{
						Object_current_idx = i;
						m_iSelectIdx = Object_current_idx;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::Separator();
			ImGui::SeparatorText("MATRIX : ");
			ImGui::InputFloat4("Right", &m_ObjectMatrix.m[0][0], 0);
			ImGui::InputFloat4("Up", &m_ObjectMatrix.m[1][0], 0);
			ImGui::InputFloat4("Look", &m_ObjectMatrix.m[2][0], 0);
			ImGui::InputFloat4("Position", &m_ObjectMatrix.m[3][0], 0);
			ImGui::Separator();
			if (ImGui::Button("Delete"))
			{
				Delete_Dummy();
			}

			ImGui::SameLine();

			if (ImGui::Button("Create"))
			{
				if (Object_current_idx != -1)
				{
					Create_Dummy(Object_current_idx);
				}
			}

			ImGui::SeparatorText("Save / Load");

			if (ImGui::Button("SAVE"))
			{
				Save_Object();
			}
			ImGui::SameLine();
			if (ImGui::Button("LOAD"))
			{
				Load_Object();
			}
			ImGui::EndTabItem();


		}

#pragma endregion

#pragma region 환경
		if (ImGui::BeginTabItem("Environment"))
		{
			m_eItemType = ItemType::Environment;
			/* 환경 */
			static int iSelectEnvir = 0;
			ImGui::RadioButton("Tree", &iSelectEnvir, 0); ImGui::SameLine();
			ImGui::RadioButton("Grass", &iSelectEnvir, 1); ImGui::SameLine();
			ImGui::RadioButton("Rock", &iSelectEnvir, 2);

			_int iEnvirCount = m_EnvirList.size();
			_int iPickingCount = m_vInstancePos.size();
			ImGui::InputInt("Count", &iEnvirCount, 14);
			ImGui::InputInt("Picking Count", &iPickingCount, 14);

			ImGui::SeparatorText("LIST");
			static int Environment_current_idx = 0;
			ImGui::Text("Environment");

			if(iSelectEnvir == 0){m_eType = TEXT("Tree");}
			else if (iSelectEnvir == 1)	{m_eType = TEXT("Grass");}
			else if (iSelectEnvir == 2)	{m_eType = TEXT("Rock");}

			ImGui::Separator();
			ImGui::NewLine();

			static ImGuiTextFilter Filter;
			Filter.Draw("Search##1");
			ImGui::SameLine();
			_bool shouldScrollToSelectedItem{};
			if (ImGui::Button("Scroll##1"))
			{
				shouldScrollToSelectedItem = true;
			}
			for (int i = 0; i < Envirs[m_eType].size(); ++i)
			{
				if (Filter.PassFilter(Envirs[m_eType][i]))
				{
					_bool isSelected = (i == m_iSelectIdx);

					if (ImGui::Selectable(Envirs[m_eType][i], isSelected))
					{
						Environment_current_idx = i;
						m_iSelectIdx = Environment_current_idx;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::Separator();
			ImGui::SeparatorText("MATRIX : ");
			ImGui::InputFloat4("Right", &m_ObjectMatrix.m[0][0], 0);
			ImGui::InputFloat4("Up", &m_ObjectMatrix.m[1][0], 0);
			ImGui::InputFloat4("Look", &m_ObjectMatrix.m[2][0], 0);
			ImGui::InputFloat4("Position", &m_ObjectMatrix.m[3][0], 0);
			ImGui::Separator();
			if (ImGui::Button("Delete"))
			{
				Delete_Dummy();
				//if (!m_vInstancePos.empty())
					m_vInstancePos.clear();
					m_vInstanceNor.clear();
			}

			ImGui::SameLine();

			if (ImGui::Button("Create"))
			{
				if (Environment_current_idx != -1)
				{
					Create_Dummy(Environment_current_idx);
					//if (!m_vInstancePos.empty())
						m_vInstancePos.clear();
						m_vInstanceNor.clear();
				}
			}
			ImGui::SeparatorText("Picking Info");

			if (ImGui::Button("Picking Delete"))
			{
				//if (!m_vInstancePos.empty())
					m_vInstancePos.clear();
					m_vInstanceNor.clear();
			}
			ImGui::SameLine();

			if (ImGui::Button("Picking One_Delete"))
			{
				if (!m_vInstancePos.empty() && !m_vInstanceNor.empty())
				{
					m_vInstancePos.pop_back();
					m_vInstanceNor.pop_back();
				}
			}


			ImGui::SeparatorText("Save / Load");

			if (ImGui::Button("SAVE"))
			{
				Save_Envir();
			}
			ImGui::SameLine();
			if (ImGui::Button("LOAD"))
			{
				Load_Envir();
			}
			ImGui::EndTabItem();
		}

#pragma endregion


#pragma region 상호작용 오브젝트
		if (ImGui::BeginTabItem("Interaction"))
		{
			m_eItemType = ItemType::Interaction;
			/* 상호작용 */
			static int iSelectInteraction = 0;
			ImGui::RadioButton("NonAnim", &iSelectInteraction, 0); ImGui::SameLine();
			ImGui::RadioButton("Anim", &iSelectInteraction, 1);

			_int iInteractionCount = m_InteractionList.size();
			_int iPickingCount = m_vInstancePos.size();
			ImGui::InputInt("Count", &iInteractionCount, 14);
			ImGui::InputInt("Picking Count", &iPickingCount, 14);

			ImGui::SeparatorText("LIST");
			static int Interaction_current_idx = 0;
			ImGui::Text("Environment");

			if (iSelectInteraction == 0) { m_eType = TEXT("NonAnim"); }
			else if (iSelectInteraction == 1) { m_eType = TEXT("Anim"); }

			ImGui::Separator();
			ImGui::NewLine();

			static ImGuiTextFilter Filter;
			Filter.Draw("Search##1");
			ImGui::SameLine();
			_bool shouldScrollToSelectedItem{};
			if (ImGui::Button("Scroll##1"))
			{
				shouldScrollToSelectedItem = true;
			}
			for (int i = 0; i < Interactions[m_eType].size(); ++i)
			{
				if (Filter.PassFilter(Interactions[m_eType][i]))
				{
					_bool isSelected = (i == m_iSelectIdx);

					if (ImGui::Selectable(Interactions[m_eType][i], isSelected))
					{
						Interaction_current_idx = i;
						m_iSelectIdx = Interaction_current_idx;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::Separator();
			ImGui::SeparatorText("MATRIX : ");
			ImGui::InputFloat4("Right", &m_ObjectMatrix.m[0][0], 0);
			ImGui::InputFloat4("Up", &m_ObjectMatrix.m[1][0], 0);
			ImGui::InputFloat4("Look", &m_ObjectMatrix.m[2][0], 0);
			ImGui::InputFloat4("Position", &m_ObjectMatrix.m[3][0], 0);
			ImGui::Separator();
			if (ImGui::Button("Delete"))
			{
				Delete_Dummy();
			}

			ImGui::SameLine();

			if (ImGui::Button("Create"))
			{
				if (Interaction_current_idx != -1)
				{
					Create_Dummy(Interaction_current_idx);
				}
			}
			ImGui::SeparatorText("Save / Load");

			if (ImGui::Button("SAVE"))
			{
				Save_Interaction();
			}
			ImGui::SameLine();
			if (ImGui::Button("LOAD"))
			{
				Load_Interaction();
			}
			ImGui::EndTabItem();
		}

#pragma endregion

#pragma region 몬스터
		if (ImGui::BeginTabItem("Monster"))
		{
			/* 몬스터 */
			static int iSelectMonster = 0;
			ImGui::RadioButton("Monster", &iSelectMonster, 0); ImGui::SameLine();
			ImGui::RadioButton("Boss", &iSelectMonster, 1);

			m_eItemType = ItemType::Monster;
			ImGui::SeparatorText("LIST");
			static int Monster_current_idx = 0;
			ImGui::Text("MONSTER");

			if (iSelectMonster == 0)
			{
				m_eType = TEXT("Monster");
			}
			else if (iSelectMonster == 1)
			{
				m_eType = TEXT("Boss");
			}

			ImGui::NewLine();

			static ImGuiTextFilter Filter;
			Filter.Draw("Search##2");
			ImGui::SameLine();
			_bool shouldScrollToSelectedItem{};
			if (ImGui::Button("Serch##2"))
			{
				shouldScrollToSelectedItem = true;
			}
			for (int i = 0; i < Monsters[m_eType].size(); ++i)
			{
				if (Filter.PassFilter(Monsters[m_eType][i]))
				{
					_bool isSelected = (i == m_iSelectIdx);

					if (ImGui::Selectable(Monsters[m_eType][i], isSelected))
					{
						Monster_current_idx = i;
						m_iSelectIdx = Monster_current_idx;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::Separator();
			ImGui::SeparatorText("MATRIX : ");
			ImGui::InputFloat4("Right", &m_ObjectMatrix.m[0][0]);
			ImGui::InputFloat4("Up", &m_ObjectMatrix.m[1][0]);
			ImGui::InputFloat4("Look", &m_ObjectMatrix.m[2][0]);
			ImGui::InputFloat4("Position", &m_ObjectMatrix.m[3][0]);
			ImGui::Separator();

			ImGui::Separator();
			if (ImGui::Button("Delete"))
			{
				Delete_Dummy();
			}
			ImGui::SameLine();

			if (ImGui::Button("Create"))
			{
				if (Monster_current_idx != -1)
				{
					Create_Dummy(Monster_current_idx);
				}
			}
			ImGui::SeparatorText("Save / Load");
			if (ImGui::Button("SAVE"))
			{
				Save_Monster();
			}
			ImGui::SameLine();
			if (ImGui::Button("LOAD"))
			{
				Load_Monster();
			}
			ImGui::EndTabItem();
		}

#pragma endregion
#pragma region 엔피씨
		if (ImGui::BeginTabItem("NPC"))
		{
			/* NPC */
			m_eItemType = ItemType::NPC;
			ImGui::SeparatorText("LIST");
			static int NPC_current_idx = 0;
			ImGui::Text("NPC");
	
			ImGui::NewLine();

			static ImGuiTextFilter Filter;
			Filter.Draw("Search##2");
			ImGui::SameLine();
			_bool shouldScrollToSelectedItem{};
			if (ImGui::Button("Serch##2"))
			{
				shouldScrollToSelectedItem = true;
			}
			for (int i = 0; i < NPCs.size(); ++i)
			{
				if (Filter.PassFilter(NPCs[i]))
				{
					_bool isSelected = (i == m_iSelectIdx);

					if (ImGui::Selectable(NPCs[i], isSelected))
					{
						NPC_current_idx = i;
						m_iSelectIdx = NPC_current_idx;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::Separator();
			ImGui::SeparatorText("MATRIX : ");
			ImGui::InputFloat4("Right", &m_ObjectMatrix.m[0][0], 0);
			ImGui::InputFloat4("Up", &m_ObjectMatrix.m[1][0], 0);
			ImGui::InputFloat4("Look", &m_ObjectMatrix.m[2][0], 0);
			ImGui::InputFloat4("Position", &m_ObjectMatrix.m[3][0], 0);
			ImGui::Separator();
			if (ImGui::Button("Delete"))
			{
				Delete_Dummy();
			}
			ImGui::SameLine();

			if (ImGui::Button("Create"))
			{
				if (m_iSelectIdx != -1)
				{
					Create_Dummy(NPC_current_idx);
				}
			}
			ImGui::Separator();
			if (ImGui::Button("SAVE"))
			{
				Save_NPC();
			}
			ImGui::SameLine();
			if (ImGui::Button("LOAD"))
			{
				Load_NPC();
			}
			ImGui::EndTabItem();
		}
#pragma endregion
#pragma region 카메라
		if (ImGui::BeginTabItem("Camera"))
		{
			m_eItemType = ItemType::Camera;
			static int item_current = 0;
			ImGui::Separator();

			ImGui::InputFloat4("Picking Start", &m_fCameraPickingPos[0].x, 0);
			ImGui::InputFloat4("Picking End", &m_fCameraPickingPos[1].x, 0);

			ImGui::Separator();
			ImGui::SeparatorText("Eye : ");
			ImGui::InputFloat4("Start Eye Curved", &vEyeStartCurved.x, nullptr, 0);
			ImGui::InputFloat4("Start Eye Pos", &vEyeStartPos.x, 0);
			ImGui::InputFloat4("End Eye Pos", &vEyeEndPos.x, 0);
			ImGui::InputFloat4("End Eye Curved", &vEyeEndCurved.x, nullptr, 0);

			ImGui::SeparatorText("At : ");
			ImGui::InputFloat4("Start At Curved", &vAtStartCurved.x, nullptr, 0);
			ImGui::InputFloat4("Start At Pos", &vAtStartPos.x, 0);
			ImGui::InputFloat4("End At Pos", &vAtEndPos.x, 0);
			ImGui::InputFloat4("End At Curved", &vAtEndCurved.x, nullptr, 0);

			ImGui::Separator();
			//ImGui::InputText("Section Name", &SectionName[0], SectionName.size());
			//ImGui::InputFloat("CameraSpeed : ", &fCameraSpeed, 0);

			if (!m_pSelectCamera)
			{
				if (ImGui::Button("Create Camera"))
				{
					Create_Camera();
				}
			}
			else
			{
				if (ImGui::Button("Delete Camera"))
				{
					Delete_Camera();
				}
			}

			ImGui::Separator();

			//_int iCameraSize = m_CameraList.size();
			//for (_uint i = 0; i < iCameraSize; i++)
			//{
				//if (!m_CameraList.empty())
				if (m_pSelectCamera)
				{
					//string strName = m_CameraList[i]->Get_Name();
					//string strName = m_pSelectCamera->Get_Name();
					if (ImGui::TreeNode("Camera"/*strName.c_str()*/))
					{
						static int iEye_Count = 0;
						//vector<CCutScene_Curve*>& pEyeCurve = m_CameraList[i]->Get_EyeCurve();
						vector<CCutScene_Curve*>& pEyeCurve = m_pSelectCamera->Get_EyeCurve();
						vector<string> pCurveEyeName;
						if (!pEyeCurve.empty())
						{
							for (auto iter = pEyeCurve.begin(); iter != pEyeCurve.end(); iter++)
							{
								pCurveEyeName.push_back((*iter)->Get_SectionName());
							}
							vector<const char*> pEyeCurveName;
							for (const auto& str : pCurveEyeName)
							{
								pEyeCurveName.push_back(str.c_str());
							}
							for (int n = 0; n < pEyeCurveName.size(); n++)
							{
								const bool is_selected = (iEye_Count == n);
								if (ImGui::Selectable(pEyeCurveName[n], is_selected))
									iEye_Count = n;

								if (is_selected)
								{
									ImGui::SetItemDefaultFocus();

								}
							}
						}
						ImGui::Separator();

						static int iAt_Count = 0;
						//vector<CCutScene_Curve*>& pAtCurve = m_CameraList[i]->Get_AtCurve();
						vector<CCutScene_Curve*>& pAtCurve = m_pSelectCamera->Get_AtCurve();
						vector<string> pCurveAtName;
						if (!pAtCurve.empty())
						{
							for (auto iter = pAtCurve.begin(); iter != pAtCurve.end(); iter++)
							{
								pCurveAtName.push_back((*iter)->Get_SectionName());
							}
							vector<const char*> pAtCurveName;
							for (const auto& str : pCurveAtName)
							{
								pAtCurveName.push_back(str.c_str());
							}

							for (int n = 0; n < pAtCurveName.size(); n++)
							{
								const bool is_selected = (iAt_Count == n);
								if (ImGui::Selectable(pAtCurveName[n], is_selected))
									iAt_Count = n;

								if (is_selected)
								{
									ImGui::SetItemDefaultFocus();
								}
							}
						}
							ImGui::Separator();
							if (!pEyeCurve.empty() && !pAtCurve.empty())
							{
								if (ImGui::Button("Get Point"))
								{
									_mat* pOutPoints = nullptr;
									pEyeCurve[iEye_Count]->Get_ControlPoints(&pOutPoints);
									m_mCameraEyePoint = *pOutPoints;

									vEyeStartCurved = m_mCameraEyePoint.Right();
									vEyeStartPos = m_mCameraEyePoint.Up();
									vEyeEndPos = m_mCameraEyePoint.Look();
									vEyeEndCurved = m_mCameraEyePoint.Position();

									pAtCurve[iAt_Count]->Get_ControlPoints(&pOutPoints);
									m_mCameraAtPoint = *pOutPoints;

									vAtStartCurved = m_mCameraAtPoint.Right();
									vAtStartPos = m_mCameraAtPoint.Up();
									vAtEndPos = m_mCameraAtPoint.Look();
									vAtEndCurved = m_mCameraAtPoint.Position();
								}
								ImGui::SameLine();
								if (ImGui::Button("Set Point"))
								{
									m_mCameraEyePoint.Right(vEyeStartCurved);
									m_mCameraEyePoint.Up(vEyeStartPos);
									m_mCameraEyePoint.Look(vEyeEndPos);
									m_mCameraEyePoint.Position(vEyeEndCurved);

									m_mCameraAtPoint.Right(vAtStartCurved);
									m_mCameraAtPoint.Up(vAtStartPos);
									m_mCameraAtPoint.Look(vAtEndPos);
									m_mCameraAtPoint.Position(vAtEndCurved);

									pEyeCurve[iEye_Count]->Set_ControlPoints(m_mCameraEyePoint);
									pAtCurve[iAt_Count]->Set_ControlPoints(m_mCameraAtPoint);
								}
							}
							ImGui::Separator();

							if (ImGui::Button("Curve Add"))
							{
								//Create_Curve(m_CameraList[i], m_fCameraPickingPos[0], m_fCameraPickingPos[1]);
								Create_Curve(m_pSelectCamera, m_fCameraPickingPos[0], m_fCameraPickingPos[1]);
							}
							ImGui::SameLine();
							if (ImGui::Button("Curve Delete"))
							{
								if (!pEyeCurve.empty() && !pAtCurve.empty())
								{
									Delete_Curve(pEyeCurve, pAtCurve);
								}
							}
							ImGui::Separator();
						

						ImGui::TreePop();
						ImGui::Separator();

						//vector<_bool> isCameraPlaying(m_CameraList.size(), false);

						//if (m_CameraList.size() != 0)
						//{
							string PlayButtonName = "Play";
							if (ImGui::Button(PlayButtonName.c_str()))
							{
								if (!pEyeCurve.empty() && !pAtCurve.empty())
								{
									//m_pGameInstance->Set_CameraModeIndex(CM_CUTSCENE);
									if(m_pSelectCamera)
										m_pSelectCamera->Start_Play();
								}
							}
							ImGui::SameLine();
							if (ImGui::Button("Stop"))
							{
								if (!pEyeCurve.empty() && !pAtCurve.empty())
								{
									if (m_pSelectCamera)
										m_pSelectCamera->Stop_Play();
								}
							}
							ImGui::SameLine();
							if (ImGui::Button("Reset Play"))
							{
								if (!pEyeCurve.empty() && !pAtCurve.empty())
								{
									if (m_pSelectCamera)
										m_pSelectCamera->Reset_Play();
								}
							}

							m_iFrame = m_pSelectCamera->Get_Frame();
							string FrameButtonName = "Frame";
							ImGui::SliderInt(FrameButtonName.c_str(), &m_iFrame, 0, 300);
						//}
							string SpeedButtonName = "Speed";
							ImGui::InputFloat(SpeedButtonName.c_str(), &fEyeSpeed, 0);

							if (ImGui::Button("Set Speed"))
							{
								pEyeCurve[iEye_Count]->Set_SectionSpeed(fEyeSpeed);
							}
							ImGui::SameLine();

							if (ImGui::Button("Get Speed"))
							{
								fEyeSpeed = pEyeCurve[iEye_Count]->Get_SectionSpeed();
							}
						ImGui::Separator();
						if (ImGui::Button("SAVE"))
						{
							Save_CutScene(m_pSelectCamera);
						}
						ImGui::SameLine();
						if (ImGui::Button("LOAD"))
						{
							Load_CutScene();
						}
					}
				//}
				
			}
				ImGui::EndTabItem();
		}
#pragma endregion
#pragma region 트리거
		if (ImGui::BeginTabItem("Trigger"))
		{
			/* Trigger */
			m_eItemType = ItemType::Trigger;

			_int m_iTriggerCount = m_TriggerList.size();

			ImGui::InputInt("Trigger Count", &m_iTriggerCount, 14);
			ImGui::Separator();

			
			ImGui::Separator();
			ImGui::InputFloat("Size", &m_fTriggerSize,1.f, 5.f, "%.3f", 0);
			ImGui::Separator();
			ImGui::InputFloat4("Position", &m_ObjectMatrix.m[3][0], 0);
			ImGui::Separator();
			ImGui::Checkbox("Limited", &m_isTriggerCheck);
			ImGui::Separator();
			if (ImGui::Button("Create"))
			{
				Create_Dummy(0);
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				Delete_Dummy();
			}
			ImGui::Separator();

			if (ImGui::Button("SAVE"))
			{
				Save_Trigger();
			}
			ImGui::SameLine();
			if (ImGui::Button("LOAD"))
			{
				Load_Trigger();
			}

			ImGui::EndTabItem();
		}
#pragma endregion
		ImGui::EndTabBar();
	}

	ImGui::End();

	return S_OK;
}

HRESULT CImGui_Manager::ImGuiPos()
{
	ImGui::Begin("Position");

	ImGui::SeparatorText("Camera Position : ");
	ImGui::InputFloat3("Cam Pos", &m_vCamPosition.x, 0);

	ImGui::Checkbox("CamPosition Get", &m_isCamPosition);
	if (m_isCamPosition == true)
	{
		m_vCamPosition = m_pGameInstance->Get_CameraPos();
	}

	if (ImGui::Button("Set CameraPos"))
	{
		CTransform* pCameraTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")));
		pCameraTransform->Set_Position(_vec3(m_isCamPosition));
	}

	ImGui::SeparatorText("Mouse Pos : ");
	ImGui::InputFloat2("Mouse Pos", &m_vMousePos.x, 0);

	ImGui::SeparatorText("RayCast Position : ");
	ImGui::InputFloat3("RayCast Pos", &m_vRayCastPos.x, 0);

	ImGui::SeparatorText("RayCast Normal : ");
	ImGui::InputFloat3("RayCast Normal", &m_vRayCastNor.x, 0);

	ImGui::SeparatorText("Picking Pos : ");
	ImGui::InputFloat3("Picking Pos", &m_PickingPos.x, 0);

	if (m_eItemType == ItemType::Terrain)
	{
		ImGui::SeparatorText("TERRAIN POS : ");
		ImGui::InputFloat3("Terrain Pos", &m_TerrainPos.x, 0);

		ImGui::SeparatorText("Wireframe : ");
		ImGui::Checkbox("WireFrame", &m_isMode);

		if (m_pTerrain)
		{
			m_pTerrain->Mode(m_isMode);
		}
		if (m_pSelectMap)
		{
			m_pSelectMap->Mode(m_isMode);
		}
	}

	ImGui::SeparatorText("ImGuizmo : ");
	ImGui::Checkbox("ImGui Check", &m_iImGuizmoCheck);

	ImGui::SeparatorText("DummyCount : ");
	_int iDummyCount = m_DummyList.size();
	ImGui::InputInt("Dummy Size", &iDummyCount, 14);

	ImGui::Checkbox("Use Normal", &m_isUseNormal);


	// 마스크맵 만들기
	//if (m_eItemType == ItemType::Map)
	//{
	//	if (ImGui::Button("Normal Save"))
	//	{
	//		MeshToMask();
	//	}
	//}

	if (m_eItemType != ItemType::Terrain || m_eItemType != ItemType::Monster || m_eItemType != ItemType::NPC)
	{
		if (m_pSelectedDummy)
		{
			CModel* pModel = dynamic_cast<CModel*>(m_pSelectedDummy->Find_Component(L"Com_Model"));
			m_fRadius = pModel->Get_Radius();

			ImGui::InputFloat("Radius", &m_fRadius);
		}
	}

	ImGui::SeparatorText("All Reset : ");

	if (ImGui::Button("Reset"))
	{
		Reset();
	}

	if (ImGui::Button("Save_WorldMatrix"))
	{
		Save_WorldMatrix();
	}
	ImGui::SameLine();

	ImGui::End();
	return S_OK;
}

// 검색
_int CImGui_Manager::FindByName(char* SearchStr, vector<const char*> List)
{
	for (_uint n = 0; n < List.size(); ++n)
	{
		if (strstr (List[n], SearchStr) == 0)
			return n;
	}

	return -1;
}
HRESULT CImGui_Manager::ImGuizmoMenu()
{
	m_ViewMatrix = m_pGameInstance->Get_Transform(TransformType::View);
	m_ProjMatrix = m_pGameInstance->Get_Transform(TransformType::Proj);
	if (m_eItemType != ItemType::Map && m_eItemType != ItemType::Camera)
	{
		if (m_pSelectedDummy)
		{
			CTransform* pObjectsTransform = (CTransform*)m_pSelectedDummy->Find_Component(TEXT("Com_Transform"));
			m_ObjectMatrix = pObjectsTransform->Get_World_Matrix();
			//ImGuizmo::Manipulate(&m_ViewMatrix.m[0][0], &m_ProjMatrix.m[0][0], ImGuizmo::OPERATION::SCALE, ImGuizmo::MODE::WORLD, &m_ObjectMatrix.m[0][0]);
			ImGuizmo::Manipulate(&m_ViewMatrix.m[0][0], &m_ProjMatrix.m[0][0], ImGuizmo::OPERATION::ROTATE, ImGuizmo::MODE::WORLD, &m_ObjectMatrix.m[0][0]);
			ImGuizmo::Manipulate(&m_ViewMatrix.m[0][0], &m_ProjMatrix.m[0][0], ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, &m_ObjectMatrix.m[0][0]);
			if (ImGuizmo::IsUsing() == true)
			{
				_vector ObjRight = { m_ObjectMatrix._11, m_ObjectMatrix._12, m_ObjectMatrix._13, m_ObjectMatrix._14 };
				_vector ObjUp = { m_ObjectMatrix._21, m_ObjectMatrix._22, m_ObjectMatrix._23, m_ObjectMatrix._24 };
				_vector ObjLook = { m_ObjectMatrix._31, m_ObjectMatrix._32, m_ObjectMatrix._33, m_ObjectMatrix._34 };
				_vector ObjPosition = { m_ObjectMatrix._41, m_ObjectMatrix._42, m_ObjectMatrix._43, m_ObjectMatrix._44 };
				pObjectsTransform->Set_State(State::Right, ObjRight);
				pObjectsTransform->Set_State(State::Up, ObjUp);
				pObjectsTransform->Set_State(State::Look, ObjLook);
				pObjectsTransform->Set_State(State::Pos, ObjPosition);
			}
		}
	}
	else if(m_eItemType == ItemType::Map)
	{
		if (m_pSelectMap)
		{
			CTransform* pMapTransform = (CTransform*)m_pSelectMap->Find_Component(TEXT("Com_Transform"));
			m_MapMatrix = pMapTransform->Get_World_Matrix();
			//ImGuizmo::Manipulate(&m_ViewMatrix.m[0][0], &m_ProjMatrix.m[0][0], ImGuizmo::OPERATION::SCALE, ImGuizmo::MODE::WORLD, &m_MapMatrix.m[0][0]);
			ImGuizmo::Manipulate(&m_ViewMatrix.m[0][0], &m_ProjMatrix.m[0][0], ImGuizmo::OPERATION::ROTATE, ImGuizmo::MODE::WORLD, &m_MapMatrix.m[0][0]);
			ImGuizmo::Manipulate(&m_ViewMatrix.m[0][0], &m_ProjMatrix.m[0][0], ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, &m_MapMatrix.m[0][0]);
			if (ImGuizmo::IsUsing() == true)
			{
				_vector ObjRight = { m_MapMatrix._11, m_MapMatrix._12, m_MapMatrix._13, m_MapMatrix._14 };
				_vector ObjUp = { m_MapMatrix._21, m_MapMatrix._22, m_MapMatrix._23, m_MapMatrix._24 };
				_vector ObjLook = { m_MapMatrix._31, m_MapMatrix._32, m_MapMatrix._33, m_MapMatrix._34 };
				_vector ObjPosition = { m_MapMatrix._41, m_MapMatrix._42, m_MapMatrix._43, m_MapMatrix._44 };
				pMapTransform->Set_State(State::Right, ObjRight);
				pMapTransform->Set_State(State::Up, ObjUp);
				pMapTransform->Set_State(State::Look, ObjLook);
				pMapTransform->Set_State(State::Pos, ObjPosition);
			}
		}
	}
	else if (m_eItemType == ItemType::Camera)
	{
		if (m_pSelectCamera)
		{
			CTransform* pCameraTransform = (CTransform*)m_pSelectMap->Find_Component(TEXT("Com_Transform"));
			m_CameraMatrix = pCameraTransform->Get_World_Matrix();
			ImGuizmo::Manipulate(&m_ViewMatrix.m[0][0], &m_ProjMatrix.m[0][0], ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, &m_MapMatrix.m[0][0]);
			if (ImGuizmo::IsUsing() == true)
			{
				_vector ObjPosition = { m_MapMatrix._41, m_MapMatrix._42, m_MapMatrix._43, m_MapMatrix._44 };
				pCameraTransform->Set_State(State::Pos, ObjPosition);
			}
		}
	}
	return S_OK;
}

void CImGui_Manager::Create_Dummy(const _int& iListIndex)
{
	if (m_pSelectedDummy)
	{
		m_pSelectedDummy->Select(false);
		m_pSelectedDummy = nullptr;
	}

	if (m_eItemType == ItemType::Environment)
	{
		for (_uint i = 0; i < m_vInstancePos.size(); i++)
		{
			DummyInfo Info{};
			Info.ppDummy = &m_pSelectedDummy;
			Info.Prototype = L"Prototype_Model_";
			Info.eType = m_eItemType;


			_vec3 vUp{ m_vInstanceNor[i] };
			_vec3 vRight = vUp.Cross(_vec3(0.f, 1.f, 0.f)).Get_Normalized();
			_vec3 vLook = vRight.Cross(vUp).Get_Normalized();

			Info.mMatrix.Right(vRight);
			Info.mMatrix.Up(vUp);
			Info.mMatrix.Look(vLook);
			Info.mMatrix.Position(m_vInstancePos[i]);



			_tchar strUnicode[MAX_PATH]{};
			MultiByteToWideChar(CP_ACP, 0, Envirs[m_eType][iListIndex], static_cast<int>(strlen(Envirs[m_eType][iListIndex])), strUnicode, static_cast<int>(strlen(Envirs[m_eType][iListIndex])));
			Info.Prototype += strUnicode;


			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &Info)))
			{
				MSG_BOX("Failed to Add Layer : Dummy");
			}

			CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectedDummy->Find_Component(L"Com_Transform"));
			random_device rd;
			_randNum RandNum(rd());
			_randFloat Random = _randFloat(0.f, 360.f);
			pTransform->Rotation(vUp, _float(Random(RandNum)));

			m_EnvirList.push_back(m_pSelectedDummy);
			m_DummyList.emplace(m_pSelectedDummy->Get_ID(), m_pSelectedDummy);
			m_pSelectedDummy = nullptr;
		}
	}
	else
	{
		DummyInfo Info{};
		Info.ppDummy = &m_pSelectedDummy;
		//Info.vPos = _vec4(m_PickingPos.x, m_PickingPos.y, m_PickingPos.z, 1.f);
		Info.mMatrix.Position(_vec4(m_PickingPos.x, m_PickingPos.y, m_PickingPos.z, 1.f));
		if (m_isUseNormal)
			Info.vNormal = m_vRayCastNor;
		else
			Info.vNormal = _vec3(0.f, 1.f, 0.f);
		Info.Prototype = L"Prototype_Model_";
		Info.eType = m_eItemType;

		if (m_eItemType == ItemType::Trigger)
		{
			Info.iTriggerNum = m_TriggerList.size();
			Info.fTriggerSize = m_fTriggerSize;
			Info.bCheck = m_isTriggerCheck;
		}
		else
		{
			Info.iTriggerNum = 0;
			Info.fTriggerSize = 0;
		}

		_tchar strUnicode[MAX_PATH]{};
		switch (m_eItemType)
		{
		case ItemType::Objects:
			MultiByteToWideChar(CP_ACP, 0, Objects[m_eType][iListIndex], static_cast<int>(strlen(Objects[m_eType][iListIndex])), strUnicode, static_cast<int>(strlen(Objects[m_eType][iListIndex])));
			break;
		case ItemType::Monster:
			MultiByteToWideChar(CP_ACP, 0, Monsters[m_eType][iListIndex], static_cast<int>(strlen(Monsters[m_eType][iListIndex])), strUnicode, static_cast<int>(strlen(Monsters[m_eType][iListIndex])));
			break;
		case ItemType::NPC:
			MultiByteToWideChar(CP_ACP, 0, NPCs[iListIndex], static_cast<int>(strlen(NPCs[iListIndex])), strUnicode, static_cast<int>(strlen(NPCs[iListIndex])));
			break;
		case ItemType::Interaction:
			MultiByteToWideChar(CP_ACP, 0, Interactions[m_eType][iListIndex], static_cast<int>(strlen(Interactions[m_eType][iListIndex])), strUnicode, static_cast<int>(strlen(Interactions[m_eType][iListIndex])));
			break;
		case ItemType::Trigger:
			Info.Prototype = L"Prototype_Model_Collider";

		}
		Info.Prototype += strUnicode;


		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &Info)))
		{
			MSG_BOX("Failed to Add Layer : Dummy");
		}

		switch (m_eItemType)
		{
		case MapEditor::ItemType::Objects:
			m_ObjectsList.push_back(m_pSelectedDummy);
			break;
		case MapEditor::ItemType::Monster:
			m_MonsterList.push_back(m_pSelectedDummy);
			break;
		case MapEditor::ItemType::NPC:
			m_NPCList.push_back(m_pSelectedDummy);
			break;
		case MapEditor::ItemType::Interaction:
			m_InteractionList.push_back(m_pSelectedDummy);
			break;
		case MapEditor::ItemType::Trigger:
			m_TriggerList.push_back(m_pSelectedDummy);
			break;
		}
		m_DummyList.emplace(m_pSelectedDummy->Get_ID(), m_pSelectedDummy);
		m_pSelectedDummy = nullptr;
	}
	
}

void CImGui_Manager::Create_Map(const _int& iListIndex)
{
	MapInfo Info{};

	Info.ppMap = &m_pSelectMap;
	Info.vPos = _vec4(m_MapMatrix.Position());
	Info.Prototype = L"Prototype_Model_";
	Info.eType = m_eItemType;
	Info.iStageIndex = 0;

	_tchar strUnicode[MAX_PATH]{};

	MultiByteToWideChar(CP_ACP, 0, Maps[m_eType][iListIndex], static_cast<int>(strlen(Maps[m_eType][iListIndex])), strUnicode, static_cast<int>(strlen(Maps[m_eType][iListIndex])));
	Info.Prototype += strUnicode;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Map"), TEXT("Prototype_GameObject_Map"), &Info)))
	{
		MSG_BOX("Failed to Add Layer : Map");
	}
	m_MapsList.push_back(m_pSelectMap);
	m_Map.emplace(m_pSelectMap->Get_ID(), m_pSelectMap);
	//m_pSelectMap = nullptr;

}

HRESULT CImGui_Manager::Create_Terrain()
{
	TERRAIN_INFO_MAPTOOL pTerrainInfo{};
	pTerrainInfo.vTerrainSize.x = TerrainCount[0];
	pTerrainInfo.vTerrainSize.y = TerrainCount[1];

	pTerrainInfo.ppTerrain = &m_pTerrain;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"), &pTerrainInfo)))
	{
		return E_FAIL;
	}


	return S_OK;
}

void CImGui_Manager::Delete_Dummy()
{
	if (m_pSelectedDummy)
	{
		m_pSelectedDummy->Kill();
		m_pSelectedDummy = nullptr;

		auto it = m_DummyList.find(DummyIndex);
		if (it != m_DummyList.end()) {
			it->second->Kill();
			m_DummyList.erase(it);
		}


		if (m_eItemType == ItemType::Objects)
		{

			for (auto it = m_ObjectsList.begin(); it != m_ObjectsList.end(); it++)
			{
				if ((*it)->Get_Selected() == true)
				{
					m_ObjectsList.erase(it);
					break;
				}
			}

		}
		else if (m_eItemType == ItemType::Monster)
		{
			for (auto it = m_MonsterList.begin(); it != m_MonsterList.end(); it++)
			{
				if ((*it)->Get_Selected() == true)
				{
					m_MonsterList.erase(it);
					break;
				}
			}

		}
		else if (m_eItemType == ItemType::NPC)
		{
			for (auto it = m_NPCList.begin(); it != m_NPCList.end(); it++)
			{
				if ((*it)->Get_Selected() == true)
				{
					m_NPCList.erase(it);
					break;
				}
			}

		}
		else if (m_eItemType == ItemType::Environment)
		{
			for (auto it = m_EnvirList.begin(); it != m_EnvirList.end(); it++)
			{
				if ((*it)->Get_Selected() == true)
				{
					m_EnvirList.erase(it);
					break;
				}
			}

		}
		else if (m_eItemType == ItemType::Interaction)
		{
			for (auto it = m_InteractionList.begin(); it != m_InteractionList.end(); it++)
			{
				if ((*it)->Get_Selected() == true)
				{
					m_InteractionList.erase(it);
					break;
				}
			}

		}
		else if (m_eItemType == ItemType::Trigger)
		{
			for (auto it = m_TriggerList.begin(); it != m_TriggerList.end(); it++)
			{
				if ((*it)->Get_Selected() == true)
				{
					m_TriggerList.erase(it);
					break;
				}
			}

		}
	

	}
}

void CImGui_Manager::Delete_Camera()
{
	/*if (!m_CameraList.empty())
	{
		m_CameraList.back()->Kill();
		m_CameraList.pop_back();
	}*/
	m_pSelectCamera->Kill();
	m_pSelectCamera = nullptr;
}

void CImGui_Manager::Delete_Map()
{
	if (m_pSelectMap)
	{
		if (!m_Map.empty())
			m_Map.erase(MapIndex);

		for (auto it = m_MapsList.begin(); it != m_MapsList.end(); it++)
		{
			if ((*it)->Get_Selected() == true)
			{
				m_MapsList.erase(it);
				break;
			}
		}
		m_pSelectMap->Set_Dead();
		m_pSelectMap = nullptr;
	}
}

void CImGui_Manager::Delete_Terrain()
{
	if (m_pTerrain)
	{
		m_pTerrain->Kill();
		m_pTerrain = nullptr;
	}
}

void CImGui_Manager::Delete_Curve(vector<CCutScene_Curve*>& pEyeCurve, vector<CCutScene_Curve*>& pAtCurve)
{
	if (!pEyeCurve.empty() && !pAtCurve.empty())
	{
		pEyeCurve.back()->Kill();
		pEyeCurve.pop_back();
		pAtCurve.back()->Kill();
		pAtCurve.pop_back();
	}
}

HRESULT CImGui_Manager::Modify_Terrain()
{
	CVIBuffer_Terrain* pVIBuffer = (CVIBuffer_Terrain*)m_pTerrain->Find_Component(TEXT("Com_VIBuffer"));
	pVIBuffer->ModifyTerrainVertexBuffer(TerrainCount[0], TerrainCount[1]);

	return S_OK;
}

void CImGui_Manager::Create_Camera()
{

	CameraInfo Info{};
	//Info.strName = InputName;
	Info.ppCamera = &m_pSelectCamera;
	Info.eType = ItemType::Camera;
	Info.vStartCutScene = m_fCameraPickingPos[0];
	Info.vEndCutScene = m_fCameraPickingPos[1];
	Info.fCameraSpeed = fCameraSpeed;
	//_tchar strUnicode[MAX_PATH]{};

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_CutScene_Camera"), TEXT("Prototype_GameObject_Camera_CutScene"), &Info)))
	{
		MSG_BOX("Failed to Add Layer : CutScene_Camera");
	}

}

void CImGui_Manager::Create_Curve(CCamera_CutScene* pCamera, _vec4 FirstPos, _vec4 SecondPos)
{
	pCamera->Add_Eye_Curve(FirstPos, SecondPos, fEyeSpeed);

	_vec4 vFixedFirstPos = _vec4(FirstPos.x + 10.f, FirstPos.y-10.f, FirstPos.z + 10.f, 1.f);
	_vec4 vFixedSecondPos = _vec4(SecondPos.x + 10.f, SecondPos.y - 10.f, SecondPos.z + 10.f, 1.f);
	pCamera->Add_At_Curve(vFixedFirstPos, vFixedSecondPos, fEyeSpeed);
}

void CImGui_Manager::Reset()
{
	for (auto iter : m_MapsList)
	{
		iter->Kill();
	}
	m_MapsList.clear();

	for (auto iter : m_ObjectsList)
	{
		iter->Kill();
	}
	m_ObjectsList.clear();

	for (auto iter : m_MonsterList)
	{
		iter->Kill();
	}
	m_MonsterList.clear();

	for (auto iter : m_NPCList)
	{
		iter->Kill();
	}
	m_NPCList.clear();

	for (auto iter : m_EnvirList)
	{
		iter->Kill();
	}
	m_EnvirList.clear();

	for (auto iter : m_InteractionList)
	{
		iter->Kill();
	}
	m_InteractionList.clear();

	//for (auto iter : m_CameraList)
	//{
	//	iter->Set_Dead();
	//}
	//m_CameraList.clear();

	for (auto Pair : m_DummyList)
	{
		Pair.second->Kill();
	}
	m_DummyList.clear();

	if (m_pSelectedDummy)
		m_pSelectedDummy = nullptr;
	if (m_pSelectMap)
		m_pSelectMap = nullptr;
	if (m_pTerrain)
		m_pTerrain = nullptr;
	if (m_pSelectCamera)
		m_pSelectCamera = nullptr;
}

void CImGui_Manager::PopBack_Dummy()
{
	if (m_eItemType == ItemType::Map)
	{
		m_MapsList.pop_back();
	}
	if (m_eItemType == ItemType::Objects)
	{
		m_ObjectsList.pop_back();
	}
	if (m_eItemType == ItemType::Environment)
	{
		m_EnvirList.pop_back();
	}
	if (m_eItemType == ItemType::Monster)
	{
		m_MonsterList.pop_back();
	}

	if (m_eItemType == ItemType::NPC)
	{
		m_NPCList.pop_back();
	}


	if (m_pSelectedDummy)
		m_pSelectedDummy = nullptr;
	if (m_pSelectMap)
		m_pSelectMap = nullptr;
	if (m_pTerrain)
		m_pTerrain = nullptr;
}

void CImGui_Manager::Search_Map()
{
	string strFilePath = "../../Client/Bin/Resources/StaticMesh/Map/Tutorial/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strFilePath))
	{
		if (entry.is_regular_file())
		{
			string strName = entry.path().stem().string();
			string extension = entry.path().extension().string();

			if (extension == ".hyuntrastatmesh") {
				char* cstr = new char[strName.length() + 1];
				strcpy_s(cstr, strName.length() + 1, strName.c_str());
				Maps[TEXT("ETC")].push_back(cstr);
			}
		}
	}

	strFilePath = "../../Client/Bin/Resources/StaticMesh/Map/Midgard/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strFilePath))
	{
		if (entry.is_regular_file())
		{
			string strName = entry.path().stem().string();
			string extension = entry.path().extension().string();

			if (extension == ".hyuntrastatmesh") {
				char* cstr = new char[strName.length() + 1];
				strcpy_s(cstr, strName.length() + 1, strName.c_str());
				Maps[TEXT("Build")].push_back(cstr);
			}
		}
	}

	strFilePath = "../../Client/Bin/Resources/StaticMesh/Map/Dungeon/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strFilePath))
	{
		if (entry.is_regular_file())
		{
			string strName = entry.path().stem().string();
			string extension = entry.path().extension().string();

			if (extension == ".hyuntrastatmesh") {
				char* cstr = new char[strName.length() + 1];
				strcpy_s(cstr, strName.length() + 1, strName.c_str());
				Maps[TEXT("Dungeon")].push_back(cstr);
			}
		}
	}

}

void CImGui_Manager::Search_Object()
{
	string strFilePath = "../../Client/Bin/Resources/StaticMesh/Object/Tutorial/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strFilePath))
	{
		if (entry.is_regular_file())
		{
			string strName = entry.path().stem().string();
			string extension = entry.path().extension().string();

			if (extension == ".hyuntrastatmesh") {

				char* cstr = new char[strName.length() + 1];
				strcpy_s(cstr, strName.length() + 1, strName.c_str());
				Objects[L"ETC"].push_back(cstr);
			}
		}

	}
	strFilePath = "../../Client/Bin/Resources/StaticMesh/Object/Midgard/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strFilePath))
	{
		if (entry.is_regular_file())
		{
			string strName = entry.path().stem().string();
			string extension = entry.path().extension().string();

			if (extension == ".hyuntrastatmesh") {

				char* cstr = new char[strName.length() + 1];
				strcpy_s(cstr, strName.length() + 1, strName.c_str());
				Objects[L"Build"].push_back(cstr);
			}
		}
	}
	strFilePath = "../../Client/Bin/Resources/StaticMesh/Object/Dungeon/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strFilePath))
	{
		if (entry.is_regular_file())
		{
			string strName = entry.path().stem().string();
			string extension = entry.path().extension().string();

			if (extension == ".hyuntrastatmesh") {

				char* cstr = new char[strName.length() + 1];
				strcpy_s(cstr, strName.length() + 1, strName.c_str());
				Objects[L"Dungeon"].push_back(cstr);
			}
		}
	}
}

void CImGui_Manager::Search_Monster()
{

	string strFilePath = "../../Client/Bin/Resources/AnimMesh/Monster/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strFilePath))
	{
		if (entry.is_regular_file())
		{
			string strName = entry.path().stem().string();
			string extension = entry.path().extension().string();

			if (extension == ".hyuntraanimmesh") {
				char* cstr = new char[strName.length() + 1];
				strcpy_s(cstr, strName.length() + 1, strName.c_str());
				Monsters[L"Monster"].push_back(cstr);
			}
		}
		
	}
	strFilePath = "../../Client/Bin/Resources/AnimMesh/Boss/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strFilePath))
	{
		if (entry.is_regular_file())
		{
			string strName = entry.path().stem().string();
			string extension = entry.path().extension().string();

			if (extension == ".hyuntraanimmesh") {
				char* cstr = new char[strName.length() + 1];
				strcpy_s(cstr, strName.length() + 1, strName.c_str());
				Monsters[L"Boss"].push_back(cstr);
			}
		}

	}

}

void CImGui_Manager::Search_NPC()
{
	if (!NPCs.empty())
		NPCs.clear();
	string strFilePath = "../../Client/Bin/Resources/AnimMesh/NPC/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strFilePath))
	{
		if (entry.is_regular_file())
		{
			string strName = entry.path().stem().string();
			string extension = entry.path().extension().string();

			if (extension == ".hyuntraanimmesh") {
				char* cstr = new char[strName.length() + 1];
				strcpy_s(cstr, strName.length() + 1, strName.c_str());
				NPCs.push_back(cstr);

			}
		}
	}
}

void CImGui_Manager::Search_Envir()
{
	string strFilePath = "../../Client/Bin/Resources/StaticMesh/Environment/Tree/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strFilePath))
	{

		if (entry.is_regular_file())
		{
			
			string strName = entry.path().stem().string();
			string extension = entry.path().extension().string();

			if (extension == ".hyuntrastatmesh") {
				char* cstr = new char[strName.length() + 1];
				strcpy_s(cstr, strName.length() + 1, strName.c_str());
				Envirs[L"Tree"].push_back(cstr);

			}
		}
	}

	strFilePath = "../../Client/Bin/Resources/StaticMesh/Environment/Grass/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strFilePath))
	{
		if (entry.is_regular_file())
		{
			string strName = entry.path().stem().string();
			string extension = entry.path().extension().string();

			if (extension == ".hyuntrastatmesh") {
				char* cstr = new char[strName.length() + 1];
				strcpy_s(cstr, strName.length() + 1, strName.c_str());
				Envirs[L"Grass"].push_back(cstr);

			}
		}
	}

	strFilePath = "../../Client/Bin/Resources/StaticMesh/Environment/Rock/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strFilePath))
	{
		if (entry.is_regular_file())
		{
			string strName = entry.path().stem().string();
			string extension = entry.path().extension().string();

			if (extension == ".hyuntrastatmesh") {
				char* cstr = new char[strName.length() + 1];
				strcpy_s(cstr, strName.length() + 1, strName.c_str());
				Envirs[L"Rock"].push_back(cstr);

			}
		}
	}

}

void CImGui_Manager::Search_Inter()
{
	string strFilePath = "../../Client/Bin/Resources/StaticMesh/Environment/Interaction/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strFilePath))
	{
		if (entry.is_regular_file())
		{
			string strName = entry.path().stem().string();
			string extension = entry.path().extension().string();

			if (extension == ".hyuntrastatmesh") {
				char* cstr = new char[strName.length() + 1];
				strcpy_s(cstr, strName.length() + 1, strName.c_str());
				Interactions[L"NonAnim"].push_back(cstr);

			}
		}
	}
	strFilePath = "../Bin/Resources/Model/Interaction/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strFilePath))
	{
		if (entry.is_regular_file())
		{
			string strName = entry.path().stem().string();
			string extension = entry.path().extension().string();

			if (extension == ".hyuntraanimmesh") {
				char* cstr = new char[strName.length() + 1];
				strcpy_s(cstr, strName.length() + 1, strName.c_str());
				Interactions[L"Anim"].push_back(cstr);

			}
		}
	}
}

void CImGui_Manager::Mouse_Pos()
{
	// 마우스 좌표를 월드 좌표로
	GetCursorPos(&m_ptMouse);
	ScreenToClient(m_hWnd, &m_ptMouse);
	m_vMousePos.x = (_float)m_ptMouse.x;
	m_vMousePos.y = (_float)m_ptMouse.y;

	_vec2 normalizedDeviceCoords;
	normalizedDeviceCoords.x = m_ptMouse.x / (0.5f * m_iWinSizeX) - 1.0f;
	normalizedDeviceCoords.y = m_ptMouse.y / (m_iWinSizeY * - 0.5f) + 1.f;

	_vec4 clipCoords;
	clipCoords.x = normalizedDeviceCoords.x;
	clipCoords.y = normalizedDeviceCoords.y;
	clipCoords.z = 0.f;
	clipCoords.w = 1.0f;

	_mat InversViewMat = m_pGameInstance->Get_Transform_Inversed(TransformType::View);
	_mat InversProjMat = m_pGameInstance->Get_Transform_Inversed(TransformType::Proj);

	_vec4 eyeCoords = XMVector3TransformCoord(XMLoadFloat4(&clipCoords), InversProjMat);
	eyeCoords = XMVector3TransformCoord(XMLoadFloat4(&eyeCoords), InversViewMat);

	if (m_pTerrain)
	{

		//m_pTerrain->Set_MousePos(m_PickingPos);
		m_pTerrain->Set_MousePos(m_TerrainPos);
	}

	m_vMouseWorld = eyeCoords;
}

void CImGui_Manager::FastPicking()
{
	if (m_pSelectedDummy)
	{
		m_pSelectedDummy->Select(false);
		return;
	}
	if (m_pSelectMap)
	{
		m_pSelectMap->Select(false);
	}
	if (m_pSelectCamera)
	{
		return;
	}
	if (m_eItemType != ItemType::Map && m_eItemType != ItemType::Camera)
	{
		DummyIndex = -1;

		if ((m_vMousePos.x >= 0.f && m_vMousePos.x < m_iWinSizeX) && (m_vMousePos.y >= 0.f && m_vMousePos.y < m_iWinSizeY))
		{
			DummyIndex = m_pGameInstance->FastPicking((_uint)m_vMousePos.x, (_uint)m_vMousePos.y);

			auto iter = m_DummyList.find(DummyIndex);
			if (iter != m_DummyList.end())
			{

				m_pSelectedDummy = (*iter).second;
				m_pSelectedDummy->Select(true);
				CTransform* pObjectsTransform = (CTransform*)m_pSelectedDummy->Find_Component(TEXT("Com_Transform"));
				m_ObjectMatrix = pObjectsTransform->Get_World_Matrix();
			}
		}
	}
	else if(m_eItemType == ItemType::Map)
	{
		MapIndex = -1;

		if ((m_vMousePos.x >= 0.f && m_vMousePos.x < m_iWinSizeX) && (m_vMousePos.y >= 0.f && m_vMousePos.y < m_iWinSizeY))
		{
			MapIndex = m_pGameInstance->FastPicking((_uint)m_vMousePos.x, (_uint)m_vMousePos.y);

			auto iter = m_Map.find(MapIndex);
			if (iter != m_Map.end())
			{
				m_pSelectMap = (*iter).second;
				m_pSelectMap->Select(true);
				CTransform* pMapTransform = (CTransform*)m_pSelectMap->Find_Component(TEXT("Com_Transform"));
				m_MapMatrix = pMapTransform->Get_World_Matrix();
			}
		}
	}
	//else if (m_eItemType == ItemType::Camera)
	//{
	//	CameraIndex = 0;

	//	if ((m_vMousePos.x >= 0.f && m_vMousePos.x < m_iWinSizeX) && (m_vMousePos.y >= 0.f && m_vMousePos.y < m_iWinSizeY))
	//	{
	//		CameraIndex = m_pGameInstance->FastPicking((_uint)m_vMousePos.x, (_uint)m_vMousePos.y);

	//		auto iter = m_Camera.find(CameraIndex);
	//		if (iter != m_Camera.end())
	//		{
	//			m_pSelectCamera = (*iter).second;
	//			m_pSelectCamera->Select(true);
	//			CTransform* pMapTransform = (CTransform*)m_pSelectCamera->Find_Component(TEXT("Com_Transform"));
	//			m_CameraMatrix = pMapTransform->Get_World_Matrix();
	//		}
	//	}
	//}
}

void CImGui_Manager::PickingRayCast()
{
	PxRaycastBuffer Buffer{};
	_float fDistance{1000.f};

	if (m_pGameInstance->Raycast(m_pGameInstance->Get_World_Pos(),  m_pGameInstance->Get_World_Dir(), fDistance, Buffer))
	{
		m_vRayCastPos = _vec4(PxVec3ToVector(Buffer.block.position, 1.f));
		m_vRayCastNor = _vec3(PxVec3ToVector(Buffer.block.normal));
	}
}

void CImGui_Manager::Picking_On_Terrain()
{
	if (m_pTerrain)
	{
		CVIBuffer* pBuffer = dynamic_cast<CVIBuffer*>(m_pTerrain->Find_Component(TEXT("Com_VIBuffer")));
		CVIBuffer_Terrain* pTerrainBuffer = dynamic_cast<CVIBuffer_Terrain*>(pBuffer);
		CTransform* pTransform = static_cast<CTransform*>(m_pTerrain->Find_Component(TEXT("Com_Transform")));
		if (!m_pTerrain->Get_Mesh_Terrain())
		{
			m_TerrainPos = m_pGameInstance->Compute_MousePicked_Terrain(pTransform->Get_World_Matrix(), pTerrainBuffer->Get_Vertices(), pTerrainBuffer->Get_NumVerticesX(), pTerrainBuffer->Get_NumVerticesZ());
		}
		else
		{
			//m_TerrainPos = m_pGameInstance->Compute_MousePicked_MeshTerrain(pTransform->Get_World_Matrix(), pTerrainBuffer->Get_Vertices(), pTerrainBuffer->Get_TerrainVertices(), pTerrainBuffer->Get_TerrainIndices());

		}
	}
}

void CImGui_Manager::MeshToMask()
{
	if (m_pSelectMap)
	{
		CModel* pModel = dynamic_cast<CModel*>(m_pSelectMap->Find_Component(TEXT("Com_Model")));
		vector<VTXSTATICMESH> VerticesPos = pModel->Get_StaticMeshVertices();

		m_pSelectMap->Create_HightMap(VerticesPos);
	}
}

HRESULT CImGui_Manager::Save_Map_Vertex_Info()
{
	vector<VTXSTATICMESH>Vertices;
	vector<_ulong>Indices;
	if (m_pSelectMap)
	{
		CModel* pModel = dynamic_cast<CModel*>(m_pSelectMap->Find_Component(TEXT("Com_Model")));
		Vertices = pModel->Get_StaticMeshVertices();
		Indices = pModel->Get_StaticMeshIndices();

		const TCHAR* pGetPath = TEXT("../Bin/Resources/Info/MeshInfo.dat");

		std::ofstream outFile(pGetPath, std::ios::binary);

		if (!outFile.is_open())
		{
			MessageBox(g_hWnd, L"파일 저장 실패", L"파일 저장", MB_OK);

			return E_FAIL;
		}

		size_t VerticesSize = Vertices.size();
		outFile.write(reinterpret_cast<char*>(&VerticesSize), sizeof(size_t));
		outFile.write(reinterpret_cast<char*>(Vertices.data()), Vertices.size() * sizeof(VTXSTATICMESH));


		size_t IndicesSize = Indices.size();
		outFile.write(reinterpret_cast<char*>(&IndicesSize), sizeof(size_t));
		outFile.write(reinterpret_cast<char*>(Indices.data()), Indices.size() * sizeof(_ulong));
	
		outFile.close();

		m_pSelectMap->Select(false);
		m_pSelectMap->Kill();
		m_pSelectMap = nullptr;

	}
	return S_OK;
}

HRESULT CImGui_Manager::Load_Map_Vertex_Info_For_Terrain()
{
	const TCHAR* pGetPath = TEXT("../Bin/Resources/Info/MeshInfo.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MessageBox(g_hWnd, L"파일 로드 실패", L"파일 로드", MB_OK);
		return E_FAIL;
	}

	vector<VTXSTATICMESH> LoadVertices;
	size_t VerticesSize;
	inFile.read(reinterpret_cast<char*>(&VerticesSize), sizeof(size_t));

	LoadVertices.resize(VerticesSize);

	inFile.read(reinterpret_cast<char*>(LoadVertices.data()), VerticesSize * sizeof(VTXSTATICMESH));


	vector<_ulong> LoadIndices;
	size_t IndicesSize;
	inFile.read(reinterpret_cast<char*>(&IndicesSize), sizeof(size_t));
	LoadIndices.resize(IndicesSize);
	inFile.read(reinterpret_cast<char*>(LoadIndices.data()), IndicesSize * sizeof(_ulong));


	if (!m_pTerrain)
	{
		TERRAIN_INFO_MAPTOOL pTerrainInfo{};
		pTerrainInfo.Vertices = LoadVertices;
		pTerrainInfo.Indices = LoadIndices;
		pTerrainInfo.ppTerrain = &m_pTerrain;
		pTerrainInfo.isMesh = true;

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"), &pTerrainInfo)))
		{
			return E_FAIL;
		}
	}
	inFile.close();

	return S_OK;
}

#pragma region 맵 저장 / 불러오기
HRESULT CImGui_Manager::Save_Map()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"MapData.dat";
	static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0dat 파일\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data";

	if (GetSaveFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ofstream outFile(pGetPath, std::ios::binary);

		if (!outFile.is_open())
			return E_FAIL;

		_uint MapListSize = (_uint)m_MapsList.size();
		outFile.write(reinterpret_cast<const char*>(&MapListSize), sizeof(_uint));

		for (auto& Map : m_MapsList)
		{
			CTransform* pMapTransform = dynamic_cast<CTransform*>(Map->Find_Component(TEXT("Com_Transform")));

			wstring MapPrototype = Map->Get_Info().Prototype;
			_ulong MapPrototypeSize = (_ulong)MapPrototype.size();

			outFile.write(reinterpret_cast<const char*>(&MapPrototypeSize), sizeof(_ulong));
			outFile.write(reinterpret_cast<const char*>(MapPrototype.c_str()), MapPrototypeSize * sizeof(wchar_t));

			_mat MapWorldMat = pMapTransform->Get_World_Matrix();
			outFile.write(reinterpret_cast<const char*>(&MapWorldMat), sizeof(_mat));
		}

		MessageBox(g_hWnd, L"파일 저장 완료", L"파일 저장", MB_OK);
		outFile.close();

	}
	return S_OK;
}
HRESULT CImGui_Manager::Load_Map()
{

	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	static TCHAR filter[] = L"모두(*.*)\0*.*\0데이터 파일(*.dat)\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = filePathName;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data\\";

	if (GetOpenFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MessageBox(g_hWnd, L"파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
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
			MapInfo.eType = ItemType::Map;
			MapInfo.Prototype = MapPrototype;
			MapInfo.vPos = _float4(MapWorldMat._41, MapWorldMat._42, MapWorldMat._43, 1.f);
			MapInfo.ppMap = &m_pSelectMap;

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Map"), TEXT("Prototype_GameObject_Map"), &MapInfo)))
			{
				MessageBox(g_hWnd, L"파일 로드 실패", L"파일 로드", MB_OK);
				return E_FAIL;
			}

			m_Map.emplace(m_pSelectMap->Get_ID(), m_pSelectMap);
			m_MapsList.push_back(m_pSelectMap);

			//CTransform* pMapTransform = dynamic_cast<CTransform*>(m_pSelectMap->Find_Component(TEXT("Com_Transform")));

			//pMapTransform->Set_State(State::Right, MapWorldMat.Right());
			//pMapTransform->Set_State(State::Up, MapWorldMat.Up());
			//pMapTransform->Set_State(State::Look, MapWorldMat.Look());
			//pMapTransform->Set_State(State::Pos, MapWorldMat.Position());

			m_pSelectMap = nullptr;
		}

		MessageBox(g_hWnd, L"파일 로드 완료", L"파일 로드", MB_OK);
		inFile.close();

	}
	return S_OK;
}
#pragma endregion

#pragma region 오브젝트 저장 / 불러오기
HRESULT CImGui_Manager::Save_Object()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"ObjectsData.dat";
	static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0dat 파일\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data";

	if (GetSaveFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ofstream outFile(pGetPath, std::ios::binary);

		if (!outFile.is_open())
			return E_FAIL;

		_uint ObjectsListSize = (_uint)m_ObjectsList.size();
		outFile.write(reinterpret_cast<const char*>(&ObjectsListSize), sizeof(_uint));

		for (auto& Objects : m_ObjectsList)
		{
			CTransform* pObjectsTransform = dynamic_cast<CTransform*>(Objects->Find_Component(TEXT("Com_Transform")));

			wstring ObjectsPrototype = Objects->Get_Info().Prototype;
			_ulong ObjectsPrototypeSize = (_ulong)ObjectsPrototype.size();

			outFile.write(reinterpret_cast<const char*>(&ObjectsPrototypeSize), sizeof(_ulong));
			outFile.write(reinterpret_cast<const char*>(ObjectsPrototype.c_str()), ObjectsPrototypeSize * sizeof(wchar_t));

			_mat ObjectsWorldMat = pObjectsTransform->Get_World_Matrix();
			outFile.write(reinterpret_cast<const char*>(&ObjectsWorldMat), sizeof(_mat));
		}

		MessageBox(g_hWnd, L"오브젝트 파일 저장 완료", L"파일 저장", MB_OK);
		outFile.close();

	}
	return S_OK;
}
HRESULT CImGui_Manager::Load_Object()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	static TCHAR filter[] = L"모두(*.*)\0*.*\0데이터 파일(*.dat)\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = filePathName;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data\\";

	if (GetOpenFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MessageBox(g_hWnd, L"파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
			return E_FAIL;
		}

		_uint ObjectsListSize;
		inFile.read(reinterpret_cast<char*>(&ObjectsListSize), sizeof(_uint));


		for (_uint i = 0; i < ObjectsListSize; ++i)
		{
			_ulong ObjectsPrototypeSize;
			inFile.read(reinterpret_cast<char*>(&ObjectsPrototypeSize), sizeof(_ulong));

			wstring ObjectsPrototype;
			ObjectsPrototype.resize(ObjectsPrototypeSize);
			inFile.read(reinterpret_cast<char*>(&ObjectsPrototype[0]), ObjectsPrototypeSize * sizeof(wchar_t));

			_mat ObjectsWorldMat;
			inFile.read(reinterpret_cast<char*>(&ObjectsWorldMat), sizeof(_mat));

			DummyInfo ObjectsInfo{};
			ObjectsInfo.eType = ItemType::Objects;
			ObjectsInfo.Prototype = ObjectsPrototype;
			//ObjectsInfo.vLook = _float3(ObjectsWorldMat._31, ObjectsWorldMat._32, ObjectsWorldMat._33);
			//ObjectsInfo.vPos = _float4(ObjectsWorldMat._41, ObjectsWorldMat._42, ObjectsWorldMat._43, 1.f);
			ObjectsInfo.mMatrix = ObjectsWorldMat;
			ObjectsInfo.ppDummy = &m_pSelectedDummy;

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &ObjectsInfo)))
			{
				MessageBox(g_hWnd, L"파일 로드 실패", L"파일 로드", MB_OK);
				return E_FAIL;
			}

			m_DummyList.emplace(m_pSelectedDummy->Get_ID(), m_pSelectedDummy);
			m_ObjectsList.push_back(m_pSelectedDummy);

			//CTransform* pObjectsTransform = dynamic_cast<CTransform*>(m_pSelectedDummy->Find_Component(TEXT("Com_Transform")));

			//pObjectsTransform->Set_State(State::Right, ObjectsWorldMat.Right());
			//pObjectsTransform->Set_State(State::Up, ObjectsWorldMat.Up());
			//pObjectsTransform->Set_State(State::Look, ObjectsWorldMat.Look());
			//pObjectsTransform->Set_State(State::Pos, ObjectsWorldMat.Position());

			m_pSelectedDummy = nullptr;
		}

		MessageBox(g_hWnd, L"파일 로드 완료", L"파일 로드", MB_OK);
		inFile.close();

	}
	return S_OK;
}
#pragma endregion

#pragma region 몬스터 저장 / 불러오기
HRESULT CImGui_Manager::Save_Monster()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"MonsterData.dat";
	static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0dat 파일\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data";

	if (GetSaveFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ofstream outFile(pGetPath, std::ios::binary);

		if (!outFile.is_open())
			return E_FAIL;

		_uint MonsterListSize = (_uint)m_MonsterList.size();
		outFile.write(reinterpret_cast<const char*>(&MonsterListSize), sizeof(_uint));

		for (auto& monster : m_MonsterList)
		{
			CTransform* pMonsterTransform = dynamic_cast<CTransform*>(monster->Find_Component(TEXT("Com_Transform")));

			wstring MonsterPrototype = monster->Get_Info().Prototype;
			_ulong MonsterPrototypeSize = (_ulong)MonsterPrototype.size();

			outFile.write(reinterpret_cast<const char*>(&MonsterPrototypeSize), sizeof(_ulong));
			outFile.write(reinterpret_cast<const char*>(MonsterPrototype.c_str()), MonsterPrototypeSize * sizeof(wchar_t));

			_mat MonsterWorldMat = pMonsterTransform->Get_World_Matrix();
			outFile.write(reinterpret_cast<const char*>(&MonsterWorldMat), sizeof(_mat));
		}

		MessageBox(g_hWnd, L"파일 저장 완료", L"파일 저장", MB_OK);
		outFile.close();

	}
	return S_OK;
}
HRESULT CImGui_Manager::Load_Monster()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	static TCHAR filter[] = L"모두(*.*)\0*.*\0데이터 파일(*.dat)\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = filePathName;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data\\";

	if (GetOpenFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MessageBox(g_hWnd, L"파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
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

			DummyInfo MonsterInfo{};
			MonsterInfo.eType = ItemType::Monster;
			MonsterInfo.Prototype = MonsterPrototype;
			//MonsterInfo.vLook = _float3(MonsterWorldMat._31, MonsterWorldMat._32, MonsterWorldMat._33);
			//MonsterInfo.vPos = _float4(MonsterWorldMat._41, MonsterWorldMat._42, MonsterWorldMat._43, 1.f);
			MonsterInfo.mMatrix = MonsterWorldMat;
			MonsterInfo.ppDummy = &m_pSelectedDummy;

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &MonsterInfo)))
			{
				MessageBox(g_hWnd, L"파일 로드 실패", L"파일 로드", MB_OK);
				return E_FAIL;
			}

			m_DummyList.emplace(m_pSelectedDummy->Get_ID(), m_pSelectedDummy);
			m_MonsterList.push_back(m_pSelectedDummy);

			//CTransform* pMonsterTransform = dynamic_cast<CTransform*>(m_pSelectedDummy->Find_Component(TEXT("Com_Transform")));

			//pMonsterTransform->Set_State(State::Right, MonsterWorldMat.Right());
			//pMonsterTransform->Set_State(State::Up, MonsterWorldMat.Up());
			//pMonsterTransform->Set_State(State::Look, MonsterWorldMat.Look());
			//pMonsterTransform->Set_State(State::Pos, MonsterWorldMat.Position());

			m_pSelectedDummy = nullptr;
		}

		MessageBox(g_hWnd, L"파일 로드 완료", L"파일 로드", MB_OK);
		inFile.close();

	}
	return S_OK;
}
#pragma endregion
#pragma region NPC
HRESULT CImGui_Manager::Save_NPC()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"NPCData.dat";
	static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0dat 파일\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data";

	if (GetSaveFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ofstream outFile(pGetPath, std::ios::binary);

		if (!outFile.is_open())
			return E_FAIL;

		_uint NPCListSize = (_uint)m_NPCList.size();
		outFile.write(reinterpret_cast<const char*>(&NPCListSize), sizeof(_uint));

		for (auto& NPC : m_NPCList)
		{
			CTransform* pNPCTransform = dynamic_cast<CTransform*>(NPC->Find_Component(TEXT("Com_Transform")));

			wstring NPCPrototype = NPC->Get_Info().Prototype;
			_ulong NPCPrototypeSize = (_ulong)NPCPrototype.size();

			outFile.write(reinterpret_cast<const char*>(&NPCPrototypeSize), sizeof(_ulong));
			outFile.write(reinterpret_cast<const char*>(NPCPrototype.c_str()), NPCPrototypeSize * sizeof(wchar_t));

			_mat NPCWorldMat = pNPCTransform->Get_World_Matrix();
			outFile.write(reinterpret_cast<const char*>(&NPCWorldMat), sizeof(_mat));
		}

		MessageBox(g_hWnd, L"파일 저장 완료", L"파일 저장", MB_OK);
		outFile.close();
	}
	return S_OK;
}
HRESULT CImGui_Manager::Load_NPC()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	static TCHAR filter[] = L"모두(*.*)\0*.*\0데이터 파일(*.dat)\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = filePathName;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data\\";

	if (GetOpenFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MessageBox(g_hWnd, L"파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
			return E_FAIL;
		}

		_uint NPCListSize;
		inFile.read(reinterpret_cast<char*>(&NPCListSize), sizeof(_uint));


		for (_uint i = 0; i < NPCListSize; ++i)
		{
			_ulong NPCPrototypeSize;
			inFile.read(reinterpret_cast<char*>(&NPCPrototypeSize), sizeof(_ulong));

			wstring NPCPrototype;
			NPCPrototype.resize(NPCPrototypeSize);
			inFile.read(reinterpret_cast<char*>(&NPCPrototype[0]), NPCPrototypeSize * sizeof(wchar_t));

			_mat NPCWorldMat;
			inFile.read(reinterpret_cast<char*>(&NPCWorldMat), sizeof(_mat));

			DummyInfo NPCInfo{};
			NPCInfo.eType = ItemType::NPC;
			NPCInfo.Prototype = NPCPrototype;
			//NPCInfo.vLook = _float3(NPCWorldMat._31, NPCWorldMat._32, NPCWorldMat._33);
			//NPCInfo.vPos = _float4(NPCWorldMat._41, NPCWorldMat._42, NPCWorldMat._43, 1.f);
			NPCInfo.mMatrix = NPCWorldMat;
;			NPCInfo.ppDummy = &m_pSelectedDummy;

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &NPCInfo)))
			{
				MessageBox(g_hWnd, L"파일 로드 실패", L"파일 로드", MB_OK);
				return E_FAIL;
			}

			m_DummyList.emplace(m_pSelectedDummy->Get_ID(), m_pSelectedDummy);
			m_MonsterList.push_back(m_pSelectedDummy);

			//CTransform* pMonsterTransform = dynamic_cast<CTransform*>(m_pSelectedDummy->Find_Component(TEXT("Com_Transform")));

			//pMonsterTransform->Set_State(State::Right, NPCWorldMat.Right());
			//pMonsterTransform->Set_State(State::Up, NPCWorldMat.Up());
			//pMonsterTransform->Set_State(State::Look, NPCWorldMat.Look());
			//pMonsterTransform->Set_State(State::Pos, NPCWorldMat.Position());

			m_pSelectedDummy = nullptr;
		}

		MessageBox(g_hWnd, L"파일 로드 완료", L"파일 로드", MB_OK);
		inFile.close();

	}
	return S_OK;
}
#pragma endregion
#pragma region 환경 저장 / 불러오기
HRESULT CImGui_Manager::Save_Envir()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"EnvirData.dat";
	static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0dat 파일\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data";

	if (GetSaveFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ofstream outFile(pGetPath, std::ios::binary);

		if (!outFile.is_open())
			return E_FAIL;

		_uint EnvirListSize = (_uint)m_EnvirList.size();
		outFile.write(reinterpret_cast<const char*>(&EnvirListSize), sizeof(_uint));

		for (auto& Envir : m_EnvirList)
		{
			CTransform* pEnvirTransform = dynamic_cast<CTransform*>(Envir->Find_Component(TEXT("Com_Transform")));

			wstring EnvirPrototype = Envir->Get_Info().Prototype;
			_ulong EnvirPrototypeSize = (_ulong)EnvirPrototype.size();

			outFile.write(reinterpret_cast<const char*>(&EnvirPrototypeSize), sizeof(_ulong));
			outFile.write(reinterpret_cast<const char*>(EnvirPrototype.c_str()), EnvirPrototypeSize * sizeof(wchar_t));

			_mat EnvirWorldMat = pEnvirTransform->Get_World_Matrix();
			outFile.write(reinterpret_cast<const char*>(&EnvirWorldMat), sizeof(_mat));
		}

		MessageBox(g_hWnd, L"파일 저장 완료", L"파일 저장", MB_OK);
		outFile.close();

	}
	return S_OK;
}
HRESULT CImGui_Manager::Load_Envir()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	static TCHAR filter[] = L"모두(*.*)\0*.*\0데이터 파일(*.dat)\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = filePathName;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data\\";

	if (GetOpenFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MessageBox(g_hWnd, L"파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
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

			DummyInfo EnvirInfo{};
			EnvirInfo.eType = ItemType::Environment;
			EnvirInfo.Prototype = EnvirPrototype;
			//EnvirInfo.vLook = _float4(EnvirWorldMat._31, EnvirWorldMat._32, EnvirWorldMat._33, EnvirWorldMat._34);
			//EnvirInfo.vLook = _float3(EnvirWorldMat._31, EnvirWorldMat._32, EnvirWorldMat._33);
			EnvirInfo.mMatrix = EnvirWorldMat;
			//EnvirInfo.vPos = _float4(EnvirWorldMat._41, EnvirWorldMat._42, EnvirWorldMat._43, 1.f);
			EnvirInfo.ppDummy = &m_pSelectedDummy;

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &EnvirInfo)))
			{
				MessageBox(g_hWnd, L"파일 로드 실패", L"파일 로드", MB_OK);
				return E_FAIL;
			}

			m_EnvirList.push_back(m_pSelectedDummy);
			m_DummyList.emplace(m_pSelectedDummy->Get_ID(), m_pSelectedDummy);

			//CTransform* pEnvirTransform = dynamic_cast<CTransform*>(m_pSelectedDummy->Find_Component(TEXT("Com_Transform")));

			//pEnvirTransform->Set_State(State::Right, EnvirWorldMat.Right());
			//pEnvirTransform->Set_State(State::Up, EnvirWorldMat.Up());
			//pEnvirTransform->Set_State(State::Look, EnvirWorldMat.Look());
			//pEnvirTransform->Set_State(State::Pos, EnvirWorldMat.Position());


			m_pSelectedDummy = nullptr;
		}

		MessageBox(g_hWnd, L"파일 로드 완료", L"파일 로드", MB_OK);
		inFile.close();

	}
	return S_OK;
}
#pragma endregion

#pragma region 상호작용 저장 / 불러오기
HRESULT CImGui_Manager::Save_Interaction()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"_InteractionData.dat";
	static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0dat 파일\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data";

	if (GetSaveFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ofstream outFile(pGetPath, std::ios::binary);

		if (!outFile.is_open())
			return E_FAIL;

		_uint InteractionListSize = (_uint)m_InteractionList.size();
		outFile.write(reinterpret_cast<const char*>(&InteractionListSize), sizeof(_uint));

		for (auto& Interaction : m_InteractionList)
		{
			CTransform* pInteractionTransform = dynamic_cast<CTransform*>(Interaction->Find_Component(TEXT("Com_Transform")));

			wstring InteractionPrototype = Interaction->Get_Info().Prototype;
			_ulong InteractionPrototypeSize = (_ulong)InteractionPrototype.size();

			outFile.write(reinterpret_cast<const char*>(&InteractionPrototypeSize), sizeof(_ulong));
			outFile.write(reinterpret_cast<const char*>(InteractionPrototype.c_str()), InteractionPrototypeSize * sizeof(wchar_t));

			_mat InteractionWorldMat = pInteractionTransform->Get_World_Matrix();
			outFile.write(reinterpret_cast<const char*>(&InteractionWorldMat), sizeof(_mat));
		}

		MessageBox(g_hWnd, L"파일 저장 완료", L"파일 저장", MB_OK);
		outFile.close();

	}
	return S_OK;
}

HRESULT CImGui_Manager::Load_Interaction()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	static TCHAR filter[] = L"모두(*.*)\0*.*\0데이터 파일(*.dat)\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = filePathName;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data\\";

	if (GetOpenFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MessageBox(g_hWnd, L"파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
			return E_FAIL;
		}

		_uint InteractionListSize;
		inFile.read(reinterpret_cast<char*>(&InteractionListSize), sizeof(_uint));

		for (_uint i = 0; i < InteractionListSize; ++i)
		{
			_ulong InteractionPrototypeSize;
			inFile.read(reinterpret_cast<char*>(&InteractionPrototypeSize), sizeof(_ulong));

			wstring InteractionPrototype;
			InteractionPrototype.resize(InteractionPrototypeSize);
			inFile.read(reinterpret_cast<char*>(&InteractionPrototype[0]), InteractionPrototypeSize * sizeof(wchar_t));

			_mat InteractionWorldMat;
			inFile.read(reinterpret_cast<char*>(&InteractionWorldMat), sizeof(_mat));

			DummyInfo EnvirInfo{};
			EnvirInfo.eType = ItemType::Interaction;
			EnvirInfo.Prototype = InteractionPrototype;
			EnvirInfo.mMatrix = InteractionWorldMat;
			EnvirInfo.ppDummy = &m_pSelectedDummy;

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &EnvirInfo)))
			{
				MessageBox(g_hWnd, L"파일 로드 실패", L"파일 로드", MB_OK);
				return E_FAIL;
			}

			m_DummyList.emplace(m_pSelectedDummy->Get_ID(), m_pSelectedDummy);
			m_InteractionList.push_back(m_pSelectedDummy);

			m_pSelectedDummy = nullptr;
		}

		MessageBox(g_hWnd, L"파일 로드 완료", L"파일 로드", MB_OK);
		inFile.close();

	}
	return S_OK;
}
#pragma endregion

#pragma region 트리거 저장 / 불러오기
HRESULT CImGui_Manager::Save_Trigger()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"_Trigger.dat";
	static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0dat 파일\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data";

	if (GetSaveFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ofstream outFile(pGetPath, std::ios::binary);

		if (!outFile.is_open())
			return E_FAIL;

		_uint TriggerList = (_uint)m_TriggerList.size();
		outFile.write(reinterpret_cast<const char*>(&TriggerList), sizeof(_uint));

		_uint iIndex = 0;
		for (auto& Trigger : m_TriggerList)
		{
			outFile.write(reinterpret_cast<const char*>(&iIndex), sizeof(_uint));

			wstring TriggerPrototype = Trigger->Get_Info().Prototype;
			_ulong TriggerPrototypeSize = (_ulong)TriggerPrototype.size();

			_bool TriggerCheck = Trigger->Get_Info().bCheck;
			outFile.write(reinterpret_cast<const char*>(&TriggerCheck), sizeof(_bool));

			outFile.write(reinterpret_cast<const char*>(&TriggerPrototypeSize), sizeof(_ulong));
			outFile.write(reinterpret_cast<const char*>(TriggerPrototype.c_str()), TriggerPrototypeSize * sizeof(wchar_t));

			_float fSize = Trigger->Get_Size();
			outFile.write(reinterpret_cast<const char*>(&fSize), sizeof(_float));

			CTransform* pEnvirTransform = dynamic_cast<CTransform*>(Trigger->Find_Component(TEXT("Com_Transform")));
			_mat TriggerWorldMat = pEnvirTransform->Get_World_Matrix();
			outFile.write(reinterpret_cast<const char*>(&TriggerWorldMat), sizeof(_mat));
			iIndex++;
		}

		MessageBox(g_hWnd, L"파일 저장 완료", L"파일 저장", MB_OK);
		outFile.close();


	}
	return S_OK;
}
HRESULT CImGui_Manager::Load_Trigger()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	static TCHAR filter[] = L"모두(*.*)\0*.*\0데이터 파일(*.dat)\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = filePathName;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data\\";

	if (GetOpenFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MessageBox(g_hWnd, L"파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
			return E_FAIL;
		}

		_uint TriggerListSize;
		inFile.read(reinterpret_cast<char*>(&TriggerListSize), sizeof(_uint));


		for (_uint i = 0; i < TriggerListSize; ++i)
		{
			DummyInfo TriggerInfo{};

			_uint iIndex{};
			inFile.read(reinterpret_cast<char*>(&iIndex), sizeof(_uint));

			TriggerInfo.iTriggerNum = iIndex;

			_bool bCheck{};
			inFile.read(reinterpret_cast<char*>(&bCheck), sizeof(_bool));

			_ulong TriggerPrototypeSize;
			inFile.read(reinterpret_cast<char*>(&TriggerPrototypeSize), sizeof(_ulong));

			wstring TriggerPrototype;
			TriggerPrototype.resize(TriggerPrototypeSize);
			inFile.read(reinterpret_cast<char*>(&TriggerPrototype[0]), TriggerPrototypeSize * sizeof(wchar_t));

			_float TriggerSize{};
			inFile.read(reinterpret_cast<char*>(&TriggerSize), sizeof(_float));
			TriggerInfo.fTriggerSize = TriggerSize;

			_mat TriggerWorldMat;
			inFile.read(reinterpret_cast<char*>(&TriggerWorldMat), sizeof(_mat));

			TriggerInfo.eType = ItemType::Trigger;
			//TriggerInfo.vLook = _float4(TriggerWorldMat._31, TriggerWorldMat._32, TriggerWorldMat._33, TriggerWorldMat._34);
			//TriggerInfo.vLook = _float3(TriggerWorldMat._31, TriggerWorldMat._32, TriggerWorldMat._33);
			//TriggerInfo.vPos = _float4(TriggerWorldMat._41, TriggerWorldMat._42, TriggerWorldMat._43, TriggerWorldMat._44);
			TriggerInfo.mMatrix = TriggerWorldMat;
			TriggerInfo.bCheck = bCheck;
			TriggerInfo.fTriggerSize = TriggerSize;
			TriggerInfo.iTriggerNum = iIndex;
			TriggerInfo.Prototype = TriggerPrototype;
			TriggerInfo.ppDummy = &m_pSelectedDummy;

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Dummy"), TEXT("Prototype_GameObject_Dummy"), &TriggerInfo)))
			{
				MessageBox(g_hWnd, L"파일 로드 실패", L"파일 로드", MB_OK);
				return E_FAIL;
			}

			m_DummyList.emplace(m_pSelectedDummy->Get_ID(), m_pSelectedDummy);
			m_TriggerList.push_back(m_pSelectedDummy);

			//CTransform* pEnvirTransform = dynamic_cast<CTransform*>(m_pSelectedDummy->Find_Component(TEXT("Com_Transform")));

			//pEnvirTransform->Set_State(State::Right, TriggerWorldMat.Right());
			//pEnvirTransform->Set_State(State::Up, TriggerWorldMat.Up());
			//pEnvirTransform->Set_State(State::Look, TriggerWorldMat.Look());
			//pEnvirTransform->Set_State(State::Pos, TriggerWorldMat.Position());

			m_pSelectedDummy = nullptr;
		}

		MessageBox(g_hWnd, L"파일 로드 완료", L"파일 로드", MB_OK);
		inFile.close();

	}
	return S_OK;
}
#pragma endregion
#pragma region 컷씬 저장 / 불러오기
HRESULT CImGui_Manager::Save_CutScene(CCamera_CutScene* pCamera)
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"_CutScene.dat";
	static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0dat 파일\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data";

	if (GetSaveFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ofstream outFile(pGetPath, std::ios::binary);

		if (!outFile.is_open())
			return E_FAIL;
		

		_uint NameSize = m_pSelectCamera->Get_Name().size();
		string CameraName = m_pSelectCamera->Get_Name();

		outFile.write(reinterpret_cast<const char*>(&NameSize), sizeof(_uint));
		outFile.write(reinterpret_cast<const char*>(CameraName.c_str()), sizeof(NameSize));

		vector<CCutScene_Curve*>& pEyeCurve = pCamera->Get_EyeCurve();
		vector<CCutScene_Curve*>& pAtCurve = pCamera->Get_AtCurve();

		_uint iEyeSize = pEyeCurve.size();
		_uint iAtSize = pAtCurve.size();

		outFile.write(reinterpret_cast<const char*>(&iEyeSize), sizeof(_uint));
		outFile.write(reinterpret_cast<const char*>(&iAtSize), sizeof(_uint));



		_mat* pOutPoints = nullptr;

		for (_uint i = 0; i < iEyeSize; i++)
		{
			pEyeCurve[i]->Get_ControlPoints(&pOutPoints);
			m_mCameraEyePoint = *pOutPoints;

			vEyeStartCurved = m_mCameraEyePoint.Right();
			vEyeStartPos = m_mCameraEyePoint.Up();
			vEyeEndPos = m_mCameraEyePoint.Look();
			vEyeEndCurved = m_mCameraEyePoint.Position();

			outFile.write(reinterpret_cast<const char*>(&vEyeStartCurved), sizeof(_vec4));
			outFile.write(reinterpret_cast<const char*>(&vEyeStartPos), sizeof(_vec4));
			outFile.write(reinterpret_cast<const char*>(&vEyeEndPos), sizeof(_vec4));
			outFile.write(reinterpret_cast<const char*>(&vEyeEndCurved), sizeof(_vec4));

			_float fSectionSpeed = pEyeCurve[i]->Get_SectionSpeed();
			outFile.write(reinterpret_cast<const char*>(&fSectionSpeed), sizeof(_float));

		}
		for (_uint i = 0; i < iAtSize; i++)
		{
			pAtCurve[i]->Get_ControlPoints(&pOutPoints);
			m_mCameraAtPoint = *pOutPoints;

			vAtStartCurved = m_mCameraAtPoint.Right();
			vAtStartPos = m_mCameraAtPoint.Up();
			vAtEndPos = m_mCameraAtPoint.Look();
			vAtEndCurved = m_mCameraAtPoint.Position();

			outFile.write(reinterpret_cast<const char*>(&vAtStartCurved), sizeof(_vec4));
			outFile.write(reinterpret_cast<const char*>(&vAtStartPos), sizeof(_vec4));
			outFile.write(reinterpret_cast<const char*>(&vAtEndPos), sizeof(_vec4));
			outFile.write(reinterpret_cast<const char*>(&vAtEndCurved), sizeof(_vec4));
		}

		_float fCameraSpeed = pCamera->Get_Speed();
		outFile.write(reinterpret_cast<const char*>(&fCameraSpeed), sizeof(_float));


		MessageBox(g_hWnd, L"파일 저장 완료", L"파일 저장", MB_OK);
		outFile.close();
	}
		return S_OK;
}
HRESULT CImGui_Manager::Load_CutScene()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	static TCHAR filter[] = L"모두(*.*)\0*.*\0데이터 파일(*.dat)\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = filePathName;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data\\";

	if (GetOpenFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MessageBox(g_hWnd, L"파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
			return E_FAIL;
		}

		_uint InputNameSize;
		string InputName;
		inFile.read(reinterpret_cast<char*>(&InputNameSize), sizeof(_uint));
		inFile.read(reinterpret_cast<char*>(&InputName[0]), sizeof(InputNameSize));

		_uint iEyeSize;
		_uint iAtSize;
		inFile.read(reinterpret_cast<char*>(&iEyeSize), sizeof(_uint));
		inFile.read(reinterpret_cast<char*>(&iAtSize), sizeof(_uint));

		for (_uint i = 0; i < iEyeSize; i++)
		{

			inFile.read(reinterpret_cast<char*>(&vEyeStartCurved), sizeof(_vec4));
			m_mCameraEyePoint.Right(vEyeStartCurved);
			inFile.read(reinterpret_cast<char*>(&vEyeStartPos), sizeof(_vec4));
			m_mCameraEyePoint.Up(vEyeStartPos);
			inFile.read(reinterpret_cast<char*>(&vEyeEndPos), sizeof(_vec4));
			m_mCameraEyePoint.Look(vEyeEndPos);
			inFile.read(reinterpret_cast<char*>(&vEyeEndCurved), sizeof(_vec4));
			m_mCameraEyePoint.Position(vEyeEndCurved);

			inFile.read(reinterpret_cast<char*>(&fCameraSpeed), sizeof(_float));

			if (FAILED(m_pSelectCamera->Add_Eye_Curve(m_mCameraEyePoint, fCameraSpeed)))
			{
				return E_FAIL;
			}

		}
		for (_uint i = 0; i < iAtSize; ++i)
		{
			inFile.read(reinterpret_cast<char*>(&vAtStartCurved), sizeof(_vec4));
			m_mCameraAtPoint.Right(vAtStartCurved);
			inFile.read(reinterpret_cast<char*>(&vAtStartPos), sizeof(_vec4));
			m_mCameraAtPoint.Up(vAtStartPos);
			inFile.read(reinterpret_cast<char*>(&vAtEndPos), sizeof(_vec4));
			m_mCameraAtPoint.Look(vAtEndPos);
			inFile.read(reinterpret_cast<char*>(&vAtEndCurved), sizeof(_vec4));
			m_mCameraAtPoint.Position(vAtEndCurved);

			if (FAILED(m_pSelectCamera->Add_At_Curve(m_mCameraAtPoint)))
			{
				return E_FAIL;
			}
		}

		MessageBox(g_hWnd, L"파일 로드 완료", L"파일 로드", MB_OK);
		inFile.close();

	}
	return S_OK;
}
#pragma endregion

#pragma region 위치 저장
HRESULT CImGui_Manager::Save_Pos()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"_Pos.dat";
	static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0dat 파일\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data";

	if (GetSaveFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ofstream outFile(pGetPath, std::ios::binary);

		if (!outFile.is_open())
			return E_FAIL;

		_vec4 vSelectPos = m_PickingPos;
		outFile.write(reinterpret_cast<const char*>(&vSelectPos), sizeof(_vec4));

		MessageBox(g_hWnd, L"위치 파일 저장 완료", L"파일 저장", MB_OK);
		outFile.close();
	}
	return S_OK;
}
#pragma endregion

#pragma region 월드 저장
HRESULT CImGui_Manager::Save_WorldMatrix()
{
	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"_WorldPos.dat";
	static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0dat 파일\0*.dat";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\Data";

	if (GetSaveFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		std::ofstream outFile(pGetPath, std::ios::binary);

		if (m_pSelectedDummy)
		{
		if (!outFile.is_open())
			return E_FAIL;

		_mat mMatrix{};
			CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectedDummy->Find_Component(L"Com_Transform"));
			mMatrix = pTransform->Get_World_Matrix();

			outFile.write(reinterpret_cast<const char*>(&mMatrix), sizeof(_mat));
			MessageBox(g_hWnd, L"위치 파일 저장 완료", L"파일 저장", MB_OK);
		}
		else
		{
			MessageBox(g_hWnd, L"저장할 객체를 선택해주세요", L"저장 실패", MB_OK);
		}
		outFile.close();
	}
	return S_OK;
}
#pragma endregion
CImGui_Manager* CImGui_Manager::Create(const GRAPHIC_DESC& GraphicDesc)
{
	CImGui_Manager* pInstance = new CImGui_Manager();

	if (FAILED(pInstance->Initialize_Prototype(GraphicDesc)))
	{
		MSG_BOX("Failed to Created : CImGui_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImGui_Manager::Free()
{
	for (auto& entry : Maps) {
		for (const char* cstr : entry.second) {
			Safe_Delete_Array(cstr);
		}
		entry.second.clear(); 
	}
	Maps.clear();

	for (auto& entry : Objects)
	{
		for (const char* cstr : entry.second) {
			Safe_Delete_Array(cstr);
		}
		entry.second.clear();
	}
	Objects.clear();

	for (auto& entry : Monsters)
	{
		for (const char* cstr : entry.second) {
			Safe_Delete_Array(cstr);
		}
		entry.second.clear();
	}
	Monsters.clear();

	for (auto& cstr : NPCs)
	{
		Safe_Delete_Array(cstr);
	}
	NPCs.clear();

	for (auto& entry : Envirs)
	{
		for (const char* cstr : entry.second) {
			Safe_Delete_Array(cstr);
		}
		entry.second.clear();
	}
	Envirs.clear();

	for (auto& entry : Interactions)
	{
		for (const char* cstr : entry.second) {
			Safe_Delete_Array(cstr);
		}
		entry.second.clear();
	}
	Interactions.clear();

	for (auto& Map : m_MapsList)
	{
		Safe_Release(Map);
	}
	m_MapsList.clear();


	for (auto& cstr : m_ObjectsList)
	{
		Safe_Release(cstr);
	}
	m_ObjectsList.clear();

	for (auto& cstr : m_MonsterList)
	{
		Safe_Release(cstr);
	}
	m_MonsterList.clear();

	for (auto& cstr : m_NPCList)
	{
		Safe_Release(cstr);
	}
	m_NPCList.clear();

	for (auto& cstr : m_EnvirList)
	{
		Safe_Release(cstr);
	}
	m_EnvirList.clear();

	for (auto& cstr : m_InteractionList)
	{
		Safe_Release(cstr);
	}
	m_InteractionList.clear();

	for (auto& cstr : m_TriggerList)
	{
		Safe_Release(cstr);
	}
	m_TriggerList.clear();

	for (auto& cstr : m_CameraList)
	{
		Safe_Release(cstr);
	}
	m_CameraList.clear();

	//for (auto& Pair : m_DummyList)
	//{
	//	Safe_Release(Pair.second);
	//}
	m_DummyList.clear();


	//for (auto& Pair : m_Map)
	//{
	//	Safe_Release(Pair.second);
	//}
	m_Map.clear();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pSelectedDummy);
	Safe_Release(m_pSelectMap);
	Safe_Release(m_pSelectCamera);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
