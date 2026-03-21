#include "Pop_Skill.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "UI_Manager.h"
#include "Event_Manager.h"
#include "BlurTexture.h"
#include "FadeBox.h"
#include "Skill.h"

CPop_Skill::CPop_Skill(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CPop_Skill::CPop_Skill(const CPop_Skill& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CPop_Skill::Init_Prototype()
{
	return S_OK;
}

HRESULT CPop_Skill::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 10.f;
	m_fSizeY = 10.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = 100.f;

	m_fDepth = (_float)D_QUEST / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);


	m_iSkillNum = ((SKILLIN_DESC*)pArg)->iSkillLevel;

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}


	SKILLINFO tInfo = CUI_Manager::Get_Instance()->Get_SkillInfo(WP_BOW, m_iSkillNum);
	CSkill::SKILL_DESC Desc{};
	Desc.fDepth = m_fDepth - 0.01f;
	Desc.isScreen = false;
	Desc.tSkillInfo = tInfo;
	Desc.vPosition = m_pBoxBow->Get_Position();
	Desc.vSize = _vec2(50.f, 50.f);
	m_pSkillBow = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Skill"), &Desc);
	if (not m_pSkillBow)
	{
		return E_FAIL;
	}

	tInfo = CUI_Manager::Get_Instance()->Get_SkillInfo(WP_SWORD, m_iSkillNum);
	Desc.tSkillInfo = tInfo;
	Desc.vPosition = m_pBoxSword->Get_Position();
	Desc.vSize = _vec2(50.f, 50.f);
	m_pSkillSword = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Skill"), &Desc);
	if (not m_pSkillSword)
	{
		return E_FAIL;
	}
	/* 나중에 주석 풀어야 할 부분
	if (FAILED(CUI_Manager::Get_Instance()->Unlock_Skill(m_iSkillNum)))
	{
		return E_FAIL;
	}
	*/

	return S_OK;
}

