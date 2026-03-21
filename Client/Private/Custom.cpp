#include "Custom.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"

CCustom::CCustom(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CCustom::CCustom(const CCustom& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCustom::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CCustom::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Models()))
	{
		return E_FAIL;
	}
	
	if (FAILED(Init_Menu()))
	{
		return E_FAIL;
	}


	m_vCurSelect[C_HAIR] = _vec2(-200, 0);
	m_vCurSelect[C_FACE] = _vec2(-200, 0);
	return S_OK;
}

void CCustom::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	CUI_Manager::Get_Instance()->Set_Picking_UI(false);

	if (Set_PickingRange(ptMouse))
	{
		CUI_Manager::Get_Instance()->Set_Picking_UI(true);
	}

	if (PtInRect(&m_pCustomMenu[C_FACE]->Get_Rect(), ptMouse))
	{
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON))
		{
			m_isMenuClick[C_FACE] = true;
			m_isMenuClick[C_HAIR] = false;
			m_pSelectCustomEffect->Set_Position(m_vCurSelect[C_FACE]);
			m_pSelectMenuEffect->Set_Position(_vec2(m_pCustomMenu[C_HAIR]->Get_Position().x, m_pCustomMenu[C_FACE]->Get_Position().y + 8.f));

		}
	}
	if (PtInRect(&m_pCustomMenu[C_HAIR]->Get_Rect(), ptMouse))
	{
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON))
		{
			m_isMenuClick[C_HAIR] = true;
			m_isMenuClick[C_FACE] = false;
			m_pSelectCustomEffect->Set_Position(m_vCurSelect[C_HAIR]);
			m_pSelectMenuEffect->Set_Position(_vec2(m_pCustomMenu[C_HAIR]->Get_Position().x, m_pCustomMenu[C_HAIR]->Get_Position().y + 8.f));
		}
	}

	if (m_isMenuClick[C_FACE])
	{
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::GamePlay))
		{
			for (_int i = 0; i < 6; i++)
			{
				RECT rcRect = {};

				rcRect = {
					  (LONG)(m_Face[i].first.x - 79.f * 0.5f),
					  (LONG)(m_Face[i].first.y - 87.f * 0.5f),
					  (LONG)(m_Face[i].first.x + 79.f * 0.5f),
					  (LONG)(m_Face[i].first.y + 87.f * 0.5f)
				};
				if (PtInRect(&rcRect, ptMouse))
				{
					CUI_Manager::Get_Instance()->Set_Picking_UI(true);
					m_vCurSelect[C_FACE] = m_Face[i].first;
					m_pSelectCustomEffect->Set_Position(m_Face[i].first);
					CUI_Manager::Get_Instance()->Set_CustomPart(PART_TYPE::PT_FACE, m_Face[i].second, 0);

					break;
				}

			}
		}
		
	}
	else if (m_isMenuClick[C_HAIR])
	{
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::GamePlay))
		{
			for (_int i = 0; i < 9; i++)
			{
				RECT rcRect = {};

				rcRect = {
					  (LONG)(m_Hair[i].first.x - 79.f * 0.5f),
					  (LONG)(m_Hair[i].first.y - 87.f * 0.5f),
					  (LONG)(m_Hair[i].first.x + 79.f * 0.5f),
					  (LONG)(m_Hair[i].first.y + 87.f * 0.5f)
				};
				if (PtInRect(&rcRect, ptMouse))
				{
					CUI_Manager::Get_Instance()->Set_Picking_UI(true);
					m_vCurSelect[C_HAIR] = m_Hair[i].first;
					m_pSelectCustomEffect->Set_Position(m_Hair[i].first);
					CUI_Manager::Get_Instance()->Set_CustomPart(PART_TYPE::PT_HAIR, m_Hair[i].second, 0);
					break;
				}

			}
		}
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::Engine))
		{
			for (_int i = 0; i < 10; i++)
			{
				if (PtInRect(&m_pColorButtons[i]->Get_Rect(), ptMouse))
				{
					CUI_Manager::Get_Instance()->Set_Picking_UI(true);
					m_pSelectColorEffect->Set_Position(m_pColorButtons[i]->Get_Position());
					if (i == 0)
					{
						CUI_Manager::Get_Instance()->Set_HairColor(_vec4(0.f, 0.f, 0.f, 0.f));
					}
					else
					{
						_vec4 vColor = m_pColorButtons[i]->Get_Color();
						CUI_Manager::Get_Instance()->Set_HairColor(m_pColorButtons[i]->Get_Color());
					}
					break;
				}

			}
		}
	}
	if (PtInRect(&m_pSelectButton->Get_InitialRect(), ptMouse))
	{
		if (m_pGameInstance->Mouse_Down(DIM_RBUTTON, InputChannel::Engine))
		{
			if (m_pGameInstance->Get_LayerSize(LEVEL_CUSTOM, TEXT("Layer_SetName")) == 0)
			{
				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_CUSTOM, TEXT("Layer_SetName"), TEXT("Prototype_GameObject_NameWindow"))))
				{
					return;
				}
			}
		}
		m_pSelectButton->Set_Size(140.f, 80.f, 0.3f);
	}
	else
	{
		m_pSelectButton->Set_Size(150.f, 100.f, 0.35f);
	}
	m_pSelectMenuEffect->Tick(fTimeDelta);
	m_pSelectCustomEffect->Tick(fTimeDelta);
	m_pSelectColorEffect->Tick(fTimeDelta);
	if (m_pSelectButton != nullptr)
	{
		m_pSelectButton->Tick(fTimeDelta);
	}
	m_pBackGround->Tick(fTimeDelta);
}


