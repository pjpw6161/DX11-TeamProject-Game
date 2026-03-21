#include "InfinityTower.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "Tower.h"

CInfinityTower::CInfinityTower(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CInfinityTower::CInfinityTower(const CInfinityTower& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CInfinityTower::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CInfinityTower::Init(void* pArg)
{
	m_fDepth = (_float)D_WINDOW / (_float)D_END;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	
	if (FAILED(Ready_Tower()))
	{
		return E_FAIL;
	}

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;


	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	return S_OK;
}

void CInfinityTower::Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->IsInfinityTower())
	{
		CCamera_Manager::Get_Instance()->Set_CameraState(CS_DEFAULT);
		CUI_Manager::Get_Instance()->Open_InfinityTower(false);
		m_isActive = true;
	}
	if (!m_isActive)
	{
		return;
	}
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	/* 나가기 */
	if (PtInRect(&m_pExitButton->Get_Rect(), ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::GamePlay))
	{
		Exit_Tower();
		CUI_Manager::Get_Instance()->Set_WorldMap();
		return;
	}

	Effect_Tick(fTimeDelta);
	Tower_Tick(fTimeDelta, ptMouse);

	
	m_pTitle->Tick(fTimeDelta);
	m_pLeftBg->Tick(fTimeDelta);
	m_pStartButton->Tick(fTimeDelta);
}

void CInfinityTower::Late_Tick(_float fTimeDelta)
{
	if (!m_isActive)
	{
		return;
	}
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
	m_pLeftBg->Late_Tick(fTimeDelta);

	for (_uint i = 0; i < 3; i++)
	{
		if (m_bLightening[i] == true)
		{
			m_pLightening[i]->Late_Tick(fTimeDelta);

		}
	}
	for (size_t i = 0; i < TOWER_END; i++)
	{
		m_pTowers[i]->Late_Tick(fTimeDelta);
	}
	m_pTitle->Late_Tick(fTimeDelta);

	m_pRoundEffect->Late_Tick(fTimeDelta);
	m_pCloud->Late_Tick(fTimeDelta);
	m_pExitButton->Late_Tick(fTimeDelta);
	m_pStartButton->Late_Tick(fTimeDelta);

}

HRESULT CInfinityTower::Render()
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

