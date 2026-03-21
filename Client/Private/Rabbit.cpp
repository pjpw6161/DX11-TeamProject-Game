#include "Rabbit.h"
#include "UI_Manager.h"
#include "Event_Manager.h"

const _float CRabbit::m_fChaseRange = 5.f;
const _float CRabbit::m_fAttackRange = 2.f;

CRabbit::CRabbit(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CRabbit::CRabbit(const CRabbit& rhs)
	: CMonster(rhs)
{
}

HRESULT CRabbit::Init_Prototype()
{
	return S_OK;
}

HRESULT CRabbit::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Rabbit");

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
	m_Animation.fAnimSpeedRatio = 2.f;

	m_eCurState = STATE_IDLE;

	m_iHP = 250;
	m_iDamageAccMax = 150;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.4f;
	ControllerDesc.radius = 0.6f;
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f);
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f));
	ControllerDesc.contactOffset = 0.1f;
	ControllerDesc.stepOffset = 0.2f;

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	_vec3 vPlayerPos = __super::Compute_PlayerPos();
	m_pTransformCom->Set_Position(vPlayerPos);
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

void CRabbit::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);


	Update_Collider();
	__super::Update_BodyCollider();

	m_pTransformCom->Gravity(fTimeDelta);

}

void CRabbit::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//if (m_bStun == false)
	//{
	//	m_pModelCom->Play_Animation(fTimeDelta);
	//}


#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif
}

HRESULT CRabbit::Render()
{
	__super::Render();

	return S_OK;
}

void CRabbit::Set_Damage(_int iDamage, _uint iDamageType)
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

		//m_bStun = true;
		m_fStunTime += (1.f / 60.f);
	}

	if (iDamageType == AT_Bow_Common || iDamageType == AT_Bow_Skill1)
	{

		_vec4 vDir = m_pTransformCom->Get_State(State::Pos) - __super::Compute_PlayerPos();

		m_pTransformCom->Go_To_Dir(vDir, m_fBackPower);
	}

	if (iDamageType == AT_Bow_Skill3)
	{

		m_pTransformCom->Set_Speed(0.5f);
	}
}

