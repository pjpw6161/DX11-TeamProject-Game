#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Wearable_Slot.h"


BEGIN(Client)
class CWearable_Slot;
class CInven final : public COrthographicObject
{

private:
	CInven(_dev pDevice, _context pContext);
	CInven(const CInven& rhs);
	virtual ~CInven() = default;

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
	_bool										m_isPrototype{ false };
	_bool										m_bNewItemIn{ false };
	_bool										m_isActive{ false };
	_bool										m_isReset{ false };

	CGameObject*								m_pMoney{ nullptr };
	CGameObject*								m_pDiamond{ nullptr };
	CGameObject*								m_pNotify{ nullptr };
	CGameObject*								m_pTitleButton{ nullptr };
	CGameObject*								m_pExitButton{ nullptr };
	CGameObject*								m_pBackGround{ nullptr };
	CGameObject*								m_pSeigeLine{ nullptr };
	CGameObject*								m_pInvenFrame{ nullptr };

	CWearable_Slot*								m_pWearableSlots[WEARABLE_TYPE::W_END]{};
	
	LIGHT_DESC									m_Light_Desc{};
private:
	void Init_InvenState(); 

public:
	HRESULT Set_Item(ITEM eItem, _uint iNum = 1);
	HRESULT Set_WearableItem(WEARABLE_TYPE eType,ITEM eItemDesc);
	void Reset_WearableSlot();

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CInven* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END