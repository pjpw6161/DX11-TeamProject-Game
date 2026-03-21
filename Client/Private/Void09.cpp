#include "Void09.h"
#include "UI_Manager.h"
#include "Dead.h"

const _float CVoid09::m_fChaseRange = 7.f;
const _float CVoid09::m_fAttackRange = 2.f;

_uint CVoid09::m_iIndex = 0;

CVoid09::CVoid09(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CVoid09::CVoid09(const CVoid09& rhs)
	: CMonster(rhs)
{
}

HRESULT CVoid09::Init_Prototype()
{
    return S_OK;
}

HRESULT CVoid09::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Void09");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 30) + 60.f, 0.f, static_cast<_float>(rand() % 30) + 60.f, 1.f));

	//random_device rand;
	//_randNum RandomNumber(rand());
	//_randFloat RandomAnimPos(0.f, 1000.f);
	//m_Animation.fStartAnimPos = RandomAnimPos(RandomNumber);

	m_eCurState = STATE_IDLE;

	m_iHP = 2000;
	m_iDamageAccMax = 500;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	SURFACETRAIL_DESC Desc{};
	Desc.vColor = Colors::Sienna;
	Desc.iNumVertices = 10;

	m_pSwordTrail = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.6f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.7f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);


	switch (m_iIndex)
	{
	case 0:
		m_pTransformCom->Set_Position(_vec3(2102.f, -16.f, 2091.f));
		//m_pTransformCom->Set_Position(_vec3(2087.f, -10.f, 2086.f));
		break;
	case 1:
		m_pTransformCom->Set_Position(_vec3(2102.f, -16.f, 2081.f));
		//m_pTransformCom->Set_Position(_vec3(2087.f, -10.f, 2086.f));
		break;
	}


	++m_iIndex;

	m_MonsterHpBarPos = _vec3(0.f, 1.5f, 0.f);

	//if (pArg)
	{
		if (FAILED(__super::Init(pArg)))
		{
			return E_FAIL;
		}
	}

	m_pTransformCom->Set_Position(_vec3(__super::Compute_PlayerPos())); // Test

	return S_OK;
}

void CVoid09::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_EQUALS))
	{
		Kill();
		m_pGameInstance->Delete_CollisionObject(this);
		m_pTransformCom->Delete_Controller();
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

void CVoid09::Late_Tick(_float fTimeDelta)
{
	m_pSwordTrail->Late_Tick(fTimeDelta);

	__super::Late_Tick(fTimeDelta);

	//m_Animation.fStartAnimPos = 0.f;

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif
}

HRESULT CVoid09::Render()
{
	__super::Render();

    return S_OK;
}

void CVoid09::Set_Damage(_int iDamage, _uint iDamageType)
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

	_bool isCritical{};
	if (iDamageType == (_uint)AT_End - 1)
	{
		isCritical = true;
	}
	CHitEffect::HITEFFECT_DESC Desc{};
	Desc.iDamage = iDamage;
	Desc.isCritical = isCritical;
	Desc.pParentTransform = m_pTransformCom;
	Desc.vTextPosition = _vec2(0.f, 1.5f);
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_HitEffect"), TEXT("Prototype_GameObject_HitEffect"), &Desc)))
	{
		return;
	}
	m_fIdleTime = 0.f;


	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	m_pTransformCom->LookAt(vPlayerPos);

	if (iDamageType == AT_Sword_Common || iDamageType == AT_Sword_Skill1 || iDamageType == AT_Sword_Skill2 ||
		iDamageType == AT_Sword_Skill3 || iDamageType == AT_Sword_Skill4 || iDamageType == AT_Bow_Skill2 || iDamageType == AT_Bow_Skill4|| iDamageType == AT_Critical)
	{
		// 경직
		m_Animation.fAnimSpeedRatio = 1.f;
	}

	if (iDamageType == AT_Bow_Common || iDamageType == AT_Bow_Skill1)
	{
		// 밀려나게
		_vec4 vDir = m_pTransformCom->Get_State(State::Pos) - __super::Compute_PlayerPos();
		m_pTransformCom->Go_To_Dir(vDir, m_fBackPower);

		m_Animation.fAnimSpeedRatio = 2.f;
	}

	if (iDamageType == AT_Bow_Skill3)
	{
		// 이속 느려지게
		m_bSlow = true;
		m_Animation.fAnimSpeedRatio = 0.8f;
	}
}

