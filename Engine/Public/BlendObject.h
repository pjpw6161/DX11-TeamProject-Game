#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CBlendObject abstract : public CGameObject
{
protected:
	CBlendObject(_dev pDevice, _context pContext);
	CBlendObject(const CBlendObject& rhs);
	virtual ~CBlendObject() = default;

public:
	const _float& Get_CamDistance() const;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	void Compute_CamDistance();

protected:
	_float m_fCamDistance{};

public:
	virtual CGameObject* Clone(void*) = 0;
	virtual void Free() override;
};

END