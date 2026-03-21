#include "VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(_dev pDevice, _context pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
{
}

HRESULT CVIBuffer_Terrain::Init_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Terrain::Init(void* pArg)
{
	m_pTerrain = *(TERRAIN_INFO*)pArg;
	_bool isMesh = m_pTerrain.isMesh;

	if (isMesh == false)
	{
		m_iNumVerticesX = m_pTerrain.vTerrainSize.x;
		m_iNumVerticesZ = m_pTerrain.vTerrainSize.y;

		m_iNumVertexBuffers = 1;
		m_iVertexStride = sizeof VTXNORTEX;
		m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

		m_iIndexStride = 4;
		m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;

		m_eIndexFormat = DXGI_FORMAT_R32_UINT;
		m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region Vertex Buffer
		VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
		m_pVertices = new _float3[m_iNumVertices];
		ZeroMemory(m_pVertices, sizeof(_float3) * m_iNumVertices);

		for (_uint z = 0; z < m_iNumVerticesZ; z++)
		{
			for (_uint x = 0; x < m_iNumVerticesX; x++)
			{
				_uint iIndex = z * m_iNumVerticesX + x;

				m_pVertices[iIndex] = pVertices[iIndex].vPosition = _float3(static_cast<_float>(x), 0.f, static_cast<_float>(z));
				//XMStoreFloat3(&pVertices[iIndex].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndex].vNormal)));
				pVertices[iIndex].vNormal = _vec3(0.f, 0.f, 0.f);
				pVertices[iIndex].vTexcoord = _float2(x / (m_iNumVerticesX - 1.f), z / (m_iNumVerticesZ - 1.f));
			}
		}

#pragma endregion

#pragma region Index Buffer
		_ulong* pIndices = new _ulong[m_iNumIndices];

		_uint iNumIndices{};

		for (_uint z = 0; z < m_iNumVerticesZ - 1; z++)
		{
			for (_uint x = 0; x < m_iNumVerticesX - 1; x++)
			{
				_uint iIndex = z * m_iNumVerticesX + x;

				_uint iIndices[4] =
				{
					iIndex + m_iNumVerticesX,
					iIndex + m_iNumVerticesX + 1,
					iIndex + 1,
					iIndex
				};


				_vector vSrc{}, vDst{}, vNor{};

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];
			//pIndices[iNumIndices++] = iIndices[3];
			//pIndices[iNumIndices++] = iIndices[0];
			//pIndices[iNumIndices++] = iIndices[1];

			vSrc = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDst = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNor = XMVector3Normalize(XMVector3Cross(vSrc, vDst));
			//vSrc = XMLoadFloat3(&pVertices[iIndices[0]].vPosition) - XMLoadFloat3(&pVertices[iIndices[3]].vPosition);
			//vDst = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			//vNor = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNor);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal)) + vNor);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNor);
			//XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNor));
			//XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNor));
			//XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNor));

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];
			//pIndices[iNumIndices++] = iIndices[3];
			//pIndices[iNumIndices++] = iIndices[1];
			//pIndices[iNumIndices++] = iIndices[2];

			vSrc = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDst = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNor = XMVector3Normalize(XMVector3Cross(vSrc, vDst));
			//vSrc = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[3]].vPosition);
			//vDst = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			//vNor = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNor);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNor);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal)) + vNor);
			//XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNor));
			//XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNor));
			//XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNor));
			}
		}
#pragma endregion

		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = m_iVertexStride;

		m_InitialData.pSysMem = pVertices;
		if (FAILED(__super::Create_Buffer(&m_pVB)))
		{
			return E_FAIL;
		}

		m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = 0;

		m_InitialData.pSysMem = pIndices;
		if (FAILED(__super::Create_Buffer(&m_pIB)))
		{
			return E_FAIL;
		}

		Safe_Delete_Array(pVertices);
		Safe_Delete_Array(pIndices);
	}
	else
	{
		//Mesh_Terrain(m_pTerrain.Vertices, m_pTerrain.Indices);
	}

	return S_OK;
}


