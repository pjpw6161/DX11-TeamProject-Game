#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_BoneName() const;
	const _mat* Get_CombinedMatrix() const;
	const _mat& Get_Transformation() const;
	const _mat& Get_OffsetMatrix() const;

	void Set_OffsetMatrix(_mat OffsetMatrix);
	void Set_Transformation(_mat TransformationMatrix);

private:
	HRESULT Init(ifstream& ModelFile);

private:
	_char m_szName[MAX_PATH]{};
	_mat m_TransformationMatrix{};
	_mat m_CombindTransformationMatrix{};
	_mat m_OffsetMatrix{};

	_int m_iParentIndex{};

public:
	void Update_CombinedMatrix(const vector<CBone*>& Bones);

public:
	static CBone* Create(ifstream& ModelFile);
	CBone* Clone();
	virtual void Free() override;
};

END