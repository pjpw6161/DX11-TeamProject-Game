#include "BrickBall.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"
#include "GameInstance.h"
#include "Collider.h"
#include "UI_Manager.h"
CBrickBall::CBrickBall(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBrickBall::CBrickBall(const CBrickBall& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBrickBall::Init_Prototype()
{
	return S_OK;
}

HRESULT CBrickBall::Init(void* pArg)
{

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}
	m_fSpeed = 6.f;
	m_pTransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));
	m_pTransformCom->Set_Speed(m_fSpeed);

	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
	vPlayerPos.y = 11.5f;
	_vec4 vPlayerLook = pPlayerTransform->Get_State(State::Look);
	vPlayerLook.y = 0.f;

	m_pTransformCom->LookAt_Dir(vPlayerLook);
	m_pTransformCom->Set_Position(vPlayerPos);

	m_EffectMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_Ball_Init");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

	m_shouldRenderBlur = true;
	m_eCurBrickColor = BLUE;
	return S_OK;
}

void CBrickBall::Tick(_float fTimeDelta)
{
	Set_BallColor();
	CUI_Manager::Get_Instance()->Set_BrickBallColor(m_eCurBrickColor);

	m_fX += fTimeDelta;
	if (m_pGameInstance->Key_Down(DIK_RETURN))
	{
		m_isDead = true;
	}

	_vec2 vCenterPos = _vec2(-2000.70496f, -1999.06152f);
	_vec2 vSize = _vec2(20.f, 20.f);
	RECT rcRect = {
		  (LONG)(vCenterPos.x - vSize.x * 0.5f),
		  (LONG)(vCenterPos.y - vSize.y * 0.5f),
		  (LONG)(vCenterPos.x + vSize.x * 0.5f),
		  (LONG)(vCenterPos.y + vSize.y * 0.5f)
	};
	POINT vCurPos = { (_long)m_pTransformCom->Get_State(State::Pos).x, (_long)m_pTransformCom->Get_State(State::Pos).z };
	if (!PtInRect(&rcRect, vCurPos))
	{
		m_isDead = true;
	}


	m_pTransformCom->Go_Straight(fTimeDelta);
	m_pTransformCom->Set_Speed(m_fSpeed);


	if (m_pGameInstance->Key_Down(DIK_UP, InputChannel::GamePlay))
	{
		m_iBallColor++;
		if (m_iBallColor > (_uint)COLOR_END)
		{
			m_iBallColor = 0;
		}
	}
	Check_Collision(fTimeDelta);

	m_EffectMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));


	/*
	if (m_pEffect_Ball)
	{
		m_pEffect_Ball->Tick(fTimeDelta);
	}
	*/
}

void CBrickBall::Late_Tick(_float fTimeDelta)
{

	m_pRendererCom->Add_RenderGroup(RG_Blend, this);
	//if (m_pEffect_Ball)
	//{
	//	m_pEffect_Ball->Late_Tick(fTimeDelta);
	//}

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CBrickBall::Render()
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

		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof _vec4)))
		{
			return E_FAIL;
		}

		_bool isBlur = true;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_isBlur", &isBlur, sizeof _bool)))
		{
			return E_FAIL;
		}

		_vec2 vUV = _vec2(m_fX, 0.f);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &vUV, sizeof _vec2)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(StaticPass_MaskEffect)))
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

HRESULT CBrickBall::Add_Collider()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.vCenter = _vec3(0.f);
	CollDesc.fRadius = 0.4f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Sphere"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

void CBrickBall::Update_Collider()
{
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}


void CBrickBall::RayCast()
{
	_float fDist = 0.51f;
	PxRaycastBuffer Buffer{};

	if (m_pGameInstance->Raycast(m_pTransformCom->Get_State(State::Pos), 
		m_pTransformCom->Get_State(State::Look).Get_Normalized(),
		fDist, Buffer))
	{
		_vec3 vNormal = _vec3(PxVec3ToVector(Buffer.block.normal));
		_vec3 vLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();
		vNormal.Normalize();
		m_vDir = _vec3::Reflect(vLook, vNormal);
		m_vDir.y = 0.f;
		m_pTransformCom->LookAt_Dir(m_vDir);


		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_Ball_Smoke");
		Info.pMatrix = &m_EffectMatrix;
		CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

	}

}

