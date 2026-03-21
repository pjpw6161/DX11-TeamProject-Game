#pragma once
#include "Base.h"

BEGIN(Engine)

enum class TransformType
{
	View,
	Proj,
	End
};

class CPipeLine final : public CBase
{
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	_vec4 Get_CameraPos() const;
	_vec4 Get_CameraLook() const;
	_mat Get_Transform_vec4x4(TransformType eState) const;
	_mat Get_Transform_Inversed_vec4x4(TransformType eState) const;
	_mat Get_Transform(TransformType eState) const;
	_mat Get_Transform_Inversed(TransformType eState) const;

	void Set_Transform(TransformType eState, const _mat& TransformMatrix);
	void Set_Transform(TransformType eState, _fmatrix TransformMatrix);

	// ¿ø¸í
	void Set_OldViewMatrix(const _mat& TransformMatrix);
	_mat Get_OldViewMatrix_vec4x4() const;
	_mat Get_OldViewMatrix() const;

	_mat ChagneViewForReflection(_float fWaterHeight);

public:
	HRESULT Init();
	void Tick();

private:
	_vec4 m_vCameraPos{};
	_vec4 m_vCameraLook{};
	_mat m_TransformMatrix[ToIndex(TransformType::End)]{};
	_mat m_TransformMatrix_Inversed[ToIndex(TransformType::End)]{};

	_mat m_OldViewMatrix{};

public:
	static CPipeLine* Create();
	virtual void Free() override;
};

END