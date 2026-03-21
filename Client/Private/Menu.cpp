#include "Menu.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"
#include "FadeBox.h"

CMenu::CMenu(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CMenu::CMenu(const CMenu& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CMenu::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CMenu::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	
	m_fSizeX = 70.f;
	m_fSizeY = 70.f;

	m_fX = 1220.f;
	m_fY = 44.f;


	m_fDepth = (_float)D_MENU / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CMenu::Tick(_float fTimeDelta)
{

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	RECT rectUI = {
			  (LONG)(m_fX - m_fSizeX * 0.5f),
			  (LONG)(m_fY - m_fSizeY * 0.5f),
			  (LONG)(m_fX + m_fSizeX * 0.5f),
			  (LONG)(m_fY + m_fSizeY * 0.5f)
	};
	if (TRUE == PtInRect(&rectUI, ptMouse))
	{
		if (!m_isActive)
		{
			m_isActive = true;
		}
		if (m_isActive)
		{
			m_isActive = false;
		}
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);



}

void CMenu::Late_Tick(_float fTimeDelta)
{

	if (m_isActive)
	{

	}


	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CMenu::Render()
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


	return S_OK;
}


HRESULT CMenu::Add_Parts()
{
	/*
	
	CTextButton::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.03f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Notify");
	Button.vPosition = _vec2(m_fX + 7.f, m_fY - 14.f);
	Button.vSize = _vec2(40.f, 40.f);

	m_pNotify = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pNotify)
	{
		return E_FAIL;
	}
	*/

	return S_OK;
}

HRESULT CMenu::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_MenuBar"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMenu::Bind_ShaderResources()
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

CMenu* CMenu::Create(_dev pDevice, _context pContext)
{
	CMenu* pInstance = new CMenu(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMenu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMenu::Clone(void* pArg)
{
	CMenu* pInstance = new CMenu(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CMenu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMenu::Free()
{
	__super::Free();

	Safe_Release(m_pExitGame);
	Safe_Release(m_pSetting);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}