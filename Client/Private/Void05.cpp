#include "Void05.h"
#include "UI_Manager.h"
#include "Dead.h"

const _float CVoid05::m_fChaseRange = 7.f;
const _float CVoid05::m_fAttackRange = 2.3f;

_uint CVoid05::m_iIndex = 0;

CVoid05::CVoid05(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CVoid05::CVoid05(const CVoid05& rhs)
	: CMonster(rhs)
{
}

HRESULT CVoid05::Init_Prototype()
{
	return S_OK;
}

HRESULT CVoid05::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Void05");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	//m_Animation.iAnimIndex = TU02_SC02_MON_ATTACK_LOOP;
	//m_Animation.isLoop = true;
	//m_Animation.bSkipInterpolation = false;

	random_device rand;
	_randNum RandomNumber(rand());
	_randFloat RandomAnimPos(0.f, 1000.f);
	m_Animation.fStartAnimPos = RandomAnimPos(RandomNumber);

	m_eCurState = STATE_DIG;

	m_iHP = 1000;
	m_iDamageAccMax = 250;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	TRAIL_DESC Desc{};
	Desc.vColor = Colors::BurlyWood;
	Desc.vPSize = _vec2(0.03f, 0.03f);
	Desc.iNumVertices = 10;
	m_pLeftTrail = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);
	m_pRightTrail = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.1f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.8f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	m_MonsterHpBarPos = _vec3(0.f, 1.3f, 0.f);

	//if (pArg)
	{
		if (FAILED(__super::Init(pArg)))
		{
			return E_FAIL;
		}
	}

	++m_iIndex;

	return S_OK;
}

void CVoid05::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	Update_Collider();
	__super::Update_BodyCollider();

	Update_Trail(fTimeDelta);

	m_pTransformCom->Gravity(fTimeDelta);

}

void CVoid05::Late_Tick(_float fTimeDelta)
{
	m_pLeftTrail->Late_Tick(fTimeDelta);
	m_pRightTrail->Late_Tick(fTimeDelta);

	__super::Late_Tick(fTimeDelta);

	m_Animation.fStartAnimPos = 0.f;

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif
}

HRESULT CVoid05::Render()
{
	__super::Render();

	return S_OK;
}

void CVoid05::Set_Damage(_int iDamage, _uint iDamageType)
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
	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	m_pTransformCom->LookAt(vPlayerPos);

	if (iDamageType == AT_Sword_Common || iDamageType == AT_Sword_Skill1 || iDamageType == AT_Sword_Skill2 ||
		iDamageType == AT_Sword_Skill3 || iDamageType == AT_Sword_Skill4 || iDamageType == AT_Bow_Skill2 || iDamageType == AT_Bow_Skill4 || iDamageType == AT_Critical)
	{
		// 경직
		//m_Animation.fAnimSpeedRatio = 4.f;
	}

	if (iDamageType == AT_Bow_Common || iDamageType == AT_Bow_Skill1)
	{
		// 밀려나게
		_vec4 vDir = m_pTransformCom->Get_State(State::Pos) - __super::Compute_PlayerPos();
		m_pTransformCom->Go_To_Dir(vDir, m_fBackPower);

		//m_Animation.fAnimSpeedRatio = 3.f;
	}

	if (iDamageType == AT_Bow_Skill3)
	{
		// 이속 느려지게
		m_bSlow = true;
		//m_Animation.fAnimSpeedRatio = 0.8f;
	}
}

