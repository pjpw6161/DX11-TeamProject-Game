#include "SkillBook.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"
#include "FadeBox.h"
#include "InvenFrame.h"
#include "Event_Manager.h"
#include "SkillDesc.h"
#include "SkillSlot.h"
#include "SkillBlock.h"
#include "Skill.h"
#include "Camera_Manager.h"

CSkillBook::CSkillBook(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CSkillBook::CSkillBook(const CSkillBook& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CSkillBook::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CSkillBook::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 70.f;
	m_fSizeY = 70.f;
	m_fX = 1100.f;
	m_fY = 47.f;
	m_fDepth = (_float)D_INVEN / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	
	if (FAILED(Init_SkillDesc()))
	{
		return E_FAIL;
	}

	CUI_Manager::Get_Instance()->Set_SkillBook(this);

	Unlock_Skill(0);
	Unlock_Skill(1);
	Unlock_Skill(2);
	Unlock_Skill(3);
	return S_OK;
}

void CSkillBook::Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Get_TimeStop())
	{
		fTimeDelta /= m_pGameInstance->Get_TimeRatio();
	}

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	
	RECT rectUI = {
			  (LONG)(m_fX - m_fSizeX * 0.5f),
			  (LONG)(m_fY - m_fSizeY * 0.5f),
			  (LONG)(m_fX + m_fSizeX * 0.5f),
			  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

	if (m_isActive)
	{
		if (m_isPicking && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::Engine) && !PtInRect(&m_vecSkillDesc[m_eCurType][m_iCurIndex]->Get_Rect(), ptMouse))
		{
			m_isPicking = false;
			_bool isExist = false;
			for (size_t i = 0; i < 4; i++)
			{
				if (PtInRect(&m_pSkillSlot[m_eCurType][i]->Get_Rect(), ptMouse))
				{
					if (!m_vecSkillDesc[m_eCurType][m_iCurIndex]->Is_UnLocked())
					{
						return;
					}
					SKILLINFO tInfo = m_vecSkillDesc[m_eCurType][m_iCurIndex]->Get_SkillInfo();
					for (size_t j = 0; j < 4; j++)
					{
						if (m_pSkillSlot[m_eCurType][j]->Is_Full())
						{
							if (m_pSkillSlot[m_eCurType][j]->Get_SkillInfo().strName == tInfo.strName)
							{
								isExist = true;
								break;
							}
						}
					}
					if (!isExist)
					{
						if (CEvent_Manager::Get_Instance()->Get_TutorialLevel() == T_EQUIPSKILL)
						{
							CEvent_Manager::Get_Instance()->Set_TutorialComplete(T_EQUIPSKILL);
							CEvent_Manager::Get_Instance()->Set_TutorialSeq(T_SKILLEXIT);
						}
						m_pSkillSlot[m_eCurType][i]->Set_Skill(tInfo);
						m_pGameInstance->Play_Sound(TEXT("UI_GambleSelect_SFX_01"));
						break;
					}
				}
			}
		}

	}


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
			LightDesc->vDirection = _float4(0.f, 0.f, -1.f, 0.f);
			LightDesc->vDiffuse = _vec4(0.8f, 0.8f, 0.8f, 1.f);
			LightDesc->vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
			LightDesc->vSpecular = _vec4(1.f);

			CFadeBox::FADE_DESC Desc = {};
			Desc.fOut_Duration = 0.8f;
			CUI_Manager::Get_Instance()->Add_FadeBox(Desc);

			CCamera_Manager::Get_Instance()->Set_CameraState(CS_SKILLBOOK);
			m_bNewSkillIn = false;
			m_isActive = true;
			Init_SkillBookState();

			if (CEvent_Manager::Get_Instance()->Get_TutorialLevel() == T_OPENSKILL)
			{
				CEvent_Manager::Get_Instance()->Set_TutorialComplete(T_OPENSKILL);
				CEvent_Manager::Get_Instance()->Set_TutorialSeq(T_EQUIPSKILL);
			}
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
			if (m_Light_Desc.eType != LIGHT_DESC::TYPE::End)
			{
				LIGHT_DESC* LightDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, TEXT("Light_Main"));
				*LightDesc = m_Light_Desc;
			}
			CFadeBox::FADE_DESC Desc = {};
			Desc.fOut_Duration = 0.8f;
			CUI_Manager::Get_Instance()->Add_FadeBox(Desc);

			CCamera_Manager::Get_Instance()->Set_CameraState(CS_ENDFULLSCREEN);
			CUI_Manager::Get_Instance()->Set_SkillSlotChange(true);
			CUI_Manager::Get_Instance()->Set_FullScreenUI(false);
			m_isActive = false;
			if (CEvent_Manager::Get_Instance()->Get_TutorialLevel() == T_SKILLEXIT)
			{
				CEvent_Manager::Get_Instance()->Set_TutorialComplete(T_SKILLEXIT);
			}
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
		for (size_t i = 0; i < WP_END; i++)
		{
			if (PtInRect(&dynamic_cast<CTextButtonColor*>(m_pSkillType[i])->Get_Rect(), ptMouse))
			{
				m_ePrevType = m_eCurType;
				m_eCurType = (WEAPON_TYPE)i;

				if (m_ePrevType != m_eCurType)
				{
					dynamic_cast<CTextButtonColor*>(m_pSkillType[m_ePrevType])->Set_Alpha(0.6f);
				}
				dynamic_cast<CTextButtonColor*>(m_pSkillType[i])->Set_Alpha(1.f);
				_vec2 vPos = dynamic_cast<CTextButtonColor*>(m_pSkillType[i])->Get_Position();
				dynamic_cast<CTextButton*>(m_pSelectButton)->Set_Position(vPos);
				_vec2 fUnderBarPos = dynamic_cast<CTextButton*>(m_pUnderBar)->Get_Position();
				dynamic_cast<CTextButton*>(m_pUnderBar)->Set_Position(_vec2(vPos.x, fUnderBarPos.y));

				for (size_t j = 0; j < m_vecSkillDesc[m_eCurType].size(); j++)
				{
					m_vecSkillDesc[m_eCurType][j]->Select_Skill(false);
				}
				break;
			}
		}

		_bool bSelect = false;
		size_t i = 0;
		for (i = 0; i < m_vecSkillDesc[m_eCurType].size(); i++)
		{
			if (PtInRect(&m_vecSkillDesc[m_eCurType][i]->Get_Rect(), ptMouse))
			{
				m_pSkill_Model->Change_AnimState((CSkill_Model::SKILLMODEL_ANIM)m_vecSkillDesc[m_eCurType][i]->Get_SkillInfo().iModelSkillIndex);
				if (m_vecSkillDesc[m_eCurType][i]->Is_Selected())
				{
					m_vecSkillDesc[m_eCurType][i]->Select_Skill(false);
					break;
				}
				m_vecSkillDesc[m_eCurType][i]->Select_Skill(true);
				if (!m_vecSkillDesc[m_eCurType][i]->Is_UnLocked())
				{
					m_isPicking = false;
				}
				else if (m_vecSkillDesc[m_eCurType][i]->Is_SkillIn())
				{
					m_isPicking = true;
					m_iCurIndex = i;
				}

				bSelect = true;
				break;
			}
		}
		if (bSelect)
		{
			for (size_t j = 0; j < m_vecSkillDesc[m_eCurType].size(); j++)
			{
				if (i != j)
				{
					m_vecSkillDesc[m_eCurType][j]->Select_Skill(false);
				}
			}
		}

	}


	if (m_pGameInstance->Mouse_Down(DIM_RBUTTON, InputChannel::UI))
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (PtInRect(&m_pSkillSlot[m_eCurType][i]->Get_Rect(), ptMouse))
			{
				m_pSkillSlot[m_eCurType][i]->Delete_Skill();
				break;
			}
		}
	}

	if (PtInRect(&m_pResetSlot->Get_Rect(), ptMouse))
	{
		m_pResetSlot->Set_Size(140.f, 80.f, 0.3f);
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::Engine))
		{
			for (size_t i = 0; i < 4; i++)
			{
				m_pSkillSlot[m_eCurType][i]->Delete_Skill();
			}
		}
	}
	else
	{
		m_pResetSlot->Set_Size(150.f, 100.f, 0.35f);
	}

	if (m_isPicking)
	{
		if (m_fTime > 0.6f )
		{
			m_fTime = 0.6f;
			m_fDir = -1.f;
		}
		
		if (m_fTime < 0.2f )
		{
			m_fDir = 1.f;
		}


		m_fTime += fTimeDelta * m_fDir * 0.8f;

		for (size_t i = 0; i < 4; i++)
		{
			m_pSelectSlot[i]->Set_Alpha(m_fTime);
			m_pSelectSlot[i]->Tick(fTimeDelta);
		}
	}

	_uint iMoney = CUI_Manager::Get_Instance()->Get_Coin();;
	dynamic_cast<CTextButton*>(m_pMoney)->Set_Text(to_wstring(iMoney));

	_uint iDiamond = CUI_Manager::Get_Instance()->Get_Diamond();;
	dynamic_cast<CTextButton*>(m_pDiamond)->Set_Text(to_wstring(iDiamond));


	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	CUI_Manager::Get_Instance()->Set_FullScreenUI(true);

	for (size_t i = 0; i < WP_END; i++)
	{
		m_pSkillType[i]->Tick(fTimeDelta);
	}
	for (size_t i = 0; i < m_vecSkillDesc[m_eCurType].size(); i++)
	{
		m_vecSkillDesc[m_eCurType][i]->Tick(fTimeDelta);
	}
	for (size_t i = 0; i < 4; i++)
	{
		m_pSkillSlot[m_eCurType][i]->Tick(fTimeDelta);
	}
	m_pUnderBar->Tick(fTimeDelta);
	m_pSelectButton->Tick(fTimeDelta);
	m_pExitButton->Tick(fTimeDelta);
	m_pBackGround->Tick(fTimeDelta);
	m_pTitleButton->Tick(fTimeDelta);
	m_pSlotBackGround->Tick(fTimeDelta);
	m_pResetSlot->Tick(fTimeDelta);
	m_pSkill_Model->Tick(fTimeDelta);
	m_pScarecorw->Tick(fTimeDelta);
}

