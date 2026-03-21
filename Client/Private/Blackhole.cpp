#include "Blackhole.h"

#include "Effect_Dummy.h"
#include "Effect_Manager.h"

CBlackhole::CBlackhole(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBlackhole::CBlackhole(const CBlackhole& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlackhole::Init_Prototype()
{
    return S_OK;
}

HRESULT CBlackhole::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	_mat EffectMatrix = _mat::CreateScale(9.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateTranslation(_vec3(vPlayerPos) + _vec3(0.f, 0.1f, 0.f));

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Frame");
	Info.pMatrix = &EffectMatrix;
	m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Base");
	Info.pMatrix = &EffectMatrix;
	m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);


	m_pTransformCom->Set_Position(vPlayerPos + _vec3(0.f, 0.1f, 0.f));

    return S_OK;
}

void CBlackhole::Tick(_float fTimeDelta)
{
	m_fLifeTime += fTimeDelta;

	if (m_fLifeTime >= 12.f)
	{
		if (m_iSoundChannel != -1)
		{
			m_pGameInstance->FadeoutSound(m_iSoundChannel, fTimeDelta, 1.f, false);
		}
		Kill();
		return;
	}

	if (m_fLifeTime >= 1.5f)
	{
		if (m_pBaseEffect && m_pFrameEffect)
		{
			Safe_Release(m_pFrameEffect);
			Safe_Release(m_pBaseEffect);

			// ºí·¢È¦ ÀÌÆåÆ® »ý¼º
			_mat EffectMatrix = _mat::CreateScale(2.7f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, -0.1f, 0.f)));
			EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Blackhole_Base");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			EffectMatrix = _mat::CreateScale(2.7f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.05f, 0.f)));
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Blackhole_Base2");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Blackhole_Frame");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Blackhole_Frame_Inner");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Blackhole_Frame_Ring");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			//
			EffectMatrix = _mat::CreateScale(3.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)/* + _vec3(0.f, -0.1f, 0.f)*/));
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Blackhole_Shine");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Blackhole_Shine_White");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_Blackhole_Spread_Circle");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
		}
	}

	if (m_fLifeTime >= 3.f)
	{
		if (m_pGameInstance->Get_ChannelCurPosRatio(m_iSoundChannel) >= 0.8f)
		{
			m_pGameInstance->FadeoutSound(m_iSoundChannel, fTimeDelta, 1.f, false);
			m_iSoundChannel = m_pGameInstance->Play_Sound(TEXT("BP_Skill_10020_SFX_01"), 0.5f, false, 0.1f);
			m_pGameInstance->FadeinSound(m_iSoundChannel, fTimeDelta);
		}
	}
	else if (m_fLifeTime >= 1.2f)
	{
		if (m_iSoundChannel == -1)
		{
			m_iSoundChannel = m_pGameInstance->Play_Sound(TEXT("Sfx_Boss_IdunDark_Atk_06"));
		}
		else if (m_pGameInstance->Get_ChannelCurPosRatio(m_iSoundChannel) >= 0.3f)
		{
			m_pGameInstance->FadeoutSound(m_iSoundChannel, fTimeDelta, 1.f, false);
			m_iSoundChannel = m_pGameInstance->Play_Sound(TEXT("BP_Skill_10020_SFX_01"), 0.5f, false, 0.1f);
		}
	}

	if (m_iSoundChannel != -1)
	{
		if (not m_pGameInstance->Get_IsPlayingSound(m_iSoundChannel))
		{
			m_iSoundChannel = -1;
		}
	}

	if (m_pFrameEffect && m_pBaseEffect)
	{
		m_pFrameEffect->Tick(fTimeDelta);
		m_pBaseEffect->Tick(fTimeDelta);
	}
}

void CBlackhole::Late_Tick(_float fTimeDelta)
{
	if (m_pFrameEffect && m_pBaseEffect)
	{
		m_pFrameEffect->Late_Tick(fTimeDelta);
		m_pBaseEffect->Late_Tick(fTimeDelta);
	}

	//m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CBlackhole::Render()
{
	//if (FAILED(Bind_ShaderResources()))
	//{
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CBlackhole::Add_Components()
{
    return S_OK;
}

HRESULT CBlackhole::Bind_ShaderResources()
{
	//if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF.y", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
	//{
	//	return E_FAIL;
	//}

    return S_OK;
}

CBlackhole* CBlackhole::Create(_dev pDevice, _context pContext)
{
	CBlackhole* pInstance = new CBlackhole(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBlackhole");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlackhole::Clone(void* pArg)
{
	CBlackhole* pInstance = new CBlackhole(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBlackhole");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlackhole::Free()
{
	__super::Free();

	Safe_Release(m_pFrameEffect);
	Safe_Release(m_pBaseEffect);

	//Safe_Release(m_pRendererCom);
	//Safe_Release(m_pShaderCom);
}
