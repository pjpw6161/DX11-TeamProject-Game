#include "Player.h"
#include "UI_Manager.h"
#include "Event_Manager.h"
#include "Arrow.h"
#include "FadeBox.h"
#include "HitEffect.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"
#include "Camera_Manager.h"
#include "Dialog.h"
#include "TextButtonColor.h"

CPlayer::CPlayer(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Init_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_pTransformCom->Set_Scale(_vec3(4.f));
	Place_PartModels();
	m_pCameraTransform = GET_TRANSFORM("Layer_Camera", LEVEL_STATIC);
	Safe_AddRef(m_pCameraTransform);
	m_SwordSkill[0] = Anim_RA_9100_Ambush; // x자로 공격하기
	m_SwordSkill[1] = Anim_RA_9060_SealChain; // 앞으로 점프하면서 때리기
	m_SwordSkill[2] = Anim_RA_9040_RapidAttack; // 사라졌다가 찌르기
	m_SwordSkill[3] = Anim_RA_9050_SealStack; // 난타(쿨김)
	m_SwordSkill[4] = Anim_RA_9080_Hiding; // 은신(우클릭)

	m_BowSkill[0] = Anim_ID_8070_TripleStrike; // 트리플 샷 (주력기)
	m_BowSkill[1] = Anim_ID_8080_BackTumbling; // 백덤블링
	m_BowSkill[2] = Anim_ID_8120_RainArrow; // 화살비
	m_BowSkill[3] = Anim_ID_8130_IllusionArrow; // 분신 나와서 화살(쿨김)
	m_BowSkill[4] = Anim_RS_8110_DodgeAttack; // 에임모드 변경(우클릭)

	Change_Parts(PT_BODY, 1);
	Change_Parts(PT_HAIR, 0);
	Change_Parts(PT_FACE, 0);

	m_pGameInstance->Register_CollisionObject(this, m_pHitCollider, true, m_pAttCollider[AT_Bow_Common], m_pParryingCollider);

	SURFACETRAIL_DESC Desc{};
	Desc.vColor = _color(1.f, 0.5f, 0.1f, 1.f);

	Desc.iNumVertices = 8;
	m_pLeft_Trail = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);
	m_pRight_Trail = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	Desc.iPassIndex = 2;
	Desc.strMaskTextureTag = L"FX_J_Noise_Normal004_Tex";
	m_pLeft_Distortion_Trail = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);
	m_pRight_Distortion_Trail = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	//Desc.vColor = Colors::LightGreen;
	//m_pLeft_Trail[1] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);
	//m_pRight_Trail[1] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	//Desc.vColor = Colors::DarkRed;
	//m_pLeft_Trail[2] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);
	//m_pRight_Trail[2] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);
	
	//Desc.vColor = Colors::MediumPurple;
	//m_pLeft_Trail[3] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);
	//m_pRight_Trail[3] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	//Desc.vColor = _vec4(1.f, 0.5f, 0.1f, 1.f);
	//m_pLeft_Trail[4] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);
	//m_pRight_Trail[4] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	m_Left_Mat = m_pModelCom->Get_BoneMatrix("B_Weapon_L");
	m_Right_Mat = m_pModelCom->Get_BoneMatrix("B_Weapon_R");

	m_ShaderIndex = VTFPass_Dissolve;
	m_HairShaderIndex = VTFPass_LerpDissolve;

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_PLAYER);
	m_bReadyMove = true;
	m_Current_GroundRiding = Horse;
	m_Current_AirRiding = Bird;

	m_pCam_Manager = CCamera_Manager::Get_Instance();

	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Point;
	LightDesc.vAttenuation = LIGHT_RANGE_32;
	LightDesc.vDiffuse = _vec4(0.15f);
	LightDesc.vAmbient = _vec4(0.05f);
	LightDesc.vSpecular = _vec4(1.f);
	LightDesc.vPosition = m_pTransformCom->Get_CenterPos();

	if (FAILED(m_pGameInstance->Add_Light(LEVEL_STATIC, TEXT("Light_Player"), LightDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{

	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return;
	}

	if (m_pCam_Manager->Get_CameraState() == CS_WORLDMAP)
	{
		//return;
	}

	if (!m_bFirstVillage && m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_VILLAGE)
	{
		m_eState = Idle;
		m_bIsClimb = false;
		m_bFirstVillage = true;
	}

	if (m_bHide && m_fDissolveRatio < 1.f)
	{
		m_fDissolveRatio += fTimeDelta * 5.f;
	}
	else if (!m_bHide && m_fDissolveRatio > 0.f)
	{
		m_fDissolveRatio -= fTimeDelta * 4.f;
	}




	if (m_bHitted)
	{
		m_fRimRightTimmer += fTimeDelta;
		if (m_ShaderIndex == VTFPass_Dissolve)
		{
			m_ShaderIndex = VTFPass_Main_Rim;
			m_HairShaderIndex = VTFPass_Main_Rim;

		}
		else if (m_ShaderIndex == VTFPass_Main_Rim)
		{
			m_ShaderIndex = VTFPass_Dissolve;
			m_HairShaderIndex = VTFPass_LerpDissolve;
		}
		if (m_fRimRightTimmer >= 0.4f)
		{
			m_fRimRightTimmer = 0.f;
			m_bHitted = false;
			m_ShaderIndex = VTFPass_Dissolve;
			m_HairShaderIndex = VTFPass_LerpDissolve;
		}
	}

	TeleportSpot eTeleportSpot{};
	if (CUI_Manager::Get_Instance()->Is_Teleport(&eTeleportSpot))
	{
		CUI_Manager::Get_Instance()->Set_Teleport(false);
		Riding_Desc eRiding_Desc{};
		m_Current_AirRiding = CUI_Manager::Get_Instance()->Get_Riding(VC_FLY);
		eRiding_Desc.Type = m_Current_AirRiding;
		Summon_Riding(eRiding_Desc);
		m_bIsSkying = true;
		m_eTeleportSpot = eTeleportSpot;
	}
	
	if (m_bIsSkying)
	{
		if (m_fFadeTimmer > 1.f)
		{
			CFadeBox::FADE_DESC Desc = {};
			Desc.fIn_Duration = 1.4f;
			Desc.phasFadeCompleted = &m_bReady_Teleport;
			CUI_Manager::Get_Instance()->Add_FadeBox(Desc);
			m_fFadeTimmer = 0.f;
			m_bIsSkying = false;
		}
		m_fFadeTimmer += fTimeDelta;
	}
	if (m_bReady_Teleport)
	{
		m_pCam_Manager->Set_RidingZoom(false);
		m_pCam_Manager->Set_FlyCam(false);

		m_bIsMount = false;
		Safe_Release(m_pRiding);
		CTrigger_Manager::Get_Instance()->Teleport(m_eTeleportSpot);
		m_bReady_Teleport = false;
		Riding_Desc eRiding_Desc{};
		eRiding_Desc.Type = m_Current_AirRiding;
		eRiding_Desc.bLanding = true;
		eRiding_Desc.vSummonPos = m_pTransformCom->Get_CenterPos();
		Summon_Riding(eRiding_Desc);
		CFadeBox::FADE_DESC Desc = {};
		Desc.fIn_Duration = 0.f;
		Desc.fOut_Duration = 1.5f;
		CUI_Manager::Get_Instance()->Add_FadeBox(Desc);
	}

	if (m_pFrameEffect)
	{
		m_pFrameEffect->Tick(fTimeDelta);
	}

	if (m_pBaseEffect)
	{
		m_pBaseEffect->Tick(fTimeDelta);
	}

	if (m_bPoison)
	{
		if (m_fRimTick > 0.3f)
		{
			if (m_iPoisonCount < 8)
			{
				m_fRimRightTimmer += fTimeDelta;
				if (m_ShaderIndex == VTFPass_Dissolve)
				{
					m_ShaderIndex = VTFPass_Main_Rim;
					m_HairShaderIndex = VTFPass_Main_Rim;
					Set_Damage(m_iPoisionDamage, MonAtt_Poison);
				}
				else if (m_ShaderIndex == VTFPass_Main_Rim)
				{
					m_ShaderIndex = VTFPass_Dissolve;
					m_HairShaderIndex = VTFPass_LerpDissolve;
				}
				if (m_fRimRightTimmer >= 0.5f)
				{
					m_fRimRightTimmer = 0.f;
					m_ShaderIndex = VTFPass_Dissolve;
					m_HairShaderIndex = VTFPass_LerpDissolve;
				}
				m_iPoisonCount++;
				m_fRimTick = 0.f;
			}
			else
			{
				m_bPoison = false;
				m_iPoisonCount = 0;
				m_fRimTick = 0.f;
			}
		}
		else
		{
			m_fRimTick += fTimeDelta;
		}
	}
	if (m_bSlowSpeed > 0.f)
	{
		m_bSlowSpeed -= fTimeDelta * 0.5f;
		m_Animation.fAnimSpeedRatio = 2.f - (m_bSlowSpeed * 0.3f);
		m_pModelCom->Set_Animation(m_Animation);
	}
	else
	{
		m_bSlowSpeed = 0.f;
	}
	
	if (m_fDissolveRatio < 0.f)
	{
		m_fDissolveRatio = 0.f;
	}

	if (m_fBoostSpeed > 0.f && m_fBoostSpeedTimmer > 0.f)
	{
		m_fBoostSpeedTimmer -= fTimeDelta;
	}
	else
	{
		m_fBoostSpeed = 0.f;
	}


	m_StartRegen += fTimeDelta;
	m_fAttTimer += fTimeDelta;


	m_pTransformCom->Set_OldMatrix();
	dynamic_cast<CTransform*>(Find_Component(L"Com_Transform"))->Set_OldMatrix();

	if (m_bStartGame)
	{
		CEvent_Manager::Get_Instance()->Tick(fTimeDelta);

	}

	if (m_pCam_Manager->Get_CameraModeIndex() == CM_DEBUG)
	{
		return;
	}
	if (CUI_Manager::Get_Instance()->Get_Heal())
	{
		_uint iHeal = 0;
		CUI_Manager::Get_Instance()->Get_Heal(&iHeal);
		CUI_Manager::Get_Instance()->Set_Heal(false);
		m_Status.Current_Hp += iHeal;
		if (m_Status.Current_Hp > m_Status.Max_Hp)
		{
			m_Status.Current_Hp = m_Status.Max_Hp;
		}
		CUI_Manager::Get_Instance()->Set_Hp(m_Status.Current_Hp, m_Status.Max_Hp);
	}

	if (CUI_Manager::Get_Instance()->Get_MpState())
	{
		_uint iMp = 0;
		CUI_Manager::Get_Instance()->Get_MpState(&iMp);
		CUI_Manager::Get_Instance()->Set_MpState(false);
		m_Status.Current_Mp += iMp;
		if (m_Status.Current_Mp > m_Status.Max_Mp)
		{
			m_Status.Current_Mp = m_Status.Max_Mp;
		}
		CUI_Manager::Get_Instance()->Set_Mp(m_Status.Current_Mp, m_Status.Max_Mp);
	}

	PART_TYPE eType = CUI_Manager::Get_Instance()->Is_CustomPartChanged();


	if (PART_TYPE::PT_END != eType)
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f);
		if (eType == PT_SWORD || eType == PT_BOW)
		{
			WEAPON_TYPE eWpType{};
			_uint iWpIdx = CUI_Manager::Get_Instance()->Get_WeaponType(eType, &eWpType);

			Change_Weapon(eWpType, (WEAPON_INDEX)iWpIdx);

		}
		else
		{
			Change_Parts(eType, CUI_Manager::Get_Instance()->Get_CustomPart(eType));
		}

	}

	if (CUI_Manager::Get_Instance()->Is_Collecting())
	{
		m_eState = Collect_Start;
	}
	if (m_pGameInstance->Key_Down(DIK_Z))
	{
		Set_Damage(0, MonAtt_Poison);
	}

	if (m_pGameInstance->Key_Down(DIK_C))
	{

		if (m_eState == Idle or m_eState == Mount or m_eState == Mount_Run or m_eState == Mount_Walk or m_eState == Run_End)
		{

			if (!m_bIsMount)
			{

				m_Current_GroundRiding = CUI_Manager::Get_Instance()->Get_Riding(VC_GROUND);
				if (m_Current_GroundRiding != Type_End)
				{
					m_bIsMount = true;
					Riding_Desc Desc{};
					Desc.Type = m_Current_GroundRiding;
					Summon_Riding(Desc);
					m_eState = Mount;
				}
			}
			else
			{
				m_pRiding->Delete_Riding();
			}
		}
	}

	Front_Ray_Check();
	Health_Regen(fTimeDelta);
	if (m_bIsMount)
	{
		m_pRiding->Tick(fTimeDelta);
		Tick_Riding(fTimeDelta);
	}

	if (m_bIsClimb)
	{
		Is_Climb(fTimeDelta);
	}

	if (m_pCam_Manager->Get_CameraState() == CS_ZOOM && m_Animation.iAnimIndex != Anim_Talk_reaction)
	{
		m_Animation.iAnimIndex = Anim_Talk_reaction;
		m_Animation.isLoop = true;
	}
	else if (m_Animation.iAnimIndex == Anim_Talk_reaction && m_pCam_Manager->Get_CameraState() != CS_ZOOM)
	{
		m_eState = Idle;
	}



	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		if (CUI_Manager::Get_Instance()->Is_InvenActive())
		{
			m_Animation.iAnimIndex = Anim_idle_00;
			m_Animation.isLoop = true;
			m_hasJumped = false;
			m_pModelCom->Set_Animation(m_Animation);
			m_pModelCom->Play_Animation(fTimeDelta);
			m_isInvenActive = true;
			if (CUI_Manager::Get_Instance()->Set_CurrentPlayerPos(m_pTransformCom->Get_State(State::Pos)))
			{
				m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f);
				m_Animation.iAnimIndex = Anim_idle_00;
				m_Animation.isLoop = true;
				m_hasJumped = false;
				m_iSuperArmor = {};
			}

			m_pTransformCom->Set_State(State::Pos, CUI_Manager::Get_Instance()->Get_InvenPos());

			if (m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
			{
				_long dwMouseMove;

				if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
				{
					m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * dwMouseMove * -1.f * 0.1f);
				}
			}
			return;
		}

	}
	if (m_isInvenActive && !CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f);
		m_pTransformCom->Set_State(State::Pos, CUI_Manager::Get_Instance()->Get_LastPlayerPos());
		m_isInvenActive = false;
		return;
	}

	if (m_pGameInstance->Get_CurrentLevelIndex() != LEVEL_CUSTOM)
	{
	

		if (!m_bIsMount)
		{
			Move(fTimeDelta);
		}
		Init_State();
		if (!m_bIsMount)
		{
			Tick_State(fTimeDelta);
		}

		if (m_Current_Weapon == WP_SWORD)
		{
			After_SwordAtt(fTimeDelta);
		}
		else if (m_Current_Weapon == WP_BOW)
		{
			After_BowAtt(fTimeDelta);
		}

	}
	_float fMouseSensor = 0.1f;
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_CUSTOM)
	{
		m_Animation.bSkipInterpolation = true;
		m_Animation.isLoop = true;
		m_Animation.iAnimIndex = Anim_idle_00;
		if (!CUI_Manager::Get_Instance()->Is_Picking_UI() && m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
		{
			_long dwMouseMove;

			if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
			{
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * dwMouseMove * -10.f * fMouseSensor);
			}
		}
	}




	if (m_bArrowRain_Start)
	{
		Arrow_Rain();
	}

	for (int i = 0; i < AT_Bow_Skill1; i++)
	{
		_mat offset = /*_mat::CreateScale(_vec3(1.f,6.5f,1.f))* */_mat::CreateTranslation(_vec3(0.f, 1.f, 0.f));
		m_pAttCollider[i]->Update(offset * m_pTransformCom->Get_World_Matrix());
	}

	if (m_pNameTag != nullptr)
	{
		m_pNameTag->Tick(fTimeDelta);
	}

	if (m_eState == Skill4 or m_eState == Skill3 or m_eState == Skill2)
	{
		m_UsingMotionBlur = true;
	}
	else
	{
		m_UsingMotionBlur = false;
	}

	if (m_pEffect_Shield)
	{
		m_ShieldMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));
		m_pEffect_Shield->Tick(fTimeDelta);
	}

	LIGHT_DESC* pLight = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, L"Light_Player");
	if (pLight)
	{
		pLight->vDiffuse = _vec4(0.4f);
		pLight->vSpecular = _vec4(0.f);
		pLight->vPosition = m_pTransformCom->Get_CenterPos();
	}

	if (m_pGameInstance->Key_Down(DIK_J))
	{
		m_pGameInstance->Delete_Light(LEVEL_STATIC, L"Light_Player", 0.5f);
	}
	if (m_pGameInstance->Key_Down(DIK_H))
	{
		LIGHT_DESC LightDesc{};

		LightDesc.eType = LIGHT_DESC::Point;
		LightDesc.vAttenuation = LIGHT_RANGE_32;
		LightDesc.vDiffuse = _vec4(0.15f);
		LightDesc.vAmbient = _vec4(0.05f);
		LightDesc.vSpecular = _vec4(1.f);
		LightDesc.vPosition = m_pTransformCom->Get_CenterPos();

		if (FAILED(m_pGameInstance->Add_Light(LEVEL_STATIC, TEXT("Light_Player"), LightDesc)))
		{
		}
	}

	Set_ExtraStatus();
	m_pModelCom->Set_Animation(m_Animation);
	m_pParryingCollider->Update(m_pTransformCom->Get_World_Matrix());
	Update_Trail(fTimeDelta);
	m_pHitCollider->Update(m_pTransformCom->Get_World_Matrix());
