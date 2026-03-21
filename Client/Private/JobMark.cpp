#include "JobMark.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"

CJobMark::CJobMark(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CJobMark::CJobMark(const CJobMark& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CJobMark::Init_Prototype()
{
	return S_OK;
}

HRESULT CJobMark::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 120.f;
	m_fSizeY = 120.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = 630.f;

	m_fDepth = (_float)D_JOBMARK / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);


	CTextButtonColor::TEXTBUTTON_DESC Button = {};
	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Image_Emblem_02");
	Button.strTexture2 = TEXT("Prototype_Component_Texture_UI_Gameplay_BG_Yggdrasil_E02");
	Button.vPosition = _vec2(m_fX, m_fY);
	Button.vSize = _vec2(140.f, 140.f);

	m_pJob = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &Button);
	if (not m_pJob)
	{
		return E_FAIL;
	}
	m_pJob->Set_Pass(VTPass_Bright);

	m_eType = WP_SWORD;


	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CJobMark::Tick(_float fTimeDelta)
{

	WEAPON_TYPE eType{};
	_int iIdx = CUI_Manager::Get_Instance()->Get_WeaponType(PT_SWORD, &eType);
	if(m_eType != eType)
	{

		Safe_Release(m_pJob);
		Safe_Release(m_pAlphaJob);

		CTextButtonColor::TEXTBUTTON_DESC Button = {};
		Button.eLevelID = LEVEL_STATIC;
		Button.fDepth = m_fDepth - 0.01f;
		Button.vPosition = _vec2(m_fX, m_fY);
		Button.vSize = _vec2(140.f, 140.f);

		switch (eType)
		{
		case WP_BOW:
			Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Image_Emblem_01");
			Button.strTexture2 = TEXT("Prototype_Component_Texture_UI_Gameplay_BG_Yggdrasil_E01");
			m_pSmoke->Set_Color(_vec4(0.6f, 0.8f, 0.9f, 1.f));
			break;
		case WP_SWORD:
			Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Image_Emblem_02");
			Button.strTexture2 = TEXT("Prototype_Component_Texture_UI_Gameplay_BG_Yggdrasil_E02");
			m_pSmoke->Set_Color(_vec4(0.f, 0.f, 0.f, 1.f));
			break;
		}
		m_pJob = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &Button);

		Button.vSize = _vec2(300.f, 300.f);
		m_pAlphaJob = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &Button);

		if (not m_pJob || not m_pAlphaJob)
		{
			return;

		}
		m_pJob->Set_Pass(VTPass_Bright);
		m_pAlphaJob->Set_Pass(VTPass_UI_Alpha);
		m_pAlphaJob->Set_Alpha(0.45f);

		m_pJob->Set_Factor(4.f);
		m_fBright = 4.f;
		m_pSmoke->Set_Factor(0.f);
		m_pRaySec->Set_Size(80.f, 80.f);
		m_pRaySec->Set_Alpha(0.8f);
		m_pRay->Set_Alpha(0.8f);
		m_pRay->Set_Size(80.f, 80.f);
		m_fIndex = 0.f;
		m_bStartEffect = true;
		m_fTime = 0.f;
		m_ePrevState = m_eCurState;
		m_eType = eType;


	}
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);


	if (m_bStartEffect)
	{
		if (m_pAlphaJob->Get_Size().x > 140.f)
		{
			m_pAlphaJob->Set_Size(m_pAlphaJob->Get_Size().x - fTimeDelta * 800.f, m_pAlphaJob->Get_Size().y - fTimeDelta * 800.f);
		}

		if (m_fBright >= 1.f)
		{
			m_fBright -= fTimeDelta * 10.f;
			m_pJob->Set_Factor(m_fBright);
		}

		m_fIndex += 24.f * fTimeDelta;
		if (m_fIndex >= 24.f)
		{
			m_bStartEffect = false;
		}
		if (m_fIndex >= 18.f)
		{
			m_fTime += fTimeDelta * 2.f;
			if (m_fTime <= 1.f)
			{
				m_pSmoke->Set_Factor(m_fTime);
			}

		}

		m_pSmoke->Set_Index(m_fIndex);

		if (m_pRay->Get_Size().x <= 280.f)
		{
			m_pRay->Set_Size(m_pRay->Get_Size().x + fTimeDelta * 1000.f, m_pRay->Get_Size().y + fTimeDelta * 1000.f);
		}
		else
		{
			if (m_pRay->Get_Alpha() >= 0.f)
			{
				m_pRay->Set_Alpha(m_pRay->Get_Alpha() - fTimeDelta);
			}
		}
		if (m_pRaySec->Get_Size().x <= 310.f)
		{
			m_pRaySec->Set_Size(m_pRaySec->Get_Size().x + fTimeDelta *800.f, m_pRaySec->Get_Size().y + fTimeDelta * 800.f);
		}
		else
		{
			if (m_pRaySec->Get_Alpha() >= 0.f)
			{
				m_pRaySec->Set_Alpha(m_pRaySec->Get_Alpha() - fTimeDelta);
			}
		}


		m_pRay->Tick(fTimeDelta);
		m_pRaySec->Tick(fTimeDelta);
		m_pSmoke->Tick(fTimeDelta);
		m_pAlphaJob->Tick(fTimeDelta);
	}
	if(!m_bStartEffect && m_pJob != nullptr)
	{
		m_pJob->Set_Factor(1.f);
	}
}

