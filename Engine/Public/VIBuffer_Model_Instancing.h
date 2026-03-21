#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Model_Instancing final : public CVIBuffer
{
protected:
	CVIBuffer_Model_Instancing(_dev pDevice, _context pContext);
	CVIBuffer_Model_Instancing(const CVIBuffer_Model_Instancing& rhs);
	virtual ~CVIBuffer_Model_Instancing() = default;

public:
	virtual HRESULT Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix, _uint iNumInstance);
	virtual HRESULT Init(void* pArg) override;
	virtual void Update(_float fTimeDelta, _mat WorldMatrix);
	virtual HRESULT Render() override;


protected:
	D3D11_BUFFER_DESC m_InstancingBufferDesc{};
	D3D11_SUBRESOURCE_DATA m_InstancingInitialData{};

	ID3D11Buffer* m_pVBInstance{ nullptr };

	_uint m_iNumInstances{};
	_uint m_iIndexCountPerInstance{};
	_uint m_iInstanceStride{};

	static CVIBuffer_Model_Instancing* Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END