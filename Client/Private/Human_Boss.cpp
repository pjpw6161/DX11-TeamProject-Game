#include "Human_Boss.h"
#include "Effect_Manager.h"
#include "Effect_Dummy.h"
#include "Camera_Manager.h"
CHuman_Boss::CHuman_Boss(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CHuman_Boss::CHuman_Boss(const CHuman_Boss& rhs)
	: CGameObject(rhs)
{
}

HRESULT CHuman_Boss::Init_Prototype()
{
	return S_OK;
}

HRESULT CHuman_Boss::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, _vec4(0.f, -1000.f, 0.f, 1.f));
	m_pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	Safe_AddRef(m_pPlayerTransform);
	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 1.8f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.7f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(1.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.01f; // 캐릭터가 오를 수 있는 계단의 최대 높이
	m_pGameInstance->Register_CollisionObject(this, m_pBodyCollider);
	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);
	m_pTransformCom->Set_Position(_vec3(-3017.f, -1.f, -3000.f));
	m_pTransformCom->Set_Speed(3.f);
	m_iPassIndex = AnimPass_DissolveNoCull;
	m_iWeaponPassIndex = AnimPass_Dissolve;
	m_iHP = 100;
	m_eState = Idle; 
	return S_OK;
}

void CHuman_Boss::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_NUMPAD8, InputChannel::UI))
	{
		m_eState = Counter_Start;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD9, InputChannel::UI))
	{
		m_pTransformCom->Set_Position(_vec3(-2998.008f, -0.200f, -3006.094f));
		m_pTransformCom->LookAt_Dir(_vec4(-0.997f, 0.f, -0.042f, 0.f));
		m_eState = Pizza_Start;
	}

	if (m_pFrameEffect)
	{
		m_pFrameEffect->Tick(fTimeDelta);
	}
	if (m_pRingEffect)
	{
		m_pRingEffect->Tick(fTimeDelta);
	}
	if (m_pBaseEffect)
	{
		m_pBaseEffect->Tick(fTimeDelta);
	}
	if (m_pDimEffect)
	{
		m_pDimEffect->Tick(fTimeDelta);
	}
	if (m_pAttackEffect)
	{
		m_pAttackEffect->Tick(fTimeDelta);
	}
	if (m_pCounterEffect)
	{
		m_pCounterEffect->Tick(fTimeDelta);
	}
	if (!m_bViewModel&& m_fModelDissolveRatio < 1.f)
	{
		m_fModelDissolveRatio += fTimeDelta * 2.f;
	}
	else if (m_bViewModel && m_fModelDissolveRatio > 0.f)
	{
		m_fModelDissolveRatio -= fTimeDelta * 2.f;
	}

	if (!m_bViewWeapon && m_fWeaponDissolveRatio < 1.f)
	{
		m_fWeaponDissolveRatio += fTimeDelta * 2.f;
	}
	else if (m_bViewWeapon && m_fWeaponDissolveRatio > 0.f)
	{
		m_fWeaponDissolveRatio -= fTimeDelta * 2.f;
	}
	m_pTransformCom->Set_OldMatrix();
	Init_State(fTimeDelta);
	m_pModelCom->Set_Animation(m_Animation);
	Tick_State(fTimeDelta);
	Update_Collider();
	After_Attack(fTimeDelta);
	m_pTransformCom->Gravity(fTimeDelta);
}

void CHuman_Boss::Late_Tick(_float fTimeDelta)
{
	if (m_pFrameEffect)
	{
		m_pFrameEffect->Late_Tick(fTimeDelta);
	}
	if (m_pRingEffect)
	{
		m_pRingEffect->Late_Tick(fTimeDelta);
	}
	if (m_pBaseEffect)
	{
		m_pBaseEffect->Late_Tick(fTimeDelta);
	}
	if (m_pDimEffect)
	{
		m_pDimEffect->Late_Tick(fTimeDelta);
	}
	if (m_pCounterEffect)
	{
		m_pCounterEffect->Late_Tick(fTimeDelta);
	}
	if (m_pAttackEffect)
	{
		m_pAttackEffect->Late_Tick(fTimeDelta);
	}
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyCollider);
	m_pRendererCom->Add_DebugComponent(m_pCommonAttCollider);
#endif

}

