#include "Groar_Boss.h"

#include "Missile.h"
#include "HPBoss.h"
#include "3DUITex.h"
#include "UI_Manager.h"
#include "Event_Manager.h"
#include "TextButton.h"
#include "DialogText.h"
#include "TextButtonColor.h"
#include "Trigger_Manager.h"
#include "HitEffect.h"
#include "Camera_Manager.h"

const _float CGroar_Boss::m_fChaseRange = 10.f;
const _float CGroar_Boss::m_fAttackRange = 6.f;

CGroar_Boss::CGroar_Boss(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CGroar_Boss::CGroar_Boss(const CGroar_Boss& rhs)
	: CGameObject(rhs)
{
}

HRESULT CGroar_Boss::Init_Prototype()
{
	return S_OK;
}

HRESULT CGroar_Boss::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_Position(_vec3(2109.f, -15.f, 2092.f));
	PxRaycastBuffer RayBuff{};
	if (m_pGameInstance->Raycast(m_pTransformCom->Get_State(State::Pos), _vec4(0.f, -1.f, 0.f, 0.f), 50.f, RayBuff))
	{
		m_pTransformCom->Set_Position(_vec3(PxVec3ToVector(RayBuff.block.position)));
	}

	m_pTransformCom->LookAt_Dir(_vec4(-1.f, 0.f, 0.f, 0.f));
	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	m_eCurState = STATE_NPC;

	m_Animation.iAnimIndex = NPC_IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 1.5f;

	m_iHP = 10000;

	if (FAILED(Init_Dialog()))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CGroar_Boss::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_OldMatrix();

	//if (m_pGameInstance->Key_Down(DIK_Q, InputChannel::UI)) // 괴물들 잡아달라 하고 보스방으로 순간이동 하는 타이밍(한번만 들어와야 함)
	if (m_strQuestOngoing == TEXT("그로아를 지켜라") || m_pGameInstance->Key_Down(DIK_Q, InputChannel::UI))
	{
		if (!m_bChangePos)
		{
			m_pTransformCom->Set_Position(_vec3(2179.f, -20.f, 2083.f));
			PxRaycastBuffer RayBuff{};
			if (m_pGameInstance->Raycast(m_pTransformCom->Get_State(State::Pos), _vec4(0.f, -1.f, 0.f, 0.f), 50.f, RayBuff))
			{
				m_pTransformCom->Set_Position(_vec3(PxVec3ToVector(RayBuff.block.position)));
			}

			//CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
			//_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
			//pPlayerTransform->Set_Position(vPlayerPos + _vec3(100.f));

			for (size_t i = 0; i < 2; i++)
			{
				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Void09"), TEXT("Prototype_GameObject_Void09"))))
				{

				}
			}

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Void20"), TEXT("Prototype_GameObject_Void20"))))
			{

			}
			//pPlayerTransform->Set_Position(vPlayerPos);

			m_bChangePos = true;
		}
	}

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	Update_Collider();

	if (m_pHpBoss != nullptr)
	{
		m_pHpBoss->Tick(fTimeDelta);
	}

	m_pTransformCom->Gravity(fTimeDelta);
}

void CGroar_Boss::Late_Tick(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CGroar_Boss::STATE_NPC:
		NPC_LateTick(fTimeDelta);
		m_pNPCModelCom->Play_Animation(fTimeDelta);
		break;
	case Client::CGroar_Boss::STATE_SCENE01:
		m_pScene01ModelCom->Play_Animation(fTimeDelta);
		break;
	case Client::CGroar_Boss::STATE_SCENE02:
		m_pScene02ModelCom->Play_Animation(fTimeDelta);
		break;
	case Client::CGroar_Boss::STATE_BOSS:
		m_pBossModelCom->Play_Animation(fTimeDelta);
		break;
	}

	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif
	if (m_pHpBoss != nullptr)
	{
		m_pHpBoss->Late_Tick(fTimeDelta);
	}

}

