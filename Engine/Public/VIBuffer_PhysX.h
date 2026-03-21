#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_PhysX final : public CVIBuffer
{
private:
	CVIBuffer_PhysX(_dev pDevice, _context pContext);
	CVIBuffer_PhysX(const CVIBuffer_PhysX& rhs);
	virtual ~CVIBuffer_PhysX() = default;

public:
	virtual HRESULT Init_Prototype(const PxRenderBuffer& Buffer);
	virtual HRESULT Init(void* pArg) override;

public:
	virtual HRESULT Update_Buffer(const PxRenderBuffer& Buffer);

public:
	static CVIBuffer_PhysX* Create(_dev pDevice, _context pContext, const PxRenderBuffer& Buffer);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END