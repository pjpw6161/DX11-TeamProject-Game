#include "BrickBar.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"
#include "GameInstance.h"
#include "Collider.h"
CBrickBar::CBrickBar(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBrickBar::CBrickBar(const CBrickBar& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBrickBar::Init_Prototype()
{
	return S_OK;
}

HRESULT CBrickBar::Init(void* pArg)
{

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}
	m_fSpeed = 5.f;
	m_pTransformCom->Set_Scale(_vec3(8.f, 8.f, 8.f));
	m_pTransformCom->Set_Speed(m_fSpeed);

	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
	vPlayerPos.y += 1.f; 
	_vec4 vCenterPos = _vec4(-2000.70496f, 11.4677677f, -1992.06152f, 1.f);
	_vec4 vPlayerLook = pPlayerTransform->Get_State(State::Look);
	vPlayerLook.y = 0.f;

	//m_pTransformCom->LookAt_Dir(vPlayerLook);
	m_pTransformCom->Set_State(State::Pos, vCenterPos);

	m_EffectMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"BrickArrow");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

	m_pColliderCom->Set_Normal();

	return S_OK;
}

void CBrickBar::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_LEFT))
	{
		_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
		vPos.x += fTimeDelta * 5.f; 
		m_pTransformCom->Set_State(State::Pos, vPos);
	}
	if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
		vPos.x -= fTimeDelta * 5.f; 
		m_pTransformCom->Set_State(State::Pos, vPos);
	}


	
	m_EffectMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

	Update_Collider();

	if (m_pEffect_Ball)
	{
		m_pEffect_Ball->Tick(fTimeDelta);
	}

	

	return;


}

void CBrickBar::Late_Tick(_float fTimeDelta)
{
	//m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
	/*
	if (m_pEffect_Ball)
	{
		m_pEffect_Ball->Late_Tick(fTimeDelta);
	}
	*/

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CBrickBar::Render()
{
	return S_OK;
}

HRESULT CBrickBar::Add_Collider()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::AABB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(0.1f, 0.2f, 0.05f);
	CollDesc.vCenter = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Bar"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

void CBrickBar::Update_Collider()
{
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}



HRESULT CBrickBar::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_Ball");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	m_pEffect_Ball = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
	
	return S_OK;
}

HRESULT CBrickBar::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CBrickBar* CBrickBar::Create(_dev pDevice, _context pContext)
{
	CBrickBar* pInstance = new CBrickBar(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBrickBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBrickBar::Clone(void* pArg)
{
	CBrickBar* pInstance = new CBrickBar(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBrickBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBrickBar::Free()
{
	__super::Free();

	Safe_Release(m_pEffect_Ball);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
