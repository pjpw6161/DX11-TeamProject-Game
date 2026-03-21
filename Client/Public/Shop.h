#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Wearable_Slot.h"


BEGIN(Client)
class CWearable_Slot;
class CShopDesc;
class CShop final : public COrthographicObject
{
private:
	enum STATE { EXPENDABLE, EQUIP, STATE_END };
private:
	CShop(_dev pDevice, _context pContext);
	CShop(const CShop& rhs);
	virtual ~CShop() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };

private:
	STATE										m_eCurShopState{ EXPENDABLE };
	STATE										m_ePrevShopType{ EXPENDABLE };

	_bool										m_bQuestTrigger{ false };
	_bool										m_isPrototype{ false };
	_bool										m_isActive{ false };
	_bool										m_isFrameExist{ false };

	CGameObject*								m_pMoney{ nullptr };
	CGameObject*								m_pDiamond{ nullptr };
	CGameObject*								m_pTitleButton{ nullptr };
	CGameObject*								m_pExitButton{ nullptr };
	CGameObject*								m_pBackGround{ nullptr };
	CGameObject*								m_pInvenFrame{ nullptr };

	CGameObject*								m_pUnderBar{ nullptr };
	CGameObject*								m_pSelectButton{ nullptr };
	CGameObject*								m_pShopMenu[STATE_END];
	
	vector<CShopDesc*>							m_vecShopItems[STATE_END];

	_bool m_isReadytoDeactivate{};
	_bool m_isReadytoActivate{};

public:
	_bool IsActive() { return m_isActive; }
	void Set_ItemPosition(STATE eState);
	void Open_Shop();

private:
	void Init_ShopState();

private:
	HRESULT Init_ShopItems();
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CShop* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END