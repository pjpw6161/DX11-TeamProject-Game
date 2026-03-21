#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CHPBoss final : public COrthographicObject
{
public:
	typedef struct tagNameTagDesc
	{
		wstring strName;
		_uint iMaxHp;
		LEVEL_ID eLevelID;
	}HPBOSS_DESC;
private:
	CHPBoss(_dev pDevice, _context pContext);
	CHPBoss(const CHPBoss& rhs);
	virtual ~CHPBoss() = default;

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
	CTexture* m_pBGTextureCom{ nullptr };
	CTexture* m_pBorderTextureCom{ nullptr };

private:
	LEVEL_ID		m_eLevel{};

	_bool			m_bEffect{false};
	_int			m_iCurHp{};
	_uint			m_iMaxHp{};
	_int			m_iTargetHp{};

	_float			m_fTime{};
	wstring			m_strName;

	CGameObject*	m_pIcon{ nullptr };
public:
	void Set_HP(const _uint iHp) { m_iTargetHp = iHp; }

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CHPBoss* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END