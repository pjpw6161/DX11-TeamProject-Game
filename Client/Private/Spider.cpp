#include "Spider.h"
#include "UI_Manager.h"

const _float CSpider::m_fChaseRange = 7.f;
const _float CSpider::m_fAttackRange = 4.f;

_uint CSpider::m_iSpiderID = 0;
_bool CSpider::m_bDirSelected[8] = { false };

CSpider::CSpider(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CSpider::CSpider(const CSpider& rhs)
	: CMonster(rhs)
{
}

HRESULT CSpider::Init_Prototype()
{
	return S_OK;
}

HRESULT CSpider::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Spider");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_Animation.iAnimIndex = SALEROBIA_RAGE;
	m_Animation.isLoop = false;
	m_Animation.bSkipInterpolation = false;

	m_eCurState = STATE_RAGE;

	m_iHP = 1000;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);


	CTransform* pGroarTransform = GET_TRANSFORM("Layer_Groar_Boss", LEVEL_VILLAGE);
	_vec4 vGroarPos = pGroarTransform->Get_State(State::Pos);
	//vGroarPos.y -= 0.1f;
	_vec4 vGroarLook = pGroarTransform->Get_State(State::Look).Get_Normalized();

	_vec4 vDir[8] = {};

	for (size_t i = 0; i < 8; i++)
	{
		vDir[i] = _vec4::Transform(vGroarLook, _mat::CreateRotationY(XMConvertToRadians(45.f * i)));
	}

	_uint iRandomDir = {};

	if (m_iSpiderID == 0)
	{
		iRandomDir = rand() % 8;
		m_bDirSelected[iRandomDir] = true;
	}
	else
	{
		iRandomDir = rand() % 8;
		while (m_bDirSelected[iRandomDir] == true)
		{
			iRandomDir = rand() % 8;
		}

		m_bDirSelected[iRandomDir] = true;
	}

	m_pTransformCom->Set_Position(_vec3(vGroarPos + 10 * vDir[iRandomDir]));
	m_pTransformCom->Set_Scale(_vec3(2.f, 2.f, 2.f));

	++m_iSpiderID;

	if (m_iSpiderID == 3)
	{
		m_iSpiderID = 0;
		
		for (auto& it : m_bDirSelected)
		{
			it = false;
		}
	}

	return S_OK;
}

void CSpider::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_E))
	{
		Set_Damage(0, AT_Sword_Common);
	}

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	Update_Collider();
	__super::Update_BodyCollider();

	__super::Tick(fTimeDelta);

}

void CSpider::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
#endif
}

HRESULT CSpider::Render()
{
	__super::Render();

	return S_OK;
}

void CSpider::Set_Damage(_int iDamage, _uint iDamageType)
{
	//m_iHP = 0;
	m_eCurState = STATE_DIE;

	//m_bDamaged = true;

	//_vec4 vPlayerPos = __super::Compute_PlayerPos();
	//m_pTransformCom->LookAt(vPlayerPos);

	//if (iDamageType == AT_Sword_Common || iDamageType == AT_Sword_Skill1 || iDamageType == AT_Sword_Skill2 ||
	//	iDamageType == AT_Sword_Skill3 || iDamageType == AT_Sword_Skill4 || iDamageType == AT_Bow_Skill2 || iDamageType == AT_Bow_Skill4)
	//{
	//	// 경직
	//	m_Animation.fAnimSpeedRatio = 1.f;
	//}

	//if (iDamageType == AT_Bow_Common || iDamageType == AT_Bow_Skill1)
	//{
	//	// 밀려나게
	//	_vec4 vDir = m_pTransformCom->Get_State(State::Pos) - __super::Compute_PlayerPos();
	//	m_pTransformCom->Go_To_Dir(vDir, m_fBackPower);

	//	m_Animation.fAnimSpeedRatio = 2.f;
	//}

	//if (iDamageType == AT_Bow_Skill3)
	//{
	//	// 이속 느려지게
	//	m_bSlow = true;
	//	m_Animation.fAnimSpeedRatio = 0.8f;
	//}
}

