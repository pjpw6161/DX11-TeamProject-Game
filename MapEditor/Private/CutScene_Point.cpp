#include "CutScene_Point.h"

CCutScene_Point::CCutScene_Point(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CCutScene_Point::CCutScene_Point(const CCutScene_Point& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCutScene_Point::Init_Prototype()
{
	return S_OK;
}

HRESULT CCutScene_Point::Init(void* pArg)
{
	_vec4 vPos = *(_vec4*)pArg;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Position(_vec3(vPos));

	return S_OK;
}

void CCutScene_Point::Tick(_float TimeDelta)
{

}

void CCutScene_Point::Late_Tick(_float TimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CCutScene_Point::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			continue;
		}


		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

void CCutScene_Point::Select(const _bool& isSelected)
{
	m_isSelected = isSelected;
}


HRESULT CCutScene_Point::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	m_iShaderPass = StaticPass_Default;
	m_iOutLineShaderPass = StaticPass_OutLine;

		
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Collider"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CCutScene_Point::Bind_ShaderResources()
{
		
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iID", &m_iID, sizeof _int)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
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

	return S_OK;
}


CCutScene_Point* CCutScene_Point::Create(_dev pDevice, _context pContext)
{
	CCutScene_Point* pInstance = new CCutScene_Point(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created CCutScene_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCutScene_Point::Clone(void* pArg)
{
	CCutScene_Point* pInstance = new CCutScene_Point(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned CCutScene_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCutScene_Point::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}