#include "VIBuffer_Model_Instancing.h"
#include "GameInstance.h"
#include "Mesh.h"
#include "Texture.h"

CVIBuffer_Model_Instancing::CVIBuffer_Model_Instancing(_dev pDevice, _context pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Model_Instancing::CVIBuffer_Model_Instancing(const CVIBuffer_Model_Instancing& rhs)
	: CVIBuffer(rhs)
	, m_InstancingBufferDesc(rhs.m_InstancingBufferDesc)
	, m_InstancingInitialData(rhs.m_InstancingInitialData)
	, m_iNumInstances(rhs.m_iNumInstances)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
	, m_iInstanceStride(rhs.m_iInstanceStride)
{
}

HRESULT CVIBuffer_Model_Instancing::Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix, _uint iNumInstance)
{
	

	return S_OK;
}

HRESULT CVIBuffer_Model_Instancing::Init(void* pArg)
{

	return S_OK;
}

void CVIBuffer_Model_Instancing::Update(_float fTimeDelta, _mat WorldMatrix)
{
	//D3D11_MAPPED_SUBRESOURCE SubResource{};

	//m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	//list<VTXMESHINSTANCING> VertexList{};

	//for (size_t i = 0; i < m_iNumInstances; i++)
	//{
	//	VTXMESHINSTANCING* pVertex = &reinterpret_cast<VTXMESHINSTANCING*>(SubResource.pData)[i];

	//	pVertex->vRight = _vec4(WorldMatrix._11, WorldMatrix._12, WorldMatrix._13, WorldMatrix._14);
	//	pVertex->vUp = _vec4(WorldMatrix._21, WorldMatrix._22, WorldMatrix._23, WorldMatrix._24);
	//	pVertex->vLook = _vec4(WorldMatrix._31, WorldMatrix._32, WorldMatrix._33, WorldMatrix._34);
	//	pVertex->vPos = _vec4(WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, WorldMatrix._44);
	//	
	//	VertexList.push_back(*pVertex);
	//	reinterpret_cast<VTXMESHINSTANCING*>(SubResource.pData)[i] = VertexList.front();
	//}

	//m_pContext->Unmap(m_pVBInstance, 0);

	//VertexList.pop_front();
	
}

HRESULT CVIBuffer_Model_Instancing::Render()
{
	ID3D11Buffer* pVertexBuffer[] =
	{
		m_pVB,
		m_pVBInstance,
	};

	_uint iVertexStrides[] =
	{
		m_iVertexStride,
		m_iInstanceStride,
	};

	_uint iOffsets[] =
	{
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffer, iVertexStrides, iOffsets);

	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstances, 0, 0, 0);

	return S_OK;
}



CVIBuffer_Model_Instancing* CVIBuffer_Model_Instancing::Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix, _uint iNumInstance)
{
	CVIBuffer_Model_Instancing* pInstance = new CVIBuffer_Model_Instancing(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strFilePath, isCOLMesh, PivotMatrix, iNumInstance)))
	{
		MSG_BOX("Failed to Create : CModel");
	}

	return pInstance;
}

CComponent* CVIBuffer_Model_Instancing::Clone(void* pArg)
{
	CVIBuffer_Model_Instancing* pInstance = new CVIBuffer_Model_Instancing(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CModel");
	}

	return pInstance;
}


void CVIBuffer_Model_Instancing::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}