void CSkillBook::Late_Tick(_float fTimeDelta)
{


	if (m_isActive)
	{
		for (size_t i = 0; i < WP_END; i++)
		{
			m_pSkillType[i]->Late_Tick(fTimeDelta);
		}
		for (size_t i = 0; i < m_vecSkillDesc[m_eCurType].size(); i++)
		{
			m_vecSkillDesc[m_eCurType][i]->Late_Tick(fTimeDelta);
		}
		for (size_t i = 0; i < 4; i++)
		{
			m_pSkillSlot[m_eCurType][i]->Late_Tick(fTimeDelta);
		}
		if (m_isPicking)
		{
			for (size_t i = 0; i < 4; i++)
			{
				m_pSelectSlot[i]->Late_Tick(fTimeDelta);
			}
		}
		m_pUnderBar->Late_Tick(fTimeDelta);
		m_pSelectButton->Late_Tick(fTimeDelta);

		m_pMoney->Late_Tick(fTimeDelta);
		m_pDiamond->Late_Tick(fTimeDelta);
		m_pExitButton->Late_Tick(fTimeDelta);
		m_pBackGround->Late_Tick(fTimeDelta);
		m_pTitleButton->Late_Tick(fTimeDelta);
		m_pSlotBackGround->Late_Tick(fTimeDelta);
		m_pResetSlot->Late_Tick(fTimeDelta);
		m_pSkill_Model->Late_Tick(fTimeDelta);
		m_pScarecorw->Late_Tick(fTimeDelta);
	}


	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}
	if (m_bNewSkillIn)
	{
		m_pNotify->Late_Tick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CSkillBook::Render()
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

HRESULT CSkillBook::Unlock_Skill(_uint iIndex)
{
	for (size_t j = 0; j < WP_END; j++)
	{
		for (size_t i = 0; i < m_vecSkillDesc[j].size(); i++)
		{
			if (m_vecSkillDesc[j][i]->Get_SkillInfo().iSkillIdx == iIndex)
			{
				m_vecSkillDesc[j][i]->Unlock_Skill();
				break;
			}
		}
	}

	return S_OK;
}

void CSkillBook::Init_SkillBookState()
{
	_int iTemp = CUI_Manager::Get_Instance()->Get_WeaponType(PT_SWORD, &m_eCurType);
	m_ePrevType = m_eCurType;

	_uint iMoney = CUI_Manager::Get_Instance()->Get_Coin();;
	dynamic_cast<CTextButton*>(m_pMoney)->Set_Text(to_wstring(iMoney));

	_uint iDiamond = CUI_Manager::Get_Instance()->Get_Diamond();;
	dynamic_cast<CTextButton*>(m_pDiamond)->Set_Text(to_wstring(iDiamond));

	for (size_t j = 0; j < m_vecSkillDesc[m_eCurType].size(); j++)
	{
		m_vecSkillDesc[m_eCurType][j]->Select_Skill(false);
	}

	dynamic_cast<CTextButtonColor*>(m_pSkillType[m_eCurType])->Set_Alpha(1.f);
	_vec2 vPos = dynamic_cast<CTextButtonColor*>(m_pSkillType[m_eCurType])->Get_Position();
	dynamic_cast<CTextButton*>(m_pSelectButton)->Set_Position(vPos);
	_vec2 fUnderBarPos = dynamic_cast<CTextButton*>(m_pUnderBar)->Get_Position();
	dynamic_cast<CTextButton*>(m_pUnderBar)->Set_Position(_vec2(vPos.x, fUnderBarPos.y));

}

HRESULT CSkillBook::Init_SkillDesc()
{
	for (size_t i = 0; i < WP_END; i++)
	{
		_float fStartY = 166.f;
		_float fTerm = 2.f;
		_float fDescY = 90.f;
		for (_uint j = 0; j < 4; j++)
		{
			SKILLINFO tInfo = CUI_Manager::Get_Instance()->Get_SkillInfo((WEAPON_TYPE)i, j);

			CSkillDesc::SKILLBOOK_DESC SkillDesc = {};
			SkillDesc.fDepth = m_fDepth - 0.01f;
			SkillDesc.tSkillInfo = tInfo;
			SkillDesc.vPosition = _vec2(1105.f, fStartY + 85.f * j);

			CSkillDesc* pSkillDesc = (CSkillDesc*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SkillDesc"), &SkillDesc);

			if (not pSkillDesc)
			{
				return E_FAIL;
			}

			m_vecSkillDesc[i].push_back(pSkillDesc);
		}

	}
	return S_OK;
}

HRESULT CSkillBook::Add_Parts()
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
	Button.strText = TEXT("스킬북");
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
	TextButton.strText = TEXT("스나이퍼");
	TextButton.vPosition = _vec2(fStartX, fY);
	TextButton.vSize = _vec2(m_fSizeX / 2.f, 70.f);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_NoTex");
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pSkillType[WP_BOW] = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pSkillType[WP_BOW])
	{
		return E_FAIL;
	}
	dynamic_cast<CTextButtonColor*>(m_pSkillType[WP_BOW])->Set_Pass(VTPass_UI_Alpha);

	TextButton.strText = TEXT("어쌔신");
	TextButton.vPosition = _vec2(fStartX + fTerm, fY);
	TextButton.vSize = _vec2(m_fSizeX / 2.f, 70.f);
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pSkillType[WP_SWORD] = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pSkillType[WP_SWORD])
	{
		return E_FAIL;
	}
	dynamic_cast<CTextButtonColor*>(m_pSkillType[WP_SWORD])->Set_Pass(VTPass_UI_Alpha);



	TextButton.fDepth = m_fDepth - 0.01f;
	TextButton.strText = TEXT("");
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Select_BG_BoxEfc_WhiteBlur");
	TextButton.vSize = _vec2(300.f, 132.f);
	TextButton.vColor = _vec4(0.125f, 0.153f, 0.169f, 0.6f);
	TextButton.fAlpha = 0.6f;
	TextButton.vPosition = _vec2(1107.5f, 623.f);
	m_pSlotBackGround = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
	if (not m_pSlotBackGround)
	{
		return E_FAIL;
	}

	TextButton.fDepth = m_fDepth - 0.02f;
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_buttonRed");
	TextButton.strText = TEXT("초기화");
	TextButton.vPosition = _vec2(1160.f, 665.f);
	m_pResetSlot = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
	if (not m_pResetSlot)
	{
		return E_FAIL;
	}
	m_pResetSlot->Set_Pass(VTPass_UI_Alpha);


	TextButton.fDepth = m_fDepth - 0.05f;
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BloomRect");
	TextButton.strText = TEXT("");
	TextButton.vPosition = _vec2(1160.f, 665.f);
	TextButton.vSize = _vec2(80.f, 80.f);

	for (_uint i = 0; i < 4; i++)
	{
		TextButton.vPosition = _vec2(fStartX - 30.f + 70.f * i, 665.f - 62.5f);
		m_pSelectSlot[i] = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
		if (not m_pSelectSlot)
		{
			return E_FAIL;
		}
		m_pSelectSlot[i]->Set_Pass(VTPass_UI_Alpha);
	}

	
	for (size_t j = 0; j < WP_END; j++)
	{
		for (_uint i = 0; i < 4; i++)
		{
			CSkillSlot::SKILLSLOT_DESC SkillSlotDesc = {};
			SkillSlotDesc.eSlotMode = CSkillSlot::SKILLBOOK;
			SkillSlotDesc.vSize = _vec2(60.f, 60.f);
			SkillSlotDesc.fDepth = m_fDepth - 0.03f;
			SkillSlotDesc.vPosition = _vec2(fStartX - 30.f + 70.f * i, 665.f - 62.5f);
			m_pSkillSlot[j][i] = (CSkillSlot*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SkillSlot"), &SkillSlotDesc);
			if (m_pSkillSlot[j][i] == nullptr)
			{
				return E_FAIL;
			}
			if (FAILED(CUI_Manager::Get_Instance()->Set_SkillBookSlots((WEAPON_TYPE)j, (CSkillBlock::SKILLSLOT)i, m_pSkillSlot[j][i])))
			{
				return E_FAIL;
			}
		}
	}

	m_pSkill_Model = (CSkill_Model*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Skill_Model"));
	m_pScarecorw = (CScarecrow*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Scarecrow"));

	return S_OK;
}

HRESULT CSkillBook::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_SkillbookBar"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSkillBook::Bind_ShaderResources()
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

CSkillBook* CSkillBook::Create(_dev pDevice, _context pContext)
{
	CSkillBook* pInstance = new CSkillBook(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSkillBook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkillBook::Clone(void* pArg)
{
	CSkillBook* pInstance = new CSkillBook(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSkillBook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkillBook::Free()
{
	__super::Free();


	if (!m_isPrototype)
	{
		for (size_t i = 0; i < WP_END; i++)
		{
			Safe_Release(m_pSkillType[i]);
		}

		for (size_t i = 0; i < WP_END; i++)
		{
			for (_uint j = 0; j < 4; j++)
			{
				Safe_Release(m_pSkillSlot[i][j]);
			}
		}

		for (size_t i = 0; i < 4; i++)
		{
			Safe_Release(m_pSelectSlot[i]);
		}
	}
	
	for (auto& iter : m_vecSkillDesc[WP_BOW])
	{
		Safe_Release(iter);
	}
	m_vecSkillDesc[WP_BOW].clear();

	for (auto& iter : m_vecSkillDesc[WP_SWORD])
	{
		Safe_Release(iter);
	}
	m_vecSkillDesc[WP_SWORD].clear();


	Safe_Release(m_pScarecorw);
	Safe_Release(m_pSkill_Model);

	Safe_Release(m_pUnderBar);
	Safe_Release(m_pResetSlot);
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