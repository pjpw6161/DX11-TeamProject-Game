#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing_Mesh final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Instancing_Mesh(_dev pDevice, _context pContext);
	CVIBuffer_Instancing_Mesh(const CVIBuffer_Instancing_Mesh& rhs);
	virtual ~CVIBuffer_Instancing_Mesh() = default;

public:
	const _uint& Get_NumMeshes() const;
	HRESULT Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix, _uint iNumInstance);
	HRESULT Init(void* pArg) override;

	HRESULT Read_Meshes(ifstream& File, const ModelType& eType, _fmatrix PivotMatrix);
	HRESULT Read_Materials(ifstream& File, const string& strFilePath);
	HRESULT Bind_Material(class CShader* pShader, const _char* pVariableName, TextureType eTextureType);


	//HRESULT Render(_uint iMeshIndex);

	_char m_szFilePath[MAX_PATH] = "";
	_char m_szName[MAX_PATH]{};
	_uint m_iMatIndex{};
	_uint m_iNumMeshes{};

	_uint m_iNumMaterials{};
	vector<Model_Material> m_Materials{};
	//vector<VTXSTATICMESH> vVertices {};
	//vector<_ulong> vIndices {};

private:
	_mat m_PivotMatrix{};

public:
	static CVIBuffer_Instancing_Mesh* Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh = false, _fmatrix PivotMatrix = XMMatrixIdentity(), _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END