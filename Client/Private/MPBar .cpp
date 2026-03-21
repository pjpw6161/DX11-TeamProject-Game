#include "MPBar.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"

CMPBar::CMPBar(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CMPBar::CMPBar(const CMPBar& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CMPBar::Init_Prototype()
{
	return S_OK;
}

HRESULT CMPBar::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 193.f;
	m_fSizeY = 22.f;

	m_fX = 780.f;
	m_fY = 630.f;

	m_fDepth = (_float)D_BAR / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	m_vRatio = _float2(500.f, 500.f);

	CTextButton::TEXTBUTTON_DESC Button = {};
	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth + 0.01f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BarContext");
	Button.vPosition = _vec2(m_fX, m_fY);
	Button.vSize = _vec2(m_fSizeX + 2.f, m_fSizeY);

	m_pBackground = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pBackground)
	{
		return E_FAIL;
	}

	Button.fDepth = m_fDepth - 0.01f;
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BarBorder");
	m_pBorder= m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pBorder)
	{
		return E_FAIL;
	}

	return S_OK;
}

void CMPBar::Tick(_float fTimeDelta)
{
	m_vRatio = CUI_Manager::Get_Instance()->Get_Mp();
	m_fTime += fTimeDelta;
}

void CMPBar::Late_Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}

	m_pBorder->Late_Tick(fTimeDelta);
	m_pBackground->Late_Tick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CMPBar::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_HP)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(static_cast<int>(m_vRatio.x)) + TEXT(" / ") + to_wstring(static_cast<int>(m_vRatio.y)), _vec2(m_fX - 0.1f, m_fY + 0.f), 0.3f);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(static_cast<int>(m_vRatio.x)) + TEXT(" / ") + to_wstring(static_cast<int>(m_vRatio.y)), _vec2(m_fX + 0.1f, m_fY + 0.f), 0.3f);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(static_cast<int>(m_vRatio.x)) + TEXT(" / ") + to_wstring(static_cast<int>(m_vRatio.y)), _vec2(m_fX, m_fY + 0.1f), 0.3f);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(static_cast<int>(m_vRatio.x)) + TEXT(" / ") + to_wstring(static_cast<int>(m_vRatio.y)), _vec2(m_fX, m_fY + 0.9f), 0.3f);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(static_cast<int>(m_vRatio.x)) + TEXT(" / ") + to_wstring(static_cast<int>(m_vRatio.y)), _vec2(m_fX, m_fY + 0.f), 0.3f);

	return S_OK;
}

HRESULT CMPBar::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_MPBar"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Mask_FlagMove"), TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMPBar::Bind_ShaderResources()
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

	_float fRatio = m_vRatio.x / m_vRatio.y;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fHpRatio", &fRatio, sizeof(_float))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CMPBar* CMPBar::Create(_dev pDevice, _context pContext)
{
	CMPBar* pInstance = new CMPBar(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMPBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMPBar::Clone(void* pArg)
{
	CMPBar* pInstance = new CMPBar(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CMPBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMPBar::Free()
{
	__super::Free();

	Safe_Release(m_pBorder);
	Safe_Release(m_pBackground);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
