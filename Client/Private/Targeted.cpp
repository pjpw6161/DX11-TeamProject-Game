#include "Targeted.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "3DUITex.h"

CTargeted::CTargeted(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CTargeted::CTargeted(const CTargeted& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CTargeted::Init_Prototype()
{
	return S_OK;
}

HRESULT CTargeted::Init(void* pArg)
{

	
	m_fDepth = (_float)D_NAMETAG / (_float)D_END;

	m_fSizeX = 150.f;
	m_fSizeY = 150.f;
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}			
	m_fX = -50.f;
	m_fY = -50.f;
	return S_OK;
}

void CTargeted::Tick(_float fTimeDelta)
{
	/*
	
	if (CUI_Manager::Get_Instance()->Is_Targeting())
	{
		_vec4 vTargetPos = CUI_Manager::Get_Instance()->Get_TargetPos();
		m_pTransformCom->Set_State(State::Pos, vTargetPos);

		_vec2 v2DPos = __super::Convert_To_2D(m_pTransformCom);

		m_fSizeX = 500.f;
		m_fSizeY = 500.f;
		m_fX = v2DPos.x;
		m_fY = v2DPos.y;
		m_pTransformCom->Rotation(_vec4(0.f, 0.f, 1.f, 0.f), -2.f);

		__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	}
	*/

}

void CTargeted::Late_Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}
	/*
	
	if (CUI_Manager::Get_Instance()->Is_Targeting())
	{
		m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
	}
	*/
}

HRESULT CTargeted::Render()
{


	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Mask_ColorAlpha)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CTargeted::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FX_A_TargetLine002_Tex"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTargeted::Bind_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	_vec4 vColor = _vec4(1.f, 0.2f, 0.2f, 1.f);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof _vec4)))
	{
		return E_FAIL;
	}

	_float fAlpha = 0.7f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof _float)))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

CTargeted* CTargeted::Create(_dev pDevice, _context pContext)
{
	CTargeted* pInstance = new CTargeted(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTargeted");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTargeted::Clone(void* pArg)
{
	CTargeted* pInstance = new CTargeted(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTargeted");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTargeted::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
