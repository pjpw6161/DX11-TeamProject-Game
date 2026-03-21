#include "Loading_Horse.h"
#include "GameInstance.h"

CLoading_Horse::CLoading_Horse(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CLoading_Horse::CLoading_Horse(const CLoading_Horse& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CLoading_Horse::Init_Prototype()
{
	return S_OK;
}

HRESULT CLoading_Horse::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}


	m_fSizeX = 100.f;
	m_fSizeY = 60.f;

	m_fX = 1180.f;
	m_fY = 620.f;

	m_fDepth = (_float)D_LOADING / (_float)D_END - 0.01f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CLoading_Horse::Tick(_float fTimeDelta)
{

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	m_fIndex += 32.f * fTimeDelta * 2.f;
	if (m_fIndex >= 32.f)
		m_fIndex = 0.f;
}

void CLoading_Horse::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CLoading_Horse::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_SpriteMaskColor)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}
	m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("Loading..."), _vec2(m_fX, m_fY + 50.f), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CLoading_Horse::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_LH_Loading"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading_Horse::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	_int2 vNumSprite = { 4, 8 };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vNumSprite", &vNumSprite, sizeof(_int2))))
	{
		return E_FAIL;
	}
	int iIndex = static_cast<_int>(m_fIndex);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
	{
		return E_FAIL;
	}
	
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}
	
	_vec4 vColor{ 1.f, 1.f, 1.f, 1.f, };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof _vec4)))
	{
		return E_FAIL;
	}
	

	return S_OK;
}

CLoading_Horse* CLoading_Horse::Create(_dev pDevice, _context pContext)
{
	CLoading_Horse* pInstance = new CLoading_Horse(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLoading_Horse");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoading_Horse::Clone(void* pArg)
{
	CLoading_Horse* pInstance = new CLoading_Horse(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLoading_Horse");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading_Horse::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