void CCustom::Late_Tick(_float fTimeDelta)
{
	m_pTitleButton->Late_Tick(fTimeDelta);
	m_pSelectMenuEffect->Late_Tick(fTimeDelta);
	if (m_isMenuClick[C_FACE])
	{
		m_pFaceGroup->Late_Tick(fTimeDelta);
	}
	else if (m_isMenuClick[C_HAIR])
	{
		m_pHairGroup->Late_Tick(fTimeDelta);
		for (_uint i = 0; i < 10; i++)
		{
			m_pColorButtons[i]->Late_Tick(fTimeDelta);
		}
		m_pSelectColorEffect->Late_Tick(fTimeDelta);
		m_pLine1->Late_Tick(fTimeDelta);
		m_pLine2->Late_Tick(fTimeDelta);
	}

	m_pSelectCustomEffect->Late_Tick(fTimeDelta);

	for (size_t i = 0; i < C_END; i++)
	{
		m_pCustomMenu[i]->Late_Tick(fTimeDelta);
	}

	m_pClassButton->Late_Tick(fTimeDelta);
	if (m_pSelectButton != nullptr)
	{
		m_pSelectButton->Late_Tick(fTimeDelta);
	}
	m_pBackGround->Late_Tick(fTimeDelta);
}

HRESULT CCustom::Render()
{

	return S_OK;
}

HRESULT CCustom::Add_Components()
{
	
	return S_OK;
}