void CVoid09::Init_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	_float fDistance = __super::Compute_PlayerDistance();
	_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
	vDir.y = 0.f;

	if (m_iHP <= 0)
	{
		//m_eCurState = STATE_DIE;
		Kill();

		CDead::DEAD_DESC Desc = {};
		Desc.eDead = CDead::VOID09;
		Desc.vPos = m_pTransformCom->Get_State(State::Pos);
		m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Void09_Die"), TEXT("Prototype_GameObject_Dead"), &Desc);
	}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CVoid09::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;

			m_pTransformCom->Set_Speed(1.5f);
			break;

		case Client::CVoid09::STATE_WALK:
			m_Animation.iAnimIndex = WALK;
			m_Animation.isLoop = false;
			{
				random_device rd;
				_randNum RandNum(rd());
				_randFloat Random = _randFloat(-1.f, 1.f);
				m_pTransformCom->LookAt_Dir(_vec4(Random(RandNum), 0.f, Random(RandNum), 0.f));
			}
			break;

		case Client::CVoid09::STATE_CHASE:
			if (fDistance >= m_fAttackRange)
			{
				m_Animation.iAnimIndex = RUN;
			}

			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 3.f;

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

		case Client::CVoid09::STATE_ATTACK:
			m_bDamaged = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			m_bAttacking = true;

			m_pTransformCom->LookAt_Dir(vDir);

			break;

		case Client::CVoid09::STATE_HIT:
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

		case Client::CVoid09::STATE_KNOCKDOWN:
			m_Animation.iAnimIndex = KNOCKDOWN;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			m_Animation.fInterpolationTime = 0.4f;

		case Client::CVoid09::STATE_DIE:
			m_Animation.iAnimIndex = DIE;
			m_Animation.isLoop = false;

			_uint iRandomExp = rand() % 100;
			CUI_Manager::Get_Instance()->Set_Exp_ByPercent(15.f + (_float)iRandomExp / 2.f * 0.1f);
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CVoid09::Tick_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	_float fDistance = __super::Compute_PlayerDistance();

	switch (m_eCurState)
	{
	case Client::CVoid09::STATE_IDLE:
	{
		m_fIdleTime += fTimeDelta;

		//if (m_bAttacking == true)
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
		//else
		//{
		//	if (m_fIdleTime >= static_cast<_float>(rand() % 4) + 2.f)
		//	{
		//		m_eCurState = STATE_WALK;
		//		m_fIdleTime = 0.f;
		//	}

		//}

		//if (fDistance <= m_fChaseRange)
		//{
		//	m_eCurState = STATE_CHASE;
		//}
	}

		break;

	case Client::CVoid09::STATE_WALK:
	{
		_float fDist = 1.2f; PxRaycastBuffer Buffer1{};
		if (m_pGameInstance->Raycast(m_pTransformCom->Get_CenterPos(), m_pTransformCom->Get_State(State::Look).Get_Normalized(), fDist, Buffer1))
		{
			m_pTransformCom->LookAt_Dir(PxVec3ToVector(Buffer1.block.normal));
		}

		_float fHeight = 3.f; PxRaycastBuffer Buffer2{};
		if (m_pGameInstance->Raycast(m_pTransformCom->Get_CenterPos() + 0.2f * m_pTransformCom->Get_State(State::Look).Get_Normalized(),
			_vec4(0.f, -1.f, 0.f, 0.f), fHeight, Buffer2))
		{
			m_pTransformCom->Go_Straight(fTimeDelta);
		}

		if (m_pModelCom->IsAnimationFinished(WALK))
		{
			m_eCurState = STATE_IDLE;
		}
	}
		break;

	case Client::CVoid09::STATE_CHASE:
	{
		_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
		vDir.y = 0.f;

		//if (fDistance > m_fChaseRange && !m_bDamaged)
		//{
		//	m_eCurState = STATE_IDLE;
		//	m_bSlow = false;
		//	m_bAttacking = false;

		//	break;
		//}

		if (fDistance <= m_fAttackRange)
		{
			m_eCurState = STATE_ATTACK;
			m_Animation.isLoop = true;
			m_bSlow = false;
		}
		else
		{
			m_pTransformCom->LookAt_Dir(vDir);

			_float fHeight = 3.f; PxRaycastBuffer Buffer2{};
			if (m_pGameInstance->Raycast(m_pTransformCom->Get_CenterPos() + 0.2f * m_pTransformCom->Get_State(State::Look).Get_Normalized(),
				_vec4(0.f, -1.f, 0.f, 0.f), fHeight, Buffer2))
			{
				m_pTransformCom->Go_Straight(fTimeDelta);
			}
		}
	}
		break;

	case Client::CVoid09::STATE_ATTACK:

		if (!m_bSelectAttackPattern)
		{
			if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
				m_pModelCom->IsAnimationFinished(ATTACK03))
			{
				m_iAttackPattern = rand() % 3;
				m_bSelectAttackPattern = true;
				m_bAttacked = false;
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
				if (fAnimpos >= 31.f && fAnimpos <= 33.f && !m_bAttacked)
				{
					if (m_pGameInstance->CheckCollision_Parrying(m_pAttackColliderCom))
					{
						m_eCurState = STATE_KNOCKDOWN;
						break;
					}

					_uint iDamage = m_iSmallDamage + rand() % 10;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				if (fAnimpos >= 29.f && fAnimpos <= 33.f)
				{
					m_pSwordTrail->On();
				}
			}
			break;

		case 1:
			m_Animation.iAnimIndex = ATTACK02;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 43.f && fAnimpos <= 45.f && !m_bAttacked)
				{
					_uint iDamage = m_iSmallDamage + rand() % 10;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
			}
			break;

		case 2:
			m_Animation.iAnimIndex = ATTACK03;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				if (m_pGameInstance->CheckCollision_Parrying(m_pAttackColliderCom))
				{
					m_eCurState = STATE_KNOCKDOWN;
					break;
				}

				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 36.f && fAnimpos <= 38.f && !m_bAttacked)
				{
					_uint iDamage = m_iSmallDamage + rand() % 10;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Stun);
					m_bAttacked = true;
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

	case Client::CVoid09::STATE_HIT:

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

	case Client::CVoid09::STATE_KNOCKDOWN:

		if (m_pModelCom->IsAnimationFinished(KNOCKDOWN))
		{
			m_eCurState = STATE_CHASE;
		}

		break;

	case Client::CVoid09::STATE_DIE:

		if (m_pModelCom->IsAnimationFinished(DIE))
		{
			m_fDeadTime += fTimeDelta;
		}

		break;
	}
}

