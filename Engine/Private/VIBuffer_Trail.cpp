#include "VIBuffer_Trail.h"

CVIBuffer_Trail::CVIBuffer_Trail(_dev pDevice, _context pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& rhs)
	: CVIBuffer(rhs)
	, m_TrailBufferDesc(rhs.m_TrailBufferDesc)
	, m_TrailInitialData(rhs.m_TrailInitialData)
	, m_iDetailRatio(rhs.m_iDetailRatio)
{
}

HRESULT CVIBuffer_Trail::Init_Prototype(const _uint iNumVertices, _float2 vPSize)
{
	m_iDetailRatio = 4;
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof VTXTRAIL;
	m_iNumVertices = iNumVertices * m_iDetailRatio;

	m_iIndexStride = 2;
	m_iNumIndices = iNumVertices * m_iDetailRatio;

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

#pragma region Vertex
	ZeroMemory(&m_TrailBufferDesc, sizeof m_TrailBufferDesc);
	m_TrailBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_TrailBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_TrailBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_TrailBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_TrailBufferDesc.MiscFlags = 0;
	m_TrailBufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_TrailInitialData, sizeof m_TrailInitialData);

	VTXTRAIL* pVertices = new VTXTRAIL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTRAIL) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		pVertices[i].vPosition = _float3(0.f, 0.f, 0.f);
		pVertices[i].vPSize = vPSize;
		pVertices[i].vColor = _float4(1.f, 1.f, 1.f, 1.f);
	}

	m_TrailInitialData.pSysMem = pVertices;
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

HRESULT CVIBuffer_Trail::Init(void* pArg)
{
	if (FAILED(m_pDevice->CreateBuffer(&m_TrailBufferDesc, &m_TrailInitialData, &m_pVB)))
	{
		Safe_Delete_Array(m_TrailInitialData.pSysMem);
		return E_FAIL;
	}

	return S_OK;
}

void CVIBuffer_Trail::Update(_uint iNumVerticesToUse, _vec3* pPositions, _vec4* pColors, _vec2* pPSize)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	_uint iCMRIndex[4]{ 0, 0, 1, 2 };
	_float fWeight{};

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		if (i >= (iNumVerticesToUse - 1) * m_iDetailRatio)
		{
			reinterpret_cast<VTXTRAIL*>(SubResource.pData)[i].vPosition = pPositions[iNumVerticesToUse - 1];
			reinterpret_cast<VTXTRAIL*>(SubResource.pData)[i].vColor = _float4();
			reinterpret_cast<VTXTRAIL*>(SubResource.pData)[i].vPSize = _float2();
		}
		else
		{
			if (i == m_iDetailRatio)
			{
				iCMRIndex[1]++;
				iCMRIndex[2]++;
				iCMRIndex[3]++;
				fWeight = 0.f;
			}
			else if (i and i % m_iDetailRatio == 0)
			{
				iCMRIndex[0]++;
				iCMRIndex[1]++;
				iCMRIndex[2]++;
				if (iCMRIndex[3] < iNumVerticesToUse - 1)
				{
					iCMRIndex[3]++;
				}
				fWeight = 0.f;
			}

			if (pPositions)
			{
				reinterpret_cast<VTXTRAIL*>(SubResource.pData)[i].vPosition = _vec3(_vec4::CatmullRom(pPositions[iCMRIndex[0]], pPositions[iCMRIndex[1]], pPositions[iCMRIndex[2]], pPositions[iCMRIndex[3]], fWeight));
			}
			if (pColors)
			{
				reinterpret_cast<VTXTRAIL*>(SubResource.pData)[i].vColor = _vec4::Lerp(pColors[iCMRIndex[1]], pColors[iCMRIndex[2]], fWeight);
			}
			if (pPSize)
			{
				reinterpret_cast<VTXTRAIL*>(SubResource.pData)[i].vPSize = _vec2::Lerp(pPSize[iCMRIndex[1]], pPSize[iCMRIndex[2]], fWeight);
			}

			fWeight += 1.f / static_cast<_float>(m_iDetailRatio);
		}
	}

	m_pContext->Unmap(m_pVB, 0);
}

CVIBuffer_Trail* CVIBuffer_Trail::Create(_dev pDevice, _context pContext, _uint iNumVertices, _float2 vPSize)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(iNumVertices, vPSize)))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Trail::Clone(void* pArg)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();

	if (not m_hasCloned)
	{
		Safe_Delete_Array(m_TrailInitialData.pSysMem);
	}
}