#ifdef _DEBUG
	m_pGameInstance->Get_StringStream() << "카메라 전환 : P" << endl;
	m_pGameInstance->Get_StringStream() << "디버그 랜더 On/Off : F1" << endl;
	m_pGameInstance->Get_StringStream() << "플레이어 위치 등은 성능 문제로 디버그 랜더시에만 니오게 함." << endl;
	m_pGameInstance->Get_StringStream() << endl;

	if (not m_pGameInstance->IsSkipDebugRendering())
	{
		_vector Pos = m_pTransformCom->Get_State(State::Pos);
		_vector Look = m_pTransformCom->Get_State(State::Look);
		m_pGameInstance->Get_StringStream() << "플레이어 이동속도 :" << m_pTransformCom->Get_Speed() << endl;
		m_pGameInstance->Get_StringStream() << "플레이어 애니메이션 인덱스 :" << m_pModelCom->Get_CurrentAnimationIndex() << endl;
		m_pGameInstance->Get_StringStream() << endl;
		m_pGameInstance->Get_StringStream() << "플레이어 위치 X :" << Pos.m128_f32[0] << endl;
		m_pGameInstance->Get_StringStream() << "플레이어 위치 Y :" << Pos.m128_f32[1] << endl;
		m_pGameInstance->Get_StringStream() << "플레이어 위치 Z :" << Pos.m128_f32[2] << endl;
		m_pGameInstance->Get_StringStream() << endl;
		m_pGameInstance->Get_StringStream() << "플레이어 룩 X :" << Look.m128_f32[0] << endl;
		m_pGameInstance->Get_StringStream() << "플레이어 룩 Y :" << Look.m128_f32[1] << endl;
		m_pGameInstance->Get_StringStream() << "플레이어 룩 Z :" << Look.m128_f32[2] << endl;
		m_pGameInstance->Get_StringStream() << endl;
	}
#endif // _DEBUG
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	if (m_pFrameEffect)
	{
		m_pFrameEffect->Late_Tick(fTimeDelta);
	}

	if (m_pBaseEffect)
	{
		m_pBaseEffect->Late_Tick(fTimeDelta);
	}

	m_pLeft_Trail->Late_Tick(fTimeDelta);
	m_pLeft_Distortion_Trail->Late_Tick(fTimeDelta);
	m_pRight_Trail->Late_Tick(fTimeDelta);
	m_pRight_Distortion_Trail->Late_Tick(fTimeDelta);

	if (m_pCam_Manager->Get_CameraState() == CS_WORLDMAP)
	{
		//return;
	}

	if (m_bIsMount)
	{
		m_pRiding->Late_Tick(fTimeDelta);
	}

	if (m_bStartGame)
	{
		CEvent_Manager::Get_Instance()->Late_Tick(fTimeDelta);

	}

	if (!m_bStartGame && m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY)
	{

		m_pTransformCom->Set_Scale(_vec3(1.0f));
		Add_Info();

		m_strPlayerName = m_pGameInstance->Get_InputString();
		dynamic_cast<CNameTag*>(m_pNameTag)->Set_Text(m_strPlayerName);

		m_bStartGame = true;
		CEvent_Manager::Get_Instance()->Init();

		CUI_Manager::Get_Instance()->Set_WeaponType(WP_SWORD);
		WEAPON_TYPE eWpType{};
		_uint iWpIdx = CUI_Manager::Get_Instance()->Get_WeaponType(PT_SWORD, &eWpType);
		Change_Weapon(eWpType, (WEAPON_INDEX)iWpIdx);
		m_Weapon_CurrentIndex = SWORD_UNEQUIP;
		m_Current_Weapon = WP_SWORD;
		m_bWeapon_Unequip = false;

		CTextButtonColor::TEXTBUTTON_DESC ButtonDesc = {};
		ButtonDesc.eLevelID = LEVEL_STATIC;
		ButtonDesc.fDepth = (_float)D_SCREEN / (_float)D_END;
		ButtonDesc.strText = TEXT("");
		ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Aim");
		ButtonDesc.vPosition = _vec2((_float)g_ptCenter.x, (_float)g_ptCenter.y);
		ButtonDesc.vSize = _vec2(40.f, 40.f);
		ButtonDesc.vColor = _vec4(1.f, 1.f, 1.f, 1.f);
		ButtonDesc.fAlpha = 0.7f;

		m_pAim = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ButtonDesc);
		if (not m_pAim)
		{
			return;
		}
		dynamic_cast<CTextButtonColor*>(m_pAim)->Set_Pass(VTPass_Mask_ColorAlpha);
		CUI_Manager::Get_Instance()->Set_Mp(m_Status.Current_Mp, m_Status.Max_Mp);
	}




	if (m_pEffect_Shield)
	{
		m_pEffect_Shield->Late_Tick(fTimeDelta);
	}

#ifdef _DEBUG
	//m_pRendererCom->Add_DebugComponent(m_pHitCollider);
	m_pRendererCom->Add_DebugComponent(m_pParryingCollider);

	//for (int i = 0; i < AT_End; i++)
	{
		/*m_pRendererCom->Add_DebugComponent(m_pAttCollider[AT_Sword_Skill1]);
		m_pRendererCom->Add_DebugComponent(m_pAttCollider[AT_Sword_Skill2]);
		m_pRendererCom->Add_DebugComponent(m_pAttCollider[AT_Sword_Skill3]);
		m_pRendererCom->Add_DebugComponent(m_pAttCollider[AT_Sword_Skill4]);*/
	}

#endif // DEBUG

	m_pModelCom->Play_Animation(fTimeDelta, m_bAttacked);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

	if (true == m_pGameInstance->Get_TurnOnShadow())
		m_pRendererCom->Add_RenderGroup(RG_Shadow, this);

	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}


	if (m_pNameTag != nullptr)
	{
		m_pNameTag->Late_Tick(fTimeDelta);
	}
	if (m_bLockOn && m_pAim != nullptr)
	{
		m_pAim->Late_Tick(fTimeDelta);
	}

}

HRESULT CPlayer::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (m_Body_CurrentIndex >= 0)
	{
		Render_Parts(PT_BODY, m_Body_CurrentIndex);
	}
	if (m_Hair_CurrentIndex >= 0 and m_bHelmet_Hide)
	{
		Render_Parts(PT_HAIR, m_Hair_CurrentIndex);
	}
	else if (m_Hair_CurrentIndex >= 0 and m_Helmet_CurrentIndex <= 0)
	{
		Render_Parts(PT_HAIR, m_Hair_CurrentIndex);
	}
	if (m_Face_CurrentIndex >= 0 and m_bHelmet_Hide)
	{
		Render_Parts(PT_FACE, m_Face_CurrentIndex);
	}
	else if (m_Hair_CurrentIndex >= 0 and m_Helmet_CurrentIndex <= 0)
	{
		Render_Parts(PT_FACE, m_Face_CurrentIndex);
	}
	else if (m_Hair_CurrentIndex >= 0 and m_Helmet_CurrentIndex == 1)
	{
		Render_Parts(PT_FACE, m_Face_CurrentIndex);
	}
	else if (m_Hair_CurrentIndex >= 0 and m_Helmet_CurrentIndex == 2)
	{
		Render_Parts(PT_FACE, m_Face_CurrentIndex);
	}
	if (m_Helmet_CurrentIndex >= 0 && !m_bHelmet_Hide)
	{
		Render_Parts(PT_HELMET, m_Helmet_CurrentIndex);
	}
	if (m_Current_Weapon == WP_SWORD && m_Weapon_CurrentIndex != SWORD_UNEQUIP)
	{
		Render_Parts(PT_SWORD, (_uint)m_Weapon_CurrentIndex - 6);
	}
	else if (m_Current_Weapon == WP_BOW && m_Weapon_CurrentIndex != BOW_UNEQUIP)
	{
		Render_Parts(PT_BOW, (_uint)m_Weapon_CurrentIndex);
	}

	return S_OK;
}

HRESULT CPlayer::Render_Shadow()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pModelCom->Bind_Bone(m_pShaderCom)))
		return E_FAIL;

	CASCADE_DESC Desc = m_pGameInstance->Get_CascadeDesc();

	if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeView", Desc.LightView, 3)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeProj", Desc.LightProj, 3)))
		return E_FAIL;

	if (m_Body_CurrentIndex >= 0)
	{
		Render_Shadow_Parts(PT_BODY, m_Body_CurrentIndex);
	}
	if (m_Hair_CurrentIndex >= 0 and m_bHelmet_Hide)
	{
		Render_Shadow_Parts(PT_HAIR, m_Hair_CurrentIndex);
	}
	else if (m_Hair_CurrentIndex >= 0 and m_Helmet_CurrentIndex <= 0)
	{
		Render_Shadow_Parts(PT_HAIR, m_Hair_CurrentIndex);
	}
	if (m_Face_CurrentIndex >= 0 and m_bHelmet_Hide)
	{
		Render_Shadow_Parts(PT_FACE, m_Face_CurrentIndex);
	}
	else if (m_Hair_CurrentIndex >= 0 and m_Helmet_CurrentIndex <= 0)
	{
		Render_Shadow_Parts(PT_FACE, m_Face_CurrentIndex);
	}
	else if (m_Hair_CurrentIndex >= 0 and m_Helmet_CurrentIndex == 1)
	{
		Render_Shadow_Parts(PT_FACE, m_Face_CurrentIndex);
	}
	else if (m_Hair_CurrentIndex >= 0 and m_Helmet_CurrentIndex == 2)
	{
		Render_Shadow_Parts(PT_FACE, m_Face_CurrentIndex);
	}
	if (m_Helmet_CurrentIndex >= 0 && !m_bHelmet_Hide)
	{
		Render_Shadow_Parts(PT_HELMET, m_Helmet_CurrentIndex);
	}

	if (m_Current_Weapon == WP_SWORD && m_Weapon_CurrentIndex != SWORD_UNEQUIP)
	{
		Render_Shadow_Parts(PT_SWORD, (_uint)m_Weapon_CurrentIndex - 6);
	}
	else if (m_Current_Weapon == WP_BOW && m_Weapon_CurrentIndex != BOW_UNEQUIP)
	{
		Render_Shadow_Parts(PT_BOW, (_uint)m_Weapon_CurrentIndex);
	}


	return S_OK;
}

CComponent* CPlayer::Find_Component(const wstring& strComTag)
{
	if (strComTag == TEXT("Com_Transform") && m_bIsMount)
	{
		return m_pRiding->Find_Component(strComTag);
	}
	else
	{
		auto& it = m_Components.find(strComTag);
		if (it == m_Components.end())
		{
			return nullptr;
		}

		return it->second;
	}
}

