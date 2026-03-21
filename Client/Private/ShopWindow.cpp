#include "ShopWindow.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "ShopDesc.h"
#include "UI_Manager.h"
#include "Event_Manager.h"

CShopWindow::CShopWindow(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CShopWindow::CShopWindow(const CShopWindow& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CShopWindow::Init_Prototype()
{
	return S_OK;
}

HRESULT CShopWindow::Init(void* pArg)
{

	m_iItemNum = ((SHOPWINDOW_DESC*)pArg)->iCurItemNum;
	m_strShopItem = ((SHOPWINDOW_DESC*)pArg)->strShopItem;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 500.f;
	m_fSizeY = 500.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = (_float)g_iWinSizeY / 2.f - 70.f;


	m_fDepth = (_float)D_SHOP / (_float)D_END - 0.05f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	

	m_rcRect = {
		  (LONG)(m_fX - m_fSizeX * 0.5f),
		  (LONG)(m_fY - m_fSizeY * 0.5f),
		  (LONG)(m_fX + m_fSizeX * 0.5f),
		  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	m_iAvailable = m_pShopDesc->Get_ItemDesc().iLimited;
	_vec2 vStartPos = _vec2(m_fX - m_pNumBar->Get_Size().x / 2.f, m_pNumBar->Get_Position().y);

	m_pNumButtom->Set_Position(vStartPos);
	m_pNumBar->Set_Alpha(0.f);
	return S_OK;
}

void CShopWindow::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);


	if (PtInRect(&m_pExitButton->Get_Rect(), ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
	{
		m_isDead = true;
		return;
	}


	if (PtInRect(&m_pExitButton->Get_InitialRect(), ptMouse))
	{
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
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
			if (m_iCurItemNum == 0)
			{
				CEvent_Manager::Get_Instance()->Set_Alert(TEXT("수량을 선택해 주세요."));
				return;
			}

			if (m_iAvailable - m_iItemNum == 0)
			{
				CEvent_Manager::Get_Instance()->Set_Alert(TEXT("구매 가능한 수량이 없습니다."));
				return;
			}		
			
			if (CUI_Manager::Get_Instance()->Get_Coin() < m_iTotalCost)
			{
				CEvent_Manager::Get_Instance()->Set_Alert(TEXT("코인이 부족하여 구매할 수 없습니다."));
				m_pGameInstance->Play_Sound(TEXT("Auto_Item_Fail"));
				return;
			}

			_int iCoin = m_iTotalCost * -1;
			CUI_Manager::Get_Instance()->Set_Coin(iCoin);
			ITEM eItem = m_pShopDesc->Get_ItemDesc();
			CUI_Manager::Get_Instance()->Set_Item(eItem.strName, m_iCurItemNum);
			if (eItem.strName == TEXT("체력 포션") && CEvent_Manager::Get_Instance()->Find_Quest(TEXT("체력포션 구매")))
			{
				CEvent_Manager::Get_Instance()->Set_QuestTrigger(CEvent_Manager::POTION);
			}
			m_isDead = true;

			m_pGameInstance->Play_Sound(TEXT("btn_purchase"));

			return;
		}

		m_pSelectButton->Set_Size(140.f, 80.f, 0.3f);
	}
	else
	{
		m_pSelectButton->Set_Size(150.f, 100.f, 0.35f);
	}



	_float fPercent = m_pNumBar->Get_Size().x / ((_float)m_iAvailable - (_float)m_iItemNum);
	if (PtInRect(&m_pNumButtom->Get_Rect(), ptMouse) && m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
	{
		m_isPickingButton = true;
	}

	if (m_isPickingButton)
	{
		CUI_Manager::Get_Instance()->Set_MouseState(CUI_Manager::M_GRAB);
		_float fMouseSensor = 0.1f;
		_long dwMouseMove;
		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
		{
			_vec2 vPos = _vec2(m_pNumButtom->Get_TransPosition().x + dwMouseMove * fMouseSensor * 5.5f, m_pNumBar->Get_Position().y);
			m_pNumButtom->Set_Position(vPos);
			if (m_pNumButtom->Get_TransPosition().x <= m_pNumBar->Get_Position().x - m_pNumBar->Get_Size().x / 2.f)
			{
				if (dwMouseMove < 0.f)
				{
					m_pNumButtom->Set_Position(_vec2(m_pNumBar->Get_Position().x - m_pNumBar->Get_Size().x / 2.f, m_pNumBar->Get_Position().y));
					return;
				}
			}
			if (m_pNumButtom->Get_TransPosition().x > m_pNumBar->Get_Position().x + m_pNumBar->Get_Size().x / 2.f)
			{
				if (dwMouseMove > 0.f)
				{
					m_pNumButtom->Set_Position(_vec2(m_pNumBar->Get_Position().x + m_pNumBar->Get_Size().x / 2.f + fPercent, m_pNumBar->Get_Position().y));
					return;
				}
			}

			m_iCurItemNum = (_uint)((m_pNumButtom->Get_TransPosition().x - (m_pNumBar->Get_Position().x - m_pNumBar->Get_Size().x / 2.f)) / fPercent);
			if (m_iCurItemNum <= 0)
			{
				m_iCurItemNum = 0;
			}
		}
	}

	if (m_isPickingButton && m_pGameInstance->Mouse_Up(DIM_LBUTTON))
	{
		m_isPickingButton = false;
	}

	if (PtInRect(&m_pPlusBtn->Get_InitialRect(), ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
	{
		m_pPickingCircle->Set_Position(m_pPlusBtn->Get_Position());
		if (m_iCurItemNum < m_iAvailable - (_float)m_iItemNum)
		{
			m_iCurItemNum++;
			m_pNumButtom->Set_Position(_vec2(m_pNumButtom->Get_TransPosition().x + fPercent, m_pNumBar->Get_Position().y));
		}
	}
	if (PtInRect(&m_pMinusBtn->Get_InitialRect(), ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
	{
		m_pPickingCircle->Set_Position(m_pMinusBtn->Get_Position());
		if (m_iCurItemNum > 0)
		{
			m_iCurItemNum--;
			m_pNumButtom->Set_Position(_vec2(m_pNumButtom->Get_TransPosition().x - fPercent, m_pNumBar->Get_Position().y));
		}
	}

	if (PtInRect(&m_pPlusBtn->Get_InitialRect(), ptMouse) && m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
	{
		m_isPickingCircle = true;
	}
	else if (PtInRect(&m_pMinusBtn->Get_InitialRect(), ptMouse) && m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
	{
		m_isPickingCircle = true;
	}
	else
	{
		m_isPickingCircle = false;
	}


	_float fRatio = (_float)m_iCurItemNum * fPercent / m_pNumBar->Get_Size().x;
	m_pNumBar->Set_Alpha(fRatio);

	if (m_isPickingCircle)
	{
		m_pPickingCircle->Tick(fTimeDelta);
	}
	m_pPlusBtn->Tick(fTimeDelta);
	m_pMinusBtn->Tick(fTimeDelta);
	m_pCoinTex->Tick(fTimeDelta);
	m_pNumBar->Tick(fTimeDelta);
	m_pNumButtom->Tick(fTimeDelta);
	m_pShopDesc->Tick(fTimeDelta);
	m_pSelectButton->Tick(fTimeDelta);
	m_pExitButton->Tick(fTimeDelta);
	//__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CShopWindow::Late_Tick(_float fTimeDelta)
{
	if (m_isPickingCircle)
	{
		m_pPickingCircle->Late_Tick(fTimeDelta);
	}
	m_pPlusBtn->Late_Tick(fTimeDelta);
	m_pMinusBtn->Late_Tick(fTimeDelta);
	m_pCoinTex->Late_Tick(fTimeDelta);
	m_pNumBar->Late_Tick(fTimeDelta);
	m_pNumButtom->Late_Tick(fTimeDelta);
	m_pShopDesc->Late_Tick(fTimeDelta);
	m_pSelectButton->Late_Tick(fTimeDelta);
	m_pExitButton->Late_Tick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

}

HRESULT CShopWindow::Render()
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

	_vec2 vStartPos = _vec2(m_fX - 60.f, m_fY - 28.f);
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("구매 가능 수량 : "), _vec2(vStartPos.x + 1.f, vStartPos.y), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true );
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("구매 가능 수량 : "), _vec2(vStartPos.x, +vStartPos.y + 1.f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("구매 가능 수량 : "), _vec2(vStartPos.x, +vStartPos.y), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	
	vStartPos = _vec2(m_fX + 20.f, vStartPos.y);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iAvailable - m_iItemNum) + TEXT(" / ") + to_wstring(m_iAvailable), _vec2(vStartPos.x + 1.f, vStartPos.y), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true );
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iAvailable - m_iItemNum) + TEXT(" / ") + to_wstring(m_iAvailable), _vec2(vStartPos.x, +vStartPos.y + 1.f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iAvailable - m_iItemNum) + TEXT(" / ") + to_wstring(m_iAvailable), _vec2(vStartPos.x, +vStartPos.y), 0.3f, _vec4(1.f, 0.8f, 0.f, 1.f), 0.f, true);
	

	vStartPos = _vec2(m_fX, m_fY + 10.f);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iCurItemNum) + TEXT("개"), _vec2(vStartPos.x + 1.f, vStartPos.y), 0.5f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iCurItemNum) + TEXT("개"), _vec2(vStartPos.x, +vStartPos.y + 1.f), 0.5f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iCurItemNum) + TEXT("개"), _vec2(vStartPos.x, +vStartPos.y), 0.5f, _vec4(1.f, 0.8f, 0.f, 1.f));
	
	vStartPos = _vec2(m_fX- 30.f, m_fY + 65.f);
	_uint iPurchase = m_pShopDesc->Get_ItemDesc().iPurchase;
	m_iTotalCost = iPurchase * m_iCurItemNum;
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iTotalCost), _vec2(vStartPos.x + 1.f, vStartPos.y), 0.5f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iTotalCost), _vec2(vStartPos.x, +vStartPos.y + 1.f), 0.5f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iTotalCost), _vec2(vStartPos.x, +vStartPos.y), 0.5f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	
	vStartPos = _vec2(m_fX, m_fY - 150.f);
	m_pGameInstance->Render_Text(L"Font_Malang",TEXT("구매"), _vec2(vStartPos.x + 1.f, vStartPos.y), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang",TEXT("구매"), _vec2(vStartPos.x, vStartPos.y + 1.f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang",TEXT("구매"), _vec2(vStartPos.x, vStartPos.y), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f));

	/*
	vStartPos = _vec2(m_fX + 14.f, m_fY + 20.f);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iNum), _vec2(vStartPos.x + 1.f, vStartPos.y), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iNum), _vec2(vStartPos.x, +vStartPos.y + 1.f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iNum), _vec2(vStartPos.x, +vStartPos.y), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f));
	*/
	return S_OK;
}


