#include "Client_Define.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Main final : public CCamera
{
private:
	CCamera_Main(_dev pDevice, _context pContext);
	CCamera_Main(const CCamera_Main& rhs);
	virtual ~CCamera_Main() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

	_bool Get_CheckCollision() { return m_isCollision; }
public:
	void Camera_Zoom(_float fTimeDelta);

private: // CAM_MODE
	void Default_Mode(_float fTimeDelta);
	void Shop_Mode(_float fTimeDelta);
	void ZOOM_Mode(_float fTimeDelta);
	void Custom_Mode(_float fTimeDelta);
	_bool Inven_Mode(_float fTimeDelta);
	void Select_Mode(_float fTimeDelta);
	void WorldMap_Mode(_float fTimeDelta);
	void SkillBook_Mode(_float fTimeDelta);
	void BrickGame_Mode(_float fTimeDelta);

private:
	class CCamera_Manager* m_pCam_Manager{ nullptr };

private:
	_bool m_isCollision{false};
	_bool m_isWorldMap{};
	_bool m_isReady{};
	_int m_iRotation{ 1 };
	_float m_fLerpTime{};
	_vec4 m_vAimCamPos{};
	_vec4 m_vOriCamPos{};
	_float m_CurrentTime{};
	_float m_fSpeed{ 10.f };
	_float m_AimZoomInTime{};
	_float m_SwaySpeed = 3.0f;
	_float m_AimZoomOutTime{};
	_float m_SwayAmount = 3.0f;
	_float m_TimeOffset = 0.5f;
	_float m_fSelectRotation{};
	_bool m_isMoveMode{ true };
	_float m_fLerpDistance{ 4.f };
	_float m_fShakeAcc{ 100.f };
	_float m_fFirstDistance{ 4.f };
	_float m_fMouseSensor{ 0.08f };
	_float m_fPlayerDistance{ 4.f };
	_bool  m_bInitMode[CAMERA_MODE::CM_END]{};
	CAMERA_STATE m_eCurrState{};
	CAMERA_STATE m_ePrevState{};
	CTransform* m_pPlayerTransform{ nullptr };

	_bool m_isFadeReady{};
	_vec4 m_vOriginalLook{};
	LIGHT_DESC m_Original_Light_Desc{};
private:
	_bool m_bSelect{};
	_bool m_bZoomEnd{};
	 _uint m_iCamChange{};
	_uint m_iSceneIndex{};
	_float m_fSceneTimer{};
	 _float m_fCamChangeTime{};
	 _float m_fMap_UpDistance{};
	 _float m_fLerp_UpDistance{};
	CUTSCENE* m_pScene{ nullptr };
	 _float m_fMap_LookDistance{};
	 _float m_fMap_RightDistance{};
	 _float m_fLerp_LookDistance{};
	 _float m_fLerp_RightDistance{};
	 _float m_fCurrentMapZoom{ 1017.16f };
	 _vec4 m_vMapPos{ 990.944f, 1017.16f, 1003.75f, 1.f	};
	 //_matrix matProj{};
	 //_matrix matView{};

private:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);


	HRESULT Add_Collider();

	CCollider* m_pColliderCom{ nullptr };
	CRenderer* m_pRendererCom{ nullptr };

public:
	static CCamera_Main* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END