HRESULT CPlayer::Add_Info()
{
	CNameTag::NAMETAG_DESC NameTagDesc = {};
	NameTagDesc.eLevelID = LEVEL_STATIC;
	NameTagDesc.fFontSize = 0.32f;
	NameTagDesc.pParentTransform = m_pTransformCom;
	NameTagDesc.strNameTag = m_strPlayerName;
	NameTagDesc.vColor = _vec4(0.5f, 0.7f, 0.5f, 1.f);
	NameTagDesc.vTextPosition = _vec2(0.f, 1.9f);

	m_pNameTag = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NameTag"), &NameTagDesc);
	if (not m_pNameTag)
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CPlayer::Place_PartModels()
{
	CRealtimeVTFModel::PART_DESC Desc{};
	Desc.ePartType = PT_BODY;

	Desc.FileName = "body0";

	if (FAILED(m_pModelCom->Place_Parts(Desc, true)))
	{
		return E_FAIL;
	}

	Desc.FileName = "body1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "body2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "body3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "body4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "body5";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.ePartType = PT_HELMET;
	Desc.FileName = "helmet0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "helmet1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}
	Desc.FileName = "helmet2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}
	Desc.FileName = "helmet3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}
	Desc.FileName = "helmet4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}
	Desc.FileName = "helmet5";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}
	Desc.ePartType = PT_FACE;
	Desc.FileName = "face0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "face1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "face2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "face3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "face4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "face5";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.ePartType = PT_HAIR;
	Desc.FileName = "hair0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "hair1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "hair2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "hair3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "hair4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "hair5";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "hair6";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "hair7";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "hair8";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.ePartType = PT_BOW;

	Desc.FileName = "bow0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "bow1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "bow2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "bow3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "bow4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.ePartType = PT_SWORD;

	Desc.FileName = "sword0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "sword1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "sword2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "sword3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}
	Desc.FileName = "sword4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::Render_Parts(PART_TYPE Parts, _uint Index)
{

	for (size_t k = 0; k < m_pModelCom->Get_Num_PartMeshes(Parts, Index); k++)
	{
		if (FAILED(m_pModelCom->Bind_Part_Material(m_pShaderCom, "g_DiffuseTexture", TextureType::Diffuse, (_uint)Parts, (_uint)Index, k)))
		{
			continue;
		}

		_bool HasNorTex{};
		if (FAILED(m_pModelCom->Bind_Part_Material(m_pShaderCom, "g_NormalTexture", TextureType::Normals, (_uint)Parts, (_uint)Index, k)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		_bool g_HasMaskTex{};
		if (FAILED(m_pModelCom->Bind_Part_Material(m_pShaderCom, "g_MaskTexture", TextureType::Shininess, (_uint)Parts, (_uint)Index, k)))
		{
			g_HasMaskTex = false;
		}
		else
		{
			g_HasMaskTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}


		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &g_HasMaskTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (Parts == PT_HAIR)
		{
			if (FAILED(m_pShaderCom->Begin(m_HairShaderIndex)))
			{
				return E_FAIL;
			}
			//if (FAILED(m_pShaderCom->Begin(5)))
			//	return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(m_ShaderIndex)))
			{
				return E_FAIL;
			}
		}
		if (FAILED(m_pModelCom->Render_Part((_uint)Parts, (_uint)Index, k)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CPlayer::Render_Shadow_Parts(PART_TYPE Parts, _uint Index)
{


	for (size_t k = 0; k < m_pModelCom->Get_Num_PartMeshes(Parts, Index); k++)
	{
		if (FAILED(m_pModelCom->Bind_Part_Material(m_pShaderCom, "g_DiffuseTexture", TextureType::Diffuse, (_uint)Parts, (_uint)Index, k)))
		{
			continue;
		}

		if (FAILED(m_pShaderCom->Begin(6)))
		{
			return E_FAIL;
		}
		if (FAILED(m_pModelCom->Render_Part((_uint)Parts, (_uint)Index, k)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CPlayer::Add_Riding()
{
	Riding_Desc riding_desc{};
	riding_desc.Type = Bird;

	m_pRiding = (CRiding*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Riding"), &riding_desc);

	if (m_pRiding == nullptr)
	{
		return E_FAIL;
	}

	return S_OK;
}

void CPlayer::Set_Damage(_int iDamage, _uint MonAttType)
{
	if (m_eState == Revival_Start or m_eState == Revival_End or m_eState == Die)
	{
		return;
	}

	if (m_bLockOn)
	{
		m_pCam_Manager->Set_AimMode(false);
		m_bLockOn = false;
	}
	if (m_pEffect_Shield)
	{
		// 보호막 깨지는 이펙트
		Safe_Release(m_pEffect_Shield);
		m_pGameInstance->Play_Sound(TEXT("ShieldBreak"));
		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Shield_Diss");
		Info.pMatrix = &m_ShieldMatrix;
		CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

		return;

	}


	m_Status.Current_Hp -= (iDamage - iDamage * (_int)(m_Status.Armor / 0.01));



	CHitEffect::HITEFFECT_DESC Desc{};
	_int iRandomX = rand() % 100;
	_int iRandomY = rand() % 50 + 130;
	Desc.iDamage = iDamage;
	Desc.pParentTransform = m_pTransformCom;
	Desc.isPlayer = true;
	Desc.vTextPosition = _vec2((_float)(iRandomX - 50) * 0.01f, (_float)iRandomY * 0.01f);
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_HitEffect"), TEXT("Prototype_GameObject_HitEffect"), &Desc)))
	{
		return;
	}
	CUI_Manager::Get_Instance()->Set_Hp(m_Status.Current_Hp, m_Status.Max_Hp);

	if (m_Status.Current_Hp <= 0)
	{
		if (m_bIsMount)
		{
			m_pCam_Manager->Set_RidingZoom(false);
			Safe_Release(m_pRiding);
			m_bIsMount = false;
			_vec4 vLook = m_pGameInstance->Get_CameraLook();
			vLook.y = 0.f;
			m_pTransformCom->LookAt_Dir(vLook);
		}
		m_Status.Current_Hp = 0;
		CUI_Manager::Get_Instance()->Set_Hp(m_Status.Current_Hp, m_Status.Max_Hp);
		m_eState = Die;
	}
	else
	{
		if (m_iSuperArmor > 0 && MonAttType != Parrying_Succescc)
		{
			return;
		}

		if (m_eState == Stun_Start or m_eState == Stun)
		{
			if (MonAttType == MonAtt_Stun)
			{
				return;
			}
		}

		if (m_eState == KnockDown or m_bIsMount)
		{
			return;
		}

		switch (MonAttType)
		{
		case MonAtt_Hit:
		{
			m_eState = Hit;
			m_Animation.iAnimIndex = Anim_Stun_start;
			m_Animation.fDurationRatio = 0.4f;
			m_Animation.fAnimSpeedRatio = 2.f;
			m_Animation.fStartAnimPos = 18.f;
			m_Animation.isLoop = false;
			m_hasJumped = false;
			if (!m_bPoison)
			{
				m_bHitted = true;
				m_vRimColor = _vec4(1.f, 1.f, 1.f, 1.f);
			}
		}
		break;
		case MonAtt_KnockDown:
		{
			m_eState = KnockDown;
			m_pCam_Manager->Set_RidingZoom(false);
			m_bLockOn = false;
		}
		break;
		case MonAtt_Stun:
		{
			m_eState = Stun_Start;
			m_pCam_Manager->Set_RidingZoom(false);
			m_bLockOn = false;
		}
		break;
		case MonAtt_Poison:
		{
			if (!m_bPoison)
			{
				m_iPoisionDamage = iDamage;
				m_bPoison = true;
				m_vRimColor = _vec4(0.f, 1.f, 0.f, 1.f);
				if (m_bSlowSpeed == 0.f)
				{
					m_bSlowSpeed = 3.f;
				}
			}
		}
		break;
		case Parrying_Succescc:
		{
			m_fBoostSpeedTimmer = 5.f;
			m_fBoostSpeed = 3.f;
			//보호막 생성
			if (not m_pEffect_Shield)
			{
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Shield");
				Info.isFollow = true;
				Info.pMatrix = &m_ShieldMatrix;
				m_pEffect_Shield = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
			}
		}
		break;

		}
	}

	m_StartRegen = 0.f;
}

void CPlayer::Change_Parts(PART_TYPE PartsType, _int ChangeIndex)
{
	switch (PartsType)
	{
	case Client::PT_HAIR:
	{
		m_Hair_CurrentIndex = ChangeIndex;
	}
	break;
	case Client::PT_FACE:
	{
		m_Face_CurrentIndex = ChangeIndex;
	}
	break;
	case Client::PT_BODY:
	{
		m_Body_CurrentIndex = ChangeIndex;
	}
	break;
	case Client::PT_HELMET:
	{
		m_Helmet_CurrentIndex = ChangeIndex;
	}
	break;
	default:
		break;
	}
}

void CPlayer::Change_Weapon(WEAPON_TYPE PartsType, WEAPON_INDEX ChangeIndex)
{/*

	if (ChangeIndex == SWORD_UNEQUIP)
	{
		m_bWeapon_Unequip = true;
	}
	else
	{

	}
<<<<<<< HEAD

=======
	*/
	m_Weapon_CurrentIndex = ChangeIndex;
	m_Current_Weapon = PartsType;
	m_bWeapon_Unequip = false;
}


void CPlayer::Front_Ray_Check()
{
	PxRaycastBuffer Buffer{};
	_vec4 vRayDir{};
	_vec4 vMyPos = m_pTransformCom->Get_State(State::Pos);
	_vec4 PlayerCenter = m_pTransformCom->Get_CenterPos();
	PlayerCenter.y += 0.6f;
	vRayDir = m_pTransformCom->Get_State(State::Look).Get_Normalized();
	_float fDist = XMVectorGetX(XMVector3Length(vRayDir)) - 0.4f;
	if (m_pGameInstance->Raycast(PlayerCenter + vRayDir * 0.4f, vRayDir, fDist, Buffer))
	{
		m_bReady_Climb = true;
	}
	else
	{
		m_bReady_Climb = false;
	}
}

_bool CPlayer::Turn_Ray_Check(_bool bRight)
{
	PxRaycastBuffer Buffer{};
	_vec4 vRayDir{};
	_vec4 vMyPos = m_pTransformCom->Get_State(State::Pos);
	_vec4 vRight = m_pTransformCom->Get_State(State::Right).Get_Normalized();
	_vec4 PlayerCenter = m_pTransformCom->Get_CenterPos();
	PlayerCenter.y += 0.6f;
	if (bRight)
	{
		PlayerCenter += vRight * 0.5f;
	}
	else
	{
		PlayerCenter -= vRight * 0.5f;
	}

	vRayDir = m_pTransformCom->Get_State(State::Look).Get_Normalized();
	_float fDist = XMVectorGetX(XMVector3Length(vRayDir)) - 0.4f;

	return (m_pGameInstance->Raycast(PlayerCenter + vRayDir * 0.4f, vRayDir, fDist, Buffer));
}

void CPlayer::Health_Regen(_float fTImeDelta)
{
	if (m_StartRegen > 5.f && m_Status.Max_Hp > m_Status.Current_Hp)
	{
		m_fHpRegenTime += fTImeDelta;
	}

	if (m_Status.Max_Mp > m_Status.Current_Mp)
	{
		m_fMpRegenTime += fTImeDelta;
	}

	if (m_fHpRegenTime >= 1.f)
	{
		m_Status.Current_Hp += 1;
		m_fHpRegenTime = 0;
	}

	if (m_fMpRegenTime >= 1.f)
	{
		m_Status.Current_Mp += 1;
		m_fMpRegenTime = 0;
	}


	if (m_Status.Max_Hp < m_Status.Current_Hp)
	{
		m_Status.Current_Hp = m_Status.Max_Hp;
	}
	if (m_Status.Max_Mp < m_Status.Current_Mp)
	{
		m_Status.Current_Mp = m_Status.Max_Mp;
	}
	CUI_Manager::Get_Instance()->Set_Mp(m_Status.Current_Mp, m_Status.Max_Mp);
}

void CPlayer::Move(_float fTimeDelta)
{
	_bool hasMoved{};
	_vec4 vForwardDir = m_pGameInstance->Get_CameraLook();
	vForwardDir.y = 0.f;
	_vec4 vRightDir = XMVector3Cross(m_pTransformCom->Get_State(State::Up), vForwardDir);
	vRightDir.Normalize();
	_vec4 vDirection{};

	if (m_eState == Revival_Start or m_eState == Revival_End
		or m_eState == KnockDown or m_eState == Stun_Start
		or m_eState == Stun or m_eState == Die or m_eState == Jump_Long_End)
	{
		m_pTransformCom->Gravity(fTimeDelta);
		return;
	}

	_bool isUnequip{};
	if (m_Current_Weapon == WP_SWORD && m_Weapon_CurrentIndex == SWORD_UNEQUIP)
	{
		isUnequip = true;
	}
	else if (m_Current_Weapon == WP_BOW && m_Weapon_CurrentIndex == BOW_UNEQUIP)
	{
		isUnequip = true;
	}
	if (m_eState == Jump)
	{
		if (!m_pTransformCom->Is_Jumping())
		{
			m_eState = Jump_End;
		}
	}
	if (!m_bReadySwim && !isUnequip)
	{


		if (m_eState == SkillR || m_eState == AimMode_Start)
		{
			m_pTransformCom->LookAt_Dir(m_pCameraTransform->Get_State(State::Look));
		}

		if (m_pGameInstance->Key_Down(DIK_1))
		{
			if (m_eState != Skill1)
			{
				Ready_Skill(ST_Skill1); // 1번창에 있던 스킬 넣어주기
				return;
			}
		}

		if (m_pGameInstance->Key_Down(DIK_2))
		{
			if (m_eState != Skill2)
			{
				Ready_Skill(ST_Skill2);
				return;
			}
		}


		if (m_pGameInstance->Key_Down(DIK_3))
		{
			if (m_eState != Skill3)
			{
				Ready_Skill(ST_Skill3);
				return;
			}
		}

		if (m_pGameInstance->Key_Down(DIK_4))
		{
			if (m_eState != Skill4)
			{
				Ready_Skill(ST_Skill4);
				return;
			}

		}


		/*CSkillBlock::SKILLSLOT eSlotIdx{};
		_bool isPress = false;
		if (m_pGameInstance->Key_Down(DIK_1))
		{
			eSlotIdx = CSkillBlock::SKILL1;
			isPress = true;
		}
		if (m_pGameInstance->Key_Down(DIK_2))
		{
			eSlotIdx = CSkillBlock::SKILL2;
			isPress = true;
		}
		if (m_pGameInstance->Key_Down(DIK_3))
		{
			eSlotIdx = CSkillBlock::SKILL3;
			isPress = true;
		}
		if (m_pGameInstance->Key_Down(DIK_4))
		{
			eSlotIdx = CSkillBlock::SKILL4;
			isPress = true;
		}
		if (isPress && m_fSkiilTimer > 1.2f)
		{
			_int iSkillNum = 0;
			_int iMp = 0;
			if (CUI_Manager::Get_Instance()->Use_Skill(m_Current_Weapon, eSlotIdx, &iSkillNum, &iMp))
			{
				Ready_Skill((Skill_Type)iSkillNum);
				m_Status.Current_Mp -= iMp;
				CUI_Manager::Get_Instance()->Set_Mp(m_Status.Current_Mp, m_Status.Max_Mp);
				return;
			}

		}*/

		if (m_pGameInstance->Key_Down(DIK_5))
		{
			if (m_eState == Idle or m_eState == Run_Start or m_eState == Run or m_eState == Run_End
				or m_eState == Walk or m_eState == Jump_Start or m_eState == Jump_End or m_eState == Attack_Idle)
			{
				if (m_Current_Weapon == WP_SWORD)
				{
					CUI_Manager::Get_Instance()->Set_WeaponType(WP_BOW);
					WEAPON_TYPE eWpType{};
					_uint iWpIdx = CUI_Manager::Get_Instance()->Get_WeaponType(PT_BOW, &eWpType);
					Change_Weapon(eWpType, (WEAPON_INDEX)iWpIdx);
					/*
					CUI_Manager::Get_Instance()->Set_WeaponType(WP_BOW);
					Change_Weapon(WP_BOW, BOW0);
					*/
				}
				else
				{
					CUI_Manager::Get_Instance()->Set_WeaponType(WP_SWORD);
					WEAPON_TYPE eWpType{};
					_uint iWpIdx = CUI_Manager::Get_Instance()->Get_WeaponType(PT_SWORD, &eWpType);
					Change_Weapon(eWpType, (WEAPON_INDEX)iWpIdx);
					/*
					CUI_Manager::Get_Instance()->Set_WeaponType(WP_SWORD);
					Change_Weapon(WP_SWORD, SWORD0);
					*/
				}
			}
		}
		if (m_bLockOn)
		{
			_vec4 vCamLook = m_pGameInstance->Get_CameraLook();

			//vCamLook.y = 0.f;
			m_pTransformCom->LookAt_Dir(vCamLook);
		}

	}
	if (m_eState == Jump_Start)
	{
		if (!m_pTransformCom->Is_Jumping())
		{
			m_eState = Run;
		}
	}

	if (m_pGameInstance->Mouse_Pressing(DIM_RBUTTON))
	{
		if (m_Current_Weapon == WP_BOW && m_eState != AimMode_End && m_eState != Hit &&
			m_eState != KnockDown && m_eState != Stun && m_eState != Stun_Start && m_eState != AimMode)
		{
			if (!m_bLockOn)
			{
				if (m_eState != Skill1 and m_eState != Skill2 and m_eState != Skill3 and m_eState != Skill4)
				{
					m_pCam_Manager->Set_AimMode(true);
					m_eState = AimMode_Start;
					m_bLockOn = true;
					CUI_Manager::Get_Instance()->Set_MouseState(CUI_Manager::M_HIDE);
				}

			}
		}
	}

	if (!m_pGameInstance->Mouse_Pressing(DIM_RBUTTON))
	{


		if (m_bLockOn and m_eState != Skill1 and m_eState != Skill2 and m_eState != Skill3 and m_eState != Skill4)
		{
			_vec4 vLook = m_pTransformCom->Get_State(State::Look);
			vLook.y = 0.f;
			m_pTransformCom->LookAt_Dir(vLook);
			m_bLockOn = false;
			m_pCam_Manager->Set_AimMode(false);
			m_bMove_AfterSkill = true;
			m_eState = AimMode_End;
			CUI_Manager::Get_Instance()->Set_MouseState(CUI_Manager::M_DEFAULT);
		}
		else if (m_bLockOn and m_bMove_AfterSkill)
		{
			_vec4 vLook = m_pTransformCom->Get_State(State::Look);
			vLook.y = 0.f;
			m_bLockOn = false;
			m_pTransformCom->LookAt_Dir(vLook);
			m_pCam_Manager->Set_AimMode(false);
			m_eState = AimMode_End;
			CUI_Manager::Get_Instance()->Set_MouseState(CUI_Manager::M_DEFAULT);
		}
	}
	if (m_bLockOn)
	{
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON))
		{
			m_eState = Attack;
			Common_Attack();
		}

	}
	else if ((m_bMove_AfterSkill && m_eState != SkillR && m_eState != AimMode_Start && m_eState != AimMode))
	{


		if (m_pGameInstance->Key_Pressing(DIK_W))
		{
			CEvent_Manager::Get_Instance()->Set_TutorialSeq(T_OPENINVEN);
			vDirection += vForwardDir;


			hasMoved = true;
		}
		else if (m_pGameInstance->Key_Pressing(DIK_S))
		{
			vDirection -= vForwardDir;
			hasMoved = true;
		}

		if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			vDirection += vRightDir;
			hasMoved = true;
		}
		else if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			vDirection -= vRightDir;
			hasMoved = true;
		}
		vDirection.Normalize();

		if (m_pGameInstance->Key_Down(DIK_F, InputChannel::UI))
		{
			if (m_bIsClimb or m_bReadySwim or m_hasJumped)
			{
				return;
			}
			if (!isUnequip)
			{
				if (vDirection != _vec4())
				{
					m_pTransformCom->LookAt_Dir(vDirection);
				}


				m_eState = Attack;
				Common_Attack();
				hasMoved = false;
			}

		}
		if (m_pGameInstance->Key_Down(DIK_SPACE))
		{
			if (m_bReadySwim)
			{
				return;
			}
			if (!m_hasJumped)
			{
				m_pTransformCom->Jump(8.f);
				m_eState = Jump_Start;
				CEvent_Manager::Get_Instance()->Update_Quest(TEXT("점프하기"));
				CEvent_Manager::Get_Instance()->Update_Quest(TEXT("그로아의 부탁"));
			}
			else if (m_bReady_Climb)
			{
				m_eState = Climb;
				m_bIsClimb = true;
			}
			else
			{
				if (!m_pTransformCom->Is_OnGround())
				{
					if (!m_bIsMount)
					{
						m_Current_AirRiding = CUI_Manager::Get_Instance()->Get_Riding(VC_FLY);
						if (m_Current_AirRiding != Type_End)
						{
							m_bIsMount = true;

							Riding_Desc Desc{};
							Desc.Type = m_Current_AirRiding;
							Desc.bGlide = true;
							m_eState = Mount_Fly;
							Summon_Riding(Desc);
						}
					}
				}
			}
		}
		// 수영
		if (m_bReadySwim && hasMoved)
		{
			if (m_eState == Swim_Idle or m_eState == Swim)
			{
				m_eState = Swim;
			}
			else
			{
				return;
			}

			m_pTransformCom->Set_Speed(m_fRunSpeed + m_Status.Speed + m_fBoostSpeed -m_bSlowSpeed);
			m_pTransformCom->Go_To_Dir(vDirection, fTimeDelta);

			_vec4 vLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();

			if (vLook.Dot(vDirection) < 0)
			{
				vLook = vDirection;
			}
			else
			{
				vLook = XMVectorLerp(vLook, vDirection, 0.15f);
			}

			m_pTransformCom->LookAt_Dir(vLook);

		}
		else if (m_bReadySwim && !hasMoved)
		{
			if (m_eState != Swim_collect)
			{
				m_eState = Swim_Idle;
			}
		}
		else if (hasMoved && !m_bIsClimb && m_bReadyMove && !m_bReadySwim)
		{

			if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
			{
				if (m_eState == Walk or
					m_eState == Idle)
				{
					m_eState = Run_Start;

				}
				else if (m_eState == Attack or
						 m_eState == Skill1 or
						 m_eState == Skill2 or
						 m_eState == Skill3 or
						 m_eState == Skill4 or
						 m_eState == AimMode_End)
				{
					m_eState = Attack_Run;
				}
				else if (m_eState == Run or
						 m_eState == Run_End or
						 m_pModelCom->IsAnimationFinished(Anim_Normal_run_start))
				{
					m_eState = Run;

				}
				else if (m_eState == Attack_Idle)
				{
					m_eState = Attack_Run;

				}
				else if (m_eState == Jump_End)
				{
					m_eState = Jump_Run;

				}
				m_pTransformCom->Set_Speed(m_fRunSpeed + m_Status.Speed + m_fBoostSpeed - m_bSlowSpeed);
			}
			else
			{
				if (m_eState == Attack or
					m_eState == Attack_Idle or
					m_eState == Skill1 or
					m_eState == Skill2 or
					m_eState == Skill3 or
					m_eState == Skill4 or
					m_eState == AimMode_End)
				{
					m_eState = Attack_Run;
					m_pTransformCom->Set_Speed(m_fRunSpeed + m_Status.Speed + m_fBoostSpeed - m_bSlowSpeed);
				}
				else if (m_eState == Jump_End)
				{
					m_eState = Jump_Run;
					m_pTransformCom->Set_Speed(m_fRunSpeed + m_Status.Speed + m_fBoostSpeed - m_bSlowSpeed);
				}
				else if (m_eState == Run or
						 m_eState == Run_End or
						 m_eState == Run_Start or
						 m_eState == Jump_Long_End
						 )
				{
					m_eState = Run;
					m_pTransformCom->Set_Speed(m_fRunSpeed + m_Status.Speed + m_fBoostSpeed - m_bSlowSpeed);
				}
				else
				{
					if (m_eState == Idle or
						m_eState == Walk)
					{
						m_eState = Walk;
						m_pTransformCom->Set_Speed(m_fWalkSpeed + m_Status.Speed / 3.f);
					}
					else if (m_eState == Jump or
							 m_eState == Jump_Start)
					{
						m_pTransformCom->Set_Speed(m_fRunSpeed + m_Status.Speed + m_fBoostSpeed - m_bSlowSpeed);
					}
					else
					{
						m_pTransformCom->Set_Speed(m_fRunSpeed + m_Status.Speed + m_fBoostSpeed - m_bSlowSpeed);
					}
				}


			}



			if (m_eState != Attack or (m_eState == Attack && m_fAttTimer > 0.75f))
			{
				m_pTransformCom->Go_To_Dir(vDirection, fTimeDelta);
			}


			_vec4 vLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();

			if (vLook.Dot(vDirection) < 0)
			{
				vLook = vDirection;
			}
			else
			{
				vLook = XMVectorLerp(vLook, vDirection, 0.15f);
			}

			m_pTransformCom->LookAt_Dir(vLook);

		}
		else if (m_eState == Walk or m_eState == Run_Start)
		{
			m_eState = Idle;
		}
		if (m_eState == Jump_Start)
		{
			if (!m_pTransformCom->Is_Jumping())
			{
				m_eState = Jump_Run;
			}
		}
	}


	if (!m_bLockOn)
	{
		m_fAttackZoom = Lerp(m_fAttackZoom, 0.f, 0.02f);
		m_pCam_Manager->Set_CameraAttackZoom(m_fAttackZoom);

	}
	/*else if (m_iCurrentSkill_Index != 0 && m_ReturnZoomTime > 1.0f)
	{
		m_fAttackZoom = Lerp(m_fAttackZoom, 0.f, 0.04f);
		m_pCam_Manager->Set_CameraAttackZoom(m_fAttackZoom);

	}*/

	if (!m_bIsClimb)
	{
		m_pTransformCom->Gravity(fTimeDelta);
	}

}

void CPlayer::Is_Climb(_float fTimeDelta)
{
	m_pTransformCom->Set_Speed(0.9f);
	_bool hasMove{};
	if (m_eState == Jump_Start)
	{
		if (m_pModelCom->Get_CurrentAnimationIndex() == Anim_jump_start)
		{
			_float Index = m_pModelCom->Get_CurrentAnimPos();
			if (!m_bReady_Climb && Index > 48)
			{
				m_eState = Idle;
				m_bIsClimb = false;
				return;
			}
		}
	}
	if (m_eState == Jump)
	{
		m_bIsClimb = false;
		return;
	}

	if (m_eState != Jump_Start)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
		{
			m_pTransformCom->Go_Up(fTimeDelta);
			if (!m_bReady_Climb)
			{
				m_pTransformCom->Jump(8.f);
				m_eState = Jump_Start;
				m_bIsClimb = false;
				return;
			}
			hasMove = true;
			m_eState = Climb_U;

		}
		else if (m_pGameInstance->Key_Pressing(DIK_S))
		{
			m_pTransformCom->Go_Down(fTimeDelta);
			if (!m_pTransformCom->Is_Jumping())
			{
				m_eState = Idle;
				m_bIsClimb = false;
				return;
			}
			hasMove = true;
			m_eState = Climb_D;

		}

		if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			m_pTransformCom->Go_Right(fTimeDelta);
			hasMove = true;
			m_eState = Climb_R;
		}
		else if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			m_pTransformCom->Go_Left(fTimeDelta);

			hasMove = true;
			m_eState = Climb_L;
		}
	}
	if (m_eState == Climb_R)
	{
		if (!Turn_Ray_Check(true))
		{
			m_pTransformCom->Set_RotationPerSec(-40.f);
			m_pTransformCom->Turn(_vec4(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		}
	}
	if (m_eState == Climb_L)
	{
		if (!Turn_Ray_Check(false))
		{
			m_pTransformCom->Set_RotationPerSec(40.f);
			m_pTransformCom->Turn(_vec4(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		}
	}
	if (!hasMove && m_eState != Jump_Start)
	{
		m_eState = Climb;
	}


	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		if (m_eState == Jump_Start)
		{
			if (m_pModelCom->Get_CurrentAnimationIndex() == Anim_jump_start)
			{
				_float Index = m_pModelCom->Get_CurrentAnimPos();
				if (Index > 48)
				{
					if (m_bReady_Climb)
					{
						m_eState = Climb;
					}
				}
			}
		}
		else
		{
			m_pTransformCom->Jump(8.f);
			m_eState = Jump_Start;
			m_bIsClimb = false;
		}

	}


}

void CPlayer::Common_Attack()
{
	if (m_bLockOn)
	{
		if (m_fAttTimer < 0.3f)
		{
			return;
		}
	}
	else
	{
		if (m_fAttTimer < 0.44f)
		{
			return;
		}
	}


	if (m_fAttTimer > 1.2f || m_iAttackCombo > 3 || (m_eState != Attack_Idle && m_eState != Attack))
	{
		m_iAttackCombo = 0;
	}

	m_Animation.bSkipInterpolation = false;
	m_iCurrentSkill_Index = 0;
	m_bComboZoom = false;
	m_bAttacked = false;
	m_bReadyMove = false;
	m_Animation.isLoop = false;

	if (!m_bLockOn)
	{
		_vec4 vCamLook = m_pGameInstance->Get_CameraLook();
		vCamLook.y = 0.f;
		m_pTransformCom->LookAt_Dir(vCamLook);
	}

	m_pAttCollider[AT_Bow_Common]->Update(_mat::CreateTranslation(_vec3(0.f, 1.f, 0.f)) * m_pTransformCom->Get_World_Matrix());
	if (m_Current_Weapon == WP_SWORD)
	{
		m_Animation.fAnimSpeedRatio = 3.f;
		switch (m_iAttackCombo)
		{
		case 0:
		{
			m_Animation.iAnimIndex = Anim_Assassin_Attack01_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
		}
		break;
		case 1:
		{
			m_Animation.iAnimIndex = Anim_Assassin_Attack02_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
		}
		break;
		case 2:
		{
			m_Animation.iAnimIndex = Anim_Assassin_Attack03_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
		}
		break;
		case 3:
		{
			m_Animation.fAnimSpeedRatio = 3.5f;
			m_Animation.iAnimIndex = Anim_Assassin_Attack04_A;
			m_fAttTimer = 0.f;
			m_fSkillSpeed = 12.f;
			m_iAttackCombo++;
		}

		break;

		default:
			break;
		}
	}
	else if (m_Current_Weapon == WP_BOW)
	{

		if (m_bLockOn)
		{
			m_Animation.fAnimSpeedRatio = 4.5;
			CCollider* pMonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
			if (pMonCollider != nullptr)
			{
				_vec4 vMonPos = _vec4(pMonCollider->Get_ColliderPos(), 1.f);
				vMonPos.y = m_pTransformCom->Get_State(State::Pos).y;
				//m_pTransformCom->LookAt(vMonPos);
			}
		}
		else
		{
			m_Animation.fAnimSpeedRatio = 3.5f;
		}
		switch (m_iAttackCombo)
		{
		case 0:
		{
			m_Animation.iAnimIndex = Anim_ID_Sniper_attack01_A;
			m_fAttTimer = 0.f;
			m_ReadyArrow = true;
			m_iAttackCombo++;
		}
		break;
		case 1:
		{
			m_Animation.iAnimIndex = Anim_ID_Sniper_attack02_A;
			m_fAttTimer = 0.f;
			m_ReadyArrow = true;
			m_iAttackCombo++;
		}
		break;
		case 2:
		{
			m_Animation.iAnimIndex = Anim_ID_Sniper_attack03_A;
			m_fAttTimer = 0.f;
			m_ReadyArrow = true;
			m_iAttackCombo++;
		}
		break;
		case 3:
		{
			m_Animation.iAnimIndex = Anim_ID_Sniper_attack04_A;
			m_fAttTimer = 0.f;
			m_ReadyArrow = true;
			m_iAttackCombo++;
		}
		break;

		default:
			break;
		}
	}

}
void CPlayer::Skill1_Attack()
{
	_vec4 vCamLook = m_pGameInstance->Get_CameraLook();
	vCamLook.y = 0.f;
	m_pTransformCom->LookAt_Dir(vCamLook);
	m_pAttCollider[AT_Bow_Common]->Update(_mat::CreateTranslation(_vec3(0.f, 1.f, 0.f)) * m_pTransformCom->Get_World_Matrix());
	if (m_Current_Weapon == WP_SWORD)
	{
		m_pTransformCom->Set_Speed(6.f);
		m_Animation.iAnimIndex = m_SwordSkill[0];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = { 1.f };
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		if (m_bLockOn)
		{
			CCollider* pMonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
			if (pMonCollider != nullptr)
			{
				_vec4 vMonPos = _vec4(pMonCollider->Get_ColliderPos(), 1.f);
				vMonPos.y = m_pTransformCom->Get_State(State::Pos).y;
				m_pTransformCom->LookAt(vMonPos);
				CUI_Manager::Get_Instance()->Set_TargetPos(vMonPos);
			}
			//CUI_Manager::Get_Instance()->Set_TargetPos(vMonPos);
		}

		m_Animation.iAnimIndex = m_BowSkill[0];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = { 1.f };
		m_Animation.fAnimSpeedRatio = 2.7f;
		m_Animation.fDurationRatio = 0.8f;
	}
	m_bAttacked = false;
}
void CPlayer::Skill2_Attack()
{
	_vec4 vCamLook = m_pGameInstance->Get_CameraLook();
	vCamLook.y = 0.f;
	m_pTransformCom->LookAt_Dir(vCamLook);
	m_pAttCollider[AT_Bow_Common]->Update(_mat::CreateTranslation(_vec3(0.f, 1.f, 0.f)) * m_pTransformCom->Get_World_Matrix());
	if (m_Current_Weapon == WP_SWORD)
	{
		m_Animation.iAnimIndex = m_SwordSkill[1];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = { 1.f };
		m_fSkillSpeed = 10.f;
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		if (m_bLockOn)
		{
			CCollider* pMonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
			if (pMonCollider != nullptr)
			{
				_vec4 vMonPos = _vec4(pMonCollider->Get_ColliderPos(), 1.f);
				vMonPos.y = m_pTransformCom->Get_State(State::Pos).y;
				m_pTransformCom->LookAt(vMonPos);
				CUI_Manager::Get_Instance()->Set_TargetPos(vMonPos);
			}
		}

		m_pTransformCom->Set_Speed(10.f);
		m_Animation.iAnimIndex = m_BowSkill[1];
		m_Animation.isLoop = false;
		m_hasJumped = false;

		m_iSuperArmor = { 1.f };
	}
	m_bAttacked = false;
}
void CPlayer::Skill3_Attack()
{
	_vec4 vCamLook = m_pGameInstance->Get_CameraLook();
	vCamLook.y = 0.f;
	m_pTransformCom->LookAt_Dir(vCamLook);
	m_pAttCollider[AT_Bow_Common]->Update(_mat::CreateTranslation(_vec3(0.f, 1.f, 0.f)) * m_pTransformCom->Get_World_Matrix());
	if (m_Current_Weapon == WP_SWORD)
	{
		m_Animation.iAnimIndex = m_SwordSkill[2];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = { 1.f };
		m_fSkillSpeed = 15.f;
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		if (m_bLockOn)
		{
			CCollider* pMonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
			if (pMonCollider != nullptr)
			{
				m_vArrowRainPos = _vec4(pMonCollider->Get_ColliderPos(), 1.f);
				_vec4 vMonPos = _vec4(pMonCollider->Get_ColliderPos(), 1.f);
				vMonPos.y = m_pTransformCom->Get_State(State::Pos).y;
				m_pTransformCom->LookAt(vMonPos);
				CUI_Manager::Get_Instance()->Set_TargetPos(vMonPos);
			}
		}

		m_Animation.iAnimIndex = m_BowSkill[2];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = { 1.f };
	}
	m_bAttacked = false;
}

void CPlayer::Skill4_Attack()
{
	if (m_Current_Weapon == WP_SWORD)
	{
		_vec4 vLook = m_pGameInstance->Get_CameraLook();
		vLook.y = m_pTransformCom->Get_State(State::Look).y;
		m_pTransformCom->LookAt_Dir(vLook);
		m_Animation.iAnimIndex = m_SwordSkill[3];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = { 1.f };
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		_vec4 vCamLook = m_pGameInstance->Get_CameraLook();
		vCamLook.y = 0.f;
		m_pTransformCom->LookAt_Dir(vCamLook);
		_mat offset = _mat::CreateTranslation(_vec3(0.f, 1.f, 0.f));
		m_pAttCollider[AT_Bow_Common]->Update(offset * m_pTransformCom->Get_World_Matrix());
		if (m_bLockOn)
		{

			CCollider* pMonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
			if (pMonCollider != nullptr)
			{
				_vec4 vMonPos = _vec4(pMonCollider->Get_ColliderPos(), 1.f);
				vMonPos.y = m_pTransformCom->Get_State(State::Pos).y;
				m_pTransformCom->LookAt(vMonPos);
				CUI_Manager::Get_Instance()->Set_TargetPos(vMonPos);
			}
		}

		m_Animation.iAnimIndex = m_BowSkill[3];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = { 1.f };
	}
	m_bAttacked = false;
}
void CPlayer::SkillR_Attack()
{
	if (m_Current_Weapon == WP_SWORD)
	{
		/*	m_Animation.iAnimIndex = m_SwordSkill[4];
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
			m_fSkiilTimer = 0.f;*/
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		m_Animation.iAnimIndex = m_BowSkill[4];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = {};
	}
}
void CPlayer::Ready_Skill(Skill_Type Type)
{
	switch (Type)
	{
	case Client::ST_Skill1:
	{
		m_eState = Skill1;
		m_iCurrentSkill_Index = Skill1;
	}
	break;
	case Client::ST_Skill2:
	{
		m_eState = Skill2;

		m_iCurrentSkill_Index = Skill2;
	}
	break;
	case Client::ST_Skill3:
	{
		m_eState = Skill3;
		m_iCurrentSkill_Index = Skill3;
	}
	break;
	case Client::ST_Skill4:
	{
		m_eState = Skill4;
		m_iCurrentSkill_Index = Skill4;
	}
	break;

	default: // 쿨탐이거나 얻지 못한 스킬일때 처리
		break;
	}
	m_ReadyArrow = true;
	m_bMove_AfterSkill = false;
}
void CPlayer::Cam_AttackZoom(_float fZoom)
{

	m_fAttackZoom = fZoom;

	m_pCam_Manager->Set_CameraAttackZoom(m_fAttackZoom);

}
void CPlayer::Return_Attack_IdleForm()
{
	m_Animation.fAnimSpeedRatio = 3.f;
	if (m_Current_Weapon == WP_SWORD)
	{
		if (m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack01_A))
		{
			m_Animation.iAnimIndex = Anim_Assassin_Attack01_B;
			m_bReadyMove = true;
		}
		else if (m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack02_A))
		{
			m_Animation.iAnimIndex = Anim_Assassin_Attack02_B;
			m_bReadyMove = true;
		}
		else if (m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack03_A))
		{
			m_Animation.iAnimIndex = Anim_Assassin_Attack03_B;
			m_bReadyMove = true;
		}
		else if (m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack04_A))
		{
			m_Animation.iAnimIndex = Anim_Assassin_Attack04_B;
			m_bReadyMove = true;
		}
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		if (m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_attack01_A))
		{
			m_Animation.iAnimIndex = Anim_ID_Sniper_Attack_01_B;
			m_bReadyMove = true;
		}
		else if (m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_attack02_A))
		{
			m_Animation.iAnimIndex = Anim_ID_Sniper_Attack_02_B;
			m_bReadyMove = true;
		}
		else if (m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_attack03_A))
		{
			m_Animation.iAnimIndex = Anim_ID_Sniper_Attack_03_B;
			m_bReadyMove = true;
		}
		else if (m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_attack04_A))
		{
			m_Animation.iAnimIndex = Anim_ID_Sniper_Attack_04_B;
			m_bReadyMove = true;
		}
	}

}