void CVoid05::Init_State(_float fTimeDelta)
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
		Desc.eDead = CDead::VOID05;
		Desc.vPos = m_pTransformCom->Get_State(State::Pos);
		m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Void05_Die"), TEXT("Prototype_GameObject_Dead"), &Desc);
	}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CVoid05::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;

			m_pTransformCom->Set_Speed(1.5f);
			break;

		case Client::CVoid05::STATE_DIG:
			m_Animation.iAnimIndex = TU02_SC02_MON_ATTACK_LOOP;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 3.f;

			break;

		case Client::CVoid05::STATE_CHASE:
		{
			_float fDistance = __super::Compute_PlayerDistance();
			if (fDistance >= m_fAttackRange)
			{
				m_Animation.iAnimIndex = RUN;
			}

			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;
			m_Animation.fInterpolationTime = 0.2f;
			m_Animation.fDurationRatio = 1.f;

			if (m_bSlow == true)
			{
				m_pTransformCom->Set_Speed(1.f);
			}
			else
			{
				m_pTransformCom->Set_Speed(4.f);
			}
		}
		break;

		case Client::CVoid05::STATE_ATTACK:
			m_bDamaged = false;
			m_pTransformCom->LookAt_Dir(vDir);

			break;

		case Client::CVoid05::STATE_HIT:
			m_Animation.iAnimIndex = KNOCKDOWN;
			m_Animation.isLoop = false;

			if (m_bHit == true)
			{
				m_Animation.fDurationRatio = 1.f;
				m_Animation.fAnimSpeedRatio = 3.f;
			}
			else
			{
				m_Animation.fDurationRatio = 0.026f;
				m_Animation.fAnimSpeedRatio = 2.f;
			}

			break;

		case Client::CVoid05::STATE_DIE:
			m_Animation.iAnimIndex = DIE;
			m_Animation.isLoop = false;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CVoid05::Tick_State(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CVoid05::STATE_IDLE:

		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 1.f)
		{
			m_fIdleTime = 0.f;
			m_eCurState = STATE_CHASE;
		}

		break;

	case Client::CVoid05::STATE_DIG:
		break;

	case Client::CVoid05::STATE_CHASE:
	{
		_vec4 vPlayerPos = __super::Compute_PlayerPos();
		_float fDistance = __super::Compute_PlayerDistance();
		_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
		vDir.y = 0.f;

		//if (fDistance > m_fChaseRange && !m_bDamaged)
		//{
		//	m_eCurState = STATE_IDLE;
		//	m_bSlow = false;
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
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
	}
		break;

	case Client::CVoid05::STATE_ATTACK:

		if (!m_bSelectAttackPattern)
		{
			if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
				m_pModelCom->IsAnimationFinished(ATTACK03))
			{
				m_iAttackPattern++;

				if (m_iAttackPattern == 3)
				{
					m_iAttackPattern = 0;
				}

				//m_iAttackPattern = rand() % 3;
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
				if (fAnimpos >= 48.f && fAnimpos <= 50.f && !m_bAttacked)
				{
					_uint iDamage = m_iSmallDamage / 2 + rand() % 10;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				if (fAnimpos >= 43.f && fAnimpos <= 52.f)
				{
					m_pLeftTrail->On();
					m_pRightTrail->On();
				}
			}
			break;

		case 1:
			m_Animation.iAnimIndex = ATTACK02;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 51.f && fAnimpos <= 53.f && !m_bAttacked)
				{
					_uint iDamage = m_iSmallDamage / 2 + rand() % 10;
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
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 22.f && fAnimpos <= 24.f && !m_bAttacked)
				{
					_uint iDamage = m_iSmallDamage / 2 + rand() % 10;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				if (fAnimpos >= 43.f && fAnimpos <= 45.f && !m_bAttacked2)
				{
					_uint iDamage = m_iSmallDamage / 2 + rand() % 10;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked2 = true;
				}
				if (fAnimpos >= 17.f && fAnimpos <= 50.f)
				{
					m_pLeftTrail->On();
					m_pRightTrail->On();
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

	case Client::CVoid05::STATE_HIT:

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

	case Client::CVoid05::STATE_DIE:

		if (m_pModelCom->IsAnimationFinished(DIE))
		{
			m_fDeadTime += fTimeDelta;
		}

		break;
	}
}

void CVoid05::Update_Trail(_float fTimeDelta)
{
	_mat LMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-L-Finger22");
	LMatrix *= m_pTransformCom->Get_World_Matrix();

	m_pLeftTrail->Tick(LMatrix.Position_vec3());

	_mat RMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-R-Finger22");
	RMatrix *= m_pTransformCom->Get_World_Matrix();

	m_pRightTrail->Tick(RMatrix.Position_vec3());
}

HRESULT CVoid05::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(0.5f, 0.5f, 1.1f);
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
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 1.5f, 0.01f, 2.f);
	XMStoreFloat4x4(&ColDesc.matFrustum, matView * matProj);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CVoid05::Update_Collider()
{
	_mat Offset = _mat::CreateTranslation(0.f, 0.5f, 0.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

CVoid05* CVoid05::Create(_dev pDevice, _context pContext)
{
	CVoid05* pInstance = new CVoid05(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVoid05");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVoid05::Clone(void* pArg)
{
	CVoid05* pInstance = new CVoid05(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVoid05");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVoid05::Free()
{
	__super::Free();

	Safe_Release(m_pLeftTrail);
	Safe_Release(m_pRightTrail);
}
