#include "Dummy.h"

static _int iID = 0;

CDummy::CDummy(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CDummy::CDummy(const CDummy& rhs)
	: CGameObject(rhs)
	//, m_pImGui_Manager(CImGui_Manager::Get_Instance())
{
	m_iID = iID++;
	//Safe_AddRef(m_pImGui_Manager);
}

void CDummy::Select(const _bool& isSelected)
{
	m_isSelected = isSelected;
}


void CDummy::Get_State(_float4& vPos, _float4& vLook)
{
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(State::Pos));
	XMStoreFloat4(&vLook, m_pTransformCom->Get_State(State::Look));
}


_uint CDummy::Get_AnimIndex()
{
	return m_pModelCom->Get_NumAnim();
}

HRESULT CDummy::Init_Prototype()
{
	return S_OK;
}

HRESULT CDummy::Init(void* pArg)
{

	m_Info = *(DummyInfo*)pArg;
	m_eType = m_Info.eType;
	if(m_Info.eType == ItemType::Monster || m_Info.eType == ItemType::NPC)
	{
		m_isAnim = true;
		m_Animation.isLoop = true;
	}
	else if (m_Info.eType == ItemType::Trigger)
	{
		m_iTrigger = m_Info.iTriggerNum;
		m_fTriggerSize = m_Info.fTriggerSize;

		m_iShaderPass = StaticPass_Default;

	}
	else
	{
		m_iShaderPass = StaticPass_AlphaTestMeshes;
	}
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (m_Info.ppDummy)
	{
		*m_Info.ppDummy = this;
		m_Info.ppDummy = nullptr;
	}


	m_pTransformCom->Set_Matrix(m_Info.mMatrix);
	
	m_pModelCom->Apply_TransformToActor(m_pTransformCom->Get_World_Matrix());

	return S_OK;
}

void CDummy::Tick(_float fTimeDelta)
{
	if (m_eType == ItemType::Trigger)
	{
		m_pCollider->Update(m_pTransformCom->Get_World_Matrix());
	}
}

void CDummy::Late_Tick(_float fTimeDelta)
{
	if (m_eType == ItemType::Monster || m_eType == ItemType::NPC)
		m_pModelCom->Play_Animation(fTimeDelta);

	#ifdef _DEBUG
	if(m_eType == ItemType::Trigger)
		m_pRendererCom->Add_DebugComponent(m_pCollider);
	#endif

	if (m_eType == ItemType::Environment)
	{
		if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos)))
		{
			m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend_Instance, this);
		}	
	}
	else
		m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);

}

HRESULT CDummy::Render()
{
	
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			continue;
		}

		_bool HasNorTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bSelected", &m_isSelected, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (m_isAnim)
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			{
				return E_FAIL;
			}
		}


		if (FAILED(m_pShaderCom->Begin(m_iOutLineShaderPass)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}
		
	}
	
	

	return S_OK;
}

HRESULT CDummy::Render_Instance()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CDummy::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}
	if (m_isAnim)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		{
			return E_FAIL;
		}
		m_iShaderPass = AnimPass_Default;
		m_iOutLineShaderPass = AnimPass_OutLine;
	}
	else
	{
		if (m_eType == ItemType::Environment)
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh_Instance"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			{
				return E_FAIL;
			}
			m_pShaderCom->Set_PassIndex(3);

		}
		else
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			{
				return E_FAIL;
			}
		}
		m_iShaderPass = StaticPass_Default;
		m_iOutLineShaderPass = StaticPass_OutLine;
	}
	
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, m_Info.Prototype, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		{
			return E_FAIL;
		}
	}

	if (m_eType == ItemType::Trigger)
	{
		/* For.Com_Collider_SPHERE */
	// Com_Collider
		Collider_Desc CollDesc = {};
		CollDesc.eType = ColliderType::Sphere;
		CollDesc.fRadius = m_fTriggerSize;
		CollDesc.vCenter = _vec3(0.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Trigger_Sphere"), (CComponent**)&m_pCollider, &CollDesc)))
			return E_FAIL;
	}



	return S_OK;
}

HRESULT CDummy::Bind_ShaderResources()
{
	if (m_eType != ItemType::Environment)
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


		if (m_Info.eType == ItemType::Trigger)
		{
			_float4 vColor{ 0.3f, 0.8f, 0.3f, 0.5f };
			if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof _float4)))
			{
				return E_FAIL;
			}

			const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(LEVEL_EDITOR, TEXT("Light_Main"));
			if (!pLightDesc)
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof _float4)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof _float4)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof _float4)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof _float4)))
			{
				return E_FAIL;
			}
		}

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

CDummy* CDummy::Create(_dev pDevice, _context pContext)
{
	CDummy* pInstance = new CDummy(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CDummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy::Clone(void* pArg)
{
	CDummy* pInstance = new CDummy(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CDummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy::Free()
{
	__super::Free();

	//if(m_pImGui_Manager)
	//Safe_Release(m_pImGui_Manager);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	//Safe_Release(m_pVIBuffer);

	if(m_eType == ItemType::Trigger)
		Safe_Release(m_pCollider);
}
