#include "Inven.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"
#include "InvenFrame.h"
#include "Event_Manager.h"
#include "Camera_Manager.h"

CInven::CInven(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CInven::CInven(const CInven& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CInven::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CInven::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	
	m_fSizeX = 70.f;
	m_fSizeY = 70.f;
	m_fX = 1160.f;
	m_fY = 45.f;
	m_fDepth = (_float)D_INVEN / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	HRESULT hr = CUI_Manager::Get_Instance()->Set_Inven(this);
	if (FAILED(hr))
	{
		return E_FAIL;
	}
	/*
	
	wstring strItem = TEXT("Ã¼·Â Æ÷¼Ç");
	CUI_Manager::Get_Instance()->Set_Item(strItem, 80);
	
	strItem = TEXT("¸¶³ª Æ÷¼Ç");
	CUI_Manager::Get_Instance()->Set_Item(strItem, 60);
	
	strItem = TEXT("±×³É¸ðÀÚ");
	CUI_Manager::Get_Instance()->Set_Item(strItem);
	
	strItem = TEXT("¾ÈÈçÇÑ¸ðÀÚ");
	CUI_Manager::Get_Instance()->Set_Item(strItem);
	
	strItem = TEXT("À¯´ÏÅ©Çï¸ä");
	CUI_Manager::Get_Instance()->Set_Item(strItem);
	
	strItem = TEXT("À¯´ÏÅ©ÇÑ¿Ê");
	CUI_Manager::Get_Instance()->Set_Item(strItem);
	
	strItem = TEXT("½ÅÈ­¿Ê");
	CUI_Manager::Get_Instance()->Set_Item(strItem);
	*/

	wstring strItem = TEXT("°¡Á× ¿Ê");
	ITEM eItem = CUI_Manager::Get_Instance()->Find_Item(strItem);
	m_pWearableSlots[W_CHEST]->Set_WearableItem(eItem);

	//strItem = TEXT("¹«½º »Ô ´Ü°Ë");
	//eItem = CUI_Manager::Get_Instance()->Find_Item(strItem);
	//m_pWearableSlots[W_SWORD]->Set_WearableItem(eItem);

	strItem = TEXT("¹«½º »Ô È°");
	CUI_Manager::Get_Instance()->Set_Item(strItem);	
	
	strItem = TEXT("¿ÀµòÀÇ ±Ã´Ï¸£ È°");
	CUI_Manager::Get_Instance()->Set_Item(strItem);	
	
	strItem = TEXT("¹«½º »Ô ´Ü°Ë");
	CUI_Manager::Get_Instance()->Set_Item(strItem);	
	
	strItem = TEXT("¿ÀµòÀÇ ±Ã´Ï¸£ ´Ü°Ë");
	CUI_Manager::Get_Instance()->Set_Item(strItem);	

	strItem = TEXT("°ÅÀÎÀÇ °­Ã¶ ´Ü°Ë");
	CUI_Manager::Get_Instance()->Set_Item(strItem);

	strItem = TEXT("Æø±º ¼öµå¸®ÀÇ ´Ü°Ë");
	CUI_Manager::Get_Instance()->Set_Item(strItem);

	strItem = TEXT("ÇìÀÓ´ÞÀÇ ´Ü°Ë");
	CUI_Manager::Get_Instance()->Set_Item(strItem);

	strItem = TEXT("·¹±ä·¹ÀÌÇÁÀÇ ºÒ¸ê °©¿Ê");
	CUI_Manager::Get_Instance()->Set_Item(strItem);	

	strItem = TEXT("¿¤µå·éÀÇ ¼öÈ£ °©¿Ê");
	CUI_Manager::Get_Instance()->Set_Item(strItem);	
	
	strItem = TEXT("Ã¼·Â Æ÷¼Ç");
	CUI_Manager::Get_Instance()->Set_Item(strItem, 10);
	
	strItem = TEXT("[½ÅÈ­]Å» °Í ¼ÒÈ¯ Ä«µå");
	CUI_Manager::Get_Instance()->Set_Item(strItem, 10);
	
	strItem = TEXT("[Èñ±Í]Å» °Í ¼ÒÈ¯ Ä«µå");
	CUI_Manager::Get_Instance()->Set_Item(strItem, 10);
	
	strItem = TEXT("[ÀÏ¹Ý]Å» °Í ¼ÒÈ¯ Ä«µå");
	CUI_Manager::Get_Instance()->Set_Item(strItem, 10);
	
	strItem = TEXT("°í¾çÀÌ");
	CUI_Manager::Get_Instance()->Set_Item(strItem);

	strItem = TEXT("µå·¡°ï");
	CUI_Manager::Get_Instance()->Set_Item(strItem);

	//m_pWearableSlots[W_EQUIP]->Set_WearableItem(eItem);
	return S_OK;
}

void CInven::Tick(_float fTimeDelta)
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
	if (TRUE == PtInRect(&rectUI, ptMouse))
	{
		if (!m_isActive && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
			{
				return;
			}

			if (CEvent_Manager::Get_Instance()->Get_TutorialLevel() == T_OPENINVEN)
			{
				CEvent_Manager::Get_Instance()->Set_TutorialComplete(T_OPENINVEN);
				CEvent_Manager::Get_Instance()->Set_TutorialSeq(T_EQUIP);
			}

			LIGHT_DESC* LightDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, TEXT("Light_Main"));

			m_Light_Desc = *LightDesc;
			LightDesc->eType = LIGHT_DESC::Directional;
			LightDesc->vDirection = _float4(0.f, 0.f, -1.f, 0.f);
			LightDesc->vDiffuse = _vec4(0.8f, 0.8f, 0.8f, 1.f);
			LightDesc->vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
			LightDesc->vSpecular = _vec4(1.f);

			for (size_t i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
			{
				if (m_pGameInstance->Get_IsLoopingSound(i))
				{
					m_pGameInstance->FadeoutSound(i, fTimeDelta, 1.f, true, 0.3f);
				}
			}

			CFadeBox::FADE_DESC Desc = {};
			Desc.fOut_Duration = 0.8f;
			CUI_Manager::Get_Instance()->Add_FadeBox(Desc);

			CCamera_Manager::Get_Instance()->Set_CameraState(CS_INVEN);
			CUI_Manager::Get_Instance()->Set_InvenActive(true);
			m_bNewItemIn = false;
			m_isActive = true;
			Init_InvenState();
		}
	}

	if (!m_isActive)
	{
		return;
	}
	if (TRUE == PtInRect(&dynamic_cast<CTextButton*>(m_pExitButton)->Get_Rect(), ptMouse)
		|| PtInRect(&dynamic_cast<CTextButton*>(m_pTitleButton)->Get_Rect(), ptMouse))
	{
		if (m_isActive && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			if (m_bNewItemIn)
			{
				m_bNewItemIn = false;
			}

			if (CEvent_Manager::Get_Instance()->Get_TutorialLevel() == T_EXIT)
			{
				CEvent_Manager::Get_Instance()->Set_TutorialComplete(T_EXIT);
				//CEvent_Manager::Get_Instance()->Set_TutorialSeq(T_EXIT);
			}

			if (m_Light_Desc.eType != LIGHT_DESC::TYPE::End)
			{
				LIGHT_DESC* LightDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, TEXT("Light_Main"));
				*LightDesc = m_Light_Desc;
			}
			
			for (size_t i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
			{
				if (m_pGameInstance->Get_IsLoopingSound(i))
				{
					m_pGameInstance->FadeinSound(i, fTimeDelta);
				}
			}

			CFadeBox::FADE_DESC Desc = {};
			Desc.fOut_Duration = 0.8f;
			CUI_Manager::Get_Instance()->Add_FadeBox(Desc);

			CCamera_Manager::Get_Instance()->Set_CameraState(CS_ENDFULLSCREEN);
			CUI_Manager::Get_Instance()->Set_FullScreenUI(false);
			CUI_Manager::Get_Instance()->Set_InvenActive(false);
			m_isActive = false;
		}
	}



	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (m_isActive)
	{
		for (size_t i = 0; i < WEARABLE_TYPE::W_END; i++)
		{
			if (PtInRect(&m_pWearableSlots[i]->Get_Rect(), ptMouse) && m_pGameInstance->Mouse_Down(DIM_RBUTTON))
			{
				if (m_pWearableSlots[i]->Is_Full())
				{
					ITEM Item = m_pWearableSlots[i]->Get_ItemDesc();
					dynamic_cast<CInvenFrame*>(m_pInvenFrame)->Set_Item(Item);
					m_pWearableSlots[i]->Delete_Item();
				}
				break;
			}
		}	

		if (m_isReset)
		{
			_bool isReset = false;
			for (size_t i = 0; i < WEARABLE_TYPE::W_END; i++)
			{
				if (m_pWearableSlots[i]->Is_Full())
				{
					ITEM Item = m_pWearableSlots[i]->Get_ItemDesc();
					dynamic_cast<CInvenFrame*>(m_pInvenFrame)->Set_Item(Item);
					m_pWearableSlots[i]->Delete_Item();
					isReset = true;
					break;
				}
			}
			if (!isReset)
			{
				m_isReset = false;
			}
		}
		if (m_pInvenFrame == nullptr)
		{
			int a = 10;
		}

		CUI_Manager::Get_Instance()->Set_FullScreenUI(true);
		m_pExitButton->Tick(fTimeDelta);
		m_pInvenFrame->Tick(fTimeDelta);
		m_pBackGround->Tick(fTimeDelta);
		m_pTitleButton->Tick(fTimeDelta);
		for (size_t i = 0; i < WEARABLE_TYPE::W_END; i++)
		{
			m_pWearableSlots[i]->Tick(fTimeDelta);
		}

	}

}

