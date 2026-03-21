#include "VehicleBook.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"
#include "FadeBox.h"
#include "Event_Manager.h"
#include "Vehicle.h"
#include "Camera_Manager.h"

CVehicleBook::CVehicleBook(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CVehicleBook::CVehicleBook(const CVehicleBook& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CVehicleBook::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CVehicleBook::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 70.f;
	m_fSizeY = 70.f;
	m_fX = 1040.f;
	m_fY = 47.f;
	m_fDepth = (_float)D_INVEN / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	for (size_t i = 0; i < Type_End; i++)
	{
		if (FAILED(Set_Vehicle((Riding_Type)i)))
		{
			return E_FAIL;
		}
	}



	return S_OK;
}

void CVehicleBook::Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Get_TimeStop())
	{
		fTimeDelta /= m_pGameInstance->Get_TimeRatio();
	}

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	
	Riding_Type eType = CUI_Manager::Get_Instance()->Get_NewRiding();
	if (eType != Type_End)
	{
		if (FAILED(Set_Vehicle(eType)))
		{
			return;
		}
		CUI_Manager::Get_Instance()->Set_NewRiding(Type_End);
		return;
	}

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
			LIGHT_DESC* LightDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, TEXT("Light_Main"));

			m_Light_Desc = *LightDesc;
			LightDesc->eType = LIGHT_DESC::Directional;
			LightDesc->vDirection = _float4(0.f, 0.f, 1.f, 0.f);
			LightDesc->vDiffuse = _vec4(0.8f, 0.8f, 0.8f, 1.f);
			LightDesc->vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
			LightDesc->vSpecular = _vec4(1.f);

			CFadeBox::FADE_DESC Desc = {};
			Desc.fOut_Duration = 0.8f;
			CUI_Manager::Get_Instance()->Add_FadeBox(Desc);

			CCamera_Manager::Get_Instance()->Set_CameraState(CS_SKILLBOOK);
			m_bNewVehicleIn = false;
			m_isActive = true;
			Init_VehicleBookState();

			for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
			{
				if (m_pGameInstance->Get_IsLoopingSound(i))
				{
					m_pGameInstance->FadeoutSound(i, fTimeDelta);
				}
			}
			m_iSoundChannel = m_pGameInstance->Play_Sound(TEXT("BGM_Night_Field_01"), 0.3f, true);
			m_pGameInstance->FadeinSound(m_iSoundChannel, fTimeDelta);
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
			for (size_t j = 0; j < m_vecVehicle[m_eCurType].size(); j++)
			{
				m_vecVehicle[m_eCurType][j]->Set_SelectVehicle(false);
			}
			if (m_Light_Desc.eType != LIGHT_DESC::TYPE::End)
			{
				LIGHT_DESC* LightDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, TEXT("Light_Main"));
				*LightDesc = m_Light_Desc;
			}
			CFadeBox::FADE_DESC Desc = {};
			Desc.fOut_Duration = 0.8f;
			CUI_Manager::Get_Instance()->Add_FadeBox(Desc);

			CCamera_Manager::Get_Instance()->Set_CameraState(CS_ENDFULLSCREEN);
			CUI_Manager::Get_Instance()->Set_FullScreenUI(false);
			m_isActive = false;

			for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
			{
				if (m_pGameInstance->Get_IsLoopingSound(i))
				{
					m_pGameInstance->FadeinSound(i, fTimeDelta, 1.f);
				}
			}
			m_pGameInstance->FadeoutSound(m_iSoundChannel, fTimeDelta, 1.f, false);
			return;
		}
	}
	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
	{
		/* 인벤토리 메뉴 피킹 */
		for (size_t i = 0; i < VC_END; i++)
		{
			if (PtInRect(&dynamic_cast<CTextButtonColor*>(m_pVehicleType[i])->Get_Rect(), ptMouse))
			{
				for (size_t j = 0; j < m_vecVehicle[m_eCurType].size(); j++)
				{
					m_vecVehicle[m_eCurType][j]->Set_SelectVehicle(false);
				}
				m_ePrevType = m_eCurType;
				m_eCurType = (VEHICLE_TYPE)i;

				if (m_ePrevType != m_eCurType)
				{
					dynamic_cast<CTextButtonColor*>(m_pVehicleType[m_ePrevType])->Set_Alpha(0.6f);
				}
				dynamic_cast<CTextButtonColor*>(m_pVehicleType[i])->Set_Alpha(1.f);
				_vec2 vPos = dynamic_cast<CTextButtonColor*>(m_pVehicleType[i])->Get_Position();
				dynamic_cast<CTextButton*>(m_pSelectButton)->Set_Position(vPos);
				_vec2 fUnderBarPos = dynamic_cast<CTextButton*>(m_pUnderBar)->Get_Position();
				dynamic_cast<CTextButton*>(m_pUnderBar)->Set_Position(_vec2(vPos.x, fUnderBarPos.y));


				break;
			}
		}


		size_t i = 0;
		for (i = 0; i < m_vecVehicle[m_eCurType].size(); i++)
		{
			if (PtInRect(&m_vecVehicle[m_eCurType][i]->Get_Rect(), ptMouse))
			{
				if (m_vecVehicle[m_eCurType][i]->Is_Selected())
				{
					m_vecVehicle[m_eCurType][i]->Set_SelectVehicle(false);
					m_isPicking = false;
					break;
				}
				m_vecVehicle[m_eCurType][i]->Set_SelectVehicle(true);
				m_isPicking = true;
				m_iCurIndex = i;
				for (size_t j = 0; j < m_vecVehicle[m_eCurType].size(); j++)
				{
					if (m_iCurIndex != j)
					{
						m_vecVehicle[m_eCurType][j]->Set_SelectVehicle(false);
					}
				}
				break;
			}
		}
	}



	if (PtInRect(&m_pEquipButton->Get_InitialRect(), ptMouse))
	{
		m_pEquipButton->Set_Size(140.f, 80.f, 0.3f);
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI) && m_isPicking)
		{
			m_isPicking = false;
			m_vecVehicle[m_eCurType][m_iCurIndex]->Set_Equip(true);
			m_pSelectedVehicle[m_eCurType] = m_vecVehicle[m_eCurType][m_iCurIndex];
			CUI_Manager::Get_Instance()->Set_Riding(m_eCurType, m_vecVehicle[m_eCurType][m_iCurIndex]->Get_VehicleInfo().eRidingType);
			for (size_t i = 0; i < m_vecVehicle[m_eCurType].size(); i++)
			{
				if (m_iCurIndex != i)
				{
					m_vecVehicle[m_eCurType][i]->Set_Equip(false);
				}
			}
		}
	}
	else
	{
		m_pEquipButton->Set_Size(150.f, 100.f, 0.35f);
	}


	_uint iMoney = CUI_Manager::Get_Instance()->Get_Coin();;
	dynamic_cast<CTextButton*>(m_pMoney)->Set_Text(to_wstring(iMoney));

	_uint iDiamond = CUI_Manager::Get_Instance()->Get_Diamond();;
	dynamic_cast<CTextButton*>(m_pDiamond)->Set_Text(to_wstring(iDiamond));


	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	CUI_Manager::Get_Instance()->Set_FullScreenUI(true);

	for (size_t i = 0; i < VC_END; i++)
	{
		m_pVehicleType[i]->Tick(fTimeDelta);
	}
	for (size_t i = 0; i < m_vecVehicle[m_eCurType].size(); i++)
	{
		m_vecVehicle[m_eCurType][i]->Tick(fTimeDelta);
	}

	m_pUnderBar->Tick(fTimeDelta);
	m_pSelectButton->Tick(fTimeDelta);
	m_pExitButton->Tick(fTimeDelta);
	m_pBackGround->Tick(fTimeDelta);
	m_pTitleButton->Tick(fTimeDelta);
	m_pSlotBackGround->Tick(fTimeDelta);
	m_pEquipButton->Tick(fTimeDelta);
}

