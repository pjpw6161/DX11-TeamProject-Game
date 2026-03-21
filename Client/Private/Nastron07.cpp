#include "Nastron07.h"

const _float CNastron07::m_fChaseRange = 7.f;
const _float CNastron07::m_fAttackRange = 3.f;

CNastron07::CNastron07(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CNastron07::CNastron07(const CNastron07& rhs)
	: CMonster(rhs)
{
}

HRESULT CNastron07::Init_Prototype()
{
	return S_OK;
}

HRESULT CNastron07::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Nastron07");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;

	m_eCurState = STATE_IDLE;

	m_iHP = 5000;
	m_iDamageAccMax = 700;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	TRAIL_DESC Desc{};
	Desc.vColor = Colors::LightBlue;
	Desc.vPSize = _vec2(0.02f, 0.02f);
	Desc.iNumVertices = 10;
	m_pLeftTrail1 = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);
	m_pLeftTrail2 = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);
	m_pLeftTrail3 = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);
	m_pRightTrail1 = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);
	m_pRightTrail2 = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);
	m_pRightTrail3 = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 1.4f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.4f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	//m_pTransformCom->Set_Position( _vec3(5.f, 0.f, 0.f));

	m_MonsterHpBarPos = _vec3(0.f, 1.2f, 0.f);

	//if (pArg)
	{
		if (FAILED(__super::Init(pArg)))
		{
			return E_FAIL;
		}
	}

	_vec3 vPos = m_pTransformCom->Get_State(State::Pos);
	vPos.y += 3.f;

	m_pTransformCom->Set_Position(vPos);

    return S_OK;
}

void CNastron07::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_7))
	{
		Set_Damage(0, AT_Bow_Common);
	}

	__super::Tick(fTimeDelta);

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	Update_Collider();
	__super::Update_BodyCollider();

	Update_Trail(fTimeDelta);

	m_pTransformCom->Gravity(fTimeDelta);

}

void CNastron07::Late_Tick(_float fTimeDelta)
{
	m_pLeftTrail1->Late_Tick(fTimeDelta);
	m_pLeftTrail2->Late_Tick(fTimeDelta);
	m_pLeftTrail3->Late_Tick(fTimeDelta);
	m_pRightTrail1->Late_Tick(fTimeDelta);
	m_pRightTrail2->Late_Tick(fTimeDelta);
	m_pRightTrail3->Late_Tick(fTimeDelta);

	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif
}

HRESULT CNastron07::Render()
{
	__super::Render();

	return S_OK;
}

void CNastron07::Set_Damage(_int iDamage, _uint iDamageType)
{
	m_fHittedTime = 6.f;
	m_eCurState = STATE_HIT;

	m_iHP -= iDamage;
	m_bDamaged = true;
	m_bChangePass = true;
	if (m_bHit == false)
	{
		m_iDamageAcc += iDamage;
	}

	m_fIdleTime = 0.f;

	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	m_pTransformCom->LookAt(vPlayerPos);

	if (iDamageType == AT_Sword_Common || iDamageType == AT_Sword_Skill1 || iDamageType == AT_Sword_Skill2 ||
		iDamageType == AT_Sword_Skill3 || iDamageType == AT_Sword_Skill4 || iDamageType == AT_Bow_Skill2 || iDamageType == AT_Bow_Skill4)
	{
		// 경직
		m_Animation.fAnimSpeedRatio = 1.f;
	}

	if (iDamageType == AT_Bow_Common || iDamageType == AT_Bow_Skill1)
	{
		// 밀려나게
		_vec4 vDir = m_pTransformCom->Get_State(State::Pos) - __super::Compute_PlayerPos();
		m_pTransformCom->Go_To_Dir(vDir, m_fBackPower);

		m_Animation.fAnimSpeedRatio = 2.5f;
	}

	if (iDamageType == AT_Bow_Skill3)
	{
		// 이속 느려지게
		m_bSlow = true;
		m_Animation.fAnimSpeedRatio = 0.8f;
	}
}

