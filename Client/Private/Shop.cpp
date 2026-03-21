#include "Shop.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "UI_Manager.h"
#include "FadeBox.h"
#include "InvenFrame.h"
#include "ShopDesc.h"
#include "ShopWindow.h"
#include "Event_Manager.h"
#include "Camera_Manager.h"

CShop::CShop(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CShop::CShop(const CShop& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CShop::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CShop::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	
	m_fX = 300.f;
	m_fY = (_float)g_ptCenter.y;

	m_fSizeX = 300.f;
	m_fSizeY = 660.f;

	m_fDepth = (_float)D_SHOP / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	
	if (FAILED(Init_ShopItems()))
	{
		return E_FAIL;
	}
	

	return S_OK;
}

void CShop::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	RECT rectUI = {
			  (LONG)(m_fX - m_fSizeX * 0.5f),
			  (LONG)(m_fY - m_fSizeY * 0.5f),
			  (LONG)(m_fX + m_fSizeX * 0.5f),
			  (LONG)(m_fY + m_fSizeY * 0.5f)
	};
	
	if (TRUE == PtInRect(&dynamic_cast<CTextButton*>(m_pExitButton)->Get_Rect(), ptMouse)
		|| PtInRect(&dynamic_cast<CTextButton*>(m_pTitleButton)->Get_Rect(), ptMouse))
	{
		if (m_isActive && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			CFadeBox::FADE_DESC Desc = {};
			Desc.fIn_Duration = 0.5f;
			Desc.fOut_Duration = 1.f;
			Desc.phasFadeCompleted = &m_isReadytoDeactivate;
			CUI_Manager::Get_Instance()->Add_FadeBox(Desc);

			for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
			{
				if (m_pGameInstance->Get_IsLoopingSound(i))
				{
					m_pGameInstance->FadeinSound(i, fTimeDelta);
				}
			}

		}
	}
	
	if (m_isReadytoDeactivate)
	{
		dynamic_cast<CInvenFrame*>(m_pInvenFrame)->Init_SellItem();
		CCamera_Manager::Get_Instance()->Set_CameraState(CS_ENDFULLSCREEN);

		CUI_Manager::Get_Instance()->Set_FullScreenUI(false);
		if (!m_bQuestTrigger && CEvent_Manager::Get_Instance()->Get_QuestTrigger(CEvent_Manager::POTION))
		{
			CEvent_Manager::Get_Instance()->Update_Quest(TEXT("체력포션 구매"));
		}
		m_isActive = false;
	}

	if (m_isReadytoActivate)
	{
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (!m_isActive)
	{
		return;
	}
	if (!m_isFrameExist)
	{
		m_pInvenFrame = CUI_Manager::Get_Instance()->Get_InvenFrame();
		if (m_pInvenFrame)
		{
			dynamic_cast<CInvenFrame*>(m_pInvenFrame)->Set_FrameMode(CInvenFrame::F_SHOP);
			dynamic_cast<CInvenFrame*>(m_pInvenFrame)->Init_State();

			m_isFrameExist = true;
		}

	}

	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
	{
		/* 인벤토리 메뉴 피킹 */
		for (size_t i = 0; i < STATE_END; i++)
		{
			if (PtInRect(&dynamic_cast<CTextButtonColor*>(m_pShopMenu[i])->Get_Rect(), ptMouse))
			{
				m_ePrevShopType = m_eCurShopState;
				m_eCurShopState = (STATE)i;

				if (m_ePrevShopType != m_eCurShopState)
				{
					dynamic_cast<CTextButtonColor*>(m_pShopMenu[m_ePrevShopType])->Set_Alpha(0.6f);
				}
				dynamic_cast<CTextButtonColor*>(m_pShopMenu[i])->Set_Alpha(1.f);
				_vec2 vPos = dynamic_cast<CTextButtonColor*>(m_pShopMenu[i])->Get_Position();
				dynamic_cast<CTextButton*>(m_pSelectButton)->Set_Position(vPos);
				_vec2 fUnderBarPos = dynamic_cast<CTextButton*>(m_pUnderBar)->Get_Position();
				dynamic_cast<CTextButton*>(m_pUnderBar)->Set_Position(_vec2(vPos.x, fUnderBarPos.y));

				Set_ItemPosition(m_eCurShopState);
				break;
			}
		}
	}




	_uint iMoney = CUI_Manager::Get_Instance()->Get_Coin();;
	dynamic_cast<CTextButton*>(m_pMoney)->Set_Text(to_wstring(iMoney));

	_uint iDiamond = CUI_Manager::Get_Instance()->Get_Diamond();;
	dynamic_cast<CTextButton*>(m_pDiamond)->Set_Text(to_wstring(iDiamond));
	

	CUI_Manager::Get_Instance()->Set_FullScreenUI(true);




	for (size_t i = 0; i < m_vecShopItems[m_eCurShopState].size(); i++)
	{
		if (PtInRect(&m_vecShopItems[m_eCurShopState][i]->Get_Rect(), ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::GamePlay))
		{
			if (m_pGameInstance->Get_LayerSize(LEVEL_STATIC, TEXT("Layer_ShopWindow")) == 0)
			{
				CShopWindow::SHOPWINDOW_DESC Desc{};
				ITEM eItem = m_vecShopItems[m_eCurShopState][i]->Get_ItemDesc();
				CItem* pItem = (CItem*)dynamic_cast<CInvenFrame*>(m_pInvenFrame)->Find_Item(eItem.strName);
				if (pItem == nullptr)
				{
					Desc.iCurItemNum = 0;
				}
				else
				{
					Desc.iCurItemNum = pItem->Get_ItemNum();
				}
				Desc.strShopItem = eItem.strName;
				//Desc.iCurItemNum = 
				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_ShopWindow"), TEXT("Prototype_GameObject_ShopWindow"), &Desc)))
				{
					return;
				}
			}
		}
	}


	m_pExitButton->Tick(fTimeDelta);
	if (m_pInvenFrame != nullptr)
	{
		m_pInvenFrame->Tick(fTimeDelta);
	}
	
	m_pBackGround->Tick(fTimeDelta);
	m_pTitleButton->Tick(fTimeDelta);
	m_pUnderBar->Tick(fTimeDelta);
	m_pSelectButton->Tick(fTimeDelta);

	for (size_t i = 0; i < STATE_END; i++)
	{
		m_pShopMenu[i]->Tick(fTimeDelta);
	}
	
	for (auto& iter : m_vecShopItems[m_eCurShopState])
	{
		iter->Tick(fTimeDelta);
	}
	

}

void CShop::Late_Tick(_float fTimeDelta)
{
	if (!m_isActive)
	{
		return;
	}
	m_pMoney->Late_Tick(fTimeDelta);
	m_pDiamond->Late_Tick(fTimeDelta);
	if (m_pInvenFrame != nullptr)
	{
		m_pInvenFrame->Late_Tick(fTimeDelta);
	}

	m_pExitButton->Late_Tick(fTimeDelta);
	m_pBackGround->Late_Tick(fTimeDelta);
	m_pTitleButton->Late_Tick(fTimeDelta);
	m_pUnderBar->Late_Tick(fTimeDelta);
	m_pSelectButton->Late_Tick(fTimeDelta);

	for (size_t i = 0; i < STATE_END; i++)
	{
		m_pShopMenu[i]->Late_Tick(fTimeDelta);
	}
	
	for (auto& iter : m_vecShopItems[m_eCurShopState])
	{
		iter->Late_Tick(fTimeDelta);
	}
	


	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}


	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CShop::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	return S_OK;
}


