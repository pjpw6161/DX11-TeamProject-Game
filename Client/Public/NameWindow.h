#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CNameWindow final : public COrthographicObject
{
private:
	CNameWindow(_dev pDevice, _context pContext);
	CNameWindow(const CNameWindow& rhs);
	virtual ~CNameWindow() = default;

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
	_uint					m_iNum{1};
	_bool					m_bStartInput{ false };

	class CTextButton*		m_pExitButton{ nullptr };
	CTextButton*			m_pSelectButton{ nullptr };


private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CNameWindow* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END