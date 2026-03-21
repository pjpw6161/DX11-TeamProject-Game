#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CLogo_Stone final : public COrthographicObject
{
private:
	CLogo_Stone(_dev pDevice, _context pContext);
	CLogo_Stone(const CLogo_Stone& rhs);
	virtual ~CLogo_Stone() = default;

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
	_float m_fU{ 0.f };
	_float m_fV{ 0.f };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLogo_Stone* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END