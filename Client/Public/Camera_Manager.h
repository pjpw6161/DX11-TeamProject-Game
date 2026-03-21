#pragma once
#include "Client_Define.h"

BEGIN(Client)

class CCamera_Manager : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)
private:
	CCamera_Manager() = default;
	virtual ~CCamera_Manager() = default;

public:
	const _float& Get_CameraAttackZoom() { return m_fCameraAttackZoom; }
	const _bool& Get_RidingZoom() { return m_bRidingZoom; }
	const CAMERA_STATE& Get_CameraState()  const;
	const _float& Get_ZoomFactor() const;
	const _vec4& Get_CameraTargetPos() const;
	const _vec4& Get_CameraTargetLook();
	const _bool& Have_TargetLook() const;
	const _vec3& Get_AimPos() { return m_AimPos; }
	const _bool& Get_AimMode() { return m_AimMode; }
	const _bool& Get_FlyCam() { return m_bFlyCam; }
	const _bool& Get_LandingCam() { return m_bLandingCam; }
	const _float& Get_ShakePower() { return m_fShakePower; }
	// 카메라에서 쉐이킹 해야되는지 받는 함수.
	const _bool& Get_ShakeCam() const;
	// 현재 카메라가 메인 카메라인지 디버그 카메라인지 반환함. client define에 이넘 있음.
	const CAMERA_MODE& Get_CameraModeIndex() const;

	void Set_ZoomFactor(const _float fFactor);
	void Set_RidingZoom(_bool RidingZoom);
	void Set_CameraState(const CAMERA_STATE& iIndex);
	void Set_CameraTargetPos(const _vec4& vPos);
	void Set_CameraTargetLook(const _vec4& vLook);
	void Set_Have_TargetLook(const _bool& bHaveLook);
	void Set_AimMode(_bool Aim, _vec3 AimPos = _vec3(0.6f, 1.7f, 1.4f));
	void Set_FlyCam(_bool Landing) { m_bFlyCam = Landing; }
	void Set_LandingCam(_bool Fly) { m_bLandingCam = Fly; }
	void Set_CameraAttackZoom(_float fAttackZoom) { m_fCameraAttackZoom = fAttackZoom; }
	// 카메라 쉐이크 기능. true 던지면 카메라가 한번 흔들림.
	void Set_ShakeCam(const _bool& bShake, _float fShakePower = 0.1f);
	// 카메라 모드를 지정함. 카메라에서 말고는 쓰지 말것.
	void Set_CameraModeIndex(const CAMERA_MODE& iIndex);

private:
	CAMERA_MODE m_eCamMode{};
	CAMERA_STATE m_iCameraState{};
	_float m_fZoomFactor{ 3.f };
	_float m_fCameraAttackZoom{};
	_bool m_bTargetLook{ false };
	_bool m_bRidingZoom{};
	_vec4 m_vTarget{};
	_vec4 m_vTargetLook{};
	_bool m_AimMode{};
	_vec3 m_AimPos{};
	_bool m_bFlyCam{};
	_bool m_bLandingCam{};
	_bool m_bShakeCamera{};
	_float m_fShakePower{};

public:
	virtual void Free() override;
};

END