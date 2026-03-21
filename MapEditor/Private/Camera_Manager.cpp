#include "Camera_Manager.h"

IMPLEMENT_SINGLETON(CCamera_Manager)

const CAMERA_STATE& CCamera_Manager::Get_CameraState() const
{
	return m_iCameraState;
}

const _float& CCamera_Manager::Get_ZoomFactor() const
{
	return m_fZoomFactor;
}

const _vec4& CCamera_Manager::Get_CameraTargetPos() const
{
	return m_vTarget;
}

const _vec4& CCamera_Manager::Get_CameraTargetLook()
{
	return m_vTargetLook;
}

const _bool& CCamera_Manager::Have_TargetLook() const
{
	return m_bTargetLook;
}

const _bool& CCamera_Manager::Get_ShakeCam() const
{
	return m_bShakeCamera;
}

const CAMERA_MODE& CCamera_Manager::Get_CameraModeIndex() const
{
	return m_eCamMode;
}

void CCamera_Manager::Set_ZoomFactor(const _float fFactor)
{
	m_fZoomFactor = fFactor;
}

void CCamera_Manager::Set_CameraState(const CAMERA_STATE& iIndex)
{
	m_iCameraState = iIndex;
}

void CCamera_Manager::Set_CameraTargetPos(const _vec4& vPos)
{
	m_vTarget = vPos;
}

void CCamera_Manager::Set_CameraTargetLook(const _vec4& vLook)
{
	m_vTargetLook = vLook;
}

void CCamera_Manager::Set_Have_TargetLook(const _bool& bHaveLook)
{
	m_bTargetLook = bHaveLook;
}

void CCamera_Manager::Set_AimMode(_bool Aim, _vec3 AimPos)
{
	m_AimMode = Aim;
	m_AimPos = AimPos;
}

void CCamera_Manager::Set_ShakeCam(const _bool& bShake, _float fShakePower)
{
	m_bShakeCamera = bShake;
	m_fShakePower = fShakePower;
}

void CCamera_Manager::Set_CameraModeIndex(const CAMERA_MODE& iIndex)
{
	m_eCamMode = iIndex;
}

void CCamera_Manager::Free()
{
}
