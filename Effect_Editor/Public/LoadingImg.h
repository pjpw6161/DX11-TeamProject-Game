#pragma once
#include "Effect_Define.h"
#include "OrthographicObject.h"

BEGIN(Effect)

class CLoadingImg final : public COrthographicObject
{
private:
	CLoadingImg(_dev pDevice, _context pContext);
	CLoadingImg(const CLoadingImg& rhs);
	virtual ~CLoadingImg() = default;

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
	_bool* m_pLoadingComplete{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLoadingImg* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END