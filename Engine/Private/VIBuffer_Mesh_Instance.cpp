#include "VIBuffer_Mesh_Instance.h"
#include "GameInstance.h"

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(_dev pDevice, _context pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& rhs)
	: CVIBuffer(rhs)
	, m_InstancingBufferDesc(rhs.m_InstancingBufferDesc)
	, m_InstancingInitialData(rhs.m_InstancingInitialData)
	, m_iNumInstances(rhs.m_iNumInstances)
	, m_iInstanceStride(rhs.m_iInstanceStride)

{
}

HRESULT CVIBuffer_Mesh_Instance::Init_Prototype(_uint iMaxCount)
{
	m_iMaxCount = iMaxCount;
	vector<Instance_Data> MeshInstance(m_iMaxCount);

	m_iVertexStride = sizeof(Instance_Data);
	m_iNumVertexBuffers = 1;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = (_uint)(m_iVertexStride * MeshInstance.size());
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = MeshInstance.data();

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Init(void* pArg)
{
	return S_OK;
}

void CVIBuffer_Mesh_Instance::Update(_float fTimeDelta, _mat WorldMatrix, _int iNumUse, _bool bApplyGravity, _vec3 vGravityDir)
{
	
}

HRESULT CVIBuffer_Mesh_Instance::Render(CMesh* pMesh)
{
	ID3D11Buffer* pVertexBuffer[] =
	{
		pMesh->Get_VB(),
	};

	_uint iVertexStrides[] =
	{
		pMesh->Get_Stride(),
	};

	_uint iOffsets[] =
	{
		0,
	};

	m_pContext->IASetVertexBuffers(0, pMesh->Get_NumVertexBuffer(), pVertexBuffer, iVertexStrides, iOffsets);

	m_pContext->IASetIndexBuffer(pMesh->Get_IB(), pMesh->Get_IndexFormat(), 0);

	m_pContext->IASetPrimitiveTopology(pMesh->Get_PrimitiveTopology());
	const _uint iInstanceCount = static_cast<_uint>(m_vMeshInstance.size());
	D3D11_MAPPED_SUBRESOURCE SubResource{};
	SubResource.RowPitch = iInstanceCount;
	
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	{
		::memcpy(SubResource.pData, m_vMeshInstance.data(), sizeof(Instance_Data) * iInstanceCount);
	}

	m_pContext->Unmap(m_pVB, 0);
	
	m_pContext->IASetVertexBuffers(1, 1, &m_pVB, &m_iVertexStride, iOffsets);
	m_pContext->DrawIndexedInstanced(pMesh->Get_NumIndices(), iInstanceCount, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Mesh_Instance::Add_Instance(Instance_Data& MeshData)
{
	m_vMeshInstance.push_back(MeshData);
}

void CVIBuffer_Mesh_Instance::Clear_Instance()
{
	m_vMeshInstance.clear();
}

CVIBuffer_Mesh_Instance* CVIBuffer_Mesh_Instance::Create(_dev pDevice, _context pContext)
{
	CVIBuffer_Mesh_Instance* pInstance = new CVIBuffer_Mesh_Instance(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Mesh_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Mesh_Instance::Free()
{
	__super::Free();
	//Safe_Release(m_pVBInstance);
}
