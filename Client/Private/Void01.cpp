#include "Void01.h"
#include "UI_Manager.h"
#include "Dead.h"

const _float CVoid01::m_fChaseRange = 7.f;
const _float CVoid01::m_fAttackRange = 2.5f;

CVoid01::CVoid01(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CVoid01::CVoid01(const CVoid01& rhs)
	: CMonster(rhs)
{
}

HRESULT CVoid01::Init_Prototype()
{
	return S_OK;
}

HRESULT CVoid01::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Void01");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_eCurState = STATE_READY;

	m_iHP = 1500;
	m_iDamageAccMax = 250;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	TRAIL_DESC Desc{};
	Desc.vColor = Colors::RosyBrown;
	Desc.vPSize = _vec2(0.05f, 0.05f);
	Desc.iNumVertices = 10;
	m_pLeftTrail = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);
	m_pRightTrail = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 1.2f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.4f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	m_MonsterHpBarPos = _vec3(0.f, 2.8f, 0.f);

	m_pTransformCom->Set_Position(_vec3(100.f, 5.f, 127.f));
	m_pTransformCom->LookAt_Dir(_vec4(0.f, 0.f, -1.f, 0.f));

	//if (pArg)
	{
		if (FAILED(__super::Init(pArg)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

void CVoid01::Tick(_float fTimeDelta)
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

void CVoid01::Late_Tick(_float fTimeDelta)
{
	m_pLeftTrail->Late_Tick(fTimeDelta);
	m_pRightTrail->Late_Tick(fTimeDelta);

	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif
}

HRESULT CVoid01::Render()
{
	//if (m_eCurState == STATE_READY)
	//{
	//	_vec4 vColor = Colors::Gold;
	//	if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &vColor, sizeof vColor)))
	//	{
	//		return E_FAIL;
	//	}
	//}

	__super::Render();

	return S_OK;
}

void CVoid01::Set_Damage(_int iDamage, _uint iDamageType)
{
	m_fHittedTime = 6.f;
	//m_iPassIndex = AnimPass_Default;

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
		//m_Animation.fAnimSpeedRatio = 1.f;
	}

	if (iDamageType == AT_Bow_Common || iDamageType == AT_Bow_Skill1)
	{
		// 밀려나게
		//_vec4 vDir = m_pTransformCom->Get_State(State::Pos) - __super::Compute_PlayerPos();
		//m_pTransformCom->Go_To_Dir(vDir, m_fBackPower);

		//m_Animation.fAnimSpeedRatio = 2.5f;
	}

	if (iDamageType == AT_Bow_Skill3)
	{
		// 이속 느려지게
		m_bSlow = true;
		//m_Animation.fAnimSpeedRatio = 0.8f;
	}
}

void CVoid01::Init_State(_float fTimeDelta)
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
		Desc.eDead = CDead::VOID01;
		Desc.vPos = m_pTransformCom->Get_State(State::Pos);
		m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Void01_Die"), TEXT("Prototype_GameObject_Dead"), &Desc);
	}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CVoid01::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;

			m_pTransformCom->Set_Speed(1.5f);
			break;

		case Client::CVoid01::STATE_READY:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;

			//m_iPassIndex = AnimPass_Rim;

			break;

		case Client::CVoid01::STATE_CHASE:
		{
			_float fDistance = __super::Compute_PlayerDistance();
			if (fDistance >= m_fAttackRange)
			{
				m_Animation.iAnimIndex = RUN;
			}

			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;

			if (m_bSlow == true)
			{
				m_pTransformCom->Set_Speed(1.f);
			}
			else
			{
				m_pTransformCom->Set_Speed(3.f);
			}
		}
		break;

		case Client::CVoid01::STATE_ATTACK:
			m_bDamaged = false;
			m_pTransformCom->LookAt_Dir(vDir);

			break;

		case Client::CVoid01::STATE_HIT:

			if (m_bHit == true)
			{
				m_Animation.iAnimIndex = KNOCKBACK;
			}

			else
			{
				_uint iRandom = rand() % 2;
				switch (iRandom)
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

		case Client::CVoid01::STATE_DIE:
			m_Animation.iAnimIndex = KNOCKBACK;
			m_Animation.isLoop = false;

			_uint iRandomExp = rand() % 100;
			CUI_Manager::Get_Instance()->Set_Exp_ByPercent(15.f + (_float)iRandomExp / 2.f * 0.1f);

			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CVoid01::Tick_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	_float fDistance = __super::Compute_PlayerDistance();
	_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
	vDir.y = 0.f;

	switch (m_eCurState)
	{
	case Client::CVoid01::STATE_IDLE:

		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 1.f)
		{
			m_fIdleTime = 0.f;
			m_eCurState = STATE_CHASE;
		}

		break;

	case Client::CVoid01::STATE_READY:
		break;

	case Client::CVoid01::STATE_CHASE:

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
		break;

	case Client::CVoid01::STATE_ATTACK:

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
				if (fAnimpos >= 87.f && fAnimpos <= 89.f && !m_bAttacked)
				{
					_uint iDamage = m_iSmallDamage + rand() % 20;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				//if (fAnimpos >= 18.f && fAnimpos <= 23.f)
				//{
				//	m_pLeftTrail->On();
				//	m_pRightTrail->On();
				//}
			}
			break;
		case 1:
			m_Animation.iAnimIndex = ATTACK02;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 68.f && fAnimpos <= 70.f && !m_bAttacked)
				{
					_uint iDamage = m_iSmallDamage + rand() % 20;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				if (fAnimpos >= 40.f && fAnimpos <= 75.f)
				{
					m_pLeftTrail->On();
					m_pRightTrail->On();
				}
			}
			break;
		case 2:
			m_Animation.iAnimIndex = ATTACK03;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 44.f && fAnimpos <= 46.f && !m_bAttacked)
				{
					_uint iDamage = m_iSmallDamage + rand() % 20;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				if (fAnimpos >= 42.f && fAnimpos <= 50.f)
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

	case Client::CVoid01::STATE_HIT:

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

	case Client::CVoid01::STATE_DIE:

		if (m_pModelCom->IsAnimationFinished(KNOCKBACK))
		{
			m_fDeadTime += fTimeDelta;
		}

		break;
	}
}

void CVoid01::Update_Trail(_float fTimeDelta)
{
	_mat LMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-L-Finger21");
	LMatrix *= m_pModelCom->Get_PivotMatrix();
	LMatrix *= m_pTransformCom->Get_World_Matrix();

	m_pLeftTrail->Tick(LMatrix.Position_vec3());

	_mat RMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-R-Finger21");
	RMatrix *= m_pModelCom->Get_PivotMatrix();
	RMatrix *= m_pTransformCom->Get_World_Matrix();

	m_pRightTrail->Tick(RMatrix.Position_vec3());
}

HRESULT CVoid01::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(0.3f, 0.8f, 0.3f);
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

void CVoid01::Update_Collider()
{
	_mat Offset = _mat::CreateTranslation(0.f, 0.8f, 0.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

CVoid01* CVoid01::Create(_dev pDevice, _context pContext)
{
	CVoid01* pInstance = new CVoid01(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVoid01");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVoid01::Clone(void* pArg)
{
	CVoid01* pInstance = new CVoid01(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVoid01");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVoid01::Free()
{
	__super::Free();

	Safe_Release(m_pLeftTrail);
	Safe_Release(m_pRightTrail);
}