void CPlayer::Check_Att_Collider(ATTACK_TYPE Att_Type)
{


	_int RandomPercentage = rand() % 100 + 1;
	_uint RandomDmg = rand() % 10;
	_uint Critical{};

	if (RandomPercentage <= m_Status.Critical)
	{
		Critical = m_Status.Attack * (_uint)(m_Status.Critical_Dmg * 0.01f) - m_Status.Attack;
	}


	switch (Att_Type)
	{
	case Client::AT_Sword_Common:
	{
		m_pGameInstance->Attack_Monster(m_pAttCollider[Att_Type], m_Status.Attack + Critical + RandomDmg, Att_Type);

	}
	break;
	case Client::AT_Sword_Skill1:
	{
		m_pGameInstance->Attack_Monster(m_pAttCollider[Att_Type], (_int)(m_Status.Attack * 1.5f) + Critical + RandomDmg, Att_Type);
	}
	break;
	case Client::AT_Sword_Skill2:
	{
		m_pGameInstance->Attack_Monster(m_pAttCollider[Att_Type], (_int)(m_Status.Attack * 1.5f) + Critical + RandomDmg, Att_Type);
	}
	break;
	case Client::AT_Sword_Skill3:
	{
		m_pGameInstance->Attack_Monster(m_pAttCollider[Att_Type], (_int)(m_Status.Attack * 1.3f) + Critical + RandomDmg, Att_Type);
	}
	break;
	case Client::AT_Sword_Skill4:
	{
		m_pGameInstance->Attack_Monster(m_pAttCollider[Att_Type], (_int)(m_Status.Attack * 2.f) + Critical + RandomDmg, Att_Type);
	}
	break;
	default:
		break;
	}
}
void CPlayer::Set_ExtraStatus()
{
	PLAYER_STATUS ExtraStat = CUI_Manager::Get_Instance()->Get_ExtraStatus();
	m_Status.Max_Hp = m_OriStatus.Max_Hp + ExtraStat.Max_Hp;
	m_Status.Attack = m_OriStatus.Attack + ExtraStat.Attack;
	m_Status.Max_Mp = m_OriStatus.Max_Mp + ExtraStat.Max_Mp;
	m_Status.Critical = m_OriStatus.Critical + ExtraStat.Critical;

}
void CPlayer::After_SwordAtt(_float fTimeDelta)
{
	if (m_eState == Attack)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (m_iAttackCombo == 1)
		{
			if (m_pModelCom->Get_CurrentAnimationIndex() == Anim_Assassin_Attack01_A)
			{
				if (Index >= 13.f && Index <= 20.f)
				{
					m_pTransformCom->Set_Speed(3.f);
					m_pTransformCom->Go_Straight(fTimeDelta);
					m_fAttackZoom = 1.5;
					Cam_AttackZoom(m_fAttackZoom);
				}

				if (Index >= 13.f && Index <= 23.f)
				{
					m_pLeft_Trail->Set_Color(_color(1.f, 0.5f, 0.1f, 1.f));
					m_pLeft_Trail->On();
					m_pLeft_Distortion_Trail->On();
				}

				if (Index >= 19.f && Index <= 22.f)
				{
					if (!m_bAttacked)
					{
						Check_Att_Collider(AT_Sword_Common);
						if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Common]))
						{
							m_pGameInstance->Set_TimeRatio(0.05f);
							m_bAttacked = true;
						}

					}
					else if (m_bAttacked)
					{
						m_pGameInstance->Set_TimeRatio(1.f);
					}
					if (m_bAttacked)
					{
						m_pTransformCom->Set_Speed(5.f);
						m_pTransformCom->Go_Straight(fTimeDelta);
						m_pCam_Manager->Set_ShakeCam(true, 2.0f);
					}

				}
				else
				{
					m_pGameInstance->Set_TimeRatio(1.f);
				}
			}
		}
		else if (m_iAttackCombo == 2)
		{
			if (m_pModelCom->Get_CurrentAnimationIndex() == Anim_Assassin_Attack02_A)
			{
				if (Index >= 12.5f && Index <= 20.f)
				{
					m_pTransformCom->Set_Speed(3.f);
					m_pTransformCom->Go_Straight(fTimeDelta);
					if (!m_bComboZoom)
					{
						m_fAttackZoom += 1.f;
						m_bComboZoom = true;
					}
				}
				if (Index >= 20.5f && Index <= 26.f)
				{
					m_pLeft_Trail->Set_Color(_color(1.f, 0.5f, 0.1f, 1.f));
					m_pLeft_Trail->On();
					m_pLeft_Distortion_Trail->On();
				}

				if (Index >= 22.f && Index <= 25.f)
				{
					if (!m_bAttacked)
					{
						Check_Att_Collider(AT_Sword_Common);
						if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Common]))
						{
							m_pGameInstance->Set_TimeRatio(0.05f);
							m_bAttacked = true;

						}




					}
					else if (m_bAttacked)
					{
						m_pGameInstance->Set_TimeRatio(1.f);
					}
				}
				else
				{
					m_pGameInstance->Set_TimeRatio(1.f);
				}
			}
		}
		else if (m_iAttackCombo == 3)
		{
			if (m_pModelCom->Get_CurrentAnimationIndex() == Anim_Assassin_Attack03_A)
			{
				if (Index >= 13.f && Index <= 20.f)
				{
					m_pTransformCom->Set_Speed(3.f);
					m_pTransformCom->Go_Straight(fTimeDelta);
					if (!m_bComboZoom)
					{
						m_fAttackZoom += 1.f;
						m_bComboZoom = true;
					}
				}
				if (Index >= 17.f && Index <= 23.f)
				{
					m_pLeft_Trail->Set_Color(_color(1.f, 0.5f, 0.1f, 1.f));
					m_pLeft_Trail->On();
					m_pLeft_Distortion_Trail->On();
				}
				if (Index >= 19.f && Index <= 22.f)
				{

					if (!m_bAttacked)
					{
						Check_Att_Collider(AT_Sword_Common);
						if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Common]))
						{
							m_pGameInstance->Set_TimeRatio(0.05f);
							m_bAttacked = true;

						}

					}
					else if (m_bAttacked)
					{
						m_pCam_Manager->Set_ShakeCam(true, 2.0f);
						m_pGameInstance->Set_TimeRatio(1.f);
					}
				}
				else
				{
					m_pGameInstance->Set_TimeRatio(1.f);
				}

			}
		}
		else if (m_iAttackCombo == 4)
		{
			if (m_pModelCom->Get_CurrentAnimationIndex() == Anim_Assassin_Attack04_A)
			{
				if (Index >= 7.5f && Index <= 25.f)
				{
					m_pTransformCom->Set_Speed(m_fSkillSpeed);
					m_pTransformCom->Go_Straight(fTimeDelta);
					if (!m_bComboZoom)
					{
						m_fAttackZoom += 3.f;
						m_bComboZoom = true;
					}

				}
				if (Index >= 15.5f && Index <= 40.f)
				{
					m_fAttTimer = 0.f;
				}
				if (Index >= 18.f && Index <= 21.f)
				{
					m_pLeft_Trail->Set_Color(_color(1.f, 0.5f, 0.1f, 1.f));
					m_pLeft_Trail->On();
					m_pLeft_Distortion_Trail->On();
				}

				if (Index >= 16.f && Index <= 18.f)
				{
					if (!m_bAttacked)
					{
						Check_Att_Collider(AT_Sword_Common);
						if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Common]))
						{
							m_pGameInstance->Set_TimeRatio(0.05f);
							m_bAttacked = true;
							m_fSkillSpeed = 0.f;

						}

						m_pGameInstance->Set_TimeRatio(1.f);

					}
					else
					{
						m_pGameInstance->Set_TimeRatio(1.f);
					}
					if (m_bAttacked)
					{
						m_pCam_Manager->Set_ShakeCam(true, 2.0f);
					}
				}

				else if (Index > 18.f && Index < 21.f)
				{
					m_bAttacked = false;
				}
				else if (Index >= 22.f && Index <= 24.f)
				{
					if (!m_bAttacked)
					{
						Check_Att_Collider(AT_Sword_Common);
						if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Common]))
						{
							m_pGameInstance->Set_TimeRatio(0.2f);
							m_bAttacked = true;

						}
					}
					m_pCam_Manager->Set_ShakeCam(true, 2.0f);
				}
				else
				{
					m_pGameInstance->Set_TimeRatio(1.f);
				}
			}
		}

	}
	else if (m_eState == Skill1)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 7.f && Index <= 16.f)
		{
			m_iHP = 1;
		}
		else
		{
			m_iHP = 0;
		}

		if (Index >= 9.f && Index < 13.f)
		{
			m_pLeft_Trail->Set_Color(_color(Colors::MediumPurple));
			m_pRight_Trail->Set_Color(_color(Colors::MediumPurple));
			m_pLeft_Trail->On();
			m_pLeft_Distortion_Trail->On();
			m_pRight_Trail->On();
			m_pRight_Distortion_Trail->On();
		}

		if (Index >= 5.f && Index < 15.f)
		{
			m_pTransformCom->Set_Speed(3.f);
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
		if (Index >= 5.f && Index < 17.f)
		{
			Cam_AttackZoom(2.6f);
		}
		if (Index >= 11.f && Index <= 13.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill1);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill1]))
				{
					m_pGameInstance->Set_TimeRatio(0.4f);
					m_bAttacked = true;
					m_pCam_Manager->Set_ShakeCam(true, 2.0f);

				}
			}
		}
		else if (Index > 13.5f && Index <= 15.5f)
		{
			m_bAttacked = false;
		}
		else if (Index >= 15.5f && Index <= 17.5f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill1);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill1]))
				{
					m_pGameInstance->Set_TimeRatio(0.4f);
					m_bAttacked = true;
					m_pCam_Manager->Set_ShakeCam(true, 2.0f);
				}

			}
		}
		else if (Index >= 26.f && !m_bMove_AfterSkill)
		{
			m_bMove_AfterSkill = true;
		}
		else
		{
			m_pGameInstance->Set_TimeRatio(1.f);
		}

	}
	else if (m_eState == Skill2)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 6.f && Index < 66.f)
		{
			m_pLeft_Trail->Set_Color(_color(1.f, 0.5f, 0.1f, 1.f));
			m_pRight_Trail->Set_Color(_color(1.f, 0.5f, 0.1f, 1.f));

			m_pLeft_Trail->On();
			m_pLeft_Distortion_Trail->On();
			m_pRight_Trail->On();
			m_pRight_Distortion_Trail->On();
		}
		if (Index >= 5.f && Index <= 31.f)
		{
			m_pTransformCom->Set_Speed(m_fSkillSpeed);
			m_pTransformCom->Go_Straight(fTimeDelta);
		}

		if (Index >= 55.f && Index <= 67.f)
		{
			m_pTransformCom->Set_Speed(m_fSkillSpeed / 3);
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
		if (Index >= 30.f && Index <= 32.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill2);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill2]))
				{
					m_pGameInstance->Set_TimeRatio(0.4f);
					m_bAttacked = true;
					m_pCam_Manager->Set_ShakeCam(true, 2.0f);
					m_fSkillSpeed = 3.f;
				}
			}

		}
		else if (Index > 33.f && Index <= 35.f)
		{
			m_bAttacked = false;
		}
		else if (Index >= 36.f && Index <= 40.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill2);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill2]))
				{
					m_pGameInstance->Set_TimeRatio(0.4f);
					m_bAttacked = true;
					m_fSkillSpeed = 3.f;
					m_pCam_Manager->Set_ShakeCam(true, 2.0f);
				}

			}
		}
		else if (Index > 40.f && Index <= 43.f)
		{
			m_bAttacked = false;
		}
		else if (Index >= 52.f && Index <= 54.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill2);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill2]))
				{
					m_pGameInstance->Set_TimeRatio(0.4f);
					m_bAttacked = true;
					m_fSkillSpeed = 3.f;
					m_pCam_Manager->Set_ShakeCam(true, 2.0f);
				}

			}
		}
		else if (Index > 55.f && Index <= 57.f)
		{
			m_bAttacked = false;
		}
		else if (Index >= 65.f && Index <= 67.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill2);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill2]))
				{
					m_pGameInstance->Set_TimeRatio(0.4f);
					m_bAttacked = true;
					m_fSkillSpeed = 3.f;
					m_pCam_Manager->Set_ShakeCam(true, 1.0f);
				}

			}
		}
		else if (Index >= 74.f && !m_bMove_AfterSkill)
		{
			m_bMove_AfterSkill = true;
		}
		else
		{
			m_pGameInstance->Set_TimeRatio(1.f);
		}
	}
	else if (m_eState == Skill3)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 19.f && Index < 29.f)
		{
			m_pLeft_Trail->Set_Color(_color(1.f, 0.5f, 0.1f, 1.f));
			m_pLeft_Trail->On();
			m_pLeft_Distortion_Trail->On();
		}
		if (Index >= 2.f && Index < 12.f)
		{
			m_bHide = true;
		}
		else if (Index >= 13.f && Index < 15.f)
		{
			m_bHide = false;
		}
		else if (Index >= 15.f && Index <= 27.f)
		{
			Cam_AttackZoom(2.5f);
			m_pTransformCom->Set_Speed(m_fSkillSpeed);
			m_pTransformCom->Go_Straight(fTimeDelta);
		}


		if (Index >= 27.f && Index <= 29.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill3);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill3]))
				{
					m_pGameInstance->Set_TimeRatio(0.5f);
					m_bAttacked = true;
					m_pCam_Manager->Set_ShakeCam(true, 2.0f);
					m_fSkillSpeed = 0.f;
				}
			}
		}
		else if (Index > 29.f && Index <= 31.f)
		{
			m_bAttacked = false;
		}
		else if (Index >= 32.f && Index <= 34.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill3);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill3]))
				{
					m_bAttacked = true;
					m_pCam_Manager->Set_ShakeCam(true, 1.5f);
					Cam_AttackZoom(3.f);
					m_fSkillSpeed = 0.f;
				}

			}
		}
		else if (Index >= 45.f && !m_bMove_AfterSkill)
		{
			m_bMove_AfterSkill = true;
		}
		else
		{
			m_pGameInstance->Set_TimeRatio(1.f);
		}

	}
	else if (m_eState == Skill4)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 5.f && Index <= 17.f)
		{
			m_pLeft_Trail->Set_Color(_color(Colors::MediumPurple));
			m_pRight_Trail->Set_Color(_color(Colors::MediumPurple));
			m_pLeft_Trail->On();
			m_pLeft_Distortion_Trail->On();
			m_pRight_Trail->On();
			m_pRight_Distortion_Trail->On();
		}
		if (Index >= 23.f && Index <= 27.f)
		{
			m_pLeft_Trail->On();
			m_pLeft_Distortion_Trail->On();
			m_pRight_Trail->On();
			m_pRight_Distortion_Trail->On();
		}
		if (Index >= 54.f && Index <= 70.f)
		{
			m_pLeft_Trail->On();
			m_pLeft_Distortion_Trail->On();
			m_pRight_Trail->On();
			m_pRight_Distortion_Trail->On();
		}

		if (Index >= 14.f && Index <= 20.f)
		{
			m_pGameInstance->Set_TimeRatio(0.3f);
		}
		else if (Index >= 26.f && Index <= 30.f)
		{
			m_pGameInstance->Set_TimeRatio(0.3f);
		}
		else if (Index >= 35.f && Index <= 40.f)
		{
			m_pGameInstance->Set_TimeRatio(0.5f);

		}
		else if (Index >= 56.f && Index <= 59.f)
		{
			m_pGameInstance->Set_TimeRatio(0.3f);

		}
		else if (Index >= 67.f && Index <= 70.f)
		{
			m_pGameInstance->Set_TimeRatio(0.3f);

		}
		else
		{
			m_pGameInstance->Set_TimeRatio(1.f);
		}

		if (Index >= 10.f && Index <= 17.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill4);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill4]))
				{
					m_bAttacked = true;
				}

			}

			if (!m_pCam_Manager->Get_AimMode())
			{
				m_SaveCamPos = m_pCameraTransform->Get_State(State::Pos);
				m_SaveCamLook = m_pCameraTransform->Get_State(State::Look);
			}

			//if (m_bAttacked)
			{
				m_pCam_Manager->Set_ShakeCam(true, 0.5f);
			}
			m_pCam_Manager->Set_AimMode(true, _vec3(-0.8f, 2.0f, 1.2f));


		}
		else if (Index >= 18.f && Index < 24.f)
		{
			m_bAttacked = false;
			m_pCam_Manager->Set_AimMode(false);
		}
		else if (Index >= 25.f && Index <= 29.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill4);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill4]))
				{
					m_bAttacked = true;
				}
			}
			//if (m_bAttacked)
			{
				m_pCam_Manager->Set_ShakeCam(true, 0.5f);
			}
			m_pCam_Manager->Set_AimMode(true, _vec3(0.8f, 2.0f, 1.2f));
			//m_pCameraTransform->LookAt(m_pTransformCom->Get_CenterPos());
		}
		else if (Index >= 33.f && Index <= 35.f)
		{
			m_pCam_Manager->Set_AimMode(false);
			m_bAttacked = false;
		}
		else if (Index >= 55.f && Index <= 58.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill4);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill4]))
				{
					m_bAttacked = true;
				}
			}
			//if (m_bAttacked)
			{
				m_pCam_Manager->Set_ShakeCam(true, 0.5f);
			}
			Cam_AttackZoom(3.6f);
		}
		else if (Index >= 59.f && Index <= 62.f)
		{
			m_bAttacked = false;
		}
		else if (Index >= 67.f && Index <= 69.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill4);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill4]))
				{
					m_bAttacked = true;
				}
			}
			//if (m_bAttacked)
			{
				m_pCam_Manager->Set_ShakeCam(true, 0.5f);
			}
			Cam_AttackZoom(4.4f);
		}
		else if (Index >= 80.f && !m_bMove_AfterSkill)
		{
			m_bMove_AfterSkill = true;
		}
	}

}
void CPlayer::After_BowAtt(_float fTimeDelta)
{
	_float Index = m_pModelCom->Get_CurrentAnimPos();

	if (m_eState == Attack)
	{
		switch (m_iAttackCombo)
		{
		case 1:
			if (Index >= 4.f && Index <= 5.f && !m_bLockOn)
			{
				Cam_AttackZoom(2.5f);

			}
			else if (Index >= 5.f && Index <= 7.f && m_ReadyArrow)
			{
				Create_Arrow(AT_Bow_Common);
				m_pGameInstance->Set_TimeRatio(0.3f);
				m_ReadyArrow = false;
			}
			else
			{
				m_pGameInstance->Set_TimeRatio(1.f);
			}
			break;
		case 2:
			if (Index >= 5.f && Index <= 6.f && !m_bLockOn)
			{
				m_fAttackZoom += 0.7f;
			}
			else if (Index >= 6.f && Index <= 8.f && m_ReadyArrow)
			{

				Create_Arrow(AT_Bow_Common);
				m_pGameInstance->Set_TimeRatio(0.3f);
				m_ReadyArrow = false;
			}
			else
			{
				m_pGameInstance->Set_TimeRatio(1.f);
			}
			break;
		case 3:
			if (Index >= 6.f && Index <= 7.f)
			{
				m_fAttackZoom += 0.7f;
			}
			else if (Index >= 7.f && Index <= 9.f && m_ReadyArrow)
			{
				Create_Arrow(AT_Bow_Common);
				m_pGameInstance->Set_TimeRatio(0.3f);
				m_ReadyArrow = false;
			}
			else
			{
				m_pGameInstance->Set_TimeRatio(1.f);
			}
			break;
		case 4:
			if (Index >= 5.f && Index <= 6.f)
			{
				m_fAttackZoom += 0.7f;
			}
			else if (Index >= 2.f && Index <= 4.f && m_ReadyArrow)
			{
				Create_Arrow(AT_Bow_Common);
				m_pGameInstance->Set_TimeRatio(0.3f);
				m_ReadyArrow = false;
			}
			else
			{
				m_pGameInstance->Set_TimeRatio(1.f);
			}
			break;
		default:
			break;
		}
	}
	else if (m_eState == Skill1)
	{

		if (Index >= 16.f && Index <= 19.f && m_ReadyArrow)
		{
			Create_Arrow(AT_Bow_Skill1);
			m_ReadyArrow = false;
		}
		else if (Index > 20.f && Index <= 25.f && !m_ReadyArrow)
		{
			m_ReadyArrow = true;
		}
		else if (Index >= 30.f && Index <= 32.f && m_ReadyArrow)
		{
			Create_Arrow(AT_Bow_Skill1);
			m_ReadyArrow = false;
		}
		else if (Index > 33.f && Index <= 35.f && !m_ReadyArrow)
		{
			m_ReadyArrow = true;
		}
		else if (Index >= 47.f && Index <= 49.f && m_ReadyArrow)
		{
			Create_Arrow(AT_Bow_Skill1);
			m_ReadyArrow = false;
		}
		else if (Index >= 64.f && !m_bMove_AfterSkill)
		{
			m_bMove_AfterSkill = true;
		}
	}
	else if (m_eState == Skill2)
	{
		if (m_bLockOn)
		{
			if (Index >= 29.f && Index <= 34.f)
			{
				m_pGameInstance->Set_TimeRatio(0.2f);
				Cam_AttackZoom(10.f);
			}
			else if (Index >= 35.f && Index <= 37.f)
			{
				Cam_AttackZoom(0.f);
			}
			else
			{
				m_pGameInstance->Set_TimeRatio(1.f);
			}
		}

		if (Index > 5.f && Index < 25.f)
		{
			m_pTransformCom->Go_Backward(fTimeDelta);
		}
		if (Index >= 5.f && Index <= 6.f)
		{
			m_pTransformCom->Jump(7.f);
		}
		else if (Index >= 34.f && Index <= 36.f && m_ReadyArrow)
		{
			m_pGameInstance->Set_TimeRatio(1.f);
			Create_Arrow(AT_Bow_Skill2);
			m_ReadyArrow = false;
		}
		else if (Index >= 51.f && !m_bMove_AfterSkill)
		{
			m_bMove_AfterSkill = true;
		}
	}
	else if (m_eState == Skill3)
	{
		if (Index >= 3.f && Index < 55.f)
		{
			Cam_AttackZoom(-3.f);
		}
		if (Index >= 19.f && Index <= 21.f && m_ReadyArrow)
		{
			Create_Arrow(AT_Bow_Skill3_Start);
			m_ReadyArrow = false;
		}
		else if (Index >= 22.f && Index <= 24.f)
		{
			m_ReadyArrow = true;
		}
		else if (Index >= 50.f && m_ReadyArrow)
		{
			Create_Arrow(AT_Bow_Skill3);
			m_ReadyArrow = false;
		}
		else if (Index >= 51.f && !m_bMove_AfterSkill)
		{
			m_bMove_AfterSkill = true;
		}
	}
	else if (m_eState == Skill4)
	{
		if (Index >= 2.f && Index < 50.f)
		{
			Cam_AttackZoom(-2.f);
		}
		if (Index >= 26.f && Index <= 28.f)
		{
			m_pCam_Manager->Set_ShakeCam(true, 1.f);
			m_pGameInstance->Set_TimeRatio(0.2f);
			m_UsingMotionBlur = true;
		}
		else if (Index >= 52.f && !m_bMove_AfterSkill)
		{
			m_bMove_AfterSkill = true;
		}
		else
		{
			m_pGameInstance->Set_TimeRatio(1.f);
			m_UsingMotionBlur = false;
		}
	}
	else if (m_eState == SkillR)
	{
		if (Index >= 8.f && Index <= 9.f)
		{
			Create_Arrow(AT_Bow_SkillR);
			//Cam_AttackZoom(2.f);
			//m_pCam_Manager->Set_ShakeCam(true, 0.1f);
		}
	}

}

