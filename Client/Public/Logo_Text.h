#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CLogo_Text final : public COrthographicObject
{
private:
	CLogo_Text(_dev pDevice, _context pContext);
	CLogo_Text(const CLogo_Text& rhs);
	virtual ~CLogo_Text() = default;

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
	_float m_fAlpha{ 0.f };
	_float m_fDir{ 1.f };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLogo_Text* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END