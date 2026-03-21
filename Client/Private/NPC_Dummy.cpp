#include "NPC_Dummy.h"
#include "UI_Manager.h"
#include "Dialog.h"

CNPC_Dummy::CNPC_Dummy(_dev pDevice, _context pContext)
	: CNPC(pDevice, pContext)
{
}

CNPC_Dummy::CNPC_Dummy(const CNPC_Dummy& rhs)
	: CNPC(rhs)
{
}

HRESULT CNPC_Dummy::Init_Prototype()
{
    return S_OK;
}

HRESULT CNPC_Dummy::Init(void* pArg)
{
	if (pArg)
	{
		m_pInfo = *(NPC_INFO*)pArg;
		m_strModelTag = m_pInfo.strNPCPrototype;
	}

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.vCenter = _vec3(0.f, 0.f, 0.f);
	CollDesc.fRadius = 12.f;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_NpcCol"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;


	CDialog::DIALOG_DESC DialogDesc = {};
	DialogDesc.eLevelID = LEVEL_STATIC;
	DialogDesc.pParentTransform = m_pTransformCom;
	DialogDesc.vPosition = _vec3(0.f, 2.2f, 0.f);
	DialogDesc.strText = TEXT("테스트입니다~");

#pragma region Animal

	if (m_strModelTag == TEXT("Prototype_Model_Donkey"))
	{
		m_Animation.iAnimIndex = 1;
		DialogDesc.strText = TEXT("히히히히힝");
	}

#pragma endregion Animal

#pragma region IDLE NPC

	if (m_strModelTag == TEXT("Prototype_Model_Dwarf_Male_002"))
	{
		m_Animation.iAnimIndex = 0;
		DialogDesc.strText = TEXT("우리 강아지 귀엽죠?");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_003"))
	{
		m_Animation.iAnimIndex = 2;
		DialogDesc.strText = TEXT("날씨가 참 좋네~");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_004"))
	{
		m_Animation.iAnimIndex = 2;
		DialogDesc.strText = TEXT("마나하임은 참 살기 좋은 곳이야");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_006"))
	{
		m_Animation.iAnimIndex = 4;
		DialogDesc.strText = TEXT("아니 얘는 5시에 보자면서 왜 안와");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_009"))
	{
		m_Animation.iAnimIndex = 1;
		DialogDesc.strText = TEXT("심심하다");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_013"))
	{
		DialogDesc.strText = TEXT("오늘 장터 반찬 뭐야?");
		m_Animation.iAnimIndex = 1;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_015"))
	{
		m_Animation.iAnimIndex = 2;
		DialogDesc.strText = TEXT("지나다니는 사람이 많구만");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_018"))
	{
		DialogDesc.strText = TEXT("우리집 고양이가 어디갔지?");
		m_Animation.iAnimIndex = 0;
	}

	if (m_strModelTag == TEXT("Prototype_Model_SkillMerchant"))
	{
		DialogDesc.strText = TEXT("안녕하세요");
		m_Animation.iAnimIndex = 0;
	}

#pragma endregion IDLE NPC

#pragma region DANCE NPC

	if (m_strModelTag == TEXT("Prototype_Model_Female_013"))
	{
		m_Animation.iAnimIndex = 1;
		DialogDesc.strText = TEXT("신난다~");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_027"))
	{
		DialogDesc.strText = TEXT("난 슬플 때 힙합을 춰");
		m_Animation.iAnimIndex = 2;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_Chi"))
	{
		DialogDesc.strText = TEXT("나랑 같이 춤추자");
		m_Animation.iAnimIndex = 3;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_016"))
	{
		DialogDesc.strText = TEXT("내 춤을 좀 멈춰줘");
		m_Animation.iAnimIndex = 0;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_020"))
	{
		DialogDesc.strText = TEXT("IDLE 상태로 바꿔주세요 ㅠㅠ");
		m_Animation.iAnimIndex = 0;
	}

#pragma endregion DANCE NPC

#pragma region TALK NPC

	if (m_strModelTag == TEXT("Prototype_Model_Female_002"))
	{
		m_Animation.iAnimIndex = 7;
		DialogDesc.strText = TEXT("놀라운 사실이야"); 
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_005"))
	{
		m_Animation.iAnimIndex = 1;
		DialogDesc.strText = TEXT("어머 세상에 정말이야?");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_010"))
	{
		m_Animation.iAnimIndex = 10;
		DialogDesc.strText = TEXT("걔가 그랬다니까~ ");
	}

#pragma endregion TALK NPC

#pragma region 기타 치는애

	if (m_strModelTag == TEXT("Prototype_Model_Male_027"))
	{
		DialogDesc.strText = TEXT("내 멋진 기타를 봐");
		m_Animation.iAnimIndex = 1;
	}

#pragma endregion 기타 치는애

	m_Animation.isLoop = true;
	m_Animation.fAnimSpeedRatio = 2.f;

	m_pDialog = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Dialog"), &DialogDesc);
	if (m_pDialog == nullptr)
	{
		return E_FAIL;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Donkey"))
	{
		PxCapsuleControllerDesc ControllerDesc{};
		ControllerDesc.height = 0.7f; // 높이(위 아래의 반구 크기 제외
		ControllerDesc.radius = 1.f; // 위아래 반구의 반지름
		ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
		ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
		ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
		ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

		m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);
	}

	else if (m_strModelTag == TEXT("Prototype_Model_Dwarf_Male_002"))
	{
		PxCapsuleControllerDesc ControllerDesc{};
		ControllerDesc.height = 0.4f; // 높이(위 아래의 반구 크기 제외
		ControllerDesc.radius = 0.8f; // 위아래 반구의 반지름
		ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
		ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
		ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
		ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

		m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);
	}

	else
	{
		PxCapsuleControllerDesc ControllerDesc{};
		ControllerDesc.height = 0.8f; // 높이(위 아래의 반구 크기 제외
		ControllerDesc.radius = 0.6f; // 위아래 반구의 반지름
		ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
		ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
		ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
		ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

		m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);
	}

	if (pArg)
	{
		if (FAILED(__super::Init(pArg)))
		{
			return E_FAIL;
		}
	}

    return S_OK;
}

void CNPC_Dummy::Tick(_float fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	if (m_strModelTag == TEXT("Prototype_Model_Male_009"))
	{
		dynamic_cast<CDialog*>(m_pDialog)->Set_Text(L"현재 프레임은 " + to_wstring(m_pGameInstance->Get_FPS()) + L"이구만.");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_027"))
	{
		m_fTimer += fTimeDelta;

		if (m_fTimer > 3.f)
		{
			_int iRandomNum = rand() % 3;

			switch (iRandomNum)
			{
			case 0:
				dynamic_cast<CDialog*>(m_pDialog)->Set_Text(L"내 멋진 기타를 봐");
				break;
			case 1:
				dynamic_cast<CDialog*>(m_pDialog)->Set_Text(L"딩가딩가링");
				break;
			case 2:
				dynamic_cast<CDialog*>(m_pDialog)->Set_Text(L"딩가디 딩가딩가");
				break;
			}

			m_fTimer = {};
		}
	}


	m_pModelCom->Set_Animation(m_Animation);
	m_pDialog->Tick(fTimeDelta);

	__super::Update_Collider();
	m_pTransformCom->Gravity(fTimeDelta);

}

void CNPC_Dummy::Late_Tick(_float fTimeDelta)
{
	if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos), 2.f))
	{
		__super::Late_Tick(fTimeDelta);
		CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Player_Hit_OBB"));
		_bool isColl = m_pColliderCom->Intersect(pCollider);
		if (isColl)
		{
			if (m_strModelTag == TEXT("Prototype_Model_Male_009"))
			{
				_int a = 10;
			}

			//wstring strTest = m_strModelTag;
			m_pDialog->Late_Tick(fTimeDelta);
		}
	}
}

HRESULT CNPC_Dummy::Render()
{
	__super::Render();

    return S_OK;
}

CNPC_Dummy* CNPC_Dummy::Create(_dev pDevice, _context pContext)
{
	CNPC_Dummy* pInstance = new CNPC_Dummy(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CNPC_Dummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNPC_Dummy::Clone(void* pArg)
{
	CNPC_Dummy* pInstance = new CNPC_Dummy(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CNPC_Dummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Dummy::Free()
{
	//CUI_Manager::Get_Instance()->Delete_RadarPos(CUI_Manager::NPC, m_pTransformCom);
	__super::Free();
	Safe_Release(m_pDialog);
}
