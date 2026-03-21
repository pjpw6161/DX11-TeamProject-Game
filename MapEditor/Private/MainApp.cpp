#include "imgui.h"
#include "ImGuizmo.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "ImGui_Manager.h"

#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Manager.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Init()
{
	if (not m_pGameInstance)
	{
		return E_FAIL;
	}



	srand(static_cast<_uint>(time(nullptr)));
//#ifdef _DEBUG
//#ifdef UNICODE
//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
//#else
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif
//#endif // _DEBUG

	RECT rc = {};
	GetClientRect(g_hWnd, &rc);

	GRAPHIC_DESC        GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.hInst = g_hInst;
	GraphicDesc.isWindowed =true;
	GraphicDesc.iWinSizeX = rc.right - rc.left;
	GraphicDesc.iWinSizeY = rc.bottom - rc.top;

	m_pImGui_Manager = CImGui_Manager::Create(GraphicDesc);
	if (m_pImGui_Manager == nullptr)
		return E_FAIL;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();


	if (FAILED(m_pGameInstance->Init_Engine(LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;
	

	if (FAILED(Ready_Prototype_Component_For_Static()))
	{
		return E_FAIL;
	}

	m_pGameInstance->Ready_Texture2D();
	m_pGameInstance->Ready_FastPicking();

	if (FAILED(Ready_Prototype_For_Loading()))
	{
		return E_FAIL;
	}

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	if (FAILED(Open_Level(LEVEL_EDITOR)))
	{
		return E_FAIL;
	}


	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	if (not m_pGameInstance)
	{
		return;
	}

	_float fFinalTimeDelta = fTimeDelta * m_pGameInstance->Get_TimeRatio();

	m_fTimeAcc += fFinalTimeDelta;

	m_pImGui_Manager->Tick(fTimeDelta);
	m_pGameInstance->Tick_Engine(fFinalTimeDelta);

}

HRESULT CMainApp::Render()
{
	++m_iFrameCount;

	if (m_fTimeAcc >= 1.f)
	{
		wstring fps = L"FPS: " + to_wstring(m_iFrameCount);

		SetWindowText(g_hWnd, fps.c_str());

		m_fTimeAcc = 0.f;
		m_iFrameCount = 0;
	}


	if (FAILED(m_pImGui_Manager->Render()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 1.f, 0.f, 1.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Clear_DepthStencil_View()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pRenderer->Draw_RenderGroup()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Render()))
	{
		return E_FAIL;
	}
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (FAILED(m_pGameInstance->Present()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL_ID eLevelID)
{
	if (eLevelID == LEVEL_LOADING)
	{
		return E_FAIL;
	}

	CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID);
	if (not pLevel)
	{
		return E_FAIL;
	}

	return m_pGameInstance->Open_Level(LEVEL_LOADING, pLevel);
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	if (not m_pGameInstance)
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	Safe_AddRef(m_pRenderer);

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Curve"), CVIBuffer_Curve::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Vtxtex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxStatMesh.hlsl"), VTXSTATICMESH::Elements, VTXSTATICMESH::iNumElements))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh_Instance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxStatMesh_Instance.hlsl"), VTXMESH_INSTANCING::Elements, VTXMESH_INSTANCING::iNumElements))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Instancing"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex_Instancing.hlsl"), VTXPOINT_INSTANCING::Elements, VTXPOINT_INSTANCING::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Trail"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Vtxtex_Trail.hlsl"), VTXTRAIL::Elements, VTXTRAIL::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Curve"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Curve.hlsl"), VTXPOSCOLOR::Elements, VTXPOSCOLOR::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	//MakeSpriteFont "HY헤드라인M" /FontSize:30 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 138ex.spritefont
	if (FAILED(m_pGameInstance->Add_Font(L"Font_Malang", L"../../Client/Bin/Resources/Font/Naruto.spritefont")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Font(L"Font_Normal", L"../../Client/Bin/Resources/Font/NarutoNormal.spritefont")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Font(L"Font_Dialogue", L"../../Client/Bin/Resources/Font/NarutoDialogue.spritefont")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), CCollider::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_For_Loading()
{


#pragma region Prototype
#pragma endregion


	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pImGui_Manager);
	CCamera_Manager::Destroy_Instance();
	CGameInstance::Release_Engine();

	if (_heapchk() != _HEAPOK)
	{
		MSG_BOX("Heap currupted");
	}


}