HRESULT CGroar_Boss::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	CModel* pModel = { nullptr };

	switch (m_eCurState)
	{
	case Client::CGroar_Boss::STATE_NPC:
		pModel = m_pNPCModelCom;
		break;
	case Client::CGroar_Boss::STATE_SCENE01:
		pModel = m_pScene01ModelCom;
		break;
	case Client::CGroar_Boss::STATE_SCENE02:
		pModel = m_pScene02ModelCom;
		break;
	case Client::CGroar_Boss::STATE_BOSS:
		pModel = m_pBossModelCom;
		break;
	}

	for (_uint i = 0; i < pModel->Get_NumMeshes(); i++)
	{
		if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
		}

		_bool HasNorTex{};
		if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		_bool HasMaskTex{};
		if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_MaskTexture", i, TextureType::Shininess)))
		{
			HasMaskTex = false;
		}
		else
		{
			HasMaskTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(pModel->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(m_iPassIndex)))
		{
			return E_FAIL;
		}

		if (FAILED(pModel->Render(i)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

void CGroar_Boss::Set_Damage(_int iDamage, _uint iDamageType)
{
	if (m_eCurState == STATE_BOSS)
	{
		m_iHP -= iDamage;
		m_bChangePass = true;

		m_pHpBoss->Set_HP(m_iHP);

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
	}	
	
	//if (iDamageType == AT_Sword_Common || iDamageType == AT_Sword_Skill1 || iDamageType == AT_Sword_Skill2 ||
	//	iDamageType == AT_Sword_Skill3 || iDamageType == AT_Sword_Skill4 || iDamageType == AT_Bow_Skill2 || iDamageType == AT_Bow_Skill4)
	//{
	//	// 경직
	//	//m_Animation.fAnimSpeedRatio = 4.f;
	//}

	//if (iDamageType == AT_Bow_Common || iDamageType == AT_Bow_Skill1)
	//{
	//	// 밀려나게
	//	_vec4 vDir = m_pTransformCom->Get_State(State::Pos) - __super::Compute_PlayerPos();
	//	m_pTransformCom->Go_To_Dir(vDir, m_fBackPower);

	//	//m_Animation.fAnimSpeedRatio = 3.f;
	//}

	//if (iDamageType == AT_Bow_Skill3)
	//{
	//	// 이속 느려지게
	//	m_bSlow = true;
	//	//m_Animation.fAnimSpeedRatio = 0.8f;
	//}
}

void CGroar_Boss::Init_State(_float fTimeDelta)
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
	_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
	vDir.y = 0.f;

	if (m_bChangePass == true)
	{
		m_fHitTime += fTimeDelta;

		if (m_iPassIndex == AnimPass_Default)
		{
			m_iPassIndex = AnimPass_Rim;
		}
		else
		{
			m_iPassIndex = AnimPass_Default;
		}

		if (m_fHitTime >= 0.3f)
		{
			m_fHitTime = 0.f;
			m_bChangePass = false;
			m_iPassIndex = AnimPass_Default;
		}
	}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CGroar_Boss::STATE_NPC:
			break;

		case Client::CGroar_Boss::STATE_SCENE01:
			m_Animation.iAnimIndex = 0;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 1.5f;

			break;

		case Client::CGroar_Boss::STATE_SCENE02:
			m_Animation.iAnimIndex = 0;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CGroar_Boss::STATE_BOSS:

			CHPBoss::HPBOSS_DESC Desc{};
			Desc.strName = L"Groar";
			Desc.eLevelID = LEVEL_STATIC;
			Desc.iMaxHp = m_iHP;

			m_pHpBoss = (CHPBoss*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_HPBoss"), &Desc);
			if (not m_pHpBoss)
			{
				return;
			}
			_vec3 vPos = m_pTransformCom->Get_State(State::Pos);

			CUI_Manager::Get_Instance()->Set_Symbol(CSymbol::GROAR);

			m_pTransformCom->Set_Position(_vec3(0.f));

			m_pTransformCom->Delete_Controller();

			PxCapsuleControllerDesc ControllerDesc{};
			ControllerDesc.height = 2.f; // 높이(위 아래의 반구 크기 제외
			ControllerDesc.radius = 2.2f; // 위아래 반구의 반지름
			ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
			ControllerDesc.slopeLimit = cosf(PxDegToRad(0.1f)); // 캐릭터가 오를 수 있는 최대 각도
			ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
			ControllerDesc.stepOffset = 0.1f; // 캐릭터가 오를 수 있는 계단의 최대 높이

			m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);
			
			m_pTransformCom->Set_Position(vPos + _vec3(10.f, 0.6f, 0.f));
			m_pTransformCom->LookAt_Dir(_vec4(-1.f, 0.f, 0.f, 0.f));

			break;
		}

		m_ePreState = m_eCurState;
	}

	if (m_iHP <= 0)
	{
		m_eBossCurState = BOSS_STATE_DIE;
	}

	if (m_eBossPreState != m_eBossCurState)
	{
		switch (m_eBossCurState)
		{
		case Client::CGroar_Boss::BOSS_STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;

			break;

		case Client::CGroar_Boss::BOSS_STATE_ROAR:
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK_RAGE;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			m_pTransformCom->LookAt_Dir(vDir);
			break;

		case Client::CGroar_Boss::BOSS_STATE_CHASE:
			m_Animation.iAnimIndex = RUN;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;

			m_bSelectAttackPattern = false;

			m_bCreateMissile = false;
			m_bCreateSpider = false;

			m_bAttacked1 = false;
			m_bAttacked2 = false;
			m_bAttacked3 = false;

			break;

		case Client::CGroar_Boss::BOSS_STATE_THROW_ATTACK: // 00, 01

			if (m_iThrowAttackCombo % 2 == 0)
			{
				m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK00;
			}
			else
			{
				m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK01;
			}

			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			break;

		case Client::CGroar_Boss::BOSS_STATE_SIX_MISSILE: // 02
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK02;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CGroar_Boss::BOSS_STATE_TAKE_DOWN: // 03
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK03;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			break;

		case Client::CGroar_Boss::BOSS_STATE_SPIDER: // 05
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK05;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CGroar_Boss::BOSS_STATE_TENTACLE: // 06
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK06;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 1.f;

			break;

		case Client::CGroar_Boss::BOSS_STATE_XBEAM: // 07
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK07;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CGroar_Boss::BOSS_STATE_RAGE: // 08
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK08;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CGroar_Boss::BOSS_STATE_DIE:
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_DIE;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			_uint iRandomExp = rand() % 100;
			CUI_Manager::Get_Instance()->Set_Exp_ByPercent(1000.f + (_float)iRandomExp / 2.f * 0.1f);

			break;
		}

		m_eBossPreState = m_eBossCurState;
	}
}

