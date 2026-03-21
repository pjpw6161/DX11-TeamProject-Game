#pragma once
#include "Component.h"

BEGIN(Engine)

class CCustom_Part final : public CComponent
{
private:
	CCustom_Part(_dev pDevice, _context pContext);
	CCustom_Part(const CCustom_Part& rhs);
	virtual ~CCustom_Part() = default;

public:
	const _uint& Get_NumMeshes() const;

public:
	HRESULT Init_Prototype(ifstream& File, const string& strTexturePath, _fmatrix PivotMatrix);
	HRESULT Init(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pVariableName, vector<class CBone*>& Bones);
	HRESULT Bind_Material(class CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType);

	HRESULT Render(_uint iMeshIndex);

	_bool Intersect_RayModel(_fmatrix WorldMatrix, _float4* pPickPos);

private:
	_uint m_iNumMeshes{};
	vector<class CMesh*> m_Meshes{};

	_uint m_iNumMaterials{};
	vector<Model_Material> m_Materials{};

	_float44 m_PivotMatrix{};

private:
	HRESULT Read_Meshes(ifstream& File, _fmatrix PivotMatrix);
	HRESULT Read_Materials(ifstream& File, const string& strTexturePath);

public:
	static CCustom_Part* Create(_dev pDevice, _context pContext, ifstream& File, const string& strTexturePath,_fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END