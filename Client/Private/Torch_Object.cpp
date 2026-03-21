#pragma once
#include "Torch_Object.h"
#include "Camera_Manager.h"
#include "Effect_Manager.h"
#include "Effect_Dummy.h"

CTorch_Object::CTorch_Object(_dev pDevice, _context pContext)
	: CObjects(pDevice, pContext)
{
}

CTorch_Object::CTorch_Object(const CTorch_Object& rhs)
	: CObjects(rhs)
{
	m_isInstancing = true;
}


HRESULT CTorch_Object::Init_Prototype()
{
	return S_OK;
}

HRESULT CTorch_Object::Init(void* pArg)
{
	m_Info = *(ObjectInfo*)pArg;
	strPrototype = m_Info.strPrototypeTag;

	if (FAILED(__super::Add_Components(strPrototype, m_Info.eObjectType)))
	{
		return E_FAIL;
	}

	m_iShaderPass = 0;

	_vec4 vRight = _vec4(m_Info.m_WorldMatrix._11, m_Info.m_WorldMatrix._12, m_Info.m_WorldMatrix._13, m_Info.m_WorldMatrix._14);
	_vec4 vUp = _vec4(m_Info.m_WorldMatrix._21, m_Info.m_WorldMatrix._22, m_Info.m_WorldMatrix._23, m_Info.m_WorldMatrix._24);
	_vec4 vLook = _vec4(m_Info.m_WorldMatrix._31, m_Info.m_WorldMatrix._32, m_Info.m_WorldMatrix._33, m_Info.m_WorldMatrix._34);
	_vec4 vPos = _vec4(m_Info.m_WorldMatrix._41, m_Info.m_WorldMatrix._42, m_Info.m_WorldMatrix._43, m_Info.m_WorldMatrix._44);

	m_pTransformCom->Set_State(State::Right, vRight);
	m_pTransformCom->Set_State(State::Up, vUp);
	m_pTransformCom->Set_State(State::Look, vLook);
	m_pTransformCom->Set_State(State::Pos, vPos);

	m_pModelCom->Apply_TransformToActor(m_Info.m_WorldMatrix);
	
	EffectInfo Effect = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"TorchFire_Dun");

	_vec3 vFire_Hight{};
	if (strPrototype == L"Prototype_Model_Wall_Torch")
	{
		vFire_Hight = _vec3(0.f, 1.5f, 0.f);
	}
	else if (strPrototype == L"Prototype_Model_Stand_Torch1")
	{
		vFire_Hight = _vec3(0.f, 2.f, 0.f);
	}
	else if (strPrototype == L"Prototype_Model_Stand_Torch2")
	{
		vFire_Hight = _vec3(0.f, 1.f, 0.f);
		Effect = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"TorchFire_Boss");
	}
	else if (strPrototype == L"Prototype_Model_Torch")
	{
		vFire_Hight = _vec3(0.f, 2.f, 0.f);
	}
	else if (strPrototype == L"Prototype_Model_Brazier")
	{
		vFire_Hight = _vec3(0.f, 0.5f, 0.f);
	}

	//_mat FireMat{};
	m_FireMat.Position(vPos + vFire_Hight);
	Effect.pMatrix = &m_FireMat;
	Effect.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Effect);

	return S_OK;
}

void CTorch_Object::Tick(_float fTimeDelta)
{
	_float fScale{};
	_vec3 vFire_Hight{};
	if (strPrototype == L"Prototype_Model_Wall_Torch")
	{
		vFire_Hight = _vec3(0.f, 1.2f, 0.f);
		fScale = 1.7f;
	}
	else if (strPrototype == L"Prototype_Model_Stand_Torch1")
	{
		vFire_Hight = _vec3(0.f, 1.5f, 0.f);
		fScale = 2.2f;
	}
	else if (strPrototype == L"Prototype_Model_Stand_Torch2")
	{
		vFire_Hight = _vec3(0.f, 3.7f, 0.f);
		fScale = 6.2f;
	}
	else if (strPrototype == L"Prototype_Model_Torch")
	{
		vFire_Hight = _vec3(0.f, 1.3f, 0.f);
		fScale = 2.f;
	}
	else if (strPrototype == L"Prototype_Model_Brazier")
	{
		vFire_Hight = _vec3(0.f, 0.4f, 0.f);
		fScale = 2.2f;
	}

	m_FireMat = _mat::CreateScale(fScale);
	m_FireMat.Position(m_pTransformCom->Get_State(State::Pos) + vFire_Hight);
}

void CTorch_Object::Late_Tick(_float fTimeDelta)
{
	CAMERA_STATE CamState = CCamera_Manager::Get_Instance()->Get_CameraState();
	if (CamState == CS_SKILLBOOK or CamState == CS_INVEN)// or CamState == CS_WORLDMAP)
	{
		return;
	}
	__super::Late_Tick(fTimeDelta);
}

HRESULT CTorch_Object::Render()
{

	__super::Render();

	return S_OK;
}


CTorch_Object* CTorch_Object::Create(_dev pDevice, _context pContext)
{
	CTorch_Object* pInstance = new CTorch_Object(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTorch_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTorch_Object::Clone(void* pArg)
{
	CTorch_Object* pInstance = new CTorch_Object(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTorch_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTorch_Object::Free()
{
	__super::Free();
}
