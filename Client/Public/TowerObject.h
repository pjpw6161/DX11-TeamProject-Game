#pragma once
#include "Client_Define.h"
#include "Objects.h"


BEGIN(Client)

class CTowerObject final : public CObjects
{
private:
	CTowerObject(_dev pDevice, _context pContext);
	CTowerObject(const CTowerObject& rhs);
	virtual ~CTowerObject() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CTowerObject* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END