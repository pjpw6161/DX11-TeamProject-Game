#include "SummonWindow.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "UI_Manager.h"
#include "Event_Manager.h"
#include "BlurTexture.h"
#include "FadeBox.h"
#include "Skill.h"

CSummonWindow::CSummonWindow(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CSummonWindow::CSummonWindow(const CSummonWindow& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CSummonWindow::Init_Prototype()
{
	return S_OK;
}

HRESULT CSummonWindow::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 1300.f;
	m_fSizeY = 1300.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = (_float)g_iWinSizeY / 2.f - 150.f;

	m_fDepth = (_float)D_WINDOW / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);


	m_iItemTier = ((SUMMON_DESC*)pArg)->iItemTier;

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	m_fBright = 12.f;
	m_fSmokeSize = m_pSmoke->Get_Size().x;
	return S_OK;
}

void CSummonWindow::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);



	if (m_bSetResult)
	{
		m_pButton->Set_Text(TEXT("확인"));
	}
	if (PtInRect(&m_pButton->Get_InitialRect(), ptMouse))
	{
		m_pButton->Set_Size(140.f, 80.f, 0.3f);
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			if (m_bSetResult)
			{
				m_isDead = true;
				return;
			}
			m_bSetResult = true;
			return;
		}
	}
	else
	{
		m_pButton->Set_Size(150.f, 100.f, 0.35f);
	}

	if (m_pGameInstance->Key_Down(DIK_RETURN))
	{
		m_isDead = true;
		return;
	}

	if (m_fSizeX <= 500.f)
	{
		m_fSizeX = 500.f;
		m_bStartEffect = true;
	}
	if(!m_bStartEffect)
	{
		m_fSizeX -= 50.f; 
		m_fSizeY -= 50.f; 
	}
	if (m_bStartEffect)
	{
		if (m_fBright >= 4.f)
		{
			m_fBright -= fTimeDelta * 30.f;
		}

		m_fStop += fTimeDelta;
		if (m_fStop >= 4.f)
		{
			m_fStop = 0.f;
		}
		else if (m_fStop >= 2.f)
		{
			m_fSmokeSize += fTimeDelta * 50.f;
		}
		else
		{
			m_fSmokeSize -= fTimeDelta * 50.f;
		}
		m_pSmoke->Set_Size(m_fSmokeSize, m_fSmokeSize);

	}

	if (m_pCard != nullptr)
	{
		if (m_fBrightCard > 2.f)
		{
			m_fBrightCard -= fTimeDelta * 30.f;
			m_pCard->Set_Factor(m_fBrightCard);
		}
		if (m_pCard->Get_Size().x >= 80.f)
		{
			m_pCard->Set_Size(m_pCard->Get_Size().x - 10.f, m_pCard->Get_Size().y - 20.f);
		}

		if (PtInRect(&m_pCard->Get_Rect(), ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI) && !m_bStartShake)
		{
			if (m_pCard->Get_Size().x < 100.f)
			{
				m_bStartShake = true;
			}

		}
		if (m_bStartShake)
		{
			m_fShakeTime += fTimeDelta;

			_float fShakeAmount = sin(m_fShakeAcc * 20.f) * powf(0.9f, m_fShakeAcc) * 2.f;
			_vec2 vShakePos = m_pCard->Get_TransPosition();
			vShakePos += _vec2(fShakeAmount, -fShakeAmount);
			m_pCard->Set_Position(vShakePos);
			m_fShakeAcc += fTimeDelta * 3.f;

			if (m_fShakeTime >= 1.5f)
			{
				m_bSetResult = true;
				m_bStartShake = false;
				Safe_Release(m_pCard);
			}

		}
	}
	if (m_bSetResult)
	{
		if (m_pResultEffect != nullptr && m_pResultEffect->Get_Size().x >= 1.f)
		{
			m_pResultEffect->Set_Size(m_pResultEffect->Get_Size().x - fTimeDelta * 900.f, m_pResultEffect->Get_Size().x - fTimeDelta * 900.f);
		}
		if (m_pResultEffect != nullptr && m_pResultEffect->Get_Size().x <= 1.f)
		{
			Safe_Release(m_pResultEffect);
		}
		if (m_pResultEffect != nullptr)
		{
			m_pResultEffect->Tick(fTimeDelta);
		}
	}

	m_fTime += fTimeDelta * 0.2f;
	m_fMoveY += fTimeDelta * 0.2f;

	m_pBackGround->Set_UV(0.f, m_fMoveY);


	if (m_pCard != nullptr)
	{
		m_pCard->Tick(fTimeDelta);
	}

	m_pSmoke->Tick(fTimeDelta);
	m_pPattern->Tick(fTimeDelta);
	m_pButton->Tick(fTimeDelta);

	m_pBackGround->Tick(fTimeDelta);

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CSummonWindow::Late_Tick(_float fTimeDelta)
{
	if (m_pCard != nullptr)
	{
		m_pCard->Late_Tick(fTimeDelta);
	}
	if (m_bSetResult)
	{
		if (m_pResultEffect != nullptr)
		{
			m_pResultEffect->Late_Tick(fTimeDelta);
		}
		m_pResultCard->Late_Tick(fTimeDelta);
	}

	m_pSmoke->Late_Tick(fTimeDelta);
	m_pPattern->Late_Tick(fTimeDelta);
	m_pButton->Late_Tick(fTimeDelta);
	m_pBackGround->Late_Tick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CSummonWindow::Render()
{

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Bright)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}


	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("소환 결과"), _vec2(m_fX + 1.f, m_fY + 90.f), 0.45f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("소환 결과"), _vec2(m_fX, m_fY + 90.f + 1.f), 0.45f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("소환 결과"), _vec2(m_fX, m_fY + 90.f), 0.45f, _vec4(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CSummonWindow::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Gacha0"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Noise_04"), TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSummonWindow::Add_Parts()
{
	CTextButton::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.strText = TEXT("바로보기");
	Button.fFontSize = 0.4f;
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(0.f, -2.f);
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Button_Blue");
	Button.vPosition = _vec2(m_fX, (_float)g_iWinSizeY - 80.f);
	Button.vSize = _vec2(150.f, 100.f);
	m_pButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pButton)
	{
		return E_FAIL;
	}

	CTextButtonColor::TEXTBUTTON_DESC TextButton = {};
	TextButton.eLevelID = LEVEL_STATIC;
	TextButton.fDepth = m_fDepth + 0.01f;
	TextButton.fAlpha = 0.8f;
	TextButton.fFontSize = 0.35f;
	TextButton.strText = TEXT("");
	TextButton.vPosition = _vec2(m_fX, m_fY);
	TextButton.vSize = _vec2(500.f, 500.f);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Gacha1");
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pPattern = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pPattern)
	{
		return E_FAIL;
	}
	m_pPattern->Set_Pass(VTPass_UI_Alpha);



	TextButton.fDepth = m_fDepth - 0.01f;
	TextButton.fAlpha = 0.2f;
	TextButton.fFontSize = 0.35f;
	TextButton.vColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.vPosition = _vec2(m_fX, m_fY);
	TextButton.vSize = _vec2(600.f, 600.f);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_Effect_FX_B_Glow003_Tex");
	//TextButton.strTexture2 = TEXT("Prototype_Component_Texture_Effect_FX_J_Dust001_Tex");
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pSmoke = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pSmoke)
	{
		return E_FAIL;
	}
	m_pSmoke->Set_Pass(VTPass_Mask_ColorAlpha);
	

	TextButton.fDepth = m_fDepth - 0.03f;
	TextButton.fAlpha = 0.2f;
	TextButton.fFontSize = 0.35f;
	TextButton.vColor = _vec4(0.9f, 0.9f, 1.f, 1.f);
	TextButton.vPosition = _vec2(m_fX, (_float)g_iWinSizeY - 200.f);
	TextButton.vSize = _vec2(500.f, 500.f);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_Effect_FX_B_VerBeam001_Tex");
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pResultEffect = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pResultEffect)
	{
		return E_FAIL;
	}
	m_pResultEffect->Set_Pass(VTPass_Mask_Color);


	TextButton.fDepth = m_fDepth + 0.02f;
	TextButton.fAlpha = 0.f;
	TextButton.fFontSize = 0.35f;
	TextButton.strText = TEXT("");
	TextButton.vColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.vPosition = _vec2((_float)g_iWinSizeX / 2.f, (_float)g_iWinSizeY / 2.f);
	TextButton.vSize = _vec2((_float)g_iWinSizeX, (_float)g_iWinSizeY);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_background3");
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pBackGround = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
	if (not m_pBackGround)
	{
		return E_FAIL;
	}
	m_pBackGround->Set_Pass(VTPass_Move);
	

	TextButton.fDepth = m_fDepth - 0.02f;
	TextButton.fAlpha = 0.f;
	TextButton.vColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.vPosition = _vec2(m_fX, (_float)g_iWinSizeY - 200.f);
	TextButton.vSize = _vec2(200.f, 400.f);
	if (m_iItemTier == TIER_COMMON)
	{
		TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BG_VehicleCard");
	}
	else if (m_iItemTier == TIER_RARE)
	{
		TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BG_VehicleCard02");
	}
	else if (m_iItemTier == TIER_LEGENDARY)
	{
		TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BG_VehicleCard2");
	}
	TextButton.strTexture2 = TEXT("Prototype_Component_Texture_Effect_FX_B_Gradient010_Tex");
	m_pCard = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
	if (not m_pCard)
	{
		return E_FAIL;
	}
	m_pCard->Set_Pass(VTPass_Bright);
	
	TextButton.fDepth = m_fDepth - 0.02f;
	TextButton.fAlpha = 0.f;
	TextButton.vColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.vPosition = _vec2(m_fX, (_float)g_iWinSizeY - 200.f);
	TextButton.vSize = _vec2(75.f, 150.f);
	_uint iRandom = rand() % 10;
	if (m_iItemTier == TIER_COMMON)
	{
		if (iRandom <= 5)
		{
			TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_c_bird");
			m_eRidingType = Bird;
		}
		else
		{
			TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_c_horse");
			m_eRidingType = Horse;
		}
	}
	else if (m_iItemTier == TIER_RARE)
	{
		if (iRandom <= 5)
		{
			TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_c_tiger");
			m_eRidingType = Tiger;
		}
		else
		{
			TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_c_wyvern");
			m_eRidingType = Wyvern;
		}
	}
	else if (m_iItemTier == TIER_LEGENDARY)
	{
		if (iRandom <= 5)
		{
			TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_c_falar");
			m_eRidingType = Falar;
		}
		else
		{
			TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_c_nihilir");
			m_eRidingType = Nihilir;
		}
	}
	
	m_pResultCard = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
	if (not m_pResultCard)
	{
		return E_FAIL;
	}
	m_pResultCard->Set_Pass(VTPass_UI);


	return S_OK;
}

HRESULT CSummonWindow::Bind_ShaderResources()
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

	if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fx", &m_fTime, sizeof(_float))))
	{
		return E_FAIL;
	}
	_float fTemp = 0.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &fTemp, sizeof(_float))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightFactor", &m_fBright, sizeof(_float))))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

CSummonWindow* CSummonWindow::Create(_dev pDevice, _context pContext)
{
	CSummonWindow* pInstance = new CSummonWindow(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSummonWindow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSummonWindow::Clone(void* pArg)
{
	CSummonWindow* pInstance = new CSummonWindow(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSummonWindow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSummonWindow::Free()
{
	__super::Free();


	Safe_Release(m_pCard);
	Safe_Release(m_pSmoke);
	Safe_Release(m_pButton);
	Safe_Release(m_pPattern);
	Safe_Release(m_pBackGround);
	Safe_Release(m_pResultCard);
	Safe_Release(m_pResultEffect);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
