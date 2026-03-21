#include "NameWindow.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"

CNameWindow::CNameWindow(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CNameWindow::CNameWindow(const CNameWindow& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CNameWindow::Init_Prototype()
{
	return S_OK;
}

HRESULT CNameWindow::Init(void* pArg)
{

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 500.f;
	m_fSizeY = 500.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = (_float)g_iWinSizeY / 2.f - 50.f;

	m_fDepth = 0.3f;
	
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	
	m_pGameInstance->Set_InputString(L"");

	//CGameInstance::Get_Instance()->Set_InputString(CompleteString + composingString);

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CNameWindow::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	CUI_Manager::Get_Instance()->Set_Picking_UI(true);


	if (PtInRect(&m_pExitButton->Get_InitialRect(), ptMouse))
	{
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			m_pGameInstance->Set_InputString(L"");
			CUI_Manager::Get_Instance()->Set_Picking_UI(false);
			m_isDead = true;
			return;
		}
		m_pExitButton->Set_Size(140.f, 80.f, 0.3f);
	}
	else
	{
		m_pExitButton->Set_Size(150.f, 100.f, 0.35f);
	}
	if (PtInRect(&m_pSelectButton->Get_InitialRect(), ptMouse))
	{
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			CUI_Manager::Get_Instance()->Set_Picking_UI(false);
			m_isDead = true;
			m_pGameInstance->Level_ShutDown(LEVEL_CUSTOM);
			return;
		}

		m_pSelectButton->Set_Size(140.f, 80.f, 0.3f);
	}
	else
	{
		m_pSelectButton->Set_Size(150.f, 100.f, 0.35f);
	}

	if (m_pGameInstance->Key_Down(DIK_RETURN, InputChannel::UI))
	{
		CUI_Manager::Get_Instance()->Set_Picking_UI(false);
		m_isDead = true;
		m_pGameInstance->Level_ShutDown(LEVEL_CUSTOM);
		return;
	}
	RECT rcNameSpace = {};

	rcNameSpace = {
		  (LONG)(m_fX - m_fSizeX / 2.f * 0.5f),
		  (LONG)(267.f - 30.f * 0.5f),
		  (LONG)(m_fX + m_fSizeX / 2.f * 0.5f),
		  (LONG)(267.f + 30.f * 0.5f)
	};

	if (PtInRect(&rcNameSpace, ptMouse))
	{
		CUI_Manager::Get_Instance()->Set_MouseState(CUI_Manager::M_TEXT);
	}

	if (m_pGameInstance->Get_InputString() != L"")
	{
		m_bStartInput = true;
	}
	if (m_pGameInstance->Get_InputString() == L"" && PtInRect(&rcNameSpace, ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::Default))
	{
		m_bStartInput = true;
	}
	if (m_pGameInstance->Get_InputString() == L"" && !PtInRect(&rcNameSpace, ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::Default))
	{
		m_bStartInput = false;
	}

	if (m_pGameInstance->Key_Down(DIK_BACKSPACE, InputChannel::UI))
	{
		m_pGameInstance->Popback_InputString();
	}

	m_pExitButton->Tick(fTimeDelta);
	m_pSelectButton->Tick(fTimeDelta);
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CNameWindow::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

	m_pExitButton->Late_Tick(fTimeDelta);
	m_pSelectButton->Late_Tick(fTimeDelta);

}

HRESULT CNameWindow::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_UI)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}
	_vec2 vStartPos = _vec2(m_fX, 267.f);
	if (!m_bStartInput)
	{
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("닉네임을 입력하세요."), _vec2(vStartPos.x, vStartPos.y), 0.35f, _vec4(0.6f, 0.6f, 0.6f, 1.f));
	}
	else
	{
		m_pGameInstance->Render_Text(L"Font_Malang", m_pGameInstance->Get_InputString(), _vec2(vStartPos.x, vStartPos.y), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f));
	}
	/*
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	wstring strTest = to_wstring((_uint)ptMouse.x) + to_wstring((_uint)ptMouse.y);
	m_pGameInstance->Render_Text(L"Font_Malang", strTest, _vec2(ptMouse.x + 20.f, ptMouse.y + 20.f), 0.7f, _vec4(0.7f, 0.7f, 1.f, 1.f));
	*/
	return S_OK;
}


HRESULT CNameWindow::Add_Parts()
{
	CTextButton::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.strText = TEXT("취소");
	Button.fFontSize = 0.4f;
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(0.f, - 2.f);
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Button_Blue");
	Button.vPosition = _vec2(m_fX - 80.f, m_fY + 90.f);
	Button.vSize = _vec2(150.f, 100.f);

	m_pExitButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pExitButton)
	{
		return E_FAIL;
	}
	Button.strText = TEXT("확인");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Button_Blue2");
	Button.vPosition = _vec2(m_fX + 80.f, m_fY + 90.f);

	m_pSelectButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pSelectButton)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNameWindow::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_NameWindow"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CNameWindow::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	return S_OK;
}

CNameWindow* CNameWindow::Create(_dev pDevice, _context pContext)
{
	CNameWindow* pInstance = new CNameWindow(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNameWindow::Clone(void* pArg)
{
	CNameWindow* pInstance = new CNameWindow(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNameWindow::Free()
{
	__super::Free();

	Safe_Release(m_pExitButton);
	Safe_Release(m_pSelectButton);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