void CJobMark::Late_Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
	if (m_pJob != nullptr)
	{
		m_pJob->Late_Tick(fTimeDelta);
	}

	if (m_bStartEffect)
	{
		m_pRaySec->Late_Tick(fTimeDelta);
		m_pRay->Late_Tick(fTimeDelta);
		m_pSmoke->Late_Tick(fTimeDelta);
		m_pAlphaJob->Late_Tick(fTimeDelta);
	}
}

HRESULT CJobMark::Render()
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

HRESULT CJobMark::Add_Parts()
{

	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = m_fDepth - 0.01f;
	ColButtonDesc.fAlpha = 0.8f;
	ColButtonDesc.fFontSize = 0.f;
	ColButtonDesc.strText = TEXT("");
	ColButtonDesc.strTexture2 = TEXT("Prototype_Component_Texture_Effect_FX_J_Noise_Tile009_Tex");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_Effect_FX_A_Dust008_Tex");
	ColButtonDesc.vSize = _vec2(200.f, 200.f);
	ColButtonDesc.vPosition = _vec2(m_fX, m_fY);
	ColButtonDesc.vColor = _vec4(0.36f, 0.24f, 0.56f, 1.f);

	m_pSmoke = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pSmoke)
	{
		return E_FAIL;
	}
	m_pSmoke->Set_Sprite(_int2(4, 6));
	m_pSmoke->Set_Pass(VTPass_SpriteMaskColor_Dissolve);

	ColButtonDesc.fDepth = m_fDepth + 0.01f;
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Fx_Ray");
	ColButtonDesc.vSize = _vec2(80.f, 80.f);
	ColButtonDesc.vPosition = _vec2(m_fX, m_fY);
	ColButtonDesc.vColor = _vec4(0.9f, 0.9f, 1.f, 1.f);
	
	m_pRay = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pRay)
	{
		return E_FAIL;
	}
	m_pRay->Set_Pass(VTPass_Mask_ColorAlpha);
	

	m_pRaySec = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pRaySec)
	{
		return E_FAIL;
	}
	m_pRaySec->Set_Pass(VTPass_Mask_ColorAlpha);


	return S_OK;
}

HRESULT CJobMark::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Job"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CJobMark::Bind_ShaderResources()
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

CJobMark* CJobMark::Create(_dev pDevice, _context pContext)
{
	CJobMark* pInstance = new CJobMark(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CJobMark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CJobMark::Clone(void* pArg)
{
	CJobMark* pInstance = new CJobMark(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CJobMark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJobMark::Free()
{
	__super::Free();

	Safe_Release(m_pSmoke);
	Safe_Release(m_pJob);
	Safe_Release(m_pAlphaJob);

	Safe_Release(m_pRay);
	Safe_Release(m_pRaySec);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
