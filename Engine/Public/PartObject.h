#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
protected:
	CPartObject(_dev pDevice, _context pContext);
	CPartObject(const CPartObject& rhs);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Init(void* pArg) override;

protected:
	CTransform* m_pParentTransform{ nullptr };
	_float44 m_Worldmatrix{};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END