HRESULT CHuman_Boss::Render()
{

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (i == 3)
		{
			if (m_bViewModel)
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fModelDissolveRatio, sizeof _float)))
				{
					return E_FAIL;
				}
			}
			else
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fWeaponDissolveRatio, sizeof _float)))
				{
					return E_FAIL;
				}
			}		
		}
		else
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fModelDissolveRatio, sizeof _float)))
			{
				return E_FAIL;
			}
		}
	
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

		_bool HasMaskTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", i, TextureType::Shininess)))
		{
			HasMaskTex = false;
		}
		else
		{
			HasMaskTex = true;
		}



		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
		{
			return E_FAIL;
		}

		if (i == 3)
		{
			if (FAILED(m_pShaderCom->Begin(m_iWeaponPassIndex)))
			{
				return E_FAIL;
			}

		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(m_iPassIndex)))
			{
				return E_FAIL;
			}

		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}

	}

	return S_OK;
}

void CHuman_Boss::Init_State(_float fTimeDelta)
{

	if (m_bChangePass == true)
	{
		m_fHitTime += fTimeDelta;
		m_vRimColor = Colors::Red;
		if (m_iPassIndex == AnimPass_DissolveNoCull)
		{
			m_iPassIndex = AnimPass_Rim;
			m_iWeaponPassIndex = AnimPass_Rim;
		}
		else
		{
			m_iPassIndex = AnimPass_DissolveNoCull;
			m_iWeaponPassIndex = AnimPass_Dissolve;
		}
		if (m_fHitTime >= 0.3f)
		{
			m_fHitTime = 0.f;
			m_bChangePass = false;
			m_iPassIndex = AnimPass_DissolveNoCull;
			m_iWeaponPassIndex = AnimPass_Dissolve;
		}
	}

	if (m_ePreState != m_eState)
	{
		Safe_Release(m_pBaseEffect);
		Safe_Release(m_pDimEffect);
		Safe_Release(m_pFrameEffect);
		m_Animation = {};
		m_Animation.fAnimSpeedRatio = 1.5f;
		m_bReflectOn = false;
		m_bAttacked = false;
		m_bLeftPattern = false;
		switch (m_eState)
		{
		case Client::CHuman_Boss::CommonAtt0:
		{
			_vec4 vPlayerPos = m_pPlayerTransform->Get_CenterPos();
			vPlayerPos.y = m_pTransformCom->Get_State(State::Pos).y;
			m_pTransformCom->LookAt(vPlayerPos);
			m_Animation.iAnimIndex = BossAnim_attack01;
			m_bAttacked = false;
			View_Attack_Range(Range_135);
		}
			break;
		case Client::CHuman_Boss::CommonAtt1:
		{
			m_Animation.iAnimIndex = BossAnim_attack02;
			_vec4 vLook = m_pTransformCom->Get_State(State::Look);
			vLook.x *= -1;
			vLook.y = 0;
			vLook.z *= -1;
			m_pTransformCom->LookAt_Dir(vLook);
			View_Attack_Range(Range_135);
		}
		break;
		case Client::CHuman_Boss::CommonAtt2:
			m_Animation.iAnimIndex = BossAnim_attack03;
			View_Attack_Range(Range_360);
			break;
		case Client::CHuman_Boss::Pizza_Start:
			m_Animation.iAnimIndex = BossAnim_attack13;
			m_Animation.fDurationRatio = 0.5f;
			m_iPizzaAttCount = 0;
			m_fAttackRange = 0.f;
			m_Animation.bSkipInterpolation = true;
			break;
		case Client::CHuman_Boss::Pizza_Loop:
			m_Animation.iAnimIndex = BossAnim_attack13;
			m_Animation.fDurationRatio = 0.5f;
			m_Animation.fStartAnimPos = 100.f;
			m_Animation.bSkipInterpolation = true;
			break;
		case Client::CHuman_Boss::Pizza_BackLoop:
			m_Animation.iAnimIndex = BossAnim_attack13;
			m_Animation.fDurationRatio = 0.65f;
			m_Animation.fStartAnimPos = 140.f;
			m_Animation.bRewindAnimation = true;
			m_Animation.bSkipInterpolation = true;
			break;
		case Client::CHuman_Boss::Pizza_End:
			m_Animation.iAnimIndex = BossAnim_attack13;
			m_Animation.fStartAnimPos = 100.f;
			View_Attack_Range(Range_360);
			break;
		case Client::CHuman_Boss::Counter_Start:
			m_bViewWeapon = true;
			m_bCounter_Success = false;
			m_Animation.iAnimIndex = BossAnim_attack05;
			m_Animation.bSkipInterpolation = false;
			break;
		case Client::CHuman_Boss::Counter_Fail:
			m_Animation.bSkipInterpolation = false;
			m_Animation.iAnimIndex = BossAnim_Die;
			m_Animation.fDurationRatio = 0.5f;
			Set_Damage(1000, 0);
			break;
		case Client::CHuman_Boss::Hide_Start:
			m_Animation.iAnimIndex = BossAnim_attack06_Start;
			m_bViewWeapon = false;
			m_Animation.isLoop = false;
			break;
		case Client::CHuman_Boss::Hide:
			m_Animation.iAnimIndex = BossAnim_attack06_Loop;
			m_Animation.isLoop = true;
			break;
		case Client::CHuman_Boss::Hide_Att:
		{
			m_bViewModel = true;
			m_bViewWeapon = true;
			m_Animation.fAnimSpeedRatio = 2.f;
			m_Animation.iAnimIndex = BossAnim_attack06_End;
		}
		break;
		case Client::CHuman_Boss::Throw_Sickle:
		{
			m_Animation.iAnimIndex = BossAnim_attack07;
			m_bViewWeapon = true;
		}
			break;
		case Client::CHuman_Boss::Hit:
			break;
		case Client::CHuman_Boss::Idle:
		{
			m_Animation.iAnimIndex = BossAnim_Idle;
			m_Animation.isLoop = true;
			m_fPatternDelay = 0.f;
		}
			break;
		case Client::CHuman_Boss::Walk:
			break;
		case Client::CHuman_Boss::Roar:
			break;
		case Client::CHuman_Boss::Run:
			m_Animation.iAnimIndex = BossAnim_Walk;
			m_Animation.fAnimSpeedRatio = 3.f;
			m_Animation.isLoop = true;
			m_fPatternDelay = 0.f;
			break;
		case Client::CHuman_Boss::Die:
			break;
		case Client::CHuman_Boss::Spwan:
			break;
		default:
			break;
		}
		m_ePreState = m_eState;
	}

}

