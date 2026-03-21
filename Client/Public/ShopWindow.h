#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)
class CShopDesc;
class CShopWindow final : public COrthographicObject
{
public:
	typedef struct tagItemSlotDesc
	{
		wstring strShopItem;
		_uint iCurItemNum;
	}SHOPWINDOW_DESC;

private:
	CShopWindow(_dev pDevice, _context pContext);
	CShopWindow(const CShopWindow& rhs);
	virtual ~CShopWindow() = default;

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
	_bool					m_isPickingButton{ false };
	_bool					m_isPickingCircle{ false };

	CShopDesc*				m_pShopDesc{nullptr};
	_uint					m_iItemNum{};
	_uint					m_iCurItemNum{};
	_uint					m_iTotalCost{};
	_uint					m_iAvailable{};
	RECT					m_rcRect{};
	wstring					m_strShopItem{};

	class CTextButton*		m_pExitButton{ nullptr };
	CTextButton*			m_pSelectButton{ nullptr };

	CTextButton*			m_pPlusBtn{ nullptr };
	CTextButton*			m_pMinusBtn{ nullptr };
	CTextButton*			m_pCoinTex{ nullptr };
	CTextButton*			m_pNumButtom{ nullptr };
	CTextButton*			m_pPickingCircle{ nullptr };
	class CTextButtonColor*	m_pNumBar{ nullptr };

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CShopWindow* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END