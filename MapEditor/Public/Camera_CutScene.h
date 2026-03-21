#pragma once
#include "MapEditor_Define.h"
#include "Camera.h"
#include "ImGui_Manager.h"
#include "CutScene_Curve.h"

BEGIN(MapEditor)

class CCamera_CutScene final : public CCamera
{
private:
	CCamera_CutScene(_dev pDevice, _context pContext);
	CCamera_CutScene(const CCamera_CutScene& rhs);
	virtual ~CCamera_CutScene() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render();
	void Set_Delete_Curve();
	void Set_Speed(_float fSpeed) { m_fCutSceneSpeed = fSpeed;} 
	_float Get_Speed() { return m_fCutSceneSpeed;} 
	string Get_Name();
	
	HRESULT Add_Eye_Curve(_vec4 vFirstPoint, _vec4 vSecondPoint, _float fSectionSpeed = 5.f);
	HRESULT Add_Eye_Curve(_mat matPoints, _float fCurveSpeed);
	HRESULT Add_At_Curve(_vec4 vFirstPoint, _vec4 vSecondPoint, _float fSectionSpeed = 5.f);
	HRESULT Add_At_Curve(_mat matPoints);
	HRESULT Delete_Curve();

	_int Get_Frame() { return m_iFrame; }
	vector<class CCutScene_Curve*>& Get_EyeCurve() { return m_CameraEyeList; }
	vector<class CCutScene_Curve*>& Get_AtCurve() { return m_CameraAtList; }

	void Play_Camera(_float fTimeDelta);

	void Start_Play();
	void Stop_Play();
	void Reset_Play();

private:
	_bool m_isMoveMode{ true };
	_float m_fMouseSensor{ 0.08f };
	_float m_fSpeed{ 10.f };
	_float m_fPlayerDistance{ 4.f };
	_float m_fShakeAcc{ 100.f };

private:
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	class CCamera_Manager* m_pCam_Manager{ nullptr };

private:
	_float m_fSceneTimer{};
	_uint m_iSceneIndex{};
	CUTSCENE* m_pScene{ nullptr };
	CImGui_Manager* m_pImGui_Manager{nullptr};

	CCutScene_Curve* m_pEyeCurve{nullptr};
	CCutScene_Curve* m_pAtCurve{nullptr};

	vector<class CCutScene_Curve*> m_CameraAtList;
	vector<class CCutScene_Curve*> m_CameraEyeList;

	_float					m_fCutSceneSpeed = { 1.f };
	_uint					m_iCurrentSectionIndex = { 0 };
	_uint					m_iNextSectionIndex = { 0 };
	_uint					m_iFrame = { 0 };

	_uint					m_iTotalFrame = { 0 };
	_uint					m_iSectionCount = { 0 };
	_uint					m_iLastFrame = { 0 };

	_float					m_fTimeDeltaAcc = { 0.f };
	_float					m_fTotalTimeDeltaAcc = { 0.f };

	_bool					m_isShaking = { false };
	_bool                   m_isShakeBasic = { false };
	_bool                   m_isShakeEye = { false };
	_bool                   m_isShakeAt = { false };
	_bool                   m_isShakePower = { false };
	_bool                   m_isStop = { false };

	_float                  m_fShakePower = { 0.f };
	_float                  m_fShakeDuration = { 0.f };
	_float                  m_fShakeTimeAcc = { 0.f };

	_bool					m_isPreCameraLerp = { false };
	_float3					m_vPreEye = { 0.f, 0.f, 0.f };
	_float3					m_vPreAt = { 0.f, 0.f, 0.f };
	_float					m_fPreCameraLerpTimeAcc = { 0.f };
	_float					m_fPreCameraLerpTime = { 0.f };
	CameraInfo				CamInfo{};
	wstring					m_strName{};
	wstring					strSectionName{};

private:
	HRESULT		Add_Components();
	HRESULT		Bind_ShaderResources();

public:
	static CCamera_CutScene* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END