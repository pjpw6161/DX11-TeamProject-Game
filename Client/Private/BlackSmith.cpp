#include "BlackSmith.h"
#include "UI_Manager.h"

CBlackSmith::CBlackSmith(_dev pDevice, _context pContext)
	: CNPC(pDevice, pContext)
{
}

CBlackSmith::CBlackSmith(const CBlackSmith& rhs)
	: CNPC(rhs)
{
}

HRESULT CBlackSmith::Init_Prototype()
{
    return S_OK;
}

HRESULT CBlackSmith::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_BlackSmith");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 20), 0.f, static_cast<_float>(rand() % 20), 1.f));
	m_pTransformCom->Set_State(State::Pos, _vec4(70.f, 0.f, 100.f, 1.f));

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 2.f;

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.4f; // 높이(위 아래의 반구 크기 제외
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

void CBlackSmith::Tick(_float fTimeDelta)
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
	else
	{
		m_Animation.iAnimIndex = IDLE;
	}

	m_pModelCom->Set_Animation(m_Animation);

	m_pTransformCom->Gravity(fTimeDelta);

}

void CBlackSmith::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBlackSmith::Render()
{
	__super::Render();

    return S_OK;
}

CBlackSmith* CBlackSmith::Create(_dev pDevice, _context pContext)
{
	CBlackSmith* pInstance = new CBlackSmith(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBlackSmith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlackSmith::Clone(void* pArg)
{
	CBlackSmith* pInstance = new CBlackSmith(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBlackSmith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlackSmith::Free()
{
	__super::Free();
}