HRESULT CInfinityTower::Ready_Tower()
{
	m_vDefaultPoint = _vec2(200.f, (_float)g_iWinSizeY - 100.f);
	m_vInitialPoint = m_vDefaultPoint;

	_float fTermX = 137.f;

	CTower::TOWER_DESC Desc{};

	Desc.vDefaultPoint = &m_vDefaultPoint;
	for (size_t i = 0; i < TOWER_END; i++)
	{
		if ((TOWER)i == BOSS1 || (TOWER)i == BOSS2)
		{
			Desc.isBossRoom = true;
		}
		else
		{
			Desc.isBossRoom = false;
		}
		Desc.vPosition = _vec2(0.f, -(fTermX * (_uint)i));

		CTower* pTower = (CTower*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Tower"), &Desc);

		if (not pTower)
		{
			return E_FAIL;
		}
		m_pTowers[(TOWER)i] = pTower;
	}


	return S_OK;
}

HRESULT CInfinityTower::Add_Parts()
{
	CTextButton::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.07f;
	Button.fFontSize = 0.5f;
	Button.strText = TEXT("유한의 탑");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Back");
	Button.vPosition = _vec2(20.f, 20.f);
	Button.vSize = _vec2(50.f, 50.f);
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(60.f, 0.f);

	m_pTitle = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pTitle)
	{
		return E_FAIL;
	}

	Button.fDepth = m_fDepth - 0.06f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Img_Cloud01");
	Button.vPosition = _vec2(180.f, 20.f);
	Button.vSize = _vec2(450.f, 300.f);
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(60.f, 0.f);

	m_pCloud = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pCloud)
	{
		return E_FAIL;
	}

	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Out");
	Button.vPosition = _vec2(1230.f, 30.f);
	Button.vSize = _vec2(70.f, 70.f);

	m_pExitButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pExitButton)
	{
		return E_FAIL;
	}

	Button.fDepth = m_fDepth - 0.01f;
	Button.strText = TEXT("입장하기");
	Button.fFontSize = 0.4f;
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(0.f, -2.f);
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_buttonRed");
	Button.vPosition = _vec2(1150.f, 660.f);
	Button.vSize = _vec2(150.f, 100.f);
	m_pStartButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pStartButton)
	{
		return E_FAIL;
	}

	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = m_fDepth - 0.01f;
	ColButtonDesc.fAlpha = 0.8f;
	ColButtonDesc.fFontSize = 0.f;
	ColButtonDesc.strText = TEXT("");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_FadeBox");
	ColButtonDesc.vSize = _vec2((_float)g_iWinSizeX / 2.f, (_float)g_iWinSizeY);
	ColButtonDesc.vPosition = _vec2(100.f, (_float)g_ptCenter.y);

	m_pLeftBg = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pLeftBg)
	{
		return E_FAIL;
	}
	m_pLeftBg->Set_Pass(VTPass_FadeHorizontal);
	
	ColButtonDesc.fDepth = m_fDepth - 0.01f;
	ColButtonDesc.fAlpha = 0.8f;
	ColButtonDesc.fFontSize = 0.f;
	ColButtonDesc.vColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ColButtonDesc.strText = TEXT("");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BG_LightningMask01");
	ColButtonDesc.vSize = _vec2(350.f, 350.f);
	ColButtonDesc.vPosition = _vec2((_float)g_ptCenter.x + 90.f, 150.f);

	m_pRoundEffect = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pRoundEffect)
	{
		return E_FAIL;
	}
	dynamic_cast<CTextButtonColor*>(m_pRoundEffect)->Set_Pass(VTPass_Mask_ColorAlpha);


	ColButtonDesc.vSize = _vec2(400.f, 400.f);
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BG_LightningEff01");
	ColButtonDesc.vPosition = _vec2((_float)g_ptCenter.x - 60.f, 380.f);

	CTextButtonColor* pLightening = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not pLightening)
	{
		return E_FAIL;
	}
	m_pLightening[0] = pLightening;
	m_pLightening[0]->Set_Pass(VTPass_Mask_ColorAlpha);
	ColButtonDesc.vSize = _vec2(200.f, 200.f);
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BG_LightningEff02");
	ColButtonDesc.vPosition = _vec2((_float)g_ptCenter.x - 55.f, 400.f);

	pLightening = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not pLightening)
	{
		return E_FAIL;
	}
	m_pLightening[1] = pLightening;
	m_pLightening[1]->Set_Pass(VTPass_Mask_ColorAlpha);

	ColButtonDesc.vSize = _vec2(400.f, 400.f);
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BG_LightningEff03");
	ColButtonDesc.vPosition = _vec2((_float)g_ptCenter.x + 390.f, 400.f);

	pLightening = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not pLightening)
	{
		return E_FAIL;
	}
	m_pLightening[2] = pLightening;
	m_pLightening[2]->Set_Pass(VTPass_Mask_ColorAlpha);

	
	return S_OK;
}

HRESULT CInfinityTower::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Bg_InfinityTower_01"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CInfinityTower::Bind_ShaderResources()
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

void CInfinityTower::Set_Active(_bool isActive)
{
	if (!m_isActive && isActive)
	{
		m_isActive = true;
	}
}

