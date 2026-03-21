#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CItemInfo final : public COrthographicObject
{
public:
	typedef struct tagItemSlotDesc
	{
		_float					fDepth;
		ITEM					eItemDesc;
	}ITEMINFO_DESC;

private:
	CItemInfo(_dev pDevice, _context pContext);
	CItemInfo(const CItemInfo& rhs);
	virtual ~CItemInfo() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };

private:
	ITEM					m_eItemDesc{};
	_uint					m_iNum{1};
	RECT					m_rcRect{};

	CGameObject*			m_pItemTex{ nullptr };
	class CNineSlice*		m_pBackGround{ nullptr };
	class CTextButton*		m_pExitButton{ nullptr };
	CTextButton*			m_SelectButton{ nullptr };


private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CItemInfo* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END