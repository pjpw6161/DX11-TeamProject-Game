#include "ItemMerchant.h"
#include "Shop.h"
#include "3DUITex.h"
#include "UI_Manager.h"

CItemMerchant::CItemMerchant(_dev pDevice, _context pContext)
	: CNPC(pDevice, pContext)
{
}

CItemMerchant::CItemMerchant(const CItemMerchant& rhs)
	: CNPC(rhs)
{
}

HRESULT CItemMerchant::Init_Prototype()
{
	return S_OK;
}

HRESULT CItemMerchant::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_ItemMerchant");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	//m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 20), 0.f, static_cast<_float>(rand() % 20), 1.f));
	m_pTransformCom->Set_State(State::Pos, _vec4(75.f, 0.f, 100.f, 1.f));

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 2.f;

	CUI_Manager::Get_Instance()->Set_RadarPos(CUI_Manager::NPC, m_pTransformCom);

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.8f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.6f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	if (pArg)
	{
		if (FAILED(__super::Init(pArg)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

void CItemMerchant::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	if (m_pShop->IsActive())
	{
		m_pTransformCom->Set_State(State::Pos, _vec4(m_pTransformCom->Get_State(State::Pos).x, 1000.f, m_pTransformCom->Get_State(State::Pos).z, 1.f));
		m_isFadeReady = false;
	}
	else if (m_bTalking)
	{
		m_pTransformCom->Set_State(State::Pos, _vec4(m_pTransformCom->Get_State(State::Pos).x, 0.f, m_pTransformCom->Get_State(State::Pos).z, 1.f));
		m_bTalking = false;
	}

	if (m_bTalking == true)
	{

		m_Animation.iAnimIndex = TALK;
		m_pShop->Tick(fTimeDelta);
	}
	else
	{
		m_Animation.iAnimIndex = IDLE;
		m_pSpeechBubble->Tick(fTimeDelta);

	}

	CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Player_Hit_OBB"));
	_bool isColl = m_pColliderCom->Intersect(pCollider);
	m_isColl = isColl;
	if (!m_bTalking && isColl && m_pGameInstance->Key_Down(DIK_E))
	{
		CFadeBox::FADE_DESC Desc = {};
		Desc.fIn_Duration = 0.5f;
		Desc.fOut_Duration = 1.f;
		Desc.phasFadeCompleted = &m_isFadeReady;
		CUI_Manager::Get_Instance()->Add_FadeBox(Desc);

		for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
		{
			if (m_pGameInstance->Get_IsLoopingSound(i))
			{
				m_pGameInstance->FadeoutSound(i, fTimeDelta, 1.f, true, 0.3f);
			}
		}
	}
	if (m_isFadeReady and not m_bTalking)
	{
		m_pShop->Open_Shop();
		m_bTalking = true;
	}

	m_pModelCom->Set_Animation(m_Animation);

	__super::Update_Collider();

	m_pTransformCom->Gravity(fTimeDelta);
}

void CItemMerchant::Late_Tick(_float fTimeDelta)
{
	if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos), 2.f))
	{
		if (m_bTalking == true)
		{
			m_pTransformCom->Set_State(State::Pos, _vec4(m_pTransformCom->Get_State(State::Pos).x, 1000.f, m_pTransformCom->Get_State(State::Pos).z, 1.f));
			m_pShop->Late_Tick(fTimeDelta);
		}
		else
		{
			if (m_isColl)
			{
				m_pSpeechBubble->Late_Tick(fTimeDelta);
			}
		}

		__super::Late_Tick(fTimeDelta);
	}


#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif // _DEBUG

}

HRESULT CItemMerchant::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CItemMerchant::Add_Parts()
{
	m_pShop = (CShop*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Shop"));

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
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.vCenter = _vec3(0.f, 0.f, 0.f);
	CollDesc.fRadius = 8.f;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Roskva"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

CItemMerchant* CItemMerchant::Create(_dev pDevice, _context pContext)
{
	CItemMerchant* pInstance = new CItemMerchant(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItemMerchant");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItemMerchant::Clone(void* pArg)
{
	CItemMerchant* pInstance = new CItemMerchant(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CItemMerchant");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemMerchant::Free()
{
//	CUI_Manager::Get_Instance()->Delete_RadarPos(CUI_Manager::NPC, m_pTransformCom);
	__super::Free();
	Safe_Release(m_pShop);
	Safe_Release(m_pSpeechBubble);
}
