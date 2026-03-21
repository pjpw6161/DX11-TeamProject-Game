#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CInvenWindow final : public COrthographicObject
{
private:
	CInvenWindow(_dev pDevice, _context pContext);
	CInvenWindow(const CInvenWindow& rhs);
	virtual ~CInvenWindow() = default;

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
	_bool					m_bSell{ false };
	_bool					m_isActive{ false };

	_uint					m_iCost{};
	class CTextButton*		m_pMoney{ nullptr };
	CTextButton*			m_pExitButton{ nullptr };
	CTextButton*			m_pSelectButton{ nullptr };

public:
	void Set_Active(_bool isActive) { m_isActive = isActive; }
	_bool Is_Active() { return m_isActive; }
	_bool Picked_SellBtn() { return m_bSell; }
	void Set_SellBtn(_bool bSellBtn) { m_bSell = bSellBtn; }
	void Set_Cost(_uint iCost) { m_iCost = iCost; }

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CInvenWindow* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END