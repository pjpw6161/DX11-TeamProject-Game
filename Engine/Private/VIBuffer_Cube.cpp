#include "VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(_dev pDevice, _context pContext)
    : CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube& rhs)
    : CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Cube::Init_Prototype()
{
    m_iNumVertexBuffers = 1;
    m_iVertexStride = sizeof VTXCUBETEX;
    m_iNumVertices = 8;

    m_iIndexStride = 2;
    m_iNumIndices = 36;

    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region Vertex
    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXCUBETEX* pVertices = new VTXCUBETEX[m_iNumVertices];

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTexcoord = pVertices[0].vPosition;
	XMStoreFloat3(&pVertices[0].vNormal,XMVector3Normalize(XMLoadFloat3(&pVertices[0].vPosition)));

	pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vTexcoord = pVertices[1].vPosition;
	XMStoreFloat3(&pVertices[1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[1].vPosition)));

	pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTexcoord = pVertices[2].vPosition;
	XMStoreFloat3(&pVertices[2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[2].vPosition)));

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vTexcoord = pVertices[3].vPosition;
	XMStoreFloat3(&pVertices[3].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[3].vPosition)));

	// µÞ ¸é =====================================================
	pVertices[4].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[4].vTexcoord = pVertices[4].vPosition;
	XMStoreFloat3(&pVertices[4].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[4].vPosition)));

	pVertices[5].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[5].vTexcoord = pVertices[5].vPosition;
	XMStoreFloat3(&pVertices[5].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[5].vPosition)));

	pVertices[6].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[6].vTexcoord = pVertices[6].vPosition;
	XMStoreFloat3(&pVertices[6].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[6].vPosition)));

	pVertices[7].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[7].vTexcoord = pVertices[7].vPosition;
	XMStoreFloat3(&pVertices[7].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[7].vPosition)));

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
	{
		return E_FAIL;
	}
	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region Index
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];

	// ¾Õ¸é
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	// À­¸é		 
	pIndices[6] = 5;
	pIndices[7] = 4;
	pIndices[8] = 1;

	pIndices[9] = 5;
	pIndices[10] = 1;
	pIndices[11] = 0;

	// ¿ì¸é
	pIndices[12] = 1;
	pIndices[13] = 4;
	pIndices[14] = 7;

	pIndices[15] = 1;
	pIndices[16] = 7;
	pIndices[17] = 2;

	// ÁÂ¸é
	pIndices[18] = 5;
	pIndices[19] = 0;
	pIndices[20] = 3;

	pIndices[21] = 5;
	pIndices[22] = 3;
	pIndices[23] = 6;

	// ÇÏ¸é
	pIndices[24] = 3;
	pIndices[25] = 2;
	pIndices[26] = 7;

	pIndices[27] = 3;
	pIndices[28] = 7;
	pIndices[29] = 6;

	// ÈÄ¸é
	pIndices[30] = 4;
	pIndices[31] = 5;
	pIndices[32] = 6;

	pIndices[33] = 4;
	pIndices[34] = 6;
	pIndices[35] = 7;

	m_InitialData.pSysMem = pIndices;
	if (FAILED(__super::Create_Buffer(&m_pIB)))
	{
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);
#pragma endregion


    return S_OK;
}

HRESULT CVIBuffer_Cube::Init(void* pArg)
{
    return S_OK;
}

CVIBuffer_Cube* CVIBuffer_Cube::Create(_dev pDevice, _context pContext)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Cube::Clone(void* pArg)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();
}