void CShop::Open_Shop()
{
	if (!m_isActive)
	{
		if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
		{
			return;
		}
		CCamera_Manager::Get_Instance()->Set_CameraState(CS_SHOP);
		m_isActive = true;
		Init_ShopState();

		m_isReadytoActivate = false;
		m_isReadytoDeactivate = false;

	}
}

void CShop::Set_ItemPosition(STATE eState)
{
	return;
}

void CShop::Init_ShopState()
{
	if (m_pInvenFrame != nullptr)
	{
		dynamic_cast<CInvenFrame*>(m_pInvenFrame)->Set_FrameMode(CInvenFrame::F_SHOP);
		dynamic_cast<CInvenFrame*>(m_pInvenFrame)->Init_State();
	}

	_uint iMoney = CUI_Manager::Get_Instance()->Get_Coin();;
	dynamic_cast<CTextButton*>(m_pMoney)->Set_Text(to_wstring(iMoney));

	_uint iDiamond = CUI_Manager::Get_Instance()->Get_Diamond();;
	dynamic_cast<CTextButton*>(m_pDiamond)->Set_Text(to_wstring(iDiamond));
	
	
	m_eCurShopState = EXPENDABLE;
	Set_ItemPosition(m_eCurShopState);

	dynamic_cast<CTextButtonColor*>(m_pShopMenu[EXPENDABLE])->Set_Alpha(1.f);
	dynamic_cast<CTextButtonColor*>(m_pShopMenu[EQUIP])->Set_Alpha(0.6f);
	_vec2 vPos = dynamic_cast<CTextButtonColor*>(m_pShopMenu[EXPENDABLE])->Get_Position();
	dynamic_cast<CTextButton*>(m_pSelectButton)->Set_Position(vPos);
	_vec2 fUnderBarPos = dynamic_cast<CTextButton*>(m_pUnderBar)->Get_Position();
	dynamic_cast<CTextButton*>(m_pUnderBar)->Set_Position(_vec2(vPos.x, fUnderBarPos.y));


}


