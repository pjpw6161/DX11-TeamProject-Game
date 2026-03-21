#include "HPBoss.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"
#include "3DUITex.h"

CHPBoss::CHPBoss(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CHPBoss::CHPBoss(const CHPBoss& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CHPBoss::Init_Prototype()
{
	return S_OK;
}

HRESULT CHPBoss::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_iMaxHp = ((HPBOSS_DESC*)pArg)->iMaxHp;
	m_eLevel = ((HPBOSS_DESC*)pArg)->eLevelID;
	m_strName = ((HPBOSS_DESC*)pArg)->strName;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = 90.f;
	m_fSizeX = 435.f;
	m_fSizeY = 40.f;
	m_fDepth = (_float)D_SCREEN / (_float)D_END;

	m_iCurHp = m_iMaxHp;
	m_iTargetHp = m_iCurHp;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	CUI_Manager::Get_Instance()->Set_isBoss(true);
	//	CUI_Manager::Get_Instance()->Set_isBoss(false); 이거는 보스 죽을때

	if (m_strName == TEXT(""))
	{
		//텍스쳐 변경
	}
	CTextButton::TEXTBUTTON_DESC Button = {};
	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Boss_BossName");
	Button.vPosition = _vec2(m_fX, m_fY - 40.f);
	Button.vSize = _vec2(400.f, 36.f);
	Button.fFontSize = 0.5f;
	Button.vTextPosition = _vec2(120.f, 0.f);
	Button.vTextColor = _vec4(0.5f, 0.07f, 0.04f, 1.f);
	m_pIcon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pIcon)
	{
		return E_FAIL;
	}
	

	return S_OK;
}

void CHPBoss::Tick(_float fTimeDelta)
{
	m_fTime += fTimeDelta;
	if (m_iTargetHp <= 0)
	{
		m_iTargetHp = 0;
	}

	m_bEffect = false;

	if (m_iTargetHp < m_iCurHp)
	{
		m_bEffect = true;
		_float fHp = fTimeDelta * 500.f;
		m_iCurHp -= (_int)fHp;
		
	}

	if (m_iCurHp <= 0)
	{
		m_iCurHp = 0;
	}

	m_pIcon->Tick(fTimeDelta);
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CHPBoss::Late_Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}
	m_pIcon->Late_Tick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CHPBoss::Render()
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

	if (FAILED(m_pBGTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
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


	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_HPBoss)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pBorderTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
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

HRESULT CHPBoss::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Boss_BossHp"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Boss_BossHPBorder"), TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pBorderTextureCom))))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Boss_BossHpBG"), TEXT("Com_Texture3"), reinterpret_cast<CComponent**>(&m_pBGTextureCom))))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Mask_FlagMove"), TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CHPBoss::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
	{
		return E_FAIL;
	}
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bOn", &m_bEffect, sizeof(_bool))))
	{
		return E_FAIL;
	}

	_float fRatio = (_float)m_iCurHp / (_float)m_iMaxHp;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fHpRatio", &fRatio, sizeof(_float))))
	{
		return E_FAIL;
	}
	return S_OK;
}

CHPBoss* CHPBoss::Create(_dev pDevice, _context pContext)
{
	CHPBoss* pInstance = new CHPBoss(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CHPBoss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHPBoss::Clone(void* pArg)
{
	CHPBoss* pInstance = new CHPBoss(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CHPBoss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHPBoss::Free()
{
	__super::Free();

	Safe_Release(m_pIcon);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBGTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pBorderTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
