#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CCharacterSelect final : public COrthographicObject
{
public:
	enum STATE { NONALPHA, ALPHA, STATE_END };
private:
	CCharacterSelect(_dev pDevice, _context pContext);
	CCharacterSelect(const CCharacterSelect& rhs);
	virtual ~CCharacterSelect() = default;

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
	_bool				m_isActive{ false };

	STATE				m_eState{ NONALPHA };


	_float				m_fAlpha{ 1.f };
	_float				m_fDuration{ 0.f };

public:
	void Set_Active_Alpha(STATE	eState);

public:
	static CCharacterSelect* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END