void CHuman_Boss::Tick_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case CommonAtt0:
		if (m_pModelCom->IsAnimationFinished(BossAnim_attack01))
		{
			m_eState = CommonAtt1;
		}
		break;
	case CommonAtt1:
		if (m_pModelCom->IsAnimationFinished(BossAnim_attack02))
		{
			m_eState = CommonAtt2;
		}
		break;
	case CommonAtt2:
		if (m_pModelCom->IsAnimationFinished(BossAnim_attack03))
		{
			m_eState = Idle;
		}
		break;
	case Client::CHuman_Boss::Counter_Start:
		if (m_pModelCom->IsAnimationFinished(BossAnim_attack05))
		{
			m_eState = Idle;
		}
		break;
	case Client::CHuman_Boss::Counter_Fail:
		if (m_pModelCom->IsAnimationFinished(BossAnim_Die))
		{
			m_eState = Idle;
		}
		break;
	case Hide_Start:
		if (m_pModelCom->IsAnimationFinished(BossAnim_attack06_Start))
		{
			m_eState = Hide;
		}
		break;
	case Hide:
		break;
	case Hide_Att:
		if (m_pModelCom->IsAnimationFinished(BossAnim_attack06_End))
		{
			m_eState = Idle;
		}
		break;
	case Pizza_Start:
		if (m_pModelCom->IsAnimationFinished(BossAnim_attack13))
		{
			m_eState = Pizza_BackLoop;
		}
		
		break;
	case Pizza_Loop:
		if (m_pModelCom->IsAnimationFinished(BossAnim_attack13))
		{
			m_eState = Pizza_BackLoop;
			m_iPizzaAttCount++;
		}
		break;
	case Pizza_BackLoop:
			if (m_pModelCom->IsAnimationFinished(BossAnim_attack13))
			{
				m_iPizzaAttCount++;
				if (m_iPizzaAttCount >= 7)
				{
					m_eState = Pizza_End;
				}
				else
				{
					m_eState = Pizza_Loop;
				}
			}
		break;
	case Pizza_End:
		if (m_pModelCom->IsAnimationFinished(BossAnim_attack13))
		{
			m_eState = Idle;
		}
		break;
	case Throw_Sickle:
		if (m_pModelCom->IsAnimationFinished(BossAnim_attack07))
		{
			m_eState = Idle;
		}
		break;
	case Run:
	{
		m_fPatternDelay += fTimeDelta;
		if (m_fPatternDelay > 4.f or Compute_Distance() <2.5f)
		{
			Set_Pattern();
			return;
		}
		_vec4 vPlayerPos = m_pPlayerTransform->Get_CenterPos();
		vPlayerPos.y = m_pTransformCom->Get_State(State::Pos).y;
		m_pTransformCom->LookAt(vPlayerPos);
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
		break;
	case Idle:
		m_fPatternDelay += fTimeDelta;
		if (m_fPatternDelay > 2.5f)
		{
			m_eState = Run;
			return;
		}
		break;
	case Spwan:
		break;
	default:
		break;
	}

}