HRESULT CVIBuffer_Terrain::Mesh_Terrain(vector<VTXSTATICMESH> vVertices, vector<_ulong> vIndices)
{
#pragma region VERTEX_BUFFER
	//D3D11_MAPPED_SUBRESOURCE MappedVertex;
	//m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedVertex);
	//VTXNORTEX* pVertices = (VTXNORTEX*)MappedVertex.pData;

	m_iNumVerticesX = m_pTerrain.vTerrainSize.x;
	m_iNumVerticesZ = m_pTerrain.vTerrainSize.y;

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof VTXNORTEX;
	m_iNumVertices = vVertices.size();

	m_iIndexStride = 4;
	m_iNumIndices = vIndices.size();

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		pVertices[i].vPosition = vVertices[i].vPosition;
		pVertices[i].vNormal = vVertices[i].vNormal;
		pVertices[i].vTexcoord = vVertices[i].vTexcoord;

		m_vVertices.push_back(pVertices[i]);
	}



	//m_pContext->Unmap(m_pVB, 0);

#pragma endregion

#pragma region INDEX_BUFFER

#pragma endregion
	//D3D11_MAPPED_SUBRESOURCE MappedIndex;
	//m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedIndex);
	//_ulong* pIndices = (_ulong*)MappedIndex.pData;
	_ulong* pIndices = new _ulong[m_iNumIndices];

	for (_uint i = 0; i < m_iNumIndices; i++)
	{
		pIndices[i] = vIndices[i];
		m_vIndices.push_back(pIndices[i]);
	}

	//m_pContext->Unmap(m_pIB, 0);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_InitialData.pSysMem = pVertices;
	if (FAILED(__super::Create_Buffer(&m_pVB)))
	{
		return E_FAIL;
	}

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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

HRESULT CVIBuffer_Terrain::Modify_Terrain_Hight(_uint iNumVerticesX, _uint iNumVerticesZ, _vec3 PickedPosition, _float fNewHeight, _float fSize)
{
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof VTXNORTEX;
	m_iNumVertices = iNumVerticesX * iNumVerticesZ;
	m_iIndexStride = 4;
	m_iNumIndices = (iNumVerticesX - 1) * (iNumVerticesZ - 1) * 2 * 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	D3D11_MAPPED_SUBRESOURCE MappedVertex;
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedVertex);
	VTXNORTEX* pVertices = (VTXNORTEX*)MappedVertex.pData;

	for (_uint z = 0; z < iNumVerticesZ; z++)
	{
		for (_uint x = 0; x < iNumVerticesX; x++)
		{
			_uint iIndex = z * iNumVerticesX + x;

			_vec3 vertexPosition = _float3(pVertices[iIndex].vPosition.x, pVertices[iIndex].vPosition.y, pVertices[iIndex].vPosition.z);
			//_float distance = XMVectorGetX(XMVector3Length(PickedPosition - vertexPosition));
			//_float fDistance = SimpleMath::Vector3::Distance(vertexPosition, PickedPosition);
			_vec2 vVertexPos = _vec2(vertexPosition.x, vertexPosition.z);
			_vec2 vPickedPos = _vec2(PickedPosition.x, PickedPosition.z);

			//_float fDistance = SimpleMath::Vector3::Distance(vertexPosition, PickedPosition);
			_float fDistance = SimpleMath::Vector2::Distance(vVertexPos, vPickedPos);
			if (fDistance <= fSize)
			{
				_float fBlend = 3.0f - (fDistance / fSize);
				_float fInterpolatedHeight = Lerp(pVertices[iIndex].vPosition.y, fNewHeight, fBlend);
				pVertices[iIndex].vPosition.y = fInterpolatedHeight;

				//pVertices[iIndex].vPosition.y = fNewHight;
				XMStoreFloat3(&pVertices[iIndex].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndex].vNormal)));
				pVertices[iIndex].vTexcoord = _float2(x / (iNumVerticesX - 1.f), z / (iNumVerticesZ - 1.f));
			}
		}
	}
	m_pContext->Unmap(m_pVB, 0);