void CPlayer::Create_Arrow(ATTACK_TYPE Att_Type)
{
	_mat OffsetMat{};
	if (m_bLockOn)
	{
		OffsetMat = _mat::CreateTranslation(_vec3(0.f, 1.8f, 0.f));
	}
	else
	{
		OffsetMat = _mat::CreateTranslation(_vec3(0.f, 1.4f, 0.f));
	}

	_mat BoneMat = (*m_pModelCom->Get_BoneMatrix("bowstring"));

	_mat ArrowMat = OffsetMat * BoneMat * m_pTransformCom->Get_World_Matrix();
	Arrow_Type type{};




	switch (Att_Type)
	{
	case Client::AT_Bow_Common:
	{
		type.world = ArrowMat;
		type.vLook = m_pTransformCom->Get_State(State::Look);
		type.iDamage = m_Status.Attack + rand() % 30;
		type.Att_Type = Att_Type;

		if (m_bLockOn)
		{
			type.bAimMode = true;
		}
		else
		{
			type.MonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
		}

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &type)))
		{
			return;
		}
	}
	break;
	case Client::AT_Bow_Skill1:
	{
		type.world = ArrowMat;
		type.vLook = m_pTransformCom->Get_State(State::Look);
		type.Att_Type = AT_Bow_Skill1;
		type.iDamage = m_Status.Attack + rand() % 30 + 30;
		if (m_bLockOn)
		{
			type.bAimMode = true;
		}
		else
		{
			type.MonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
		}
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &type)))
		{
			return;
		}
	}
	break;
	case Client::AT_Bow_Skill2:
	{
		type.world = ArrowMat;
		type.vLook = m_pTransformCom->Get_State(State::Look);
		if (m_bLockOn)
		{
			type.bAimMode = true;
		}
		else
		{
			type.MonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
		}
		type.Att_Type = AT_Bow_Skill2;
		type.iDamage = (_int)((m_Status.Attack) * 2.5f) + rand() % 30;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &type)))
		{
			return;
		}
	}
	break;
	case Client::AT_Bow_Skill3_Start:
	{
		type.world = ArrowMat;
		type.vLook = m_pTransformCom->Get_State(State::Look);
		type.vLook.y += 0.6f;
		type.Att_Type = AT_Bow_Skill3_Start;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &type)))
		{
			return;
		}
	}
	break;
	case Client::AT_Bow_Skill3:
	{

		m_bArrowRain_Start = true;
		m_iArrowRain = 0;
	}
	break;
	case Client::AT_Bow_Skill4:
	{
		type.world = ArrowMat;
		type.vLook = m_pTransformCom->Get_State(State::Look);
		type.vLook.y -= 0.25f;
		type.Att_Type = AT_Bow_Skill4;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &type)))
		{
			return;
		}
	}
	break;
	case Client::AT_Bow_SkillR:
	{
		type.world = ArrowMat;
		type.vLook = m_pTransformCom->Get_State(State::Look);
		type.Att_Type = AT_Bow_SkillR;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &type)))
		{
			return;
		}
	}
	break;

	default:
		break;
	}

}
void CPlayer::Summon_Riding(Riding_Desc Type)
{
	Riding_Desc Desc{};
	Desc = Type;
	Desc.vSummonPos = m_pTransformCom->Get_State(State::Pos);
	m_bIsMount = true;


	wstring strName{};

	strName = TEXT("Prototype_GameObject_Riding");

	m_pRiding = dynamic_cast<CRiding*>(m_pGameInstance->Clone_Object(strName, &Desc));

	if (m_pRiding == nullptr)
	{
		return;
	}
}
void CPlayer::Tick_Riding(_float fTimeDelta)
{
	if (m_pRiding == nullptr)
	{
		return;
	}

	if (m_pRiding->Get_Delete())
	{
		m_pCam_Manager->Set_RidingZoom(false);
		m_pCam_Manager->Set_FlyCam(false);

		if (m_pRiding->Get_RidingType() != Bird)
		{
			m_pTransformCom->Set_Position(_vec3(m_pRiding->Get_Pos() + _vec3(0.f, 2.f, 0.f)));

			m_bIsMount = false;
			Safe_Release(m_pRiding);
		}
		else if (m_pRiding->Get_State() == Riding_Glide)
		{
			Safe_Release(m_pRiding);
			m_bIsMount = false;
		}
		else
		{
			Safe_Release(m_pRiding);
			m_bIsMount = false;
			m_pCam_Manager->Set_FlyCam(false);
		}

		_vec4 vLook = m_pTransformCom->Get_State(State::Look);
		vLook.y = 0.f;
		m_pTransformCom->LookAt_Dir(vLook);
		m_eState = Jump_End;
		return;
	}

	m_pTransformCom->Set_Matrix(m_pRiding->Get_Mat());
	//m_pTransformCom->Set_Position(_vec3(m_pRiding->Get_Pos()));
	Riding_State State = m_pRiding->Get_State();

	if (m_Riding_State != State)
	{
		if (State == Riding_Glide)
		{
			m_eState = Mount_Fly;
		}
		else if (State == Riding_Run)
		{
			m_eState = Mount_Run;

		}
		else if (State == Riding_Walk)
		{
			m_eState = Mount_Walk;
		}
		else
		{
			m_eState = Mount;
		}

		m_Riding_State = State;
	}
}