void CGroar_Boss::Tick_State(_float fTimeDelta)
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
	_float fDistance = (vPlayerPos - vPos).Length();

	_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
	vDir.y = 0.f;

	switch (m_eCurState)
	{
	case Client::CGroar_Boss::STATE_NPC:
		NPC_Tick(fTimeDelta);
		m_pNPCModelCom->Set_Animation(m_Animation);
		break;

	case Client::CGroar_Boss::STATE_SCENE01:
		if (m_pScene01ModelCom->IsAnimationFinished(0))
		{
			m_eCurState = STATE_SCENE02;
			CTrigger_Manager::Get_Instance()->Set_AfterSuicide();
		}

		m_pScene01ModelCom->Set_Animation(m_Animation);
		break;

	case Client::CGroar_Boss::STATE_SCENE02:
		if (m_pScene02ModelCom->IsAnimationFinished(0))
		{
			m_eCurState = STATE_BOSS;
			m_eBossCurState = BOSS_STATE_ROAR;

			m_Animation.iAnimIndex = 0;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;
		}
		else
		{
			m_Animation.fAnimSpeedRatio = 4.5f;
		}

		m_pScene02ModelCom->Set_Animation(m_Animation);
		break;

	case Client::CGroar_Boss::STATE_BOSS:
		m_pBossModelCom->Set_Animation(m_Animation);
		break;
	}

	switch (m_eBossCurState)
	{
	case Client::CGroar_Boss::BOSS_STATE_IDLE:

		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 3.f)
		{
			m_eBossCurState = BOSS_STATE_CHASE;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_ROAR:

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 95.f && m_pBossModelCom->Get_CurrentAnimPos() <= 175.f)
		{
			CCamera_Manager::Get_Instance()->Set_ShakeCam(true, 0.5f);
			m_pGameInstance->FadeoutSound(0, fTimeDelta, 1.f, false);
		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK_RAGE))
		{
			m_eBossCurState = BOSS_STATE_IDLE;
			CTrigger_Manager::Get_Instance()->Set_BossStart();
			m_pGameInstance->PlayBGM(TEXT("BGM_GuildDungeon_01"));
			m_pGameInstance->FadeinSound(0, fTimeDelta);
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_CHASE:
	{
		GROAR_BOSS_STATE eTempBossCurState = { BOSS_STATE_END };

		if (!m_bSelectAttackPattern)
		{
			_bool bReset = { true };
			for (size_t i = 0; i < ATTACK_END; i++)
			{
				if (m_bAttack_Selected[i] == false)
				{
					bReset = false;
					break;
				}
			}

			if (bReset == true)
			{
				for (auto& it : m_bAttack_Selected)
				{
					it = false;
				}
			}

			if (m_bAttack_Selected[ATTACK_THROW] == true && m_bAttack_Selected[ATTACK_XBEAM] == false)
			{
				m_eBossCurState = BOSS_STATE_XBEAM;

				if (m_iThrowAttackCombo > 0)
				{
					eTempBossCurState = BOSS_STATE_THROW_ATTACK;
				}

				m_bSelectAttackPattern = true;

				break;
			}

			GROAR_ATTACK eAttackRandom = ATTACK_END;
			eAttackRandom = static_cast<GROAR_ATTACK>(rand() % ATTACK_XBEAM);

			// 랜덤 방지용
			while (m_bAttack_Selected[eAttackRandom] == true)
			{
				eAttackRandom = static_cast<GROAR_ATTACK>(rand() % ATTACK_XBEAM);
			}

			switch (eAttackRandom)
			{
			case Client::CGroar_Boss::ATTACK_THROW:
				eTempBossCurState = BOSS_STATE_THROW_ATTACK;
				break;
			case Client::CGroar_Boss::ATTACK_SIX_MISSILE:
				eTempBossCurState = BOSS_STATE_SIX_MISSILE;
				break;
			case Client::CGroar_Boss::ATTACK_TAKE_DOWN:
				eTempBossCurState = BOSS_STATE_TAKE_DOWN;
				break;
			case Client::CGroar_Boss::ATTACK_SPIDER:
				eTempBossCurState = BOSS_STATE_SPIDER;
				break;
			case Client::CGroar_Boss::ATTACK_TENTACLE:
				eTempBossCurState = BOSS_STATE_TENTACLE;
				break;
			case Client::CGroar_Boss::ATTACK_XBEAM:
				eTempBossCurState = BOSS_STATE_XBEAM;
				break;
			case Client::CGroar_Boss::ATTACK_YELL:
				eTempBossCurState = BOSS_STATE_RAGE;
				break;
			}

			if (m_iThrowAttackCombo > 0)
			{
				eTempBossCurState = BOSS_STATE_THROW_ATTACK;
			}

			m_bSelectAttackPattern = true;
		}

		if (eTempBossCurState == BOSS_STATE_THROW_ATTACK || eTempBossCurState == BOSS_STATE_SIX_MISSILE || eTempBossCurState == BOSS_STATE_XBEAM)
		{
			if (fDistance >= m_fChaseRange)
			{
				m_pTransformCom->LookAt_Dir(vDir);
				m_pTransformCom->Go_Straight(fTimeDelta);
				m_bSelectAttackPattern = false;
			}
			else
			{
				m_eBossCurState = eTempBossCurState;
			}

		}
		else
		{
			if (fDistance >= m_fAttackRange)
			{
				m_pTransformCom->LookAt_Dir(vDir);
				m_pTransformCom->Go_Straight(fTimeDelta);
				m_bSelectAttackPattern = false;
			}
			else
			{
				m_eBossCurState = eTempBossCurState;
			}
		}

		//m_eBossCurState = BOSS_STATE_SPIDER; // 테스트용
	}

	break;

	case Client::CGroar_Boss::BOSS_STATE_THROW_ATTACK:

		if (m_pBossModelCom->Get_CurrentAnimationIndex() == MON_GROAR_ASGARD_ATTACK00)
		{
			if (m_pBossModelCom->Get_CurrentAnimPos() >= 10.f && !m_bCreateMissile)
			{
				CMissile::MISSILE_TYPE eType = CMissile::RIGHT_THROW;
				m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Missile"), TEXT("Prototype_GameObject_Missile"), &eType);
				m_bCreateMissile = true;

				++m_iThrowAttackCombo;
			}

			if (m_pBossModelCom->Get_CurrentAnimPos() < 10.f)
			{
				m_bCreateMissile = false;
			}

			if (m_pBossModelCom->Get_CurrentAnimPos() <= 38.f)
			{
				m_pTransformCom->LookAt_Dir(vDir);
			}

		}

		else if (m_pBossModelCom->Get_CurrentAnimationIndex() == MON_GROAR_ASGARD_ATTACK01)
		{
			if (m_pBossModelCom->Get_CurrentAnimPos() >= 15.f && !m_bCreateMissile)
			{
				CMissile::MISSILE_TYPE eType = CMissile::LEFT_THROW;
				m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Missile"), TEXT("Prototype_GameObject_Missile"), &eType);
				m_bCreateMissile = true;

				++m_iThrowAttackCombo;
			}

			if (m_pBossModelCom->Get_CurrentAnimPos() < 15.f)
			{
				m_bCreateMissile = false;
			}

			if (m_pBossModelCom->Get_CurrentAnimPos() <= 51.f)
			{
				m_pTransformCom->LookAt_Dir(vDir);
			}

		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK00) || m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK01))
		{
			if (m_iThrowAttackCombo >= 3)
			{
				m_iThrowAttackCombo = 0;
				m_bAttack_Selected[ATTACK_THROW] = true;
			}

			m_eBossCurState = BOSS_STATE_CHASE;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_SIX_MISSILE:

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 60.f && !m_bCreateMissile)
		{
			for (size_t i = 0; i < 6; i++)
			{
				CMissile::MISSILE_TYPE eType = CMissile::SIX_MISSILE;
				m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Missile"), TEXT("Prototype_GameObject_Missile"), &eType);
			}

			m_bCreateMissile = true;
		}

		if (m_pBossModelCom->Get_CurrentAnimPos() <= 100.f)
		{
			m_pTransformCom->LookAt_Dir(vDir);
		}

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 99.f && m_pBossModelCom->Get_CurrentAnimPos() <= 110.f)
		{
			CCamera_Manager::Get_Instance()->Set_ShakeCam(true);
		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK02))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
			m_bAttack_Selected[ATTACK_SIX_MISSILE] = true;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_TAKE_DOWN:

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 69.f && m_pBossModelCom->Get_CurrentAnimPos() <= 72.f)
		{
			CCamera_Manager::Get_Instance()->Set_ShakeCam(true);

			if (fDistance <= 7.5f)
			{
				if (!m_bAttacked1)
				{
					_uint iDamage = 50 + rand() % 20;
					m_pGameInstance->Attack_Player(nullptr, iDamage, MonAtt_Hit);
					m_bAttacked1 = true;
				}
			}
		}

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 97.f && m_pBossModelCom->Get_CurrentAnimPos() <= 100.f)
		{
			CCamera_Manager::Get_Instance()->Set_ShakeCam(true);

			if (fDistance <= 7.5f)
			{
				if (!m_bAttacked2)
				{
					_uint iDamage = 50 + rand() % 20;
					m_pGameInstance->Attack_Player(nullptr, iDamage, MonAtt_Hit);
					m_bAttacked2 = true;
				}
			}
		}

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 135.f && m_pBossModelCom->Get_CurrentAnimPos() <= 138.f)
		{
			CCamera_Manager::Get_Instance()->Set_ShakeCam(true);

			if (fDistance <= 7.5f)
			{
				if (!m_bAttacked3)
				{
					_uint iDamage = 80 + rand() % 20;
					m_pGameInstance->Attack_Player(nullptr, iDamage, MonAtt_KnockDown);
					m_bAttacked3 = true;
				}
			}
		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK03))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
			m_bAttack_Selected[ATTACK_TAKE_DOWN] = true;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_SPIDER:

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 80.f && !m_bCreateSpider)
		{
			for (size_t i = 0; i < 3; i++)
			{
				m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Spider"), TEXT("Prototype_GameObject_Spider"));
			}

			m_bCreateSpider = true;
		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK05))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
			m_bAttack_Selected[ATTACK_SPIDER] = true;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_TENTACLE:
		if (m_pBossModelCom->Get_CurrentAnimPos() >= 48.f)
		{
			m_fTentacleTime += fTimeDelta;
		}

		if (m_fTentacleTime >= 0.7f)
		{
			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Tentacle"), TEXT("Prototype_GameObject_Tentacle"));
			m_fTentacleTime = 0.f;
		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK06))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
			m_bAttack_Selected[ATTACK_TENTACLE] = true;
			m_fTentacleTime = 0.f;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_XBEAM:

		if (m_pBossModelCom->Get_CurrentAnimPos() < 102.f)
		{
			m_pTransformCom->LookAt_Dir(vDir);
		}

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 102.f && !m_bCreateXBeam)
		{
			_vec3 vPos = m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 3.f, 0.f);
			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_XBeam"), TEXT("Prototype_GameObject_XBeam"), &vPos);

			m_bCreateXBeam = true;
		}

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 108.f && m_pBossModelCom->Get_CurrentAnimPos() <= 115.f)
		{
			CCamera_Manager::Get_Instance()->Set_ShakeCam(true);
		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK07))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
			m_bAttack_Selected[ATTACK_XBEAM] = true;
			m_bCreateXBeam = false;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_RAGE:

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 95.f && m_pBossModelCom->Get_CurrentAnimPos() <= 175.f)
		{
			CCamera_Manager::Get_Instance()->Set_ShakeCam(true, 0.5f);
		}

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 98.f)
		{
			m_fRageTime += fTimeDelta;

			if (m_fRageTime >= 0.25f)
			{
				if (fDistance <= 7.5f)
				{
					_uint iDamage = 20 + rand() % 10;
					m_pGameInstance->Attack_Player(nullptr, iDamage, MonAtt_Stun);
				}

				m_fRageTime = 0.f;
			}
		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK08))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
			m_bAttack_Selected[ATTACK_YELL] = true;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_DIE:
		break;
	}
}

