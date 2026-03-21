#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CTutorial final : public COrthographicObject
{
public:
	typedef struct tagTutoDesc
	{
		TUTO_SEQ eTuto;
	}TUTO_DESC;
private:
	CTutorial(_dev pDevice, _context pContext);
	CTutorial(const CTutorial& rhs);
	virtual ~CTutorial() = default;

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
	_float m_fTime{ 0.4f };
	_float m_fDir{ 1.f };

	TUTO_SEQ m_eTuto{ TUTO_END };
	class CTextButtonColor* m_pPoint{ nullptr };

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTutorial* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END