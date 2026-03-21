#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CExpBar final : public COrthographicObject
{
private:
	CExpBar(_dev pDevice, _context pContext);
	CExpBar(const CExpBar& rhs);
	virtual ~CExpBar() = default;

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
	CTexture* m_pMaskTextureCom{ nullptr };

private:
	_uint			m_iLevel{ 1 };
	_float			m_fTime{};
	_float2			m_vRatio{};
	CGameObject* m_pBackground{ nullptr };


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CExpBar* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END