void CVehicleBook::Late_Tick(_float fTimeDelta)
{


	if (m_isActive)
	{
		for (size_t i = 0; i < VC_END; i++)
		{
			m_pVehicleType[i]->Late_Tick(fTimeDelta);
		}
		for (size_t i = 0; i < m_vecVehicle[m_eCurType].size(); i++)
		{
			m_vecVehicle[m_eCurType][i]->Late_Tick(fTimeDelta);
		}
		m_pUnderBar->Late_Tick(fTimeDelta);
		m_pSelectButton->Late_Tick(fTimeDelta);

		m_pMoney->Late_Tick(fTimeDelta);
		m_pDiamond->Late_Tick(fTimeDelta);
		m_pExitButton->Late_Tick(fTimeDelta);
		m_pBackGround->Late_Tick(fTimeDelta);
		m_pTitleButton->Late_Tick(fTimeDelta);
		m_pSlotBackGround->Late_Tick(fTimeDelta);
		m_pEquipButton->Late_Tick(fTimeDelta);

	}


	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}
	if (m_bNewVehicleIn)
	{
		m_pNotify->Late_Tick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CVehicleBook::Render()
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


void CVehicleBook::Init_VehicleBookState()
{
	m_eCurType = VC_GROUND;
	m_ePrevType = VC_GROUND;

	_uint iMoney = CUI_Manager::Get_Instance()->Get_Coin();;
	dynamic_cast<CTextButton*>(m_pMoney)->Set_Text(to_wstring(iMoney));

	_uint iDiamond = CUI_Manager::Get_Instance()->Get_Diamond();;
	dynamic_cast<CTextButton*>(m_pDiamond)->Set_Text(to_wstring(iDiamond));

	for (size_t j = 0; j < m_vecVehicle[m_eCurType].size(); j++)
	{
		//m_vecVehicle[m_eCurType][j]->Select_Skill(false);
	}

	dynamic_cast<CTextButtonColor*>(m_pVehicleType[m_eCurType])->Set_Alpha(1.f);
	_vec2 vPos = dynamic_cast<CTextButtonColor*>(m_pVehicleType[m_eCurType])->Get_Position();
	dynamic_cast<CTextButton*>(m_pSelectButton)->Set_Position(vPos);
	_vec2 fUnderBarPos = dynamic_cast<CTextButton*>(m_pUnderBar)->Get_Position();
	dynamic_cast<CTextButton*>(m_pUnderBar)->Set_Position(_vec2(vPos.x, fUnderBarPos.y));

}

HRESULT CVehicleBook::Set_Vehicle(Riding_Type eType)
{
	_bool isExist = false;
	for (size_t i = 0; i < VC_END; i++)
	{
		for (size_t j = 0; j < m_vecVehicle[i].size(); j++)
		{
			if (m_vecVehicle[i][j]->Get_VehicleInfo().eRidingType == eType)
			{
				isExist = true;
				break;
			}
		}
	}
	if (!isExist)
	{
		CVehicle::VEHICLE_DESC Desc{};
		Desc.eRidingType = eType;
		Desc.fDepth = m_fDepth - 0.01f;
		Desc.vSize = _vec2(300.f, 90.f);
		CVehicle* pVehicle = (CVehicle*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Vehicle"), &Desc);
		if (pVehicle == nullptr)
		{
			return E_FAIL;
		}
		VEHICLE_TYPE eVehicleType = pVehicle->Get_VehicleInfo().eType;
		_float fStartY = 166.f;
		_float fIdx = 0.f;
		if (m_vecVehicle[eVehicleType].empty())
		{
			fIdx = 0;
		}
		else
		{
			fIdx = (float)m_vecVehicle[eVehicleType].size();
		}

		pVehicle->Set_Position(_vec2(1105.f, fStartY + 85.f * fIdx));
		m_vecVehicle[eVehicleType].push_back(pVehicle);
	}

	
	return S_OK;
}

HRESULT CVehicleBook::Add_Parts()
{
	CTextButton::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.03f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Notify");
	Button.vPosition = _vec2(m_fX + 7.f, m_fY - 14.f);
	Button.vSize = _vec2(40.f, 40.f);

	m_pNotify = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pNotify)
	{
		return E_FAIL;
	}

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.fFontSize = 0.5f;
	Button.strText = TEXT("탈 것");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Back");
	Button.vPosition = _vec2(40.f, 20.f);
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

	_float fY = 85.f;
	_float fTerm = 300.f / (_uint)WP_END;
	_float fStartX = 880.f + fTerm;


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

	UiInfo info{};
	info.strTexture = TEXT("Prototype_Component_Texture_Skill_Background");
	info.vPos = _vec2((_float)g_iWinSizeX/2.f, (_float)g_iWinSizeY / 2.f);
	info.vSize = _vec2((_float)g_iWinSizeX, (_float)g_iWinSizeY);
	info.iLevel = (_uint)LEVEL_CUSTOM;

	m_pBackGround = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BackGround_Mask"), &info);
	if (not m_pBackGround)
	{
		return E_FAIL;
	}



	CTextButtonColor::TEXTBUTTON_DESC TextButton = {};
	TextButton.eLevelID = LEVEL_STATIC;
	TextButton.strTexture = TEXT("");
	TextButton.fDepth = m_fDepth - 0.01f;
	TextButton.fAlpha = 1.f;
	TextButton.fFontSize = 0.35f;
	TextButton.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.strText = TEXT("지상");
	TextButton.vPosition = _vec2(fStartX, fY);
	TextButton.vSize = _vec2(m_fSizeX / 2.f, 70.f);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_NoTex");
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pVehicleType[VC_GROUND] = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pVehicleType[VC_GROUND])
	{
		return E_FAIL;
	}
	dynamic_cast<CTextButtonColor*>(m_pVehicleType[VC_GROUND])->Set_Pass(VTPass_UI_Alpha);

	TextButton.strText = TEXT("공중");
	TextButton.vPosition = _vec2(fStartX + fTerm, fY);
	TextButton.vSize = _vec2(m_fSizeX / 2.f, 70.f);
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pVehicleType[VC_FLY] = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pVehicleType[VC_FLY])
	{
		return E_FAIL;
	}
	dynamic_cast<CTextButtonColor*>(m_pVehicleType[VC_FLY])->Set_Pass(VTPass_UI_Alpha);

	TextButton.fDepth = m_fDepth + 0.01f;
	TextButton.strText = TEXT("");
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Select_BG_BoxEfc_WhiteBlur");
	TextButton.vSize = _vec2(300.f, 580.f);
	TextButton.vColor = _vec4(0.125f, 0.153f, 0.169f, 0.6f);
	TextButton.fAlpha = 0.6f;
	TextButton.vPosition = _vec2(1107.5f, (_float)g_iWinSizeY / 2.f + 50.f);
	m_pSlotBackGround = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
	if (not m_pSlotBackGround)
	{
		return E_FAIL;
	}

	TextButton.fDepth = m_fDepth - 0.02f;
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Button_Blue");
	TextButton.strText = TEXT("탈 것 장착");
	TextButton.vSize = _vec2(150.f, 100.f);
	TextButton.vPosition = _vec2(1160.f, 665.f);
	m_pEquipButton = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
	if (not m_pEquipButton)
	{
		return E_FAIL;
	}
	m_pEquipButton->Set_Pass(VTPass_UI_Alpha);

	return S_OK;
}

