#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Item.h"

BEGIN(Client)

class CItemSlot final : public COrthographicObject
{
public:
	enum ITSLOTMODE { ITSLOT_SCREEN, ITSLOT_INVEN, ITSLOT_END };

	typedef struct tagItemSlotDesc
	{
		ITSLOTMODE eSlotMode;
		_float fDepth;
		_float2 vSize;
		_float2 vPosition;
	}ITEMSLOT_DESC;

private:
	CItemSlot(_dev pDevice, _context pContext);
	CItemSlot(const CItemSlot& rhs);
	virtual ~CItemSlot() = default;

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
	ITSLOTMODE		m_eSlotMode{ ITSLOT_END };
	wstring			m_strTexture{};
	_uint			m_iMaxNum{50};
	_bool			m_isFull{ false };
	_float			m_fTime{};
	_float			m_fCoolTime{};
	RECT			m_rcRect{};
	CItem*			m_pItem{ nullptr };
	CGameObject*	m_pItemTex{ nullptr };

public:
	const _bool& Is_Full() const { return m_isFull; }
	//HRESULT Set_Item(class CItem::ITEM_DESC eItemDesc);
	HRESULT Set_Item(CItem* pItem, _int* iNum = nullptr);

	void Use_Item();
	void Delete_Item();
	void Set_FullSlot(CItem* pItem, _int* iNum = nullptr);
	const wstring Get_ItemName();
	const ITEM_USAGE Get_ItemUsage();
	const CItem* Get_ItemObject();
	const RECT& Get_Rect() const { return m_rcRect; }
	const _vec2 Get_Position() { return _vec2(m_fX, m_fY); }

private: 
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CItemSlot* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END