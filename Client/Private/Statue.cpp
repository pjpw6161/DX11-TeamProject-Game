#include "Statue.h"

CStatue::CStatue(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CStatue::CStatue(const CStatue& rhs)
	: CMonster(rhs)
{
}

HRESULT CStatue::Init_Prototype()
{
	return S_OK;
}

HRESULT CStatue::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Statue");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_Animation.iAnimIndex = 0;
	m_Animation.isLoop = false;
	m_Animation.fAnimSpeedRatio = 2.f;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	//m_pTransformCom->Set_Scale(_vec3(1.5f));
	m_pTransformCom->Set_Position(_vec3(__super::Compute_PlayerPos()) + _vec3(0.f, 1.f, 0.f)); // Test
	
	//m_pTransformCom->LookAt_Dir(_vec4(0.f, 0.f, -1.f, 0.f));
	m_pTransformCom->LookAt_Dir(__super::Compute_PlayerLook());

	m_pModelCom->Play_Animation(0.f);

	return S_OK;
}

void CStatue::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//if (m_pGameInstance->Key_Pressing(DIK_DOWN))
	//{
	//	m_bDamaged = true;

	//}

	if (m_pModelCom->IsAnimationFinished(0))
	{
		//Kill();
		m_fDeadTime += fTimeDelta;
	}

	__super::Update_BodyCollider();

	m_pModelCom->Set_Animation(m_Animation);
}

void CStatue::Late_Tick(_float fTimeDelta)
{
	if (m_bDamaged == true)
	{
		m_pModelCom->Play_Animation(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
#endif

}

HRESULT CStatue::Render()
{
	__super::Render();

	return S_OK;
}

void CStatue::Set_Damage(_int iDamage, _uint iDamageType)
{
	if (iDamageType == AT_Bow_Skill2)
	{
		m_bDamaged = true;
	}
}

HRESULT CStatue::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(0.5f, 1.4f, 0.3f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
		return E_FAIL;

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 1.f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.6f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	return S_OK;
}

void CStatue::Update_Collider()
{
}

CStatue* CStatue::Create(_dev pDevice, _context pContext)
{
	CStatue* pInstance = new CStatue(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CStatue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStatue::Clone(void* pArg)
{
	CStatue* pInstance = new CStatue(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CStatue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatue::Free()
{
	__super::Free();
}