HRESULT CShop::Add_Parts()
{
	_float fY = 85.f;
	_float fTerm = m_fSizeX / (_uint)STATE_END;
	_float fStartX = fTerm;


	CTextButton::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.fFontSize = 0.5f;
	Button.strText = TEXT("소모품 상점");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Back");
	Button.vPosition = _vec2(20.f, 20.f);
	Button.vSize = _vec2(50.f, 50.f);
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(70.f, 0.f);

	m_pTitleButton =m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pTitleButton)
	{
		return E_FAIL;
	}
	
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Out");
	Button.vPosition = _vec2(1230.f, 30.f);
	Button.vSize = _vec2(70.f, 70.f);
	m_pExitButton = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pExitButton)
	{
		return E_FAIL;
	}

	Button.fDepth = m_fDepth - 0.01f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_InvenUnderBar");
	Button.vSize = _vec2(150.f, 150.f);
	Button.vPosition = _vec2(fStartX, fY + 12.f);
	Button.vTextPosition = _vec2(0.f, 0.f);
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);

	m_pUnderBar = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pUnderBar)
	{
		return E_FAIL;
	}

	Button.fDepth = m_fDepth - 0.02f;
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_SelecInvenMenu");
	Button.vSize = _vec2(fTerm, 34.f);
	Button.vPosition = _vec2(fStartX, fY);
	m_pSelectButton = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pSelectButton)
	{
		return E_FAIL;
	}

	_uint iMoney = CUI_Manager::Get_Instance()->Get_Coin();
	Button.fFontSize = 0.45f;
	Button.strText = to_wstring(iMoney);
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_coin");
	Button.vPosition = _vec2(1080.f, 30.f);
	Button.vSize = _vec2(25.f, 25.f);
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(Button.vSize.x + 30.f, Button.vSize.y - 26.f);

	m_pMoney = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pMoney)
	{
		return E_FAIL;
	}
	_uint iDiamond = CUI_Manager::Get_Instance()->Get_Diamond();;
	Button.strText = to_wstring(iDiamond);
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Diamond");
	Button.vPosition = _vec2(990.f, 30.f);
	Button.vSize = _vec2(25.f, 25.f);
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(Button.vSize.x + 10.f, Button.vSize.y - 26.f);

	m_pDiamond = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pDiamond)
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
	TextButton.strText = TEXT("소모품");
	TextButton.vPosition = _vec2(fStartX, fY);
	TextButton.vSize = _vec2(m_fSizeX / 2.f, 70.f);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_NoTex");
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pShopMenu[EXPENDABLE] = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pShopMenu[EXPENDABLE])
	{
		return E_FAIL;
	}
	dynamic_cast<CTextButtonColor*>(m_pShopMenu[EXPENDABLE])->Set_Pass(VTPass_UI_Alpha);

	TextButton.strText = TEXT("장비");
	TextButton.vPosition = _vec2(fStartX + fTerm, fY);
	TextButton.vSize = _vec2(m_fSizeX / 2.f, 70.f);
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pShopMenu[EQUIP] = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pShopMenu[EQUIP])
	{
		return E_FAIL;
	}
	dynamic_cast<CTextButtonColor*>(m_pShopMenu[EQUIP])->Set_Pass(VTPass_UI_Alpha);

	UiInfo info{};
	info.strTexture = TEXT("Prototype_Component_Texture_BackGround_Mask");
	info.vPos = _vec2((_float)g_iWinSizeX/2.f, (_float)g_iWinSizeY / 2.f);
	info.vSize = _vec2((_float)g_iWinSizeX, (_float)g_iWinSizeY);
	info.iLevel = (_uint)LEVEL_STATIC;

	m_pBackGround = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BackGround_Mask"), &info);
	if (not m_pBackGround)
	{
		return E_FAIL;
	}


	
	m_pInvenFrame = CUI_Manager::Get_Instance()->Get_InvenFrame();
	if (m_pInvenFrame)
	{
		m_isFrameExist = true;
	}

	

	return S_OK;
}