void CInven::Late_Tick(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_pMoney->Late_Tick(fTimeDelta);
		m_pDiamond->Late_Tick(fTimeDelta);
		m_pSeigeLine->Late_Tick(fTimeDelta);
		m_pInvenFrame->Late_Tick(fTimeDelta);
		m_pExitButton->Late_Tick(fTimeDelta);
		m_pBackGround->Late_Tick(fTimeDelta);
		m_pTitleButton->Late_Tick(fTimeDelta);
		for (size_t i = 0; i < WEARABLE_TYPE::W_END; i++)
		{
			m_pWearableSlots[i]->Late_Tick(fTimeDelta);
		}
	}


	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}
	if (m_bNewItemIn)
	{
		m_pNotify->Late_Tick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CInven::Render()
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


	return S_OK;
}


void CInven::Init_InvenState()
{
	dynamic_cast<CInvenFrame*>(m_pInvenFrame)->Set_FrameMode(CInvenFrame::F_INVEN);
	dynamic_cast<CInvenFrame*>(m_pInvenFrame)->Init_State();
	dynamic_cast<CInvenFrame*>(m_pInvenFrame)->Set_Parent(this);

	_uint iMoney = CUI_Manager::Get_Instance()->Get_Coin();;
	dynamic_cast<CTextButton*>(m_pMoney)->Set_Text(to_wstring(iMoney));

	_uint iDiamond = CUI_Manager::Get_Instance()->Get_Diamond();;
	dynamic_cast<CTextButton*>(m_pDiamond)->Set_Text(to_wstring(iDiamond));

}

