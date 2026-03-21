#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"


CTransform::CTransform(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

_vec4 CTransform::Get_State(State eState) const
{
	return XMLoadFloat4x4(&m_WorldMatrix).r[ToIndex(eState)];
}

_vec3 CTransform::Get_Scale() const
{
	return _vec3(XMVectorGetX(XMVector3Length(Get_State(State::Right)))
				 , XMVectorGetX(XMVector3Length(Get_State(State::Up)))
				 , XMVectorGetX(XMVector3Length(Get_State(State::Look))));
}

const _float& CTransform::Get_Speed() const
{
	return m_fSpeedPerSec;
}

const _bool& CTransform::Is_Jumping() const
{
	return m_isJumping;
}

PxController* CTransform::Get_Controller() const
{
	return m_pController;
}

_vec4 CTransform::Get_CenterPos() const
{
	if (not m_pController)
	{
		return Get_State(State::Pos);
	}

	return PxExVec3ToVector(m_pController->getPosition(), 1.f);
}

const _bool CTransform::Is_OnGround() const
{
	if (not m_pScene)
	{
		return false;
	}
	PxRaycastBuffer HitBuffer{};

	_bool HasCollided = m_pScene->raycast(PxExVec3ToPxVec3(m_pController->getFootPosition()), PxVec3(0.f, -1.f, 0.f), 2.f, HitBuffer);

	return HasCollided;
}

_mat CTransform::Get_World_Matrix() const
{
	return m_WorldMatrix;
}

_mat CTransform::Get_OldWorld_Matrix() const
{
	return m_OldWorldMatrix;
}

_mat CTransform::Get_World_Inverse() const
{
	return m_WorldMatrix.Invert();
}

void CTransform::Set_OldMatrix()
{
	m_OldWorldMatrix = m_WorldMatrix;
}

void CTransform::Set_Position(_vec3 vPosition)
{
	if (m_pController)
	{
		PxExtendedVec3 NewPos{ static_cast<_double>(vPosition.x), static_cast<_double>(vPosition.y), static_cast<_double>(vPosition.z) };
		m_pController->setPosition(NewPos);
		Set_State(State::Pos, PxExVec3ToVector(m_pController->getFootPosition(), 1.f));
	}
	else
	{
		Set_State(State::Pos, _vec4(vPosition, 1.f));
	}
}

void CTransform::Set_FootPosition(_vec3 vPosition)
{
	if (m_pController)
	{
		PxExtendedVec3 NewPos{ static_cast<_double>(vPosition.x), static_cast<_double>(vPosition.y), static_cast<_double>(vPosition.z) };
		m_pController->setFootPosition(NewPos);
		Set_State(State::Pos, PxExVec3ToVector(m_pController->getFootPosition(), 1.f));
	}
	else
	{
		return;
	}
}

void CTransform::Set_FootPosition(PxVec3 vPosition)
{
	if (m_pController)
	{
		m_pController->setFootPosition(PxVec3ToPxExVec3(vPosition));
		Set_State(State::Pos, PxExVec3ToVector(m_pController->getFootPosition(), 1.f));
	}
}

void CTransform::Set_UpDirection(_vec4 vUp)
{
	_vec3 vScale = Get_Scale();

	_vec4 vRight = XMVector3Cross(vUp, Get_State(State::Look));
	_vec4 vLook = XMVector3Cross(vRight, vUp);

	Set_State(State::Right, XMVector3Normalize(vRight) * vScale.x);
	Set_State(State::Up, XMVector3Normalize(vUp) * vScale.y);
	Set_State(State::Look, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::Set_State(State eState, _vec4 vState)
{
	switch (eState)
	{
	case Engine::State::Right:
		m_WorldMatrix.Right(vState);
		break;
	case Engine::State::Up:
		m_WorldMatrix.Up(vState);
		break;
	case Engine::State::Look:
		m_WorldMatrix.Look(vState);
		break;
	case Engine::State::Pos:
		m_WorldMatrix.Position(vState);
		break;
	}
}

void CTransform::Set_Matrix(_mat Matrix)
{

	m_WorldMatrix = Matrix;

	if (m_pController)
	{
		_vec3 vPosition = m_WorldMatrix.Position_vec3();
		PxExtendedVec3 NewPos{ static_cast<_double>(vPosition.x), static_cast<_double>(vPosition.y), static_cast<_double>(vPosition.z) };
		m_pController->setPosition(NewPos);
	}
}

void CTransform::Set_Scale(_vec3 fScale)
{
	_vec4 vRight = XMVector3Normalize(Get_State(State::Right));
	_vec4 vUp = XMVector3Normalize(Get_State(State::Up));
	_vec4 vLook = XMVector3Normalize(Get_State(State::Look));

	Set_State(State::Right, vRight * fScale.x);
	Set_State(State::Up, vUp * fScale.y);
	Set_State(State::Look, vLook * fScale.z);
}

void CTransform::Set_Speed(_float fSpeed)
{
	m_fSpeedPerSec = fSpeed;
}

void CTransform::Set_RotationPerSec(_float fAngle)
{
	m_fRotationPerSec = XMConvertToRadians(fAngle);
}

void CTransform::Set_Controller(PxController* pController)
{
	m_pController = pController;
	m_pScene = m_pController->getScene();
}

void CTransform::Delete_Controller()
{
	if (m_pController)
	{
		m_pController->release();
	}

	m_pController = nullptr;
}

HRESULT CTransform::Init_Prototype()
{
	m_WorldMatrix = _mat::Identity;

	return S_OK;
}

HRESULT CTransform::Init(void* pArg)
{
	if (pArg)
	{
		TRANSFORM_DESC Desc = *(TRANSFORM_DESC*)pArg;

		m_fSpeedPerSec = Desc.fSpeedPerSec;
		m_fRotationPerSec = Desc.fRotationPerSec;
	}

	return S_OK;
}

void CTransform::Gravity(_float fTimeDelta, _float fGravity)
{
	if (not m_pController)
	{
		return;
	}


	PxVec3 UpDir = { 0.f,1.f,0.f };

	//WallTest();

	if (m_fJumpForce > 0.f)
	{
		m_fJumpForce += fGravity * fTimeDelta;
		m_fGravity = m_fJumpForce;
		//m_fGravity = 25 * sin(40.f * XM_PI / 180.f) - 9.8f * fTimeDelta;
		//Gravity.y = m_fGravity;
	}
	else if (!m_CollisionFlags.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN))
	{
		m_fGravity += fGravity * fTimeDelta;
	}

	m_CollisionFlags = m_pController->move(UpDir * m_fGravity * fTimeDelta, 0.0001f, fTimeDelta, 0);
	//m_CollisionFlags = m_pController->move(PxVec3(0.f, m_fGravity, 0.f) * fTimeDelta, 0.0001f, fTimeDelta, 0);
	if (m_CollisionFlags.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN))
	{
		m_fJumpForce = 0.f;
		m_fGravity = 0.f;
		m_isJumping = false;
	}
	else
	{
		m_isJumping = true;
	}
	PxExtendedVec3 MovedPos = m_pController->getFootPosition();
	Set_State(State::Pos, PxExVec3ToVector(MovedPos, 1.f));
}

void CTransform::Reset_Gravity()
{
	m_fGravity = {};
	m_fJumpForce = {};
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	if (m_pController)
	{
		PxVec3 Disp = VectorToPxVec3(XMVector3Normalize(Get_State(State::Look)) * m_fSpeedPerSec * fTimeDelta);
		//PxVec3 Disp = VectorToPxVec3(XMVector3Normalize(XMVectorSetY(Get_State(State::Look), 0.f)) * m_fSpeedPerSec * fTimeDelta);
		m_pController->move(Disp, 0.0001f, fTimeDelta, 0);

		PxExtendedVec3 MovedPos = m_pController->getFootPosition();
		Set_State(State::Pos, PxExVec3ToVector(MovedPos, 1.f));
	}
	else
	{
		_vec4 vPos = Get_State(State::Pos);
		_vec4 vLook = Get_State(State::Look);

		vPos += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
		Set_State(State::Pos, vPos);
	}
}

const _bool CTransform::Go_To(_vec4 vTargetPos, _float fTimeDelta, _float fOffset)
{
	if (m_pController)
	{
		PxVec3 Disp = VectorToPxVec3(XMVector3Normalize(vTargetPos - Get_State(State::Pos)) * m_fSpeedPerSec * fTimeDelta);
		m_pController->move(Disp, 0.0001f, fTimeDelta, 0);

		_vec4 vMovedPos = PxExVec3ToVector(m_pController->getFootPosition(), 1.f);
		if (XMVectorGetX(XMVector3Length(vMovedPos - vTargetPos)) < fOffset)
		{
			return true;
		}
		Set_State(State::Pos, vMovedPos);
	}
	else
	{
		_vec4 vPos = Get_State(State::Pos);
		_vec4 vDisp = vTargetPos - vPos;

		vPos += XMVector3Normalize(vDisp) * m_fSpeedPerSec * fTimeDelta;

		if (XMVectorGetX(XMVector3Length(vPos - vTargetPos)) < fOffset)
		{
			return true;
		}
		Set_State(State::Pos, vPos);
	}

	return false;
}

void CTransform::Go_To_Dir(_vec4 vDir, _float fTimeDelta)
{
	if (m_pController)
	{
		PxVec3 Disp = VectorToPxVec3(XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta);
		m_pController->move(Disp, 0.0001f, fTimeDelta, 0);

		PxExtendedVec3 MovedPos = m_pController->getFootPosition();
		Set_State(State::Pos, PxExVec3ToVector(MovedPos, 1.f));
	}
	else
	{
		_vec4 vPos = Get_State(State::Pos);

		vPos += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;
		Set_State(State::Pos, vPos);
	}
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	if (m_pController)
	{
		PxVec3 Disp = -VectorToPxVec3(XMVector3Normalize(Get_State(State::Look)) * m_fSpeedPerSec * fTimeDelta);
		//PxVec3 Disp = VectorToPxVec3(XMVector3Normalize(XMVectorSetY(Get_State(State::Look), 0.f)) * m_fSpeedPerSec * fTimeDelta);
		m_pController->move(Disp, 0.0001f, fTimeDelta, 0);

		PxExtendedVec3 MovedPos = m_pController->getFootPosition();
		Set_State(State::Pos, PxExVec3ToVector(MovedPos, 1.f));
	}
	else
	{
		_vec4 vPos = Get_State(State::Pos);
		_vec4 vLook = Get_State(State::Look);

		vPos -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

		Set_State(State::Pos, vPos);
	}
}

void CTransform::Go_Left(_float fTimeDelta)
{
	if (m_pController)
	{
		PxVec3 Disp = -VectorToPxVec3(XMVector3Normalize(Get_State(State::Right)) * m_fSpeedPerSec * fTimeDelta);
		//PxVec3 Disp = VectorToPxVec3(XMVector3Normalize(XMVectorSetY(Get_State(State::Look), 0.f)) * m_fSpeedPerSec * fTimeDelta);
		m_pController->move(Disp, 0.0001f, fTimeDelta, 0);

		PxExtendedVec3 MovedPos = m_pController->getFootPosition();
		Set_State(State::Pos, PxExVec3ToVector(MovedPos, 1.f));
	}
	else
	{
		_vec4 vPos = Get_State(State::Pos);
		_vec4 vRight = Get_State(State::Right);

		vPos -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

		Set_State(State::Pos, vPos);
	}
}

void CTransform::Go_Right(_float fTimeDelta)
{
	if (m_pController)
	{
		PxVec3 Disp = VectorToPxVec3(XMVector3Normalize(Get_State(State::Right)) * m_fSpeedPerSec * fTimeDelta);
		//PxVec3 Disp = VectorToPxVec3(XMVector3Normalize(XMVectorSetY(Get_State(State::Look), 0.f)) * m_fSpeedPerSec * fTimeDelta);
		m_pController->move(Disp, 0.0001f, fTimeDelta, 0);

		PxExtendedVec3 MovedPos = m_pController->getFootPosition();
		Set_State(State::Pos, PxExVec3ToVector(MovedPos, 1.f));
	}
	else
	{
		_vec4 vPos = Get_State(State::Pos);
		_vec4 vRight = Get_State(State::Right);

		vPos += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

		Set_State(State::Pos, vPos);
	}
}

void CTransform::Go_Up(_float fTimeDelta)
{
	if (m_pController)
	{
		PxVec3 Disp = VectorToPxVec3(XMVector3Normalize(Get_State(State::Up)) * m_fSpeedPerSec * fTimeDelta);
		//PxVec3 Disp = VectorToPxVec3(XMVector3Normalize(XMVectorSetY(Get_State(State::Look), 0.f)) * m_fSpeedPerSec * fTimeDelta);
		m_pController->move(Disp, 0.0001f, fTimeDelta, 0);

		PxExtendedVec3 MovedPos = m_pController->getFootPosition();
		Set_State(State::Pos, PxExVec3ToVector(MovedPos, 1.f));
	}
	else
	{
		_vec4 vPos = Get_State(State::Pos);
		_vec4 vUp = Get_State(State::Up);

		vPos += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

		Set_State(State::Pos, vPos);
	}
}

void CTransform::Go_Down(_float fTimeDelta)
{
	if (m_pController)
	{
		PxVec3 Disp = -VectorToPxVec3(XMVector3Normalize(Get_State(State::Up)) * m_fSpeedPerSec * fTimeDelta);
		//PxVec3 Disp = VectorToPxVec3(XMVector3Normalize(XMVectorSetY(Get_State(State::Look), 0.f)) * m_fSpeedPerSec * fTimeDelta);
		m_pController->move(Disp, 0.0001f, fTimeDelta, 0);

		PxExtendedVec3 MovedPos = m_pController->getFootPosition();
		Set_State(State::Pos, PxExVec3ToVector(MovedPos, 1.f));
	}
	else
	{
		_vec4 vPos = Get_State(State::Pos);
		_vec4 vUp = Get_State(State::Up);

		vPos -= XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

		Set_State(State::Pos, vPos);
	}
}

void CTransform::Jump(_float fJumpForce)
{
	m_isJumping = true;
	m_fJumpForce = fJumpForce;
}

void CTransform::LookAt(_vec4 vTargetPos)
{
	_vec3 vScale = Get_Scale();

	_vec4 vLook = vTargetPos - Get_State(State::Pos);
	_vec4 vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vec4 vUp = XMVector3Cross(vLook, vRight);

	Set_State(State::Right, XMVector3Normalize(vRight) * vScale.x);
	Set_State(State::Up, XMVector3Normalize(vUp) * vScale.y);
	Set_State(State::Look, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAway(_vec4 vTargetPos)
{
	_vec3 vScale = Get_Scale();

	_vec4 vLook = Get_State(State::Pos) - vTargetPos;
	_vec4 vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vec4 vUp = XMVector3Cross(vLook, vRight);

	Set_State(State::Right, XMVector3Normalize(vRight) * vScale.x);
	Set_State(State::Up, XMVector3Normalize(vUp) * vScale.y);
	Set_State(State::Look, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_Dir(_vec4 vDir)
{
	_vec4 vTargetPos = Get_State(State::Pos) + vDir;
	LookAt(vTargetPos);
}

void CTransform::Move_to(_vec4 vTargetPos, _float fTimeDelta, _float fMargin)
{
	_vec4 vPos = Get_State(State::Pos);

	_vec4 vDir = vTargetPos - vPos;

	_float fDist = XMVectorGetX(XMVector3Length(vDir));

	if (fDist < fMargin)
	{
		return;
	}

	vPos += vDir * m_fSpeedPerSec * fTimeDelta;

	Set_State(State::Pos, vPos);
}

void CTransform::Turn(_vec4 vAxis, _float fTimeDelta)
{
	_mat Rotation = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	_vec4 vRight = XMVector3TransformNormal(Get_State(State::Right), Rotation);
	_vec4 vUp = XMVector3TransformNormal(Get_State(State::Up), Rotation);
	_vec4 vLook = XMVector3TransformNormal(Get_State(State::Look), Rotation);

	Set_State(State::Right, vRight);
	Set_State(State::Up, vUp);
	Set_State(State::Look, vLook);
}

void CTransform::Rotation(_vec4 vAxis, _float fAngle)
{
	_vec3 vScale = Get_Scale();

	_vec4 vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vec4 vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vec4 vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_mat Rotation = XMMatrixRotationAxis(vAxis, XMConvertToRadians(fAngle));

	vRight = XMVector3TransformNormal(vRight, Rotation);
	vUp = XMVector3TransformNormal(vUp, Rotation);
	vLook = XMVector3TransformNormal(vLook, Rotation);

	Set_State(State::Right, vRight);
	Set_State(State::Up, vUp);
	Set_State(State::Look, vLook);
}

void CTransform::Set_Rotation(_vec4 vQuaternion)
{
	_vec3 vScale = Get_Scale();

	_vec4 vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vec4 vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vec4 vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_mat Rotation = XMMatrixRotationQuaternion(vQuaternion);

	vRight = XMVector3TransformNormal(vRight, Rotation);
	vUp = XMVector3TransformNormal(vUp, Rotation);
	vLook = XMVector3TransformNormal(vLook, Rotation);

	Set_State(State::Right, vRight);
	Set_State(State::Up, vUp);
	Set_State(State::Look, vLook);
}

HRESULT CTransform::Bind_WorldMatrix(CShader* pShader, const _char* pVariableName)
{
	return pShader->Bind_Matrix(pVariableName, m_WorldMatrix);
}

HRESULT CTransform::Bind_OldWorldMatrix(CShader* pShader, const _char* pVariableName)
{
	return pShader->Bind_Matrix(pVariableName, m_OldWorldMatrix);
}

CTransform* CTransform::Create(_dev pDevice, _context pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

	if (m_pController)
	{
		m_pController->release();
	}
}
