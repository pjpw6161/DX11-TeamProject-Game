#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CSelectDesc final : public COrthographicObject
{
public:
	enum CHARACTER { SORCERESS, WARRIOR, ROGUE, PRIEST, CHARACTER_END };
	typedef struct tagSelectDesc
	{
		CHARACTER eCharacter;
	}SELECT_DESC;
private:
	CSelectDesc(_dev pDevice, _context pContext);
	CSelectDesc(const CSelectDesc& rhs);
	virtual ~CSelectDesc() = default;

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
	_float				m_fAlpha{ 0.f };
	CHARACTER			m_eCharacter{ CHARACTER_END };
	wstring				m_strTexture;

public:
	static CSelectDesc* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END