HRESULT CShopWindow::Add_Parts()
{
	CTextButton::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.strText = TEXT("취소");
	Button.fFontSize = 0.4f;
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(0.f, -2.f);
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Button_Blue");
	Button.vPosition = _vec2(m_fX - 80.f, m_fY + 140.f);
	Button.vSize = _vec2(150.f, 100.f);

	m_pExitButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pExitButton)
	{
		return E_FAIL;
	}
	Button.strText = TEXT("구매");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Button_Blue2");
	Button.vPosition = _vec2(m_fX + 80.f, m_fY + 140.f);

	m_pSelectButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pSelectButton)
	{
		return E_FAIL;
	}
	Button.strText = TEXT("");
	Button.fDepth = m_fDepth - 0.02f;
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_numbutton");
	Button.vPosition = _vec2(m_fX, m_fY);
	Button.vSize = _vec2(20.f, 20.f);

	m_pNumButtom = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pNumButtom)
	{
		return E_FAIL;
	}
	Button.strText = TEXT("");
	Button.fDepth = m_fDepth - 0.02f;
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_plus");
	Button.vPosition = _vec2(m_fX + 190.f, m_fY + 40.f);
	Button.vSize = _vec2(25.f, 25.f);

	m_pPlusBtn = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pPlusBtn)
	{
		return E_FAIL;
	}
	Button.strText = TEXT("");
	Button.fDepth = m_fDepth - 0.02f;
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_minus");
	Button.vPosition = _vec2(m_fX - 190.f, m_fY + 40.f);
	Button.vSize = _vec2(25.f, 25.f);

	m_pMinusBtn = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pMinusBtn)
	{
		return E_FAIL;
	}

	Button.strText = TEXT("");
	Button.fDepth = m_fDepth - 0.01f;
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_coin");
	Button.vPosition = _vec2(m_fX - 45.f, m_fY + 80.f);
	Button.vSize = _vec2(30.f, 30.f);

	m_pCoinTex = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pCoinTex)
	{
		return E_FAIL;
	}

	Button.strText = TEXT("");
	Button.fDepth = m_fDepth - 0.01f;
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_pickingcircle");
	Button.vPosition = _vec2(m_fX - 45.f, m_fY + 80.f);
	Button.vSize = _vec2(25.f, 25.f);

	m_pPickingCircle = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pPickingCircle)
	{
		return E_FAIL;
	}


	CTextButtonColor::TEXTBUTTON_DESC TextButton = {};
	TextButton.eLevelID = LEVEL_STATIC;
	TextButton.strTexture = TEXT("");
	TextButton.fDepth = m_fDepth - 0.01f;
	TextButton.fAlpha = 1.f;
	TextButton.fFontSize = 0.45f;
	TextButton.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.strText = TEXT("");
	TextButton.vSize = _vec2(m_fSizeX - 200.f, 3.f);
	TextButton.vPosition = _vec2(m_fX, m_fY + 40.f);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_numbar");
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pNumBar = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
	if (not m_pNumBar)
	{
		return E_FAIL;
	}
	m_pNumBar->Set_Pass(VTPass_HPNoMask);


	CShopDesc::SHOPITEM_DESC ShopDesc = {};
	ShopDesc.fDepth = m_fDepth - 0.01f;
	ShopDesc.strItemName = m_strShopItem;
	ShopDesc.vPosition = _vec2(m_fX, m_fY - 70.f);
	m_pShopDesc = (CShopDesc*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ShopDesc"), &ShopDesc);
	if (not m_pShopDesc)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShopWindow::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_DescWindow"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CShopWindow::Bind_ShaderResources()
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

CShopWindow* CShopWindow::Create(_dev pDevice, _context pContext)
{
	CShopWindow* pInstance = new CShopWindow(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShopWindow::Clone(void* pArg)
{
	CShopWindow* pInstance = new CShopWindow(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShopWindow::Free()
{
	__super::Free();

	Safe_Release(m_pPlusBtn);
	Safe_Release(m_pMinusBtn);
	Safe_Release(m_pNumBar);
	Safe_Release(m_pCoinTex);
	Safe_Release(m_pNumButtom);

	Safe_Release(m_pShopDesc);
	Safe_Release(m_pExitButton);
	Safe_Release(m_pSelectButton);
	Safe_Release(m_pPickingCircle);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