void CNastron07::Init_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	_float fDistance = __super::Compute_PlayerDistance();
	_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
	vDir.y = 0.f;

	if (m_iHP <= 0)
	{
		m_eCurState = STATE_DIE;
	}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CNastron07::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;

			m_pTransformCom->Set_Speed(1.f);
			break;

		case Client::CNastron07::STATE_WALK:
			m_Animation.iAnimIndex = WALK;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 4.f;
			{
				random_device rd;
				_randNum RandNum(rd());
				_randFloat Random = _randFloat(-1.f, 1.f);
				m_pTransformCom->LookAt_Dir(_vec4(Random(RandNum), 0.f, Random(RandNum), 0.f));
			}
			break;

		case Client::CNastron07::STATE_CHASE:
			if (fDistance >= m_fAttackRange)
			{
				m_Animation.iAnimIndex = RUN;
			}

			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 4.f;

			if (m_bSlow == true)
			{
				m_pTransformCom->Set_Speed(1.f);
			}
			else
			{
				m_pTransformCom->Set_Speed(5.f);
			}

			break;

		case Client::CNastron07::STATE_ATTACK:
			m_bDamaged = false;
			m_Animation.fAnimSpeedRatio = 3.f;
			m_bAttacking = true;

			m_pTransformCom->LookAt_Dir(vDir);

			break;

		case Client::CNastron07::STATE_HIT:
			if (m_bHit == true)
			{
				m_Animation.iAnimIndex = KNOCKDOWN;
			}

			else
			{
				_uint iHitPattern = rand() % 2;
				switch (iHitPattern)
				{
				case 0:
					m_Animation.iAnimIndex = HIT_L;
					break;
				case 1:
					m_Animation.iAnimIndex = HIT_R;
					break;
				}
			}

			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CNastron07::STATE_DIE:
			m_Animation.iAnimIndex = DIE;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 3.f;

			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CNastron07::Tick_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	_float fDistance = __super::Compute_PlayerDistance();

	switch (m_eCurState)
	{
	case Client::CNastron07::STATE_IDLE:
	{
		m_fIdleTime += fTimeDelta;

		if (m_bAttacking == true)
		{
			if (m_fIdleTime >= 1.f)
			{
				if (fDistance >= m_fAttackRange)
				{
					m_eCurState = STATE_CHASE;
				}
				else
				{
					m_eCurState = STATE_ATTACK;
				}

				m_fIdleTime = 0.f;
			}

		}
		else
		{
			//if (m_fIdleTime >= 4.f)
			//{
			//	m_eCurState = STATE_WALK;
			//	m_fIdleTime = 0.f;
			//}

		}

		//if (fDistance <= m_fChaseRange)
		//{
		//	m_eCurState = STATE_CHASE;
		//}
	}
	break;

	case Client::CNastron07::STATE_WALK:
	{
		_float fDist = 1.2f;
		PxRaycastBuffer Buffer{};

		if (m_pGameInstance->Raycast(m_pTransformCom->Get_CenterPos(),
			m_pTransformCom->Get_State(State::Look).Get_Normalized(),
			fDist, Buffer))
		{
			m_pTransformCom->LookAt_Dir(PxVec3ToVector(Buffer.block.normal));
		}

		m_pTransformCom->Go_Straight(fTimeDelta);

		if (m_pModelCom->IsAnimationFinished(WALK))
		{
			m_eCurState = STATE_IDLE;
		}
	}
		break;

	case Client::CNastron07::STATE_CHASE:
	{
		_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
		vDir.y = 0.f;

		if (fDistance > m_fChaseRange && !m_bDamaged)
		{
			m_eCurState = STATE_IDLE;
			m_bSlow = false;
			m_bAttacking = false;

			break;
		}

		if (fDistance <= m_fAttackRange)
		{
			m_eCurState = STATE_ATTACK;
			m_Animation.isLoop = true;
			m_bSlow = false;
		}
		else
		{
			m_pTransformCom->LookAt_Dir(vDir);
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
	}
	break;

	case Client::CNastron07::STATE_ATTACK:

		if (!m_bSelectAttackPattern)
		{
			if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
				m_pModelCom->IsAnimationFinished(ATTACK03))
			{
				m_iAttackPattern = rand() % 3;
				m_bSelectAttackPattern = true;
				m_bAttacked = false;
				m_bAttacked2 = false;
				m_bAttacked3 = false;
			}
		}

		switch (m_iAttackPattern)
		{
		case 0:
			m_Animation.iAnimIndex = ATTACK01;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 54.f && fAnimpos <= 56.f && !m_bAttacked)
				{
					_uint iDamage = m_iDefaultDamage1 + rand() % 30;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				if (fAnimpos >= 49.f && fAnimpos <= 58.f)
				{
					m_pLeftTrail1->On();
					m_pLeftTrail2->On();
					m_pLeftTrail3->On();
					m_pRightTrail1->On();
					m_pRightTrail2->On();
					m_pRightTrail3->On();
				}
			}
			break;

		case 1:
			m_Animation.iAnimIndex = ATTACK02;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 39.f && fAnimpos <= 41.f && !m_bAttacked)
				{
					_uint iDamage = m_iDefaultDamage1 + rand() % 30;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				if (fAnimpos >= 82.f && fAnimpos <= 84.f && !m_bAttacked2)
				{
					_uint iDamage = m_iDefaultDamage1 + rand() % 30;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked2 = true;
				}

				if (fAnimpos >= 34.f && fAnimpos <= 44.f)
				{
					m_pLeftTrail1->On();
					m_pLeftTrail2->On();
					m_pLeftTrail3->On();
					m_pRightTrail1->On();
					m_pRightTrail2->On();
					m_pRightTrail3->On();

				}
				if (fAnimpos >= 79.f && fAnimpos <= 86.f)
				{
					m_pLeftTrail1->On();
					m_pLeftTrail2->On();
					m_pLeftTrail3->On();
					m_pRightTrail1->On();
					m_pRightTrail2->On();
					m_pRightTrail3->On();

				}

			}
			break;

		case 2:
			m_Animation.iAnimIndex = ATTACK03;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 56.f && fAnimpos <= 58.f && !m_bAttacked)
				{
					_uint iDamage = m_iDefaultDamage1 + rand() % 30;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				if (fAnimpos >= 86.f && fAnimpos <= 88.f && !m_bAttacked2)
				{
					_uint iDamage = m_iDefaultDamage1 + rand() % 30;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked2 = true;
				}
				if (fAnimpos >= 140.f && fAnimpos <= 142.f && !m_bAttacked3)
				{
					_uint iDamage = m_iDefaultDamage1 + rand() % 30;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Stun);
					m_bAttacked3 = true;
				}

			}
			break;
		}

		if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
			m_pModelCom->IsAnimationFinished(ATTACK03))
		{
			m_eCurState = STATE_IDLE;
		}
		break;

	case Client::CNastron07::STATE_HIT:

		if (m_pModelCom->IsAnimationFinished(m_Animation.iAnimIndex))
		{
			m_eCurState = STATE_CHASE;
			m_fIdleTime = 0.f;

			if (m_bHit == true)
			{
				m_iDamageAcc = 0;
				m_bHit = false;
			}
		}

		break;

	case Client::CNastron07::STATE_DIE:

		if (m_pModelCom->IsAnimationFinished(DIE))
		{
			m_fDeadTime += fTimeDelta;
		}

		break;
	}
}

