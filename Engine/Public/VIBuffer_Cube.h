#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cube final : public CVIBuffer
{
private:
	CVIBuffer_Cube(_dev pDevice, _context pContext);
	CVIBuffer_Cube(const CVIBuffer_Cube& rhs);
	virtual ~CVIBuffer_Cube() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;

public:
	static CVIBuffer_Cube* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END