void CVoid09::Update_Trail(_float fTimeDelta)
{
	_mat Matrix = *m_pModelCom->Get_BoneMatrix("Bip001-R-Hand");
	_mat Offset = _mat::CreateTranslation(_vec3(-0.3f, 0.1f, -1.69f));
	_mat Result1 = Offset * Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

	Matrix = *m_pModelCom->Get_BoneMatrix("Bip001-R-Hand");
	Offset = _mat::CreateTranslation(_vec3(0.29f, 0.07f, -0.36f));
	_mat Result2 = Offset * Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

	m_pSwordTrail->Tick(Result1.Position_vec3(), Result2.Position_vec3());
}

HRESULT CVoid09::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(0.5f, 0.6f, 0.5f);
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

void CVoid09::Update_Collider()
{
	_mat Offset = _mat::CreateTranslation(0.f, 0.6f, 0.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

CVoid09* CVoid09::Create(_dev pDevice, _context pContext)
{
	CVoid09* pInstance = new CVoid09(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVoid09");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVoid09::Clone(void* pArg)
{
	CVoid09* pInstance = new CVoid09(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVoid09");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVoid09::Free()
{
	__super::Free();
	CEvent_Manager::Get_Instance()->Update_Quest(TEXT("그로아를 지켜라"));
	Safe_Release(m_pSwordTrail);
}
