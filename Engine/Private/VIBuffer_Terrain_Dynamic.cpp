#include "VIBuffer_Terrain_Dynamic.h"

CVIBuffer_Terrain_Dynamic::CVIBuffer_Terrain_Dynamic(_dev pDevice, _context pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain_Dynamic::CVIBuffer_Terrain_Dynamic(const CVIBuffer_Terrain_Dynamic& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Terrain_Dynamic::Init_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Terrain_Dynamic::Init(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	VERTEX_DESC* pDesc = reinterpret_cast<VERTEX_DESC*>(pArg);

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof VTXPOSTEX;
	_uint iNumVerticesX = pDesc->iSize.x / pDesc->iC.x + 1;
	_uint iNumVerticesZ = pDesc->iSize.y / pDesc->iC.y + 1;
	m_iNumVertices = iNumVerticesX * iNumVerticesZ;

	m_iIndexStride = 4;
	m_iNumIndices = (iNumVerticesX - 1) * (iNumVerticesZ - 1) * 2 * 3;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region Vertex Buffer
	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];

	for (_uint z = 0; z < iNumVerticesZ; z++)
	{
		for (_uint x = 0; x < iNumVerticesX; x++)
		{
			_uint iIndex = z * iNumVerticesX + x;

			pVertices[iIndex].vPosition = _float3(static_cast<_float>(x), 0.f, static_cast<_float>(z));
			pVertices[iIndex].vTexcoord = _float2(x / (iNumVerticesX - 1.f), z / (iNumVerticesZ - 1.f));
		}
	}
#pragma endregion

#pragma region Index Buffer
	_ulong* pIndices = new _ulong[m_iNumIndices];

	_uint iNumIndices = 0;

	for (_uint z = 0; z < iNumVerticesZ - 1; z++)
	{
		for (_uint x = 0; x < iNumVerticesX - 1; x++)
		{
			_uint iIndex = z * iNumVerticesX + x;

			_uint iIndices[4] =
			{
				iIndex + iNumVerticesX,
				iIndex + iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

		}
	}
#pragma endregion

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_InitialData.pSysMem = pVertices;
	if (FAILED(__super::Create_Buffer(&m_pVB)))
	{
		return E_FAIL;
	}

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	m_InitialData.pSysMem = pIndices;
	if (FAILED(__super::Create_Buffer(&m_pIB)))
	{
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

CVIBuffer_Terrain_Dynamic* CVIBuffer_Terrain_Dynamic::Create(_dev pDevice, _context pContext)
{
	CVIBuffer_Terrain_Dynamic* pInstance = new CVIBuffer_Terrain_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Terrain_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain_Dynamic::Clone(void* pArg)
{
	CVIBuffer_Terrain_Dynamic* pInstance = new CVIBuffer_Terrain_Dynamic(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_Terrain_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain_Dynamic::Free()
{
	__super::Free();
}
