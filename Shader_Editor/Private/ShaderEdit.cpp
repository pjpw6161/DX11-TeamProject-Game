
#include "ShaderEdit.h"
#include "GameInstance.h"
#include "ImGui_Mgr.h"

#include "Shader_Camera.h"
#include "Shader_Loading.h"


CShaderEdit::CShaderEdit()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CShaderEdit::Init()
{
#ifdef _DEBUG
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif // _DEBUG

	GRAPHIC_DESC Desc = {};
	Desc.hWnd = g_hWnd;
	Desc.hInst = g_hInst;
	Desc.iWinSizeX = g_iWinSizeX;
	Desc.iWinSizeY = g_iWinSizeY;
	Desc.isWindowed = true;

	if (FAILED(m_pGameInstance->Init_Engine(LEVEL_END, Desc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	m_pImgui = CImGui_Mgr::Get_Instance();
	if (FAILED(m_pImgui->Init(m_pDevice, m_pContext, m_pGameInstance, m_pRenderer)))
		return E_FAIL;

	if (FAILED(Open_Level()))
		return E_FAIL;

	m_pGameInstance->Initialize_Level(LEVEL_END);

	return S_OK;
}

void CShaderEdit::Tick(_float fTimeDelta)
{
	_float fFinalTimeDelta = fTimeDelta * m_pGameInstance->Get_TimeRatio();

	m_fTimeAcc += fFinalTimeDelta;

	m_pImgui->Frame();

	m_pImgui->Editing(fFinalTimeDelta);

	m_pGameInstance->Tick_Engine(fFinalTimeDelta);
}

HRESULT CShaderEdit::Render()
{
	++m_iFrameCount;

	if (1.f <= m_fTimeAcc) {
		wstring fps = L"FPS: " + to_wstring(m_iFrameCount);

		SetWindowText(g_hWnd, fps.c_str());

		m_fTimeAcc = 0.f;
		m_iFrameCount = 0;
	}

	if (FAILED(m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Clear_DepthStencil_View()))
		return E_FAIL;

	if (FAILED(m_pRenderer->Draw_RenderGroup()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render()))
		return E_FAIL;

	m_pImgui->Render();

	if (FAILED(m_pGameInstance->Present()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShaderEdit::Open_Level()
{
	CLevel* pLevel = CShader_Loading::Create(m_pDevice, m_pContext, LEVEL_ShaderTest);

	if (nullptr == pLevel)
		return E_FAIL;

	return m_pGameInstance->Open_Level(LEVEL_LOADING, pLevel);
}

HRESULT CShaderEdit::Ready_Prototype_Component()
{
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, L"Prototype_Component_Renderer",
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), 
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_Vtxtex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxStatMesh.hlsl"), VTXSTATICMESH::Elements, VTXSTATICMESH::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), 
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Instancing"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxTex_Instancing.hlsl"), VTXPOINT_INSTANCING::Elements, VTXPOINT_INSTANCING::iNumElements))))
	{
		return E_FAIL;
	}

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

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera_Main"), CShader_Camera::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CShaderEdit* CShaderEdit::Create()
{
	CShaderEdit* pInstance = new CShaderEdit();

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CShaderEdit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShaderEdit::Free()
{
	Safe_Release(m_pImgui);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	CGameInstance::Release_Engine();

	if (_heapchk() != _HEAPOK)
	{
		MSG_BOX("Heap currupted");
	}
}
