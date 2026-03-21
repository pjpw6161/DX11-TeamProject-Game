#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"


BEGIN(Client)
class CHitEffect final : public COrthographicObject
{
public:
	typedef struct tagNameTagDesc
	{
		_bool isCritical{};
		_bool isPlayer{ false };
		_uint	iDamage;
		_vec2	vTextPosition;
		class CTransform* pParentTransform;
	}HITEFFECT_DESC;
private:
	CHitEffect(_dev pDevice, _context pContext);
	CHitEffect(const CHitEffect& rhs);
	virtual ~CHitEffect() = default;

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
	_bool				m_isCritical{ false };
	_bool				m_isPlayer{ false };
	_bool				m_isEffect{ true };
	LEVEL_ID			m_eLevel{};

	_uint				m_iDamage{};
	_uint				m_iNumIdx{};
	_uint				m_iRenderNum{};
	_float				m_fFontSize{ 0.f };

	_float				m_fTime{};
	_float				m_fAlpha{};
	_vec2				m_vTextPosition;
	_vec4				m_vColor{ _vec4(1.f, 1.f, 1.f, 1.f) };

	CTransform*			m_pParentTransform{ nullptr };
	class C3DUITex*		m_pEffect{ nullptr };
	


public:
	static CHitEffect* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END