void CBrickBall::Check_Collision(_float fTimeDelta)
{

	Update_Collider();

	CCollider* pCollider{ nullptr };
	_bool isColl{};
	for (_uint i = 0; i < 4; i++)
	{
		pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Wall"), TEXT("Com_Collider_BrickWall"), i);
		if (pCollider == nullptr)
		{
			return;
		}
		isColl = m_pColliderCom->Intersect(pCollider);
		if (isColl)
		{
			break;
		}
	}

	if (isColl)
	{
		if (!m_isColl)
		{
			_vec3 vLook = m_pTransformCom->Get_State(State::Look);
			_vec3 vNormal = pCollider->Get_Normal(m_pGameInstance->Get_CollideFace(pCollider, m_pColliderCom));
			vNormal.Normalize();
			m_vDir = _vec3::Reflect(vLook, vNormal);
			m_vDir.y = 0.f;
			m_pTransformCom->LookAt_Dir(m_vDir);
		}
		m_isColl = true;
	}
	else
	{
		m_isColl = false;
	}

	CCollider* pBarCollider{ nullptr };
	_bool isBarColl{};

	pBarCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_VILLAGE, TEXT("Layer_BrickBar"), TEXT("Com_Collider_Bar"));
	if (pBarCollider == nullptr)
	{
		return;
	}
	isBarColl = m_pColliderCom->Intersect(pBarCollider);

	if (isBarColl)
	{
		_vec3 vLook = m_pTransformCom->Get_State(State::Look);
		_vec3 vNormal = pBarCollider->Get_Normal(m_pGameInstance->Get_CollideFace(pBarCollider, m_pColliderCom));
		/*
		
		if (vNormal.z > 0)
		{
			
			if (pBarCollider->Get_ColliderPos().x + pBarCollider->Get_Extents().x / 4.f < m_pColliderCom->Get_ColliderPos().x)
			{
				vNormal.x = 0.5f;
			}
			else if (pBarCollider->Get_ColliderPos().x - pBarCollider->Get_Extents().x / 4.f > m_pColliderCom->Get_ColliderPos().x)
			{
				vNormal.x = -0.5f;
			}
			
		}
		*/
		vNormal.Normalize();
		m_vDir = _vec3::Reflect(vLook, vNormal);
		m_vDir.y = 0.f;
		m_pTransformCom->LookAt_Dir(m_vDir);
		m_eCurBrickColor = (BrickColor)m_iBallColor;
	}


	CCollider* pBalloonCollider{ nullptr };
	_bool isBalloonColl{};
	_uint iNum = m_pGameInstance->Get_LayerSize(LEVEL_VILLAGE, TEXT("Layer_Balloons"));
	for (_uint i = 0; i < iNum; i++)
	{
		pBalloonCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_VILLAGE, TEXT("Layer_Balloons"), TEXT("Com_Collider_Sphere"), i);
		if (pBalloonCollider == nullptr)
		{
			return;
		}
		isBalloonColl = m_pColliderCom->Intersect(pBalloonCollider);
		if (isBalloonColl)
		{
			break;
		}
	}

	if (isBalloonColl)
	{
		if (!m_isBalloonColl)
		{
			_vec3 vLook = m_pTransformCom->Get_State(State::Look);
			_vec3 vNormal = pBalloonCollider->Get_Normal(m_pGameInstance->Get_CollideFace(pBalloonCollider, m_pColliderCom));
			vNormal.Normalize();
			m_vDir = _vec3::Reflect(vLook, vNormal);
			m_vDir.y = 0.f;
			m_pTransformCom->LookAt_Dir(m_vDir);
		}
		m_isBalloonColl = true;
	}
	else
	{
		m_isBalloonColl = false;
	}

}

void CBrickBall::Set_BallColor()
{

	switch (m_eCurBrickColor)
	{
	case PINK:
		m_vColor = _vec4(1.f, 0.56f, 0.93f, 1.f);
		break;
	case YELLOW:
		m_vColor = _vec4(0.94f, 0.77f, 0.2f, 1.f);
		break;
	case PURPLE:
		m_vColor = _vec4(0.63f, 0.4f, 0.9f, 1.f);
		break;
	case BLUE:
		m_vColor = _vec4(0.f, 0.6f, 1.f, 1.f);
		break;
	case COLOR_END:
		break;
	default:
		break;
	}
}

HRESULT CBrickBall::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh_Effect"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Effect_FX_A_SphereBuff002_SM.mo"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FX_A_Fractal006_Tex"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}
	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_Ball");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	m_pEffect_Ball = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
	
	return S_OK;
}

HRESULT CBrickBall::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CBrickBall* CBrickBall::Create(_dev pDevice, _context pContext)
{
	CBrickBall* pInstance = new CBrickBall(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBrickBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBrickBall::Clone(void* pArg)
{
	CBrickBall* pInstance = new CBrickBall(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBrickBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBrickBall::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pEffect_Ball);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
