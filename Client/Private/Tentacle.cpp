#include "Tentacle.h"

#include "Effect_Dummy.h"
#include "Effect_Manager.h"

CTentacle::CTentacle(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CTentacle::CTentacle(const CTentacle& rhs)
	: CMonster(rhs)
{
}

HRESULT CTentacle::Init_Prototype()
{
    return S_OK;
}

HRESULT CTentacle::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Groar_Tentacle");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_Animation.iAnimIndex = 0;
	m_Animation.isLoop = true;
	m_Animation.fAnimSpeedRatio = 3.f;
	m_Animation.fStartAnimPos = 70.f; // 65 end

	_vec4 vPlayerPos = __super::Compute_PlayerPos();

	CTransform* pGroarTransform = GET_TRANSFORM("Layer_Groar_Boss", LEVEL_VILLAGE);
	_vec4 vGroarPos = pGroarTransform->Get_State(State::Pos);

	vPlayerPos.y = vGroarPos.y + 0.1f;

	_mat EffectMatrix = _mat::CreateScale(2.5f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateTranslation(_vec3(vPlayerPos)/* + _vec3(0.f, 0.2f, 0.f)*/);
			
	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Frame");
	Info.pMatrix = &EffectMatrix;
	m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Base");
	Info.pMatrix = &EffectMatrix;
	m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	_vec3 vSetPos = vPlayerPos + _vec3(0.f, 0.f, 1.2f);
	vSetPos.y = vGroarPos.y - 1.5f;
	m_pTransformCom->Set_Position(vSetPos);

    return S_OK;
}

void CTentacle::Tick(_float fTimeDelta)
{
	m_fTime += fTimeDelta;

	if (m_fTime >= 1.5f)
	{
		if (m_pBaseEffect && m_pFrameEffect)
		{
			Safe_Release(m_pFrameEffect);
			Safe_Release(m_pBaseEffect);

			EffectInfo Info{};
			_vec4 vTargetPos = m_pTransformCom->Get_State(State::Pos);
			vTargetPos -= _vec3(0.f, -1.55f, 1.2f);

			_mat EffectMatrix = _mat::CreateScale(2.5f) * _mat::CreateTranslation(_vec3(vTargetPos));
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Small_Crack_Stone");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Rock_Frag");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			_uint iDamage = m_iSmallDamage - rand() % 15;
			m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
		}

		if (m_pModelCom->Get_CurrentAnimPos() >= 65.f && m_pModelCom->Get_CurrentAnimPos() <= 67.f)
		{
			Kill();
		}

		if (m_Animation.fStartAnimPos == 70.f)
		{
			m_pGameInstance->Play_Sound(TEXT("WD_3130_WideBring_01_SFX_01"));
		}

		if (m_pModelCom->Get_CurrentAnimPos() >= 85.f && m_pModelCom->Get_CurrentAnimPos() <= 160.f)
		{
			if (m_iSoundChannel == -1)
			{
				_uint iSoundIndex = rand() % 5 + 1;
				wstring strSoundTag = TEXT("Whip_Whoosh_Base_A_SFX_0") + to_wstring(iSoundIndex);
				m_iSoundChannel = m_pGameInstance->Play_Sound(strSoundTag, 0.7f);
			}
			else
			{
				if (not m_pGameInstance->Get_IsPlayingSound(m_iSoundChannel))
				{
					m_iSoundChannel = -1;
				}
			}
		}
		m_pModelCom->Set_Animation(m_Animation);
		m_Animation.fStartAnimPos = 0.f;

	}

	if (m_pFrameEffect)
	{
		m_pFrameEffect->Tick(fTimeDelta);
	}
	
	if (m_pBaseEffect)
	{
		m_pBaseEffect->Tick(fTimeDelta);
	}

	Update_Collider();
}

void CTentacle::Late_Tick(_float fTimeDelta)
{
	if (m_pFrameEffect)
	{
		m_pFrameEffect->Late_Tick(fTimeDelta);
	}

	if (m_pBaseEffect)
	{
		m_pBaseEffect->Late_Tick(fTimeDelta);
	}

	if (m_fTime >= 1.5f)
	{
		__super::Late_Tick(fTimeDelta);
	}

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif

}

HRESULT CTentacle::Render()
{
	__super::Render();

    return S_OK;
}

HRESULT CTentacle::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::AABB;
	BodyCollDesc.vExtents = _vec3(1.f, 2.f, 1.f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_AABB"), (CComponent**)&m_pAttackColliderCom, &BodyCollDesc)))
	{
		return E_FAIL;
	}

    return S_OK;
}

void CTentacle::Update_Collider()
{
	_vec4 vPlayerPos = __super::Compute_PlayerPos();

	_mat Offset = _mat::CreateTranslation(0.f, 1.2f, 1.2f);
	m_pAttackColliderCom->Update(Offset * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix());
}

CTentacle* CTentacle::Create(_dev pDevice, _context pContext)
{
	CTentacle* pInstance = new CTentacle(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTentacle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTentacle::Clone(void* pArg)
{
	CTentacle* pInstance = new CTentacle(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTentacle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTentacle::Free()
{
	__super::Free();
}