void CPlayer::Arrow_Rain()
{
	if (m_iArrowRain == 0)
	{
		_mat EffectMatrix{};
		m_vArrowLook = m_pTransformCom->Get_State(State::Look);
		m_vArrowLook = m_pTransformCom->Get_State(State::Pos) + m_vArrowLook * 10.f;
		if (m_vArrowRainPos == _vec4())
		{
			EffectMatrix = _mat::CreateScale(10.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + m_vArrowLook * 10.f) + _vec3(0.f, 0.2f, 0.f));
		}
		else
		{
			EffectMatrix = _mat::CreateScale(10.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateTranslation(_vec3(m_vArrowRainPos) + _vec3(0.f, 0.2f, 0.f));
		}
		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Player_Circle_Frame");
		Info.pMatrix = &EffectMatrix;
		m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Player_Circle_Base");
		Info.pMatrix = &EffectMatrix;
		m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	}

	if (m_iArrowRain < 80)
	{
		Arrow_Type Type{};
		Type.iDamage = (_int)(m_Status.Attack * 0.8f) + rand() % 30;
		Type.Att_Type = AT_Bow_Skill3;
		_float random = (_float)(rand() % 70);
		_int randommos = rand() % 2;
		if (randommos == 0)
		{
			random *= -1;
		}
		random *= 0.05f;
		_float  random2 = (_float)(rand() % 70);
		int randommo = rand() % 2;
		if (randommo == 0)
		{
			random2 *= -1;
		}
		random2 *= 0.05f;
		if (m_vArrowRainPos == _vec4())
		{
			Type.vPos = m_vArrowLook + _vec4(random, 13.f, random2, 0.f)/* + m_pTransformCom->Get_State(State::Right) * 4.f*/;
		}
		else
		{
			Type.vPos = m_vArrowRainPos + _vec4(random, 13.f, random2, 0.f);
		}
		Type.vLook = _vec4(0.01f, -1.f, 0.f, 0.f);

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &Type)))
		{
			return;
		}
		m_iArrowRain++;
	}
	else
	{
		m_vArrowRainPos = _vec4();
		m_bArrowRain_Start = false;
		Safe_Release(m_pFrameEffect);
		Safe_Release(m_pBaseEffect);
	}
}

