#pragma once
#include "Base.h"
#include "GameInstance.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(_dev pDevice, _context pContext);
	virtual ~CLevel() = default;

public :
	virtual HRESULT Init();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };
	class CGameInstance* m_pGameInstance{ nullptr };

public:
	virtual void Free() override;
};

END