void CInfinityTower::Effect_Tick(_float fTimeDelta)
{
	m_fTime += fTimeDelta;

	if (!m_isPlaying && m_fTime >= 4.f)
	{
		m_isPlaying = true;
		for (_uint i = 0; i < 3; i++)
		{
			m_bLightening[i] = false;
			m_pLightening[i]->Set_Alpha(0.4f);
		}
		m_fTime = 0.f;
	}
	if (!m_isPlaying)
	{
		for (_uint i = 0; i < 3; i++)
		{
			if (m_bLightening[i] == true)
			{
				if (m_pLightening[i]->Get_Alpha() < 0.05f)
				{
					m_pLightening[i]->Set_Alpha(0.f);
					m_bLightening[i] = false;
					break;
				}
				m_pLightening[i]->Set_Alpha(m_pLightening[i]->Get_Alpha() - fTimeDelta);
			}

		}
	}

	if (m_isPlaying)
	{
		if (m_pRoundEffect->Get_Alpha() >= 0.8f)
		{
			m_fDir = -1.f;
		}
		if (m_pRoundEffect->Get_Alpha() <= 0.2f)
		{
			if (m_fTime >= 1.f)
			{
				m_isPlaying = false;
			}
			m_fDir = 1.f;
		}
		if (m_fTime >= 0.2)
		{
			m_bLightening[0] = true;
		}
		if (m_fTime >= 0.25)
		{
			m_bLightening[1] = true;
		}
		if (m_fTime >= 0.28)
		{
			m_bLightening[2] = true;
		}
		m_pRoundEffect->Set_Alpha(m_pRoundEffect->Get_Alpha() + fTimeDelta * m_fDir * 10.f);
	}


	for (_uint i = 0; i < 3; i++)
	{
		if (m_bLightening[i] == true)
		{
			m_pLightening[i]->Tick(fTimeDelta);
		}
	}
	m_pRoundEffect->Tick(fTimeDelta);
}

void CInfinityTower::Tower_Tick(_float fTimeDelta, POINT& ptMouse)
{
	_float fTermX = 137.f;
	if (m_pGameInstance->Get_MouseMove(MouseState::wheel) > 0)
	{
		if (m_pTowers[BRICK]->Get_Position().y > m_vInitialPoint.y)
		{
			m_vDefaultPoint.y -= fTimeDelta * 500.f;
		}
		if (m_pTowers[BRICK]->Get_Position().y <= m_vInitialPoint.y)
		{
			m_vDefaultPoint.y = m_vInitialPoint.y;
		}

	}
	if (m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
	{
			m_vDefaultPoint.y += fTimeDelta * 500.f;
	}

	/* 입장 버튼 */
	if (PtInRect(&m_pStartButton->Get_InitialRect(), ptMouse))
	{
		m_pStartButton->Set_Size(140.f, 80.f, 0.3f);
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			switch (m_iCurIndex)
			{
			case BRICK:
				CUI_Manager::Get_Instance()->Set_MiniGameStage(BRICK);
				break;
			default:
				break;
			}
			Exit_Tower();
		}
	}
	else
	{
		m_pStartButton->Set_Size(150.f, 100.f, 0.35f);
	}



	/* 타워 선택 */
	_bool isSelect{ false };

	for (size_t i = 0; i < TOWER_END; i++)
	{
		if (PtInRect(&m_pTowers[i]->Get_Rect(), ptMouse))
		{
			if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
			{
				m_pTowers[i]->Select_Object(true);
				isSelect = true;
				m_iCurIndex = (TOWER)i;
				break;
			}
		}
	}

	for (size_t i = 0; i < TOWER_END; i++)
	{
		if (isSelect && m_iCurIndex != i)
		{
			m_pTowers[i]->Select_Object(false);
		}
		m_pTowers[i]->Tick(fTimeDelta);
	}
}

void CInfinityTower::Exit_Tower()
{
	for (size_t i = 0; i < TOWER_END; i++)
	{
		m_pTowers[i]->Select_Object(false);
	}
	CUI_Manager::Get_Instance()->Set_FullScreenUI(false);
	m_vDefaultPoint = m_vInitialPoint;
	m_isActive = false;
}

CInfinityTower* CInfinityTower::Create(_dev pDevice, _context pContext)
{
	CInfinityTower* pInstance = new CInfinityTower(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CInfinityTower");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInfinityTower::Clone(void* pArg)
{
	CInfinityTower* pInstance = new CInfinityTower(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CInfinityTower");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInfinityTower::Free()
{
	__super::Free();
	if (!m_isPrototype)
	{
		for (_uint i = 0; i < 3; i++)
		{
			Safe_Release(m_pLightening[i]);
		}
		for (_uint i = 0; i < TOWER_END; i++)
		{
			Safe_Release(m_pTowers[i]);
		}
	}
	Safe_Release(m_pCloud);
	Safe_Release(m_pTitle);
	Safe_Release(m_pLeftBg);
	Safe_Release(m_pRoundEffect);
	Safe_Release(m_pExitButton);
	Safe_Release(m_pStartButton);
	Safe_Release(m_pCloudBottom);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
