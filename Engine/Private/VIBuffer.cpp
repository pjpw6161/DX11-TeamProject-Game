#include "VIBuffer.h"

CVIBuffer::CVIBuffer(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_iNumVertexBuffers(rhs.m_iNumVertexBuffers)
	, m_iIndexSizePrimitives(rhs.m_iIndexSizePrimitives)
	, m_iNumIndicesPrimitives(rhs.m_iNumIndicesPrimitives)
	, m_iNumPrimitives(rhs.m_iNumPrimitives)
	, m_iVertexStride(rhs.m_iVertexStride)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iIndexStride(rhs.m_iIndexStride)
	, m_iNumIndices(rhs.m_iNumIndices)
	, m_eIndexFormat(rhs.m_eIndexFormat)
	, m_ePrimitiveTopology(rhs.m_ePrimitiveTopology)
	, m_pVertices(rhs.m_pVertices)
	, m_vVertices(rhs.m_vVertices)
	, m_vIndices(rhs.m_vIndices)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Init_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Init(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	ID3D11Buffer* pVertexBuffers[] = 
	{
		m_pVB,
	};

	_uint pVertexStrides[] =
	{
		m_iVertexStride,
	};

	_uint pOffsets[] =
	{
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, pVertexStrides, pOffsets);

	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer** ppBuffer)
{
	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_InitialData, ppBuffer);
}

void CVIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
}
