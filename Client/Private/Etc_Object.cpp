#pragma once
#include "Etc_Object.h"

CEtc_Object::CEtc_Object(_dev pDevice, _context pContext)
	: CObjects(pDevice, pContext)
{
}

CEtc_Object::CEtc_Object(const CEtc_Object& rhs)
	: CObjects(rhs)
{
	m_isInstancing = false;
}


HRESULT CEtc_Object::Init_Prototype()
{
	return S_OK;
}

HRESULT CEtc_Object::Init(void* pArg)
{
	m_Info = *(ObjectInfo*)pArg;
	wstring strPrototype = m_Info.strPrototypeTag;

	if (FAILED(__super::Add_Components(strPrototype, m_Info.eObjectType)))
	{
		return E_FAIL;
	}

	m_iShaderPass = StaticPass_AlphaTestMeshes;

	m_pTransformCom->Set_Matrix(m_Info.m_WorldMatrix);
	m_pModelCom->Apply_TransformToActor(m_Info.m_WorldMatrix);
	
	return S_OK;
}

void CEtc_Object::Tick(_float fTimeDelta)
{

	__super::Tick(fTimeDelta);
}

void CEtc_Object::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CEtc_Object::Render()
{

	__super::Render();

	return S_OK;
}


CEtc_Object* CEtc_Object::Create(_dev pDevice, _context pContext)
{
	CEtc_Object* pInstance = new CEtc_Object(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CEtc_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEtc_Object::Clone(void* pArg)
{
	CEtc_Object* pInstance = new CEtc_Object(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CEtc_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEtc_Object::Free()
{
	__super::Free();
}
