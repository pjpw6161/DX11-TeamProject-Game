#include "imgui.h"
#include "ImGuizmo.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "Imgui_Manager.h"

#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Effect_Manager.h"

#include "Effect_Manager.h"

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
#ifdef _DEBUG
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif // _DEBUG

	RECT rc = {};
	GetClientRect(g_hWnd, &rc);

	GRAPHIC_DESC        GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.hInst = g_hInst;
	GraphicDesc.isWindowed = TRUE;
	GraphicDesc.iWinSizeX = rc.right - rc.left;
	GraphicDesc.iWinSizeY = rc.bottom - rc.top;

	if (FAILED(m_pGameInstance->Init_Engine(LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	m_pImgui_Manager = CImgui_Manager::Create(m_pDevice, m_pContext, GraphicDesc);
	if (m_pImgui_Manager == nullptr)
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Prototype_For_Loading()))
	{
		return E_FAIL;
	}

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	if (FAILED(Open_Level(LEVEL_TOOL)))
	{
		return E_FAIL;
	}

	CEffect_Manager::Get_Instance()->Register_Callback();

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

	m_pGameInstance->Tick_Engine(fFinalTimeDelta);

	m_pImgui_Manager->Tick(fTimeDelta);

	m_pImgui_Manager->Late_Tick(fTimeDelta);

	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_TOOL)
	{
		m_pImgui_Manager->CreateModel();
	}
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

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();

	if (FAILED(m_pImgui_Manager->Render()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Clear_BackBuffer_View(_float4(0.2f, 0.2f, 0.2f, 1.f))))
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

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instancing_Point"), CVIBuffer_Instancing_Point::Create(m_pDevice, m_pContext))))

	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_Vtxtex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Effect"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_Vtxtex_Effect.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCubeTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX::Elements, VTXCUBETEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxStatMesh.hlsl"), VTXSTATICMESH::Elements, VTXSTATICMESH::iNumElements))))
	{
		return E_FAIL; 
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh_Effect"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxStatMesh_Effect.hlsl"), VTXSTATICMESH::Elements, VTXSTATICMESH::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Instancing"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxTex_Instancing.hlsl"), VTXPOINT_INSTANCING::Elements, VTXPOINT_INSTANCING::iNumElements))))
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
#pragma region Textures
	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Screen"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/LoadingScreen_%d.dds")))))
	//{
	//	return E_FAIL;
	//}
	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Indicator"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/Loading_Indicator.dds")))))
	//{
	//	return E_FAIL;
	//}
	//if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Icon"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/Loading_Icon.dds")))))
	//{
	//	return E_FAIL;
	//}
#pragma endregion

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

	CGameInstance::Release_Engine();
	Safe_Release(m_pImgui_Manager);

	CEffect_Manager::Destroy_Instance();
	
	if (_heapchk() != _HEAPOK)
	{
		MSG_BOX("Heap currupted");
	}
}
