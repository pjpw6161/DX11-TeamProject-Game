#include "Logo_Dust.h"
#include "GameInstance.h"

CLogo_Dust::CLogo_Dust(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CLogo_Dust::CLogo_Dust(const CLogo_Dust& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CLogo_Dust::Init_Prototype()
{
	return S_OK;
}

HRESULT CLogo_Dust::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	m_fX = g_iWinSizeX >> 1;
	m_fY = (g_iWinSizeY >> 1) - 200.f;

	m_fDepth = 0.7f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CLogo_Dust::Tick(_float fTimeDelta)
{


	m_fU -= 0.2f * fTimeDelta;
	if (m_fV >= 1.f)
		m_fV = 0.f;
	m_fV += 0.3f * fTimeDelta;
}

void CLogo_Dust::Late_Tick(_float fTimeDelta)
{

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CLogo_Dust::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Dust)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	_vec2 vUV{ m_fU * 0.5f, m_fV * 0.5f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fx", &vUV.x, sizeof _float)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &vUV.y, sizeof _float)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(VTPass_Dust)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLogo_Dust::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_Logo_Dust"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLogo_Dust::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fx", &m_fU, sizeof _float)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &m_fV, sizeof _float)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CLogo_Dust* CLogo_Dust::Create(_dev pDevice, _context pContext)
{
	CLogo_Dust* pInstance = new CLogo_Dust(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLogo_Dust");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLogo_Dust::Clone(void* pArg)
{
	CLogo_Dust* pInstance = new CLogo_Dust(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLogo_Dust");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogo_Dust::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
