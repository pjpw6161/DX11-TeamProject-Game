#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"


BEGIN(Client)
class CTargeted final : public COrthographicObject
{
private:
	CTargeted(_dev pDevice, _context pContext);
	CTargeted(const CTargeted& rhs);
	virtual ~CTargeted() = default;

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
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	_vec4				m_vColor{ _vec4(1.f, 1.f, 1.f, 1.f) };




public:
	static CTargeted* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END