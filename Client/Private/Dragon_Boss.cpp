#include "Dragon_Boss.h"

#include "Camera_Manager.h"
#include "Effect_Manager.h"
#include "Effect_Dummy.h"

const _float CDragon_Boss::m_fAttackRange = 10.f;

CDragon_Boss::CDragon_Boss(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CDragon_Boss::CDragon_Boss(const CDragon_Boss& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDragon_Boss::Init_Prototype()
{
	return S_OK;
}

HRESULT CDragon_Boss::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	TRAIL_DESC Desc{};
	Desc.vColor = Colors::Maroon;
	Desc.vPSize = _vec2(0.1f, 0.1f);
	Desc.iNumVertices = 10;
	m_pLeftTrail1 = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);
	m_pLeftTrail2 = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);
	m_pLeftTrail3 = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);
	m_pRightTrail1 = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);
	m_pRightTrail2 = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);
	m_pRightTrail3 = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);

	m_eCurState = STATE_ROAR;
	//m_eCurState = STATE_FLY_FIRE;

	m_Animation.fInterpolationTime = 0.5f;

	m_iHP = 20000;

	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	m_pTransformCom->Set_Position(_vec3(vPlayerPos) + _vec3(0.f, 3.f, 0.f));

	return S_OK;
}

void CDragon_Boss::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_OldMatrix();

	if (m_pGameInstance->Key_Down(DIK_DELETE))
	{
		Kill();
		m_pGameInstance->Delete_CollisionObject(this);
		m_pTransformCom->Delete_Controller();
	}

	if (m_pGameInstance->Key_Down(DIK_RIGHT))
	{
		m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Blackhole"), TEXT("Prototype_GameObject_Blackhole"));
	}

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	Update_Collider();
	Update_Trail(fTimeDelta);

	m_pTransformCom->Gravity(fTimeDelta);
}

void CDragon_Boss::Late_Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif

}

HRESULT CDragon_Boss::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
		}

		_bool HasNorTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(m_iPassIndex)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}

	}

	return S_OK;
}

void CDragon_Boss::Set_Damage(_int iDamage, _uint iDamageType)
{
	m_iHP -= iDamage;
	m_bChangePass = true;

	//CHitEffect::HITEFFECT_DESC Desc{};
	//Desc.iDamage = iDamage;
	//Desc.pParentTransform = m_pTransformCom;
	//Desc.vTextPosition = _vec2(0.f, 1.5f);
	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_HitEffect"), TEXT("Prototype_GameObject_HitEffect"), &Desc)))
	//{
	//	return;
	//}
}

