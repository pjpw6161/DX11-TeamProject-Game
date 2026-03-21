#include "Select.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Manager.h"

CSelect::CSelect(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSelect::CSelect(const CSelect& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSelect::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CSelect::Init(void* pArg)
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



	return S_OK;
}

void CSelect::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pSelectDesc == nullptr)
	{
		for (_uint i = 0; i < 4; ++i)
		{
			RECT rcRect = {
			  (LONG)((160.f + 320.f * i) - 320.f * 0.5f),
			  (LONG)(360.f - 360.f * 0.5f),
			  (LONG)((160.f + 320.f * i) + 320.f * 0.5f),
			  (LONG)(360.f + 360.f * 0.5f)
			};
			if (PtInRect(&rcRect, ptMouse))
			{
				m_bShow = true;
				m_eCurModel = (SELECT_MODEL)i;
				m_pCharacterSelect->Set_Active_Alpha(CCharacterSelect::ALPHA);
				Set_SelectDesc(i);
				Set_CameraState(i);
				break;
			}
		}
	}

	if (m_bShow && PtInRect(&m_pBackButton->Get_InitialRect(), ptMouse))
	{
		m_pBackButton->Set_Size(140.f, 80.f, 0.3f);
	}
	else
	{
		m_pBackButton->Set_Size(150.f, 100.f, 0.35f);
	}

	if (m_bShow && PtInRect(&m_pSelectButton->Get_InitialRect(), ptMouse))
	{
		m_pSelectButton->Set_Size(140.f, 80.f, 0.3f);
	}
	else
	{
		m_pSelectButton->Set_Size(150.f, 100.f, 0.35f);
	}

	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI) && m_bShow)
	{
		if (PtInRect(&m_pSelectButton->Get_Rect(), ptMouse))
		{
			CCamera_Manager::Get_Instance()->Set_CameraState(CS_DEFAULT);
			m_pGameInstance->Level_ShutDown(LEVEL_SELECT);
		}
		if (PtInRect(&m_pBackButton->Get_Rect(), ptMouse))
		{
			if (m_pSelectDesc)
			{
				m_pSelectModels[m_eCurModel]->Change_AnimState(CSelect_Model::S_CANCEL);
				CCamera_Manager::Get_Instance()->Set_CameraState(CS_DEFAULT);
				m_pBackButton->Set_Size(150.f, 30.f, 0.35f);
				Safe_Release(m_pSelectDesc);
				m_bShow = false;
				m_pCharacterSelect->Set_Active_Alpha(CCharacterSelect::NONALPHA);
			}
		}

	}


	m_pCharacterSelect->Tick(fTimeDelta);

	if (m_pSelectDesc != nullptr && m_bShow)
	{
		m_pSelectDesc->Tick(fTimeDelta);
		if (m_pSelectButton != nullptr)
		{
			m_pSelectButton->Tick(fTimeDelta);
		}
		if (m_pBackButton != nullptr)
		{
			m_pBackButton->Tick(fTimeDelta);
		}
	}

	for (_int i = 0; i < 4; i++)
	{
		m_pSelectModels[i]->Tick(fTimeDelta);
	}
}

void CSelect::Late_Tick(_float fTimeDelta)
{
	m_pTitleButton->Late_Tick(fTimeDelta);
	m_pCharacterSelect->Late_Tick(fTimeDelta);
	m_pClassButton->Late_Tick(fTimeDelta);
	if (m_pSelectDesc != nullptr && m_bShow)
	{
		m_pSelectDesc->Late_Tick(fTimeDelta);
		if (m_pSelectButton != nullptr)
		{
			m_pSelectButton->Late_Tick(fTimeDelta);
		}
		if (m_pBackButton != nullptr)
		{
			m_pBackButton->Late_Tick(fTimeDelta);
		}
	}
	for (_int i = 0; i < 4; i++)
	{
		m_pSelectModels[i]->Late_Tick(fTimeDelta);
	}
}

HRESULT CSelect::Render()
{

	return S_OK;
}

HRESULT CSelect::Add_Components()
{
	
	return S_OK;
}

HRESULT CSelect::Add_Parts()
{
	m_pCharacterSelect = (CCharacterSelect*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CharacterSelect"));
	if (not m_pCharacterSelect)
	{
		return E_FAIL;
	}

	CTextButton::TEXTBUTTON_DESC ButtonDesc = {};
	ButtonDesc.eLevelID = LEVEL_SELECT;
	ButtonDesc.fDepth = 0.5f;
	ButtonDesc.fFontSize = 0.4f;
	ButtonDesc.strText = TEXT("클래스 선택");
	ButtonDesc.strTexture = TEXT("");
	ButtonDesc.vPosition = _vec2(65.f, 20.f);
	ButtonDesc.vSize = _vec2(20.f, 20.f);
	ButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ButtonDesc.vTextPosition = _vec2(20.f, 0.f);
	
	m_pClassButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pClassButton)
	{
		return E_FAIL;
	}
	ButtonDesc.eLevelID = LEVEL_STATIC;
	ButtonDesc.strText = TEXT("");
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Back");
	ButtonDesc.vPosition = _vec2(20.f, 20.f);
	ButtonDesc.vSize = _vec2(50.f, 50.f);	
	m_pTitleButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pTitleButton)
	{
		return E_FAIL;
	}


	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = 0.5f;
	ColButtonDesc.fAlpha = 0.8f;
	ColButtonDesc.fFontSize = 0.35f;
	ColButtonDesc.vColor = _vec4(0.2f, 0.2f, 0.2f, 0.5f);
	ColButtonDesc.strText = TEXT("선택");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Button_Blue");
	ColButtonDesc.vPosition = _vec2(1125.f, 670.f);
	ColButtonDesc.vSize = _vec2(150.f,100.f);
	ColButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ColButtonDesc.vTextPosition = _vec2(0.f, -2.f);


	m_pSelectButton = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pSelectButton)
	{
		return E_FAIL;  
	}
	m_pSelectButton->Set_Pass(VTPass_UI_Alpha);

	ColButtonDesc.strText = TEXT("뒤로 가기");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Button_Blue");
	ColButtonDesc.vPosition = _vec2(155.f, 670.f);

	
	m_pBackButton = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pBackButton)
	{
		return E_FAIL;
	}
	m_pBackButton->Set_Pass(VTPass_UI_Alpha);


	return S_OK;
}

