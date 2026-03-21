#pragma once
#include "Component.h"

BEGIN(Engine)

enum PART_TYPE
{
	PT_FACE,
	PT_HEAD,
	PT_LOWER_BODY,
	PT_UPPER_BODY,
	PT_END
};

class ENGINE_DLL CCustom_Model final : public CComponent
{
private:
	CCustom_Model(_dev pDevice, _context pContext);
	CCustom_Model(const CCustom_Model& rhs);
	virtual ~CCustom_Model() = default;

public:
	const _uint& Get_NumMeshes(_uint eType) const;
	const _bool& IsAnimationFinished(_uint iAnimIndex) const;
	const _uint& Get_CurrentAnimationIndex() const;
	
	void Set_Animation(_uint iAnimIndex, const _bool& isLoop = false);
	void Select_Part(_uint eType, _uint iPartIndex);

public:
	HRESULT Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix);
	HRESULT Init(void* pArg) override;

public:
	void Play_Animation(_float fTimeDelta);
	HRESULT Bind_BoneMatrices(_uint eType, _uint iMeshIndex, class CShader* pShader, const _char* pVariableName);
	HRESULT Bind_Material(_uint eType, class CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType);

	HRESULT Render(_uint eType, _uint iMeshIndex);

private:
	_uint m_iNumParts[PT_END]{};
	vector<class CCustom_Part*> m_Parts[PT_END]{};
	_uint m_iSelectedPartIndex[PT_END]{};

	vector<class CBone*> m_Bones{};

	_uint m_iNumAnimations{};
	vector<class CAnimation*> m_Animations{};

	_float44 m_PivotMatrix{};


	_uint m_iCurrentAnimIndex{};
	_uint m_iPrevAnimIndex{};
	_bool m_isAnimChanged{};
	_bool m_isLoop{};

private:
	HRESULT Read_Bones(ifstream& File);
	HRESULT Read_Animations(ifstream& File);

public:
	static CCustom_Model* Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh = false, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END