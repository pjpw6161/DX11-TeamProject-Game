#include "Projectile.h"

#include "Effect_Dummy.h"
#include "Effect_Manager.h"

_uint CProjectile::m_iProjectileID = 0;

CProjectile::CProjectile(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CProjectile::CProjectile(const CProjectile& rhs)
	: CGameObject(rhs)
{
}

HRESULT CProjectile::Init_Prototype()
{
    return S_OK;
}

HRESULT CProjectile::Init(void* pArg)
{
	m_ProjectileDesc = *(PROJECTILE_DESC*)pArg;

	switch (m_ProjectileDesc.eType)
	{
	case Client::CProjectile::TYPE_RANDOM_POS:

	{
		m_UpdateMatrix = _mat::CreateScale(0.5f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Yellow");
		Info.pMatrix = &m_UpdateMatrix;
		Info.isFollow = true;
		m_pBall = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Parti_Yellow");
		Info.pMatrix = &m_UpdateMatrix;
		Info.isFollow = true;
		m_pBallParticle = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		_vec4 vLauncherLook = m_ProjectileDesc.pLauncherTransform->Get_State(State::Look).Get_Normalized();
		_vec4 vDir = _vec4::Transform(vLauncherLook, _mat::CreateRotationY(XMConvertToRadians(45.f * m_iProjectileID)));

		m_pTransformCom->Set_Position(m_ProjectileDesc.vStartPos);
		m_pTransformCom->LookAt_Dir(vDir);

		++m_iProjectileID;

		if (m_iProjectileID == 8)
		{
			m_iProjectileID = 0;
		}
	}
		break;

	case Client::CProjectile::TYPE_FLOOR:

	{
		m_pTransformCom->Set_Position(m_ProjectileDesc.vStartPos);

		_mat Matrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Blue");
		Info.pMatrix = &Matrix;
		CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Parti_Blue");
		Info.pMatrix = &Matrix;
		CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);


		_mat FrameMatrix = _mat::CreateScale(3.f) * _mat::CreateRotationX(XMConvertToRadians(90.f))
			* _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Frame");
		Info.pMatrix = &FrameMatrix;
		m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		m_UpdateMatrix = _mat::CreateScale(m_fCircleRange) * _mat::CreateRotationX(XMConvertToRadians(90.f))
			* _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Base");
		Info.pMatrix = &m_UpdateMatrix;
		Info.isFollow = true;
		m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	}

		break;

	case Client::CProjectile::TYPE_GUIDED_MISSILE:
		break;

	case Client::CProjectile::TYPE_TANGHURU:
		break;
	}


    return S_OK;
}

void CProjectile::Tick(_float fTimeDelta)
{
	m_fTime += fTimeDelta;


	switch (m_ProjectileDesc.eType)
	{
	case Client::CProjectile::TYPE_RANDOM_POS:

		if (m_fTime >= 2.f)
		{
			Kill();
			Safe_Release(m_pBall);
			Safe_Release(m_pBallParticle);
		}

		m_pTransformCom->Go_Straight(fTimeDelta);

		m_UpdateMatrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

		if (m_pBall && m_pBallParticle)
		{
			m_pBall->Tick(fTimeDelta);
			m_pBallParticle->Tick(fTimeDelta);
		}

		break;

	case Client::CProjectile::TYPE_FLOOR:


		m_UpdateMatrix = _mat::CreateScale(m_fCircleRange) * _mat::CreateRotationX(XMConvertToRadians(90.f))
			* _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));

		//if (m_fTime >= 1.5f)
		if (m_fCircleRange >= 3.f)
		{
			Safe_Release(m_pFrameEffect);
			Safe_Release(m_pBaseEffect);

			m_fTime = 0.f;
		}

		if (m_pBaseEffect && m_pFrameEffect)
		{
			m_fCircleRange += 0.1f;

			m_pBaseEffect->Tick(fTimeDelta);
			m_pFrameEffect->Tick(fTimeDelta);
		}

		break;

	case Client::CProjectile::TYPE_GUIDED_MISSILE:
		break;
	case Client::CProjectile::TYPE_TANGHURU:
		break;
	}



}

void CProjectile::Late_Tick(_float fTimeDelta)
{
	switch (m_ProjectileDesc.eType)
	{
	case Client::CProjectile::TYPE_RANDOM_POS:

		if (m_pBall && m_pBallParticle)
		{
			m_pBall->Late_Tick(fTimeDelta);
			m_pBallParticle->Late_Tick(fTimeDelta);
		}

		break;

	case Client::CProjectile::TYPE_FLOOR:

		if (m_pFrameEffect && m_pBaseEffect)
		{
			m_pFrameEffect->Late_Tick(fTimeDelta);
			m_pBaseEffect->Late_Tick(fTimeDelta);
		}

		break;

	case Client::CProjectile::TYPE_GUIDED_MISSILE:
		break;
	case Client::CProjectile::TYPE_TANGHURU:
		break;
	}
}

HRESULT CProjectile::Render()
{
    return S_OK;
}

HRESULT CProjectile::Add_Components()
{
    return S_OK;
}

HRESULT CProjectile::Bind_ShaderResources()
{
    return S_OK;
}

CProjectile* CProjectile::Create(_dev pDevice, _context pContext)
{
	CProjectile* pInstance = new CProjectile(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CProjectile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProjectile::Clone(void* pArg)
{
	CProjectile* pInstance = new CProjectile(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CProjectile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile::Free()
{
	__super::Free();

	Safe_Release(m_pBall);
	Safe_Release(m_pBallParticle);

	Safe_Release(m_pFrameEffect);
	Safe_Release(m_pBaseEffect);
}
