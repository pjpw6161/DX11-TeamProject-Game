#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)
class CTextButtonColor;
class CTextButton;
class CItem final : public COrthographicObject
{
public:
	typedef struct tagItemSlotDesc
	{
		_bool					isScreen;
		_bool					bCanInteract;
		_bool					haveBG{ false };
		_uint					iNum{ 1 };
		_float					fDepth;
		_float2					vSize;
		_float2					vPosition;
		ITEM					eItemDesc;
	}ITEM_DESC;

private:
	CItem(_dev pDevice, _context pContext);
	CItem(const CItem& rhs);
	virtual ~CItem() = default;

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
	_bool					m_bBorder{ false };
	_bool					m_bCanInteract{ false };
	_bool					m_isScreen{ false };
	_bool					m_haveBG{ false };
	ITEM					m_eItemDesc{};
	_uint					m_iNum{1};
	RECT					m_rcRect{};
	
	CTextButton*			m_pBorder{ nullptr };
	CTextButtonColor*		m_pBackGround{ nullptr };

public:
	const ITEM& Get_ItemDesc() const { return m_eItemDesc; }
	void Set_ItemNum(_int iNum) { m_iNum += iNum; }
	void Set_Position(_vec2 vPos);
	const RECT& Get_Rect() const { return m_rcRect; }
	const _uint& Get_ItemNum() const { return m_iNum; }
	void Set_Border(_bool isBorder) { m_bBorder = isBorder; }
	_bool Get_Border() { return m_bBorder; }

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CItem* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END