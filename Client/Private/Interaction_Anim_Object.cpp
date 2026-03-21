#pragma once
#include "Interaction_Anim_Object.h"
#include "Camera_Manager.h"
#include "NameTag.h"
#include "UI_Manager.h"

// 부셔지는 애니메이션 하나 밖에 없음
// Prototype_Model_GoldStone
// Prototype_Model_SaltStone

CInteraction_Anim::CInteraction_Anim(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CInteraction_Anim::CInteraction_Anim(const CInteraction_Anim& rhs)
	: CGameObject(rhs)
	, m_pTrigger_Manager(CTrigger_Manager::Get_Instance())

{
	Safe_AddRef(m_pTrigger_Manager);
}


HRESULT CInteraction_Anim::Init_Prototype()
{
	return S_OK;
}

HRESULT CInteraction_Anim::Init(void* pArg)
{
	m_Info = *(ObjectInfo*)pArg;
	m_ePlaceType = (PlaceType)m_Info.m_iIndex;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	_vec4 vRight = _vec4(m_Info.m_WorldMatrix._11, m_Info.m_WorldMatrix._12, m_Info.m_WorldMatrix._13, m_Info.m_WorldMatrix._14);
	_vec4 vUp = _vec4(m_Info.m_WorldMatrix._21, m_Info.m_WorldMatrix._22, m_Info.m_WorldMatrix._23, m_Info.m_WorldMatrix._24);
	_vec4 vLook = _vec4(m_Info.m_WorldMatrix._31, m_Info.m_WorldMatrix._32, m_Info.m_WorldMatrix._33, m_Info.m_WorldMatrix._34);
	_vec4 vPos = _vec4(m_Info.m_WorldMatrix._41, m_Info.m_WorldMatrix._42, m_Info.m_WorldMatrix._43, 1.f);

	m_pTransformCom->Set_State(State::Right, vRight);
	m_pTransformCom->Set_State(State::Up, vUp);
	m_pTransformCom->Set_State(State::Look, vLook);
	m_pTransformCom->Set_State(State::Pos, vPos);

	m_Animation.iAnimIndex = 0;
	m_Animation.fAnimSpeedRatio = 1.5f;
	m_Animation.isLoop = false;

	//m_pModelCom->Apply_TransformToActor(m_Info.m_WorldMatrix);

	CNameTag::NAMETAG_DESC NameTagDesc = {};
	NameTagDesc.eLevelID = LEVEL_STATIC;
	NameTagDesc.fFontSize = 0.34f;
	NameTagDesc.pParentTransform = m_pTransformCom;
	if (m_Info.strPrototypeTag == TEXT("Prototype_Model_GoldStone"))
	{
		NameTagDesc.strNameTag = TEXT("금광석");
	}
	else
	{
		NameTagDesc.strNameTag = TEXT("소금 광석");
	}

	NameTagDesc.vColor = _vec4(0.f, 0.6f, 0.8f, 1.f);
	NameTagDesc.vTextPosition = _vec2(0.f, 2.5f);

	m_pNameTag = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NameTag"), &NameTagDesc);
	if (not m_pNameTag)
	{
		return E_FAIL;
	}

	return S_OK;
}

void CInteraction_Anim::Tick(_float fTimeDelta)
{
	if (m_isCollect)
	{
		m_fCollectTime += fTimeDelta;
		if (m_fCollectTime >= 3.f)
		{
			m_isDead = true;
		}
	}
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
	m_pWideColliderCom->Update(m_pTransformCom->Get_World_Matrix());

	CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Player_Hit_OBB"));
	m_isCollision = m_pColliderCom->Intersect(pCollider);
	m_isWideCollision = m_pWideColliderCom->Intersect(pCollider);
	if (m_isCollision)
	{
		if (!m_isCollect && m_pGameInstance->Key_Down(DIK_E))
		{
			CUI_Manager::Get_Instance()->Set_Collect();
			m_isCollect = true;
		}
	}
	if (m_isWideCollision)
	{
		dynamic_cast<CNameTag*>(m_pNameTag)->Tick(fTimeDelta);
	}

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pWideColliderCom);
#endif
}

void CInteraction_Anim::Late_Tick(_float fTimeDelta)
{
	CAMERA_STATE CamState = CCamera_Manager::Get_Instance()->Get_CameraState();
	if (CamState == CS_SKILLBOOK or CamState == CS_INVEN)// or CamState == CS_WORLDMAP)
	{
		return;
	}

	if (m_isCollision)
	{
		m_pModelCom->Play_Animation(fTimeDelta);
	}

	if (m_isWideCollision)
	{
		m_pNameTag->Late_Tick(fTimeDelta);
	}
	m_pRendererCom->Add_RenderGroup(RG_AnimNonBlend_Instance, this);
}

HRESULT CInteraction_Anim::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom->Bind_Animation(m_pShaderCom)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom->Bind_PlayAnimation(m_pShaderCom)))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			_bool bFailed = true;
		}

		_bool HasNorTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		if (FAILED(m_pShaderCom->Begin(0)))
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


HRESULT CInteraction_Anim::Render_Instance()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CInteraction_Anim::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VTF_Instance"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	m_pShaderCom->Set_PassIndex(0);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_Info.strPrototypeTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	/* For.Com_Collider_SPHERE */
// Com_Collider
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.fRadius = 5.f;
	CollDesc.vCenter = _vec3(0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Interaction_Sphere"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;
	
	Collider_Desc ColDesc2{};
	ColDesc2.eType = ColliderType::Sphere;
	ColDesc2.fRadius = 27.f;
	ColDesc2.vCenter = _vec3(0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Interaction_Sphere1"), (CComponent**)&m_pWideColliderCom, &ColDesc2)))
		return E_FAIL;


	return S_OK;
}

HRESULT CInteraction_Anim::Bind_ShaderResources()
{
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

	if (true == m_pGameInstance->Get_TurnOnShadow()) {

		CASCADE_DESC Desc = m_pGameInstance->Get_CascadeDesc();

		if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeView", Desc.LightView, 3)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeProj", Desc.LightProj, 3)))
			return E_FAIL;

	}

	return S_OK;
}



CInteraction_Anim* CInteraction_Anim::Create(_dev pDevice, _context pContext)
{
	CInteraction_Anim* pInstance = new CInteraction_Anim(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CInteraction_Anim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInteraction_Anim::Clone(void* pArg)
{
	CInteraction_Anim* pInstance = new CInteraction_Anim(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CInteraction_Anim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInteraction_Anim::Free()
{
	__super::Free();
	Safe_Release(m_pNameTag);

	Safe_Release(m_pTrigger_Manager);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pWideColliderCom);
}
