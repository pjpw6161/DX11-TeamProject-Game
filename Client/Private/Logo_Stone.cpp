#include "Logo_Stone.h"
#include "GameInstance.h"

CLogo_Stone::CLogo_Stone(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CLogo_Stone::CLogo_Stone(const CLogo_Stone& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CLogo_Stone::Init_Prototype()
{
	return S_OK;
}

HRESULT CLogo_Stone::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	m_fX = g_iWinSizeX >> 1;
	m_fY = (g_iWinSizeY >> 1);

	m_fDepth = 0.9f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CLogo_Stone::Tick(_float fTimeDelta)
{


	//m_fU -= 0.2f * fTimeDelta;
	//if (m_fV >= 1.f)
		//m_fV = 0.f;
	m_fV += 0.1f * fTimeDelta;
}

void CLogo_Stone::Late_Tick(_float fTimeDelta)
{

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CLogo_Stone::Render()
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
	
	
	_vec2 vUV{ m_fU + 0.2f, m_fV * 0.2f };
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
	

	vUV = { m_fU + 0.4f, m_fV * 0.4f };
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

HRESULT CLogo_Stone::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_Logo_Stone"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLogo_Stone::Bind_ShaderResources()
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

CLogo_Stone* CLogo_Stone::Create(_dev pDevice, _context pContext)
{
	CLogo_Stone* pInstance = new CLogo_Stone(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLogo_Stone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLogo_Stone::Clone(void* pArg)
{
	CLogo_Stone* pInstance = new CLogo_Stone(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLogo_Stone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogo_Stone::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