HRESULT CHuman_Boss::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(2.f, 2.f, 2.f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pBodyCollider, &BodyCollDesc)))
		return E_FAIL;

	// Frustum
	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Sphere;
	ColDesc.vCenter = {};
	ColDesc.fRadius = 11.3f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pCommonAttCollider), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CHuman_Boss::Update_Collider()
{
	m_pBodyCollider->Update(m_pTransformCom->Get_World_Matrix());
	m_pCommonAttCollider->Update(m_pTransformCom->Get_World_Matrix());
}

void CHuman_Boss::Set_Damage(_int iDamage, _uint MonAttType)
{
	if (m_eState == Hide)
	{
		return;
	}
	if (m_bReflectOn)
	{
		(_int)iDamage /= 3;
		m_pGameInstance->Attack_Player(nullptr, iDamage, MonAtt_Hit);
		return;
	}

	switch ((ATTACK_TYPE)MonAttType)
	{
	case Client::AT_Sword_Common:
		break;
	case Client::AT_Sword_Skill1:
		break;
	case Client::AT_Sword_Skill2:
		break;
	case Client::AT_Sword_Skill3:
		m_bCounter_Success = true;
		break;
	case Client::AT_Sword_Skill4:
		break;
	case Client::AT_Bow_Common:
		break;
	case Client::AT_Bow_Skill1:
		break;
	case Client::AT_Bow_Skill2:
		break;
	case Client::AT_Bow_Skill3_Start:
		break;
	case Client::AT_Bow_Skill3:
		break;
	case Client::AT_Bow_Skill4:
		break;
	case Client::AT_Bow_SkillR:
		break;
	case Client::AT_Critical:
		break;
	case Client::AT_End:
		break;
	default:
		break;
	}
	m_bChangePass = true;

}

void CHuman_Boss::View_Attack_Range(ATTACK_RANGE Range, _float fRotationY)
{
	Safe_Release(m_pBaseEffect);
	Safe_Release(m_pDimEffect);
	Safe_Release(m_pFrameEffect);

	_mat EffectMatrix{};
	EffectInfo Info{};
	switch (Range)
	{
	case Range_45:
	{
		if (fRotationY == 0.f)
		{
			EffectMatrix = _mat::CreateScale(30.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.25f, 0.f));
		}
		else
		{
			EffectMatrix = _mat::CreateScale(30.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateRotationY(XMConvertToRadians(fRotationY)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.25f, 0.f));
		}
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_45_Frame");
		Info.pMatrix = &EffectMatrix;
		m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_45_Dim");
		Info.pMatrix = &EffectMatrix;
		m_pDimEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		m_fBaseEffectScale = 1.f;
		if (fRotationY == 0.f)
		{
			m_BaseEffectOriMat = _mat::CreateScale(m_fBaseEffectScale) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.27f, 0.f));
		}
		else
		{
			m_BaseEffectOriMat = _mat::CreateScale(m_fBaseEffectScale) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateRotationY(XMConvertToRadians(fRotationY)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.27f, 0.f));
		}
		m_BaseEffectMat = m_BaseEffectOriMat;
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_45_Base");
		Info.pMatrix = &m_BaseEffectMat;
		Info.isFollow = true;
		m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
		break;
	}
	case Range_90:
	{
		if (fRotationY == 0.f)
		{
			EffectMatrix = _mat::CreateScale(30.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.25f, 0.f));
		}
		else
		{
			EffectMatrix = _mat::CreateScale(30.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateRotationY(XMConvertToRadians(fRotationY)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.25f, 0.f));
		}
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_90_Frame");
		Info.pMatrix = &EffectMatrix;
		m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_90_Dim");
		Info.pMatrix = &EffectMatrix;
		m_pDimEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		m_fBaseEffectScale = 1.f;
		if (fRotationY == 0.f)
		{
			m_BaseEffectOriMat = _mat::CreateScale(m_fBaseEffectScale) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.27f, 0.f));
		}
		else
		{
			m_BaseEffectOriMat = _mat::CreateScale(m_fBaseEffectScale) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateRotationY(XMConvertToRadians(fRotationY)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.27f, 0.f));
		}
		m_BaseEffectMat = m_BaseEffectOriMat;
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_90_Base");
		Info.pMatrix = &m_BaseEffectMat;
		Info.isFollow = true;
		m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
		break;
	}
	case Range_135:
	{
		if (fRotationY == 0.f)
		{
			EffectMatrix = _mat::CreateScale(30.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.25f, 0.f));
		}
		else
		{
			EffectMatrix = _mat::CreateScale(30.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateRotationY(XMConvertToRadians(fRotationY)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.25f, 0.f));
		}	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_135_Frame");
		Info.pMatrix = &EffectMatrix;
		m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_135_Dim");
		Info.pMatrix = &EffectMatrix;
		m_pDimEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		m_fBaseEffectScale = 1.f;
		if (fRotationY == 0.f)
		{
			m_BaseEffectOriMat = _mat::CreateScale(m_fBaseEffectScale) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.27f, 0.f));
		}
		else
		{
			m_BaseEffectOriMat = _mat::CreateScale(m_fBaseEffectScale) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateRotationY(XMConvertToRadians(fRotationY)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.27f, 0.f));
		}
		m_BaseEffectMat = m_BaseEffectOriMat;
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_135_Base");
		Info.pMatrix = &m_BaseEffectMat;
		Info.isFollow = true;
		m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
		break;
	}
	case Range_360:
	{
		if (fRotationY == 0.f)
		{
			EffectMatrix = _mat::CreateScale(30.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.25f, 0.f));
		}
		else
		{
			EffectMatrix = _mat::CreateScale(30.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateRotationY(XMConvertToRadians(fRotationY)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.25f, 0.f));
		}	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Frame");
		Info.pMatrix = &EffectMatrix;
		m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Dim");
		Info.pMatrix = &EffectMatrix;
		m_pDimEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		m_fBaseEffectScale = 1.f;
		if (fRotationY == 0.f)
		{
			m_BaseEffectOriMat = _mat::CreateScale(m_fBaseEffectScale) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.27f, 0.f));
		}
		else
		{
			m_BaseEffectOriMat = _mat::CreateScale(m_fBaseEffectScale) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateRotationY(XMConvertToRadians(fRotationY)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.27f, 0.f));
		}	m_BaseEffectMat = m_BaseEffectOriMat;
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Base");
		Info.pMatrix = &m_BaseEffectMat;
		Info.isFollow = true;
		m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
		break;
	}
	default:
		break;
	}

}