HRESULT CVehicleBook::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_VehicleBar"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CVehicleBook::Bind_ShaderResources()
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

CVehicleBook* CVehicleBook::Create(_dev pDevice, _context pContext)
{
	CVehicleBook* pInstance = new CVehicleBook(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVehicleBook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVehicleBook::Clone(void* pArg)
{
	CVehicleBook* pInstance = new CVehicleBook(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVehicleBook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVehicleBook::Free()
{
	__super::Free();


	if (!m_isPrototype)
	{
		for (size_t i = 0; i < VC_END; i++)
		{
			Safe_Release(m_pVehicleType[i]);
		}
	}
	
	for (auto& iter : m_vecVehicle[VC_GROUND])
	{
		Safe_Release(iter);
	}
	m_vecVehicle[VC_GROUND].clear();

	for (auto& iter : m_vecVehicle[VC_FLY])
	{
		Safe_Release(iter);
	}
	m_vecVehicle[VC_FLY].clear();

	Safe_Release(m_pUnderBar);
	Safe_Release(m_pEquipButton);
	Safe_Release(m_pSelectButton);
	Safe_Release(m_pSlotBackGround);

	Safe_Release(m_pMoney);
	Safe_Release(m_pDiamond);
	Safe_Release(m_pNotify);
	Safe_Release(m_pBackGround);
	Safe_Release(m_pExitButton);
	Safe_Release(m_pTitleButton);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}