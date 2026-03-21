#pragma once
#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(_dev pDevice, _context pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	const _uint& Get_MatIndex() const { return m_iMatIndex; }

public:
	HRESULT Init_Prototype(ModelType eType, ifstream& ModelFile, _mat OffsetMatrix);
	HRESULT Init(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const vector<class CBone*>& Bones, const _char* pVariableName, _mat PivotMatrix);
	_bool Intersect_RayMesh(_mat WorldMatrix, _vec4* pPickPos);
	void Apply_TransformToActor(_mat WorldMatrix);

	HRESULT Set_Bone_Offset(const vector<class CBone*>& Bones);
	vector<VTXSTATICMESH> Get_VtxStaticInfo() { return m_Vertices; }
	vector<_ulong> Get_InidcesStaticInfo() { return m_Indices; }
	_uint Get_NumIndices() { return m_iNum; }
	_float Get_Radius() { return m_fRadius; }

private:
	_float3* m_pVerticesPos{ nullptr };
	_float3* m_pVerticesNor{ nullptr };
	_ulong* m_pIndices{ nullptr };

	_vec3 m_vCenterPos{};

	_float m_fRadius{ 0.f };

	_char m_szName[MAX_PATH]{};
	_uint m_iMatIndex{};
	_uint m_iNumBones{};
	vector<_uint> m_BoneIndices{};
	vector<_mat> m_OffsetMatrices{};

	vector<VTXSTATICMESH> m_Vertices{};
	vector<_ulong> m_Indices{};

	_uint m_iNum{};
	_mat* m_BoneMatrices{};

	PxRigidStatic* m_pActor{ nullptr };
	ModelType m_eType{};
	VTXSTATICMESH m_VerticesInfo{};

private:
	HRESULT Ready_StaticMesh(ifstream& ModelFile, _mat OffsetMatrix);
	HRESULT Ready_AnimMesh(ifstream& ModelFile);

public:
	static CMesh* Create(_dev pDevice, _context pContext, ModelType eType, ifstream& ModelFile, _mat OffsetMatrix);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END