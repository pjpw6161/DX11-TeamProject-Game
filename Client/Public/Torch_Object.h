#pragma once
#include "Client_Define.h"
#include "Objects.h"
/*
	Prototype_Model_Wall_Torch
	Prototype_Model_Stand_Torch1
	Prototype_Model_Stand_Torch2
	Prototype_Model_Torch
	Prototype_Model_Brazier
*/
BEGIN(Client)

class CTorch_Object final : public CObjects
{
private:
	CTorch_Object(_dev pDevice, _context pContext);
	CTorch_Object(const CTorch_Object& rhs);
	virtual ~CTorch_Object() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_mat m_FireMat{};
	wstring strPrototype{};

public:
	static CTorch_Object* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END