#pragma region INDEX_BUFFER
	D3D11_MAPPED_SUBRESOURCE MappedIndex;
	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedIndex);
	_ulong* pIndices = (_ulong*)MappedIndex.pData;

	_uint iNumIndices = 0;
	for (size_t i = 0; i < iNumVerticesZ - 1; ++i)
	{
		for (size_t j = 0; j < iNumVerticesX - 1; ++j)
		{
			_uint	iIndex = (_uint)i * iNumVerticesX + (_uint)j;
			_uint	iIndices[4] = {
				iIndex + iNumVerticesX,
				iIndex + iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};
			_vector vSrc{}, vDst{}, vNor{};

			//pIndices[iNumIndices++] = iIndices[0];
			//pIndices[iNumIndices++] = iIndices[1];
			//pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];
			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];

			//vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			//vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			//vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));
			vSrc = XMLoadFloat3(&pVertices[iIndices[0]].vPosition) - XMLoadFloat3(&pVertices[iIndices[3]].vPosition);
			vDst = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vNor = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			//XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
			//XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal)) + vNormal);
			//XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNor));
			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNor));
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNor));

			//pIndices[iNumIndices++] = iIndices[0];
			//pIndices[iNumIndices++] = iIndices[2];
			//pIndices[iNumIndices++] = iIndices[3];
			pIndices[iNumIndices++] = iIndices[3];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			//vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			//vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			//vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));
			vSrc = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[3]].vPosition);
			vDst = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNor = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			//XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
			//XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNormal);
			//XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNor));
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNor));
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNor));
		}
	}

	m_pContext->Unmap(m_pIB, 0);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Terrain_Init(_uint iNumVerticesX, _uint iNumVerticesZ, _vec3 PickedPosition, _float fNewHeight, _float fSize)
{
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof VTXNORTEX;
	m_iNumVertices = iNumVerticesX * iNumVerticesZ;
	m_iIndexStride = 4;
	m_iNumIndices = (iNumVerticesX - 1) * (iNumVerticesZ - 1) * 2 * 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	D3D11_MAPPED_SUBRESOURCE MappedVertex;
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedVertex);
	VTXNORTEX* pVertices = (VTXNORTEX*)MappedVertex.pData;

	for (_uint z = 0; z < iNumVerticesZ; z++)
	{
		for (_uint x = 0; x < iNumVerticesX; x++)
		{
			_uint iIndex = z * iNumVerticesX + x;
			m_pVertices[iIndex] = pVertices[iIndex].vPosition = _float3(static_cast<_float>(x), 0.f, static_cast<_float>(z));
			pVertices[iIndex].vNormal = _vec3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(x / (m_iNumVerticesX - 1.f), z / (m_iNumVerticesZ - 1.f));
		}
	}


	m_pContext->Unmap(m_pVB, 0);


#pragma region INDEX_BUFFER
	D3D11_MAPPED_SUBRESOURCE MappedIndex;
	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedIndex);
	_ulong* pIndices = (_ulong*)MappedIndex.pData;

	_uint iNumIndices = 0;
	for (size_t i = 0; i < iNumVerticesZ - 1; ++i)
	{
		for (size_t j = 0; j < iNumVerticesX - 1; ++j)
		{
			_uint	iIndex = (_uint)i * iNumVerticesX + (_uint)j;
			_uint	iIndices[4] = {
				iIndex + iNumVerticesX,
				iIndex + iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};


			_vector vSrc{}, vDst{}, vNor{};

			//pIndices[iNumIndices++] = iIndices[0];
			//pIndices[iNumIndices++] = iIndices[1];
			//pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];
			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];

			//vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			//vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			//vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));
			vSrc = XMLoadFloat3(&pVertices[iIndices[0]].vPosition) - XMLoadFloat3(&pVertices[iIndices[3]].vPosition);
			vDst = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vNor = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			//XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
			//XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal)) + vNormal);
			//XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNor));
			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNor));
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNor));

			//pIndices[iNumIndices++] = iIndices[0];
			//pIndices[iNumIndices++] = iIndices[2];
			//pIndices[iNumIndices++] = iIndices[3];
			pIndices[iNumIndices++] = iIndices[3];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			//vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			//vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			//vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));
			vSrc = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[3]].vPosition);
			vDst = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNor = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			//XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
			//XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNormal);
			//XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNor));
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNor));
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNor));
		}
	}

	m_pContext->Unmap(m_pIB, 0);
