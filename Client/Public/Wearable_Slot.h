#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Item.h"

BEGIN(Client)
class CItem;
class CWearable_Slot final : public COrthographicObject
{
public:

	typedef struct tagWearableSlotDesc
	{
		WEARABLE_TYPE eType;
		_float fDepth;
		_float2 vSize;
		_float2 vPosition;
	}WEARABLESLOT;
private:
	CWearable_Slot(_dev pDevice, _context pContext);
	CWearable_Slot(const CWearable_Slot& rhs);
	virtual ~CWearable_Slot() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer*			m_pRendererCom{ nullptr };
	CShader*			m_pShaderCom{ nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom{ nullptr };
	CTexture*			m_pTextureCom{ nullptr };

private:
	PART_TYPE				m_ePartType{};
	RECT					m_rcRect;
	_bool					m_isFull = { false };

	WEARABLE_TYPE			m_eType = { W_END };
	CItem*					m_pItem = { nullptr };
	CGameObject*			m_pSlotType = { nullptr };
	CGameObject*			m_pSelected = { nullptr };

public:
	const RECT& Get_Rect() const { return m_rcRect; }
	HRESULT Set_WearableItem(ITEM eItemDesc);
	_bool Is_Full() { return m_isFull; }
	const ITEM& Get_ItemDesc() const;

	void Delete_Item();

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Add_Slots();

public:
	static CWearable_Slot* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END