void CDragon_Boss::Init_State(_float fTimeDelta)
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

	if (m_iHP <= 0)
	{
		m_eCurState = STATE_DIE;
	}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CDragon_Boss::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;
			m_Animation.fDurationRatio = 1.f;

			m_fIdleTime = 0.f;
			m_fTime[0] = 0.f;
			m_fTime[1] = 0.f;

			m_fMeteorTime = 0.f;

			m_bSelectAttackPattern = false;

			m_bAttacked1 = false;
			m_bAttacked2 = false;

			m_bCanPull = false;

			m_bCreateObject = false;
			m_bCreateEffect[0] = false;
			m_bCreateEffect[1] = false;

			break;

		case Client::CDragon_Boss::STATE_ROAR:
			m_Animation.iAnimIndex = ROAR;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			m_pTransformCom->LookAt_Dir(vDir);

			break;

		case Client::CDragon_Boss::STATE_CHASE:
			m_Animation.iAnimIndex = RUN;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 1.5f;

			break;

		case Client::CDragon_Boss::STATE_RIGHT_SLASH:
			m_Animation.iAnimIndex = OUROBOROS_ATTACK01;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CDragon_Boss::STATE_TAKE_DOWN:
			m_Animation.iAnimIndex = OUROBOROS_ATTACK02;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			m_Animation.fDurationRatio = 0.9f;

			break;

		case Client::CDragon_Boss::STATE_SHOOT_FIRE:
			m_Animation.iAnimIndex = OUROBOROS_ATTACK03;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			{
				_mat EffectMatrix = _mat::CreateScale(25.f) * _mat::CreateRotationX(XMConvertToRadians(90.f))
					* _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.2f, 0.f)));

				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Frame");
				Info.pMatrix = &EffectMatrix;
				m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Base");
				Info.pMatrix = &EffectMatrix;
				m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
			}

			break;

		case Client::CDragon_Boss::STATE_BLACKHOLE:
			m_Animation.iAnimIndex = OUROBOROS_ATTACK04;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Blackhole"), TEXT("Prototype_GameObject_Blackhole"));

			{
				_mat EffectMatrix = _mat::CreateScale(10.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Blackhole_Spread_Mesh");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
			}

			break;

		case Client::CDragon_Boss::STATE_WING_ATTACK:
			m_Animation.iAnimIndex = OUROBOROS_ATTACK05;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			{
				_mat EffectMatrix = _mat::CreateScale(5.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Pull_Out");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				m_pGameInstance->Play_Sound(TEXT("Sfx_Boss_Hirokin_Atk_04_01"));
			}

			break;

			//case Client::CDragon_Boss::STATE_UNKNOWN:
			//	m_Animation.iAnimIndex = OUROBOROS_ATTACK06_START;
			//	m_Animation.isLoop = false;
			//	m_Animation.fAnimSpeedRatio = 2.f;

			//	break;

		case Client::CDragon_Boss::STATE_RAGE:
			m_Animation.iAnimIndex = OUROBOROS_ATTACK07;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CDragon_Boss::STATE_FIRE_PILLAR:
			m_Animation.iAnimIndex = OUROBOROS_ATTACK08;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			for (size_t i = 0; i < 5; i++)
			{
				m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_FirePillar"), TEXT("Prototype_GameObject_FirePillar"));
			}

			{
				_mat EffectMatrix = _mat::CreateScale(2.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Floor_Spread");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
			}

			break;

		case Client::CDragon_Boss::STATE_FLY_FIRE:
			m_Animation.iAnimIndex = OUROBOROS_ATTACK09;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			{
				m_vPlayerOldPos = vPlayerPos;

				_mat EffectMatrix = _mat::CreateScale(15.f) * _mat::CreateRotationX(XMConvertToRadians(90.f))
					* _mat::CreateTranslation(_vec3(m_vPlayerOldPos + _vec3(0.f, 0.2f, 0.f)));

				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Frame");
				Info.pMatrix = &EffectMatrix;
				m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Base");
				Info.pMatrix = &EffectMatrix;
				m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
			}

			break;

		case Client::CDragon_Boss::STATE_DOUBLE_SLASH:
			m_Animation.iAnimIndex = OUROBOROS_ATTACK10;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CDragon_Boss::STATE_HIT:
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

			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;
		}

		break;

		case Client::CDragon_Boss::STATE_DIE:
			m_Animation.iAnimIndex = DIE;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;
		}

		m_ePreState = m_eCurState;
	}

}

