#include "Tutorial.h"
#include "GameInstance.h"
#include "Event_Manager.h"
#include "TextButtonColor.h"
#include "UI_Manager.h"

CTutorial::CTutorial(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CTutorial::CTutorial(const CTutorial& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CTutorial::Init_Prototype()
{
	return S_OK;
}

HRESULT CTutorial::Init(void* pArg)
{
	m_eTuto = ((TUTO_DESC*)pArg)->eTuto;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = (_float)g_iWinSizeX;
	m_fSizeY = (_float)g_iWinSizeY;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_fDepth = (_float)D_FADE / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	//CUI_Manager::Get_Instance()->Set_TimeStop(true);
	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CTutorial::Tick(_float fTimeDelta)
{
	m_isDead = true;
	return;

	fTimeDelta /= m_pGameInstance->Get_TimeRatio();
	if (CEvent_Manager::Get_Instance()->Get_TutorialComplete(m_eTuto))
	{
		CUI_Manager::Get_Instance()->Set_TimeStop(false);
		m_pGameInstance->Set_TimeRatio(1.f);
		m_isDead = true;
	}	
	if (m_eTuto == T_OPENINVEN || m_eTuto == T_OPENSKILL)
	{
		m_pGameInstance->Set_TimeRatio(0.001f);

	}



	if (m_pPoint != nullptr)
	{
		if (m_fTime > 1.f || m_fTime < 0.4f)
		{
			m_fDir *= -1.f;

		}

		m_fTime += fTimeDelta * m_fDir;

		m_pPoint->Set_Alpha(m_fTime);
		m_pPoint->Tick(fTimeDelta);
	}
}

void CTutorial::Late_Tick(_float fTimeDelta)
{
	if (m_pPoint != nullptr)
	{
		m_pPoint->Late_Tick(fTimeDelta);
	}
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CTutorial::Render()
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

	switch (m_eTuto)
	{
	case T_OPENINVEN:
	{
		m_pGameInstance->Render_Text(L"Font_Malang",TEXT("인벤토리에서 무기를 장착할 수 있습니다."), _vec2(1140.f, 170.f), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f));
	}
		break;
	case T_EQUIP:
	{
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("더블클릭하여 기본 무기를 장착합니다."), _vec2(1000.f, 300.f), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f));
	}
		break;
	case T_EXIT:
	{
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("이제 곧 전투가 시작됩니다."), _vec2(1150.f, 120.f), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("F키"), _vec2(1065.f, 140.f), 0.4f, _vec4(1.f, 0.8f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("를 눌러 몬스터를 공격하세요!"), _vec2(1165.f, 140.f), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f));
	}
		break;
	case T_OPENSKILL:
	{
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("스킬북에서 획득한 스킬을 확인할 수 있습니다."), _vec2(1120.f, 170.f), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f));
	}
		break;
	case T_EQUIPSKILL:
	{
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("잠금해제된 스킬을 눌러 장착할 수 있습니다."), _vec2(700.f, 160.f), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f));
	}
		break;
	case T_SKILLEXIT:
	{
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("스킬을 사용하여 몬스터를 공격하세요!"), _vec2(1150.f, 120.f), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f));
	}
		break;
	}


	return S_OK;
}

HRESULT CTutorial::Add_Parts()
{
	CTextButtonColor::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.strText = TEXT("");
	
	Button.fAlpha = 1.f;


	switch (m_eTuto)
	{
	case T_OPENINVEN:
		Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BloomCircle");
		Button.vPosition = _vec2(1157.f, 42.f);
		Button.vSize = _vec2(90.f, 90.f);
		break;
	case T_EQUIP:
		Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BloomRect");
		Button.vPosition = _vec2(928.f, 147.f);
		Button.vSize = _vec2(120.f, 120.f);
		break;
	case T_EXIT:
		Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BloomRect");
		Button.vPosition = _vec2(1225.f, 30.f);
		Button.vSize = _vec2(75.f, 75.f);
		break;
	case T_OPENSKILL:
		Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BloomCircle");
		Button.vPosition = _vec2(1100.f, 42.f);
		Button.vSize = _vec2(90.f, 90.f);
		break;
	case T_EQUIPSKILL:
		Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_TutoBoxBigpng");
		Button.vPosition = _vec2(_vec2(1090.f, (_float)g_iWinSizeY / 2.f + 25.f));
		Button.vSize = _vec2(680.f, 680.f);
		break;
	case T_SKILLEXIT:
		Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BloomRect");
		Button.vPosition = _vec2(1225.f, 30.f);
		Button.vSize = _vec2(75.f, 75.f);
		break;
	}	
	m_pPoint = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &Button);
	if (not m_pPoint)
	{
		return E_FAIL;
	}
	m_pPoint->Set_Pass(VTPass_UI_Alpha);

	return S_OK;
}

HRESULT CTutorial::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Tuto"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTutorial::Bind_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", (_uint)m_eTuto)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CTutorial* CTutorial::Create(_dev pDevice, _context pContext)
{
	CTutorial* pInstance = new CTutorial(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCTutorial");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTutorial::Clone(void* pArg)
{
	CTutorial* pInstance = new CTutorial(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTutorial");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTutorial::Free()
{
	__super::Free();

	Safe_Release(m_pPoint);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
