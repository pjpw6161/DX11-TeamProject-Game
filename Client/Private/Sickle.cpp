#include "Sickle.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"
#include "Collision_Manager.h"

CSickle::CSickle(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CSickle::CSickle(const CSickle& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CSickle::Init_Prototype()
{
	return S_OK;
}

HRESULT CSickle::Init(void* pArg)
{
	_uint iRandom = rand() % 4;


	_vec3 StartPos = *reinterpret_cast<_vec3*>(pArg);

	_float fSpeed{ 10.f };

	m_pTransformCom->Set_Position(StartPos);
	m_pTransformCom->Set_Speed(fSpeed);
	m_pTransformCom->LookAt(dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_CenterPos());


	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	//m_pGameInstance->Play_Sound(TEXT("SE_5130_Meteor_SFX_01"));

	return S_OK;
}

void CSickle::Tick(_float fTimeDelta)
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_CenterPos();
	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
	vPos = XMVectorLerp(vPos, vPlayerPos,0.05f);
	m_pTransformCom->Set_State(State::Pos, vPos);
	 
	m_fLifeTimer += fTimeDelta;
	if (m_fLifeTimer > 10.f)
	{
		Kill();
	}

	m_EffectMatrices[0] = _mat::CreateScale(4.f)/* * _mat::CreateRotationY(XMConvertToRadians(m_fLifeTimer *-2000.f))*/ * m_pTransformCom->Get_World_Matrix();
	m_EffectMatrices[1] = _mat::CreateScale(4.f) * m_pTransformCom->Get_World_Matrix();
	m_EffectMatrices[2] = _mat::CreateScale(4.f)* m_pTransformCom->Get_World_Matrix();
	
	for (size_t i = 0; i < 3; i++)
	{
		if (m_pEffects[i])
		{
			m_pEffects[i]->Tick(fTimeDelta);
		}
	}

	if (m_pGameInstance->Attack_Player(m_pColliderCom, 100))
	{
		Kill();
	}

	//if (m_isDead)
	//{
	//	m_pGameInstance->Play_Sound(TEXT("SD_4062_FireBall_SFX_01"));

	//	CEffect_Manager* pEffect_Manager = CEffect_Manager::Get_Instance();
	//	Safe_AddRef(pEffect_Manager);
	//	EffectInfo Info{};

	//	_mat EffectMat{ _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos))) };

	//	Info = pEffect_Manager->Get_EffectInformation(L"Green_Explosion");
	//	Info.pMatrix = &EffectMat;
	//	pEffect_Manager->Add_Layer_Effect(Info);

	//	Info = pEffect_Manager->Get_EffectInformation(L"Groar_XBeam_Spark");
	//	Info.pMatrix = &EffectMat;
	//	pEffect_Manager->Add_Layer_Effect(Info);

	//	Info = pEffect_Manager->Get_EffectInformation(L"Groar_XBeam_Explosion");
	//	Info.pMatrix = &EffectMat;
	//	pEffect_Manager->Add_Layer_Effect(Info);

	//	Info = pEffect_Manager->Get_EffectInformation(L"Arrow_Explosion_Smoke");
	//	Info.pMatrix = &EffectMat;
	//	pEffect_Manager->Add_Layer_Effect(Info);

	//	Info = pEffect_Manager->Get_EffectInformation(L"Arrow_Explosion_Smoke2");
	//	Info.pMatrix = &EffectMat;
	//	pEffect_Manager->Add_Layer_Effect(Info);

	//	Safe_Release(pEffect_Manager);
	//}
}

void CSickle::Late_Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < 3; i++)
	{
		if (m_pEffects[i])
		{
			m_pEffects[i]->Late_Tick(fTimeDelta);
		}
	}

	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif // _DEBUG

}

HRESULT CSickle::Add_Components()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.vCenter = _vec3(0.f);
	CollDesc.fRadius = 2.7f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_SafeZone_Collider"), (CComponent**)&m_pColliderCom, &CollDesc)))
	{
		return E_FAIL;
	}

	CEffect_Manager* pEffect_Manager = CEffect_Manager::Get_Instance();
	Safe_AddRef(pEffect_Manager);
	EffectInfo Info{};

	Info = pEffect_Manager->Get_EffectInformation(L"HumanBoss_Sickle");
	Info.pMatrix = &m_EffectMatrices[0];
	Info.isFollow = true;
	m_pEffects[0] = pEffect_Manager->Clone_Effect(Info);

	Info = pEffect_Manager->Get_EffectInformation(L"HumanBoss_Ring");
	Info.pMatrix = &m_EffectMatrices[1];
	Info.isFollow = true;
	m_pEffects[1] = pEffect_Manager->Clone_Effect(Info);

	Info = pEffect_Manager->Get_EffectInformation(L"HumanBoss_Ring2");
	Info.pMatrix = &m_EffectMatrices[2];
	Info.isFollow = true;
	m_pEffects[2] = pEffect_Manager->Clone_Effect(Info);


	Safe_Release(pEffect_Manager);

#ifdef _DEBUG
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}
#endif // _DEBUG


	return S_OK;
}

CSickle* CSickle::Create(_dev pDevice, _context pContext)
{
	CSickle* pInstance = new CSickle(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMissile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSickle::Clone(void* pArg)
{
	CSickle* pInstance = new CSickle(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSickle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSickle::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	for (size_t i = 0; i < 3; i++)
	{
		Safe_Release(m_pEffects[i]);
	}

#ifdef _DEBUG
	Safe_Release(m_pRendererCom);
#endif // _DEBUG

}
