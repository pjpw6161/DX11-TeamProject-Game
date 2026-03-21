#include "QuestBox.h"
#include "GameInstance.h"
#include "TextButton.h"

CQuestBox::CQuestBox(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CQuestBox::CQuestBox(const CQuestBox& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CQuestBox::Init_Prototype()
{
	return S_OK;
}

HRESULT CQuestBox::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	

	m_fSizeX = 200.f;
	m_fSizeY = 50.f;

	m_strQuestTitle = ((QUESTBOX_DESC*)pArg)->strQuestTitle;
	m_iNum = ((QUESTBOX_DESC*)pArg)->iNum;
	m_fMin = ((QUESTBOX_DESC*)pArg)->fMin;
	m_fMax = ((QUESTBOX_DESC*)pArg)->fMax;
	m_isMain = ((QUESTBOX_DESC*)pArg)->isMain;
	m_vPosition = ((QUESTBOX_DESC*)pArg)->vPosition;
	m_strQuestTitle = ((QUESTBOX_DESC*)pArg)->strQuestTitle;
	m_strText = ((QUESTBOX_DESC*)pArg)->strText;

	m_fX = m_vPosition.x - m_fSizeX / 2.f;
	m_fY = m_vPosition.y;
	m_fDepth = (_float)D_SCREEN / (_float)D_END;
	m_iOriginNum = m_iNum;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CQuestBox::Tick(_float fTimeDelta)
{
	//m_fX = 360.f - 100.f;
	//m_fY = 50.f;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	m_pArrow->Tick(fTimeDelta);
}

void CQuestBox::Late_Tick(_float fTimeDelta)
{
	m_pArrow->Late_Tick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

}

HRESULT CQuestBox::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_ScrollAlpha)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}
	_float fTextPosX = m_fX - 60.f;
	_float fFontSize = 0.3f;
	_vec4 vTextColor = _vec4(0.f, 0.4f, 1.f, 1.f);
	wstring strType = TEXT("[서브] ");
	wstring strNum = {};
	if (m_isMain)
	{
		strType = TEXT("[메인] ");
		vTextColor = _vec4(1.f, 0.7f, 0.1f, 1.f);
	}

	m_pGameInstance->Render_Text(L"Font_Malang", strType + m_strQuestTitle, _vec2(fTextPosX - 0.1f, m_fY - 18.f), fFontSize, vTextColor, 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", strType + m_strQuestTitle, _vec2(fTextPosX + 0.1f, m_fY - 18.f), fFontSize, vTextColor, 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", strType + m_strQuestTitle, _vec2(fTextPosX, m_fY - 18.f + 0.1f), fFontSize, vTextColor, 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", strType + m_strQuestTitle, _vec2(fTextPosX, m_fY - 18.f - 0.1f), fFontSize, vTextColor, 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", strType + m_strQuestTitle, _vec2(fTextPosX, m_fY - 18.f), fFontSize, vTextColor, 0.f, true);
	
	if (m_iOriginNum > 1)
	{
		strNum = TEXT("(") + to_wstring((m_iOriginNum - m_iNum)) + TEXT("/") + to_wstring(m_iOriginNum) + TEXT(")");
	}
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText + strNum, _vec2(fTextPosX - 0.1f, m_fY + 2.f), fFontSize, _vec4(0.0f, 0.0f, 0.0f, 1.0f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText + strNum, _vec2(fTextPosX + 0.1f, m_fY + 2.f), fFontSize, _vec4(0.0f, 0.0f, 0.0f, 1.0f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText + strNum, _vec2(fTextPosX, m_fY + 2.f + 0.1f), fFontSize, _vec4(0.0f, 0.0f, 0.0f, 1.0f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText + strNum, _vec2(fTextPosX, m_fY + 2.f - 0.1f), fFontSize, _vec4(0.0f, 0.0f, 0.0f, 1.0f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText + strNum, _vec2(fTextPosX, m_fY + 2.f), fFontSize, _vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.f, true);


	return S_OK;
}

HRESULT CQuestBox::Add_Parts()
{
	CTextButton::TEXTBUTTON_DESC Button = {};
	Button.eLevelID = LEVEL_STATIC;
	Button.strText = TEXT("");
	Button.vPosition = _vec2(m_fX - 80.f, m_fY);
	Button.vSize = _vec2(20.f, 20.f);
	Button.fDepth = m_fDepth - 0.01f;
	if (m_isMain)
	{
		Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_quest_main_arrow");
	}
	else
	{
		Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_quest_arrow");
	}

	m_pArrow = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pArrow)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CQuestBox::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_QuestAlpha"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CQuestBox::Bind_ShaderResources()
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
	_float fAlpha = 0.7f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof _float)))
	{
		return E_FAIL;
	}

	_float fRatio = 0.f;
	/*
	
	if (m_fY + m_fSizeY / 2.f > m_fMax)
	{
		fRatio = (m_fY + m_fSizeY / 2.f - m_fMax) / m_fSizeY;
		fRatio = 1.f - fRatio;
		if (fRatio >= 1.f)
		{
			fRatio = 1.f;
		}
	}
	if (m_fY - m_fSizeY / 2.f < m_fMin)
	{
		fRatio = (m_fMin - m_fY - m_fSizeY / 2.f) / m_fSizeY * -1.f;
		if (fRatio < -1.f)
		{
			fRatio = -1.f;
		}
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScrollRatio", &fRatio, sizeof _float)))
	{
		return E_FAIL;
	}
	*/
	return S_OK;
}

_bool CQuestBox::Update_Quest()
{
	m_iNum--;
	if (m_iNum <=0)
	{
		return true;
	}
	return false;
}
void CQuestBox::Set_Position(_vec2 vPos)
{
	m_fX = vPos.x;
	m_fY = vPos.y;
	dynamic_cast<CTextButton*>(m_pArrow)->Set_Position(_vec2(dynamic_cast<CTextButton*>(m_pArrow)->Get_Position().x, vPos.y));
}

CQuestBox* CQuestBox::Create(_dev pDevice, _context pContext)
{
	CQuestBox* pInstance = new CQuestBox(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CQuestBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CQuestBox::Clone(void* pArg)
{
	CQuestBox* pInstance = new CQuestBox(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CQuestBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuestBox::Free()
{
	__super::Free();

	Safe_Release(m_pArrow);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}