HRESULT CCustom::Add_Parts()
{

	CTextButton::TEXTBUTTON_DESC Button = {};
	Button.eLevelID = LEVEL_CUSTOM;
	Button.fDepth = 0.5f;
	Button.fFontSize = 0.4f;
	Button.strText = TEXT("커스터마이징");
	Button.strTexture = TEXT("");
	Button.vPosition = _vec2(65.f, 20.f);
	Button.vSize = _vec2(20.f, 20.f);
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(20.f, 0.f);
	
	m_pClassButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pClassButton)
	{
		return E_FAIL;
	}

	Button.eLevelID = LEVEL_STATIC;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Back");
	Button.vPosition = _vec2(20.f, 20.f);
	Button.vSize = _vec2(50.f, 50.f);
	m_pTitleButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pTitleButton)
	{
		return E_FAIL;
	}

	CTextButton::TEXTBUTTON_DESC ButtonDesc = {};
	ButtonDesc.eLevelID = LEVEL_CUSTOM;
	ButtonDesc.fDepth = 0.5f;
	ButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ButtonDesc.strText = TEXT("얼굴");
	ButtonDesc.vTextPosition = _vec2(0.f, 40.f);
	ButtonDesc.fFontSize = 0.3f;
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Custom_Face");
	ButtonDesc.vPosition = _vec2(50.f, 120.f);
	ButtonDesc.vSize = _vec2(60.f, 60.f);

	m_pCustomMenu[C_FACE] = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pCustomMenu[C_FACE])
	{
		return E_FAIL;
	}	

	ButtonDesc.strText = TEXT("헤어");
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Custom_Hair");
	ButtonDesc.vPosition = _vec2(50.f, 220.f);

	m_pCustomMenu[C_HAIR] = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pCustomMenu[C_HAIR])
	{
		return E_FAIL;
	}

	//그룹사진
	ButtonDesc.strText = TEXT("");
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Custom_CustomFace2");
	ButtonDesc.vPosition = _vec2(1130.f, 180.f);
	ButtonDesc.vSize = _vec2(240.f, 266.f);
	m_pFaceGroup = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pFaceGroup)
	{
		return E_FAIL;
	}

	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Custom_CustomHair2");
	m_pHairGroup = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pHairGroup)
	{
		return E_FAIL;
	}
	
	//이펙트
	ButtonDesc.strText = TEXT("");
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Custom_SelectEffect");
	ButtonDesc.vPosition = _vec2(-200.f, 15.f);
	ButtonDesc.vSize = _vec2(79.f, 87.f); 
 	m_pSelectCustomEffect = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pSelectCustomEffect)
	{
		return E_FAIL;
	}
	
	ButtonDesc.vSize = _vec2(40.f, 40.f);
	m_pSelectColorEffect = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pSelectColorEffect)
	{
		return E_FAIL;
	}


	ButtonDesc.strText = TEXT("");
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Custom_FadeBox");
	ButtonDesc.vPosition = _vec2(-200.f, 15.f);
	ButtonDesc.vSize = _vec2(100.f, 80.f);
	m_pSelectMenuEffect = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pSelectMenuEffect)
	{
		return E_FAIL;
	}



	//캐릭터 생성
	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = 0.5f;
	ColButtonDesc.fAlpha = 0.8f;
	ColButtonDesc.fFontSize = 0.35f;
	ColButtonDesc.vColor = _vec4(0.2f, 0.2f, 0.2f, 0.5f);
	ColButtonDesc.strText = TEXT("캐릭터 생성");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Button_Blue");
	ColButtonDesc.vPosition = _vec2(1125.f, 670.f);
	ColButtonDesc.vSize = _vec2(150.f, 100.f);
	ColButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ColButtonDesc.vTextPosition = _vec2(0.f, -2.f);

	
	m_pSelectButton = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pSelectButton)
	{
		return E_FAIL;  
	}
	m_pSelectButton->Set_Pass(VTPass_UI_Alpha);

	//라인 생성
	ColButtonDesc.strText = TEXT("");
	ColButtonDesc.vColor = _vec4(1.f, 1.f, 1.f, 0.5f);
	ColButtonDesc.vPosition = _vec2(1130.f, 410.f);
	ColButtonDesc.vSize = _vec2(240.f, 1.f);

	m_pLine1 = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pLine1)
	{
		return E_FAIL;
	}
	ColButtonDesc.strText = TEXT("");
	ColButtonDesc.vColor = _vec4(1.f, 1.f, 1.f, 0.5f);
	ColButtonDesc.vPosition = _vec2(1130.f, 520.f);
	ColButtonDesc.vSize = _vec2(240.f, 1.f);

	m_pLine2 = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pLine2)
	{
		return E_FAIL;
	}

	//컬러 박스 생성
	ColButtonDesc.eLevelID = LEVEL_CUSTOM;
	ColButtonDesc.strText = TEXT("");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Custom_SelectColor");
	ColButtonDesc.vSize = _vec2(40.f, 40.f);

	ColButtonDesc.vColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ColButtonDesc.vPosition = _vec2(1010.f + ColButtonDesc.vSize.x / 2.f + ColButtonDesc.vSize.x * 0 + 10.f * 0, 420.f + ColButtonDesc.vSize.y / 2.f);

	m_pColorButtons[0] = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pColorButtons[0])
	{
		return E_FAIL;
	}

	ColButtonDesc.vColor = _vec4(0.f, 0.f, 0.f, 1.f);
	ColButtonDesc.vPosition = _vec2(1010.f + ColButtonDesc.vSize.x / 2.f + ColButtonDesc.vSize.x * 1 + 10.f * 1, 420.f + ColButtonDesc.vSize.y / 2.f);

	m_pColorButtons[1] = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	
	if (not m_pColorButtons[1])
	{
		return E_FAIL;
	}

	ColButtonDesc.vColor = _vec4(0.641f, 0.35f, 0.9f, 1.f);
	ColButtonDesc.vPosition = _vec2(1010.f + ColButtonDesc.vSize.x / 2.f + ColButtonDesc.vSize.x * 2 + 10.f * 2, 420.f + ColButtonDesc.vSize.y / 2.f);

	m_pColorButtons[2] = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pColorButtons[2])
	{
		return E_FAIL;
	}

	ColButtonDesc.vColor = _vec4(0.7f, 0.5f, 0.4f, 1.f);
	ColButtonDesc.vPosition = _vec2(1010.f + ColButtonDesc.vSize.x / 2.f + ColButtonDesc.vSize.x * 3 + 10.f * 3, 420.f + ColButtonDesc.vSize.y / 2.f);

	m_pColorButtons[3] = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pColorButtons[3])
	{
		return E_FAIL;
	}

	ColButtonDesc.vColor = _vec4(0.2f, 0.5f, 0.6f, 1.f);
	ColButtonDesc.vPosition = _vec2(1010.f + ColButtonDesc.vSize.x / 2.f + ColButtonDesc.vSize.x * 4 + 10.f * 4, 420.f + ColButtonDesc.vSize.y / 2.f);

	m_pColorButtons[4] = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pColorButtons[4])
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < 5; i++)
	{
		for (_uint j = 0; j < 2; j++)
		{
			if (i < 5 && j == 0)
				continue;

		
			_uint iRandomX = rand() % 10;
			_uint iRandomY = rand() % 10;
			_uint iRandomZ = rand() % 10;

			ColButtonDesc.vColor = _vec4(iRandomX * 0.1f, iRandomY * 0.1f, iRandomZ * 0.1f, 1.f);

			ColButtonDesc.vPosition = _vec2(1010.f + ColButtonDesc.vSize.x / 2.f + ColButtonDesc.vSize.x * i + 10.f * i, 420.f + ColButtonDesc.vSize.y / 2.f + ColButtonDesc.vSize.y * j + 8.f * j);

			m_pColorButtons[i + j * 5] = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
			if (not m_pColorButtons[i + j * 5])
			{
				return E_FAIL;
			}
		}

	}

	UiInfo info{};
	info.strTexture = TEXT("Prototype_Component_Texture_BackGround_Mask");
	info.vPos = _vec2(640, 360);
	info.vSize = _vec2(1280, 720);
	info.iLevel = (_uint)LEVEL_CUSTOM;

	m_pBackGround = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BackGround_Mask"), &info);
	if (not m_pBackGround)
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CCustom::Add_Models()
{

	return S_OK;
}

