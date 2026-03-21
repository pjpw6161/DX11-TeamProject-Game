#include "Part_Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

CPart_Model::CPart_Model(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

CPart_Model::CPart_Model(const CPart_Model& rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
{
	for (auto& pPrototypeMesh : rhs.m_Meshes) {
		CMesh* pMesh = reinterpret_cast<CMesh*>(pPrototypeMesh->Clone());

		m_Meshes.push_back(pMesh);
	}

	for (auto& Material : m_Materials)
		for (auto& pTexture : Material.pMaterials)
			Safe_AddRef(pTexture);
}

HRESULT CPart_Model::Init_Prototype(const string& strFilePath)
{
	m_eType = ModelType::Anim;


	ifstream ModelFile(strFilePath.c_str(), ios::binary);
	if (ModelFile.is_open())
	{
		if (FAILED(Read_Meshes(ModelFile, m_eType, XMMatrixIdentity())))
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

	return S_OK;
}

HRESULT CPart_Model::Init(void* pArg)
{
	return S_OK;
}

HRESULT CPart_Model::Bind_Material(CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType)
{
	_uint iMatIndex = m_Meshes[iMeshIndex]->Get_MatIndex();

	if (iMatIndex >= m_iNumMaterials)
	{
		return E_FAIL;
	}

	CTexture* pMaterial = m_Materials[iMatIndex].pMaterials[ToIndex(eTextureType)];
	if (!pMaterial)
	{
		return E_FAIL;
	}

	return pMaterial->Bind_ShaderResource(pShader, pVariableName);
}

HRESULT CPart_Model::Render(_uint iMeshIndex)
{
	if (FAILED(m_Meshes[iMeshIndex]->Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CPart_Model::Read_Meshes(ifstream& File, const ModelType& eType, _fmatrix PivotMatrix)
{
	File.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof _uint);
	m_Meshes.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, eType, File, PivotMatrix);
		if (!pMesh)
		{
			MSG_BOX("Failed to Read Meshes!");
			return E_FAIL;
		}
		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CPart_Model::Read_Materials(ifstream& File, const string& strFilePath)
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

HRESULT CPart_Model::Get_Bone_Offset(const vector<class CBone*>& Bones)
{
	for (auto& pMesh : m_Meshes)
		if (FAILED(pMesh->Set_Bone_Offset(Bones)))
			return E_FAIL;
	return S_OK;
}

CPart_Model* CPart_Model::Create(_dev pDevice, _context pContext, const string& strFilePath)
{
	CPart_Model* pInstance = new CPart_Model(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strFilePath)))
	{
		MSG_BOX("Failed to Create : CPart_Model");
	}

	return pInstance;
}

CComponent* CPart_Model::Clone(void* pArg)
{
	CPart_Model* pInstance = new CPart_Model(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPart_Model");
	}

	return pInstance;
}

void CPart_Model::Free()
{
	__super::Free();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterials)
		{
			Safe_Release(pTexture);
		}
	}
	m_Materials.clear();

}
