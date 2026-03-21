#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain_Dynamic final : public CVIBuffer
{
public:
	typedef struct tagVertexSize {
		_uint2 iSize{};
		_uint2 iC{};
	}VERTEX_DESC;
private:
	CVIBuffer_Terrain_Dynamic(_dev pDevice, _context pContext);
	CVIBuffer_Terrain_Dynamic(const CVIBuffer_Terrain_Dynamic& rhs);
	virtual ~CVIBuffer_Terrain_Dynamic() = default;

public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg) override;

public:
	static CVIBuffer_Terrain_Dynamic* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END