#include "SkillDesc.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "TextButton.h"
#include "ItemInfo.h"
#include "Item.h"
#include "TextButtonColor.h"

CSkillDesc::CSkillDesc(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CSkillDesc::CSkillDesc(const CSkillDesc& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CSkillDesc::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CSkillDesc::Init(void* pArg)
{
	m_tSkillInfo = ((SKILLBOOK_DESC*)pArg)->tSkillInfo;

	m_fSizeX = 300.f;
	m_fSizeY = 90.f;

	m_fX = ((SKILLBOOK_DESC*)pArg)->vPosition.x;
	m_fY = ((SKILLBOOK_DESC*)pArg)->vPosition.y;

	m_fDepth = ((SKILLBOOK_DESC*)pArg)->fDepth;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	

	if (FAILED(Set_Type()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	m_rcRect = {
		  (LONG)(m_fX - m_fSizeX * 0.5f),
		  (LONG)(m_fY - m_fSizeY * 0.5f),
		  (LONG)(m_fX + m_fSizeX * 0.5f),
		  (LONG)(m_fY + m_fSizeY * 0.5f)
	};



	m_bSkillIn = true;
	return S_OK;
}

void CSkillDesc::Tick(_float fTimeDelta)
{
	m_rcRect = {
	  (LONG)(m_fX - m_fSizeX * 0.5f),
	  (LONG)(m_fY - m_fSizeY * 0.5f),
	  (LONG)(m_fX + m_fSizeX * 0.5f),
	  (LONG)(m_fY + m_fSizeY * 0.5f)
	};
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);


	if (CUI_Manager::Get_Instance()->Get_SkillInfo((WEAPON_TYPE)m_tSkillInfo.iSkillType, m_tSkillInfo.iSkillIdx).isSkillIn)
	{
		if (!m_bSkillIn)
		{
			Unlock_Skill();
			m_bSkillIn = true;
		}

	}

	if (m_bSelect)
	{
		_vec2 vPos = m_pDetail->Get_TransPosition();
		_vec2 vSize = m_pDetail->Get_Size();
		if (vPos.x < vSize.x / 2.f)
		{
			m_pDetail->Set_Position(_vec2(vPos.x + fTimeDelta * 50.f, vPos.y));

		}

		m_fAlpha += fTimeDelta * 2.f;
		if (m_fAlpha >= 1.f)
		{
			m_fAlpha = 1.f;
		}
		m_pDetail->Set_Alpha(m_fAlpha);

		m_pDetail->Tick(fTimeDelta);
		m_pSelect->Tick(fTimeDelta);
	}
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CSkillDesc::Late_Tick(_float fTimeDelta)
{
	if (m_bSelect)
	{
		m_pDetail->Late_Tick(fTimeDelta);
		m_pSelect->Late_Tick(fTimeDelta);
	}
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CSkillDesc::Render()
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


	_vec2 vStartPos = _vec2(m_fX - 60.f, m_fY - 10.f);
	m_pGameInstance->Render_Text(L"Font_Malang", m_tSkillInfo.strName, _vec2(vStartPos.x + 1.f, vStartPos.y), 0.45f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", m_tSkillInfo.strName, _vec2(vStartPos.x, +vStartPos.y + 1.f), 0.45f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", m_tSkillInfo.strName, _vec2(vStartPos.x, +vStartPos.y), 0.45f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);

	if(!m_bSkillIn)
	{
		vStartPos = _vec2(m_fX + 100.f, m_fY + 30.f);
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("미습득"), _vec2(vStartPos.x + 1.f, vStartPos.y), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("미습득"), _vec2(vStartPos.x, +vStartPos.y + 1.f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("미습득"), _vec2(vStartPos.x, +vStartPos.y), 0.3f, _vec4(1.f, 0.f, 0.f, 1.f), 0.f);
	}



	return S_OK;
}

const SKILLINFO& CSkillDesc::Get_SkillInfo() const
{
	return m_tSkillInfo;
}
void CSkillDesc::Set_Position(_vec2 vPos)
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

void CSkillDesc::Unlock_Skill()
{
	if (m_eCurType == LOCK)
	{
		m_eCurType = UNLOCK;
	}
}

void CSkillDesc::Select_Skill(_bool isSelect)
{
	if (!m_bSelect && isSelect == true)
	{
		m_pDetail->Set_Position(_vec2(m_pDetail->Get_Position()));
		m_fAlpha = 0.f;
	}
	m_bSelect = isSelect;
}

HRESULT CSkillDesc::Set_Type()
{
	wstring strName = m_tSkillInfo.strName;
	if (strName == TEXT("환영화살"))
	{
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_sk1");
		m_strDetailTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_bowskill1");
	}
	else if (strName == TEXT("화살비"))
	{
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_sk2");
		m_strDetailTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_bowskill2");
	}
	else if (strName == TEXT("폭발 화살"))
	{
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_sk3");
		m_strDetailTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_bowskill3");
	}
	else if (strName == TEXT("삼중 연사"))
	{
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_sk4");
		m_strDetailTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_bowskill4");
	}
	else if (strName == TEXT("전광 석화"))
	{
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_sk5");
		m_strDetailTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_skill1");
	}
	else if (strName == TEXT("기습 공격"))
	{
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_sk7");
		m_strDetailTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_skill2");
	}	
	else if (strName == TEXT("인장 각인"))
	{
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_sk6");
		m_strDetailTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_skill3");
	}
	else if (strName == TEXT("연속 각인"))
	{
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_sk8");
		m_strDetailTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_skill4");
	}


	return S_OK;
}

HRESULT CSkillDesc::Add_Parts()
{
	CTextButton::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.fFontSize = 0.5f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_skSelect");
	Button.vPosition = _vec2(m_fX, m_fY);
	Button.vSize = _vec2(m_fSizeX, m_fSizeY);
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(70.f, 0.f);

	m_pSelect = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pSelect)
	{
		return E_FAIL;
	}

	CTextButtonColor::TEXTBUTTON_DESC TextButton = {};
	TextButton.eLevelID = LEVEL_STATIC;
	TextButton.strTexture = TEXT("");
	TextButton.fDepth = m_fDepth - 0.01f;
	TextButton.fAlpha = 1.f;
	TextButton.fFontSize = 0.45f;
	TextButton.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.strText = TEXT("");
	TextButton.vSize = _vec2(513.f, 513.f);
	TextButton.vPosition = _vec2(TextButton.vSize.x / 2.f - 10.f, TextButton.vSize.x / 2.f + 20.f);
	TextButton.vTextPosition = _vec2(0.f, 0.f);


	TextButton.strTexture = m_strDetailTexture;
	m_pDetail = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pDetail)
	{
		return E_FAIL;
	}
	m_pDetail->Set_Pass(VTPass_UI_Alpha);
	return S_OK;
}

HRESULT CSkillDesc::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_skLock"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[LOCK]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strTexture, TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom[UNLOCK]))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSkillDesc::Bind_ShaderResources()
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

	if (FAILED(m_pTextureCom[m_eCurType]->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	return S_OK;
}

CSkillDesc* CSkillDesc::Create(_dev pDevice, _context pContext)
{
	CSkillDesc* pInstance = new CSkillDesc(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkillDesc::Clone(void* pArg)
{
	CSkillDesc* pInstance = new CSkillDesc(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSkillDesc");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkillDesc::Free()
{
	__super::Free();

	Safe_Release(m_pSelect);
	Safe_Release(m_pDetail);
	
	if (!m_isPrototype)
	{
		for (size_t i = 0; i < TYPE_END; i++)
		{
			Safe_Release(m_pTextureCom[i]);
		}
	}


	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
