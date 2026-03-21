#pragma once
#include "TowerObject.h"

CTowerObject::CTowerObject(_dev pDevice, _context pContext)
	: CObjects(pDevice, pContext)
{
}

CTowerObject::CTowerObject(const CTowerObject& rhs)
	: CObjects(rhs)
{
	m_isInstancing = false;
}


HRESULT CTowerObject::Init_Prototype()
{
	return S_OK;
}

HRESULT CTowerObject::Init(void* pArg)
{
	m_Info = *(ObjectInfo*)pArg;
	wstring strPrototype = m_Info.strPrototypeTag;

	if (FAILED(__super::Add_Components(strPrototype, m_Info.eObjectType)))
	{
		return E_FAIL;
	}

	m_iShaderPass = StaticPass_Default;

	m_pTransformCom->Set_Matrix(m_Info.m_WorldMatrix);
	m_pModelCom->Apply_TransformToActor(m_Info.m_WorldMatrix);

	return S_OK;
}

void CTowerObject::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CTowerObject::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CTowerObject::Render()
{

	__super::Render();

	return S_OK;
}


CTowerObject* CTowerObject::Create(_dev pDevice, _context pContext)
{
	CTowerObject* pInstance = new CTowerObject(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTowerObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTowerObject::Clone(void* pArg)
{
	CTowerObject* pInstance = new CTowerObject(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTowerObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTowerObject::Free()
{
	__super::Free();
}