void CPlayer::Init_State()
{

	if (m_eState != m_ePrevState)
	{
		m_ReadyArrow = true;
		m_Animation.isLoop = false;
		m_Animation.bRewindAnimation = false;
		m_Animation.bRestartAnimation = false;
		m_Animation.bSkipInterpolation = false;
		m_Animation.fDurationRatio = 1.f;
		m_Animation.fStartAnimPos = 0.f;
		m_iSuperArmor = 0;
		m_Animation.fAnimSpeedRatio = 2.f;
		m_bReadyMove = true;
		m_iHP = 0;
		if (m_pGameInstance->Get_TimeRatio() < 1.f)
		{
			m_pGameInstance->Set_TimeRatio(1.f);
		}


		switch (m_eState)
		{
		case Client::CPlayer::Idle:
		{
			m_Animation.iAnimIndex = Anim_idle_00;
			m_Animation.isLoop = true;

			m_hasJumped = false;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Walk:
		{
			m_Animation.iAnimIndex = Anim_Normal_Walk;
			m_Animation.isLoop = true;
			m_iSuperArmor = {};
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Run_Start:
		{
			m_Animation.bSkipInterpolation = true;
			m_Animation.iAnimIndex = Anim_Normal_run_start;
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
			m_Animation.fAnimSpeedRatio = 2.f - (m_bSlowSpeed*0.3f);
		}
		break;
		case Client::CPlayer::Run:
		{
			m_Animation.bSkipInterpolation = true;
			m_Animation.iAnimIndex = Anim_Normal_run;
			m_Animation.isLoop = true;
			m_iSuperArmor = {};
			m_hasJumped = false;
			m_Animation.fAnimSpeedRatio = 2.f - (m_bSlowSpeed * 0.3f);
		}
		break;
		case Client::CPlayer::Run_End:
		{
			m_Animation.iAnimIndex = Anim_Normal_run_stop;
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Jump_Start:
		{
			m_Animation.iAnimIndex = Anim_jump_start;
			m_Animation.isLoop = false;
			m_hasJumped = true;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Jump:
		{
			m_Animation.iAnimIndex = Anim_jump_loop;
			m_Animation.isLoop = true;
			m_hasJumped = true;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Jump_Run:
		{
			m_Animation.iAnimIndex = Anim_jump_end_run;
			m_Animation.isLoop = false;
			m_Animation.fDurationRatio = 0.95f;
			m_Animation.bSkipInterpolation = true;
			m_hasJumped = false;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Jump_End:
		{
			m_Animation.iAnimIndex = Anim_jump_end;
			m_Animation.isLoop = false;
			m_hasJumped = true;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Jump_Long_End:
		{
			m_Animation.iAnimIndex = Anim_jump_end_long;
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Attack:
		{
			m_Animation.isLoop = false;
			m_Animation.bSkipInterpolation = true;
			m_Animation.fAnimSpeedRatio = 3.f;
			m_hasJumped = false;
			m_bReadyMove = false;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Attack_Idle:
			if (m_Current_Weapon == WP_SWORD)
			{
				m_Animation.iAnimIndex = Anim_Assassin_Battle_Idle;
				m_Animation.isLoop = true;
			}
			else if (m_Current_Weapon == WP_BOW)
			{
				m_Animation.iAnimIndex = Anim_B_idle_end;
				m_Animation.isLoop = false;
			}

			m_hasJumped = false;
			m_iSuperArmor = {};
			break;
		case Client::CPlayer::Attack_Run:
		{
			if (m_Current_Weapon == WP_SWORD)
			{
				m_Animation.iAnimIndex = Anim_Assassin_Battle_Run_end;
			}
			else if (m_Current_Weapon == WP_BOW)
			{
				m_Animation.iAnimIndex = Anim_Sniper_Battle_Run_end;
			}
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Skill1:
			Skill1_Attack();
			break;
		case Client::CPlayer::Skill2:
			Skill2_Attack();
			break;
		case Client::CPlayer::Skill3:
			Skill3_Attack();
			break;
		case Client::CPlayer::Skill4:
			Skill4_Attack();
			break;
		case Client::CPlayer::SkillR:
			SkillR_Attack();
			break;
		case Client::CPlayer::AimMode_Start:
		{
			m_Animation.iAnimIndex = Anim_B_idle_end;
			m_hasJumped = false;
			//m_Animation.fDurationRatio = 0.1f;
			m_iSuperArmor = {};
			m_Animation.isLoop = false;
			m_Animation.bRewindAnimation = true;
			m_Animation.fAnimSpeedRatio = 5.f;
			m_Animation.fStartAnimPos = 60.f;
		}
		break;
		case Client::CPlayer::AimMode:
		{
			m_Animation.iAnimIndex = Anim_B_idle_end;
			m_hasJumped = false;
			m_Animation.fDurationRatio = 0.01f;
			m_iSuperArmor = {};
			m_Animation.isLoop = false;
		}
		break;
		case Client::CPlayer::AimMode_End:
		{
			m_Animation.iAnimIndex = Anim_Sniper_Battle_Idle_end;
			m_hasJumped = false;
			m_iSuperArmor = {};
			m_Animation.isLoop = false;
			m_Animation.fDurationRatio = 0.5f;
			m_Animation.fAnimSpeedRatio = 5.f;
			m_bLockOn = false;
		}
		break;
		case Client::CPlayer::Climb:
		{
			m_Animation.iAnimIndex = Anim_Climb_wait;
			m_Animation.isLoop = true;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Climb_U:
		{
			m_Animation.iAnimIndex = Anim_Climb_F;
			m_Animation.isLoop = true;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Climb_D:
		{
			m_Animation.iAnimIndex = Anim_Climb_B;
			m_Animation.isLoop = true;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Climb_R:
		{
			m_Animation.iAnimIndex = Anim_Climb_R;
			m_Animation.isLoop = true;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Climb_L:
		{
			m_Animation.iAnimIndex = Anim_Climb_L;
			m_Animation.isLoop = true;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Swim_Idle:
		{
			m_Animation.iAnimIndex = Anim_Swim_Idle;
			m_Animation.isLoop = true;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Swim:
		{
			m_Animation.iAnimIndex = Anim_swim_F;
			m_Animation.isLoop = true;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Swim_collect:
		{
			m_Animation.iAnimIndex = Anim_swim_collect;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Stun_Start:
		{
			m_Animation.iAnimIndex = Anim_Stun_start;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Stun:
		{
			m_Animation.iAnimIndex = Anim_stun;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Hit:
		{
			m_Animation.iAnimIndex = Anim_Stun_start;
			m_Animation.fDurationRatio = 0.4f;
			m_Animation.fAnimSpeedRatio = 2.f;
			m_Animation.fStartAnimPos = 18.f;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::KnockDown:
		{
			if (m_Current_Weapon == WP_SWORD)
				m_Animation.iAnimIndex = Anim_Assassin_knockdown;
			else
				m_Animation.iAnimIndex = Anim_Sniper_knockdown;

			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Revival_Start:
		{
			m_Animation.iAnimIndex = Anim_revival_start;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Revival_End:
		{
			m_Animation.iAnimIndex = Anim_revival_end;
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_Status.Current_Hp = m_Status.Max_Hp;
			m_Status.Current_Mp = m_Status.Max_Mp;
			CUI_Manager::Get_Instance()->Set_Hp(m_Status.Current_Hp, m_Status.Max_Hp);
		}
		break;
		case Client::CPlayer::Die:
		{
			m_Animation.iAnimIndex = Anim_die;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Collect_Start:
		{
			m_Animation.iAnimIndex = Anim_Collect_Start;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Collect_Loop:
		{
			m_Animation.iAnimIndex = Anim_Collect_loop;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Collect_End:
		{
			m_Animation.iAnimIndex = Anim_Collect_end;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Mining:
		{
			m_Animation.iAnimIndex = Anim_Mining;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Logging:
		{
			m_Animation.iAnimIndex = Anim_logging;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Mount:
		{
			m_Animation.iAnimIndex = Anim_Mount_Idle;
			m_Animation.isLoop = true;
		}
		break;
		case Client::CPlayer::Mount_Run:
		{
			m_Animation.iAnimIndex = Anim_Mount_Run;
			m_Animation.isLoop = true;
		}
		break;
		case Client::CPlayer::Mount_Walk:
		{
			m_Animation.iAnimIndex = Anim_Mount_Walk;
			m_Animation.isLoop = true;
		}
		break;
		case Client::CPlayer::Mount_Fly:
		{
			m_Animation.iAnimIndex = Anim_Mount_fly_run;
			m_Animation.isLoop = true;
		}
		break;
		default:
			break;
		}

		m_ePrevState = m_eState;
	}

	if (m_bLockOn)
	{
		CUI_Manager::Get_Instance()->Set_isTargeting(true);
	}
	else
	{
		CUI_Manager::Get_Instance()->Set_isTargeting(false);
	}
}

void CPlayer::Tick_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CPlayer::Idle:
		break;
	case Client::CPlayer::Walk:
		m_hasJumped = false;
		break;
	case Client::CPlayer::Run:
		m_eState = Run_End;
		break;
	case Client::CPlayer::Run_End:
		if (m_pModelCom->IsAnimationFinished(Anim_Normal_run_stop))
		{
			m_eState = Idle;
			m_hasJumped = false;
		}
		break;
	case Client::CPlayer::Attack:
	{
		Return_Attack_IdleForm();
		if (m_Current_Weapon == WP_SWORD)
		{
			if (m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack01_B) or
				m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack02_B) or
				m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack03_B) or
				m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack04_B)
				)
			{
				m_eState = Attack_Idle;

			}
		}
		if (m_Current_Weapon == WP_BOW)
		{
			if (m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_Attack_01_B) or
				m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_Attack_02_B) or
				m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_Attack_03_B) or
				m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_Attack_04_B))
			{
				if (m_bLockOn or m_pCam_Manager->Get_AimMode())
				{
					m_eState = AimMode;
				}
				else
				{
					m_eState = Attack_Idle;
				}
			}
		}
	}
	break;
	case Client::CPlayer::Attack_Idle:
	{

	}
	break;
	case Client::CPlayer::Attack_Run:
		if (m_Current_Weapon == WP_SWORD)
		{
			if (m_pModelCom->IsAnimationFinished(Anim_Assassin_Battle_Run_end))
			{
				m_eState = Run;
				m_hasJumped = false;
			}
		}

		if (m_Current_Weapon == WP_BOW)
		{
			if (m_pModelCom->IsAnimationFinished(Anim_Sniper_Battle_Run_end))
			{
				m_eState = Run;
				m_hasJumped = false;
			}
		}
		break;
	case Client::CPlayer::Skill1:
	{
		if (m_Current_Weapon == WP_SWORD)
		{

			if (m_pModelCom->IsAnimationFinished(m_SwordSkill[0]))
			{
				m_eState = Attack_Idle;

			}
		}
		else if (m_Current_Weapon == WP_BOW)
		{
			if (m_bLockOn)
			{
				if (m_pModelCom->IsAnimationFinished(m_BowSkill[0]))
				{
					m_eState = AimMode;

				}
			}
			else
			{

				if (m_pModelCom->IsAnimationFinished(m_BowSkill[0]))
				{
					m_eState = Attack_Idle;

				}
			}
		}
	}
	break;
	case Client::CPlayer::Skill2:
	{
		if (m_Current_Weapon == WP_SWORD)
		{

			if (m_pModelCom->IsAnimationFinished(m_SwordSkill[1]))
			{
				m_eState = Attack_Idle;
			}
		}
		else if (m_Current_Weapon == WP_BOW)
		{
			if (m_bLockOn)
			{
				if (m_pModelCom->IsAnimationFinished(m_BowSkill[1]))
				{
					m_eState = AimMode;

				}
			}
			else
			{

				if (m_pModelCom->IsAnimationFinished(m_BowSkill[1]))
				{
					m_eState = Attack_Idle;

				}
			}
		}
	}
	break;
	case Client::CPlayer::Skill3:
	{
		if (m_Current_Weapon == WP_SWORD)
		{

			if (m_pModelCom->IsAnimationFinished(m_SwordSkill[2]))
			{
				m_eState = Attack_Idle;
			}
		}
		else if (m_Current_Weapon == WP_BOW)
		{
			if (m_bLockOn)
			{
				if (m_pModelCom->IsAnimationFinished(m_BowSkill[2]))
				{
					m_eState = AimMode;

				}
			}
			else
			{
				if (m_pModelCom->IsAnimationFinished(m_BowSkill[2]))
				{
					m_eState = Attack_Idle;

				}
			}

		}
	}
	break;
	case Client::CPlayer::AimMode_End:
	{
		if (m_pModelCom->IsAnimationFinished(Anim_Sniper_Battle_Idle_end))
		{
			m_eState = Idle;
		}
	}
	break;
	case Client::CPlayer::Skill4:
	{
		if (m_Current_Weapon == WP_SWORD)
		{

			if (m_pModelCom->IsAnimationFinished(m_SwordSkill[3]))
			{
				m_eState = Attack_Idle;
			}
		}
		else if (m_Current_Weapon == WP_BOW)
		{
			if (m_pModelCom->IsAnimationFinished(m_BowSkill[3]))
			{
				m_eState = AimMode;

			}
			else
			{
				if (m_pModelCom->IsAnimationFinished(m_BowSkill[3]))
				{
					m_eState = Attack_Idle;

				}
			}

		}
	}
	break;
	case Client::CPlayer::SkillR:
		if (m_Current_Weapon == WP_SWORD)
		{
		}
		else if (m_Current_Weapon == WP_BOW)
		{
			if (m_pModelCom->IsAnimationFinished(m_BowSkill[4]))
			{
				m_eState = AimMode_Start;
			}
		}
		break;
	case Client::CPlayer::AimMode_Start:

		break;
	case Client::CPlayer::Jump_Start:
		if (m_pModelCom->IsAnimationFinished(Anim_jump_start))
		{
			m_eState = Jump;
		}
		break;
	case Client::CPlayer::Jump:
		break;
	case Client::CPlayer::Jump_End:
		if (m_pModelCom->IsAnimationFinished(Anim_jump_end))
		{
			m_eState = Idle;
		}
		break;
	case Client::CPlayer::Jump_Long_End:
		if (m_pModelCom->IsAnimationFinished(Anim_jump_end_long))
		{
			m_eState = Idle;
		}
		break;
	case Client::CPlayer::Jump_Run:
		if (m_pModelCom->IsAnimationFinished(Anim_jump_end_run))
		{
			m_eState = Run;
			m_Animation.iAnimIndex = Anim_Normal_run;
		}
		break;
	case Client::CPlayer::Stun_Start:
		if (m_pModelCom->IsAnimationFinished(Anim_Stun_start))
		{
			m_eState = Stun;
		}
		break;
	case Client::CPlayer::Stun:
		if (m_pModelCom->IsAnimationFinished(Anim_stun))
		{
			m_eState = Idle;
		}
		break;
	case Client::CPlayer::Swim_Idle:
		break;
	case Client::CPlayer::Swim:
		if (!m_bReadySwim)
		{
			m_eState = Idle;
		}
		if (m_pModelCom->IsAnimationFinished(Anim_swim_F))
		{
			m_eState = Swim_Idle;
		}
		break;
	case Client::CPlayer::Swim_collect:
		m_Animation.bRestartAnimation = false;
		if (m_pModelCom->IsAnimationFinished(Anim_swim_collect))
		{
			m_iSwimCollectCount++;
			if (m_iSwimCollectCount >= 3)
			{
				m_eState = Swim_Idle;
				m_Animation.iAnimIndex = Anim_Swim_Idle;
			}
			else
			{
				m_Animation.iAnimIndex = Anim_swim_collect;
				m_Animation.bRestartAnimation = true;
			}
		}
		break;
	case Client::CPlayer::Hit:
		if (m_pModelCom->IsAnimationFinished(Anim_Stun_start))
		{
			m_eState = Idle;
		}
		break;
	case Client::CPlayer::KnockDown:
		if (m_Current_Weapon == WP_SWORD)
		{
			if (m_pModelCom->IsAnimationFinished(Anim_Assassin_knockdown))
			{
				m_eState = Idle;
			}
		}
		else
		{
			if (m_pModelCom->IsAnimationFinished(Anim_Sniper_knockdown))
			{
				m_eState = Idle;
			}
		}
		break;
	case Client::CPlayer::Revival_Start:
		if (m_pModelCom->IsAnimationFinished(Anim_revival_start))
		{
			m_eState = Revival_End;
		}
		break;
	case Client::CPlayer::Revival_End:
		if (m_pModelCom->IsAnimationFinished(Anim_revival_end))
		{
			m_eState = Idle;
		}
		break;
	case Client::CPlayer::Die:
		if (m_pModelCom->IsAnimationFinished(Anim_die))
		{
			m_eState = Revival_Start;
		}
		break;
	case Client::CPlayer::Collect_Start:
		if (m_pModelCom->IsAnimationFinished(Anim_Collect_Start))
		{
			m_eState = Collect_Loop;
		}
		break;
	case Client::CPlayer::Collect_Loop:
		if (m_pModelCom->IsAnimationFinished(Anim_Collect_loop))
		{
			m_eState = Collect_End;
		}
		break;
	case Client::CPlayer::Collect_End:
		if (m_pModelCom->IsAnimationFinished(Anim_Collect_end))
		{
			m_eState = Idle;
		}
		break;
	case Client::CPlayer::Mining:
		m_Animation.bRestartAnimation = false;
		if (m_pModelCom->IsAnimationFinished(Anim_Mining))
		{
			m_iMiningCount++;
			if (m_iMiningCount >= 3)
			{
				m_eState = Idle;
				m_Animation.iAnimIndex = Anim_idle_00;
			}
			else
			{
				m_Animation.iAnimIndex = Anim_Mining;
				m_Animation.bRestartAnimation = true;
			}
		}
		break;
	case Client::CPlayer::Logging:
		m_Animation.bRestartAnimation = false;
		if (m_pModelCom->IsAnimationFinished(Anim_logging))
		{
			m_iLoggingCount++;
			if (m_iLoggingCount >= 3)
			{
				m_eState = Idle;
				m_Animation.iAnimIndex = Anim_idle_00;

			}
			else
			{
				m_Animation.iAnimIndex = Anim_logging;
				m_Animation.bRestartAnimation = true;
			}
		}
		break;
	default:
		break;
	}

}

void CPlayer::Update_Trail(_float fTimeDelta)
{
	_mat UpMatrix{};
	_mat	BottomMatrix{};
	if (m_pCam_Manager->Get_CameraModeIndex() == CM_MAIN)
	{
		if (m_pLeft_Trail != nullptr)
		{
			BottomMatrix = *m_Left_Mat * m_pTransformCom->Get_World_Matrix();
			UpMatrix = _mat::CreateTranslation(0.f, -0.5f, 0.f) * *m_Left_Mat * m_pTransformCom->Get_World_Matrix();
			m_pLeft_Trail->Tick(UpMatrix.Position_vec3(), BottomMatrix.Position_vec3());

			//BottomMatrix = _mat::CreateTranslation(0.f, -0.45f, 0.f) * *m_Left_Mat * m_pTransformCom->Get_World_Matrix();;
			UpMatrix = _mat::CreateTranslation(0.f, -1.f, 0.f) * *m_Left_Mat * m_pTransformCom->Get_World_Matrix();
			m_pLeft_Distortion_Trail->Tick(UpMatrix.Position_vec3(), BottomMatrix.Position_vec3());
		}

		if (m_pRight_Trail != nullptr)
		{
			BottomMatrix = *m_Right_Mat * m_pTransformCom->Get_World_Matrix();
			UpMatrix = _mat::CreateTranslation(0.f, 0.5f, 0.f) * *m_Right_Mat * m_pTransformCom->Get_World_Matrix();
			m_pRight_Trail->Tick(UpMatrix.Position_vec3(), BottomMatrix.Position_vec3());
			m_pRight_Distortion_Trail->Tick(UpMatrix.Position_vec3(), BottomMatrix.Position_vec3());

			//BottomMatrix = _mat::CreateTranslation(0.f, 0.45f, 0.f) * *m_Left_Mat * m_pTransformCom->Get_World_Matrix();;
			//UpMatrix = _mat::CreateTranslation(0.f, 1.f, 0.f) * *m_Left_Mat * m_pTransformCom->Get_World_Matrix();
		}
	}
}

HRESULT CPlayer::Add_Components()
{

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_RTVTF"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Player"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Logo_Noise"), TEXT("Com_Dissolve_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
	{
		return E_FAIL;
	}
	// Com_Collider
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::OBB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(0.4f, 1.8f, 0.4f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
									  TEXT("Com_Player_Hit_OBB"), (CComponent**)&m_pHitCollider, &CollDesc)))
	{
		return E_FAIL;
	}

	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(1.4f, 2.f, 1.4f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.2f, 0.4f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
									  TEXT("Com_Collider_Common_Att"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Sword_Common]), &CollDesc)))
	{
		return E_FAIL;
	}





	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(2.f, 2.f, 1.6f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.2f, 0.6f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Skill1_Att"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Sword_Skill1]), &CollDesc)))
	{
		return E_FAIL;
	}

	CollDesc.vExtents = _vec3(1.5f, 2.0f, 0.7f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.3f, 0.5f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Parrying"), reinterpret_cast<CComponent**>(&m_pParryingCollider), &CollDesc)))
	{
		return E_FAIL;
	}

	CollDesc.vExtents = _vec3(0.8f, 2.f, 1.8f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.2f, 0.5f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Skill3_Att"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Sword_Skill3]), &CollDesc)))
	{
		return E_FAIL;
	}
	CollDesc.vExtents = _vec3(2.3f, 2.f, 1.8f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.2f, 0.2f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Skill2_Att"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Sword_Skill2]), &CollDesc)))
	{
		return E_FAIL;
	}
	CollDesc.vExtents = _vec3(3.f, 2.f, 3.f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.2f, 0.2f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Skill4_Att"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Sword_Skill4]), &CollDesc)))
	{
		return E_FAIL;
	}


	CollDesc.eType = ColliderType::Frustum;
	_mat matView = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	_mat matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(50.f), 2.f, 0.01f, 20.f);


	CollDesc.matFrustum = matView * matProj;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_CommonBow_Att"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Bow_Common]), &CollDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{

	if (m_ShaderIndex == VTFPass_Main_Rim)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &m_vRimColor, sizeof _vec4)))
		{
			return E_FAIL;
		}
	}

	// WorldMatrix 바인드
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	// ViewMatrix 바인드
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	// ProjMatrix 바인드
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	// 카메라 Far 바인드
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &CUI_Manager::Get_Instance()->Get_HairColor(), sizeof _vec4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissolveRatio, sizeof _float)))
	{
		return E_FAIL;
	}

	// 모션블러용 이전프레임 WorldMatrix 바인드
	if (FAILED(m_pTransformCom->Bind_OldWorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
	{
		return E_FAIL;
	}


	// 모션블러용 이전프레임 ViewMatrix 바인드
	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
	{
		return E_FAIL;
	}

	m_pModelCom->Set_UsingMotionBlur(true);

	// 뼈 바인드
	if (FAILED(m_pModelCom->Bind_Bone(m_pShaderCom)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
	{
		return E_FAIL;
	}





	return S_OK;
}

CPlayer* CPlayer::Create(_dev pDevice, _context pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();



	for (int i = 0; i < AT_End; i++)
	{
		Safe_Release(m_pAttCollider[i]);
	}


	Safe_Release(m_pLeft_Trail);
	Safe_Release(m_pRight_Trail);
	Safe_Release(m_pLeft_Distortion_Trail);
	Safe_Release(m_pRight_Distortion_Trail);

	Safe_Release(m_pAim);

	Safe_Release(m_pRiding);
	Safe_Release(m_pNameTag);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBaseEffect);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pHitCollider);
	Safe_Release(m_pFrameEffect);
	Safe_Release(m_pEffect_Shield);
	Safe_Release(m_pCameraTransform);
	Safe_Release(m_pParryingCollider);
	Safe_Release(m_pDissolveTextureCom);
}