HRESULT CCustom::Init_Menu()
{
	for (size_t i = 0; i < C_END; i++)
		m_isMenuClick[i] = false;

	m_Hair[0] =  make_pair(_vec2(1050.f, 104.f), 1);
	m_Hair[1] =  make_pair(_vec2(1130.f, 104.f), 2);
	m_Hair[2] =  make_pair(_vec2(1210.f, 104.f), 3);
	m_Hair[3] =  make_pair(_vec2(1050.f, 188.f), 4);
	m_Hair[4] =  make_pair(_vec2(1130.f, 188.f), 0);
	m_Hair[5] =  make_pair(_vec2(1210.f, 188.f), 6);
	m_Hair[6] =  make_pair(_vec2(1050.f, 272.f), 8);
	m_Hair[7] =  make_pair(_vec2(1130.f, 272.f), 7);
	m_Hair[8] =  make_pair(_vec2(1210.f, 272.f), 5);

	m_Face[0] = make_pair(_vec2(1050.f, 104.f), 2);
	m_Face[1] = make_pair(_vec2(1130.f, 104.f), 5);
	m_Face[2] = make_pair(_vec2(1210.f, 104.f), 1);
	m_Face[3] = make_pair(_vec2(1050.f, 188.f), 0);
	m_Face[4] = make_pair(_vec2(1130.f, 188.f), 3);
	m_Face[5] = make_pair(_vec2(1210.f, 188.f), 4);
											
	return S_OK;
}

