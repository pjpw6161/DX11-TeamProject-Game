#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CSymbol final : public COrthographicObject
{
public:
	enum TYPE { VILLAGE, FIELDEAST, FIELDSOUTH, GROAR, TYPE_END };
	typedef struct tagFadeDesc
	{
		TYPE eType;
	}SYMBOL_DESC;

private:
	CSymbol(_dev pDevice, _context pContext);
	CSymbol(const CSymbol& rhs);
	virtual ~CSymbol() = default;

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
	_bool m_bDissapear{ false };
	_float m_fAlpha{ 0.f };
	_float m_fTime{ 0.f };
	_float m_fDir{ 1.f };

	TYPE  m_eType{ TYPE_END };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSymbol* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END