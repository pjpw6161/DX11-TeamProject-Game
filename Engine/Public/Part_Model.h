#pragma once
#include "Component.h"

BEGIN(Engine)


class ENGINE_DLL CPart_Model final :
    public CComponent
{
private:
	CPart_Model(_dev pDevice, _context pContext);
	CPart_Model(const CPart_Model& rhs);
	virtual ~CPart_Model() = default;

public:
	HRESULT Init_Prototype(const string& strFilePath);
	HRESULT Init(void* pArg) override;

public:
	const void Set_isRender(_bool isRender) {
		m_isRender = isRender;
	}

	const _bool& Get_isRender() const {
		return m_isRender;
	}

	const _uint& Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	HRESULT Bind_Material(class CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType);
	HRESULT Get_Bone_Offset(const vector<class CBone*>& Bones);



	HRESULT Render(_uint iMeshIndex);

private:
	ModelType m_eType = ModelType::End;

	_uint m_iNumMeshes = 0;
	vector<class CMesh*> m_Meshes;

	_uint m_iNumMaterials = 0;
	vector<Model_Material> m_Materials;

	_bool m_isRender = false;

private:
	HRESULT Read_Meshes(ifstream& File, const ModelType& eType, _fmatrix PivotMatrix);
	HRESULT Read_Materials(ifstream& File, const string& strFilePath);

public:
	static CPart_Model* Create(_dev pDevice, _context pContext, const string& strFilePath);
    // CComponent을(를) 통해 상속됨
    virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END