#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect final : public CVIBuffer
{
private:
	CVIBuffer_Rect(_dev pDevice, _context pContext);
	CVIBuffer_Rect(const CVIBuffer_Rect& rhs);
	virtual ~CVIBuffer_Rect() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;

public:
	static CVIBuffer_Rect* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END