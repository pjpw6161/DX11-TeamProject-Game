#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Item.h"
#include "Inven.h"
#include "TextButtonColor.h"
#include "ItemSlot.h"

BEGIN(Client)
class CItem;
class CInven;
class CItemSlot;
class CInvenWindow;
class CTextButtonColor;
class CInvenFrame final : public COrthographicObject
{
public:
	enum FRAME_MODE { F_INVEN, F_SHOP, F_END};
	typedef struct tagInvenFrameDesc
	{
		CGameObject*			pParent;
		_float					fDepth;
		_float2					vSize;
		_float2					vPosition;
	}INVENFRAME_DESC;

private:
	CInvenFrame(_dev pDevice, _context pContext);
	CInvenFrame(const CInvenFrame& rhs);
	virtual ~CInvenFrame() = default;

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
	FRAME_MODE									m_eFrameMode{ F_INVEN };
	INVEN_TYPE									m_ePrevInvenType{ INVEN_ALL };
	INVEN_TYPE									m_eCurInvenType{ INVEN_ALL };

	_bool										m_isPrototype{ false };
	_bool										m_bNewItemIn{ false };
	_bool										m_isActive{ false };
	_bool										m_isQuickAnim{ false };
	_bool										m_isActiveQuickSlot{ false };
	_bool										m_isPicking { false };
	_bool										m_isPickingDouble{ false };

	_uint										m_iCurIndex{};
	ITEM_TYPE									m_iCurItemType{};

	_float										m_fButtonY{ 0.f };
	_float										m_fBGY{ 0.f };
	_float										m_fTime{ 0.f };
	_float										m_fDoubleClick{ 0.f };

	vector<CItem*>								m_vecItems;
	vector<CItem*>								m_vecItemsSlot[INVEN_TYPE::INVEN_END];
	vector<CItem*>								m_vecSellItems;

	CGameObject*								m_pParent{ nullptr };
	CGameObject*								m_pUnderBar{ nullptr };
	CGameObject*								m_pSelectButton{ nullptr };

	vector <CGameObject*>						m_pVerticalBar;
	CGameObject*								m_pInvenType[INVEN_TYPE::INVEN_END];

	CTextButtonColor*							m_pSlotSettingButton{ nullptr };
	CTextButtonColor*							m_pWearableClearButton{ nullptr };
	CTextButtonColor*							m_pBackGround{ nullptr };

	CTextButtonColor*							m_pExitSlotSetting{ nullptr };
	CTextButtonColor*							m_pResetSlot{ nullptr };
	CGameObject*								m_pResetSymbol{ nullptr };
	CItemSlot*									m_pSelectSlot[4];

	CTextButtonColor*							m_pSellButton{ nullptr };
	CInvenWindow*								m_pInvenWindow{ nullptr };


public:
	void Init_State();
	void Set_FrameMode(FRAME_MODE eMode) { m_eFrameMode = eMode; }
	void Set_Item(ITEM eItem, _uint iNum = 1);
	void Set_ItemPosition(INVEN_TYPE eInvenType);
	void ItemSlot_Logic(_uint iSlotIdx, _uint iIndex);
	void ItemSlot_Delete_Logic(_uint iSlotIdx);
	void Delete_Item(INVEN_TYPE eInvenType, _uint iIndex);
	void Delete_Item(INVEN_TYPE eInvenType, wstring& strName);

	void Set_Parent(CGameObject* pGameObject) { m_pParent = pGameObject; }
	const CItem* Find_Item(wstring& strName) const;
	void Init_SellItem() ;

private:
	void Picking_InvenButton(POINT ptMouse);
	void Picking_ShopButton(POINT ptMouse);

	void Inven_Tick(_float fTimeDelta, POINT ptMouse);
	void Shop_Tick(_float fTimeDelta, POINT ptMouse);

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CInvenFrame* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END