void CHuman_Boss::After_Attack(_float fTimedelta)
{

	if (m_eState == CommonAtt0)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 0.f && Index <= 49.f)
		{
			Increased_Range(50.f, fTimedelta);
		}

		if (Index >= 20.f && Index < 22.f && !m_bViewWeapon)
		{
			m_bViewWeapon = true;
		}
		else if (Index >= 49.f && Index < 51.f)
		{
			Safe_Release(m_pBaseEffect);
			Safe_Release(m_pDimEffect);
			Safe_Release(m_pFrameEffect);
		}
		else if (Index >= 51.f && Index <= 61.f)
		{
			if (!m_bAttacked)
			{
				if (Compute_Angle(135.f))
				{
					m_pGameInstance->Attack_Player(m_pCommonAttCollider, 200, MonAtt_KnockDown);
					if (m_pGameInstance->CheckCollision_Player(m_pCommonAttCollider))
					{
						m_bAttacked = true;
					}
				}
			}
			if (m_bAttacked)
			{
				_vec3 vLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();
			}
		}
		else if (Index >= 61.f && Index <= 63.f)
		{
			Safe_Release(m_pAttackEffect);
		}
		else if (Index >= 127.f && m_bViewWeapon)
		{
			m_bViewWeapon = false;

		}
	}
	else if (m_eState == CommonAtt1)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 0.f && Index <= 57.f)
		{
			Increased_Range(58.f, fTimedelta);
		}
		if (Index >= 8.f && Index < 10.f && !m_bViewWeapon)
		{
			m_bViewWeapon = true;
		}
		else if (Index >= 55.f && Index < 57.f)
		{
			Safe_Release(m_pBaseEffect);
			Safe_Release(m_pDimEffect);
			Safe_Release(m_pFrameEffect);
		}
		else if (Index >= 57.f && Index <= 67.f)
		{
			if (!m_bAttacked)
			{
				if (Compute_Angle(135.f))
				{
					m_pGameInstance->Attack_Player(m_pCommonAttCollider, 200, MonAtt_KnockDown);
					if (m_pGameInstance->CheckCollision_Player(m_pCommonAttCollider))
					{
						m_bAttacked = true;
					}
				}
			}
		}
		else if (Index >= 116.f && m_bViewWeapon)
		{
			m_bViewWeapon = false;
		}
	}
	else if (m_eState == CommonAtt2)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 0.f && Index < 115.f)
		{

			if (m_fBaseEffectScale < 30.f)
			{
				m_fBaseEffectScale += fTimedelta * 12.f;
			}

			m_BaseEffectMat = _mat::CreateScale(m_fBaseEffectScale) * m_BaseEffectOriMat;
		}
		if (Index >= 8.f && Index <= 11.f && !m_bViewWeapon)
		{
			m_bViewWeapon = true;
		}
		else if (Index >= 113.f && Index < 115.f)
		{
			Safe_Release(m_pBaseEffect);
			Safe_Release(m_pDimEffect);
			Safe_Release(m_pFrameEffect);
		}
		else if (Index >= 115.f && Index <= 120.f)
		{
			if (!m_bAttacked)
			{
				m_pGameInstance->Attack_Player(m_pCommonAttCollider, 200, MonAtt_KnockDown);
				if (m_pGameInstance->CheckCollision_Player(m_pCommonAttCollider))
				{
					m_bAttacked = true;
				}
			}
		}
		else if (Index >= 186.f && m_bViewWeapon)
		{
			m_bViewWeapon = false;
		}
	}
	else if (m_eState == Hide_Start)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 48.f && !m_bHide)
		{
			m_bHide = true;
			m_bViewModel = true;
		}
	}
	else if (m_eState == Hide_Att)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();

		if (Index >= 0.f && Index <= 49.f)
		{
			Increased_Range(50.f, fTimedelta);
		}
		else if (Index >= 49.f && Index <= 51.f)
		{
			Safe_Release(m_pBaseEffect);
			Safe_Release(m_pDimEffect);
			Safe_Release(m_pFrameEffect);
		}
		if (Index >= 0.f && Index <= 2.f)
		{
			if (!m_bAttacked)
			{
				_vec3 vPos = m_pPlayerTransform->Get_State(State::Pos) - (m_pPlayerTransform->Get_State(State::Look).Get_Normalized() * 1.7f);
				m_pTransformCom->Set_FootPosition(vPos);
				_vec4 vPlayerPos = m_pPlayerTransform->Get_State(State::Pos);
				vPlayerPos.y = m_pPlayerTransform->Get_State(State::Pos).y;
				m_pTransformCom->LookAt(vPlayerPos);
				View_Attack_Range(Range_135);
				m_bAttacked = true;
			}
		}
		else if (Index >= 2.f && Index <= 4.f)
		{
			m_bAttacked = false;
		}
		else if (Index >= 49.f && Index <= 53.f)
		{
			if (!m_bAttacked)
			{
				if (Compute_Angle(135.f))
				{
					m_pGameInstance->Attack_Player(m_pCommonAttCollider, 250, MonAtt_KnockDown);
					if (m_pGameInstance->CheckCollision_Player(m_pCommonAttCollider))
					{
						m_bAttacked = true;
					}
				}

			}
		}
	}
	else if (m_eState == Counter_Start)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();

		if (Index >= 70.f && Index <= 72.f)
		{
			if (!m_bAttacked)
			{
				if ((m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_SafeZone"), TEXT("Prototype_GameObject_SafeZone"))))
				{
					MSG_BOX("SafeZone 불러오기 실패");
					return;
				}
				m_bAttacked = true;
			}
		}
		else if (Index >= 74.f && Index <= 76.f)
		{
			m_bAttacked = false;
		}
		else if (Index >= 181.f && Index <= 183.f)
		{
			m_bAttacked = false;
		}

		if (Index >= 100.f && Index <= 160.f)
		{
			if (!m_bAttacked)
			{
				_uint iRandom = rand() % 2;
				_mat EffectMat{};
				EffectInfo Info{};
				if (iRandom)
				{
					Safe_Release(m_pCounterEffect);
					EffectMat = _mat::CreateScale(3.f) * m_pTransformCom->Get_World_Matrix();
					Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Human_Counter");
					Info.pMatrix = &EffectMat;
					m_pCounterEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
					m_vRimColor = _vec4(0.13f, 0.13f, 0.89f, 1.f);
					m_iPassIndex = AnimPass_Rim;
					m_iWeaponPassIndex = AnimPass_Rim;
				}
				else
				{
					Safe_Release(m_pCounterEffect);
					EffectMat = _mat::CreateScale(3.f) * m_pTransformCom->Get_World_Matrix();
					Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Reflect_Counter");
					Info.pMatrix = &EffectMat;
					m_pCounterEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
					m_vRimColor = _vec4(0.969f, 0.957f, 0.235f, 1.f);
					m_iPassIndex = AnimPass_Rim;
					m_iWeaponPassIndex = AnimPass_Rim;
					m_bReflectOn = true;
				}
				m_bAttacked = true;
			}
			if (!m_bReflectOn)
			{
				if (m_bCounter_Success)
				{
					m_pGameInstance->Play_Sound(TEXT("Counter"), 0.7f);
					m_eState = Counter_Fail;
					m_bCounter_Success = false;
					m_iPassIndex = AnimPass_DissolveNoCull;
					m_iWeaponPassIndex = AnimPass_Dissolve;
					return;
				}
			}
		}
		else if (Index >= 161.f && Index <= 163.f)
		{
			m_iPassIndex = AnimPass_DissolveNoCull;
			m_iWeaponPassIndex = AnimPass_Dissolve;
			m_bAttacked = false;
		}
		else if (Index >= 183.f && Index <= 186.f)
		{
			Safe_Release(m_pBaseEffect);
			Safe_Release(m_pDimEffect);
			Safe_Release(m_pFrameEffect);
			CCollider* pPlayerCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Player_Hit_OBB"));
			CCollider* pSafeZoneCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_SafeZone"), TEXT("Com_SafeZone_Collider"));
			if (m_bReflectOn)
			{
				if (!pPlayerCollider->Intersect(pSafeZoneCollider))
				{
					if (!m_bAttacked)
					{
						m_pGameInstance->Attack_Player(nullptr, 250, MonAtt_KnockDown);
						m_bAttacked = true;
					}
				}
				m_bReflectOn = false;
			}
			else
			{
				if (!m_bAttacked)
				{
					if (!m_bAttacked)
					{
						m_pGameInstance->Attack_Player(nullptr, 250, MonAtt_KnockDown);
						m_bAttacked = true;
					}
				}
			}

		}
		else
		{
			m_bCounter_Success = false;
		}
	}
	else if (m_eState == Pizza_Loop)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 100.f && Index <137.f)
		{
			if (!m_bAttacked)
			{
				if (rand() % 2 == 1)
				{
					m_fAttackRange += 45.f;
				}
				else
				{
					m_fAttackRange -= 45.f;
				}
				View_Attack_Range(Range_45, m_fAttackRange);
				m_bAttacked = true;
			}
			Increased_Range(36.f, fTimedelta);
		}
		else if (Index >= 137.f && Index <= 140.f)
		{
			if (m_bAttacked)
			{
				if (!Compute_Angle(45.f, m_fAttackRange))
				{
					m_pGameInstance->Attack_Player(nullptr, 50, MonAtt_Hit);
				}
				m_bAttacked = false;
			}
		}
	}
	else if (m_eState == Pizza_BackLoop)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index > 103.f && Index < 140.f)
		{
			if (!m_bAttacked)
			{
				if (rand() % 2 == 1)
				{
					m_fAttackRange += 45.f;
				}
				else
				{
					m_fAttackRange -= 45.f;
				}
				View_Attack_Range(Range_45, m_fAttackRange);
				m_bAttacked = true;
			}
			Increased_Range(36.f, fTimedelta);
		}
		else if (Index >= 100.f && Index <= 103.f)
		{
			if (m_bAttacked)
			{
				if (!Compute_Angle(45.f, m_fAttackRange))
				{
					m_pGameInstance->Attack_Player(nullptr, 50, MonAtt_Hit);
				}
				m_bAttacked = false;
			}
		}
	}
	else if (m_eState == Pizza_End)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index > 100.f && Index < 167.f)
		{
			Increased_Range(67.f, fTimedelta);
		}
		else if (Index > 167.f && Index < 170.f)
		{
			if (!m_bAttacked)
			{
				Safe_Release(m_pBaseEffect);
				Safe_Release(m_pDimEffect);
				Safe_Release(m_pFrameEffect);
				m_pGameInstance->Attack_Player(m_pCommonAttCollider, 50, MonAtt_KnockDown);
				m_bAttacked = true;
			}
		}
	}
	else if (m_eState == Throw_Sickle)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >=96.f && Index < 98.f)
		{
			if (!m_bAttacked)
			{
				m_bViewWeapon = false;
				_vec3 vPos = m_pTransformCom->Get_State(State::Pos);
				m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Sickle"), TEXT("Prototype_GameObject_Sickle"), &vPos);
				m_bAttacked = true;
				
			}
		}
		if (Index >= 190.f && Index < 192.f)
		{
			m_bViewWeapon = true;
		}
	}
	if (m_bHide && m_fHideTimmer < 7.f)
	{
		m_fHideTimmer += fTimedelta;
	}
	else if (m_bHide && m_fHideTimmer >= 7.f)
	{
		m_bHide = false;
		m_fHideTimmer = 0.f;
		m_eState = Hide_Att;
	}
}

