#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Riding.h"

BEGIN(Client)
class CVehicleBook final : public COrthographicObject
{
private:
	CVehicleBook(_dev pDevice, _context pContext);
	CVehicleBook(const CVehicleBook& rhs);
	virtual ~CVehicleBook() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

private:
	VEHICLE_TYPE								m_ePrevType{ VC_GROUND };
	VEHICLE_TYPE								m_eCurType{ VC_GROUND };

	_uint										m_iCurIndex{};
	_int										m_iSoundChannel = -1;

	_bool										m_isPicking{ false };
	_bool										m_isPrototype{ false };
	_bool										m_bNewVehicleIn{ false };
	_bool										m_isActive{ false };

	CGameObject*								m_pMoney{ nullptr };
	CGameObject*								m_pDiamond{ nullptr };
	CGameObject*								m_pNotify{ nullptr };
	CGameObject*								m_pTitleButton{ nullptr };
	CGameObject*								m_pExitButton{ nullptr };
	CGameObject*								m_pBackGround{ nullptr };

	CGameObject*								m_pUnderBar{ nullptr };
	CGameObject*								m_pSelectButton{ nullptr };
	CGameObject*								m_pVehicleType[VC_END];

	class CTextButtonColor*						m_pSlotBackGround{ nullptr };
	CTextButtonColor*							m_pEquipButton{ nullptr };

	class CVehicle*								m_pSelectedVehicle[VC_END];

	vector<CVehicle*>							m_vecVehicle[VC_END];

	LIGHT_DESC									m_Light_Desc{};

public:
	HRESULT Set_Vehicle(Riding_Type eType);

private:
	void Init_VehicleBookState(); 

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CVehicleBook* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END