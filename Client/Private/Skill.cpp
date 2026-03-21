#include "Skill.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "ItemInfo.h"

CSkill::CSkill(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CSkill::CSkill(const CSkill& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CSkill::Init_Prototype()
{
	return S_OK;
}

HRESULT CSkill::Init(void* pArg)
{

	m_tSkillInfo = ((SKILL_DESC*)pArg)->tSkillInfo;
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_isScreen = ((SKILL_DESC*)pArg)->isScreen;
	m_fSizeX = ((SKILL_DESC*)pArg)->vSize.x;
	m_fSizeY = ((SKILL_DESC*)pArg)->vSize.y;

	m_fX = ((SKILL_DESC*)pArg)->vPosition.x;
	m_fY = ((SKILL_DESC*)pArg)->vPosition.y;

	m_fDepth = ((SKILL_DESC*)pArg)->fDepth;
	m_fCoolTime = (_float)m_tSkillInfo.iCoolTime;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	

	m_rcRect = {
		  (LONG)(m_fX - m_fSizeX * 0.5f),
		  (LONG)(m_fY - m_fSizeY * 0.5f),
		  (LONG)(m_fX + m_fSizeX * 0.5f),
		  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

	if (m_isScreen)
	{
		CTextButtonColor::TEXTBUTTON_DESC TextButton = {};
		TextButton.eLevelID = LEVEL_STATIC;
		TextButton.fDepth = m_fDepth - 0.02f;
		TextButton.fAlpha = 0.8f;
		TextButton.strText = TEXT("");
		TextButton.vPosition = _vec2(m_fX, m_fY);
		TextButton.vSize = _vec2(60.f, 60.f);
		TextButton.vTextPosition = _vec2(0.f, 0.f);
		TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BloomRectBlue");
		m_pBorder = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
		if (not m_pBorder)
		{
			return E_FAIL;
		}
		m_pBorder->Set_Pass(VTPass_UI_Alpha);

		CTextButton::TEXTBUTTON_DESC Button{};
		Button.eLevelID = LEVEL_STATIC;
		Button.fDepth = m_fDepth - 0.01f;
		Button.strText = TEXT(" ");
		Button.vPosition = _vec2(m_fX, m_fY);
		Button.vSize = _vec2(40.f, 40.f);
		Button.vTextPosition = _vec2(0.f, 0.f);
		Button.fFontSize = 0.5f;
		Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
		Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_QuestAlpha");
		m_pBackGround = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
		if (not m_pBackGround)
		{
			return E_FAIL;
		}
	}
	return S_OK;
}

void CSkill::Tick(_float fTimeDelta)
{

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (m_isUsingSkill && m_isScreen)
	{
		if (m_fDir < 0.f && m_fAlpha < 0.4f)
		{
			m_fDir = 1.f;
		}
		if (m_fDir > 0.f && m_fAlpha > 0.9f)
		{
			m_fDir = -1.f;
		}

		m_fAlpha += m_fDir * fTimeDelta * 0.8f;
		m_pBorder->Set_Alpha(m_fAlpha);

		m_fCoolTime -= fTimeDelta;
		if (m_fCoolTime <= 0.f)
		{
			m_fCoolTime = 0.f;
			m_isUsingSkill = false;
		}


		m_pBorder->Tick(fTimeDelta);
		m_pBackGround->Tick(fTimeDelta);
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CSkill::Late_Tick(_float fTimeDelta)
{
	if (m_isUsingSkill && m_isScreen)
	{
		m_pBorder->Late_Tick(fTimeDelta);
		m_pBackGround->Late_Tick(fTimeDelta);
	}
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

}

HRESULT CSkill::Render()
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
	if (m_isScreen && m_isUsingSkill)
	{
		wstring strCoolTime = to_wstring(m_fCoolTime);
		size_t dotPos = strCoolTime.find(L'.');
		if (dotPos != wstring::npos && strCoolTime.length() > dotPos + 2) {
			strCoolTime.erase(dotPos + 2);
		}
		m_pBackGround->Set_Text(strCoolTime);

	}
	if (!m_isUsingSkill && m_isScreen)
	{
		m_pBackGround->Set_Text(TEXT(" "));
	}

	return S_OK;
}

void CSkill::Set_Position(_vec2 vPos)
{
	m_fX = vPos.x;
	m_fY = vPos.y;	
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	m_rcRect = {
	  (LONG)(m_fX - m_fSizeX * 0.5f),
	  (LONG)(m_fY - m_fSizeY * 0.5f),
	  (LONG)(m_fX + m_fSizeX * 0.5f),
	  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

}

_bool CSkill::Use_Skill()
{
	if (!m_isUsingSkill)
	{
		m_isUsingSkill = true;
		m_fCoolTime = (_float)m_tSkillInfo.iCoolTime;
		return true;
	}
	return false;
}

HRESULT CSkill::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_tSkillInfo.strTexture, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CSkill::Bind_ShaderResources()
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

CSkill* CSkill::Create(_dev pDevice, _context pContext)
{
	CSkill* pInstance = new CSkill(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSkill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill::Clone(void* pArg)
{
	CSkill* pInstance = new CSkill(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSkill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill::Free()
{
	__super::Free();
	Safe_Release(m_pBorder);
	Safe_Release(m_pBackGround);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