_bool CHuman_Boss::Compute_Angle(_float fAngle, _float RotationY)
{
	_float fLowerBound{};
	_float fUpperBound{};
	fAngle /= 2.f;
	if (RotationY < 0.f)
	{
		RotationY = 360.f + RotationY;
		
		fLowerBound = RotationY - fAngle;
		fUpperBound = RotationY + fAngle;
	}
	else if (RotationY == 0.f)
	{
		fLowerBound = 360.f - fAngle;
		fUpperBound =  fAngle;
	}
	else
	{
		
		fLowerBound = RotationY - fAngle;
		fUpperBound = RotationY + fAngle;
	}

	if (fLowerBound < 0.f)
	{
		fLowerBound = 360.f + fLowerBound;
	}
	else if (fLowerBound > 360.f)
	{
		fLowerBound -= 360.f;
	}
	if (fUpperBound < 0.f)
	{
		fUpperBound = 360.f + fUpperBound;
	}
	else if (fUpperBound > 360.f)
	{
		fUpperBound -= 360.f;
	}
	_vec4 vLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();

	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
	_vec4 vPlayerPos = m_pPlayerTransform->Get_State(State::Pos);
	vPlayerPos.y = vPos.y;
	_vec4 vDir = vPlayerPos - vPos;
	vDir.Normalize();

	_float fResult = vLook.Dot(vDir);
	if (fResult == 0.f or fResult>1.f)
	{
		fResult = 0.01f;
	}

	_float angleInRadians = acos(fResult);
	_float angleInDegrees = angleInRadians * (180.0f / XM_PI);

	_vec4 vRight = m_pTransformCom->Get_State(State::Right);

	fResult = vRight.Dot(vDir);
	
	if (fResult < 0)
	{	
		angleInDegrees = 360.f - angleInDegrees;
	}

	if (fLowerBound > fUpperBound)
	{
		fLowerBound -= 360.f;
		//if (angleInDegrees < 0.f)
		//{
		//	angleInDegrees -= 360.f;
		//}
	}
	return (angleInDegrees >= fLowerBound && angleInDegrees <= fUpperBound);
}