#pragma endregion

	return S_OK;
}


HRESULT CVIBuffer_Terrain::ModifyTerrainVertexBuffer(_uint iNumVerticesX, _uint iNumVerticesZ)
{
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof VTXNORTEX;
	m_iNumVertices = iNumVerticesX * iNumVerticesZ;
	m_iIndexStride = 4;
	m_iNumIndices = (iNumVerticesX - 1) * (iNumVerticesZ - 1) * 2 * 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	D3D11_MAPPED_SUBRESOURCE MappedVertex;
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedVertex);
	VTXNORTEX* pVertices = (VTXNORTEX*)MappedVertex.pData;

	for (_uint z = 0; z < iNumVerticesZ; z++)
	{
		for (_uint x = 0; x < iNumVerticesX; x++)
		{
			_uint iIndex = z * iNumVerticesX + x;

			pVertices[iIndex].vPosition = _float3(static_cast<_float>(x), 0.f, static_cast<_float>(z));
			XMStoreFloat3(&pVertices[iIndex].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndex].vNormal)));
			pVertices[iIndex].vTexcoord = _float2(x / (iNumVerticesX - 1.f), z / (iNumVerticesZ - 1.f));

		}
	}

	m_pContext->Unmap(m_pVB, 0);
#pragma endregion

#pragma region INDEX_BUFFER
	D3D11_MAPPED_SUBRESOURCE MappedIndex;
	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedIndex);
	_ulong* pIndices = (_ulong*)MappedIndex.pData;

	_uint iNumIndices = 0;
	for (size_t i = 0; i < iNumVerticesZ - 1; ++i)
	{
		for (size_t j = 0; j < iNumVerticesX - 1; ++j)
		{
			_uint	iIndex = (_uint)i * iNumVerticesX + (_uint)j;
			_uint	iIndices[4] = {
				iIndex + iNumVerticesX,
				iIndex + iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};


			_vector vSrc{}, vDst{}, vNor{};


			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];
			//pIndices[iNumIndices++] = iIndices[3];
			//pIndices[iNumIndices++] = iIndices[0];
			//pIndices[iNumIndices++] = iIndices[1];

			vSrc = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDst = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNor = XMVector3Normalize(XMVector3Cross(vSrc, vDst));
			//vSrc = XMLoadFloat3(&pVertices[iIndices[0]].vPosition) - XMLoadFloat3(&pVertices[iIndices[3]].vPosition);
			//vDst = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			//vNor = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNor);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal)) + vNor);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNor);
			//XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNor));
			//XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNor));
			//XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNor));

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];
			//pIndices[iNumIndices++] = iIndices[3];
			//pIndices[iNumIndices++] = iIndices[1];
			//pIndices[iNumIndices++] = iIndices[2];

			vSrc = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDst = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNor = XMVector3Normalize(XMVector3Cross(vSrc, vDst));
			//vSrc = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[3]].vPosition);
			//vDst = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			//vNor = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNor);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNor);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal)) + vNor);
			//XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNor));
			//XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNor));
			//XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNor));
		}
	}

	m_pContext->Unmap(m_pIB, 0);
#pragma endregion

	return S_OK;
}
CVIBuffer_Terrain* CVIBuffer_Terrain::Create(_dev pDevice, _context pContext)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	Safe_Delete_Array(m_pVertices);
	__super::Free();

}