void CDragon_Boss::Tick_State(_float fTimeDelta)
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
	_float fDistance = (vPlayerPos - vPos).Length();

	_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
	vDir.y = 0.f;

	switch (m_eCurState)
	{
	case Client::CDragon_Boss::STATE_IDLE:

		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 1.f)
		{
			m_eCurState = STATE_CHASE;
		}

		break;

	case Client::CDragon_Boss::STATE_ROAR:

		m_fTime[0] += fTimeDelta;

		if (m_pModelCom->Get_CurrentAnimPos() >= 138.f && m_pModelCom->Get_CurrentAnimPos() <= 259.f)
		{
			if (!m_bCreateEffect[0])
			{
				_mat EffectMatrix = _mat::CreateScale(10.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) /*+ _vec3(0.f, -0.1f, 0.f)*/));
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Roar_Spread");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(4.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Roar_Floor_Parti");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(8.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 5.f, 0.f)));
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Roar_Distortion");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				m_bCreateEffect[0] = true;
			}

			if (m_fTime[0] >= 0.1f)
			{
				_mat EffectMatrix = _mat::CreateScale(2.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 5.f, 0.f)));
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Roar_Rect");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(2.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Roar_Floor");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				m_fTime[0] = 0.f;
			}
		}
		else
		{
			m_bCreateEffect[0] = false;
		}

		if (m_pModelCom->Get_CurrentAnimPos() >= 135.f && m_pModelCom->Get_CurrentAnimPos() <= 250.f)
		{
			CCamera_Manager::Get_Instance()->Set_ShakeCam(true, 0.1f);
		}

		if (m_pModelCom->IsAnimationFinished(ROAR))
		{
			m_eCurState = STATE_IDLE;
		}

		break;

	case Client::CDragon_Boss::STATE_CHASE:
	{
		DRAGON_STATE eTempDragonState = { STATE_END };

		if (!m_bSelectAttackPattern)
		{
			_bool bReset = { true };
			for (auto& it : m_bAttack_Selected)
			{
				if (it == false)
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

			DRAGON_ATTACK eAttackRandom = static_cast<DRAGON_ATTACK>(rand() % ATTACK_END);

			while (m_bAttack_Selected[eAttackRandom] == true)
			{
				eAttackRandom = static_cast<DRAGON_ATTACK>(rand() % ATTACK_END);
			}

			switch (eAttackRandom)
			{
			case Client::CDragon_Boss::RIGHT_SLASH:
				eTempDragonState = STATE_RIGHT_SLASH;
				break;
			case Client::CDragon_Boss::TAKE_DOWN:
				eTempDragonState = STATE_TAKE_DOWN;
				break;
			case Client::CDragon_Boss::SHOOT_FIRE:
				eTempDragonState = STATE_SHOOT_FIRE;
				break;
			case Client::CDragon_Boss::BLACKHOLE:
				eTempDragonState = STATE_BLACKHOLE;
				break;
			case Client::CDragon_Boss::WING_ATTACK:
				eTempDragonState = STATE_WING_ATTACK;
				break;
				//case Client::CDragon_Boss::UNKNOWN:
				//	eTempDragonState = STATE_UNKNOWN;
				//	break;
			case Client::CDragon_Boss::RAGE:
				eTempDragonState = STATE_RAGE;
				break;
			case Client::CDragon_Boss::FIRE_PILLAR:
				eTempDragonState = STATE_FIRE_PILLAR;
				break;
			case Client::CDragon_Boss::FLY_FIRE:
				eTempDragonState = STATE_FLY_FIRE;
				break;
			case Client::CDragon_Boss::DOUBLE_SLASH:
				eTempDragonState = STATE_DOUBLE_SLASH;
				break;
			}

			m_bSelectAttackPattern = true;


		}

		m_pTransformCom->LookAt_Dir(vDir);

		if (fDistance >= 7.f)
		{
			m_pTransformCom->Go_Straight(fTimeDelta);
			m_bSelectAttackPattern = false;
		}
		else
		{
			m_eCurState = eTempDragonState;

			m_eCurState = STATE_FIRE_PILLAR; // 테스트용
		}
	}

	break;

	case Client::CDragon_Boss::STATE_RIGHT_SLASH:

		if (m_pModelCom->Get_CurrentAnimPos() >= 41.f && m_pModelCom->Get_CurrentAnimPos() <= 43.f)
		{
			if (!m_bAttacked1)
			{
				_uint iDamage = 60 + rand() % 20;
				m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
				m_bAttacked1 = true;
			}
		}

		if (m_pModelCom->Get_CurrentAnimPos() >= 37.f && m_pModelCom->Get_CurrentAnimPos() <= 43.f)
		{
			/*m_pRightTrail1->Late_Tick(fTimeDelta);
			m_pRightTrail2->Late_Tick(fTimeDelta);
			m_pRightTrail3->Late_Tick(fTimeDelta);*/

			//if (!m_bCreateEffect[0])
			//{
			//	_mat EffectMatrix = _mat::CreateScale(3.f) * /*_mat::CreateTranslation(0.f, 0.1f, 0.f) **/ (*m_pModelCom->Get_BoneMatrix("Bip001-R-Finger21"))
			//		* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();
			//	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Trail"); // 수정
			//	Info.pMatrix = &EffectMatrix;
			//	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			//	m_bCreateEffect[0] = true;
			//}
		}

		if (m_pModelCom->IsAnimationFinished(OUROBOROS_ATTACK01))
		{
			m_eCurState = STATE_IDLE;
			m_bAttack_Selected[RIGHT_SLASH] = true;
		}

		break;

	case Client::CDragon_Boss::STATE_TAKE_DOWN:

		m_fTime[0] += fTimeDelta;

		if (m_pModelCom->Get_CurrentAnimPos() >= 3.f && m_pModelCom->Get_CurrentAnimPos() <= 82.f)
		{
			m_FollowEffectMatrix[0] = _mat::CreateScale(5.f) * /*_mat::CreateTranslation(0.f, 0.1f, 0.f) * */(*m_pModelCom->Get_BoneMatrix("Bip001-R-Finger21"))
				* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

			m_FollowEffectMatrix[1] = _mat::CreateScale(5.f) * /*_mat::CreateTranslation(0.f, 0.1f, 0.f) * */(*m_pModelCom->Get_BoneMatrix("Bip001-L-Finger21"))
				* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

			if (m_fTime[0] >= 0.7f)
			{
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Mouth"); // 수정
				Info.pMatrix = &m_FollowEffectMatrix[0];
				Info.isFollow = true;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Mouth3"); // 수정
				Info.pMatrix = &m_FollowEffectMatrix[0];
				Info.isFollow = true;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);


				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Mouth"); // 수정
				Info.pMatrix = &m_FollowEffectMatrix[1];
				Info.isFollow = true;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Mouth3"); // 수정
				Info.pMatrix = &m_FollowEffectMatrix[1];
				Info.isFollow = true;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				m_fTime[0] = 0.f;
			}
		}

		if (m_pModelCom->Get_CurrentAnimPos() >= 84.f)
		{
			if (!m_bCreateEffect[0])
			{
				// 오른쪽
				_mat EffectMatrix = _mat::CreateScale(2.f) * _mat::CreateTranslation(0.f, 0.1f, 0.f) * (*m_pModelCom->Get_BoneMatrix("Bip001-R-Finger21")) 
					* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();;
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Crack"); // 수정
				//Info.pMatrix = &EffectMatrix;
				//CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(4.f) * _mat::CreateTranslation(0.f, 0.1f, 0.f) * (*m_pModelCom->Get_BoneMatrix("Bip001-R-Finger21"))
					* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();;
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Parti_Black");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(4.f) * _mat::CreateTranslation(0.f, 0.1f, 0.f) * (*m_pModelCom->Get_BoneMatrix("Bip001-R-Finger21"))
					* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();;
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Parti_Red");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(2.f) * _mat::CreateTranslation(0.f, 0.1f, 0.f) * (*m_pModelCom->Get_BoneMatrix("Bip001-R-Finger21"))
					* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();;
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Spread2");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(2.f) * _mat::CreateTranslation(0.f, 0.1f, 0.f) * (*m_pModelCom->Get_BoneMatrix("Bip001-R-Finger21"))
					* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();;
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Spread3");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(2.f) * _mat::CreateRotationX(XMConvertToRadians(180.f)) * _mat::CreateTranslation(0.f, 0.1f, 0.f) * (*m_pModelCom->Get_BoneMatrix("Bip001-R-Finger21"))
					* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();;
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Spread4");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(1.5f) * _mat::CreateRotationX(XMConvertToRadians(180.f)) * _mat::CreateTranslation(0.f, 0.1f, 0.f) * (*m_pModelCom->Get_BoneMatrix("Bip001-R-Finger21"))
					* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();;
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Spread5");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(2.2f) * _mat::CreateRotationX(XMConvertToRadians(180.f)) * _mat::CreateTranslation(0.f, 0.1f, 0.f) * (*m_pModelCom->Get_BoneMatrix("Bip001-R-Finger21"))
					* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();;
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Spread6");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);


				// 왼쪽
				EffectMatrix = _mat::CreateScale(2.f) * _mat::CreateTranslation(0.f, 0.1f, 0.f) * (*m_pModelCom->Get_BoneMatrix("Bip001-L-Finger21"))
					* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();;
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Crack"); // 수정
				//Info.pMatrix = &EffectMatrix;
				//CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(4.f) * _mat::CreateTranslation(0.f, 0.1f, 0.f) * (*m_pModelCom->Get_BoneMatrix("Bip001-L-Finger21"))
					* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();;
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Parti_Black");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(4.f) * _mat::CreateTranslation(0.f, 0.1f, 0.f) * (*m_pModelCom->Get_BoneMatrix("Bip001-L-Finger21"))
					* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();;
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Parti_Red");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(2.f) * _mat::CreateTranslation(0.f, 0.1f, 0.f) * (*m_pModelCom->Get_BoneMatrix("Bip001-L-Finger21"))
					* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();;
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Spread2");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(2.f) * _mat::CreateTranslation(0.f, 0.1f, 0.f) * (*m_pModelCom->Get_BoneMatrix("Bip001-L-Finger21"))
					* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();;
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Spread3");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(2.f) * _mat::CreateRotationX(XMConvertToRadians(180.f)) * _mat::CreateTranslation(0.f, 0.1f, 0.f) * (*m_pModelCom->Get_BoneMatrix("Bip001-L-Finger21"))
					* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();;
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Spread4");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(1.5f) * _mat::CreateRotationX(XMConvertToRadians(180.f)) * _mat::CreateTranslation(0.f, 0.1f, 0.f) * (*m_pModelCom->Get_BoneMatrix("Bip001-L-Finger21"))
					* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();;
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Spread5");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(2.2f) * _mat::CreateRotationX(XMConvertToRadians(180.f)) * _mat::CreateTranslation(0.f, 0.1f, 0.f) * (*m_pModelCom->Get_BoneMatrix("Bip001-L-Finger21"))
					* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();;
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_TakeDown_Spread6");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);


				m_bCreateEffect[0] = true;
			}
		}

		if (m_pModelCom->Get_CurrentAnimPos() >= 82.f && m_pModelCom->Get_CurrentAnimPos() <= 84.f)
		{
			if (!m_bAttacked1)
			{
				_uint iDamage = 80 + rand() % 20;
				m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
				m_bAttacked1 = true;
			}

			CCamera_Manager::Get_Instance()->Set_ShakeCam(true, 0.3f);
		}

		if (m_pModelCom->IsAnimationFinished(OUROBOROS_ATTACK02))
		{
			m_eCurState = STATE_IDLE;
			m_bAttack_Selected[TAKE_DOWN] = true;
		}

		break;

	case Client::CDragon_Boss::STATE_SHOOT_FIRE:

		m_fTime[0] += fTimeDelta;
		m_fTime[1] += fTimeDelta;


		if (m_fTime[0] >= 1.2f)
		{
			if (m_pFrameEffect && m_pBaseEffect)
			{
				Safe_Release(m_pFrameEffect);
				Safe_Release(m_pBaseEffect);

				_mat EffectMatrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Meteor_Floor");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
			}
	
			m_fMeteorTime += fTimeDelta;

			if (m_fMeteorTime >= 0.05f)
			{
				m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Meteor"), TEXT("Prototype_GameObject_Meteor"));
				m_fMeteorTime = 0.f;
			}
		}

		//if (m_pModelCom->Get_CurrentAnimPos() >= 5.f && m_pModelCom->Get_CurrentAnimPos() <= 100.f)
		//{
		//	m_UpdateMatrix[0] = _mat::CreateScale(5.f) * /*_mat::CreateTranslation(0.f, 0.1f, 0.f) * */(*m_pModelCom->Get_BoneMatrix("Nose_Bone001_end"))
		//		* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

		//	if (m_fTime[1] >= 0.5f)
		//	{
		//		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_FirePillar_Mouth2"); // 수정
		//		Info.pMatrix = &m_UpdateMatrix[0];
		//		Info.isFollow = true;
		//		CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

		//		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_FirePillar_Mouth3"); // 수정
		//		Info.pMatrix = &m_UpdateMatrix[0];
		//		Info.isFollow = true;
		//		CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

		//		m_fTime[1] = 0.f;
		//	}
		//}

		if (m_pFrameEffect && m_pBaseEffect)
		{
			m_pFrameEffect->Tick(fTimeDelta);
			m_pBaseEffect->Tick(fTimeDelta);
		}

		if (m_pFrameEffect && m_pBaseEffect)
		{
			m_pFrameEffect->Late_Tick(fTimeDelta);
			m_pBaseEffect->Late_Tick(fTimeDelta);
		}


		if (m_pModelCom->IsAnimationFinished(OUROBOROS_ATTACK03))
		{
			m_eCurState = STATE_IDLE;
			m_bAttack_Selected[SHOOT_FIRE] = true;
		}

		break;

	case Client::CDragon_Boss::STATE_BLACKHOLE:

		m_fTime[0] += fTimeDelta;

		if (m_pModelCom->Get_CurrentAnimPos() >= 13.f && m_pModelCom->Get_CurrentAnimPos() <= 58.f)
		{
			m_FollowEffectMatrix[0] = _mat::CreateScale(4.f) * /*_mat::CreateTranslation(0.f, 0.1f, 0.f) * */(*m_pModelCom->Get_BoneMatrix("Bip001-L-Finger2"))
				* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

			if (!m_bCreateEffect[0])
			{
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Blackhole_Hand_Sphere"); // 수정
				Info.pMatrix = &m_FollowEffectMatrix[0];
				Info.isFollow = true;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				m_bCreateEffect[0] = true;
			}

			m_FollowEffectMatrix[1] = _mat::CreateScale(6.f) * /*_mat::CreateTranslation(0.f, 0.1f, 0.f) * */(*m_pModelCom->Get_BoneMatrix("Bip001-L-Finger2"))
				* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

			if (m_fTime[0] >= 0.2f)
			{
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Blackhole_Hand_Rect2"); // 수정
				Info.pMatrix = &m_FollowEffectMatrix[1];
				Info.isFollow = true;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				m_fTime[0] = 0.f;
			}
		}

		if (m_pModelCom->IsAnimationFinished(OUROBOROS_ATTACK04))
		{
			m_eCurState = STATE_IDLE;
			m_bAttack_Selected[BLACKHOLE] = true;
		}

		break;

	case Client::CDragon_Boss::STATE_WING_ATTACK:

		if (m_pModelCom->Get_CurrentAnimPos() >= 28.f)
		{
			if (!m_bCreateEffect[0])
			{
				_mat EffectMatrix = _mat::CreateScale(4.f, 2.f, 1.f) * _mat::CreateTranslation(/*_vec3(m_pTransformCom->Get_State(State::Pos) + */_vec3(0.f, 12.f, 0.f))
					 * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Pull_Wing_Parti");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				m_bCreateEffect[0] = true;
			}
		}

		if (m_pModelCom->Get_CurrentAnimPos() >= 69.f && m_pModelCom->Get_CurrentAnimPos() <= 71.f)
		{
			if (!m_bCreateEffect[1])
			{
				_mat EffectMatrix = _mat::CreateScale(3.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Pull_In");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(5.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)/* + _vec3(0.f, -0.1f, 0.f)*/));
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Pull_Magnet_Effect");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + -2.f * m_pTransformCom->Get_State(State::Look).Get_Normalized()));
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Pull_Magnet_Effect2");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				//Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Pull_Magnet_Effect3");
				//Info.pMatrix = &EffectMatrix;
				//CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(3.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)/* + _vec3(0.f, -0.1f, 0.f)*/));
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Pull_Magnet_Effect4");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				m_bCreateEffect[1] = true;
			}

			if (fDistance <= 10.f)
			{
				m_bCanPull = true;

				if (!m_bAttacked1)
				{
					_uint iDamage = 100 + rand() % 20;
					m_pGameInstance->Attack_Player(nullptr, iDamage);
					m_bAttacked1 = true;
				}

			}

			if (m_bCanPull == true)
			{
				m_fTime[0] += fTimeDelta;

				if (m_fTime[0] <= 1.5f)
				{
					pPlayerTransform->Go_To_Dir(-vDir, fTimeDelta * 30.f);
				}
			}
		}

		if (m_pModelCom->IsAnimationFinished(OUROBOROS_ATTACK05))
		{
			m_eCurState = STATE_IDLE;
			m_bAttack_Selected[WING_ATTACK] = true;
		}

		break;

		//case Client::CDragon_Boss::STATE_UNKNOWN:

		//	if (m_pModelCom->IsAnimationFinished(OUROBOROS_ATTACK06_START))
		//	{
		//		m_Animation.iAnimIndex = OUROBOROS_ATTACK06_LOOP;
		//	}

		//	if (m_pModelCom->IsAnimationFinished(OUROBOROS_ATTACK06_LOOP))
		//	{
		//		m_Animation.iAnimIndex = OUROBOROS_ATTACK06_END;
		//	}

		//	if (m_pModelCom->IsAnimationFinished(OUROBOROS_ATTACK06_END))
		//	{
		//		m_eCurState = STATE_IDLE;
		//		m_bAttack_Selected[UNKNOWN] = true;
		//	}

		//	break;

	case Client::CDragon_Boss::STATE_RAGE:

		m_fTime[0] += fTimeDelta;
		m_fTime[1] += fTimeDelta;

		if (m_pModelCom->Get_CurrentAnimPos() >= 129.f && m_pModelCom->Get_CurrentAnimPos() <= 245.f)
		{
			CCamera_Manager::Get_Instance()->Set_ShakeCam(true, 0.3f);

			if (!m_bCreateEffect[0])
			{
				_mat EffectMatrix = _mat::CreateScale(15.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.2f, 0.f)));
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Rage_Floor_Parti");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(8.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 5.f, 0.f)));
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Roar_Distortion");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				m_bCreateEffect[0] = true;
			}

			if (m_fTime[0] >= 0.2f)
			{
				_mat EffectMatrix = _mat::CreateScale(20.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) /*+ _vec3(0.f, -0.1f, 0.f)*/));
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Rage");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(2.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Rage_Floor");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				m_fTime[0] = 0.f;
			}

			//if (m_fTime[1] >= 0.3f)
			//{
			//	_mat EffectMatrix = _mat::CreateScale(10.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) /*+ _vec3(0.f, -0.1f, 0.f)*/));
			//	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Rage_Spread");
			//	Info.pMatrix = &EffectMatrix;
			//	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			//	m_fTime[1] = 0.f;
			//}
		}

		if (m_pModelCom->IsAnimationFinished(OUROBOROS_ATTACK07))
		{
			m_eCurState = STATE_IDLE;
			m_bAttack_Selected[RAGE] = true;
		}

		break;

	case Client::CDragon_Boss::STATE_FIRE_PILLAR:

		m_fTime[0] += fTimeDelta;

		if (m_pModelCom->Get_CurrentAnimPos() >= 8.f && m_pModelCom->Get_CurrentAnimPos() <= 77.f)
		{
			m_FollowEffectMatrix[0] = _mat::CreateScale(5.f) * /*_mat::CreateTranslation(0.f, 0.1f, 0.f) * */(*m_pModelCom->Get_BoneMatrix("Nose_Bone001_end"))
				* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

			if (m_fTime[0] >= 0.4f)
			{
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_FirePillar_Mouth2"); // 수정
				Info.pMatrix = &m_FollowEffectMatrix[0];
				Info.isFollow = true;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_FirePillar_Mouth3"); // 수정
				Info.pMatrix = &m_FollowEffectMatrix[0];
				Info.isFollow = true;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				m_fTime[0] = 0.f;
			}

		}

		if (m_pModelCom->IsAnimationFinished(OUROBOROS_ATTACK08))
		{
			m_eCurState = STATE_IDLE;
			m_bAttack_Selected[FIRE_PILLAR] = true;
		}

		break;

	case Client::CDragon_Boss::STATE_FLY_FIRE:

	{
		m_fTime[0] += fTimeDelta;

		if (m_fTime[0] >= 2.f)
		{
			if (m_pBaseEffect && m_pFrameEffect)
			{
				Safe_Release(m_pFrameEffect);
				Safe_Release(m_pBaseEffect);

				m_fDragonHeadTime += fTimeDelta;

				//m_vDragonHeadPos = vPlayerPos;
				_mat EffectMatrix = _mat::CreateScale(2.f) * _mat::CreateTranslation(_vec3(m_vPlayerOldPos));
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Head");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(2.3f) * _mat::CreateTranslation(_vec3(m_vPlayerOldPos + _vec3(0.f, 0.2f, 0.f)));
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Head_Fire_Floor");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(2.3f) * _mat::CreateTranslation(_vec3(m_vPlayerOldPos + _vec3(0.f, 0.5f, 0.f)));
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Floor_Parti2");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				EffectMatrix = _mat::CreateScale(2.f) /** _mat::CreateRotationX(180.f)*/ * _mat::CreateTranslation(_vec3(m_vPlayerOldPos + _vec3(0.f, -1.f, 0.f)));
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Head_Fire_Parti");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			}

			m_fDragonHeadTime += fTimeDelta;

			if (m_fTime[0] <= 6.f)
			{
				if (m_fDragonHeadTime >= 0.2f)
				{
					_mat EffectMatrix = _mat::CreateScale(1.f, 2.f, 1.f) * _mat::CreateRotationX(XMConvertToRadians(180.f)) * _mat::CreateTranslation(_vec3(m_vPlayerOldPos + _vec3(0.f, 5.f, 0.f)));
					EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Head_Fire");
					Info.pMatrix = &EffectMatrix;
					CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

					m_fDragonHeadTime = 0.f;
				}

			}
		}

		if (m_pFrameEffect && m_pBaseEffect)
		{
			m_pFrameEffect->Tick(fTimeDelta);
			m_pBaseEffect->Tick(fTimeDelta);
		}

		if (m_pFrameEffect && m_pBaseEffect)
		{
			m_pFrameEffect->Late_Tick(fTimeDelta);
			m_pBaseEffect->Late_Tick(fTimeDelta);
		}

		if (m_pModelCom->Get_CurrentAnimPos() >= 326.f && m_pModelCom->Get_CurrentAnimPos() <= 330.f)
		{
			CCamera_Manager::Get_Instance()->Set_ShakeCam(true, 0.3f);

			if (!m_bCreateEffect[0])
			{
				_mat EffectMatrix = _mat::CreateScale(10.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.2f, 0.f)));
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Dust_Parti");
				Info.pMatrix = &EffectMatrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				m_bCreateEffect[0] = true;
			}
		}

		if (m_pModelCom->IsAnimationFinished(OUROBOROS_ATTACK09))
		{
			m_eCurState = STATE_IDLE;
			m_bAttack_Selected[FLY_FIRE] = true;
		}
	}

	break;

	case Client::CDragon_Boss::STATE_DOUBLE_SLASH:

		if (m_pModelCom->Get_CurrentAnimPos() >= 38.f && m_pModelCom->Get_CurrentAnimPos() <= 40.f)
		{
			if (!m_bAttacked1)
			{
				_uint iDamage = 60 + rand() % 20;
				m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
				m_bAttacked1 = true;
			}
		}

		if (m_pModelCom->Get_CurrentAnimPos() >= 51.f && m_pModelCom->Get_CurrentAnimPos() <= 53.f)
		{
			if (!m_bAttacked2)
			{
				_uint iDamage = 60 + rand() % 20;
				m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
				m_bAttacked2 = true;
			}
		}

		if (m_pModelCom->Get_CurrentAnimPos() >= 32.f && m_pModelCom->Get_CurrentAnimPos() <= 41.f)
		{
			m_pLeftTrail1->Late_Tick(fTimeDelta);
			m_pLeftTrail2->Late_Tick(fTimeDelta);
			m_pLeftTrail3->Late_Tick(fTimeDelta);
		}

		if (m_pModelCom->Get_CurrentAnimPos() >= 46.f && m_pModelCom->Get_CurrentAnimPos() <= 56.f)
		{
			m_pRightTrail1->Late_Tick(fTimeDelta);
			m_pRightTrail2->Late_Tick(fTimeDelta);
			m_pRightTrail3->Late_Tick(fTimeDelta);
		}

		if (m_pModelCom->IsAnimationFinished(OUROBOROS_ATTACK10))
		{
			m_eCurState = STATE_IDLE;
			m_bAttack_Selected[DOUBLE_SLASH] = true;
		}

		break;

	case Client::CDragon_Boss::STATE_HIT:

		if (m_pModelCom->IsAnimationFinished(HIT_L) || m_pModelCom->IsAnimationFinished(HIT_R))
		{
			m_eCurState = STATE_CHASE;
		}

		break;

	case Client::CDragon_Boss::STATE_DIE:
		break;
	}

	m_pModelCom->Set_Animation(m_Animation);
}

