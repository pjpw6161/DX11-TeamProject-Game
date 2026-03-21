#include "Arrow.h"
#include "Effect_Manager.h"
#include "Effect_Dummy.h"

CArrow::CArrow(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CArrow::CArrow(const CArrow& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CArrow::Init_Prototype()
{
	return S_OK;
}

HRESULT CArrow::Init(void* pArg)
{
	m_ArrowType = *(Arrow_Type*)pArg;
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	m_iDamage = m_ArrowType.iDamage;

	m_shouldRenderBlur = true;

	if (m_ArrowType.Att_Type != AT_Bow_Skill3)
	{
		wstring strPartiFxTag{ L"CommonArrowParti" };
		if (m_ArrowType.Att_Type == AT_Bow_Skill2)
		{
			strPartiFxTag = L"ExplosiveArrowParti";
			m_shouldRenderBlur = false;
		}

		m_pTransformCom->Set_Matrix(m_ArrowType.world);

		if (m_ArrowType.bAimMode)
		{
			m_pTransformCom->Set_Speed(40.f);
			_vec4 vLook = m_pTransformCom->Get_State(State::Look);
			PxRaycastBuffer Buffer{};
			_vec4 vRayDir = m_pGameInstance->Get_CameraLook().Get_Normalized();
			_vec4 vMyPos = m_pTransformCom->Get_State(State::Pos);
			_float fDist = 200.f;
			if (m_pGameInstance->Raycast(m_pGameInstance->Get_CameraPos(), vRayDir, fDist, Buffer))
			{
				m_vRaycastPos = PxVec3ToVector(Buffer.block.position, 1.f);
			}
			else
			{
				m_ArrowType.bAimMode = false;
			}
		}
		else
		{
			m_pTransformCom->Set_Speed(25.f);
		}
		m_pTransformCom->Set_Scale(_vec3(1.5f));

		EffectInfo EffectInfo = CEffect_Manager::Get_Instance()->Get_EffectInformation(strPartiFxTag);
		EffectInfo.pMatrix = &m_ParticleMatrix;
		EffectInfo.isFollow = true;
		m_pParticle = CEffect_Manager::Get_Instance()->Clone_Effect(EffectInfo);
	}
	else
	{
		_uint random = rand() % 12;
		m_pTransformCom->Set_State(State::Pos, m_ArrowType.vPos);
		m_pTransformCom->Set_Speed(12.f + (_float)random);
		m_pTransformCom->Set_Scale(_vec3(3.f, 3.f, 1.5f));
	}

	m_pTransformCom->LookAt_Dir(m_ArrowType.vLook);

	TRAIL_DESC trail_desc{};
	trail_desc.vColor = _vec4(0.f, 0.6f, 1.f, 1.f);
	if (m_ArrowType.Att_Type == AT_Bow_Skill2)
	{
		trail_desc.vColor = _color(1.f, 0.5f, 0.f);
	}
	trail_desc.vPSize = _vec2(0.02f, 0.02f);
	trail_desc.iNumVertices = 10;
	m_pTrail = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &trail_desc);

	return S_OK;
}

void CArrow::Tick(_float fTimeDelta)
{
	m_fDeadTime += fTimeDelta;
	if (m_fDeadTime > 2.5f)
	{
		Kill();
	}

	if (m_ArrowType.MonCollider != nullptr && !m_ArrowType.bAimMode)
	{
		m_pTransformCom->LookAt(_vec4(m_ArrowType.MonCollider->Get_ColliderPos(), 1.f));
		m_pTransformCom->Go_To(_vec4(m_ArrowType.MonCollider->Get_ColliderPos(), 1.f), fTimeDelta, 0.f);
	}
	else if (m_ArrowType.bAimMode && m_vRaycastPos != _vec4())
	{
		m_pTransformCom->LookAt(m_vRaycastPos);
		if (!m_bIntersected)
		{
			if (m_pTransformCom->Go_To(m_vRaycastPos, fTimeDelta, 0.5f))
			{

				m_bIntersected = true;
			}
		}
		else if (m_fDeadTime < 2.f)
		{
			m_fDeadTime = 2.f;
		}

	}
	else
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	switch (m_ArrowType.Att_Type)

	{
	case AT_Bow_Common:
	{
		if (m_pGameInstance->CheckCollision_Monster(m_pCollider))
		{
			m_pGameInstance->Attack_Monster(m_pCollider, m_iDamage, AT_Bow_Common);
			Kill();
		}
		break;
	}
	case AT_Bow_Skill1:
	{
		if (m_pGameInstance->CheckCollision_Monster(m_pCollider))
		{
			m_pGameInstance->Attack_Monster(m_pCollider, m_iDamage, AT_Bow_Skill1);
			Kill();
		}
		break;
	}
	case AT_Bow_Skill2:
	{
		m_pTransformCom->Set_OldMatrix();

		if (not m_ArrowType.bAimMode)
		{
			PxRaycastBuffer Buffer{};
			_float fDist = 0.3f;
			if (m_pGameInstance->Raycast(m_pTransformCom->Get_State(State::Pos), m_pTransformCom->Get_State(State::Look).Get_Normalized(), fDist, Buffer))
			{
				Kill();
			}
		}

		if (m_pGameInstance->CheckCollision_Monster(m_pCollider) or m_bIntersected)
		{
			Kill();
		}

		if (m_isDead)
		{
			m_pCollider->Set_Radius(5.f);
			m_pGameInstance->Attack_Monster(m_pCollider, m_iDamage, AT_Bow_Skill2);

			_mat EffectMat = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));
			EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Arrow_Explosion_Spark");
			Info.pMatrix = &EffectMat;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Arrow_Explosion");
			Info.pMatrix = &EffectMat;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Arrow_Explosion_Impact");
			Info.pMatrix = &EffectMat;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Arrow_Explosion_Smoke");
			Info.pMatrix = &EffectMat;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Arrow_Explosion_Smoke2");
			Info.pMatrix = &EffectMat;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Arrow_Explosion_Smoke3");
			Info.pMatrix = &EffectMat;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Arrow_Explosion_Boom");
			Info.pMatrix = &EffectMat;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Arrow_Explosion_Distortion");
			Info.pMatrix = &EffectMat;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			m_pGameInstance->Attack_Monster(m_pCollider, m_iDamage, AT_Bow_Skill2);
			m_pGameInstance->Play_Sound(TEXT("SE_5130_Meteor_SFX_02"), 0.3f);
			m_pGameInstance->Play_Sound(TEXT("Draug_Attack03_SFX_01"));
			Kill();
		}
		break;
	}
	case AT_Bow_Skill3:
	{
		if (m_pGameInstance->CheckCollision_Monster(m_pCollider))
		{
			m_pGameInstance->Attack_Monster(m_pCollider, m_iDamage, AT_Bow_Skill3);
			Kill();
		}
		break;
	}
	case AT_Bow_SkillR:
	{
		if (m_pGameInstance->CheckCollision_Monster(m_pCollider))
		{
			m_pGameInstance->Attack_Monster(m_pCollider, m_iDamage, AT_Bow_Common);
			Kill();
		}
		break;
	}
	default:
		break;
	}

	m_pCollider->Update(m_pTransformCom->Get_World_Matrix());

	_mat world{};
	world = m_pTransformCom->Get_World_Matrix();
	m_pTrail->Tick((world.Position_vec3()));
	m_ParticleMatrix = m_pTransformCom->Get_World_Matrix();
	//m_ParticleMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));
	if (m_pParticle)
	{
		m_pParticle->Tick(fTimeDelta);
	}
}

