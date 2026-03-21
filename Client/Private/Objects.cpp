#include "Objects.h"
#include "Camera_Manager.h"
#include "Trigger_Manager.h"
CObjects::CObjects(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CObjects::CObjects(const CObjects& rhs)
	: CBlendObject(rhs)
	, m_Info(rhs.m_Info)
	, m_isInstancing(rhs.m_isInstancing)
{
}


HRESULT CObjects::Init_Prototype()
{
	return S_OK;
}

HRESULT CObjects::Init(void* pArg)
{

	return S_OK;
}

void CObjects::Tick(_float fTimeDelta)
{

	m_pTransformCom->Set_OldMatrix();
}

void CObjects::Late_Tick(_float fTimeDelta)
{
	if (CTrigger_Manager::Get_Instance()->Get_CurrentSpot() != TS_Village)
	{
		return;
	}

	CAMERA_STATE CamState = CCamera_Manager::Get_Instance()->Get_CameraState();
	if (CamState == CS_SKILLBOOK or CamState == CS_INVEN) //or CamState == CS_WORLDMAP)
	{
		return;
	}

	// ÀÎ½ºÅÏ½Ì ÇÒ ¸ðµ¨°ú ¾ÈÇÒ ¸ðµ¨À» ³ª´²ÁÜ
	if (m_isInstancing == false)
	{
		//if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos)))
		//{
			m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);

			if(true == m_pGameInstance->Get_TurnOnShadow())
				m_pRendererCom->Add_RenderGroup(RenderGroup::RG_Shadow, this);
		//}
	}
	else
	{
		if(m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY)
			//if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos)))
			//{
			m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend_Instance, this);
		//}
		else
		{
			if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos), 20.f))
			{
				m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend_Instance, this);
			}

		}
	}

}

HRESULT CObjects::Render()
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
			return E_FAIL;
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

		_bool HasMaskTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", i, TextureType::Shininess)))
		{
			HasMaskTex = false;
		}
		else
		{
			HasMaskTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
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

HRESULT CObjects::Render_Shadow()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	CASCADE_DESC Desc = m_pGameInstance->Get_CascadeDesc();

	if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeView", Desc.LightView, 3)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeProj", Desc.LightProj, 3)))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			continue;
		}

		if (FAILED(m_pShaderCom->Begin(StaticPass_Shadow)))
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

HRESULT CObjects::Render_Instance()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (true == m_pGameInstance->Get_TurnOnShadow()) {

		CASCADE_DESC Desc = m_pGameInstance->Get_CascadeDesc();

		if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeView", Desc.LightView, 3)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeProj", Desc.LightProj, 3)))
			return E_FAIL;

	}

	return S_OK;
}


HRESULT CObjects::Render_Reflection(_float4 vClipPlane)
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vClipPlane", &vClipPlane, sizeof(_float4))))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			return E_FAIL;
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

		_bool HasMaskTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", i, TextureType::Shininess)))
		{
			HasMaskTex = false;
		}
		else
		{
			HasMaskTex = true;
		}


		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(StaticPass_Water)))
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

HRESULT CObjects::Add_Components(wstring strPrototype, ObjectType eType )
{
 	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (m_isInstancing == false)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh_Instance"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		{
			return E_FAIL;
		}
	}

	m_pShaderCom->Set_PassIndex(3);
	m_iShaderPass = StaticPass_Default;
	m_iOutLineShaderPass = StaticPass_OutLine;

	if (eType == Object_Environment)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, strPrototype, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), strPrototype, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CObjects::Bind_ShaderResources()
{
	if (m_isInstancing == false)
	{
		if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pTransformCom->Bind_OldWorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	return S_OK;
}


void CObjects::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
