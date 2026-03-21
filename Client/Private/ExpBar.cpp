#include "ExpBar.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "UI_Manager.h"
#include "Event_Manager.h"

CExpBar::CExpBar(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CExpBar::CExpBar(const CExpBar& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CExpBar::Init_Prototype()
{
	return S_OK;
}

HRESULT CExpBar::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}





	m_fSizeX = g_iWinSizeX;
	m_fSizeY = 10.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = 720.f;

	m_fDepth = (_float)D_BAR / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	m_vRatio = _float2(0.f, 100.f);


	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = m_fDepth + 0.01f;
	ColButtonDesc.fAlpha = 0.7f;
	ColButtonDesc.vColor = _vec4(0.f, 0.f, 0.f, 0.7f);
	ColButtonDesc.strText = TEXT("");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BG");
	ColButtonDesc.vPosition = _vec2(m_fX, m_fY);
	ColButtonDesc.vSize = _vec2(m_fSizeX, m_fSizeY + 5.f);
	ColButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ColButtonDesc.vTextPosition = _vec2(0.f, 20.f);

	m_pBackground = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);


	if (not m_pBackground)
	{
		return E_FAIL;
	}


	return S_OK;
}

void CExpBar::Tick(_float fTimeDelta)
{
	m_vRatio = CUI_Manager::Get_Instance()->Get_Exp();
	m_iLevel = CUI_Manager::Get_Instance()->Get_Level();
	m_fTime += fTimeDelta;
}

void CExpBar::Late_Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}
	m_pBackground->Late_Tick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CExpBar::Render()
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
	_float Exp = 100.f * (m_vRatio.x / m_vRatio.y);
	m_pGameInstance->Render_Text(L"Font_Dialogue",  TEXT("Lv. ") + to_wstring(m_iLevel), _vec2(50.f - 0.1f, m_fY - 20.f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Dialogue",  TEXT("Lv. ") + to_wstring(m_iLevel), _vec2(50.f + 0.1f, m_fY - 20.f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Dialogue",  TEXT("Lv. ") + to_wstring(m_iLevel), _vec2(50.f, m_fY - 20.f - 0.1f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Dialogue",  TEXT("Lv. ") + to_wstring(m_iLevel), _vec2(50.f, m_fY - 20.f + 0.1f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Dialogue",  TEXT("Lv. ") + to_wstring(m_iLevel), _vec2(50.f, m_fY - 20.f), 0.7f, _vec4(1.f, 1.f, 1.f, 1.f));

	wstring fExp = to_wstring(Exp);
	size_t dotPos = fExp.find(L'.');
	if (dotPos != wstring::npos && fExp.length() > dotPos + 3) {
		fExp.erase(dotPos + 3);
	}
	m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("경험치 ") + fExp + TEXT("% "),  _vec2(140.f - 0.1f, m_fY - 15.f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("경험치 ") + fExp + TEXT("% "), _vec2(140.f + 0.1f, m_fY  - 15.f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("경험치 ") + fExp + TEXT("% "), _vec2(140.f, m_fY - 15.f - 0.1f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("경험치 ") + fExp + TEXT("% "), _vec2(140.f, m_fY - 15.f + 0.1f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("경험치 ") + fExp + TEXT("% "), _vec2(140.f, m_fY - 15.f), 0.3f, _vec4(0.6196f, 0.8509f, 0.0196f, 1.f));


	return S_OK;
}

HRESULT CExpBar::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_EXPBar"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_maskno"), TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CExpBar::Bind_ShaderResources()
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
	_vec4 vColor = _vec4(0.6196f, 0.8509f, 0.0196f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vec4))))
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

CExpBar* CExpBar::Create(_dev pDevice, _context pContext)
{
	CExpBar* pInstance = new CExpBar(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CExpBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CExpBar::Clone(void* pArg)
{
	CExpBar* pInstance = new CExpBar(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CExpBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExpBar::Free()
{
	__super::Free();

	Safe_Release(m_pBackground);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
