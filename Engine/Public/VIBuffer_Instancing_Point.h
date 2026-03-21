#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing_Point final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Instancing_Point(_dev pDevice, _context pContext);
	CVIBuffer_Instancing_Point(const CVIBuffer_Instancing_Point& rhs);
	virtual ~CVIBuffer_Instancing_Point() = default;

public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg) override;

public:
	static CVIBuffer_Instancing_Point* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END