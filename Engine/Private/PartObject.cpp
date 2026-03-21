#include "PartObject.h"

CPartObject::CPartObject(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CPartObject::CPartObject(const CPartObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPartObject::Init(void* pArg)
{
	if (!pArg)
	{
		return E_FAIL;
	}

	m_pParentTransform = reinterpret_cast<CTransform*>(pArg);
	Safe_AddRef(m_pParentTransform);

	return S_OK;
}

void CPartObject::Free()
{
	__super::Free();

	Safe_Release(m_pParentTransform);
}
