#include "PipeLine.h"

CPipeLine::CPipeLine()
{
}

_vec4 CPipeLine::Get_CameraPos() const
{
	return m_vCameraPos;
}

_vec4 CPipeLine::Get_CameraLook() const
{
	return m_vCameraLook;
}

_mat CPipeLine::Get_Transform_vec4x4(TransformType eState) const
{
	return m_TransformMatrix[ToIndex(eState)];
}

_mat CPipeLine::Get_Transform_Inversed_vec4x4(TransformType eState) const
{
	return m_TransformMatrix_Inversed[ToIndex(eState)];
}

_mat CPipeLine::Get_Transform(TransformType eState) const
{
	return XMLoadFloat4x4(&m_TransformMatrix[ToIndex(eState)]);
}

_mat CPipeLine::Get_Transform_Inversed(TransformType eState) const
{
	return XMLoadFloat4x4(&m_TransformMatrix_Inversed[ToIndex(eState)]);
}

void CPipeLine::Set_Transform(TransformType eState, const _mat& TransformMatrix)
{
	m_TransformMatrix[ToIndex(eState)] = TransformMatrix;
}

void CPipeLine::Set_Transform(TransformType eState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrix[ToIndex(eState)],TransformMatrix);
}

void CPipeLine::Set_OldViewMatrix(const _mat& TransformMatrix)
{
	XMStoreFloat4x4(&m_OldViewMatrix, TransformMatrix);
}

_mat CPipeLine::Get_OldViewMatrix_vec4x4() const
{
	return m_OldViewMatrix;
}

_mat CPipeLine::Get_OldViewMatrix() const
{
	return XMLoadFloat4x4(&m_OldViewMatrix);
}

_mat CPipeLine::ChagneViewForReflection(_float fWaterHeight)
{
	_mat CamWorldMatrix = m_TransformMatrix_Inversed[ToIndex(TransformType::View)];
	
	_vec3 vCamPos = CamWorldMatrix.Position_vec3();
	_vec3 vCamLook = m_vCameraLook;
	_vec3 vCamRight = CamWorldMatrix.Right_vec3();

	_vec3 vTargetPos = vCamPos + vCamLook;

	_float fReflectionCamY = -vCamPos.y + 2.f * fWaterHeight;
	_vec3 vReflectionCamPos(vCamPos.x, fReflectionCamY, vCamPos.z);

	_float fReflectionTargetY = -vTargetPos.y + 2.f * fWaterHeight;
	_vec3 vReflectionTarget(vTargetPos.x, fReflectionTargetY, vTargetPos.z);

	_vec3 vReflectionLook = vReflectionTarget - vReflectionCamPos;

	_vec3 vReflectionCamUp = vCamRight.Cross(vReflectionLook);

	return XMMatrixLookAtLH(vReflectionCamPos, vReflectionTarget, vReflectionCamUp);
}

HRESULT CPipeLine::Init()
{
	for (size_t i = 0; i < ToIndex(TransformType::End); i++)
	{
		XMStoreFloat4x4(&m_TransformMatrix[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformMatrix_Inversed[i], XMMatrixIdentity());
	}

	XMStoreFloat4x4(&m_OldViewMatrix, XMMatrixIdentity());

	return S_OK;
}

void CPipeLine::Tick()
{
	for (size_t i = 0; i < ToIndex(TransformType::End); i++)
	{
		m_TransformMatrix_Inversed[i] = m_TransformMatrix[i].Invert();
	}
	m_vCameraPos = m_TransformMatrix_Inversed[ToIndex(TransformType::View)].Position(), 1.f;
	m_vCameraLook = m_TransformMatrix_Inversed[ToIndex(TransformType::View)].Look();
}

CPipeLine* CPipeLine::Create()
{
	CPipeLine* pInstance = new CPipeLine();
	
	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CPipeLine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPipeLine::Free()
{
}
