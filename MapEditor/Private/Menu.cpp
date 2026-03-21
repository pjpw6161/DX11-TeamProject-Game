#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo.h"

#include "GameInstance.h"
#include "Menu.h"
#include "VIBuffer_Terrain.h"
#include "Mesh.h"
#include "Layer.h"

#include "VIBuffer.h"


#include <filesystem>
namespace fs = filesystem;

CMenu::CMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())

{
	Safe_AddRef(m_pGameInstance);
}


HRESULT CMenu::Initialize_Prototype()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);
	ImGui_ImplWin32_Init(g_hWnd);
	XMStoreFloat4x4(&m_vObjectsMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CMenu::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	//Read_Texture_File("Terrain");
	//Read_HeightMap_File("HeightMap");

	return S_OK;
}

void CMenu::Tick(_float fTimeDelta)
{
	

	m_vProjMatrix = m_pGameInstance->Get_Transform_Float4x4(TransformType::Proj);
	m_vViewMatrix = m_pGameInstance->Get_Transform_Float4x4(TransformType::View);

	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON))
	{
		//m_pGameInstance->PickingDepth();
	}

}

void CMenu::Late_Tick(_float fTimeDelta)
{
	//m_pRendererCom->Add_RenderGroup(RG_UI, this);a

}

HRESULT CMenu::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
	ImGuizmo::SetRect(0, 0, g_iWinSizeX, g_iWinSizeY);
	Setting_Init();
	ShowInfo();
	bool bDemo = true;

	ImGui::ShowDemoWindow(&bDemo);
	

	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return S_OK;
}

void CMenu::Setting_Init()
{
	// Imgui option 설정
	ImGuiWindowFlags window_flags = 0;
	//window_flags |= ImGuiWindowFlags_NoTitleBar;
	//window_flags |= ImGuiWindowFlags_NoMove;
	//window_flags |= ImGuiWindowFlags_NoResize;
	// imgui 생성
	ImGui::Begin("Menu", nullptr, window_flags);
}

void CMenu::ShowInfo()
{
	ImGui::SeparatorText("MousePos");
	ImGui::InputFloat3("MousePos", &m_vSelectPos.x, 0);
	ImGui::SeparatorText("Type");

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

	if (ImGui::BeginTabBar("MapTool", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Map"))
		{
			m_eType = TYPE_MAP;

			
			ImGui::Spacing();
			
			ImGui::RadioButton("Solid", &m_isMode, 0); ImGui::SameLine();
			ImGui::RadioButton("WireFrame", &m_isMode, 1);

			ImGui::Separator();

			if (ImGui::Button("Apply Map"))
			{
			}
			 ImGui::SameLine();
			 if (ImGui::Button("Delete Map"))
			 {
			
			 }
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Objects"))
		{
			m_eType = TYPE_OBJECT;

			static int iSelectObject = -1;
			static int iLastSelectedObject = -1;

			
			
			ImGui::Spacing();

			ImGui::Separator();

			ImGui::InputFloat3("Object_Right", &m_vObjectsMatrix.m[0][0], 0);
			ImGui::InputFloat3("Object_Up", &m_vObjectsMatrix.m[1][0], 0);
			ImGui::InputFloat3("Object_Look", &m_vObjectsMatrix.m[2][0], 0);
			ImGui::InputFloat3("Object_Position", &m_vObjectsMatrix.m[3][0], 0);

			if (ImGui::Button("Apply Obj"))
			{
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete Obj"))
			{
			}
			ImGui::EndTabItem();
		}


		if (ImGui::BeginTabItem("Monster"))
		{
			m_eType = TYPE_MONSTER;

			static int iSelectMonster = -1;
			static int iLastSelectedMonster = -1;

			for (int n = 0; n < 3; n++)
			{
				const char* MonsterName[] = { "Monster1",
											"Monster2",
											"Monster3"
				};
				if (ImGui::Selectable(MonsterName[n], iSelectMonster == n))
				{
					
				}
			}
			ImGui::Spacing();

			ImGui::Separator();

			ImGui::InputFloat3("Object_Right", &m_vObjectsMatrix.m[0][0], 0);
			ImGui::InputFloat3("Object_Up", &m_vObjectsMatrix.m[1][0], 0);
			ImGui::InputFloat3("Object_Look", &m_vObjectsMatrix.m[2][0], 0);
			ImGui::InputFloat3("Object_Position", &m_vObjectsMatrix.m[3][0], 0);

			ImGui::Checkbox("IsLoop", &m_isLoop);

			if (iSelectMonster != -1)
			{
			}

			if (ImGui::Button("Create MonsterObj"))
			{
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete MonsterObj"))
			{
			}

			ImGui::Separator();

			if (ImGui::Button("Save_Monster"))
			{
			}

			ImGui::SameLine();
			if (ImGui::Button("Load_Monster"))
			{
			}


			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Camera"))
		{
			m_eType = TYPE_CAMERA;

			_float4 m_vCameraPos = m_pGameInstance->Get_CameraPos();
			_float4 m_vCameraLook = m_pGameInstance->Get_CameraLook();

			float vPos[4] = { m_vCameraPos.x, m_vCameraPos.y, m_vCameraPos.z, m_vCameraPos.w };
			float vLook[4] = { m_vCameraLook.x, m_vCameraLook.y, m_vCameraLook.z, m_vCameraLook.w };

			ImGui::InputFloat4("Camera Position", vPos, 0, 0);
			ImGui::InputFloat4("Camera Look", vLook, 0, 0);

			ImGui::Text("Camera....");
			ImGui::EndTabItem();
		}

		
		ImGui::EndTabBar();
			
		ImGui::Separator();

		if (ImGui::Button("Save"))
		{
		}

		ImGui::SameLine();

		if (ImGui::Button("Load"))
		{
		}
	}
}
//

HRESULT CMenu::Add_Components()
{
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	//{
	//	return E_FAIL;
	//}
	return S_OK;
}

CMenu* CMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMenu* pInstance = new CMenu(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX("Failed to Create : CMenu");
		Safe_Release(pInstance);
	}
	return pInstance;
}



void CMenu::Free()
{
	Safe_Release(m_pGameInstance);

	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();
}
