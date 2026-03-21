#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(_dev pDevice, _context pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);
	virtual HRESULT Render();

protected:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };
	class CGameInstance* m_pGameInstance { nullptr };
	_bool m_hasCloned{ false };

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END