#include "BrickWall.h"
#include "UI_Manager.h"
#include "Event_Manager.h"
#include "Effect_Manager.h"
#include "Camera_Manager.h"
#include "Effect_Dummy.h"

CBrickWall::CBrickWall(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBrickWall::CBrickWall(const CBrickWall& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBrickWall::Init_Prototype()
{
	return S_OK;
}

HRESULT CBrickWall::Init(void* pArg)
{
	//m_pTransformCom->Set_Position(vPos);

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	/*	
	//중심 DirectX::XMFLOAT4 = {x=-2000.70496 y=11.4677677 z=-1999.06152 ...}
	//왼쪽벽DirectX::XMFLOAT4 = {x=-1991.24585 y=11.4677677 z=-2000.09204 ...}
	//위쪽 벽 DirectX::XMFLOAT4 = {x=-2000.19641 y=11.4677696 z=-2007.11536 ...}
	*/
	
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	//_vec4 vCenterPos = vPlayerPos;
	_vec4 vCenterPos = _vec4(-2000.70496f, 11.4677677f, -1999.06152f, 1.f);
	_vec2 vSize = _vec2(18.f,18.f);
	m_rcRect = ((WALL_DESC*)pArg)->rcRect;
	
	if (m_rcRect.left == 1)
	{
		vCenterPos.x -= 6.f;
		m_pTransformCom->Set_Scale(_vec3(1.f, 30.f, 30.f));
	}
	else if (m_rcRect.right == 1)
	{
		vCenterPos.x += 6.f;
		m_pTransformCom->Set_Scale(_vec3(1.f, 30.f, 30.f));
	}
	else if (m_rcRect.top == 1)
	{
		vCenterPos.z -= 9.f;
		m_pTransformCom->Set_Scale(_vec3(1.f, 30.f, 30.f));
		m_pTransformCom->Rotation(_vec4(0.f, 1.f, 0.f, 0.f), 90.f);
	}
	else if (m_rcRect.bottom == 1)
	{
		vCenterPos.z += 9.f;
		m_pTransformCom->Set_Scale(_vec3(1.f, 30.f, 40.f));
		m_pTransformCom->Rotation(_vec4(0.f, 1.f, 0.f, 0.f), 90.f);
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, vCenterPos);
	m_EffectMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

	EffectInfo Info{};
	if (m_rcRect.left == 1 || m_rcRect.right == 1)
	{
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"BrickWall_ParticleRot");
	}
	else
	{
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"BrickWall_Particle");
	}
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	m_pEffect_Ball = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	m_shouldRenderBlur = true;


	m_pColliderCom->Set_Normal();
	return S_OK;
}

void CBrickWall::Tick(_float fTimeDelta)
{
	
	m_EffectMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));
	_vec3 vPos = m_pColliderCom->Get_ColliderPos();

	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
	
	if (m_fDissolveRatio >= 0.7f)
	{
		m_fDir = -1.f;
	}
	if (m_fDissolveRatio <= 0.5f)
	{
		m_fDir = 1.f;
	}
	m_fDissolveRatio += fTimeDelta * m_fDir * 0.05f;
	
	m_fDissolveRatio = 0.f;


	m_fX += fTimeDelta * 0.3f * m_fDir;

	if (m_pEffect_Ball)
	{
		m_pEffect_Ball->Tick(fTimeDelta);
	}
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());


	CCollider* pCollider{ nullptr };

	pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_VILLAGE, TEXT("Layer_BrickBall"), TEXT("Com_Collider_Sphere"));
	if (pCollider == nullptr)
	{
		return;
	}
	_bool isColl = m_pColliderCom->Intersect(pCollider);
	if (isColl)
	{
		m_isBlur = true;
	}
	else
	{
		m_isBlur = false;
	}

}

void CBrickWall::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RG_Blend, this);


	if (m_pEffect_Ball)
	{
		m_pEffect_Ball->Late_Tick(fTimeDelta);
	}

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CBrickWall::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			_bool bFailed = true;
		}

		if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
		{
			return E_FAIL;
		}
		
		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
		{
			return E_FAIL;
		}
		m_vColor = _vec4(0.8f, 0.5f, 0.98f, 1.f);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof _vec4)))
		{
			return E_FAIL;
		}
		
		_float fDissolveRatio = 0.5f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissolveRatio, sizeof _float)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_isBlur", &m_isBlur, sizeof _bool)))
		{
			return E_FAIL;
		}

		_float fAlpha = 0.1f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof _float)))
		{
			return E_FAIL;
		}
		_vec2 vUV = _vec2(m_fX, 0.f);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &vUV, sizeof _vec2)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(StaticPass_MaskDissolve)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

/*
void CBrickWall::Set_Normal()
{
	_vec3 vExtents = m_pColliderCom->Get_Extents();
	m_vNormals[FRONT] = _vec3(0.f, 0.f, 1.f) * vExtents.z;
	m_vNormals[BACK] = _vec3(0.f, 0.f, -1.f) * vExtents.z;
	m_vNormals[LEFT] = _vec3(-1.f, 0.f, 0.f) * vExtents.x;
	m_vNormals[RIGHT] = _vec3(1.f, 0.f, 0.f) * vExtents.x;

}
*/

HRESULT CBrickWall::Add_Collider()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::AABB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	if (m_rcRect.left == 1 || m_rcRect.right == 1)
	{
		CollDesc.vExtents = _vec3(0.2f, 0.2f, 0.6f);
	}
	else
	{
		CollDesc.vExtents = _vec3(0.6f, 0.2f, 0.2f);
	}

	CollDesc.vCenter = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_BrickWall"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}


void CBrickWall::Update_Collider()
{
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

HRESULT CBrickWall::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh_Effect"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Effect_FX_A_Plane005_SM.mo"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_T_EFF_Noise_04_BC"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
	{
		return E_FAIL;
	}
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FX_H_SmokeTrail001_Tex"), TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBrickWall::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CBrickWall* CBrickWall::Create(_dev pDevice, _context pContext)
{
	CBrickWall* pInstance = new CBrickWall(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBrickWall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBrickWall::Clone(void* pArg)
{
	CBrickWall* pInstance = new CBrickWall(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBrickWall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBrickWall::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pEffect_Ball);

	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pDissolveTextureCom);
}
