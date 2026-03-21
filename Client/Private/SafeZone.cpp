#include "SafeZone.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"
#include "Collision_Manager.h"

CSafeZone::CSafeZone(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CSafeZone::CSafeZone(const CSafeZone& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CSafeZone::Init_Prototype()
{
	return S_OK;
}

HRESULT CSafeZone::Init(void* pArg)
{
	
	_uint iRandom = rand() % 4;

	switch (iRandom)
	{
	case 0:m_pTransformCom->Set_Position(_vec3(-2991.653f, -2.1066f, -3006.270f));
		break;
	case 1:	m_pTransformCom->Set_Position(_vec3(-2997.939f, -2.1066f, -3012.380f));
		break;
	case 2:m_pTransformCom->Set_Position(_vec3(-3004.307f, -2.1066f, -3006.045f));
		break;
	case 3:m_pTransformCom->Set_Position(_vec3(-2997.846f, -2.1066f,-2999.703f));
		break;
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	//m_pGameInstance->Play_Sound(TEXT("SE_5130_Meteor_SFX_01"));

	return S_OK;
}

void CSafeZone::Tick(_float fTimeDelta)
{
	

	m_fLifeTimer += fTimeDelta;
	if (m_fLifeTimer > 5.f)
	{
		Kill();
	}

	m_EffectMatrices[0] = _mat::CreateScale(10.f)/*_mat::CreateRotationX(XMConvertToRadians(35.f)) * _mat::CreateRotationY(XMConvertToRadians(90.f)) **/ * m_pTransformCom->Get_World_Matrix();
	m_EffectMatrices[1] = _mat::CreateScale(10.f)/*_mat::CreateRotationX(XMConvertToRadians(35.f)) * _mat::CreateRotationY(XMConvertToRadians(90.f)) **/ * m_pTransformCom->Get_World_Matrix();
	
	for (size_t i = 0; i < 2; i++)
	{
		if (m_pEffects[i])
		{
			m_pEffects[i]->Tick(fTimeDelta);
		}
	}

	//if (m_pGameInstance->Attack_Player(m_pColliderCom, 100))
	//{
	//	Kill();
	//}

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

void CSafeZone::Late_Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < 2; i++)
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

HRESULT CSafeZone::Add_Components()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.vCenter = _vec3(0.f);
	CollDesc.fRadius = 4.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_SafeZone_Collider"), (CComponent**)&m_pColliderCom, &CollDesc)))
	{
		return E_FAIL;
	}

	CEffect_Manager* pEffect_Manager = CEffect_Manager::Get_Instance();
	Safe_AddRef(pEffect_Manager);
	EffectInfo Info{};

	Info = pEffect_Manager->Get_EffectInformation(L"SafeZone");
	Info.pMatrix = &m_EffectMatrices[0];
	Info.isFollow = true;
	m_pEffects[0] = pEffect_Manager->Clone_Effect(Info);

	Info = pEffect_Manager->Get_EffectInformation(L"SafeZone_Distortion");
	Info.pMatrix = &m_EffectMatrices[1];
	Info.isFollow = true;
	m_pEffects[1] = pEffect_Manager->Clone_Effect(Info);

	Safe_Release(pEffect_Manager);

#ifdef _DEBUG
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}
#endif // _DEBUG


	return S_OK;
}

CSafeZone* CSafeZone::Create(_dev pDevice, _context pContext)
{
	CSafeZone* pInstance = new CSafeZone(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMissile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSafeZone::Clone(void* pArg)
{
	CSafeZone* pInstance = new CSafeZone(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSafeZone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSafeZone::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	for (size_t i = 0; i < 2; i++)
	{
		Safe_Release(m_pEffects[i]);
	}

#ifdef _DEBUG
	Safe_Release(m_pRendererCom);
#endif // _DEBUG

}
