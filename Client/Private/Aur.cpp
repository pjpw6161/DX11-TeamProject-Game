#include "Aur.h"

CAur::CAur(_dev pDevice, _context pContext)
	: CNPC(pDevice, pContext)
{
}

CAur::CAur(const CAur& rhs)
	: CNPC(rhs)
{
}

HRESULT CAur::Init_Prototype()
{
    return S_OK;
}

HRESULT CAur::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Aur");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, _vec4(71.f, 8.f, 97.f, 1.f));

	m_Animation.iAnimIndex = HANGING01;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 2.f;

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.8f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.6f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	return S_OK;
}

void CAur::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	if (false) // 나중에 조건 추가
	{
		m_bTalking = true;
	}

	if (m_bTalking == true)
	{
		m_Animation.iAnimIndex = TALK01;
	}

	m_pModelCom->Set_Animation(m_Animation);
}

void CAur::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CAur::Render()
{
	__super::Render();

    return S_OK;
}

CAur* CAur::Create(_dev pDevice, _context pContext)
{
	CAur* pInstance = new CAur(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CAur");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAur::Clone(void* pArg)
{
	CAur* pInstance = new CAur(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CAur");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAur::Free()
{
	__super::Free();
}
