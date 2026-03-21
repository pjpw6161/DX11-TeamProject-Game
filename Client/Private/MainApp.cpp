#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Loading.h"
#include "Loading_Horse.h"
#include "Mouse.h"
#include "Camera_Main.h"
#include "Camera_CutScene.h"
#include "CutScene_Curve.h"
#include "UI_Manager.h"
#include "Effect_Manager.h"
#include "Event_Manager.h"
#include "Trigger_Manager.h"
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

#ifdef _DEBUG
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif // _DEBUG


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

	if (FAILED(Ready_Prototype_For_Loading()))
	{
		return E_FAIL;
	}
	/* 마우스 생성 */
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Mouse"), TEXT("Prototype_GameObject_Mouse"))))
	{
		return E_FAIL;
	}
	if (FAILED(Open_Level(LEVEL_LOGO)))
	{
		return E_FAIL;
	}

	CEffect_Manager::Get_Instance()->Register_Callback();


	//(_float)D_SCREEN1 / (_float)D_END
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

#ifdef _DEBUG
	m_pGameInstance->Get_StringStream() << "FPS: " << m_pGameInstance->Get_FPS() << endl;
	m_pGameInstance->Get_StringStream() << endl;
#endif // _DEBUG


	m_pGameInstance->Tick_Engine(fFinalTimeDelta);
	CTrigger_Manager::Get_Instance()->Tick(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	++m_iFrameCount;

	if (m_fTimeAcc >= 1.f)
	{
		wstring fps = L"FPS: " + to_wstring(m_iFrameCount);
		m_pGameInstance->Set_FPS(m_iFrameCount);

		SetWindowText(g_hWnd, fps.c_str());

		m_fTimeAcc = 0.f;
		m_iFrameCount = 0;
	}

	if (FAILED(m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f))))
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

	//원명
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect_LookZ"), CVIBuffer_Rect_LookZ::Create(m_pDevice, m_pContext))))
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

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Curve"), CVIBuffer_Curve::Create(m_pDevice, m_pContext))))

	{
		return E_FAIL;
	}
	
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail_50"), CVIBuffer_Trail::Create(m_pDevice, m_pContext, 50, _float2(0.01f, 0.01f)))))

	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail_Surface_50"), CVIBuffer_Trail_Surface::Create(m_pDevice, m_pContext, 50))))

	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Vtxtex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Effect"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Vtxtex_Effect.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
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

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Trail_Surface"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Vtxtex_Trail_Surface.hlsl"), VTXTRAILSURFACE::Elements, VTXTRAILSURFACE::iNumElements))))
	{
		return E_FAIL;
	}

	//MakeSpriteFont "HY헤드라인M" /FontSize:30 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 138ex.spritefont
	//MakeSpriteFont "나눔고딕 ExtraBold" /FontSize:30 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 NanumBold.spritefont
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
	
	if (FAILED(m_pGameInstance->Add_Font(L"Font_UI", L"../../Client/Bin/Resources/Font/NanumGothic.spritefont")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), CCollider::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	/* Mouse */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mouse"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Mouse/MouseCursor%d.png"), 3))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Mouse"), CMouse::Create(m_pDevice, m_pContext))))
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
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera_Curve"), CCutScene_Curve::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_For_Loading()
{
#pragma region Textures
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Bg_Dungeon"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/Loading_Images/Bg_Dungeon (%d).dds"), 8))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_LH_Loading"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Loading/LH_Loading.png")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Loading_Horse"), CLoading_Horse::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Loading"), CLoading::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	// 
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
	CCamera_Manager::Destroy_Instance();
	CTrigger_Manager::Destroy_Instance();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	CGameInstance::Release_Engine();

	CEffect_Manager::Destroy_Instance();
	CEvent_Manager::Destroy_Instance();
	CUI_Manager::Destroy_Instance();

	if (_heapchk() != _HEAPOK)
	{
		MSG_BOX("Heap currupted");
	}
}
