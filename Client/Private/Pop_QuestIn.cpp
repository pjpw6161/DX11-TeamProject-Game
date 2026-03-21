#include "Pop_QuestIn.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "BlurTexture.h"
#include "UI_Manager.h"

CPop_QuestIn::CPop_QuestIn(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CPop_QuestIn::CPop_QuestIn(const CPop_QuestIn& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CPop_QuestIn::Init_Prototype()
{
	return S_OK;
}

HRESULT CPop_QuestIn::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 300.f;
	m_fSizeY = 300.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = 100.f;

	m_fDepth = (_float)D_QUEST / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	m_isMain = ((QUESTIN_DESC*)pArg)->isMain;
	m_strQuestTitle = ((QUESTIN_DESC*)pArg)->strQuestTitle;
	m_strText = ((QUESTIN_DESC*)pArg)->strText;
	m_fExp = ((QUESTIN_DESC*)pArg)->fExp;
	m_iMoney = ((QUESTIN_DESC*)pArg)->iMoney;

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}


	m_fStartButtonPos = dynamic_cast<CTextButton*>(m_pButton)->Get_Position();
	m_fButtonTime = m_fStartButtonPos.y;
	return S_OK;
}

void CPop_QuestIn::Tick(_float fTimeDelta)
{
	
	if (m_fDeadTime >= 0.8f && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
	{
		m_isDead = true;
	}

	m_fTime += fTimeDelta * 0.2f;
	m_fDeadTime += fTimeDelta;

	if (dynamic_cast<CTextButton*>(m_pButton)->Get_TransPosition().y <= dynamic_cast<CTextButton*>(m_pButton)->Get_Position().y - 5.f)
	{
		m_fDir = 0.6f;
		dynamic_cast<CTextButton*>(m_pButton)->Set_Position(_float2(dynamic_cast<CTextButton*>(m_pButton)->Get_Position().x, dynamic_cast<CTextButton*>(m_pButton)->Get_Position().y - 5.f));
	}
	if (dynamic_cast<CTextButton*>(m_pButton)->Get_TransPosition().y >= dynamic_cast<CTextButton*>(m_pButton)->Get_Position().y)
	{
		m_fDir = -1.f;
		dynamic_cast<CTextButton*>(m_pButton)->Set_Position(_float2(dynamic_cast<CTextButton*>(m_pButton)->Get_Position().x, dynamic_cast<CTextButton*>(m_pButton)->Get_Position().y));
	}
	m_fButtonTime += fTimeDelta * m_fDir * 10.f;
	dynamic_cast<CTextButton*>(m_pButton)->Set_Position(_float2(dynamic_cast<CTextButton*>(m_pButton)->Get_Position().x, m_fButtonTime));




	m_pBackground->Tick(fTimeDelta);
	m_pButton->Tick(fTimeDelta);


}

void CPop_QuestIn::Late_Tick(_float fTimeDelta)
{
	m_pExp->Late_Tick(fTimeDelta);
	m_pMoney->Late_Tick(fTimeDelta);
	m_pButton->Late_Tick(fTimeDelta);
	m_pExclamationMark->Late_Tick(fTimeDelta);
	m_pBorder->Late_Tick(fTimeDelta);
	m_pBackground->Late_Tick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CPop_QuestIn::Render()
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

	m_pGameInstance->Render_Text(L"Font_Malang", m_strQuestTitle, _vec2((_float)g_iWinSizeX / 2.f - 0.2f, 230.f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strQuestTitle, _vec2((_float)g_iWinSizeX / 2.f + 0.2f, 230.f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strQuestTitle, _vec2((_float)g_iWinSizeX / 2.f, 230.f - 0.2f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strQuestTitle, _vec2((_float)g_iWinSizeX / 2.f, 230.f + 0.2f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strQuestTitle, _vec2((_float)g_iWinSizeX / 2.f, 230.f), 0.7f, _vec4(1.f, 1.f, 0.f, 1.f));
	
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2((_float)g_iWinSizeX / 2.f - 0.2f, 280.f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2((_float)g_iWinSizeX / 2.f + 0.2f, 280.f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2((_float)g_iWinSizeX / 2.f, 280.f - 0.2f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2((_float)g_iWinSizeX / 2.f, 280.f + 0.2f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2((_float)g_iWinSizeX / 2.f, 280.f), 0.7f);

	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("화면을 눌러 퀘스트를 수락하세요."), _vec2((_float)g_iWinSizeX / 2.f - 0.2f, 580.f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("화면을 눌러 퀘스트를 수락하세요."), _vec2((_float)g_iWinSizeX / 2.f + 0.2f, 580.f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("화면을 눌러 퀘스트를 수락하세요."), _vec2((_float)g_iWinSizeX / 2.f, 580.f - 0.2f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("화면을 눌러 퀘스트를 수락하세요."), _vec2((_float)g_iWinSizeX / 2.f, 580.f + 0.2f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("화면을 눌러 퀘스트를 수락하세요."), _vec2((_float)g_iWinSizeX / 2.f, 580.f), 0.4f, _vec4(0.6f, 0.6f, 0.6f, 1.f));

	return S_OK;
}

HRESULT CPop_QuestIn::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Quest"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Noise_04"), TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPop_QuestIn::Add_Parts()
{
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
	ButtonDesc.vPosition = _vec2(780.f, 585.f);
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
	ButtonDesc.vTextPosition = _vec2(60.f, 0.f);

	m_pExp = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pExp)
	{
		return E_FAIL;
	}
	ButtonDesc.strText = to_wstring(m_iMoney);
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_coin");
	ButtonDesc.vPosition = _vec2((_float)g_iWinSizeX / 2.f + 90.f, 420.f);
	ButtonDesc.vSize = _vec2(35.f, 35.f);
	ButtonDesc.vTextPosition = _vec2(60.f, 0.f);
	m_pMoney = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pMoney)
	{
		return E_FAIL;
	}


	CFadeBox::FADE_DESC Desc = {};
	Desc.fMaxAlpha = 0.7f;
	Desc.isInfiniteLoop = true;
	m_pBackground = CUI_Manager::Get_Instance()->Clone_FadeBox(Desc);
	if (not m_pBackground)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPop_QuestIn::Bind_ShaderResources()
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
	/*
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fx", &m_fTime, sizeof(_float))))
	{
		return E_FAIL;
	}	
	*/

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fx", &m_fTime, sizeof(_float))))
	{
		return E_FAIL;
	}
	_float fTemp = 0.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &fTemp, sizeof(_float))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CPop_QuestIn* CPop_QuestIn::Create(_dev pDevice, _context pContext)
{
	CPop_QuestIn* pInstance = new CPop_QuestIn(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CPop_QuestIn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPop_QuestIn::Clone(void* pArg)
{
	CPop_QuestIn* pInstance = new CPop_QuestIn(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPop_QuestIn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPop_QuestIn::Free()
{
	__super::Free();

	Safe_Release(m_pMoney);
	Safe_Release(m_pExp);
	Safe_Release(m_pBorder);
	Safe_Release(m_pExclamationMark);
	Safe_Release(m_pBackground);
	Safe_Release(m_pButton);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
