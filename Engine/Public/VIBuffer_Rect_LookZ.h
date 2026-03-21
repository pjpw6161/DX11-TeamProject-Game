#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_LookZ final :
    public CVIBuffer
{
private:
    CVIBuffer_Rect_LookZ(_dev pDevice, _context pContext);
    CVIBuffer_Rect_LookZ(const CVIBuffer_Rect_LookZ& rhs);
    virtual ~CVIBuffer_Rect_LookZ() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;

public:
	static CVIBuffer_Rect_LookZ* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END