HRESULT CCustom::Bind_ShaderResources()
{

	return S_OK;
}


void CCustom::Set_CameraState(_uint iSelect)
{
	CCamera_Manager::Get_Instance()->Set_CameraState(CS_ZOOM);
	CCamera_Manager::Get_Instance()->Set_ZoomFactor(3.5f);
}

_bool CCustom::Set_PickingRange(POINT& ptMouse)
{
	RECT rcRect = {};

	_float2 vPosition = {80.f, 170.f};
	_float2 fSize = {160.f, 300.f};
	rcRect = {
		  (LONG)(vPosition.x - fSize.x * 0.5f),
		  (LONG)(vPosition.y - fSize.y * 0.5f),
		  (LONG)(vPosition.x + fSize.x * 0.5f),
		  (LONG)(vPosition.y + fSize.y * 0.5f)
	};
	if (PtInRect(&rcRect, ptMouse))
	{
		return true;
	}

	vPosition = {1130.f,  (_float)g_iWinSizeY/ 2.f };
	fSize = {400.f, (_float)g_iWinSizeY };
	rcRect = {
		  (LONG)(vPosition.x - fSize.x * 0.5f),
		  (LONG)(vPosition.y - fSize.y * 0.5f),
		  (LONG)(vPosition.x + fSize.x * 0.5f),
		  (LONG)(vPosition.y + fSize.y * 0.5f)
	};
	if (PtInRect(&rcRect, ptMouse))
	{
		return true;
	}

	return false;
}

CCustom* CCustom::Create(_dev pDevice, _context pContext)
{
	CCustom* pInstance = new CCustom(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCustom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCustom::Clone(void* pArg)
{
	CCustom* pInstance = new CCustom(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCustom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustom::Free()
{
	__super::Free();

	if (!m_isPrototype)
	{
		for (size_t i = 0; i < C_END; i++)
		{
			Safe_Release(m_pCustomMenu[i]);
		}
	}
	if (!m_isPrototype)
	{
		for (size_t i = 0; i < 10; i++)
		{
			Safe_Release(m_pColorButtons[i]);
		}
	}

	Safe_Release(m_pBackGround);

	Safe_Release(m_pClassButton);
	Safe_Release(m_pSelectButton);

	Safe_Release(m_pLine1);
	Safe_Release(m_pLine2);

	Safe_Release(m_pFaceGroup);
	Safe_Release(m_pHairGroup);
	Safe_Release(m_pTitleButton);
	Safe_Release(m_pSelectMenuEffect);
	Safe_Release(m_pSelectColorEffect);
	Safe_Release(m_pSelectCustomEffect);
}