HRESULT CDragon_Boss::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(1.5f, 1.5f, 2.5f);
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
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), 1.f, 0.01f, 4.f);
	XMStoreFloat4x4(&ColDesc.matFrustum, matView * matProj);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 2.f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 2.5f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(1.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.01f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	return S_OK;
}

void CDragon_Boss::Update_Collider()
{
	_mat Offset = _mat::CreateTranslation(0.f, 2.f, 0.f);
	m_pBodyColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());

	Offset = _mat::CreateTranslation(0.f, 4.f, 2.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

void CDragon_Boss::Update_Trail(_float fTimeDelta)
{
	// 1번째 손가락
	_mat Matrix = *m_pModelCom->Get_BoneMatrix("Bip001-L-Finger11");
	//_mat Offset = _mat::CreateTranslation(_vec3(-1.62f, 0.01f, 0.27f));
	_mat Result = /*Offset **/ Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

	m_pLeftTrail1->Tick(Result.Position_vec3());

	Matrix = *m_pModelCom->Get_BoneMatrix("Bip001-R-Finger11");
	//Offset = _mat::CreateTranslation(_vec3(1.55f, -0.09f, -0.2f));
	Result = /*Offset * */Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

	m_pRightTrail1->Tick(Result.Position_vec3());

	// 가운데 손가락
	Matrix = *m_pModelCom->Get_BoneMatrix("Bip001-L-Finger21");
	//Offset = _mat::CreateTranslation(_vec3(-1.62f, 0.01f, 0.27f));
	Result = /*Offset * */Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

	m_pLeftTrail2->Tick(Result.Position_vec3());

	Matrix = *m_pModelCom->Get_BoneMatrix("Bip001-R-Finger21");
	//Offset = _mat::CreateTranslation(_vec3(1.62f, -0.01f, -0.27f));
	Result = /*Offset * */Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

	m_pRightTrail2->Tick(Result.Position_vec3());


	// 3번째 손가락
	Matrix = *m_pModelCom->Get_BoneMatrix("Bip001-L-Finger01");
	//Offset = _mat::CreateTranslation(_vec3(-1.52f, -0.07f, 0.25f));
	Result = /*Offset * */Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

	m_pLeftTrail3->Tick(Result.Position_vec3());

	Matrix = *m_pModelCom->Get_BoneMatrix("Bip001-R-Finger01");
	//Offset = _mat::CreateTranslation(_vec3(1.52f, 0.07f, -0.25f));
	Result = /*Offset * */Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

	m_pRightTrail3->Tick(Result.Position_vec3());
}

HRESULT CDragon_Boss::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Dragon"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDragon_Boss::Bind_ShaderResources()
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

CDragon_Boss* CDragon_Boss::Create(_dev pDevice, _context pContext)
{
	CDragon_Boss* pInstance = new CDragon_Boss(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CDragon_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDragon_Boss::Clone(void* pArg)
{
	CDragon_Boss* pInstance = new CDragon_Boss(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CDragon_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDragon_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pBodyColliderCom);
	Safe_Release(m_pAttackColliderCom);
	Safe_Release(m_pBaseEffect);
	Safe_Release(m_pFrameEffect);

	Safe_Release(m_pLeftTrail1);
	Safe_Release(m_pLeftTrail2);
	Safe_Release(m_pLeftTrail3);
	Safe_Release(m_pRightTrail1);
	Safe_Release(m_pRightTrail2);
	Safe_Release(m_pRightTrail3);

}
