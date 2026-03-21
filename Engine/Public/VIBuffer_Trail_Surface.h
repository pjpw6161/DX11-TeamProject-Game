#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail_Surface final : public CVIBuffer
{
private:
	CVIBuffer_Trail_Surface(_dev pDevice, _context pContext);
	CVIBuffer_Trail_Surface(const CVIBuffer_Trail_Surface& rhs);
	virtual ~CVIBuffer_Trail_Surface() = default;

public:
	virtual HRESULT Init_Prototype(const _uint iNumVertices);
	virtual HRESULT Init(void* pArg) override;
	void Update(_uint iNumVerticesToUse, _vec3* pTopPositionArray, _vec3* pBottomPositionArray, _float* pAlphaArray = nullptr);

private:
	D3D11_BUFFER_DESC			m_TrailBufferDesc = {};
	D3D11_SUBRESOURCE_DATA		m_TrailInitialData = {};

	_uint m_iDetailRatio{};

public:
	static CVIBuffer_Trail_Surface* Create(_dev pDevice, _context pContext, _uint iNumVertices);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END