void CHuman_Boss::Increased_Range(_float Index, _float fTImeDelta, _float fRotationY)
{
	_float fIncrease = 1400.f / Index;

	if (m_fBaseEffectScale < 30.f)
	{
		m_fBaseEffectScale += fTImeDelta * fIncrease;
	}

	m_BaseEffectMat = _mat::CreateScale(m_fBaseEffectScale) * m_BaseEffectOriMat;
}

void CHuman_Boss::Set_Pattern()
{
	if (m_vecPattern.size() == 0)
	{
		for (int i = 0; i < 5; i++)
		{
			m_vecPattern.push_back(i);
		}
	}

	

	_bool Duplication = true;
	while (Duplication)
	{
		_uint iRandom = rand() % 5;
		vector<int>::iterator it = std::find(m_vecPattern.begin(), m_vecPattern.end(), iRandom);
		if (it != m_vecPattern.end())
		{
			switch (iRandom)
			{
			case 0: m_eState = CommonAtt0;
				break;
			case 1: m_eState = Hide_Start;
				break;
			case 2: m_eState = Counter_Start;
				break;
			case 3: m_eState = Throw_Sickle;
				break;
			case 4: m_eState = Pizza_Start;
				break;
			default:
				break;
			}
			m_vecPattern.erase(it);
			Duplication = false;
		}

	}
}