void CPop_Skill::Tick(_float fTimeDelta)
{
	
	if (m_fDeadTime >= 4.f)
	{
		if (m_iSkillNum == 0)
		{
			if (CEvent_Manager::Get_Instance()->Get_TutorialLevel() == T_EXIT)
			{
				CEvent_Manager::Get_Instance()->Set_TutorialSeq(T_OPENSKILL);
			}
		}
		m_isDead = true;
	}

	if (m_fTime > 0)
	{
		m_fTime += fTimeDelta *1500.f * m_fDirEffect;
	}

	m_fDeadTime += fTimeDelta * 2.f;

	if (m_isSkillIn)
	{
		if (m_pBackground->Get_Size().x <= 220.f)
		{
			m_pBackground->Set_Size(m_pBackground->Get_Size().x + fTimeDelta * 700.f, m_pBackground->Get_Size().y);
		}
		if (m_pBoxBow->Get_TransPosition().x > m_fX - 30.f)
		{
			dynamic_cast<CSkill*>(m_pSkillBow)->Set_Position(_vec2(m_pBoxBow->Get_TransPosition().x - fTimeDelta * 100.f, 100.f));
			dynamic_cast<CSkill*>(m_pSkillSword)->Set_Position(_vec2(m_pBoxSword->Get_TransPosition().x + fTimeDelta * 100.f, 100.f));
			m_pBoxBow->Set_Position(_vec2(m_pBoxBow->Get_TransPosition().x - fTimeDelta * 100.f, m_pBoxBow->Get_Position().y));
			m_pBoxSword->Set_Position(_vec2(m_pBoxSword->Get_TransPosition().x + fTimeDelta * 100.f, m_pBoxSword->Get_Position().y));
		}
		m_pBoxBow->Tick(fTimeDelta);
		m_pBoxSword->Tick(fTimeDelta);
		m_pSkillBow->Tick(fTimeDelta);
		m_pSkillSword->Tick(fTimeDelta);
	}

	if (m_fDeadTime <= 0.8f)
	{
		m_pBackground->Set_Alpha(m_fDeadTime);
	}


	m_pBorder->Tick(fTimeDelta);
	m_pLeft->Tick(fTimeDelta);
	m_pRight->Tick(fTimeDelta);

	m_pBackground->Tick(fTimeDelta);
//	m_pTransformCom->Rotation(_vec4(0.f, 0.f, 1.f, 0.f), m_fTime / 16.f);
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CPop_Skill::Late_Tick(_float fTimeDelta)
{
	if (m_isSkillIn)
	{
		m_pSkillBow->Late_Tick(fTimeDelta);
		m_pSkillSword->Late_Tick(fTimeDelta);
		m_pBoxBow->Late_Tick(fTimeDelta);
		m_pBoxSword->Late_Tick(fTimeDelta);
	}

	m_pBorder->Late_Tick(fTimeDelta);
	m_pLeft->Late_Tick(fTimeDelta);
	m_pRight->Late_Tick(fTimeDelta);

	m_pBackground->Late_Tick(fTimeDelta);
	if (m_fSizeX >= 300.f)
	{
		m_fDirEffect = -1.f;
	}
	if (m_fDirEffect == -1.f)
	{
		if (m_pLeft->Get_TransPosition().x > m_fX - 90.f)
		{
			m_pLeft->Set_Position(_vec2(m_pLeft->Get_TransPosition().x - fTimeDelta * 300.f, m_pLeft->Get_Position().y));
			m_pRight->Set_Position(_vec2(m_pRight->Get_TransPosition().x + fTimeDelta * 300.f, m_pRight->Get_Position().y));
		}
		if (m_pLeft->Get_TransPosition().x < m_fX - 70.f && !m_isSkillIn)
		{
			m_isSkillIn = true;
		}
	}
	m_fSizeX = m_fTime;
	m_fSizeY = m_fTime;
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CPop_Skill::Render()
{

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Mask_Texture)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("스킬 개방!"), _vec2((_float)g_iWinSizeX / 2.f - 0.2f, 60.f), 0.35f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("스킬 개방!"), _vec2((_float)g_iWinSizeX / 2.f + 0.2f, 60.f), 0.35f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("스킬 개방!"), _vec2((_float)g_iWinSizeX / 2.f, 60.f - 0.2f), 0.35f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("스킬 개방!"), _vec2((_float)g_iWinSizeX / 2.f, 60.f + 0.2f), 0.35f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("스킬 개방!"), _vec2((_float)g_iWinSizeX / 2.f, 60.f), 0.35f, _vec4(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CPop_Skill::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FX_A_Shine014_Tex"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FX_A_Shine014_Tex"), TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPop_Skill::Add_Parts()
{
	//FX_A_Shine014_Tex
	CTextButton::TEXTBUTTON_DESC ButtonDesc = {};
	ButtonDesc.eLevelID = LEVEL_STATIC;
	ButtonDesc.strText = TEXT("");
	ButtonDesc.fDepth = m_fDepth - 0.03f;
	
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_triangle1");
	ButtonDesc.vPosition = _vec2(m_fX - 15.f, 100.f);
	ButtonDesc.vSize = _vec2(30.f, 30.f);

	m_pLeft = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pLeft)
	{
		return E_FAIL;
	}
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_triangle2");
	ButtonDesc.vPosition = _vec2(m_fX + 15.f, 100.f);
	ButtonDesc.vSize = _vec2(30.f, 30.f);

	m_pRight = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pRight)
	{
		return E_FAIL;
	}
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_bluerect");
	ButtonDesc.vPosition = _vec2(m_fX - 20.f, 100.f);
	ButtonDesc.vSize = _vec2(78.f, 78.f);

	m_pBoxBow = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pBoxBow)
	{
		return E_FAIL;
	}
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_bluerect");
	ButtonDesc.vPosition = _vec2(m_fX + 20.f, 100.f);
	ButtonDesc.vSize = _vec2(78.f, 78.f);

	m_pBoxSword = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pBoxSword)
	{
		return E_FAIL;
	}


	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_SiegeQuest");
	ButtonDesc.vPosition = _vec2((_float)g_iWinSizeX / 2.f, 135.f);
	ButtonDesc.vSize = _vec2(360.f, 8.f);
	m_pBorder = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pBorder)
	{
		return E_FAIL;
	}

	CTextButtonColor::TEXTBUTTON_DESC TextButton = {};
	TextButton.eLevelID = LEVEL_STATIC;
	TextButton.strTexture = TEXT("");
	TextButton.fDepth = m_fDepth + 0.01f;
	TextButton.fAlpha = 0.f;
	TextButton.fFontSize = 0.35f;
	TextButton.vColor = _vec4(0.f, 0.8f, 1.f, 1.f);
	//.vColor = _vec4(0.f, 0.f, 0.f, 1.f);
	TextButton.vTextColor = _vec4(0.8f, 0.8f, 1.f, 1.f);
	TextButton.strText = TEXT("");
	TextButton.vPosition = _vec2(m_fX, 38.f);
	TextButton.vSize = _vec2(350.f, 200.f);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_AlertMask");
	//TextButton.strTexture2 = TEXT("Prototype_Component_Texture_UI_Gameplay_GradBlue");
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pBackground = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pBackground)
	{
		return E_FAIL;
	}
	m_pBackground->Set_Pass(VTPass_Mask_Color);

	TextButton.fDepth = m_fDepth - 0.01f;
	TextButton.fAlpha = 0.f;
	TextButton.fFontSize = 0.35f;
	TextButton.vColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.vPosition = _vec2(m_fX, 100.f);
	TextButton.vSize = _vec2(200.f, 100.f);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Noise_08");
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pParticle = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pParticle)
	{
		return E_FAIL;
	}
	m_pParticle->Set_Pass(VTPass_MaskColorMove);

	return S_OK;
}

HRESULT CPop_Skill::Bind_ShaderResources()
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

	/*
	
	_int2 vNumSprite = { 6, 6 };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vNumSprite", &vNumSprite, sizeof(_int2))))
	{
		return E_FAIL;
	}
	int iIndex = static_cast<_int>(m_fIndex);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
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

CPop_Skill* CPop_Skill::Create(_dev pDevice, _context pContext)
{
	CPop_Skill* pInstance = new CPop_Skill(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CPop_Skill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPop_Skill::Clone(void* pArg)
{
	CPop_Skill* pInstance = new CPop_Skill(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPop_Skill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPop_Skill::Free()
{
	__super::Free();

	Safe_Release(m_pParticle);
	Safe_Release(m_pBorder);
	Safe_Release(m_pSkillBow);
	Safe_Release(m_pSkillSword);
	Safe_Release(m_pLeft);
	Safe_Release(m_pRight);
	Safe_Release(m_pBoxBow);
	Safe_Release(m_pBoxSword);

	Safe_Release(m_pBackground);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