HRESULT CInven::Set_Item(ITEM eItem, _uint iNum)
{
	m_bNewItemIn = true;
	dynamic_cast<CInvenFrame*>(m_pInvenFrame)->Set_Item(eItem, iNum);
	return S_OK;
}

HRESULT CInven::Set_WearableItem(WEARABLE_TYPE eType, ITEM eItemDesc)
{
	if (m_pWearableSlots[eType] == nullptr)
	{
		return E_FAIL;
	}
	if (m_pWearableSlots[eType]->Is_Full())
	{
		ITEM Item = m_pWearableSlots[eType]->Get_ItemDesc();
		if (eItemDesc.strName == Item.strName)
		{
			return E_FAIL;
		}
		dynamic_cast<CInvenFrame*>(m_pInvenFrame)->Set_Item(Item);
	}
	m_pWearableSlots[eType]->Set_WearableItem(eItemDesc);
	m_pGameInstance->Play_Sound(TEXT("WeaponEquip"));
	return S_OK;
}

void CInven::Reset_WearableSlot()
{
	m_isReset = true;
}

HRESULT CInven::Add_Parts()
{
	CTextButton::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.03f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Notify");
	Button.vPosition = _vec2(m_fX + 17.f, m_fY - 14.f);
	Button.vSize = _vec2(40.f, 40.f);

	m_pNotify = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pNotify)
	{
		return E_FAIL;
	}

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.fFontSize = 0.5f;
	Button.strText = TEXT("°¡¹æ");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Back");
	Button.vPosition = _vec2(20.f, 20.f);
	Button.vSize = _vec2(50.f, 50.f);
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(40.f, 0.f);

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
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_SiegeLine");
	Button.vPosition = _vec2(102.f, 350.f);
	Button.vSize = _vec2(6.f, 360.f);

	m_pSeigeLine = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pSeigeLine)
	{
		return E_FAIL;
	}

	
	_uint iMoney = CUI_Manager::Get_Instance()->Get_Coin();;
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

	UiInfo info{};
	info.strTexture = TEXT("Prototype_Component_Texture_BackGround_Mask");
	info.vPos = _vec2((_float)g_iWinSizeX/2.f, (_float)g_iWinSizeY / 2.f);
	info.vSize = _vec2((_float)g_iWinSizeX, (_float)g_iWinSizeY);
	info.iLevel = (_uint)LEVEL_CUSTOM;

	m_pBackGround = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BackGround_Mask"), &info);
	if (not m_pBackGround)
	{
		return E_FAIL;
	}

	for (size_t i = 0; i < WEARABLE_TYPE::W_END; i++)
	{
		CWearable_Slot::WEARABLESLOT SlotDesc = {};
		SlotDesc.eType = (WEARABLE_TYPE)i;
		SlotDesc.fDepth = m_fDepth - 0.01f;
		SlotDesc.vSize = _float2(50.f, 50.f);
		SlotDesc.vPosition = _float2(60.f, 180.f + (SlotDesc.vSize.x * (_uint)i) + (4.f * (_uint)i));
		m_pWearableSlots[i] = (CWearable_Slot*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Wearable_Slot"), &SlotDesc);
		if (not m_pWearableSlots[i])
		{
			return E_FAIL;
		}
	}

	CInvenFrame::INVENFRAME_DESC InvenDesc = {};
	//InvenDesc.pParent = this;
	InvenDesc.fDepth = m_fDepth - 0.01f;
	InvenDesc.vPosition = _float2(1070.f, 400.f);
	InvenDesc.vSize = _float2(360.f, 580.f);
	m_pInvenFrame = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_InvenFrame"), &InvenDesc);
	if (not m_pInvenFrame)
	{
		return E_FAIL;
	}
	CUI_Manager::Get_Instance()->Set_InvenFrame(m_pInvenFrame);
	return S_OK;
}

HRESULT CInven::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_InvenBar"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CInven::Bind_ShaderResources()
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

CInven* CInven::Create(_dev pDevice, _context pContext)
{
	CInven* pInstance = new CInven(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CInven");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInven::Clone(void* pArg)
{
	CInven* pInstance = new CInven(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CInven");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInven::Free()
{
	__super::Free();

	if (!m_isPrototype)
	{
		for (size_t i = 0; i < WEARABLE_TYPE::W_END; i++)
		{
			Safe_Release(m_pWearableSlots[i]);
		}
	}


	Safe_Release(m_pMoney);
	Safe_Release(m_pDiamond);
	Safe_Release(m_pNotify);
	Safe_Release(m_pSeigeLine);
	Safe_Release(m_pBackGround);
	Safe_Release(m_pExitButton);
	Safe_Release(m_pTitleButton);
	Safe_Release(m_pInvenFrame);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}