void CRabbit::Init_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	_float fDistance = __super::Compute_PlayerDistance();
	_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
	vDir.y = 0.f;

	if (m_iHP <= 0)
	{
	
		m_eCurState = STATE_DIE;
	}

	if (m_fStunTime >= 0.01f && m_fStunTime <= 0.5f)
	{
		m_bStun = true;
	}
	else
	{
		m_fStunTime = 0.f;
		m_bStun = false;
	}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CRabbit::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.2f;
			m_Animation.fInterpolationTime = 0.5f;

			m_pTransformCom->Set_Speed(3.f);
			m_bDamaged = false;
			break;

		case Client::CRabbit::STATE_ROAM:
			m_iRoamingPattern = rand() % 3;

			if (m_iRoamingPattern == 1 && m_pModelCom->IsAnimationFinished(RUN) == false)
			{
				random_device rd;
				_randNum RandNum(rd());
				_randFloat Random = _randFloat(-1.f, 1.f);
				m_pTransformCom->LookAt_Dir(_vec4(Random(RandNum), 0.f, Random(RandNum), 0.f));
			}

			switch (m_iRoamingPattern)
			{
			case 0:
				m_Animation.iAnimIndex = ROAR;
				m_Animation.isLoop = false;
				break;
			case 1:
				m_Animation.iAnimIndex = RUN;
				m_Animation.isLoop = false;
				break;
			case 2:
				m_Animation.iAnimIndex = STUN;
				m_Animation.isLoop = false;
				break;
			}

			break;

		case Client::CRabbit::STATE_CHASE:
			if (fDistance >= m_fAttackRange)
			{
				m_Animation.iAnimIndex = RUN;
			}

			m_Animation.isLoop = true;
			m_pTransformCom->Set_Speed(4.f);

			break;

		case Client::CRabbit::STATE_ATTACK:
			m_bDamaged = false;
			m_bAttacking = true;

			m_pTransformCom->LookAt_Dir(vDir);
			break;

		case Client::CRabbit::STATE_HIT:

			if (m_bHit == true)
			{
				m_Animation.iAnimIndex = KNOCKDOWN;
			}

			else
			{
				_uint iRandom = rand() % 2;
				switch (iRandom)
				{
				case 0:
					m_Animation.iAnimIndex = HIT_ADD_L;
					break;
				case 1:
					m_Animation.iAnimIndex = HIT_ADD_L;
					break;
				}
			}

			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CRabbit::STATE_DIE:
			m_Animation.iAnimIndex = DIE;
			m_Animation.isLoop = false;

			_uint iRandomExp = rand() % 100;
			CUI_Manager::Get_Instance()->Set_Exp_ByPercent(15.f + (_float)iRandomExp / 2.f * 0.1f);
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CRabbit::Tick_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	_float fDistance = __super::Compute_PlayerDistance();

	switch (m_eCurState)
	{
	case Client::CRabbit::STATE_IDLE:
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
				m_eCurState = STATE_ROAM;
				m_fIdleTime = 0.f;
			}

		}

		//if (fDistance <= m_fChaseRange)
		//{
		//	m_eCurState = STATE_CHASE;
		//}
	}
	break;

	case Client::CRabbit::STATE_ROAM:

		if (m_iRoamingPattern == 1)
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
		}

		if (m_pModelCom->IsAnimationFinished(m_Animation.iAnimIndex))
		{
			m_eCurState = STATE_IDLE;
		}

		break;

	case Client::CRabbit::STATE_CHASE:
	{
		_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
		vDir.y = 0.f;

		if (fDistance > m_fChaseRange && !m_bDamaged)
		{
			m_eCurState = STATE_IDLE;
			m_bAttacking = false;

			break;
		}

		if (fDistance <= m_fAttackRange)
		{
			m_eCurState = STATE_ATTACK;
			m_Animation.isLoop = true;
		}
		else
		{
			m_pTransformCom->LookAt_Dir(vDir);
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
	}
	break;

	case Client::CRabbit::STATE_ATTACK:

		if (!m_bSelectAttackPattern)
		{
			if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02))
			{
				m_iAttackPattern = rand() % 2;
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
				if (fAnimpos >= 45.f && fAnimpos <= 47.f && !m_bAttacked)
				{
					_uint iDamage = m_iSmallDamage / 2 - rand() % 20;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
			}
			break;

		case 1:
			m_Animation.iAnimIndex = ATTACK02;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 37.f && fAnimpos <= 39.f && !m_bAttacked)
				{
					_uint iDamage = m_iSmallDamage / 2 - rand() % 20;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				if (fAnimpos >= 52.f && fAnimpos <= 54.f && !m_bAttacked2)
				{
					_uint iDamage = m_iSmallDamage / 2 - rand() % 20;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked2 = true;
				}
			}
			break;
		}

		if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02))
		{
			m_eCurState = STATE_IDLE;
		}

		break;

	case Client::CRabbit::STATE_HIT:

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

	case Client::CRabbit::STATE_DIE:

		if (m_pModelCom->IsAnimationFinished(DIE))
		{
			m_fDeadTime += fTimeDelta;
		}

		break;
	}

}

HRESULT CRabbit::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(0.2f, 0.4f, 0.6f);
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
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 1.f / 2.f, 0.01f, 2.f);
	XMStoreFloat4x4(&ColDesc.matFrustum, matView * matProj);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CRabbit::Update_Collider()
{
	_mat Offset = _mat::CreateTranslation(0.f, 0.5f, 0.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

CRabbit* CRabbit::Create(_dev pDevice, _context pContext)
{
	CRabbit* pInstance = new CRabbit(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CRabbit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRabbit::Clone(void* pArg)
{
	CRabbit* pInstance = new CRabbit(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CRabbit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRabbit::Free()
{
	__super::Free();
	CEvent_Manager::Get_Instance()->Update_Quest(TEXT("로스크바의 부탁"));

}
