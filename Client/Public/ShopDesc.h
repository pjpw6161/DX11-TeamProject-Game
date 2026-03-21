#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)
class CItem;
class CShopDesc final : public COrthographicObject
{
public:
	typedef struct tagItemSlotDesc
	{
		_float					fDepth;
		_float2					vPosition;
		wstring					strItemName;
	}SHOPITEM_DESC;

private:
	CShopDesc(_dev pDevice, _context pContext);
	CShopDesc(const CShopDesc& rhs);
	virtual ~CShopDesc() = default;

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
	wstring					m_strItemName{};
	ITEM					m_eItemDesc{};
	_uint					m_iNum{1};

	RECT					m_rcRect{};

	CItem*					m_pItem{ nullptr };

public:
	const ITEM& Get_ItemDesc() const;
	void Set_ItemNum(_int iNum) { m_iNum += iNum; }
	void Set_Position(_vec2 vPos);
	const RECT& Get_Rect() const { return m_rcRect; }
	const _uint& Get_ItemNum() const { return m_iNum; }


private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CShopDesc* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END