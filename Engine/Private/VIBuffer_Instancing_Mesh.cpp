#include "VIBuffer_Instancing_Mesh.h"
#include "Texture.h"
#include "Mesh.h"

CVIBuffer_Instancing_Mesh::CVIBuffer_Instancing_Mesh(_dev pDevice, _context pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_Instancing_Mesh::CVIBuffer_Instancing_Mesh(const CVIBuffer_Instancing_Mesh& rhs)
	: CVIBuffer_Instancing(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterials)
		{
			Safe_AddRef(pTexture);
		}
	}

	strcpy_s(m_szFilePath, MAX_PATH, rhs.m_szFilePath);
}

const _uint& CVIBuffer_Instancing_Mesh::Get_NumMeshes() const
{
	return m_iNumMeshes;
}

HRESULT CVIBuffer_Instancing_Mesh::Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix, _uint iNumInstance)
{
	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof VTXSTATICMESH;

	m_iNumInstances = iNumInstance;
	m_iInstanceStride = sizeof Instance_Data;

	//m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstances;

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	strcpy_s(m_szFilePath, MAX_PATH, strFilePath.c_str());
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);
	ModelType eType{};
	_char szExt[MAX_PATH]{};
	_splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
	if (!isCOLMesh)
	{
		eType = ModelType::Static;
	}
	else
	{
		eType = ModelType::Collision;
	}

	ifstream ModelFile(strFilePath.c_str(), ios::binary);
	if (ModelFile.is_open())
	{

		if (FAILED(Read_Meshes(ModelFile, eType, PivotMatrix)))
		{
			return E_FAIL;
		}

		if (FAILED(Read_Materials(ModelFile, strFilePath)))
		{
			return E_FAIL;
		}

		ModelFile.close();
	}
	else
	{
		MSG_BOX("Failed to Open File!");
		return E_FAIL;
	}



#pragma region INSTANCE_BUFFER
	ZeroMemory(&m_InstancingBufferDesc, sizeof m_InstancingBufferDesc);
	m_InstancingBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances;
	m_InstancingBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstancingBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstancingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstancingBufferDesc.MiscFlags = 0;
	m_InstancingBufferDesc.StructureByteStride = m_iInstanceStride;

	ZeroMemory(&m_InstancingInitialData, sizeof m_InstancingInitialData);

	Instance_Data* pVertexInstance = new Instance_Data[m_iNumInstances];
	ZeroMemory(pVertexInstance, sizeof(Instance_Data) * m_iNumInstances);

	for (size_t i = 0; i < m_iNumInstances; i++)
	{
	//pVertexInstance[i].vRight = _vec4(1.f, 0.f, 0.f, 0.f);
	//pVertexInstance[i].vUp = _vec4(0.f, 1.f, 0.f, 0.f);
	//pVertexInstance[i].vLook = _vec4(0.f, 0.f, 1.f, 0.f);
	//pVertexInstance[i].vPos = _vec4(0.f, 0.f,0.f, 1.f);
		pVertexInstance[i].mMatrix = 
	}
	m_InstancingInitialData.pSysMem = pVertexInstance;


#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Instancing_Mesh::Init(void* pArg)
{
	//MESH_DESC* pDesc = (MESH_DESC*)pArg;
	vector<_vec4> vInstancePos = *(vector<_vec4>*)pArg;


	Instance_Data* pVertexInstance = reinterpret_cast<Instance_Data*>(const_cast<void*>(m_InstancingInitialData.pSysMem));

	if (pArg)
	{
		size_t numPositions = vInstancePos.size();
		for (size_t i = 0; i < m_iNumInstances; i++)
		{
			if (i < numPositions)
			{
				pVertexInstance[i].vPos = vInstancePos[i];
			}
			else
			{
				// 기본 위치 설정
				pVertexInstance[i].vPos = _vec4(0, 0, 0, 0);
			}
		}
		//for (size_t i = 0; i < m_iNumInstances; i++)
		//{
		//	//pVertexInstance[i].vRight = _vec4(mWorldMatrix._11, mWorldMatrix._12, mWorldMatrix._13, mWorldMatrix._14);
		//	//pVertexInstance[i].vUp = _vec4(mWorldMatrix._21, mWorldMatrix._22, mWorldMatrix._23, mWorldMatrix._24);
		//	//pVertexInstance[i].vLook = _vec4(mWorldMatrix._31, mWorldMatrix._32, mWorldMatrix._33, mWorldMatrix._34);
		//	//pVertexInstance[i].vPos = _vec4(mWorldMatrix._41, mWorldMatrix._42, mWorldMatrix._43, mWorldMatrix._44);
		//	pVertexInstance[i].vPos = vInstancePos[i];

		//}

	}
	
	if (FAILED(m_pDevice->CreateBuffer(&m_InstancingBufferDesc, &m_InstancingInitialData, &m_pVBInstance)))
	{
		Safe_Delete_Array(m_InstancingInitialData.pSysMem);
		return E_FAIL;
	}
	return S_OK;

}

