#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"


BEGIN(Client)
class CBackGround_Mask final : public COrthographicObject
{
private:
	CBackGround_Mask(_dev pDevice, _context pContext);
	CBackGround_Mask(const CBackGround_Mask& rhs);
	virtual ~CBackGround_Mask() = default;

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
	wstring m_strTexture;
	LEVEL_ID m_eLevel;
	_float m_fSpeed{};
public:
	static CBackGround_Mask* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END