void CArrow::Late_Tick(_float fTimeDelta)
{
	m_pTrail->Late_Tick(fTimeDelta);
	if (m_pParticle)
	{
		m_pParticle->Late_Tick(fTimeDelta);
	}
	__super::Compute_CamDistance();

	if (m_ArrowType.Att_Type == AT_Bow_Skill2)
	{
		m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
	}
	else
	{
		m_pRendererCom->Add_RenderGroup(RenderGroup::RG_Blend, this);
	}

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pCollider);
#endif // _DEBUG
}

HRESULT CArrow::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	else
	{
		for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
		{

			if (m_ArrowType.Att_Type == AT_Bow_Skill2)
			{
				if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
				{
					return E_FAIL;
				}

				if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
				{
					return E_FAIL;
				}

				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
				{
					int a = 0;
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
				if (FAILED(m_pShaderCom->Begin(StaticPass_Default)))
				{
					return E_FAIL;
				}
			}
			else
			{
				if (FAILED(m_pShaderCom->Begin(StaticPass_SingleColorFx)))
				{
					return E_FAIL;
				}
			}

			if (FAILED(m_pModelCom->Render(i)))
			{
				return E_FAIL;
			}
		}
	}

	return S_OK;
}
HRESULT CArrow::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}
	Collider_Desc CollDesc = {};
	if (m_ArrowType.Att_Type == AT_Bow_Skill2)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_BoomArrow"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		{
			return E_FAIL;
		}
		CollDesc.eType = ColliderType::OBB;

		CollDesc.eType = ColliderType::Sphere;
		CollDesc.fRadius = 0.2f;
		CollDesc.vCenter = _vec3(0.f, 0.f, -0.3f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
										  TEXT("Com_Arrow_Hit"), (CComponent**)&m_pCollider, &CollDesc)))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Arrow"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		{
			return E_FAIL;
		}

		CollDesc.eType = ColliderType::Sphere;
		CollDesc.fRadius = 0.2f;
		CollDesc.vCenter = _vec3(0.f, 0.f, -0.3f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
										  TEXT("Com_Arrow_Hit"), (CComponent**)&m_pCollider, &CollDesc)))
		{
			return E_FAIL;
		}

	}

	//Collider_Desc CollDesc = {};
	//CollDesc.eType = ColliderType::Sphere;
	//CollDesc.vRadians = _vec3(0.5f, 0.5f, 0.5f);
	//CollDesc.vCenter = _vec3(0.f, 0.f, 0.f);

	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
	//	TEXT("Com_Arrow_Hit"), (CComponent**)&m_pCollider, &CollDesc)))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CArrow::Bind_ShaderResources()
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

	_vec4 Color = _vec4(0.89f, 0.96f, 0.969f, 0.5f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &Color, sizeof _vec4)))
	{
		return E_FAIL;
	}

	return S_OK;
}



CArrow* CArrow::Create(_dev pDevice, _context pContext)
{
	CArrow* pInstance = new CArrow(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CArrow::Clone(void* pArg)
{
	CArrow* pInstance = new CArrow(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CArrow::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTrail);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pParticle);
}
