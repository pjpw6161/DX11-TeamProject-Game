#include "Scarecrow.h"
#include "Effect_Manager.h"
#include "Effect_Dummy.h"
#include "Camera_Manager.h"

CScarecrow::CScarecrow(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CScarecrow::CScarecrow(const CScarecrow& rhs)
	: CGameObject(rhs)
{
}

HRESULT CScarecrow::Init_Prototype()
{
	return S_OK;
}

HRESULT CScarecrow::Init(void* pArg)
{

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, _vec4(0.f,300.f,4.7f,1.f));
	m_pTransformCom->LookAt(_vec4(0.f,300.f,0.f,1.f));
	m_pGameInstance->Register_CollisionObject(this, m_pHitCollider);
	m_pHitCollider->Update(m_pTransformCom->Get_World_Matrix());
	return S_OK;
}

void CScarecrow::Tick(_float fTimeDelta)
{
	if (CCamera_Manager::Get_Instance()->Get_CameraState() != CS_SKILLBOOK)
	{
		return;
	}
	m_pTransformCom->Set_State(State::Pos, _vec4(0.f, 300.f, 4.7f, 1.f));
	m_pTransformCom->LookAt(_vec4(0.f, 300.f, 0.f, 1.f));
}

void CScarecrow::Late_Tick(_float fTimeDelta)
{
	if (CCamera_Manager::Get_Instance()->Get_CameraState() != CS_SKILLBOOK)
	{
		return;
	}

	//m_pRendererCom->Add_DebugComponent(m_pHitCollider);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CScarecrow::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
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

		if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
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

HRESULT CScarecrow::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Scarecrow"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::OBB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(0.4f, 1.f, 0.4f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.6f, -0.6f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Player_Hit_OBB"), (CComponent**)&m_pHitCollider, &CollDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CScarecrow::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CScarecrow* CScarecrow::Create(_dev pDevice, _context pContext)
{
	CScarecrow* pInstance = new CScarecrow(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CScarecrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CScarecrow::Clone(void* pArg)
{
	CScarecrow* pInstance = new CScarecrow(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CScarecrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScarecrow::Free()
{

	__super::Free();

	//m_pGameInstance->Delete_CollisionObject(this);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pHitCollider);
}
