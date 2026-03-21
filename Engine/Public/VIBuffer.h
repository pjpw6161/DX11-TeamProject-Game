#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(_dev pDevice, _context pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual HRESULT Render() override;
	ID3D11Buffer* Get_VB()
	{
		return m_pVB;
	}
	ID3D11Buffer* Get_IB()
	{
		return m_pIB;
	}

	_float3* Get_Vertices(void){ return m_pVertices;}
	_uint Get_NumVertexBuffer() { return m_iNumVertexBuffers; }
	_uint Get_Stride() { return m_iVertexStride; }
	_uint Get_NumPrimitives() { return m_iNumPrimitives; }
	_uint Get_NumIndices() { return m_iNumIndices; }
	_uint Get_NumVertices() { return m_iNumVertices; }
	_uint Get_IndexSizePrimitives() { return m_iIndexSizePrimitives; }
	_uint Get_NumIndicesPrimitives() { return m_iNumIndicesPrimitives; }
	DXGI_FORMAT Get_IndexFormat() { return m_eIndexFormat; }
	vector<VTXNORTEX> Get_TerrainVertices() { return m_vVertices; }
	vector<VTXSTATICMESH> Get_MeshVertices() { return m_vMeshVertices; }
	vector<_ulong> Get_TerrainIndices() { return m_vIndices; }
	
	D3D11_PRIMITIVE_TOPOLOGY Get_PrimitiveTopology() { return m_ePrimitiveTopology; }

protected:
	ID3D11Buffer* m_pVB{ nullptr };
	ID3D11Buffer* m_pIB{ nullptr };

	_uint m_iNumVertexBuffers{};

	_uint m_iVertexStride{};
	_uint m_iNumVertices{};

	_uint m_iIndexStride{};
	_uint m_iNumIndices{};

	DXGI_FORMAT m_eIndexFormat{};
	D3D11_PRIMITIVE_TOPOLOGY m_ePrimitiveTopology{};

	D3D11_BUFFER_DESC m_BufferDesc{};
	D3D11_SUBRESOURCE_DATA m_InitialData{};

	_float3* m_pVertices {};
	vector<VTXNORTEX> m_vVertices;
	vector<VTXSTATICMESH> m_vMeshVertices;
	vector<_ulong> m_vIndices;

	_uint m_iNumPrimitives{};
	_uint m_iIndexSizePrimitives{};
	_uint m_iNumIndicesPrimitives{};



protected:
	HRESULT Create_Buffer(ID3D11Buffer** ppBuffer);

public:
	virtual void Free() override;
};

END