HRESULT CGroar_Boss::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(2.f, 2.f, 2.f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
	{
		return E_FAIL;
	}

	// Frustum
	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Frustum;
	_matrix matView = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	// 1인자 : 절두체 각도(범위), 2인자 : Aspect, 3인자 : Near, 4인자 : Far(절두체 깊이)
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), 1.f, 0.01f, 3.f);
	XMStoreFloat4x4(&ColDesc.matFrustum, matView * matProj);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

	//PxCapsuleControllerDesc ControllerDesc{};
	//ControllerDesc.height = 0.8f; // 높이(위 아래의 반구 크기 제외
	//ControllerDesc.radius = 0.6f; // 위아래 반구의 반지름
	//ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	//ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	//ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	//ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	//m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	return S_OK;
}

void CGroar_Boss::Update_Collider()
{
	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());
	m_pNpcColliderCom->Update(m_pTransformCom->Get_World_Matrix());

	_mat Offset = _mat::CreateTranslation(0.f, 2.f, 1.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

HRESULT CGroar_Boss::Init_Dialog()
{
	for (size_t i = 0; i < size(m_strLines); i++)
	{
		m_vecDialog.push_back(m_strLines[i]);
	}

	m_vecChatt.push_back(TEXT("제 남편은 어디에 있나요.."));
	m_vecChatt.push_back(TEXT("신을 저주한다"));

	m_TalkSounds.push_back(TEXT("10044_3_EndTalk"));
	m_TalkSounds.push_back(TEXT("10043_1_StartTalk_1"));
	m_TalkSounds.push_back(TEXT("10043_1_StartTalk_2"));
	m_TalkSounds.push_back(TEXT("10053_3_EndTalk_cut"));
	m_TalkSounds.push_back(TEXT("10054_1_StartTalk_1_cut"));
	m_TalkSounds.push_back(TEXT("10056_1_StartTalk_1"));
	m_TalkSounds.push_back(TEXT("10056_1_StartTalk_2"));
	m_TalkSounds.push_back(TEXT("10056_2_InformTalk"));
	m_TalkSounds.push_back(TEXT("10058_2_InformTalk_1"));
	m_TalkSounds.push_back(TEXT("10058_2_InformTalk_2"));

	return S_OK;
}

HRESULT CGroar_Boss::Add_Parts()
{
	_float fDepth = (_float)D_TALK / (_float)D_END;
	CTextButton::TEXTBUTTON_DESC ButtonDesc = {};
	ButtonDesc.eLevelID = LEVEL_STATIC;
	ButtonDesc.fDepth = fDepth - 0.01f;
	ButtonDesc.fFontSize = 0.45f;
	ButtonDesc.strText = TEXT("");
	ButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ButtonDesc.vTextPosition = _vec2(20.f, 0.f);
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_ClickArrow");
	ButtonDesc.vPosition = _vec2(1100.f, 600.f);
	ButtonDesc.vSize = _vec2(30.f, 30.f);

	m_pArrow = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pArrow)
	{
		return E_FAIL;
	}
	ButtonDesc.vTextPosition = _vec2(40.f, 0.f);
	ButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ButtonDesc.vTextBorderColor = _vec4(0.3f, 0.75f, 0.87f, 1.f);
	ButtonDesc.strText = TEXT("SKIP");
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BlueArrow");
	ButtonDesc.vPosition = _vec2(1180.f, 40.f);
	ButtonDesc.vSize = _vec2(30.f, 30.f);

	m_pSkipButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pSkipButton)
	{
		return E_FAIL;
	}	
	
	ButtonDesc.vTextBorderColor = _vec4(0.f, 0.f, 0.f, 1.f);
	ButtonDesc.vTextColor = _vec4(1.f, 0.95f, 0.87f, 1.f);
	ButtonDesc.strText = TEXT("그로아");
	ButtonDesc.fFontSize = 0.5f;
	ButtonDesc.vTextPosition = _vec2(0.f, -30.f);
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_SiegeQuest");
	ButtonDesc.vPosition = _vec2((_float)g_ptCenter.x, 590.f);
	ButtonDesc.vSize = _vec2(400.f, 10.f);

	m_pLine = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pLine)
	{
		return E_FAIL;
	}

	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = fDepth;
	ColButtonDesc.fAlpha = 0.8f;
	ColButtonDesc.fFontSize = 0.f;
	ColButtonDesc.strText = TEXT("");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_FadeBox");
	ColButtonDesc.vSize = _vec2(g_iWinSizeX, 250.f);
	ColButtonDesc.vPosition = _vec2((_float)g_ptCenter.x, (_float)g_iWinSizeY);


	m_pBackGround = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pBackGround)
	{
		return E_FAIL;
	}
	m_pBackGround->Set_Pass(VTPass_FadeVertical);


	C3DUITex::UITEX_DESC TexDesc = {};
	TexDesc.eLevelID = LEVEL_STATIC;
	TexDesc.pParentTransform = m_pTransformCom;
	TexDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_SpeechBubble");
	TexDesc.vPosition = _vec3(0.f, 2.2f, 0.f);
	TexDesc.vSize = _vec2(40.f, 40.f);

	m_pSpeechBubble = (C3DUITex*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_3DUITex"), &TexDesc);
	if (not m_pSpeechBubble)
	{
		return E_FAIL;
	}

	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::AABB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(10.f, 1.f, 10.f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.9f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Roskva"), (CComponent**)&m_pNpcColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

void CGroar_Boss::NPC_Tick(_float fTimeDelta)
{
	if (m_bTalking == true)
	{
		if (m_pArrow->Get_TransPosition().y < m_pArrow->Get_Position().y - 5.f)
		{
			m_fDir = 0.6f;
			m_pArrow->Set_Position(_float2(m_pArrow->Get_Position().x, m_pArrow->Get_Position().y - 5.f));
		}
		if (m_pArrow->Get_TransPosition().y > m_pArrow->Get_Position().y)
		{
			m_fDir = -1.f;
			m_pArrow->Set_Position(_float2(m_pArrow->Get_Position().x, m_pArrow->Get_Position().y));
		}
		m_fButtonTime += fTimeDelta * m_fDir * 10.f;
		m_pArrow->Set_Position(_float2(m_pArrow->Get_Position().x, m_fButtonTime));
	}

	if (m_bTalking == true)
	{
		CUI_Manager::Get_Instance()->Set_FullScreenUI(true);
		if (m_pGameInstance->Mouse_Down(DIM_RBUTTON, InputChannel::UI))
		{
			if (m_eState != NPC_TALK)
			{
				CCamera_Manager::Get_Instance()->Set_CameraState(CS_ENDFULLSCREEN);
				CUI_Manager::Get_Instance()->Set_FullScreenUI(false);
				m_bTalking = false;
				return;
			}
			Set_Text(m_eState);
		}
	}
	else
	{
		m_pSpeechBubble->Tick(fTimeDelta);
		m_Animation.iAnimIndex = NPC_IDLE;
	}

	CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Player_Hit_OBB"));
	if (pCollider == nullptr)
	{
		return;
	}

	_bool isColl = m_pNpcColliderCom->Intersect(pCollider);
	m_isColl = isColl;
	if (!m_bTalking && isColl && m_pGameInstance->Key_Down(DIK_E))
	{
		m_pArrow->Set_Position(_vec2(1100.f, 600.f));
		CCamera_Manager::Get_Instance()->Set_CameraState(CS_ZOOM);
		_vec4 vLook = m_pTransformCom->Get_State(State::Look);
		vLook.Normalize();
		_vec4 vTargetPos = m_pTransformCom->Get_State(State::Pos);
		CCamera_Manager::Get_Instance()->Set_CameraTargetPos(vTargetPos);
		CCamera_Manager::Get_Instance()->Set_CameraTargetLook(vLook);
		if (m_eState == NPC_QUEST)
		{
			if (!CEvent_Manager::Get_Instance()->Find_Quest(m_strQuestOngoing))
			{
				m_eState = NPC_TALK;
				m_strQuestOngoing = TEXT("");
			}
		}
		m_bTalking = true;
		Set_Text(m_eState);
		m_Animation.iAnimIndex = TALK;
	}


	if (m_pDialogText != nullptr)
	{
		m_pDialogText->Tick(fTimeDelta);
	}
	m_pSkipButton->Tick(fTimeDelta);
	m_pLine->Tick(fTimeDelta);
	m_pBackGround->Tick(fTimeDelta);
	m_pTransformCom->Gravity(fTimeDelta);

	if (m_bTalking == true)
	{
		m_pArrow->Tick(fTimeDelta);
	}
}

void CGroar_Boss::NPC_LateTick(_float fTimeDelta)
{
	if (!m_bTalking)
	{
		if (m_isColl)
		{
			m_pSpeechBubble->Late_Tick(fTimeDelta);
		}
	}
	else
	{
		m_pSkipButton->Late_Tick(fTimeDelta);
		m_pArrow->Late_Tick(fTimeDelta);
		m_pLine->Late_Tick(fTimeDelta);
		m_pBackGround->Late_Tick(fTimeDelta);
		if (m_pDialogText != nullptr)
		{
			m_pDialogText->Late_Tick(fTimeDelta);
		}
	}

	if (m_pGameInstance->Key_Down(DIK_G)) // 치트키 변신 빨리하게
	{
		if (not m_TalkSounds.empty())
		{
			m_TalkSounds.clear();
			m_iSoundChannel = -1;
		}
	}
	if (CTrigger_Manager::Get_Instance()->Is_Coll_BossTrigger() == true)
	{
		if (m_TalkSounds.size() != 0 && m_iSoundChannel == -1)
		{
			m_iSoundChannel = m_pGameInstance->Play_Sound(m_TalkSounds.front());
			m_TalkSounds.pop_front();
		}
	}
	if (m_iSoundChannel == -1 && m_TalkSounds.size() == 0)
	{
		m_eCurState = STATE_SCENE01;
		//m_eCurState = STATE_BOSS;
		//m_eBossCurState = BOSS_STATE_ROAR;

		if (not CTrigger_Manager::Get_Instance()->Get_StartSuicide())
		{
			CFadeBox::FADE_DESC FadeBoxDesc = {};
			FadeBoxDesc.fOut_Duration = 3.f;
			CUI_Manager::Get_Instance()->Add_FadeBox(FadeBoxDesc);

			CTrigger_Manager::Get_Instance()->Set_StartSuicide();
		}
	}
	//사운드 채널 갱신 / 그로아 사운드 나오는 도중에 사운드 넘어가기 위해
	if (m_iSoundChannel != -1)
	{
		if (not m_pGameInstance->Get_IsPlayingSound(m_iSoundChannel))
		{
			m_iSoundChannel = -1;
		}
	}
}

void CGroar_Boss::Set_Text(GROAR_NPCSTATE eState)
{
	if (m_pDialogText != nullptr)
	{
		Safe_Release(m_pDialogText);
	}

	switch (eState)
	{
	case NPC_TALK:
	{
		if (m_vecDialog.empty())
		{
			return;
		}
		wstring strText = m_vecDialog.front();
		if (strText == TEXT("END"))
		{
			CCamera_Manager::Get_Instance()->Set_CameraState(CS_ENDFULLSCREEN);
			CUI_Manager::Get_Instance()->Set_FullScreenUI(false);
			m_bTalking = false;
			m_eState = NPC_END;
			return;
		}

		if (strText[0] == L'!')
		{
			CCamera_Manager::Get_Instance()->Set_CameraState(CS_ENDFULLSCREEN);
			CUI_Manager::Get_Instance()->Set_FullScreenUI(false);
			m_bTalking = false;
			wstring strQuest = strText.substr(1, strText.length());
			CEvent_Manager::Get_Instance()->Set_Quest(strQuest);
			m_eState = NPC_QUEST;
			m_strQuestOngoing = strQuest;
			m_vecDialog.pop_front();
			return;
		}

		CDialogText::DIALOGTEXT_DESC TextDesc = {};
		TextDesc.eLevelID = LEVEL_STATIC;
		TextDesc.fDepth = (_float)D_TALK / (_float)D_END - 0.01f;
		TextDesc.strText = m_vecDialog.front();
		TextDesc.vTextPos = _vec2((_float)g_ptCenter.x, 620.f);
		m_pDialogText = (CDialogText*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_DialogText"), &TextDesc);
		if (m_pDialogText == nullptr)
		{
			return;
		}
		Play_TalkSound(m_vecDialog.front());
		m_vecDialog.pop_front();
	}
	break;
	case NPC_QUEST:
	{
		CDialogText::DIALOGTEXT_DESC TextDesc = {};
		TextDesc.eLevelID = LEVEL_STATIC;
		TextDesc.fDepth = (_float)D_TALK / (_float)D_END - 0.02f;
		TextDesc.strText = m_vecChatt[0];
		TextDesc.vTextPos = _vec2((_float)g_ptCenter.x, 620.f);
		m_pDialogText = (CDialogText*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_DialogText"), &TextDesc);
		if (m_pDialogText == nullptr)
		{
			return;
		}
	}
	break;
	case NPC_END:
	{
		CDialogText::DIALOGTEXT_DESC TextDesc = {};
		TextDesc.eLevelID = LEVEL_STATIC;
		TextDesc.fDepth = (_float)D_TALK / (_float)D_END - 0.02f;
		TextDesc.strText = m_vecChatt[1];
		TextDesc.vTextPos = _vec2((_float)g_ptCenter.x, 620.f);
		m_pDialogText = (CDialogText*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_DialogText"), &TextDesc);
		if (m_pDialogText == nullptr)
		{
			return;
		}
	}
	break;
	}
}

void CGroar_Boss::Play_TalkSound(const wstring& strTalkText)
{
	if (strTalkText == m_strLines[0] ||
		strTalkText == m_strLines[1] ||
		strTalkText == m_strLines[4] ||
		strTalkText == m_strLines[8] ||
		strTalkText == m_strLines[9] ||
		strTalkText == m_strLines[10] ||
		strTalkText == m_strLines[11])
	{
		if (m_iSoundChannel != -1)
		{
			m_pGameInstance->StopSound(m_iSoundChannel);
		}
		m_iSoundChannel = m_pGameInstance->Play_Sound(m_TalkSounds.front());
		m_TalkSounds.pop_front();
	}
}

HRESULT CGroar_Boss::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Groar"), TEXT("Com_NPC_Model"), reinterpret_cast<CComponent**>(&m_pNPCModelCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_GroarScene01"), TEXT("Com_Scene01_Model"), reinterpret_cast<CComponent**>(&m_pScene01ModelCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_GroarScene02"), TEXT("Com_Scene02_Model"), reinterpret_cast<CComponent**>(&m_pScene02ModelCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Groar_Boss"), TEXT("Com_Boss_Model"), reinterpret_cast<CComponent**>(&m_pBossModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGroar_Boss::Bind_ShaderResources()
{
	if (m_iPassIndex == AnimPass_Rim)
	{
		_vec4 vColor = Colors::Red;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &vColor, sizeof vColor)))
		{
			return E_FAIL;
		}
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_OldWorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CGroar_Boss* CGroar_Boss::Create(_dev pDevice, _context pContext)
{
	CGroar_Boss* pInstance = new CGroar_Boss(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CGroar_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGroar_Boss::Clone(void* pArg)
{
	CGroar_Boss* pInstance = new CGroar_Boss(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CGroar_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGroar_Boss::Free()
{
	__super::Free();


	Safe_Release(m_pLine);
	Safe_Release(m_pArrow);
	Safe_Release(m_pSkipButton);
	Safe_Release(m_pDialogText);
	Safe_Release(m_pBackGround);
	Safe_Release(m_pSpeechBubble);


	Safe_Release(m_pHpBoss);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pNPCModelCom);
	Safe_Release(m_pScene01ModelCom);
	Safe_Release(m_pScene02ModelCom);
	Safe_Release(m_pBossModelCom);

	Safe_Release(m_pNpcColliderCom);
	Safe_Release(m_pBodyColliderCom);
	Safe_Release(m_pAttackColliderCom);

}
