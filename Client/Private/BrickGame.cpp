#include "BrickGame.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"
#include "FadeBox.h"
#include "Event_Manager.h"
#include "Vehicle.h"
#include "Camera_Manager.h"
#include "Balloon.h"
#include "InfinityTower.h"
#include "NumEffect.h"
#include "BrickWall.h"

//const _float CBrickGame::m_iRow = 7;
//const _float CBrickGame::m_iCOl = 7;

CBrickGame::CBrickGame(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBrickGame::CBrickGame(const CBrickGame& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBrickGame::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CBrickGame::Init(void* pArg)
{

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

void CBrickGame::Tick(_float fTimeDelta)
{

	//중심 DirectX::XMFLOAT4 = {x=-2000.70496 y=11.4677677 z=-1999.06152 ...}
	//왼쪽벽DirectX::XMFLOAT4 = {x=-1991.24585 y=11.4677677 z=-2000.09204 ...}
	//위쪽 벽 DirectX::XMFLOAT4 = {x=-2000.19641 y=11.4677696 z=-2007.11536 ...}
	//DirectX::XMFLOAT4 = {x=-2000.28052 y=11.3177662 z=-1991.18335 ...}
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	_vec4 vPos = pPlayerTransform->Get_State(State::Pos);
	if (!m_isActive && CUI_Manager::Get_Instance()->Get_CurrentMiniGame() == (TOWER)BRICK)
	{
		CCamera_Manager::Get_Instance()->Set_CameraState(CS_BRICKGAME);
		CUI_Manager::Get_Instance()->Set_FullScreenUI(true);
		m_isActive = true;
		CTrigger_Manager::Get_Instance()->Teleport(TS_Minigame);
	}

	if (!m_isActive)
	{
		if (CCamera_Manager::Get_Instance()->Get_CameraState() == CS_BRICKGAME)
		{
			CCamera_Manager::Get_Instance()->Set_CameraState(CS_DEFAULT);
		}
		return;
	}


	if (m_iCombo > 0 && m_pGameInstance->Get_LayerSize(LEVEL_VILLAGE, TEXT("Layer_BrickBall")) == 0)
	{
		m_iCombo = 0;
	}
	if (m_fComboTime >= 1.5f)
	{
		m_iCombo = 0;
		m_fComboTime = 0.f;
	}

	if (m_isActive && m_pGameInstance->Key_Down(DIK_PGUP))
	{
		CCamera_Manager::Get_Instance()->Set_CameraState(CS_DEFAULT);
		m_isActive = false;
		CUI_Manager::Get_Instance()->Open_InfinityTower(true);
		return;
	}
	/*
	
	for (size_t i = 0; i < BRICKROW; i++)
	{
		for (size_t j = 0; j < BRICKCOL; j++)
		{
			if (m_pBalloon[i][j] != nullptr)
			{
				if (m_pBalloon[i][j]->Is_Coll() && m_fComboTime <1.5f)
				{
					CNumEffect::NUMEFFECT_DESC Desc{};
					Desc.bOrth = true;
					Desc.iDamage = m_iCombo;
					Desc.vTextPosition = _vec2(1000.f, 200.f);
					if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_NumEffect"), TEXT("Prototype_GameObject_NumEffect"), &Desc)))
					{
						return;
					}
					m_fComboTime = 0.f;
					m_iCombo++;					
				}
				m_pBalloon[i][j]->Tick(fTimeDelta);
			}
		}
	}*/
}

void CBrickGame::Late_Tick(_float fTimeDelta)
{
	if (!m_isActive)
	{
		return;
	}
	/*
	
	for (size_t i = 0; i < BRICKROW; i++)
	{
		for (size_t j = 0; j < BRICKCOL; j++)
		{
			if (m_pBalloon[i][j] != nullptr)
			{
				m_pBalloon[i][j]->Late_Tick(fTimeDelta);
			}
		}
	}*/
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
}

HRESULT CBrickGame::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}


	return S_OK;
}



HRESULT CBrickGame::Add_Parts()
{
	CBrickWall::WALL_DESC WallDesc{};
	WallDesc.rcRect = { (_long)1.f, (_long)0.f, (_long)0.f, (_long)0.f };
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_BrickWall"), &WallDesc)))
	{
		return E_FAIL;
	}
	WallDesc.rcRect = { (_long)0.f, (_long)1.f, (_long)0.f, (_long)0.f };
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_BrickWall"), &WallDesc)))
	{
		return E_FAIL;
	}
	WallDesc.rcRect = { (_long)0.f, (_long)0.f, (_long)1.f, (_long)0.f };
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_BrickWall"), &WallDesc)))
	{
		return E_FAIL;
	}
	WallDesc.rcRect = { (_long)0.f, (_long)0.f, (_long)0.f, (_long)1.f };
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_BrickWall"), &WallDesc)))
	{
		return E_FAIL;
	}

	_vec3 vStartPos = _vec3(-1994.24585f, 11.5f, -2006.11536f);
	for (_uint i = 0; i < BRICKROW; i++)
	{
		for (_uint j = 0; j < BRICKCOL; j++)
		{
			CBalloon::BALLOON_DESC Desc{};
			Desc.vColor = { 0.f, 0.6f, 1.f, 1.f };
			Desc.vPosition = _vec3(vStartPos.x - 2.2f * j, vStartPos.y, vStartPos.z + 2.2f * i);

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Balloons"), TEXT("Prototype_GameObject_Balloon"), &Desc)))
			{
				return E_FAIL;
			}
		 /*
			CBalloon * pBalloon = (CBalloon*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Balloon"), &Desc);
			if (pBalloon == nullptr)
			{
				return E_FAIL;
			}
		 */
			//m_pBalloon[i][j] = pBalloon;
		}

	}
	return S_OK;
}

HRESULT CBrickGame::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CBrickGame::Bind_ShaderResources()
{
	return S_OK;
}

CBrickGame* CBrickGame::Create(_dev pDevice, _context pContext)
{
	CBrickGame* pInstance = new CBrickGame(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBrickGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBrickGame::Clone(void* pArg)
{
	CBrickGame* pInstance = new CBrickGame(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBrickGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBrickGame::Free()
{
	__super::Free();

	if (!m_isPrototype)
	{
		for (_uint i = 0; i < BRICKROW; i++)
		{
			for (_uint j = 0; j < BRICKCOL; j++)
			{
				Safe_Release(m_pBalloon[i][j]);
			}
		}
	}

	Safe_Release(m_pRendererCom);
}