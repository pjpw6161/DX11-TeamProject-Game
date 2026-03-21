#include "VIBuffer_Rect_LookZ.h"

CVIBuffer_Rect_LookZ::CVIBuffer_Rect_LookZ(_dev pDevice, _context pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Rect_LookZ::CVIBuffer_Rect_LookZ(const CVIBuffer_Rect_LookZ& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Rect_LookZ::Init_Prototype()
{
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof VTXNORTEX;
	m_iNumVertices = 4;

	m_iIndexStride = 2;
	m_iNumIndices = 6;

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region Vertex
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[0].vNormal = _float3(0.f, 0.f, 1.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[1].vNormal = _float3(0.f, 0.f, 1.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[2].vNormal = _float3(0.f, 0.f, 1.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[3].vNormal = _float3(0.f, 0.f, 1.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
	{
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region Index
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
	{
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect_LookZ::Init(void* pArg)
{
	return S_OK;
}

CVIBuffer_Rect_LookZ* CVIBuffer_Rect_LookZ::Create(_dev pDevice, _context pContext)
{
	CVIBuffer_Rect_LookZ* pInstance = new CVIBuffer_Rect_LookZ(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Rect_LookZ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Rect_LookZ::Clone(void* pArg)
{
	CVIBuffer_Rect_LookZ* pInstance = new CVIBuffer_Rect_LookZ(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_Rect_LookZ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_LookZ::Free()
{
	__super::Free();
}