HRESULT CVIBuffer_Instancing_Mesh::Read_Meshes(ifstream& File, const ModelType& eType, _fmatrix PivotMatrix)
{
	File.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof _uint);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		File.read(reinterpret_cast<_char*>(&m_iMatIndex), sizeof _uint);

		_uint iNameSize{};
		File.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
		if (iNameSize >= sizeof(m_szName))
		{
			MSG_BOX("Name Is Too Long!");
		}
		File.read(m_szName, iNameSize);
		File.read(reinterpret_cast<_char*>(&m_iNumVertices), sizeof _uint);
		_uint iNumFaces{};
		File.read(reinterpret_cast<_char*>(&iNumFaces), sizeof _uint);

		m_iIndexStride = 4;
		m_iNumIndices = iNumFaces * 3 * m_iNumInstances;

		m_eIndexFormat = DXGI_FORMAT_R32_UINT;
		m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


		m_iVertexStride = sizeof VTXSTATICMESH;
		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = m_iVertexStride;

		ZeroMemory(&m_InitialData, sizeof m_InitialData);

		VTXSTATICMESH* pVertices = new VTXSTATICMESH[m_iNumVertices]{};

		for (size_t i = 0; i < m_iNumVertices; i++)
		{
			File.read(reinterpret_cast<_char*>(&pVertices[i].vPosition), sizeof _float3);
			XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));
			File.read(reinterpret_cast<_char*>(&pVertices[i].vNormal), sizeof _float3);
			XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));
			File.read(reinterpret_cast<_char*>(&pVertices[i].vTexcoord), sizeof _float2);
			File.read(reinterpret_cast<_char*>(&pVertices[i].vTangent), sizeof _float3);

			m_vMeshVertices.push_back(pVertices[i]);
		}

		m_InitialData.pSysMem = pVertices;

		if (FAILED(__super::Create_Buffer(&m_pVB)))
		{
			return E_FAIL;
		}

		Safe_Delete_Array(pVertices);

#pragma region Index
		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
		m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = 0;

		m_iIndexCountPerInstance = m_iNumIndices;

		ZeroMemory(&m_InitialData, sizeof m_InitialData);

		_ulong* pIndices = new _ulong[m_iNumIndices]{};

		_ulong dwIndex{};
		for (size_t i = 0; i < iNumFaces; i++)
		{
			File.read(reinterpret_cast<_char*>(&pIndices[dwIndex]), sizeof _uint);
			dwIndex++;
			m_vIndices.push_back(pIndices[dwIndex]);
			File.read(reinterpret_cast<_char*>(&pIndices[dwIndex]), sizeof _uint);
			dwIndex++;
			m_vIndices.push_back(pIndices[dwIndex]);
			File.read(reinterpret_cast<_char*>(&pIndices[dwIndex]), sizeof _uint);
			dwIndex++;
			m_vIndices.push_back(pIndices[dwIndex]);
		}

		m_InitialData.pSysMem = pIndices;

		if (FAILED(__super::Create_Buffer(&m_pIB)))
		{
			return E_FAIL;
		}
		Safe_Delete_Array(pIndices);
#pragma endregion
	}

	return S_OK;
}

HRESULT CVIBuffer_Instancing_Mesh::Read_Materials(ifstream& File, const string& strFilePath)
{
	_char szMatFilePath[MAX_PATH]{};
	_char szFullPath[MAX_PATH]{};

	_splitpath_s(strFilePath.c_str(), nullptr, 0, szMatFilePath, MAX_PATH, nullptr, 0, nullptr, 0);
	strcat_s(szMatFilePath, "../Texture/");

	File.read(reinterpret_cast<_char*>(&m_iNumMaterials), sizeof _uint);

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		Model_Material Material{};
		_uint iNameSize{};
		_char* pFileName{};

		for (size_t j = 0; j < ToIndex(TextureType::End); j++)
		{
			File.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
			if (iNameSize == 1)
			{
				continue;
			}

			pFileName = new _char[iNameSize];
			File.read(pFileName, iNameSize);
			if (strlen(szMatFilePath) + strlen(pFileName) >= sizeof(szFullPath))
			{
				MSG_BOX("Name Is Too Long!");
			}
			strcpy_s(szFullPath, szMatFilePath);
			strcat_s(szFullPath, pFileName);
			Safe_Delete_Array(pFileName);

			_tchar szTexturePath[MAX_PATH]{};
			MultiByteToWideChar(CP_ACP, 0, szFullPath, static_cast<_int>(strlen(szFullPath)), szTexturePath, MAX_PATH);

			Material.pMaterials[j] = CTexture::Create(m_pDevice, m_pContext, szTexturePath);
			if (!Material.pMaterials[j])
			{
				MSG_BOX("Failed to Create Texture from Model!");
				return E_FAIL;
			}
		}

		m_Materials.push_back(Material);
	}

	return S_OK;
}


HRESULT CVIBuffer_Instancing_Mesh::Bind_Material(CShader* pShader, const _char* pVariableName, TextureType eTextureType)
{
	CTexture* pMaterial = m_Materials[0].pMaterials[ToIndex(eTextureType)];
	if (!pMaterial)
	{
		return E_FAIL;
	}

	return pMaterial->Bind_ShaderResource(pShader, pVariableName);
}


CVIBuffer_Instancing_Mesh* CVIBuffer_Instancing_Mesh::Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix, _uint iNumInstance)
{
	CVIBuffer_Instancing_Mesh* pInstance = new CVIBuffer_Instancing_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strFilePath, isCOLMesh, PivotMatrix, iNumInstance)))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Instancing_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CVIBuffer_Instancing_Mesh::Clone(void* pArg)
{
	CVIBuffer_Instancing_Mesh* pInstance = new CVIBuffer_Instancing_Mesh(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_Instancing_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Instancing_Mesh::Free()
{
	if (not m_hasCloned)
	{
		Safe_Delete_Array(m_InstancingInitialData.pSysMem);
	}
	__super::Free();
	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterials)
		{
			Safe_Release(pTexture);
		}
	}
	m_Materials.clear();

}