void CSpider::Init_State(_float fTimeDelta)
{
	CTransform* pGroarTransform = GET_TRANSFORM("Layer_Groar_Boss", LEVEL_VILLAGE);
	_vec4 vGroarPos = pGroarTransform->Get_State(State::Pos);

	//if (m_iHP <= 0)
	//{
	//	m_eCurState = STATE_SUICIDE;
	//}

	if (m_bAppeared == true)
	{
		m_pTransformCom->Gravity(fTimeDelta);
	}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CSpider::STATE_RAGE:
			m_Animation.iAnimIndex = SALEROBIA_RAGE;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CSpider::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;

			m_pTransformCom->Set_Speed(1.5f);
			break;

		case Client::CSpider::STATE_WALK:
			m_Animation.iAnimIndex = WALK;
			m_Animation.isLoop = false;
			{
				random_device rd;
				_randNum RandNum(rd());
				_randFloat Random = _randFloat(-1.f, 1.f);
				m_pTransformCom->LookAt_Dir(_vec4(Random(RandNum), 0.f, Random(RandNum), 0.f));
			}
			break;

		case Client::CSpider::STATE_CHASE:
			m_Animation.iAnimIndex = RUN;
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

		case Client::CSpider::STATE_ATTACK:
			m_bDamaged = false;
			m_Animation.fAnimSpeedRatio = 3.f;
			m_Animation.iAnimIndex = ROAR;
			m_Animation.isLoop = false;
			m_Animation.fDurationRatio = 0.819f;
			break;

		case Client::CSpider::STATE_HIT:
			//{
			//	_uint iHitPattern = rand() % 2;

			//	switch (iHitPattern)
			//	{
			//	case 0:
			//		m_Animation.iAnimIndex = L_HIT;
			//		m_Animation.isLoop = false;
			//		break;
			//	case 1:
			//		m_Animation.iAnimIndex = R_HIT;
			//		m_Animation.isLoop = false;
			//		break;
			//	}
			//}
			break;

		case Client::CSpider::STATE_SUICIDE:
		{
			m_Animation.iAnimIndex = KNOCKDOWN;
			m_Animation.fAnimSpeedRatio = 3.f;
			m_Animation.isLoop = false;
			m_Animation.fDurationRatio = 0.438f;

			_float fDistance = __super::Compute_PlayerDistance();

			if (fDistance <= 4.f)
			{
				m_pGameInstance->Attack_Player(nullptr, 10, MonAtt_Poison);
			}
		}

			break;

		case Client::CSpider::STATE_DIE:
			m_Animation.iAnimIndex = DIE;
			m_Animation.fAnimSpeedRatio = 3.f;
			m_Animation.isLoop = false;

			_uint iRandomExp = rand() % 100;
			CUI_Manager::Get_Instance()->Set_Exp_ByPercent(15.f + (_float)iRandomExp / 2.f * 0.1f);

			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CSpider::Tick_State(_float fTimeDelta)
{
	CTransform* pGroarTransform = GET_TRANSFORM("Layer_Groar_Boss", LEVEL_VILLAGE);
	_vec4 vGroarPos = pGroarTransform->Get_State(State::Pos);

	switch (m_eCurState)
	{
	case Client::CSpider::STATE_RAGE:

		if (m_pTransformCom->Get_State(State::Pos).y < vGroarPos.y + 0.2f)
		{
			if (m_iHP > 0 && !m_bAppeared)
			{
				m_pTransformCom->Go_Up(fTimeDelta * 0.1f);
			}
		}
		else if (m_pTransformCom->Get_State(State::Pos).y >= vGroarPos.y and m_pModelCom->IsAnimationFinished(SALEROBIA_RAGE))
		{
			m_bAppeared = true;

			PxCapsuleControllerDesc ControllerDesc{};
			ControllerDesc.height = 0.8f; // 높이(위 아래의 반구 크기 제외
			ControllerDesc.radius = 0.35f; // 위아래 반구의 반지름
			ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
			ControllerDesc.slopeLimit = cosf(PxDegToRad(10.f)); // 캐릭터가 오를 수 있는 최대 각도
			ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
			ControllerDesc.stepOffset = 0.f; // 캐릭터가 오를 수 있는 계단의 최대 높이

			m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

			m_eCurState = STATE_CHASE;
		}
		break;

	case Client::CSpider::STATE_IDLE:
	{
		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 2.f)
		{
			m_eCurState = STATE_WALK;
			m_fIdleTime = 0.f;
		}

		_float fDistance = __super::Compute_PlayerDistance();
		if (fDistance <= m_fChaseRange)
		{
			m_eCurState = STATE_CHASE;
		}
	}

	break;

	case Client::CSpider::STATE_WALK:
		m_pTransformCom->Go_Straight(fTimeDelta);

		if (m_pModelCom->IsAnimationFinished(WALK))
		{
			m_eCurState = STATE_IDLE;
		}

		break;

	case Client::CSpider::STATE_CHASE:
	{
		_vec4 vPlayerPos = __super::Compute_PlayerPos();
		_float fDistance = __super::Compute_PlayerDistance();
		_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
		vDir.y = 0.f;

		m_pTransformCom->LookAt_Dir(vDir);
		m_pTransformCom->Go_Straight(fTimeDelta);

		//if (fDistance > m_fChaseRange && !m_bDamaged)
		//{
		//	m_eCurState = STATE_IDLE;
		//	m_bSlow = false;
		//}

		if (fDistance <= m_fAttackRange)
		{
			m_eCurState = STATE_ATTACK;
			m_Animation.isLoop = false;
			m_bSlow = false;
		}
	}
	break;

	case Client::CSpider::STATE_ATTACK:

		if (m_pModelCom->IsAnimationFinished(ROAR))
		{
			m_eCurState = STATE_SUICIDE;
		}

		break;

	case Client::CSpider::STATE_HIT:

		if (m_pModelCom->IsAnimationFinished(m_Animation.iAnimIndex))
		{
			m_eCurState = STATE_CHASE;
		}

		break;

	case Client::CSpider::STATE_SUICIDE:

		if (m_pModelCom->IsAnimationFinished(KNOCKDOWN))
		{
			m_fDeadTime += fTimeDelta;
		}

		break;

	case Client::CSpider::STATE_DIE:
		
		if (m_pModelCom->IsAnimationFinished(DIE))
		{
			m_fDeadTime += fTimeDelta;
		}

		break;
	}
}

HRESULT CSpider::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(1.f, 0.3f, 1.f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
		return E_FAIL;

	return S_OK;
}

void CSpider::Update_Collider()
{
}

CSpider* CSpider::Create(_dev pDevice, _context pContext)
{
	CSpider* pInstance = new CSpider(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSpider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpider::Clone(void* pArg)
{
	CSpider* pInstance = new CSpider(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSpider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpider::Free()
{
	__super::Free();
}
