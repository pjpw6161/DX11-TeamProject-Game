#include "Map.h"
#include "Camera_Manager.h"
#include "Trigger_Manager.h"
/*

맵 수정 필요할 시 

미니게임 - Prototype_Model_Minigame
드래곤맵 - Prototype_Model_DragonMap
보스맵 - Prototype_Model_BossMap
미니던전 - Prototype_Model_MiniDungeon
생존맵 - Prototype_Model_Survival_Map

m_Info.Prototype을 가져와서 나눠주면 됩니다.

*/

CMap::CMap(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CMap::CMap(const CMap& rhs)
	: CBlendObject(rhs)
{
}


HRESULT CMap::Init_Prototype()
{
	return S_OK;
}

HRESULT CMap::Init(void* pArg)
{
	m_Info = *(MapInfo*)pArg;
	wstring strPrototype = m_Info.Prototype;
	
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_iShaderPass = 0;
	
	m_pTransformCom->Set_Matrix(m_Info.m_Matrix);
	m_pModelCom->Apply_TransformToActor(m_Info.m_Matrix);
	return S_OK;
}

void CMap::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_OldMatrix();
}

void CMap::Late_Tick(_float fTimeDelta)
{
	/*
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Camera", LEVEL_STATIC);

	_vec4 vPlayerPos = pPlayerTransform->Get_CenterPos();
	_vec4 vPos = m_pTransformCom->Get_CenterPos();

	if (abs(vPlayerPos.x - vPos.x) > 500.f)
	{
		return;
	}
	*/
	TeleportSpot eCurrentSpot = CTrigger_Manager::Get_Instance()->Get_CurrentSpot();
	switch (eCurrentSpot)
	{
	case Client::TS_Dungeon:
		if (m_Info.Prototype != TEXT("Prototype_Model_Dungeon"))
		{
			return;
		}
		break;
	case Client::TS_Village:
		if (m_Info.Prototype != TEXT("Prototype_Model_Map") and m_Info.Prototype != TEXT("Prototype_Model_Tower"))
		{
			return;
		}
		break;
	case Client::TS_Minigame:
		if (m_Info.Prototype != TEXT("Prototype_Model_BrickMap"))
		{
			return;
		}
		break;
	case Client::TS_DragonMap:
		if (m_Info.Prototype != TEXT("Prototype_Model_DragonMap"))
		{
			return;
		}
		break;
	case Client::TS_BossRoom:
		if (m_Info.Prototype != TEXT("Prototype_Model_BossRoom"))
		{
			return;
		}
		break;
	case Client::TS_MiniDungeon:
		if (m_Info.Prototype != TEXT("Prototype_Model_MiniDungeon"))
		{
			return;
		}
		break;
	case Client::TS_SurvivalMap:
		if (m_Info.Prototype != TEXT("Prototype_Model_Survival_Map"))
		{
			return;
		}
		break;
	case Client::TS_SescoMap:
		if (m_Info.Prototype != TEXT("Prototype_Model_SescoMap"))
		{
			return;
		}
		break;
	case Client::TS_END:
		break;
	default:
		break;
	}

	CAMERA_STATE CamState = CCamera_Manager::Get_Instance()->Get_CameraState();
	if (CamState == CS_SKILLBOOK or CamState == CS_INVEN)
	{
		return;
	}

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);

}

HRESULT CMap::Render()
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

HRESULT CMap::Add_Components()
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
	

	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), m_Info.Prototype, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMap::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_OldWorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
		return E_FAIL;

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


CMap* CMap::Create(_dev pDevice, _context pContext)
{
	CMap* pInstance = new CMap(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMap::Clone(void* pArg)
{
	CMap* pInstance = new CMap(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMap::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
