#include "Pop_LevelUp.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "BlurTexture.h"

CPop_LevelUp::CPop_LevelUp(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CPop_LevelUp::CPop_LevelUp(const CPop_LevelUp& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CPop_LevelUp::Init_Prototype()
{
	return S_OK;
}

HRESULT CPop_LevelUp::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 800.f;
	m_fSizeY = 800.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = 130.f;

	m_fDepth = (_float)D_QUEST / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);


	m_iLevel = ((LEVELUP_DESC*)pArg)->iLevel;

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CPop_LevelUp::Tick(_float fTimeDelta)
{
	
	if (m_fDeadTime >= 3.f)
	{
		m_fTime = 3.f;
		m_isDead = true;
	}
	if (m_fDeadTime >= 4.f)
	{
		m_fAlpha = 5.f - m_fDeadTime;
	}

	if (m_fTime >= 1.f)
	{
		m_fTime -= fTimeDelta * 8.f;
	}


	m_fDeadTime += fTimeDelta;
}

void CPop_LevelUp::Late_Tick(_float fTimeDelta)
{

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CPop_LevelUp::Render()
{

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_ChangeBright)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iLevel), _vec2((_float)g_iWinSizeX / 2.f - 5.f - 0.2f, 90.f), 0.8f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iLevel), _vec2((_float)g_iWinSizeX / 2.f - 5.f + 0.2f, 90.f), 0.8f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iLevel), _vec2((_float)g_iWinSizeX / 2.f - 5.f, 90.f - 0.2f), 0.8f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iLevel), _vec2((_float)g_iWinSizeX / 2.f - 5.f, 90.f + 0.2f), 0.8f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iLevel), _vec2((_float)g_iWinSizeX / 2.f - 5.f, 90.f), 0.8f, _vec4(1.f, 1.f, 0.f, 1.f));
	
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("레벨"), _vec2((_float)g_iWinSizeX / 2.f - 5.f - 0.2f, 115.f), 0.35f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("레벨"), _vec2((_float)g_iWinSizeX / 2.f - 5.f + 0.2f, 115.f), 0.35f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("레벨"), _vec2((_float)g_iWinSizeX / 2.f - 5.f, 115.f - 0.2f), 0.35f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("레벨"), _vec2((_float)g_iWinSizeX / 2.f - 5.f, 115.f + 0.2f), 0.35f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("레벨"), _vec2((_float)g_iWinSizeX / 2.f - 5.f, 115.f), 0.35f);

	return S_OK;
}

HRESULT CPop_LevelUp::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_LevelUp"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPop_LevelUp::Add_Parts()
{
	/*
	
	CBlurTexture::BLURTEX_DESC BLURTEXDesc = {};
	BLURTEXDesc.eLevelID = LEVEL_STATIC;
	BLURTEXDesc.fDepth = m_fDepth - 0.01f;
	BLURTEXDesc.fFontSize = 0.f;
	BLURTEXDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_ExclamationMark");
	BLURTEXDesc.vPosition = _vec2(m_fX, m_fY);
	BLURTEXDesc.vSize = _vec2(55.f, 60.f);
	BLURTEXDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	BLURTEXDesc.vColor = _vec4(1.0f, 0.5f, 0.1f, 1.f);
	BLURTEXDesc.vTextPosition = _vec2(0.f, 0.f);

	m_pExclamationMark = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BlurTexture"), &BLURTEXDesc);
	if (not m_pExclamationMark)
	{
		return E_FAIL;
	}


	CTextButton::TEXTBUTTON_DESC ButtonDesc = {};
	ButtonDesc.eLevelID = LEVEL_STATIC;
	ButtonDesc.fDepth = m_fDepth;
	ButtonDesc.fFontSize = 0.45f;
	ButtonDesc.strText = TEXT("");
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_SiegeQuest");
	ButtonDesc.vPosition = _vec2((_float)g_iWinSizeX / 2.f, 360.f);
	ButtonDesc.vSize = _vec2(360.f, 10.f);
	ButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ButtonDesc.vTextPosition = _vec2(20.f, 12.f);
	m_pBorder = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pBorder)
	{
		return E_FAIL;
	}

	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_ClickArrow");
	ButtonDesc.vPosition = _vec2(780.f, 565.f);
	ButtonDesc.vSize = _vec2(30.f, 30.f);

	m_pButton = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pButton)
	{
		return E_FAIL;
	}

	wstring fExp = to_wstring(m_fExp);
	size_t dotPos = fExp.find(L'.');
	if (dotPos != wstring::npos && fExp.length() > dotPos + 3) {
		fExp.erase(dotPos + 3);
	}
	ButtonDesc.strText = fExp +TEXT("%");

	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_exp");
	ButtonDesc.vPosition = _vec2((_float)g_iWinSizeX / 2.f - 120.f, 420.f);
	ButtonDesc.vSize = _vec2(35.f, 35.f);
	ButtonDesc.vTextPosition = _vec2(60.f, 12.f);

	m_pExp = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pExp)
	{
		return E_FAIL;
	}
	ButtonDesc.strText = to_wstring(m_iMoney);
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_coin");
	ButtonDesc.vPosition = _vec2((_float)g_iWinSizeX / 2.f + 90.f, 420.f);
	ButtonDesc.vSize = _vec2(35.f, 35.f);
	ButtonDesc.vTextPosition = _vec2(40.f, 12.f);
	m_pMoney = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pMoney)
	{
		return E_FAIL;
	}


	CFadeBox::STATE eState = CFadeBox::FADELOOP;
	m_pBackground = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_FadeBox"), &eState);
	if (not m_pBackground)
	{
		return E_FAIL;
	}
	*/
	return S_OK;
}

HRESULT CPop_LevelUp::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CPop_LevelUp* CPop_LevelUp::Create(_dev pDevice, _context pContext)
{
	CPop_LevelUp* pInstance = new CPop_LevelUp(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CPop_LevelUp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPop_LevelUp::Clone(void* pArg)
{
	CPop_LevelUp* pInstance = new CPop_LevelUp(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPop_LevelUp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPop_LevelUp::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
