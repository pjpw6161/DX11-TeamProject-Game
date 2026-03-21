#include "Logo_Text.h"
#include "GameInstance.h"

CLogo_Text::CLogo_Text(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CLogo_Text::CLogo_Text(const CLogo_Text& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CLogo_Text::Init_Prototype()
{
	return S_OK;
}

HRESULT CLogo_Text::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 200.f;
	m_fSizeY = 35.f;

	m_fX = g_iWinSizeX >> 1;
	m_fY = (g_iWinSizeY >> 1) + 160.f;

	m_fDepth = 0.2f;


	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CLogo_Text::Tick(_float fTimeDelta)
{

	m_fX = g_iWinSizeX >> 1;
	m_fY = (g_iWinSizeY >> 1) + 300.f;

	m_fDepth = 0.2f;


	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);


	if (m_fAlpha > 0.8f || m_fAlpha < 0.f)
	{
		m_fDir *= -1.f;

	}

	m_fAlpha += fTimeDelta * m_fDir * 0.7f;

}

void CLogo_Text::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CLogo_Text::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_UI_Alpha)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}



	return S_OK;
}

HRESULT CLogo_Text::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_Logo_LogoText"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLogo_Text::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CLogo_Text* CLogo_Text::Create(_dev pDevice, _context pContext)
{
	CLogo_Text* pInstance = new CLogo_Text(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLogo_Text");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLogo_Text::Clone(void* pArg)
{
	CLogo_Text* pInstance = new CLogo_Text(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLogo_Text");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogo_Text::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