HRESULT CSelect::Add_Models()
{
	ObjectInfo Info{};
	Info.strPrototypeTag = TEXT("Prototype_Model_Select0");

	_vec4 vPos;
	vPos = _vec4(1.8f, 0.07f, -2.4f, 1.f);//프리스트
	Info.vPos = vPos;
	m_pSelectModels[3] = (CSelect_Model*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Select_Model"), &Info);
	if (not m_pSelectModels[3])
	{
		return E_FAIL;
	}


	vPos = _vec4(0.7f, 0.07f, -1.2f, 1.f);//로그
	Info.vPos = vPos;
	Info.strPrototypeTag = TEXT("Prototype_Model_Select1");
	m_pSelectModels[2] = (CSelect_Model*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Select_Model"), &Info);
	if (not m_pSelectModels[2])
	{
		return E_FAIL;
	}

	vPos = _vec4(-2.2f, 0.07f, -2.7f, 1.f);//소서리스
	Info.vPos = vPos;
	Info.strPrototypeTag = TEXT("Prototype_Model_Select2");
	m_pSelectModels[0] = (CSelect_Model*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Select_Model"), &Info);
	if (not m_pSelectModels[0])
	{
		return E_FAIL;
	}

	vPos = _vec4(-0.9f, 0.07f, -1.2f, 1.f);//워리어
	Info.vPos = vPos;
	Info.strPrototypeTag = TEXT("Prototype_Model_Select3");
	m_pSelectModels[1] = (CSelect_Model*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Select_Model"), &Info);
	if (not m_pSelectModels[1])
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSelect::Bind_ShaderResources()
{

	return S_OK;
}

void CSelect::Set_SelectDesc(_uint iSelect)
{
	if (m_pSelectDesc)
	{
		Safe_Release(m_pSelectDesc);
	}

	CSelectDesc::SELECT_DESC SelectDesc = {};
	SelectDesc.eCharacter = (CSelectDesc::CHARACTER)iSelect;

	m_pSelectDesc = (CSelectDesc*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SelectDesc"), &SelectDesc);
}

void CSelect::Set_CameraState(_uint iSelect)
{
	m_pSelectModels[iSelect]->Change_AnimState(CSelect_Model::S_MOTION);
	CCamera_Manager::Get_Instance()->Set_CameraState(CS_ZOOM);
	_vec4 vTargetPos;

	CCamera_Manager::Get_Instance()->Set_Have_TargetLook(false);

	switch (iSelect)
	{
	case 0:
		CCamera_Manager::Get_Instance()->Set_ZoomFactor(3.5f);
		vTargetPos = _vec4(-0.904127300f, 1.48444211f, -3.65620041f, 1.f);
		CCamera_Manager::Get_Instance()->Set_CameraTargetLook(_vec4(-0.658045292f, -0.0125071695f, 0.752852142f, 0.f));
		CCamera_Manager::Get_Instance()->Set_Have_TargetLook(true);
		break;
	case 1:
		CCamera_Manager::Get_Instance()->Set_ZoomFactor(3.5f);
		vTargetPos = _vec4(-0.15f, 0.f, 0.2f, 1.f);
		break;
	case 2:
		CCamera_Manager::Get_Instance()->Set_ZoomFactor(3.2f);
		vTargetPos = _vec4(0.1f, 0.f, 0.2f, 1.f);
		break;
	case 3:
		vTargetPos = _vec4(0.323398888f, 1.49999952f, -3.6651926f, 1.f);
		CCamera_Manager::Get_Instance()->Set_CameraTargetLook(_vec4(0.287743211f, -0.0157100987f, 0.957573771f, 0.0f));
		CCamera_Manager::Get_Instance()->Set_Have_TargetLook(true);
		break;
	}
	CCamera_Manager::Get_Instance()->Set_CameraTargetPos(vTargetPos);
}

CSelect* CSelect::Create(_dev pDevice, _context pContext)
{
	CSelect* pInstance = new CSelect(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSelect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSelect::Clone(void* pArg)
{
	CSelect* pInstance = new CSelect(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSelect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSelect::Free()
{
	__super::Free();

	if (!m_isPrototype)
	{
		for (_int i = 0; i < 4; i++)
		{
			Safe_Release(m_pSelectModels[i]);
		}
	}

	Safe_Release(m_pSelectDesc);
	Safe_Release(m_pTitleButton);
	Safe_Release(m_pBackButton);
	Safe_Release(m_pClassButton);
	Safe_Release(m_pSelectButton);
	Safe_Release(m_pCharacterSelect);
}
