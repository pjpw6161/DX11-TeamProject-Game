#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(_dev pDevice, _context pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	const _char* Get_FilePath() const;
	const _uint& Get_NumMeshes() const;
	const _uint& Get_NumAnim() const;
	const _uint Get_NumBones() const;
	const _bool& IsAnimationFinished(_uint iAnimIndex) const;
	const _uint& Get_CurrentAnimationIndex() const;
	const _float Get_CurrentAnimPos() const;
	const _mat* Get_BoneMatrix(const _char* pBoneName) const;
	_mat Get_PivotMatrix();
	vector<class CAnimation*>& Get_Animations();
	class CAnimation* Get_Animation(_uint iAnimIndex);
	vector<class CBone*>& Get_Bones();

	void Set_Animation(ANIM_DESC Animation_Desc);
	vector<VTXSTATICMESH> Get_StaticMeshVertices();
	vector<_ulong> Get_StaticMeshIndices();
	_uint Get_NumIndices();

	//이펙트 트리거
	const _uint Get_NumTriggerEffect() const;
	TRIGGEREFFECT_DESC* Get_TriggerEffect(_uint iTriggerEffectIndex);
	vector<TRIGGEREFFECT_DESC>& Get_TriggerEffects();
	//툴에서만 쓰세요
	void Add_TriggerEffect(TRIGGEREFFECT_DESC TriggerEffectDesc);
	void Delete_TriggerEffect(_uint iTriggerEffectIndex);
	void Reset_TriggerEffects();
	//사운드 트리거
	const _uint Get_NumTriggerSound() const;
	TRIGGERSOUND_DESC* Get_TriggerSound(_uint iTriggerSoundIndex);
	vector<TRIGGERSOUND_DESC>& Get_TriggerSounds();
	void Add_TriggerSound(TRIGGERSOUND_DESC TriggerSoundDesc);
	void Delete_TriggerSound(_uint iTriggerSoundIndex);
	void Reset_TriggerSounds();

public:
	HRESULT Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix);
	HRESULT Init(void* pArg) override;

public:
	void Play_Animation(_float fTimeDelta, _bool OnClientTrigger = false);
	HRESULT Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pVariableName);
	HRESULT Bind_Material(class CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType);
	void Apply_TransformToActor(_fmatrix WorldMatrix);

	HRESULT Render(_uint iMeshIndex);
	//HRESULT Render_Instancing(_uint iMeshIndex, class CVIBuffer_Mesh_Instance*& pInstanceBuffer, CModel*& pModel, CShader*& pShader, _mat WorldMat);
	HRESULT Render_Instancing(class CVIBuffer_Mesh_Instance*& pInstanceBuffer, class CShader*& pShader);
	HRESULT Render_Reflection_Instancing(class CVIBuffer_Mesh_Instance*& pInstanceBuffer, class CShader*& pShader, _float4 vClipPlane);
	HRESULT Render_Shadow_Instancing(class CVIBuffer_Mesh_Instance*& pInstanceBuffer, class CShader*& pShader);

	_bool Intersect_RayModel(_fmatrix WorldMatrix, _vec4* pPickPos);
	_int Get_InstanceID() { return m_iInstanceID; }
	_float Get_Radius();

private:
	class CTransform* m_pOwnerTransform{};
	_bool m_IsResetTriggers{};
	//이펙트 트리거
	_uint m_iNumTriggersEffect{};
	vector<TRIGGEREFFECT_DESC> m_TriggerEffects;
	vector<_mat*> m_EffectMatrices;
	// 사운드 트리거
	_uint m_iNumTriggersSound{};
	vector<TRIGGERSOUND_DESC> m_TriggerSounds;
	_randNum m_RandomNumber;

	_char m_szFilePath[MAX_PATH] = "";
	_float3* m_Vertices{};

	_uint m_iNumMeshes{};
	vector<class CMesh*> m_Meshes{};

	_uint m_iNumMaterials{};
	vector<Model_Material> m_Materials{};

	vector<class CBone*> m_Bones{};

	_uint m_iNumAnimations{};
	vector<class CAnimation*> m_Animations{};

	_mat m_PivotMatrix{};

	_uint m_iNumFaceMeshes{};
	_uint m_iNumHeadMeshes{};
	_uint m_iNumLowerMeshes{};
	_uint m_iNumUpperMeshes{};

	_uint m_iPrevAnimIndex{};
	_bool m_isAnimChanged{};

	ANIM_DESC m_AnimDesc{};

	ID3D11Texture2D* m_pTexture{ nullptr };
	ID3D11ShaderResourceView* m_pSRV{ nullptr };

	static _int	m_iNextInstanceID;
	_int m_iInstanceID{};

private:
	HRESULT Read_Bones(ifstream& File);
	HRESULT Read_Meshes(ifstream& File, const ModelType& eType, _fmatrix PivotMatrix);
	HRESULT Read_Animations(ifstream& File);
	HRESULT Read_Materials(ifstream& File, const string& strFilePath);
	HRESULT Read_TriggerEffects(const string& strFilePath);
	HRESULT Read_TriggerSounds(const string& strFilePath);

public:
	static CModel* Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh = false, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END