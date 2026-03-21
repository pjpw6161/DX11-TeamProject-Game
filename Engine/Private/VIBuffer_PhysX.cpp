#include "VIBuffer_PhysX.h"

CVIBuffer_PhysX::CVIBuffer_PhysX(_dev pDevice, _context pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_PhysX::CVIBuffer_PhysX(const CVIBuffer_PhysX& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_PhysX::Init_Prototype(const PxRenderBuffer& Buffer)
{
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof VTXPOSCOLOR;
	m_iNumVertices = Buffer.getNbTriangles() * 3;

	m_iIndexStride = 2;
	m_iNumIndices = m_iNumVertices;

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region Vertex
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	VTXPOSCOLOR* pVertices = new VTXPOSCOLOR[m_iNumVertices]{};
	_uint iIndex{};
	for (size_t i = 0; i < Buffer.getNbTriangles(); i++)
	{
		pVertices[iIndex].vPosition.x = Buffer.getTriangles()[i].pos0.x;
		pVertices[iIndex].vPosition.y = Buffer.getTriangles()[i].pos0.y;
		pVertices[iIndex].vPosition.z = Buffer.getTriangles()[i].pos0.z;
		
		pVertices[iIndex].vColor.x = (Buffer.getTriangles()[i].color0 & 0xff) / 255.f;
		pVertices[iIndex].vColor.y = ((Buffer.getTriangles()[i].color0 >> 8) & 0xff) / 255.f;
		pVertices[iIndex].vColor.z = ((Buffer.getTriangles()[i].color0 >> 16) & 0xff) / 255.f;
		
		iIndex++;
		pVertices[iIndex].vPosition.x = Buffer.getTriangles()[i].pos1.x;
		pVertices[iIndex].vPosition.y = Buffer.getTriangles()[i].pos1.y;
		pVertices[iIndex].vPosition.z = Buffer.getTriangles()[i].pos1.z;
		
		pVertices[iIndex].vColor.x = (Buffer.getTriangles()[i].color1 & 0xff) / 255.f;
		pVertices[iIndex].vColor.y = ((Buffer.getTriangles()[i].color1 >> 8) & 0xff) / 255.f;
		pVertices[iIndex].vColor.z = ((Buffer.getTriangles()[i].color1 >> 16) & 0xff) / 255.f;

		iIndex++;
		pVertices[iIndex].vPosition.x = Buffer.getTriangles()[i].pos1.x;
		pVertices[iIndex].vPosition.y = Buffer.getTriangles()[i].pos1.y;
		pVertices[iIndex].vPosition.z = Buffer.getTriangles()[i].pos1.z;
		
		pVertices[iIndex].vColor.x = (Buffer.getTriangles()[i].color1 & 0xff) / 255.f;
		pVertices[iIndex].vColor.y = ((Buffer.getTriangles()[i].color1 >> 8) & 0xff) / 255.f;
		pVertices[iIndex].vColor.z = ((Buffer.getTriangles()[i].color1 >> 16) & 0xff) / 255.f;

		iIndex++;
	}

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
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	_ushort* pIndices = new _ushort[m_iNumIndices]{};

	for (_ushort i = 0; i < m_iNumIndices; i++)
	{
		pIndices[i] = i;
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
	{
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_PhysX::Init(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_PhysX::Update_Buffer(const PxRenderBuffer& Buffer)
{
	D3D11_MAPPED_SUBRESOURCE MappedResource{};
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	m_iNumVertices = Buffer.getNbTriangles() * 3;

	VTXPOSCOLOR* pVertices = new VTXPOSCOLOR[m_iNumVertices]{};
	_uint iIndex{};
	for (size_t i = 0; i < Buffer.getNbTriangles(); i++)
	{
		pVertices[iIndex].vPosition.x = Buffer.getTriangles()[i].pos0.x;
		pVertices[iIndex].vPosition.y = Buffer.getTriangles()[i].pos0.y;
		pVertices[iIndex].vPosition.z = Buffer.getTriangles()[i].pos0.z;

		pVertices[iIndex].vColor.x = (Buffer.getTriangles()[i].color0 & 0xff) / 255.f;
		pVertices[iIndex].vColor.y = ((Buffer.getTriangles()[i].color0 >> 8) & 0xff) / 255.f;
		pVertices[iIndex].vColor.z = ((Buffer.getTriangles()[i].color0 >> 16) & 0xff) / 255.f;

		iIndex++;
		pVertices[iIndex].vPosition.x = Buffer.getTriangles()[i].pos1.x;
		pVertices[iIndex].vPosition.y = Buffer.getTriangles()[i].pos1.y;
		pVertices[iIndex].vPosition.z = Buffer.getTriangles()[i].pos1.z;

		pVertices[iIndex].vColor.x = (Buffer.getTriangles()[i].color1 & 0xff) / 255.f;
		pVertices[iIndex].vColor.y = ((Buffer.getTriangles()[i].color1 >> 8) & 0xff) / 255.f;
		pVertices[iIndex].vColor.z = ((Buffer.getTriangles()[i].color1 >> 16) & 0xff) / 255.f;

		iIndex++;
		pVertices[iIndex].vPosition.x = Buffer.getTriangles()[i].pos2.x;
		pVertices[iIndex].vPosition.y = Buffer.getTriangles()[i].pos2.y;
		pVertices[iIndex].vPosition.z = Buffer.getTriangles()[i].pos2.z;

		pVertices[iIndex].vColor.x = (Buffer.getTriangles()[i].color2 & 0xff) / 255.f;
		pVertices[iIndex].vColor.y = ((Buffer.getTriangles()[i].color2 >> 8) & 0xff) / 255.f;
		pVertices[iIndex].vColor.z = ((Buffer.getTriangles()[i].color2 >> 16) & 0xff) / 255.f;

		iIndex++;
	}

	VTXPOSCOLOR* pVertexBuffer = reinterpret_cast<VTXPOSCOLOR*>(MappedResource.pData);
	memcpy(pVertexBuffer, pVertices, sizeof(VTXPOSCOLOR) * m_iNumVertices);

	m_pContext->Unmap(m_pVB, 0);
	Safe_Delete_Array(pVertices);

	m_iNumIndices = m_iNumVertices;

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	_ushort* pIndices = new _ushort[m_iNumIndices]{};

	for (_ushort i = 0; i < m_iNumIndices; i++)
	{
		pIndices[i] = i;
	}

	_uint* pIndexBuffer = reinterpret_cast<_uint*>(MappedResource.pData);
	memcpy(pIndexBuffer, pIndices, sizeof(_ushort) * m_iNumIndices);

	m_pContext->Unmap(m_pIB, 0);

	Safe_Delete_Array(pIndices);

	return S_OK;
}

CVIBuffer_PhysX* CVIBuffer_PhysX::Create(_dev pDevice, _context pContext, const PxRenderBuffer& Buffer)
{
	CVIBuffer_PhysX* pInstance = new CVIBuffer_PhysX(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(Buffer)))
	{
		MSG_BOX("Failed to Create : CVIBuffer_PhysX");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_PhysX::Clone(void* pArg)
{
	CVIBuffer_PhysX* pInstance = new CVIBuffer_PhysX(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_PhysX");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_PhysX::Free()
{
	__super::Free();
}
