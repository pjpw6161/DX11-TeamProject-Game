#pragma once
#include "Client_Define.h"
#include "Camera.h"
#include "Trigger_Manager.h"
#include "CutScene_Curve.h"

BEGIN(Client)

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
	void Set_Delete_Curve();
	void Set_Speed(_float fSpeed) { m_fCutSceneSpeed = fSpeed;} 
	_float Get_Speed() { return m_fCutSceneSpeed;} 
	
	HRESULT Add_Eye_Curve(_mat matPoints, _float fCurveSpeed);
	HRESULT Add_At_Curve(_mat matPoints);
	HRESULT Delete_Curve();

	void Play_Camera(_float fTimeDelta);

	HRESULT CutScene_Registration(const wstring& strDataPath);

private:
	_bool m_isMoveMode{ true };
	_float m_fSpeed{ 10.f };
	_float m_fShakeAcc{ 100.f };

private:
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	CTrigger_Manager* m_pTrigger_Manager{ nullptr };
	_float m_fSceneTimer{};
	_uint m_iSceneIndex{};
	CUTSCENE* m_pScene{ nullptr };

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

	_float                  m_fShakePower = { 0.f };
	_float                  m_fShakeDuration = { 0.f };
	_float                  m_fShakeTimeAcc = { 0.f };

private:
	class CCamera_Manager* m_pCam_Manager{ nullptr };

public:
	static CCamera_CutScene* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END