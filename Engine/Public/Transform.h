#pragma once
#include "Component.h"

BEGIN(Engine)

enum class State
{
	Right,
	Up,
	Look,
	Pos,
	End
};

class ENGINE_DLL CTransform final : public CComponent
{
public:
	using TRANSFORM_DESC = struct tagTransformDesc
	{
		_float fSpeedPerSec{};
		_float fRotationPerSec{};
	};

private:
	CTransform(_dev pDevice, _context pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_mat Get_World_Matrix() const;
	_mat Get_OldWorld_Matrix() const;
	_mat Get_World_Inverse() const;
	_vec4 Get_State(State eState) const;
	_vec3 Get_Scale() const;
	const _float& Get_Speed() const;
	const _bool& Is_Jumping() const;
	PxController* Get_Controller() const;
	_vec4 Get_CenterPos() const;
	const _bool Is_OnGround() const;

	void Set_OldMatrix();
	void Set_Position(_vec3 vPosition);
	void Set_FootPosition(_vec3 vPosition);
	void Set_FootPosition(PxVec3 vPosition);
	void Set_UpDirection(_vec4 vUp);
	void Set_State(State eState, _vec4 vState);
	void Set_Matrix(_mat Matrix);
	void Set_Scale(_vec3 fScale);
	void Set_Speed(_float fSpeed);
	void Set_RotationPerSec(_float fAngle);
	void Set_Controller(PxController* pController);
	void Delete_Controller();

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;

public:
	void Gravity(_float fTimeDelta, _float fGravity = -19.81f);
	void Reset_Gravity();

	void Go_Straight(_float fTimeDelta);
	const _bool Go_To(_vec4 vTargetPos, _float fTimeDelta, _float fOffset = 1.f);
	void Go_To_Dir(_vec4 vDir, _float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Up(_float fTimeDelta);
	void Go_Down(_float fTimeDelta);
	void Jump(_float fJumpForce = 10.f);

	void LookAt(_vec4 vTargetPos);
	void LookAway(_vec4 vTargetPos);
	void LookAt_Dir(_vec4 vDir);
	void Move_to(_vec4 vTargetPos, _float fTimeDelta, _float fMargin = 0.1f);
	void Turn(_vec4 vAxis, _float fTimeDelta);
	void Rotation(_vec4 vAxis, _float fAngle);
	void Set_Rotation(_vec4 vQuaternion);

public:
	HRESULT Bind_WorldMatrix(class CShader* pShader, const _char* pVariableName);
	HRESULT Bind_OldWorldMatrix(class CShader* pShader, const _char* pVariableName);

private:
	_mat m_WorldMatrix{};
	_mat m_OldWorldMatrix{};

	PxScene* m_pScene{ nullptr };
	PxController* m_pController{ nullptr };
	PxControllerCollisionFlags m_CollisionFlags{};

	_float m_fGravity{};

	_bool m_isJumping{};
	_float m_fJumpForce{};

	_bool m_isOnWall{};

	_float m_fSpeedPerSec{10.f};
	_float m_fRotationPerSec{XMConvertToRadians(90.f)};

public:
	static CTransform* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END