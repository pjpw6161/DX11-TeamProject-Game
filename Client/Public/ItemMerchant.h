#pragma once

#include "Client_Define.h"
#include "NPC.h"


BEGIN(Client)
class CShop;
class C3DUITex;
class CItemMerchant final : public CNPC
{
public:
	enum ITEM_MERCHANT_ANIM
	{
		IDLE,
		TALK,
		ANIM_END
	};

private:
	CItemMerchant(_dev pDevice, _context pContext);
	CItemMerchant(const CItemMerchant& rhs);
	virtual ~CItemMerchant() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool		m_bTalking = { false };
	_bool		m_isColl = { false };
	_bool		m_isFadeReady{};

	CShop*		m_pShop{ nullptr };
	C3DUITex*	m_pSpeechBubble{ nullptr };

private:
	HRESULT Add_Parts();


public:
	static CItemMerchant* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END