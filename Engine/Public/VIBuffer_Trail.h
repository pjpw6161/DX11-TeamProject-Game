#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
private:
	CVIBuffer_Trail(_dev pDevice, _context pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT Init_Prototype(const _uint iNumVertices, _float2 vPSize);
	virtual HRESULT Init(void* pArg) override;
	void Update(_uint iNumVerticesToUse, _vec3* pPositions, _vec4* pColors = nullptr, _vec2* pPSize = nullptr);

private:
	D3D11_BUFFER_DESC			m_TrailBufferDesc = {};
	D3D11_SUBRESOURCE_DATA		m_TrailInitialData = {};

	_float2 m_vPSize{};
	_uint m_iDetailRatio{};

public:
	static CVIBuffer_Trail* Create(_dev pDevice, _context pContext, _uint iNumVertices, _float2 vPSize);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END