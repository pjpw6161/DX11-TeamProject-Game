#include "SelectDesc.h"
#include "GameInstance.h"

CSelectDesc::CSelectDesc(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CSelectDesc::CSelectDesc(const CSelectDesc& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CSelectDesc::Init_Prototype()
{
	return S_OK;
}

HRESULT CSelectDesc::Init(void* pArg)
{


	m_eCharacter = ((SELECT_DESC*)pArg)->eCharacter;
	switch (m_eCharacter)
	{
	case WARRIOR:
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Select_Warrior");
		break;
	case SORCERESS:
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Select_Soceress");
		break;
	case ROGUE:
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Select_Rogue");
		break;
	case PRIEST:
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Select_Priest");
		break;
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	
	m_fSizeX = 513.f;
	m_fSizeY = 513.f;

	m_fX = m_fSizeX / 2.f - 10.f;
	m_fY = m_fSizeX / 2.f + 20.f;

	m_fDepth = 0.5f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CSelectDesc::Tick(_float fTimeDelta)
{
	if (m_fX < m_fSizeX / 2.f)
	{
		m_fX += fTimeDelta * 50.f;
		__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	}

	m_fAlpha += fTimeDelta * 2.f;

}

void CSelectDesc::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CSelectDesc::Render()
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

HRESULT CSelectDesc::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_SELECT, m_strTexture, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSelectDesc::Bind_ShaderResources()
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
	_float fAlpha = Lerp(0.f, 1.f, m_fAlpha);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	return S_OK;
}

CSelectDesc* CSelectDesc::Create(_dev pDevice, _context pContext)
{
	CSelectDesc* pInstance = new CSelectDesc(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSelectDesc");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSelectDesc::Clone(void* pArg)
{
	CSelectDesc* pInstance = new CSelectDesc(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSelectDesc");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSelectDesc::Free()
{
	__super::Free();


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}
