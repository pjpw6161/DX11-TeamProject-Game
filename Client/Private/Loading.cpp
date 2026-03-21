#include "Loading.h"
#include "GameInstance.h"

CLoading::CLoading(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CLoading::CLoading(const CLoading& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CLoading::Init_Prototype()
{
	return S_OK;
}

HRESULT CLoading::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pHorse = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Loading_Horse"));
	if (not m_pHorse)
	{
		return E_FAIL;
	}

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_fDepth = (_float)D_LOADING / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CLoading::Tick(_float fTimeDelta)
{
	m_fIndex += fTimeDelta * 0.2f;
	if (m_fIndex >= 8.f)
		m_fIndex = 0.f;

	m_pHorse->Tick(fTimeDelta);
}

void CLoading::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
	m_pHorse->Late_Tick(fTimeDelta);
}

HRESULT CLoading::Render()
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

	/*
	m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("~!~!~!설명"), _vec2(200.f, 520.f), 0.5f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("~!~!~!설명"), _vec2(200.f, 520.f), 0.5f, _vec4(1.f, 1.f, 1.f, 1.f));
	*/

	return S_OK;
}

HRESULT CLoading::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Bg_Dungeon"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading::Bind_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", (_uint)m_fIndex)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLoading* CLoading::Create(_dev pDevice, _context pContext)
{
	CLoading* pInstance = new CLoading(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLoading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoading::Clone(void* pArg)
{
	CLoading* pInstance = new CLoading(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLoading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading::Free()
{
	__super::Free();

	Safe_Release(m_pHorse);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