_float CHuman_Boss::Compute_Distance()
{
	_vec4 vPlayerPos = m_pPlayerTransform->Get_CenterPos();
	_vec4 vPos = m_pTransformCom->Get_CenterPos();
	vPlayerPos.y = vPos.y;
	
	return _vec3::Distance(_vec3(vPos), _vec3(vPlayerPos));
}

HRESULT CHuman_Boss::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_VILLAGE, TEXT("Prototype_Model_Human_Boss"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Logo_Noise"), TEXT("Com_Dissolve_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CHuman_Boss::Bind_ShaderResources()
{
	if (m_iPassIndex == AnimPass_Rim)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &m_vRimColor, sizeof m_vRimColor)))
		{
			return E_FAIL;
		}
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
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
	if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
	{
		return E_FAIL;
	}

	return S_OK;
}

CHuman_Boss* CHuman_Boss::Create(_dev pDevice, _context pContext)
{
	CHuman_Boss* pInstance = new CHuman_Boss(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CHuman_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHuman_Boss::Clone(void* pArg)
{
	CHuman_Boss* pInstance = new CHuman_Boss(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CHuman_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHuman_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pCounterEffect);
	Safe_Release(m_pRingEffect);
	Safe_Release(m_pDimEffect);
	Safe_Release(m_pBaseEffect);
	Safe_Release(m_pFrameEffect);
	Safe_Release(m_pAttackEffect);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBodyCollider);
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pCommonAttCollider);
	Safe_Release(m_pDissolveTextureCom);

}
