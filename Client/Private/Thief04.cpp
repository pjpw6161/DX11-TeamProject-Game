#include "Thief04.h"

const _float CThief04::m_fChaseRange = 7.f;
const _float CThief04::m_fAttackRange = 2.f;

CThief04::CThief04(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CThief04::CThief04(const CThief04& rhs)
	: CMonster(rhs)
{
}

HRESULT CThief04::Init_Prototype()
{
    return S_OK;
}

HRESULT CThief04::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Thief04");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 30) + 60.f, 0.f, static_cast<_float>(rand() % 30) + 60.f, 1.f));

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;

	m_eCurState = STATE_IDLE;

	m_iHP = 2000;
	m_iDamageAccMax = 400;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	SURFACETRAIL_DESC Desc{};
	Desc.vColor = Colors::Gray;
	Desc.iNumVertices = 10;

	m_pAxeTrail = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);
	m_pKnifeTrail = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.8f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.7f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	m_pTransformCom->Set_Position(_vec3(100.f, 8.f, 108.f));
	m_MonsterHpBarPos = _vec3(0.f, 1.2f, 0.f);

	//if (pArg)
	{
		if (FAILED(__super::Init(pArg)))
		{
			return E_FAIL;
		}
	}

    return S_OK;
}

void CThief04::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_T))
	{
		Set_Damage(0, AT_Sword_Common);
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

void CThief04::Late_Tick(_float fTimeDelta)
{
	m_pAxeTrail->Late_Tick(fTimeDelta);
	m_pKnifeTrail->Late_Tick(fTimeDelta);

	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif
}

HRESULT CThief04::Render()
{
	__super::Render();

    return S_OK;
}

void CThief04::Set_Damage(_int iDamage, _uint iDamageType)
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
		m_Animation.fAnimSpeedRatio = 0.8f;
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

void CThief04::Init_State(_float fTimeDelta)
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
		case Client::CThief04::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;

			m_pTransformCom->Set_Speed(1.5f);
			break;

		case Client::CThief04::STATE_WALK:
			m_Animation.iAnimIndex = WALK;
			m_Animation.isLoop = false;
			{
				random_device rd;
				_randNum RandNum(rd());
				_randFloat Random = _randFloat(-1.f, 1.f);
				m_pTransformCom->LookAt_Dir(_vec4(Random(RandNum), 0.f, Random(RandNum), 0.f));
			}
			break;

		case Client::CThief04::STATE_CHASE:
			if (fDistance >= m_fAttackRange)
			{
				m_Animation.iAnimIndex = RUN;
			}

			m_Animation.isLoop = true;
			m_pTransformCom->Set_Speed(3.f);

			if (m_bSlow == true)
			{
				m_pTransformCom->Set_Speed(1.f);
			}
			else
			{
				m_pTransformCom->Set_Speed(4.f);
			}

			break;

		case Client::CThief04::STATE_ATTACK:
			m_bDamaged = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			m_bAttacking = true;

			m_pTransformCom->LookAt_Dir(vDir);

			break;

		case Client::CThief04::STATE_HIT:
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
					m_Animation.iAnimIndex = L_HIT;
					break;
				case 1:
					m_Animation.iAnimIndex = R_HIT;
					break;
				}
			}

			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CThief04::STATE_DIE:
			m_Animation.iAnimIndex = DIE;
			m_Animation.isLoop = false;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CThief04::Tick_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	_float fDistance = __super::Compute_PlayerDistance();

	switch (m_eCurState)
	{
	case Client::CThief04::STATE_IDLE:
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
			if (m_fIdleTime >= 2.f)
			{
				m_eCurState = STATE_WALK;
				m_fIdleTime = 0.f;
			}

		}

		//if (fDistance <= m_fChaseRange)
		//{
		//	m_eCurState = STATE_CHASE;
		//}
	}

		break;

	case Client::CThief04::STATE_WALK:
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

	case Client::CThief04::STATE_CHASE:
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

	case Client::CThief04::STATE_ATTACK:

		if (!m_bSelectAttackPattern)
		{
			if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
				m_pModelCom->IsAnimationFinished(ATTACK03) || m_pModelCom->IsAnimationFinished(ATTACK04) ||
				m_pModelCom->IsAnimationFinished(ATTACK05))
			{
				m_iAttackPattern = rand() % 5;
				m_bSelectAttackPattern = true;
				m_bAttacked = false;
				m_bAttacked2 = false;
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
				if (fAnimpos >= 46.f && fAnimpos <= 48.f && !m_bAttacked)
				{
					_uint iDamage = m_iDefaultDamage1 - rand() % 10;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				if (fAnimpos >= 40.f && fAnimpos <= 48.f)
				{
					m_pAxeTrail->On();
					m_pKnifeTrail->On();
				}
			}
			break;

		case 1:
			m_Animation.iAnimIndex = ATTACK02;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 24.f && fAnimpos <= 26.f && !m_bAttacked)
				{
					_uint iDamage = m_iDefaultDamage1 - rand() % 10;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				if (fAnimpos >= 47.f && fAnimpos <= 49.f && !m_bAttacked2)
				{
					_uint iDamage = m_iDefaultDamage1 - rand() % 10;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked2 = true;
				}
				if (fAnimpos >= 22.f && fAnimpos <= 28.f)
				{
					m_pKnifeTrail->On();

				}
				if (fAnimpos >= 44.f && fAnimpos <= 49.f)
				{
					m_pAxeTrail->On();
				}
			}
			break;

		case 2:
			m_Animation.iAnimIndex = ATTACK03;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 37.f && fAnimpos <= 39.f && !m_bAttacked)
				{
					_uint iDamage = m_iDefaultDamage1 - rand() % 10;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				if (fAnimpos >= 29.f && fAnimpos <= 40.f)
				{
					m_pKnifeTrail->On();
					m_pAxeTrail->On();
				}
			}
			break;

		case 3:
			m_Animation.iAnimIndex = ATTACK04;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 33.f && fAnimpos <= 35.f && !m_bAttacked)
				{
					_uint iDamage = m_iDefaultDamage1 - rand() % 10;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				if (fAnimpos >= 30.f && fAnimpos <= 34.f)
				{
					m_pAxeTrail->On();
				}
				if (fAnimpos >= 44.f && fAnimpos <= 50.f)
				{
					m_pKnifeTrail->On();
				}
			}
			break;

		case 4:
			m_Animation.iAnimIndex = ATTACK05;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 44.f && fAnimpos <= 46.f && !m_bAttacked)
				{
					_uint iDamage = m_iDefaultDamage1 - rand() % 10;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				if (fAnimpos >= 42.f && fAnimpos <= 48.f)
				{
					m_pAxeTrail->On();
				}
			}
			break;
		}

		if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
			m_pModelCom->IsAnimationFinished(ATTACK03) || m_pModelCom->IsAnimationFinished(ATTACK04) ||
			m_pModelCom->IsAnimationFinished(ATTACK05))
		{
			m_eCurState = STATE_IDLE;
		}

		break;

	case Client::CThief04::STATE_HIT:

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

	case Client::CThief04::STATE_DIE:

		if (m_pModelCom->IsAnimationFinished(DIE))
		{
			m_fDeadTime += fTimeDelta;
		}

		break;
	}
}

