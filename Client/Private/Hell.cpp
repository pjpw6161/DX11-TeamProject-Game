#include "Hell.h"

CHell::CHell(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CHell::CHell(const CHell& rhs)
	: CGameObject(rhs)
{
}

HRESULT CHell::Init_Prototype()
{
	return S_OK;
}

HRESULT CHell::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);
	m_pTransformCom->Set_Scale(_vec3(3.f));

	return S_OK;
}

void CHell::Tick(_float fTimeDelta)
{
}

void CHell::Late_Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&m_pGameInstance->Get_CameraPos()) - XMVectorSet(0.f, 0.15f, 0.f, 0.f));
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_Priority, this);
}

HRESULT CHell::Render()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Hell)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CHell::Add_Components()
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

	return S_OK;
}

HRESULT CHell::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	_float4 vColor{ 0.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof vColor)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CHell* CHell::Create(_dev pDevice, _context pContext)
{
	CHell* pInstance = new CHell(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CHell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHell::Clone(void* pArg)
{
	CHell* pInstance = new CHell(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CHell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHell::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
