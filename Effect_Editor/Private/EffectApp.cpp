#include "EffectApp.h"
#include "GameInstance.h"
#include "Imgui_Manager.h"
#include "LoadingImg.h"
#include "Terrain.h"
#include "Effect_Dummy.h"
#include "Camera_Main.h"
#include "Select_Map.h"
#include "BackGround_Effect.h"

CEffectApp::CEffectApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

_uint APIENTRY ThreadEntry(void* pArg)
{
	if (FAILED(CoInitializeEx(nullptr, 0)))
	{
		return FALSE;
	}

	CEffectApp* pApp = reinterpret_cast<CEffectApp*>(pArg);

	pApp->Begin_Thread();

	if (FAILED(pApp->Ready_Prototype_GameObject()))
	{
		return FALSE;
	}

	pApp->End_Thread();

	return TRUE;
}

HRESULT CEffectApp::Init()
{
	if (not m_pGameInstance)
	{
		return E_FAIL;
	}

	RECT rc{};
	GetClientRect(g_hWnd, &rc);

	GRAPHIC_DESC GraphicDesc;
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.hInst = g_hInst;
	GraphicDesc.iWinSizeX = rc.right - rc.left;
	GraphicDesc.iWinSizeY = rc.bottom - rc.top;
	GraphicDesc.isWindowed = TRUE;

	m_pGameInstance->Init_Engine(LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext);

	if (FAILED(Ready_Prototype_Component_For_Static()))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Loading_Screen()))
	{
		return E_FAIL;
	}

	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, ThreadEntry, this, 0, nullptr));
	if (!m_hThread)
	{
		return E_FAIL;
	}

	return S_OK;
}

void CEffectApp::Tick(_float fTimeDelta)
{
	if (!m_pGameInstance)
	{
		return;
	}

	_float fFinalTimeDelta = fTimeDelta * m_pGameInstance->Get_TimeRatio();

	m_fTimeAcc += fFinalTimeDelta;
	m_fLoadingTime += fTimeDelta;

	m_strLoadingText = L"이펙트 툴 로딩중";

	if (m_fLoadingTime >= 0.3f)
	{
		if (m_iNumDots > 2)
		{
			m_iNumDots = 0;
		}
		m_iNumDots++;

		m_fLoadingTime = 0.f;
	}
	for (size_t i = 0; i < m_iNumDots; i++)
	{
		m_strLoadingText += L".";
	}

	if (m_bLoadComplete)
	{
		if (!m_pImguiMgr)
		{
			(m_pImguiMgr = CImgui_Manager::Get_Instance())->Init(m_pDevice, m_pContext, &m_TextureList, &m_ModelList);
			Safe_AddRef(m_pImguiMgr);
		}
	}

	m_pGameInstance->Tick_Engine(fFinalTimeDelta);

	if (m_pImguiMgr)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		m_pImguiMgr->Tick(fFinalTimeDelta);
	}
}

HRESULT CEffectApp::Render()
{
	++m_iFrameCount;

	if (m_fTimeAcc >= 1.f)
	{
		wstring fps = to_wstring(m_iFrameCount);

		SetWindowText(g_hWnd, fps.c_str());

		m_fTimeAcc = 0.f;
		m_iFrameCount = 0;
	}

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pRenderer->Draw_RenderGroup();

	if (!m_bLoadComplete)
	{
		m_pGameInstance->Render_Text(L"Font_Malang", m_strLoadingText, _float2(300.f, 300.f), 1.f);
	}

	if (m_pImguiMgr)
	{
		if (FAILED(m_pImguiMgr->Render()))
		{
			return E_FAIL;
		}
	}

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CEffectApp::Ready_Prototype_Component_For_Static()
{
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

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh_Effect"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxStatMesh_Effect.hlsl"), VTXSTATICMESH::Elements, VTXSTATICMESH::iNumElements))))
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

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, L"Prototype_Component_Texture_Terrain", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Terrain.png"))))
	{
		return E_FAIL;
	}

#pragma region Prototype
#pragma endregion


	return S_OK;
}

HRESULT CEffectApp::Ready_Loading_Screen()
{
	if (FAILED(m_pGameInstance->Add_Font(L"Font_Malang", L"../../Client/Bin/Resources/Font/Naruto.spritefont")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, L"Prototype_Component_Texture_Loading_Screen", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Loading.dds"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(L"Prototype_GameObject_LoadingImg", CLoadingImg::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, L"Layer_LoadingImg", L"Prototype_GameObject_LoadingImg", &m_bLoadComplete)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CEffectApp::Begin_Thread()
{
	EnterCriticalSection(&m_Critical_Section);
}

HRESULT CEffectApp::Ready_Prototype_GameObject()
{
	string strInputFilePath = "../../Client/Bin/Resources/Textures/Effect/";
	_uint iTextureNumber{};
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Component_Texture_Effect_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CTexture::Create(m_pDevice, m_pContext, entry.path().wstring()))))
			{
				return E_FAIL;
			}

			m_TextureList.push_back(entry.path().stem().string());
		}
	}
#pragma region Model
	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Effect/Mesh/";
	_uint iMeshNumber{};
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_Effect_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
			m_ModelList.push_back(entry.path().stem().string());
		}
	}

	_mat PivotMat = _mat::CreateScale(0.006f) * _mat::CreateRotationX(XMConvertToRadians(90.f));

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Select_Map"),
														CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/StaticMesh/Select_Map/Mesh/map.hyuntrastatmesh", false, PivotMat))))
	{
		return E_FAIL;
	}
#pragma endregion

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Dummy"), CEffect_Dummy::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera"), CCamera_Main::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Map"), CSelect_Map::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_BG"), CBackGround::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	m_bLoadComplete = true;

	return S_OK;
}

void CEffectApp::End_Thread()
{
	LeaveCriticalSection(&m_Critical_Section);
}

CEffectApp* CEffectApp::Create()
{
	CEffectApp* pInstance = new CEffectApp();

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CEffectApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectApp::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);

	CloseHandle(m_hThread);

	Safe_Release(m_pImguiMgr);
	CImgui_Manager::Destroy_Instance();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	CGameInstance::Release_Engine();
}
