#include "Meteor.h"

#include "Effect_Dummy.h"
#include "Effect_Manager.h"
#include "Camera_Manager.h"

CMeteor::CMeteor(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CMeteor::CMeteor(const CMeteor& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMeteor::Init_Prototype()
{
	return S_OK;
}

HRESULT CMeteor::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_pDragonTransform = GET_TRANSFORM("Layer_Dragon_Boss", LEVEL_VILLAGE);
	_vec3 vDragonPos = m_pDragonTransform->Get_State(State::Pos);

	random_device dev;
	_randNum RandomNumber(dev());
	_randFloat Random = _randFloat(-1.f, 1.f);

	_vec3 vRandomDir = _vec3(Random(RandomNumber), 0.f, Random(RandomNumber)).Get_Normalized();

	m_vTargetPos = vDragonPos + static_cast<_float>(rand() % 5 + 5) * vRandomDir;
	
	_vec3 vDragonRight = m_pDragonTransform->Get_State(State::Right).Get_Normalized();


	m_pTransformCom->Set_Position(m_vTargetPos + vDragonRight * 4.f + _vec3(0.f, 7.f, 0.f));
	m_pTransformCom->Set_Scale(_vec3(2.f, 2.f, 2.f));
	m_pTransformCom->Set_Speed(20.f);

	m_pTransformCom->LookAt(_vec4(m_vTargetPos, 1.f));

	m_MeteorMatrix = _mat::CreateScale(1.5f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)/* + _vec3(0.f, 0.1f, 0.f)*/));
	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Meteor");
	Info.pMatrix = &m_MeteorMatrix;
	Info.isFollow = true;
	m_pMeteor = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	//m_MeteorPartiMatrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)/* + _vec3(0.f, 0.1f, 0.f)*/));
	m_MeteorPartiMatrix = m_pTransformCom->Get_World_Matrix();
	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Meteor_Parti");
	Info.pMatrix = &m_MeteorPartiMatrix;
	Info.isFollow = true;
	m_pMeteorParti = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	//SURFACETRAIL_DESC Desc{};
	//Desc.vColor = Colors::Coral;
	//Desc.iNumVertices = 10;
	//Desc.strMaskTextureTag = TEXT("FX_F_PB5001Trail002_Tex");
	//m_pMeteorTrail = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	return S_OK;
}

void CMeteor::Tick(_float fTimeDelta)
{
	if (m_pTransformCom->Get_State(State::Pos).y <= m_pDragonTransform->Get_State(State::Pos).y)
	{
		Kill();

		if (m_pMeteor)
		{
			Safe_Release(m_pMeteor);
			Safe_Release(m_pMeteorParti);

			_mat EffectMatrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.5f, 0.f)));
			EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Meteor_Shine");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			//EffectMatrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Meteor_Explo_Floor");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			//EffectMatrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Meteor_Explo_Parti2");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
		}
	}

	m_pTransformCom->Go_Straight(fTimeDelta);


	if (m_pMeteor)
	{
		m_MeteorMatrix = _mat::CreateScale(1.5f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)/* + _vec3(0.f, 0.1f, 0.f)*/));
		//m_MeteorPartiMatrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)/* + _vec3(0.f, 0.1f, 0.f)*/));
		m_MeteorPartiMatrix = m_pTransformCom->Get_World_Matrix();

		m_pMeteor->Tick(fTimeDelta);
		m_pMeteorParti->Tick(fTimeDelta);

		//Update_Trail(fTimeDelta);
	}

	Update_Collider();

}

void CMeteor::Late_Tick(_float fTimeDelta)
{
	//m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

	if (m_pMeteor)
	{
		m_pMeteor->Late_Tick(fTimeDelta);
		m_pMeteorParti->Late_Tick(fTimeDelta);

		//m_pMeteorTrail->Late_Tick(fTimeDelta);
	}

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CMeteor::Render()
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

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(StaticPass_Default)))
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

HRESULT CMeteor::Add_Collider()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.vCenter = _vec3(0.f);
	CollDesc.fRadius = 0.5f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Sphere"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

void CMeteor::Update_Collider()
{
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

//void CMeteor::Update_Trail(_float fTimeDelta)
//{
//	//_mat TopMatrix = _mat::CreateTranslation(_vec3(0.f, 0.5f, 0.f)) * m_pTransformCom->Get_World_Matrix();
//	//_mat BottomMatrix = _mat::CreateTranslation(_vec3(0.f, -0.5f, 0.f)) * m_pTransformCom->Get_World_Matrix();
//
//	//m_pMeteorTrail->Tick(TopMatrix.Position_vec3(), BottomMatrix.Position_vec3());
//	//m_pMeteorTrail->Tick(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.5f, 0.f)), _vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, -0.5f, 0.f)));
//	//m_pMeteorTrail->Tick(m_MeteorMatrix.Position_vec3() + _vec3(0.f, 0.5f, 0.f), m_MeteorMatrix.Position_vec3() + _vec3(0.f, -0.5f, 0.f));
//}

HRESULT CMeteor::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Effect_FX_A_Sphere002_SM.mo"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMeteor::Bind_ShaderResources()
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

CMeteor* CMeteor::Create(_dev pDevice, _context pContext)
{
	CMeteor* pInstance = new CMeteor(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMeteor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMeteor::Clone(void* pArg)
{
	CMeteor* pInstance = new CMeteor(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CMeteor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeteor::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pMeteor);
	Safe_Release(m_pMeteorParti);
}
