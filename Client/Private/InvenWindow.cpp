#include "InvenWindow.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"

CInvenWindow::CInvenWindow(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CInvenWindow::CInvenWindow(const CInvenWindow& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CInvenWindow::Init_Prototype()
{
	return S_OK;
}

HRESULT CInvenWindow::Init(void* pArg)
{

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 500.f;
	m_fSizeY = 500.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = (_float)g_iWinSizeY / 2.f - 50.f;

	m_fDepth = (_float)D_INVEN / (_float)D_END - 0.05f;
	
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	
	//CGameInstance::Get_Instance()->Set_InputString(L"");

	//CGameInstance::Get_Instance()->Set_InputString(CompleteString + composingString);

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CInvenWindow::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	CUI_Manager::Get_Instance()->Set_Picking_UI(true);

	if (PtInRect(&m_pExitButton->Get_InitialRect(), ptMouse))
	{
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			CUI_Manager::Get_Instance()->Set_Picking_UI(false);
			m_bSell = false;
			m_isActive = false;
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
			if (m_isActive)
			{
				CUI_Manager::Get_Instance()->Set_Coin(m_iCost);
				m_bSell = true;
				m_isActive = false;
			}
			return;
		}

		m_pSelectButton->Set_Size(140.f, 80.f, 0.3f);
	}
	else
	{
		m_pSelectButton->Set_Size(150.f, 100.f, 0.35f);
	}

	m_pMoney->Tick(fTimeDelta);
	m_pExitButton->Tick(fTimeDelta);
	m_pSelectButton->Tick(fTimeDelta);
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CInvenWindow::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

	m_pMoney->Late_Tick(fTimeDelta);
	m_pExitButton->Late_Tick(fTimeDelta);
	m_pSelectButton->Late_Tick(fTimeDelta);

}

HRESULT CInvenWindow::Render()
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

	_vec2 vStartPos = _vec2(m_fX, 280.f);

	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("선택한 아이템을 판매하시겠습니까?"), _vec2(vStartPos.x, vStartPos.y), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f));
	
	vStartPos = _vec2(m_fX - 30.f, m_fY + 16.f);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iCost), _vec2(vStartPos.x + 1.f, vStartPos.y), 0.5f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iCost), _vec2(vStartPos.x, +vStartPos.y + 1.f), 0.5f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iCost), _vec2(vStartPos.x, +vStartPos.y), 0.5f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);


	return S_OK;
}


HRESULT CInvenWindow::Add_Parts()
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
	Button.strText = TEXT("판매");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Button_Blue2");
	Button.vPosition = _vec2(m_fX + 80.f, m_fY + 90.f);

	m_pSelectButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pSelectButton)
	{
		return E_FAIL;
	}
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_coin");
	Button.vPosition = _vec2(m_fX - 45.f, m_fY + 30.f);
	Button.vSize = _vec2(30.f, 30.f);
	m_pMoney = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pMoney)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CInvenWindow::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_smallWindow"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CInvenWindow::Bind_ShaderResources()
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

CInvenWindow* CInvenWindow::Create(_dev pDevice, _context pContext)
{
	CInvenWindow* pInstance = new CInvenWindow(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInvenWindow::Clone(void* pArg)
{
	CInvenWindow* pInstance = new CInvenWindow(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInvenWindow::Free()
{
	__super::Free();

	Safe_Release(m_pMoney);
	Safe_Release(m_pExitButton);
	Safe_Release(m_pSelectButton);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
