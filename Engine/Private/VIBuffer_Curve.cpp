#include "VIBuffer_Curve.h"


CVIBuffer_Curve::CVIBuffer_Curve(_dev pDevice, _context pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Curve::CVIBuffer_Curve(const CVIBuffer_Curve& rhs)
	: CVIBuffer(rhs)
	, m_pCurvePos(rhs.m_pCurvePos)
	, m_matControlPoints(rhs.m_matControlPoints)
{
}

HRESULT CVIBuffer_Curve::Init_Prototype()
{
	_uint iNumSegments = 300;

	m_iNumVertices = iNumSegments;
	m_iNumPrimitives = 1;
	m_iNumIndicesPrimitives = 1;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
	m_iVertexStride = sizeof VTXPOS;

#pragma region VERTEX
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, m_iVertexStride * m_iNumVertices);

	m_pCurvePos = new _float3[m_iNumVertices];


	ZeroMemory(&m_matControlPoints, sizeof _mat);
	_vec4 vPoint0 = _vec4(-0.5f, 0.5f, 0.f, 1.f);
	_vec4 vPoint1 = _vec4(-0.5f, 0.5f, 0.f, 1.f);
	_vec4 vPoint2 = _vec4(1.f, 0.f, 0.f, 1.f);
	_vec4 vPoint3 = _vec4(1.f, 0.f, 0.f, 1.f);

	m_matControlPoints.Right(vPoint0);
	m_matControlPoints.Up(vPoint1);
	m_matControlPoints.Look(vPoint2);
	m_matControlPoints.Position(vPoint3);

	_float fTime = 0.f;
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		fTime = (_float)i / (_float)m_iNumVertices;

		// Catmull-Rom
		_vec4 vLerpPoint = XMVectorCatmullRom(m_matControlPoints.Right(), m_matControlPoints.Up(), m_matControlPoints.Look(), m_matControlPoints.Position(), fTime);
	
		 pVertices[i].vPosition = _vec3(vLerpPoint);
		 m_pCurvePos[i] = _vec3(vLerpPoint);
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
	{
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = sizeof(_ushort) * iNumSegments;  // 두 개의 인덱스
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_iNumIndices = iNumSegments;
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	_ushort* pIndices = new _ushort[iNumSegments];

	for (_uint i = 0; i < iNumSegments; ++i)
	{
		pIndices[i] = i;
		//m_vIndices[i] = i;
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Curve::Init(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Curve::Render()
{
	__super::Render();

	return S_OK;
}

void CVIBuffer_Curve::Get_ControlPoints(_mat* pOutPoints)
{
	pOutPoints = &m_matControlPoints;
}

void CVIBuffer_Curve::Set_ControlPoints(_mat& Points)
{
	m_matControlPoints = Points;

}

_float3 CVIBuffer_Curve::Get_CurvePos(_uint iIndex)
{
	return m_pCurvePos[iIndex];
}

void CVIBuffer_Curve::Modify_Line()
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	//VTXPOS* pVertices = (VTXPOS*)SubResource.pData;
	_float fTime = 0.f;
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		fTime = (_float)i / m_iNumVertices;

		// Catmull-Rom
		_vec4 vLerpPoint = XMVectorCatmullRom(m_matControlPoints.Right(), m_matControlPoints.Up(), m_matControlPoints.Look(), m_matControlPoints.Position(), fTime);
		//pVertices[i].vPosition = _vec3(vLerpPoint);
		((VTXPOS*)SubResource.pData)[i].vPosition = _vec3(vLerpPoint);
		m_pCurvePos[i] = _vec3(vLerpPoint);

	}

	m_pContext->Unmap(m_pVB, 0);

}

CVIBuffer_Curve* CVIBuffer_Curve::Create(_dev pDevice, _context pContext)
{
	CVIBuffer_Curve* pInstance = new CVIBuffer_Curve(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Curve");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Curve::Clone(void* pArg)
{
	CVIBuffer_Curve* pInstance = new CVIBuffer_Curve(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_Curve");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Curve::Free()
{
	__super::Free();
	if (!m_hasCloned)
		Safe_Delete_Array(m_pCurvePos);
}