void CNastron07::Update_Trail(_float fTimeDelta)
{
	// 가운데 손가락
	_mat Matrix = *m_pModelCom->Get_BoneMatrix("Bone_Claw_L");
	_mat Offset = _mat::CreateTranslation(_vec3(-1.62f, 0.01f, 0.27f));
	_mat Result = Offset * Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

	m_pLeftTrail2->Tick(Result.Position_vec3());

	Matrix = *m_pModelCom->Get_BoneMatrix("Bone_Claw_R");
	Offset = _mat::CreateTranslation(_vec3(1.62f, -0.01f, -0.27f));
	Result = Offset * Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

	m_pRightTrail2->Tick(Result.Position_vec3());



	// 1번째 손가락
	Matrix = *m_pModelCom->Get_BoneMatrix("Bone_Claw_L");
	Offset = _mat::CreateTranslation(_vec3(-1.55f, 0.09f, 0.2f));
	Result = Offset * Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

	m_pLeftTrail1->Tick(Result.Position_vec3());

	Matrix = *m_pModelCom->Get_BoneMatrix("Bone_Claw_R");
	Offset = _mat::CreateTranslation(_vec3(1.55f, -0.09f, -0.2f));
	Result = Offset * Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

	m_pRightTrail1->Tick(Result.Position_vec3());



	// 3번째 손가락
	Matrix = *m_pModelCom->Get_BoneMatrix("Bone_Claw_L");
	Offset = _mat::CreateTranslation(_vec3(-1.52f, -0.07f, 0.25f));
	Result = Offset * Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

	m_pLeftTrail3->Tick(Result.Position_vec3());

	Matrix = *m_pModelCom->Get_BoneMatrix("Bone_Claw_R");
	Offset = _mat::CreateTranslation(_vec3(1.52f, 0.07f, -0.25f));
	Result = Offset * Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

	m_pRightTrail3->Tick(Result.Position_vec3());

}

HRESULT CNastron07::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(0.6f, 1.4f, 0.4f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
		return E_FAIL;

	// Frustum
	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Frustum;
	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
	_matrix matView = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	// 1인자 : 절두체 각도(범위), 2인자 : Aspect, 3인자 : Near, 4인자 : Far(절두체 깊이)
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 1.f, 0.01f, 3.f);
	XMStoreFloat4x4(&ColDesc.matFrustum, matView * matProj);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CNastron07::Update_Collider()
{
	_mat Offset = _mat::CreateTranslation(0.f, 1.5f, 0.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

CNastron07* CNastron07::Create(_dev pDevice, _context pContext)
{
	CNastron07* pInstance = new CNastron07(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CNastron07");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNastron07::Clone(void* pArg)
{
	CNastron07* pInstance = new CNastron07(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CNastron07");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNastron07::Free()
{
	__super::Free();

	Safe_Release(m_pLeftTrail1);
	Safe_Release(m_pLeftTrail2);
	Safe_Release(m_pLeftTrail3);
	Safe_Release(m_pRightTrail1);
	Safe_Release(m_pRightTrail2);
	Safe_Release(m_pRightTrail3);
}