void CThief04::Update_Trail(_float fTimeDelta)
{
	_mat AxeMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-R-Hand");
	_mat Offset = _mat::CreateTranslation(_vec3(0.23f, 0.04f, -0.52f));
	_mat Result = Offset * AxeMatrix * m_pTransformCom->Get_World_Matrix();

	AxeMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-R-Hand");
	Offset = _mat::CreateTranslation(_vec3(0.24f, 0.04f, -0.27f));
	_mat Result2 = Offset * AxeMatrix * m_pTransformCom->Get_World_Matrix();

	m_pAxeTrail->Tick(Result.Position_vec3(), Result2.Position_vec3());

	_mat KnifeMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-L-Hand");
	Offset = _mat::CreateTranslation(_vec3(0.06f, 0.08f, -0.34f));
	Result = Offset * KnifeMatrix * m_pTransformCom->Get_World_Matrix();

	KnifeMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-L-Hand");
	Offset = _mat::CreateTranslation(_vec3(0.07f, 0.06f, -0.17f));
	Result2 = Offset * KnifeMatrix * m_pTransformCom->Get_World_Matrix();

	m_pKnifeTrail->Tick(Result.Position_vec3(), Result2.Position_vec3());
}

HRESULT CThief04::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(0.3f, 0.8f, 0.5f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
		return E_FAIL;

	// Frustum
	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Frustum;
	_matrix matView = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	// 1인자 : 절두체 각도(범위), 2인자 : Aspect, 3인자 : Near, 4인자 : Far(절두체 깊이)
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 1.f, 0.01f, 2.f);
	XMStoreFloat4x4(&ColDesc.matFrustum, matView * matProj);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

    return S_OK;
}

void CThief04::Update_Collider()
{
	_mat Offset = _mat::CreateTranslation(0.f, 0.8f, 0.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

CThief04* CThief04::Create(_dev pDevice, _context pContext)
{
	CThief04* pInstance = new CThief04(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CThief04");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CThief04::Clone(void* pArg)
{
	CThief04* pInstance = new CThief04(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CThief04");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CThief04::Free()
{
	__super::Free();

	Safe_Release(m_pAxeTrail);
	Safe_Release(m_pKnifeTrail);
}