HRESULT CShop::Init_ShopItems()
{

	_float fStartY = 85.f + 34.f + 2.f + 45.f;
	_float fTerm = 2.f;
	_float fShopDescY = 90.f;
	_uint iIndex = 0;
	_uint iEquipIndex = 0;
	CShopDesc::SHOPITEM_DESC ShopDesc = {};
	ShopDesc.fDepth = m_fDepth - 0.01f;
	ShopDesc.strItemName = TEXT("체력 포션");
	ShopDesc.vPosition = _vec2(235.f, fStartY + fShopDescY * iIndex + fTerm * iIndex);
	CShopDesc* pShopDesc = (CShopDesc*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ShopDesc"), &ShopDesc);
	if (not pShopDesc)
	{
		return E_FAIL;
	}
	m_vecShopItems[EXPENDABLE].push_back(pShopDesc);
	
	iIndex++;

	ShopDesc.strItemName = TEXT("마나 포션");
	ShopDesc.vPosition = _vec2(235.f, fStartY + fShopDescY * iIndex + fTerm * iIndex);
	pShopDesc = (CShopDesc*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ShopDesc"), &ShopDesc);
	if (not pShopDesc)
	{
		return E_FAIL;
	}
	m_vecShopItems[EXPENDABLE].push_back(pShopDesc);


	ShopDesc.strItemName = TEXT("마나하임의 갑옷");
	ShopDesc.vPosition = _vec2(235.f, fStartY + fShopDescY * iEquipIndex + fTerm * iEquipIndex);
	pShopDesc = (CShopDesc*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ShopDesc"), &ShopDesc);
	if (not pShopDesc)
	{
		return E_FAIL;
	}
	m_vecShopItems[EQUIP].push_back(pShopDesc);

	iEquipIndex++;

	ShopDesc.strItemName = TEXT("가죽 마스크");
	ShopDesc.vPosition = _vec2(235.f, fStartY + fShopDescY * iEquipIndex + fTerm * iEquipIndex);
	pShopDesc = (CShopDesc*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ShopDesc"), &ShopDesc);
	if (not pShopDesc)
	{
		return E_FAIL;
	}
	m_vecShopItems[EQUIP].push_back(pShopDesc);
	return S_OK;
}


HRESULT CShop::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShop::Bind_ShaderResources()
{
	return S_OK;
}

CShop* CShop::Create(_dev pDevice, _context pContext)
{
	CShop* pInstance = new CShop(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CShop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShop::Clone(void* pArg)
{
	CShop* pInstance = new CShop(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CShop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShop::Free()
{
	__super::Free();


	if (!m_isPrototype)
	{
		for (size_t i = 0; i < STATE_END; i++)
		{
			Safe_Release(m_pShopMenu[i]);
		}

	}

	for (auto& iter : m_vecShopItems[EXPENDABLE])
	{
		Safe_Release(iter);
	}
	m_vecShopItems[EXPENDABLE].clear();
	for (auto& iter : m_vecShopItems[EQUIP])
	{
		Safe_Release(iter);
	}
	m_vecShopItems[EQUIP].clear();


	Safe_Release(m_pMoney);
	Safe_Release(m_pDiamond);
	Safe_Release(m_pBackGround);
	Safe_Release(m_pExitButton);
	Safe_Release(m_pTitleButton);

	Safe_Release(m_pUnderBar);
	Safe_Release(m_pSelectButton);

	Safe_Release(m_pRendererCom);
}