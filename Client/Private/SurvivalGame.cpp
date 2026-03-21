#include "SurvivalGame.h"

#include "Launcher.h"
#include "Projectile.h"

CSurvivalGame::CSurvivalGame(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSurvivalGame::CSurvivalGame(const CSurvivalGame& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSurvivalGame::Init_Prototype()
{
    return S_OK;
}

HRESULT CSurvivalGame::Init(void* pArg)
{
	m_eCurPattern = PATTERN_INIT;

    return S_OK;
}

void CSurvivalGame::Tick(_float fTimeDelta)
{
	Init_Pattern(fTimeDelta);
	Tick_Pattern(fTimeDelta);
}

void CSurvivalGame::Late_Tick(_float fTimeDelta)
{
}

HRESULT CSurvivalGame::Render()
{
    return S_OK;
}

void CSurvivalGame::Init_Pattern(_float fTimeDelta)
{
	if (m_ePrePattern != m_eCurPattern)
	{
		switch (m_eCurPattern)
		{
		case Client::CSurvivalGame::PATTERN_INIT:
			
			m_fTime = 0.f;
			m_iLauncherCount = 0;

			break;
		case Client::CSurvivalGame::PATTERN_RANDOM_MISSILE:
			break;
		case Client::CSurvivalGame::PATTERN_FLOOR:
			break;
		case Client::CSurvivalGame::PATTERN_GUIDED_MISSILE:
			break;
		case Client::CSurvivalGame::PATTERN_LASER:
			break;
		case Client::CSurvivalGame::PATTERN_PIZZA:
			break;
		case Client::CSurvivalGame::PATTERN_TANGHURU:
			break;
		case Client::CSurvivalGame::PATTERN_SUICIDE_MONSTER:
			break;
		}

		m_ePrePattern = m_eCurPattern;
	}
}

void CSurvivalGame::Tick_Pattern(_float fTimeDelta)
{
	m_fTime += fTimeDelta;

	switch (m_eCurPattern)
	{
	case Client::CSurvivalGame::PATTERN_INIT:

		if (m_pGameInstance->Key_Down(DIK_UP))
		{
			m_eCurPattern = PATTERN_FLOOR;
		}

		break;

	case Client::CSurvivalGame::PATTERN_RANDOM_MISSILE:

		if (m_fTime >= 3.f)
		{
			CLauncher::LAUNCHER_TYPE eType = CLauncher::TYPE_RANDOM_POS;
			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Launcher"), TEXT("Prototype_GameObject_Launcher"), &eType);

			m_fTime = 0.f;
			++m_iLauncherCount;
		}

		if (m_iLauncherCount >= 3)
		{
			m_eCurPattern = PATTERN_INIT;
		}

		break;

	case Client::CSurvivalGame::PATTERN_FLOOR:

		if (m_fTime >= 1.f)
		{
			CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);

			CProjectile::PROJECTILE_DESC Desc = {};
			Desc.eType = CProjectile::TYPE_FLOOR;
			Desc.vStartPos = pPlayerTransform->Get_State(State::Pos);
			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Projectile"), TEXT("Prototype_GameObject_Projectile"), &Desc);

			m_fTime = 0.f;
			++m_iLauncherCount;
		}

		if (m_iLauncherCount >= 5)
		{
			m_eCurPattern = PATTERN_INIT;
		}

		break;

	case Client::CSurvivalGame::PATTERN_GUIDED_MISSILE:
		break;

	case Client::CSurvivalGame::PATTERN_LASER:
		break;

	case Client::CSurvivalGame::PATTERN_PIZZA:
		break;

	case Client::CSurvivalGame::PATTERN_TANGHURU:
		break;

	case Client::CSurvivalGame::PATTERN_SUICIDE_MONSTER:
		break;
	}
}

CSurvivalGame* CSurvivalGame::Create(_dev pDevice, _context pContext)
{
	CSurvivalGame* pInstance = new CSurvivalGame(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSurvivalGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSurvivalGame::Clone(void* pArg)
{
	CSurvivalGame* pInstance = new CSurvivalGame(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSurvivalGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSurvivalGame::Free()
{
	__super::Free();
}
