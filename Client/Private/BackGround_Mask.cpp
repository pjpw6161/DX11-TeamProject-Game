#include "BackGround_Mask.h"
#include "GameInstance.h"

CBackGround_Mask::CBackGround_Mask(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CBackGround_Mask::CBackGround_Mask(const CBackGround_Mask& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CBackGround_Mask::Init_Prototype()
{
	return S_OK;
}

HRESULT CBackGround_Mask::Init(void* pArg)
{
	UiInfo* Ui_Info = (UiInfo*)pArg;
	
	m_strTexture = Ui_Info->strTexture;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}


	m_fSizeX = Ui_Info->vSize.x;
	m_fSizeY = Ui_Info->vSize.y;

	m_fX = Ui_Info->vPos.x;
	m_fY = Ui_Info->vPos.y;
	
	m_eLevel = (LEVEL_ID)Ui_Info->vPos.y;
	m_fDepth = 1.1f;
	return S_OK;
}

void CBackGround_Mask::Tick(_float fTimeDelta)
{
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	m_fSpeed += 0.0003f;
}

void CBackGround_Mask::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_Priority, this);

}

HRESULT CBackGround_Mask::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (m_strTexture == TEXT("Prototype_Component_Texture_Skill_Background"))
	{
		if (FAILED(m_pShaderCom->Begin(VTPass_Background)))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(m_pShaderCom->Begin(VTPass_Background_Mask)))
		{
			return E_FAIL;
		}


	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CBackGround_Mask::Add_Components()
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

	if (m_strTexture != TEXT(""))
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strTexture, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CBackGround_Mask::Bind_ShaderResources()
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
	if (m_strTexture != TEXT(""))
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		{
			return E_FAIL;
		}
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fx", &m_fSpeed, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CBackGround_Mask* CBackGround_Mask::Create(_dev pDevice, _context pContext)
{
	CBackGround_Mask* pInstance = new CBackGround_Mask(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBackGround_Mask");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackGround_Mask::Clone(void* pArg)
{
	CBackGround_Mask* pInstance = new CBackGround_Mask(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBackGround_Mask");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround_Mask::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
